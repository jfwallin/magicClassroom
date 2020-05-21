# %BANNER_BEGIN%
# ---------------------------------------------------------------------
# %COPYRIGHT_BEGIN%
#
# Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
import argparse
import os, sys
import tempfile
from argparse import ArgumentParser
import subprocess
import locale

import diags
from config import ENV_LAB_ROOT, NAME, HOSTOS, OS_WIN
from finder import find_the_lab, default_install_path

from utils import format_command_line
from workspace import get_driver_workspace


def parse_arguments():
    parser = ArgumentParser(prog=NAME, description='''
    Locate an instance of The Lab.
    ''',
                            add_help=False, allow_abbrev=False)

    parser.add_argument("--lab_root", dest="root", help="""
        Point to The Lab installation to use (default search order
        is (1) the ML_LAB_ROOT environment variable,
        (2) the last launched instance of The Lab, then 
        (3) the presumed default installation at {}).
    """.format(default_install_path()))

    parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help="""
        Verbose output.
        """.format(default_install_path()))

    # process options
    return parser.parse_known_args(sys.argv[1:])


def main():
    opts, left = parse_arguments()
    lab_root, driver = find_the_lab(opts.root)  # will exit if failed

    diags.set_debug(opts.verbose or diags.is_debug())
    diags.debug("Using installation at {}", lab_root)

    if HOSTOS == OS_WIN:
        script = 'labdriver.cmd'
    else:
        script = 'labdriver'

    exp_script = os.path.join(lab_root, script)
    if not os.path.exists(exp_script):
        diags.error("did not find expected file '{}' (this build may be too old or too new)", exp_script)

    print(exp_script)

if __name__ == "__main__":
    main()
