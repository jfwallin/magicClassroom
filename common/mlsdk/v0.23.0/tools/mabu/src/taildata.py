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

import ctypes
import os
import struct
from ctypes import sizeof
from socket import ntohl

import diags
import taildata_v2
import taildata_v3

_module = taildata_v3


def set_taildata_version(user_version):
    """
    Set the version of taildata used by the system.
    :param user_version: version to use, 2 or 3
    """
    global _module

    if not user_version:
        user_version = 3

    if user_version == 2:
        _module = taildata_v2
    elif user_version == 3:
        _module = taildata_v3
    else:
        diags.warn('unexpected taildata version {}, using 3'.format(user_version))
        _module = taildata_v3


def taildata_module():
    """
    Get the module with taildata support
    :rtype: taildata_v2 | taildata_v3
    """
    global _module
    return _module


def is_file_signed(path):
    """
    Check if file was signed
    :param path:
    """
    with open(path, 'rb') as f:
        f.seek(0, os.SEEK_END)
        filelen = f.tell()
        return _is_file_signed(f, filelen)


def file_has_devcert(path):
    """
    Check if file has taildata that provides devcert
    :param path:
    """
    taildata, _ = find_tail_data(path)
    return taildata and taildata.devcert_len != 0


class _MODULE_SIGNATURE(ctypes.Structure):
    _fields_ = [
        ("algo", ctypes.c_ubyte),
        ("hash", ctypes.c_ubyte),
        ("id_type", ctypes.c_ubyte),
        ("signer_len", ctypes.c_ubyte),
        ("key_id_len", ctypes.c_ubyte),
        ("pad", ctypes.c_ubyte*3),
        ("sig_len", ctypes.c_uint),
    ]
    PKEY_ID_PKCS7 = 2

    def values_are_defaults(self):
        return self.algo == 0 and \
            self.hash == 0 and \
            self.id_type == self.PKEY_ID_PKCS7 and \
            self.signer_len == 0 and \
            self.key_id_len == 0 and \
            self.pad[0] == 0 and self.pad[1] == 0 and self.pad[2] == 0


_MODULE_SIG_STRING = b"~Module signature appended~\n"
_MODULE_SIG_STRING_LEN = len(_MODULE_SIG_STRING)


def _is_file_signed(f, filelen):
    """
    Check if file was signed
    :param path:
    """
    if filelen < _MODULE_SIG_STRING_LEN:
        return False

    f.seek(filelen - _MODULE_SIG_STRING_LEN, os.SEEK_SET)
    sigmarker = f.read()
    return sigmarker == _MODULE_SIG_STRING


def has_taildata(path):
    """
    Check if file has taildata
    :param path:
    """
    taildata, _ = find_tail_data(path)
    return taildata is not None


def _skip_signature(f, filelen):
    """
    Skip any signature on a file.
    Position :f: to end of file or signature
    """
    f.seek(0, os.SEEK_END)
    if _is_file_signed(f, filelen):
        # Signature found, parse it in order to seek back
        modsig = _MODULE_SIGNATURE()
        modsig_struct_len = _MODULE_SIG_STRING_LEN + sizeof(_MODULE_SIGNATURE)
        if filelen < modsig_struct_len:
            # Malformed file
            return
        f.seek(-modsig_struct_len, os.SEEK_END)
        f.readinto(modsig)

        modsig_total_len = modsig_struct_len + ntohl(modsig.sig_len) + modsig.key_id_len + modsig.signer_len
        if filelen < modsig_total_len:
            # Malformed file
            return
        f.seek(-modsig_total_len, os.SEEK_END)
    else:
        f.seek(0, os.SEEK_END)


def _find_tail_data_v2(f, filelen):
    sz = sizeof(taildata_v2.ML_TAIL_DATA)

    if filelen >= sz:
        f.seek(-sz, os.SEEK_CUR)
        pos = f.tell()
        existing_taildata = taildata_v2.ML_TAIL_DATA()
        f.readinto(existing_taildata)
        if existing_taildata.tail_data_magic == taildata_v2.ML_TAIL_DATA.ML_TAIL_DATA_MAGIC_INT:
            return existing_taildata, pos

    return None, None


def _find_tail_data_v3(f, filelen):
    sz = sizeof(taildata_v3.ML_TAIL_DATA)

    if filelen >= sz:
        f.seek(-sz, os.SEEK_CUR)
        pos = f.tell()
        existing_taildata = taildata_v3.ML_TAIL_DATA()
        f.readinto(existing_taildata)
        if existing_taildata.tail_data_magic == taildata_v3.ML_TAIL_DATA.ML_TAIL_DATA_MAGIC_INT:
            return existing_taildata, pos

    return None, None


def find_tail_data(path):
    """
    This reads the taildata and also sets the developer certificate
    in the `devcert` field.
    :rtype: Tuple(taildata_v2.ML_TAIL_DATA | taildata_v3.ML_TAIL_DATA, int)
    """
    with open(path, 'rb') as f:
        f.seek(0, os.SEEK_END)
        filelen = f.tell()

        _skip_signature(f, filelen)
        endpos = f.tell()

        data, pos = _find_tail_data_v3(f, filelen)
        if data:
            if data.devcert_len > 0:
                f.seek(pos - data.devcert_len)
                data.devcert = f.read(data.devcert_len)
            return data, pos

        f.seek(endpos, os.SEEK_SET)
        data, pos = _find_tail_data_v2(f, filelen)

        if data and data.devcert_len > 0:
            f.seek(pos - data.devcert_len)
            data.devcert = f.read(data.devcert_len)

        return data, pos


def find_module_signature(f, filelen):
    """
    Find the signature info for the specified file object 'f'.
    If siginfo is found, returns tuple of (signature_total_length, sigbytes_length)
    (signature_total_length includes header length and such)
    If siginfo is not found (or error), returns None

    NOTE: This function changes the file position pointer
    :return: None | Tuple(int, int)
    """

    # Check that file contains signature
    if not _is_file_signed(f, filelen):
        return None

    # Find the signature offset and length
    modsig = _MODULE_SIGNATURE()
    modsig_struct_len = _MODULE_SIG_STRING_LEN + sizeof(_MODULE_SIGNATURE)
    if filelen < modsig_struct_len:
        # Malformed file
        return None
    f.seek(-modsig_struct_len, os.SEEK_END)
    f.readinto(modsig)

    if not modsig.values_are_defaults():
        diags.warn("Signature contains unexpected header values: " + str(modsig))
        return None

    modsig_total_len = modsig_struct_len + ntohl(modsig.sig_len) + modsig.key_id_len + modsig.signer_len
    if filelen < modsig_total_len:
        # Malformed file
        return None

    return modsig_total_len, ntohl(modsig.sig_len)


def remove_modsig(filepath):
    """
    Removes the module signature from the specified file.
    If file has no signature, does nothing.
    Returns True/False for Success/Failure.
    """
    with open(filepath, "r+b") as f:
        f.seek(0, os.SEEK_END)
        filelen = f.tell()

        # Make sure file is signed
        if not taildata_v3.is_file_signed(f, filelen):
            return True

        # Remove signature
        siginfo = find_module_signature(f, filelen)
        if siginfo is None:
            return False

        f.seek(-siginfo[0], os.SEEK_END)
        f.truncate()
        return True


def pop_tail_data(filepath):
    """
    Removes the taildata from the specified file and returns it.
    If file has no taildata, the file will not be changed and a default taildata struct will be returned.
    """
    # Check file for existing taildata
    taildata_info = find_tail_data(filepath)
    if taildata_info is None:
        return

    with open(filepath, "r+b") as f:
        f.seek(0, os.SEEK_END)

        # taildata found, remove it from the file (assume no devcert in taildata)
        taildata, taildata_pos = taildata_info
        if taildata_pos is not None:
            f.seek(taildata_pos, os.SEEK_SET)
            f.truncate()

        # Return the found/generated taildata
        return taildata


def dump_tail_data_struct(taildata, verbose=False):
    if not verbose:
        print("  DevCert Present: {}".format(taildata.devcert_len > 0 and "yes" or "no"))
        print("  Is Debuggable: {}".format(taildata.is_debuggable > 0 and "yes" or "no"))

    if verbose:
        print("  Taildata version: {}".format(taildata.taildata_ver))
        tag_bytes = struct.pack("<I", taildata.profile_tag)
        print("  Sandbox ProfileTag: {}".format(str(tag_bytes, encoding='UTF-8')))
        print("  Is Debuggable: {}".format(taildata.is_debuggable))
        print("  DevCert Length: {}".format(taildata.devcert_len))
        print("  ID: {:x}".format(taildata.id))
        if taildata.union.seinfo:
            print("  SELinux seinfo / Serial: {}".format(taildata.union.seinfo))
        else:
            print("  SELinux seinfo / Serial: None")

        if taildata.sentinel_type != 0:
            print("  Sentinel Type: {}".format(taildata.sentinel_type))
            print("  Sentinel Version: {}".format(taildata.sentinel_version))
