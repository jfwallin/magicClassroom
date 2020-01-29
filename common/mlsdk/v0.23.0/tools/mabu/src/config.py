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

import os

from diags import debug
from utils import get_script_dir

NAME = 'mabu'
VERSION = '0.23.0.191021'

# user project extension
PROJ_EXT = '.mabu'
# package project extension
PACKAGE_EXT = '.package'
# component (pre-built) extension
COMP_EXT = '.comp'
# configuration extension
CONFIG_EXT = '.config'
# option extension
OPTION_EXT = '.option'
# all known project extensions
ALL_EXTS = [PACKAGE_EXT, PROJ_EXT, COMP_EXT, CONFIG_EXT, OPTION_EXT]

# name of toplevel makefile, which means it can't be a user project
TOPLEVEL_MAKEFILE_NAME = 'top.level'

# package extension
BUILT_PACKAGE_EXT = '.mpk'

# name of the ML manifest
MANIFEST_FILE = 'manifest.xml'

# signing info
SIGN_ALG = 'sha512'
SIGN_CERT = '.cert'   # certificate
SIGN_PRIVKEY = '.privkey'  # private key


def fake_entry():
    pass


SCRIPT_DIR = get_script_dir(fake_entry)

OS_WIN = 'win'
OS_OSX = 'osx'
OS_LINUX = 'linux'

if os.name == 'nt' or os.name == 'ce':
    HOSTOS = OS_WIN
elif os.sys.platform == 'darwin':
    HOSTOS = OS_OSX
else:
    HOSTOS = OS_LINUX

os_to_platform_map = {OS_WIN: 'win64', OS_LINUX: 'linux64', OS_OSX: 'osx'}


def locate_mingw():
    path = os.path.realpath(os.path.join(BUILD_DIR, 'tools/MinGW/bin'))
    if os.path.isdir(path) or HOSTOS != OS_WIN:
        return path
    path = 'C:/MinGW'
    if os.path.isdir(path):
        return path
    path = 'E:/MinGW'
    if os.path.isdir(path):
        return path
    import diags
    diags.error("Did not detect MinGW installation")


BUILD_DIR = os.path.realpath(os.path.join(SCRIPT_DIR, '..'))
DATA_DIR = os.path.realpath(os.path.join(BUILD_DIR, 'data'))
TOOLS_DIR = os.path.realpath(os.path.join(BUILD_DIR, 'tools'))
MINGW_DIR = locate_mingw()
MSYS_DIR = os.path.realpath(os.path.join(MINGW_DIR, '../msys/1.0/bin'))


def update_data_dirs_for_mlsdk(mlsdk):
    """
    When a user MLSDK override is set, update the paths to builtin components.
    We leave BUILD_+DIR, TOOLS_DIR, etc. alone, since they are tied to the
    running mabu instance.
    """
    mlsdk_build_dir = os.path.realpath(os.path.join(mlsdk, 'tools/mabu'))
    mlsdk_data_dir = os.path.join(mlsdk_build_dir, 'data')
    import config
    config.DATA_DIR = mlsdk_data_dir
    debug("Changed DATA_DIR to", config.DATA_DIR)


# default directory for output if no "-o" or "--out" used
DEFAULT_OUT_DIR = '.out'

# where binary files live in an .mpk
DEFAULT_PACKAGE_BINARY_DIR = 'bin'


def get_dev_sdk_path():
    # location assumed relative to mabu / buildsystem
    p = os.path.join(BUILD_DIR, '../mlsdk')
    if os.path.isdir(p):
        return p
    return None


def get_host_segment():
    os_platform_map = {OS_WIN: 'win64', OS_LINUX: 'linux64', OS_OSX: 'osx'}
    return os_platform_map[HOSTOS]


def get_runtime_library_path_var():
    return "DYLD_LIBRARY_PATH" if HOSTOS == OS_OSX else \
        "LD_LIBRARY_PATH" if HOSTOS == OS_LINUX else "PATH"
