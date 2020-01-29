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

import os, sys

import diags
from config import ENV_LAB_ROOT, HOSTOS, OS_OSX, DEFAULT_INSTALL_DIR_OSX, DEFAULT_INSTALL_DIR_OTHER
from locking import AtomicOpen

from config import OS_WIN


def default_install_path():
    if HOSTOS == OS_OSX:
        return DEFAULT_INSTALL_DIR_OSX
    return DEFAULT_INSTALL_DIR_OTHER


def resolve_the_lab_exe_osx(root):
    """
    Try to find the root of The Lab from the given root,
    which is expected to be inside a 'The Lab.*.app' folder.
    :param root:
    :return: executable or None
    """
    if not os.path.isdir(root):
        root = os.path.split(root)[0]

    best_guess = None

    while len(root) > 1 and os.path.isdir(root):
        diags.debug("checking {}", root)
        if root.endswith(".app"):
            the_dir = os.path.join(root, 'Contents', 'MacOS')
            if os.path.isdir(the_dir):
                files = os.listdir(the_dir)

                diags.debug("checking executables in {}", root)
                for file in files:
                    the_exe = os.path.join(the_dir, file)
                    if os.path.isfile(the_exe) and os.access(the_exe, os.X_OK):
                        diags.debug("... checking {}", the_exe)
                        if not best_guess or 'the_lab' == file:
                            return the_exe
                        if not best_guess and 'eclipse' == file:
                            best_guess = the_exe

                    if best_guess:
                        return best_guess

        root = os.path.split(root)[0]

    return None


def resolve_the_lab_root_osx(root):
    """
    Try to find the root of The Lab from the given root,
    which is expected to be inside a 'The Lab.*.app' folder.
    :param root:
    :return: updated root or None
    """
    if not os.path.isdir(root):
        # maybe this macOS user doesn't know '.app' is the hidden suffix
        with_app = root + '.app'
        if os.path.isdir(with_app):
            root = with_app
        else:
            root = os.path.split(root)[0]

    while len(root) > 1 and os.path.isdir(root):
        diags.debug("checking {}", root)
        if root.endswith(".app"):
            the_dir = os.path.join(root, 'Contents', 'Eclipse')
            return the_dir

        root = os.path.split(root)[0]

    return None


def resolve_the_lab_exe_other(root):
    """
    Try to find the root of The Lab from the given root,
    which is expected to be inside a folder where 'plugins'
    and 'features' exist.
    :param root:
    :return: executable or None
    """
    if not os.path.isdir(root):
        root = os.path.split(root)[0]

    best_guess = None

    file_ext = ""
    if HOSTOS == OS_WIN:
        file_ext = ".exe"

    while len(root) > 3 and os.path.isdir(root):
        diags.debug("checking {}", root)
        if os.path.isdir(os.path.join(root, 'plugins')) and os.path.isdir(os.path.join(root, 'features')):
            the_dir = root
            files = os.listdir(the_dir)

            diags.debug("checking executables in {}", root)
            for file in files:
                if file.endswith(".ini"):
                    the_exe_no_ext = os.path.join(the_dir, os.path.splitext(file)[0])
                    the_exe = the_exe_no_ext + file_ext

                    if os.path.isfile(the_exe) and os.access(the_exe, os.X_OK):
                        diags.debug("... checking {}", the_exe)
                        if not best_guess or 'the_lab' in file:
                            # prefer console launcher
                            the_exec = the_exe_no_ext + "c" + file_ext
                            if os.path.isfile(the_exec) and os.access(the_exec, os.X_OK):
                                return the_exec
                            else:
                                return the_exe
                        if 'eclipse' in file:
                            best_guess = the_exe

            if best_guess:
                return best_guess

        root = os.path.split(root)[0]

    return None


def resolve_the_lab_root_other(root):
    """
    Try to find the root of The Lab from the given root,
    which is expected to be inside a folder where 'plugins'
    and 'features' exist.
    :param root:
    :return: updated root or None
    """
    if not os.path.isdir(root):
        # maybe this macOS user doesn't know '.app' is the hidden suffix
        with_app = root + '.app'
        if os.path.isdir(with_app):
            root = with_app
        else:
            root = os.path.split(root)[0]

    while len(root) > 3 and os.path.isdir(root):
        diags.debug("checking {}", root)
        if os.path.isdir(os.path.join(root, 'plugins')) and os.path.isdir(os.path.join(root, 'features')):
            return root

        root = os.path.split(root)[0]

    return None


def find_the_lab_osx(root):
    """
    Try to find the root of The Lab from the given root,
    which is expected to be inside a 'The Lab.app' folder.
    :param root:
    :return: updated root and driver path (else exits)
    """
    path = resolve_the_lab_root_osx(root)
    if not path:
        diags.error("did not detect a recent launch of The Lab or a default installation in {};\n"
                    "please pass '--lab_root ...' or set the environment variable '{}'",
                    root, ENV_LAB_ROOT)

    driver = resolve_the_lab_exe_osx(path)
    if not driver:
        diags.error("did not find The Lab executable in {}; "
                    "please pass '--lab_root ...' or set the environment variable '{}'",
                    path, ENV_LAB_ROOT)

    return path, driver


def find_the_lab_other(root):
    """
    Try to find the root of The Lab from the given root.
    :param root:
    :return: updated root or None
    """
    path = resolve_the_lab_root_other(root)
    if not path:
        diags.error("did not detect a recent launch of The Lab or a default installation in {};\n"
                    "please pass '--lab_root ...' or set the environment variable '{}'",
                    root, ENV_LAB_ROOT)

    driver = resolve_the_lab_exe_other(path)
    if not driver:
        diags.error("did not find The Lab executable in {}; "
                    "please pass '--lab_root ...' or set the environment variable '{}'",
                    path, ENV_LAB_ROOT)

    return path, driver


def get_installation_registry():
    return os.path.join(os.path.expanduser("~/.config/the_lab/installations.txt"))


class Installation(object):
    def __init__(self, directory, is_temp, timestamp):
        self.directory = directory
        self.is_temp = is_temp
        self.timestamp = timestamp

    def __repr__(self):
        return str((self.directory, self.is_temp, self.timestamp))

def get_installations():
    CURRENT_VERSION = 2

    installs = []

    regFile = get_installation_registry()

    def complain(format, content):
        diags.warn(format + ", please use --lab_root=... if needed", content, version, regFile)

    try:
        # need to provide "write" to lock a file...
        with AtomicOpen(regFile, "r+") as f:
            lines = f.readlines()
    except FileNotFoundError:
        return installs

    version = None
    for line in lines:
        # ignore comments
        line = line.strip()
        if not line or line[0] == '#':
            continue

        # first line is version code
        if not version:
            try:
                version = int(line)
                if version < CURRENT_VERSION:
                    complain("unrecognized version \"{}\"", version)
            except ValueError:
                complain("unparseable version \"{}\"", version)
                break
            continue

        # all others are entries
        fields = line.split('\t')
        if len(fields) >= 3:
            try:
                installs.append(Installation(fields[0], fields[1] == 'true', int(fields[2])))
            except:
                complain("unparseable line \"{}\"", line)
        else:
            complain("unrecognized line \"{}\"", line)

    return installs


def find_the_lab(user_root):
    if not user_root:
        installs = get_installations()
        diags.debug("raw entries: {}", installs)

        # get newest installation
        installs = [install for install in installs if not install.is_temp and os.path.isdir(install.directory)]
        installs.sort(key=lambda install: install.timestamp, reverse=True)

        diags.debug("after filtering: {}", installs)
        if installs:
            newest = installs[0]
            user_root = newest.directory
        else:
            user_root = os.path.expanduser(default_install_path())

    if HOSTOS == OS_OSX:
        return find_the_lab_osx(user_root)
    else:
        return find_the_lab_other(user_root)
