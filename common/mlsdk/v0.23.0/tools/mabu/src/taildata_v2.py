#!/usr/bin/python
# %BANNER_BEGIN%
# ---------------------------------------------------------------------
# %COPYRIGHT_BEGIN%
#
# Copyright (c) 2018 Magic Leap, Inc. (COMPANY) All Rights Reserved.
# Magic Leap, Inc. Confidential and Proprietary
#
#  NOTICE:  All information contained herein is, and remains the property
#  of COMPANY. The intellectual and technical concepts contained herein
#  are proprietary to COMPANY and may be covered by U.S. and Foreign
#  Patents, patents in process, and are protected by trade secret or
#  copyright law.  Dissemination of this information or reproduction of
#  this material is strictly forbidden unless prior written permission is
#  obtained from COMPANY.  Access to the source code contained herein is
#  hereby forbidden to anyone except current COMPANY employees, managers
#  or contractors who have executed Confidentiality and Non-disclosure
#  agreements explicitly covering such access.
#
#  The copyright notice above does not evidence any actual or intended
#  publication or disclosure  of  this source code, which includes
#  information that is confidential and/or proprietary, and is a trade
#  secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION,
#  PUBLIC  PERFORMANCE, OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS
#  SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF COMPANY IS
#  STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND
#  INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE
#  CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS
#  TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE,
#  USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.
#
# %COPYRIGHT_END%
# ---------------------------------------------------------------------
# %BANNER_END%

###############################################################################
# Imports
###############################################################################

import os
import sys
import argparse
import ctypes

from struct import unpack
from ctypes import sizeof

###############################################################################
# Constants
###############################################################################

# Must be synced with kernel: "include/uapi/linux/module_sig_structure.h"
MODULE_SIG_STRING = b"~Module signature appended~\n"
MODULE_SIG_STRING_LEN = len(MODULE_SIG_STRING)

# Default sandbox tag to be used
DEFAULT_SANDBOX_TAG = b"_ML_"
DEFAULT_SANDBOX_TAG_INT = unpack("<I", DEFAULT_SANDBOX_TAG)[0]

###############################################################################
# Typedefs
###############################################################################

class ML_TAIL_DATA(ctypes.Structure):
    # Must be synced with kernel: "include/uapi/linux/ml_sec/tail_data.h"
    ML_TAIL_DATA_MAGIC_STRING = b"TAILDATA"
    ML_TAIL_DATA_MAGIC_INT = unpack("<Q", ML_TAIL_DATA_MAGIC_STRING)[0]
    ML_TAIL_DATA_LATEST_VERSION = 2
    ML_TAIL_DATA_SEINFO_MAXSIZE = 64

    """
    #pragma pack(push, 1)
    struct ml_tail_data {
        /* Version of this structure */
        uint8_t taildata_ver;

        /* Optional - length of attached developer certificate (cert behind this structure) */
        uint32_t devcert_len;

        /* ProfileTag that represents the sandbox profile to apply */
        uint32_t profile_tag;

        /* Flag that specifies if executable's process is debuggable  */
        uint8_t is_debuggable;

        /* Guardian's sentinel type */
        uint8_t sentinel_type;

        /* Guardian's sentinel version */
        uint16_t sentinel_version;

        /* Generic ID field, currently used for watermark */
        uint32_t id;

        /* String that specifies the SELinux context to apply  */
        char seinfo[ML_TAIL_DATA_SEINFO_MAXSIZE];

        /* Magic qword set to 'TAILDATA', to identify the ml_tail_data struct */
        uint64_t tail_data_magic;
    };
    #pragma pack(pop)
    """
    _pack_ = 1
    _fields_ = [
        ("taildata_ver", ctypes.c_ubyte),
        ("devcert_len", ctypes.c_uint),
        ("profile_tag", ctypes.c_uint),
        ("is_debuggable", ctypes.c_ubyte),
        ("sentinel_type", ctypes.c_ubyte),
        ("sentinel_version", ctypes.c_ushort),
        ("id", ctypes.c_uint),
        ("seinfo", ctypes.c_char * ML_TAIL_DATA_SEINFO_MAXSIZE),
        ("tail_data_magic", ctypes.c_ulonglong),
    ]

    # Override ctor to supply default args
    def __init__(self,
                 taildata_ver=ML_TAIL_DATA_LATEST_VERSION,
                 devcert_len=0,
                 profile_tag=DEFAULT_SANDBOX_TAG_INT,
                 is_debuggable=0,
                 sentinel_type=0,
                 sentinel_version=0,
                 id=0,
                 seinfo = b"",
                 tail_data_magic=ML_TAIL_DATA_MAGIC_INT):
        super(ML_TAIL_DATA, self).__init__(taildata_ver, devcert_len, profile_tag, is_debuggable, sentinel_type, sentinel_version, id, seinfo, tail_data_magic)


###############################################################################
# Functions
###############################################################################


def parse_args():
    parser = argparse.ArgumentParser()

    # Optional args
    parser.add_argument("--forcewrite", action="store_true")
    parser.add_argument("--debuggable", action="store_true")
    parser.add_argument("--devcert", metavar="<path_to_dev_certificate>")
    parser.add_argument("--sboxtag", metavar="<4-char_sandbox_tag>")
    parser.add_argument("--seinfo", metavar="<selinux_seinfo>")
    parser.add_argument("--sentinel", metavar="<sentinel_type>", type=int)
    parser.add_argument("--sentinel-version", metavar="<sentinel_version>", type=int)

    # Required args
    parser.add_argument("target_filepath")
    return parser.parse_args()


def is_file_signed(f, filelen):
    """
    This function recieves a python file object 'f' and the file's length 'filelen' and returns
    whether the file contains a signature

    NOTE 1: This function DOES NOT verify the signature, only returns if it exists
    NOTE 2: This function changes the file position pointer
    """
    if filelen < MODULE_SIG_STRING_LEN:
        return False

    f.seek(-MODULE_SIG_STRING_LEN, os.SEEK_END)
    sigmarker = f.read()
    return sigmarker == MODULE_SIG_STRING


def apply_tail_data(target_filepath, taildata, devcert_data, forcewrite):
    with open(target_filepath, "r+b") as f:
        f.seek(0, os.SEEK_END)
        filelen = f.tell()

        # Check file for ML signature (if present, it must be at EOF)
        if is_file_signed(f, filelen):
            print("Error: cannot add tail data - '{}' is already signed".format(target_filepath))
            return False

        # Check file for existing ml_tail_data struct
        # Note that the ml_tail_data struct comes right before the ML signature
        if filelen >= sizeof(taildata):
            f.seek(-sizeof(taildata), os.SEEK_END)
            existing_taildata = ML_TAIL_DATA()
            f.readinto(existing_taildata)
            if existing_taildata.tail_data_magic == ML_TAIL_DATA.ML_TAIL_DATA_MAGIC_INT:
                if forcewrite:
                    # Remove the old taildata + devcert
                    # Note that the devcert comes right before the taildata
                    taildata_len = sizeof(taildata) + existing_taildata.devcert_len
                    if filelen < taildata_len:
                        print("Error: existing taildata in '{}' is malformed".format(target_filepath))
                        return False

                    f.seek(-taildata_len, os.SEEK_END)
                    f.truncate()
                else:
                    # No forcewrite, abort
                    print("Error: cannot add tail data - '{}' already contains tail data".format(target_filepath))
                    return False

        f.seek(0, os.SEEK_END)

        # Write attached devcert (if any)
        if devcert_data:
            f.write(devcert_data)
            taildata.devcert_len = len(devcert_data)

        # Write the tail data
        f.write(taildata)

    return True

###############################################################################
# Main
###############################################################################


def main():
    args = parse_args()
    taildata = ML_TAIL_DATA()

    # Apply argflags into taildata
    if args.sboxtag:
        if len(args.sboxtag) != 4:
            print("Error: Profile tag must have exactly 4 characters (ex. 'ABCD')")
            return -1
        taildata.profile_tag = unpack("<I", args.sboxtag.encode("utf-8"))[0]

    if args.debuggable:
        taildata.is_debuggable = 1

    if args.devcert:
        # Read the developer certificate
        with open(args.devcert, "rb") as f:
            devcert_data = f.read()
    else:
        devcert_data = b""

    if args.seinfo:
        if len(args.seinfo) > ML_TAIL_DATA.ML_TAIL_DATA_SEINFO_MAXSIZE - 1:
            print("Error: SELinux seinfo is longer than maximal length({})".format(ML_TAIL_DATA.ML_TAIL_DATA_SEINFO_MAXSIZE - 1))
            return -1
        taildata.seinfo = args.seinfo.encode("utf-8")

    if args.sentinel:
        taildata.sentinel_type = args.sentinel

    if args.sentinel_version:
        taildata.sentinel_version = args.sentinel_version

    # Write the ml_tail_data struct
    if not apply_tail_data(args.target_filepath, taildata, devcert_data, args.forcewrite):
        return -1

    print("Tail data added successfully to '{}'".format(args.target_filepath))
    return 0

if __name__ == "__main__":
    sys.exit(main())
