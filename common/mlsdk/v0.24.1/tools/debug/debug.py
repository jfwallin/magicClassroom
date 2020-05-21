#!/usr/bin/env python3
#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import argparse
import logging
import mldb
import os
import subprocess
import sys
from sys import stdout
import zipfile
import xml.etree.ElementTree as ET
import re
import json
import time
import shutil
import tempfile


def log(msg):
    logger = logging.getLogger(__name__)
    logger.info(msg)


def error(msg):
    sys.exit("ERROR: {}".format(msg))


def _extract_package_name_manifest(manifest_content):
    """ Parse a manifest xml file and return the package name. """
    manifest = ET.fromstring(manifest_content)
    return manifest.get("{magicleap}package")


def _extract_package_name(package):
    """ Extract (in memory) the manifest.xml from a package and return the package name. """
    try:
        with zipfile.ZipFile(package, 'r') as zin:
            for item in zin.infolist():
                if item.filename == 'manifest.xml':
                    manifest_content = zin.read(item.filename)
                    return _extract_package_name_manifest(manifest_content)
    except Exception as ex:
        error("Failed extract package name from '%s' because of %s" % (package, str(ex)))

    error("package %s does not contain mandatory manifest.xml" % package)


def _host_is_windows():
    return sys.platform.startswith("win")


class ArgumentParser(mldb.ArgumentParser):
    DESCRIPTION = """
Launches a Magic Leap application in gdb

Example usage:
 To deploy, launch and debug mpk:
   debug --deploy-mpk ~/myproj/.out/myproj/com.acme.myproj.mpk ~/myproj/.out/debug_lumin_clang-3.8_aarch64/myexe

 To launch and debug already deployed app:
   debug --package com.acme.myproj ~/myproj/.out/debug_lumin_clang-3.8_aarch64/myexe

 To debug already running app:
   debug --attach --package com.acme.myproj ~/myproj/.out/debug_lumin_clang-3.8_aarch64/myexe


Note:
 - you will see some noise regarding SIGSTOP at the outset of the debug session. This
   is a consequence of how the platform launches the application for debugging. It
   will not affect either the application or your ability to debug it.
"""

    def __init__(self):
        super(ArgumentParser, self).__init__(description=self.DESCRIPTION)

        required_group = self.add_argument_group("required options")
        required_group.add_argument(
            dest="program",
            help="Executable file. Should contain debug info or a link to a sym file.\nThis argument should not be a pure sym file")

        # The --deploy option is deprecated in favor of --deploy-mpk
        # It is supported in order to maintain backward compatibility
        self.add_argument(
            "--deploy", action="store_true",
            help=argparse.SUPPRESS)

        self.add_argument(
            "--verbose", "-v", action="store_true",
            help="enable verbose mode")

        debug_group = self.add_argument_group("optional debugging options")
        debug_group.add_argument(
            "--deploy-mpk", dest="deploy_mpk",
            help="deploy the application package (mpk) to the device")

        debug_group.add_argument(
            "--force", action="store_true",
            help="kill existing debug session if it exists")

        debug_group.add_argument(
            "--attach", action='store_true',
            help="attach to the running process")

        debug_group.add_argument(
            "--env", action='append',
            help="pass an environment variable to a debuggable application")

        debug_group.add_argument(
            "--setup-only", dest="setup_only", action='store_true',
            help="create GDB command file and launch gdbserver on the device, but don't launch GDB client")

        debug_group.add_argument(
            "--port", dest="port", type=int, default="7777",
            help="override port used on the host. Default is 7777")

        debug_group.add_argument(
            "-x", "--exec", dest="gdb_cmd_file",
            help="file containing gdb commands to run after establishing connection")

        debug_group.add_argument(
            "-p", "--package", dest="package",
            help="package name")

        debug_group.add_argument(
            "--sopaths", dest="sopaths",
            help="list of paths to shared libraries you want to debug. The list should be separated by colon (:) on Linux and MacOS or semicolon (;) on Windows.")

        debug_group.add_argument(
            "-c", "--component", dest="component",
            help="application component. Defaults to first component in manifest.xml if not specified")

        if _host_is_windows():
            tui_help = argparse.SUPPRESS
            no_console_help = "start gdb in main window"
        else:
            tui_help = "use GDB's tui mode"
            no_console_help = argparse.SUPPRESS

        debug_group.add_argument(
            "-t", "--tui", action="store_true", dest="tui",
            help=tui_help)

        debug_group.add_argument(
            "--no_console", action="store_true",
            help=no_console_help)

        debug_group.add_argument(
            "-i", "--init_str", dest="init_str",
            help="application initialization string")


def _tools_path():
    # path of debug.py: tools/debug/debug.py
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), '..')


def gdb_bin_path():
    return os.path.join(_tools_path(), 'toolchains', 'bin')


def handle_args():
    args = ArgumentParser().parse_args()

    if args.tui and _host_is_windows():
        error("TUI is unsupported on Windows.")

    if not os.path.isfile(args.program):
        error("File not found: '{}'.".format(args.program))

    if args.verbose:
        logger = logging.getLogger(__name__)
        handler = logging.StreamHandler(sys.stdout)
        formatter = logging.Formatter()

        handler.setFormatter(formatter)
        logger.addHandler(handler)
        logger.propagate = False

        logger.setLevel(logging.INFO)

    return args


# download a file from device to host.
def download_file(args, device_path, host_path, pkg_name):
    device = args.device
    return device.pull(device_path, host_path, package=pkg_name)


# Check if the device is running Lumin OS build before the given id.
# helper for allowing compatibility code to address issues in older devices
def _lumin_version_before(device, lumin_nightly_build_id):

    device_version = device.get_prop("ro.build.id")
    # Recognize the "N03.123" nightly build pattern, treat anything else as new.
    m = re.match("^N03.([0-9]+)$", device_version)
    return m and int(m.group(1)) < lumin_nightly_build_id


def _get_continue_cmd(args):
    if args.attach or args.setup_only:
        # Not needed for an attach setup or "--setup-only" launch (Visual Studio debug)
        return ""

    if _lumin_version_before(args.device, 112):
        # Compatibility with devices before N03.112, did not use attach for debug.
        return ""

    # To debug the application from the beginning, "mldb start -d" sends a SIGSTOP
    # to the application so that when gdbserver attaches, the app is still at the beginning.
    # This signal causes that 3 gdb continue operations are needed to resume it initially,
    # more precisely one continue and one "signal 0" (which ignores the SIGSTOP) also works,
    # but as we want the target to still be suspended when the user starts his session,
    # we do 2 continues at startup.
    return "gdb.execute('continue')\ngdb.execute('continue')\n"


def generate_gdb_command_file(args, sysroot, sopaths, src_search_path):
    base_path = os.path.join(os.path.dirname(args.program), os.path.basename(args.program))
    base_path = base_path.replace("\\", "/")
    if sopaths is None:
        sopaths = sysroot
    if src_search_path is None:
        src_search_path = sysroot   # unlikely to help but worth a try

    continue_cmd = _get_continue_cmd(args)

    gdb_commands = """
set osabi GNU/Linux
set debug aarch64
set history save on
set history filename {base_path}.gdb_history

# Set this so that gdb can find the shared library files on host. Those files
# usually contain symbol data (debug and/or symbol table).
set solib-search-path {sopaths}

# Sysroot provides a mapping for where to look for a loaded library on the host. 
# If the program loads /some/dir/libfoo.so and sysroot is c:\mysrc, then gdb 
# will look for the file at c:\mysrc\some\dir\libfoo.so. When sysroot is not
# set, gdb transfers the file up through gdbserver. This is wasteful if you have
# the library on the host already.
set sysroot {sysroot}

# Set this so that gdb can find the source files if the paths in the debug info
# aren't absolute or don't accurately reflect where the files are on host
directory {directory}

python

# Python 2.7 compatibility
from __future__ import print_function
# Set up the pretty printer
import sys
sys.path.append("{pypr_dir}")
from printers import register_libcxx_printers
register_libcxx_printers(None)

# Try to connect for a few seconds, sometimes the device gdbserver takes
# a little bit to come up.
def target_remote_with_retry(target, timeout_seconds):
    import time
    end_time = time.time() + timeout_seconds
    while True:
        try:
            gdb.execute('target remote ' + target)
            return True
        except gdb.error as e:
            time_left = end_time - time.time()
            if time_left < 0 or time_left > timeout_seconds:
                print("Error: unable to connect to device.", file=sys.stderr)
                print(e, file=sys.stderr)
                return False
            time.sleep(min(0.25, time_left))

target_remote_with_retry(':{port}', {connect_timeout})
{continue_cmd}
end
""".format(base_path=base_path,
           sysroot=sysroot,
           sopaths=sopaths,
           directory=src_search_path,
           pypr_dir=args.pypr_dir.replace("\\", "/"),
           port=args.port,
           connect_timeout=10,
           continue_cmd=continue_cmd)

    # Append the contents of the user's optional gdb command file
    if args.gdb_cmd_file is not None:
        try:
            user_cmd_file = open(args.gdb_cmd_file, "r")
        except IOError:
            error("Failed to open GDB command file: '{}'.".format(args.gdb_cmd_file))

        with user_cmd_file:
            gdb_commands += user_cmd_file.read()

    # Write out the command file
    gen_cmd_file = os.path.join(base_path + '.gdbinit')
    log("Generating GDB command file " + gen_cmd_file)

    with open(gen_cmd_file, "w") as f:
        f.write(gdb_commands)

    return gen_cmd_file


def start_gdb(gdb_path, program, command_file, gdb_flags=None, no_console=False):
    """Start gdb in the background and block until it finishes.

    Args:
        gdb_path: Path of the gdb binary.
        program: Path of the program to debug.
        command_file: file name of the GDB script to run.
        gdb_flags: List of flags to append to gdb command.
        no_console: if True no separate console is created on Windows.
    """

    gdb_args = [gdb_path, program, "-x", mldb.quote_path_if_needed(command_file)] + (gdb_flags or [])
    log("Starting GDB client... (" + ' '.join(gdb_args) + ")")

    kwargs = {}
    if _host_is_windows() and not no_console:
        kwargs["creationflags"] = subprocess.CREATE_NEW_CONSOLE

    gdb_process = subprocess.Popen(gdb_args, **kwargs)
    while gdb_process.returncode is None:
        try:
            gdb_process.communicate()
        except KeyboardInterrupt:
            pass

    if gdb_process.returncode != 0:
        return_code_str = str(gdb_process.returncode)
        if gdb_process.returncode >= 0x80000000:
            return_code_str = hex(gdb_process.returncode)
        print("gdb failed (exit code %s)" % return_code_str, file=sys.stderr)


def device_invocation(func, err_msg):
    """
    Invokes [func] and catches subprocess.CalledProcessError, printing out the
    failure

    We use this for making calls to mldb where a user error is to be expected

    @param err_msg is the final error message to display (in addition to what's
        in the exception object)
    """
    try:
        func()
    except subprocess.CalledProcessError as exc:
        print("ERROR: {}\nERROR: {}".format(str(exc), exc.output.decode('utf-8').strip()), file=sys.stderr)
        error(err_msg)


def _process_exists(device, pkg_name, component=None):
    out = device.ps(as_json=True)
    process_list = json.loads(out)
    for p in process_list:
        if p["package"] != pkg_name:
            continue
        if component is None or p["component"] == component:
            return True
    return False


def _terminate_application(device, pkg_name, component=None):
    """
    Helper to terminate an application, if it is running.
    Allow to do this silently if it is not certain that the application is running.
    """
    log("Terminating the %s application " % pkg_name)
    try:
        device.terminate(pkg_name, component, force=True)
        # device.terminate is asynchronous, wait until device is not shown in mldb ps anymore
        for unused_retries in range(0, 20):
            if not _process_exists(device, pkg_name, component):
                break
            time.sleep(0.1)  # 0.1 seconds, up to 2 seconds + processing time
        else:
            log("WARNING: %s still exists as process on target two seconds after a successful terminate request" % pkg_name)

    except subprocess.CalledProcessError as exc:
        log("ERROR: {}\nERROR: {}".format(str(exc), exc.output.decode('utf-8').strip()))


def main():
    args = handle_args()
    device = args.device

    if device is None:
        error("mldb did not find any connected devices.")

    mldb_version = mldb.version_str(device.mldb_cmd)
    log("MLDB command used: '{}'".format(" ".join(device.mldb_cmd)))
    log("MLDB version: {}".format(" ".join(mldb_version.splitlines())))

    pkg_name = None
    if args.attach:
        # Attach to running process
        if args.package is None:
            raise error("Must specify the package with --package.")

        device_invocation(lambda: device.attach(args.package, host_port=args.port), "Launch failed")
        pkg_name = args.package
    else:
        # Launch application, possibly deploying first
        if args.deploy or args.deploy_mpk:
            if args.deploy_mpk:
                args_mpk = args.deploy_mpk
            else:
                args_mpk = args.package
            log("Deploying to device...")
            # Package (mpk) installation
            if not os.path.isfile(args_mpk):
                error("File not found: '{}'.".format(args_mpk))

            # Extract the package name from the manifest. We need it
            # for launching the
            package_name = _extract_package_name(args_mpk)

            # Do the install
            # This can be a long running operation (the package zip could be hundreds
            # of MBs), and mldb prints updates to stdout throughout the process. This
            # requires special handling for this mldb invocation. Typically we invoke
            # mldb, then print the output in bulk when the command completes. Here
            # we have to forward the stdout in realtime. Also, allow IDEs to reliably
            # find that output by printing markers before and after the operation.
            if args.verbose:
                print("=== INSTALL THROUGH MLDB: BEGIN ===")
            last_line = ""

            def do_install():
                nonlocal last_line
                for line in device.install(args_mpk, replace=True, create_generator=True):
                    # If the line contains the string 'Successfully' then the package has been installed and the line
                    # should be printed on a new line. During the push of the package the progress report needs to
                    # be displayed such that each line overwrites the previous line. By prefacing the string with the
                    # '\r' (Carriage Return) the cursor is returned to the beginning of the line and the next message
                    # is printed over it. The print() function also must be passed "end=''" so that the New Line is not
                    # printed. The standard output stream must be flushed in order for the last line to be output in
                    # the console. (DTOOLS-2032)
                    if not re.match(r'Successfully', line):
                        print("\r" + line.strip(), end='')
                    else:
                        print("\n" + line.strip())
                    last_line = line
                    stdout.flush()
            device_invocation(do_install, "Install failed")

            if args.verbose:
                print("=== INSTALL THROUGH MLDB: END ===")

            if not last_line.startswith("Successfully "):
                error("mldb install printed unexpected output: " + last_line)
            pkg_name = package_name
        else:
            pkg_name = args.package

        # Force all output out now. Fixes strange interleave output issue seen with test
        stdout.flush()

        # Workaround for launch -f not killing debugged application, stop it if it is already running.
        # Remove once issue is fixed.
        _terminate_application(device, pkg_name, args.component)

        device_invocation(lambda: device.launch(pkg_name, args.component, debug=True, host_port=args.port, env=args.env, init_str=args.init_str), "Launch failed")

    args.pypr_dir = os.path.join(
        gdb_bin_path(), "..", "share", "pretty-printers", "libc++", "v1")
    args.pypr_dir = os.path.realpath(args.pypr_dir)  # canonicalize, for neatness
    if not os.path.isdir(args.pypr_dir):
        raise AssertionError("Malformed SDK. gdb pretty printer not found at: %s" % args.pypr_dir)

    # default to directory of the program
    if "ANDROID_BUILD_TOP" in os.environ:
        sysroot = os.path.join(os.environ["OUT"], "symbols")
        src_search_path = os.environ["ANDROID_BUILD_TOP"]

        system_lib_dir = os.path.join(sysroot, "system", "lib64")
        vendor_lib_dir = os.path.join(sysroot, "vendor", "lib64")

        solib_search_path = []
        system_lib_subdirs = ["", "hw", "drm", "soundfx"]
        vendor_lib_subdirs = ["", "hw", "egl", "mediadrm"]
        if args.sopaths is None:
            solib_search_path += [os.path.join(system_lib_dir, x) for x in system_lib_subdirs]
            solib_search_path += [os.path.join(vendor_lib_dir, x) for x in vendor_lib_subdirs]
            solib_search_path = ":".join(solib_search_path)
        else:
            solib_search_path = args.sopaths
    else:
        sysroot = os.path.dirname(args.program)
        sysroot = sysroot.replace("\\", "/")
        if not sysroot:
            sysroot = "."
        solib_search_path = args.sopaths
        src_search_path = None

    # upload some system library files from device to host.
    #
    # Note the linker64 is required for GDB to debug shared libraries.
    #
    # The other library files are nice to have as they contain symbol table (not debug info) that
    # make stepping into a function in the libraries (e.g. printf() in libc) a better experience.
    # Without the files, say, libc.so, user can step into printf() but
    # 1. user will get no calling stack and
    # 2. user cannot step further (with "step-mode" off in GDB) or can step further but cannot step
    #    out (with "step-mode" on in GDB) due to lack of calling stack. The only good choice for user to
    #    get out of woods is to "continue".
    #
    # On the other hand we don't want to upload many shared libraries as
    # 1. that would slow down debugger start,
    # 2. Many libraries should not be uploaded to host for security reasons.
    #
    # So this file list is to-be-determined.
    # TODO: Maybe we want to get the file list from a file ?
    #
    files = ["/system/bin/linker64",
             "/system/lib64/libc.so"]

    for file in files:
        download_file(args, file, sysroot, pkg_name)

    # Generate the gdb command file
    command_file = generate_gdb_command_file(args, sysroot, solib_search_path, src_search_path)

    if not args.setup_only:
        # Start gdb.
        gdb_flags = []
        if args.tui:
            gdb_flags.append("--tui")

        gdb_path = os.path.join(gdb_bin_path(), "gdb")
        gdb_path = os.path.realpath(gdb_path)  # canonicalize, for neatness
        try:
            start_gdb(gdb_path, mldb.quote_path_if_needed(args.program), command_file, gdb_flags, no_console=args.no_console)
        finally:
            if not args.attach:
                # gdb does not kill the launched target as we attached gdbserver to an separately started process
                # Hence ensuring that the application is terminated.
                _terminate_application(device, pkg_name, args.component)


if __name__ == "__main__":
    main()
