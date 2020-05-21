#!/usr/bin/env python

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
#
# This script will symbolize a tombstone file
# Usage: python opt.py [-t] path_to_symbols_dir tombstone.log [path_to_global_debug_dir]

import os
import re
import subprocess
import sys
import argparse
import tempfile
import platform

# print() but to stderr
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

# Return the Build ID for an elf file
# Example output of readelf -n:
# Displaying notes found at file offset 0x00000254 with length 0x00000020:
#   Owner                 Data size	Description
#   GNU                  0x00000010	NT_GNU_ABI_TAG (ABI version tag)
#     OS: Linux, ABI: 2.6.24
#
# Displaying notes found at file offset 0x00000274 with length 0x00000024:
#   Owner                 Data size	Description
#   GNU                  0x00000014	NT_GNU_BUILD_ID (unique build ID bitstring)
#     Build ID: 0f7ff287cf26eba9a6643b1226089eea57cb7e44
def get_build_id_from_elf(path):
    cmdline = [readelf_path, '-n', path ]
    try:
        output = subprocess.check_output(cmdline, stderr=subprocess.DEVNULL).decode().strip()
    except subprocess.CalledProcessError:
        eprint("warning: readelf failed. Full cmdline was : " + ' '.join(cmdline))
        return None

    pattern = re.compile(r'[ ]*Build ID: ([0-9a-f]+)')
    match = pattern.search(output)
    return None if match is None else match.groups()[0]


# Return text for the given frame info
def generate_frame_line(frame_index, function, src_file_path, src_line_number):
    return '\n    #{} {} at {}:{}'.format(frame_index, function, src_file_path, src_line_number)

# Return a dictionary of ELF:buildid pairs found in the given crashdump
#
# The crashdump mentiones the build-id whenever it references an ELF. E.g.:
#        00004000'20f50000-00004000'20febfff r-x         0     9c000  /system/bin/linker64 (BuildId: 3cc0923d613ba309d27a2ed7b0dcbffb)
def get_build_ids_from_crashdump(file):
    build_ids = {}
    pattern = re.compile(r'.+ [0-9a-f]+[ ]+(.+)[ ]+\(BuildId: ([0-9a-f]+)\)')
    for line in file:
        match = pattern.search(line)
        if match is None:
            continue

        elf = match.groups()[0]
        build_id = match.groups()[1]
        build_ids[elf] = build_id

    return build_ids

# Determine if the given line from a crashdump is a backtrace line. If so,
# parse it and return the address, ELF, and frame index (zero based).
# Otherwise, return a list of Nones
#
# Example frame line
#    #00 pc 0000000000000d3c  /data/app/com.magicleap.sample.simplenativeapp/bin/SimpleNativeApp
#    #01 pc 0000000000000bb4  /data/app/com.magicleap.sample.simplenativeapp/bin/SimpleNativeApp (main+28)
def parse_frame_line(line):
    pattern1 = re.compile(r'[ ]*#([0-9]+)[ ]+pc[ ]+([0-9a-f]+)[ ]+(.+)[ ]+\(.*\)')  # with trailing (function+offset)
    pattern2 = re.compile(r'[ ]*#([0-9]+)[ ]+pc[ ]+([0-9a-f]+)[ ]+(.+)')
    match = pattern1.search(line)
    if match is None:
        match = pattern2.search(line)
        if match is None:
            return (None, None, None)

    index = match.groups()[0]
    addr = match.groups()[1]
    elf = match.groups()[2]
    return (index, addr, elf)

# Invoke the addr2line program and return function, file_path, and line
# Example output of addr2line:
#    basic_string at /proc/self/cwd/external/libcxx/include/string:1581
def addr2line(sym_file, address):
    cmdline = [addr2line_path, '-C', '-p', '-f', '-e', sym_file, address]
    try:
        output = subprocess.check_output(cmdline, stderr=subprocess.DEVNULL).decode().strip()
    except subprocess.CalledProcessError:
        eprint("warning: addr2line failed. Full cmdline was : " + ' '.join(cmdline))
        return (None, None, None)

    pattern = re.compile(r'[ ]*Build ID: ([0-9a-f]+)')
    match = pattern.search(output)

    # Example addr2line output:
    #    crash() at /ssd/git/mlsdk-mirror-linux64-2018/samples/SimpleNativeApp/main.cpp:41
    pattern = re.compile(r'[ ]*(.*) at (.*):([0-9]+)')
    match = pattern.search(output)
    if match is None:
        return (None, None, None)

    function = match.groups()[0]
    src_file_path = match.groups()[1]
    src_file_line = match.groups()[2]
    return (function, src_file_path, src_file_line)

def is_sym_file(path):
    # Note that a SYM file is an ELF file, same as executables and shared
    # libraries. And because we cannot reasonably distinguish between the
    # two, we rely on file extensions. I.e., if a directory contains both
    # the sym file and the executable/so, there is no guarantee we'd end
    # up using the sym file. We might accidentally use the executable/so,
    # which wouldn't have the symbolics information.
    (dontcare, ext) = os.path.splitext(path)
    return ext in sym_extensions

# If an only if
def add_sym_file(path, result):
    build_id = get_build_id_from_elf(path)
    if build_id:
        result[build_id] = path
    else:
        eprint("warning: '{}' has no build-id. Ignoring".format(path))

# Returns a dictionary of build_id:ELF based on the SYM files
# found in the given directory
def get_sym_files(directory, result):
    for dirpath, dirnames, filenames in os.walk(directory):
        for filename in filenames:
            abs_path = os.path.join(dirpath, filename)
            if (is_sym_file(abs_path)):
                add_sym_file(abs_path, result)
        for dirname in dirnames:
            abs_path = os.path.join(dirpath, dirname)
            get_sym_files(abs_path, result)

# Symbolize the backtraces in the given crashdump, using symbolics
# from the given list of sym files and/or directory of sym files
def symbolize_crashdump(file, symbols_paths):
    file.seek(0, 0)
    build_ids = get_build_ids_from_crashdump(file)

    sym_files = {}
    for symbols_path in symbols_paths:
        if os.path.isdir(symbols_path):
            get_sym_files(symbols_path, sym_files)
        else:
            add_sym_file(symbols_path, sym_files)

    file.seek(0, 0)
    output = ''

    # ELFs in backtrace for which we did not find SYM files, and for which
    # we've already warned the user
    warned_elfs = set()

    for line in file:
        line = line.rstrip()
        line_handled = False

        (frame_index, addr, elf) = parse_frame_line(line)
        if frame_index:
            # It's a frame. Symbolize it
            build_id = None
            if elf in build_ids:
                build_id = build_ids[elf]

            if build_id in sym_files:
                sym_file = sym_files[build_id]
                if sym_file:
                    (function, src_file_path, src_line_number) = addr2line(sym_file, addr)
                    if function:
                        output += generate_frame_line(frame_index, function, src_file_path, src_line_number)
                        line_handled = True
            else:
                if elf not in warned_elfs and elf.startswith("/data/"):  # don't warn about system modules
                    eprint("warning: no SYM file found for '{}'".format(elf))
                    warned_elfs.add(elf)


        if not line_handled:
            # Not a backtrace line, it is but could not source-correlate it.
            # Add it to output as is
            output += '\n' + line

    file.close()
    return (output)

# Check that file/directory exists and if not print error and exit
def vet_file_or_dir(path):
    if not os.path.exists(path):
        print("File not found: '{}'".format(path))
        exit(1)

# Update the global list of sym extensions we use from the given
# list specified via --symext
def update_sym_extensions(extensions):
    for extension in extensions:
        extension = extension if (extension[0] == '.') else ('.' + extension)
        sym_extensions.add(extension)


# ---------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------

cwd = os.getcwd()
this_dir = os.path.dirname(os.path.abspath(__file__))

sym_envvar = 'ML_SYMBOLS'

# Default extensions we search for when looking for SYM files
sym_extensions = {'.sym', '.SYM'}

# Generate the absolute paths of the toolchain programs we use
tool_relpath_prefix = '../toolchains/bin/aarch64-linux-android-'

# Form the absolute paths to the toolchain programs we'll use
addr2line_path = os.path.realpath(os.path.join(this_dir, tool_relpath_prefix + 'addr2line'))
if platform.system() == 'Windows':
    addr2line_path += ".exe"
vet_file_or_dir(addr2line_path)

readelf_path = os.path.realpath(os.path.join(this_dir, tool_relpath_prefix + 'readelf'))
if platform.system() == 'Windows':
    readelf_path += ".exe"
vet_file_or_dir(readelf_path)

# Command line argument definitions
parser = argparse.ArgumentParser(description="Add source filename and line numbers to backtraces in a crash dump")
parser.epilog = \
"""
You can define environment variable {} to have one or more SYM files and/or
directories that contain SYM files, delimited with '{}'.
SYM files must contain a build-id or they will be ignored. That ID is essential
to finding the SYM file that exactly matches the executable or shared library
involved in the crash. See https://sourceware.org/gdb/onlinedocs/gdb/Separate-Debug-Files.html.
The crash dump contains the build-ids of the involved ELFs.

""".format(sym_envvar, os.pathsep)

parser.add_argument('-x', "--symext", type=str, action='append', help="Additional file extension for identifying SYM files. Default is .sym and .SYM.")
parser.add_argument('-s', '--symbols', type=str, action='append', help="SYM file or directory containing SYM files. Used in addition to any specified in environment variable " + sym_envvar)
parser.add_argument('crashdump', nargs='?', type=argparse.FileType('r'), default=sys.stdin, help="Crash dump file or stdin")
args = parser.parse_args()

# Each --symbols can be a directory or a file
symbols_abs_paths = []
if args.symbols:
    for symbols_abs_path in args.symbols:
        if not os.path.isabs(symbols_abs_path):
            symbols_abs_path = os.path.join(cwd, symbols_abs_path)
        vet_file_or_dir(symbols_abs_path)
        symbols_abs_paths.append(symbols_abs_path)

# Because specifying the symbol files on each invocation can be cumbersome,
# we support specifying them via an environment variable
if sym_envvar in os.environ:
    envvar_syms = os.environ[sym_envvar]
    for element in envvar_syms.split(os.pathsep):
        if not os.path.isabs(element):
            eprint("warning: ignoring file/dir in environment variable {} because it is not an absolute path: {}".format(sym_envvar, element))
            continue
        if not os.path.exists(element):
            eprint("warning: ignoring file/dir in environment variable {} because it does not exist: {}".format(sym_envvar, element))
            continue
        symbols_abs_paths.append(element)

if len(symbols_abs_paths) == 0:
    parser.print_usage()
    print("error: no SYM files specified via cmdline arguments or environment variable {}\n".format(sym_envvar))
    exit(1)

# User can specify additional SYM files extensions
if args.symext:
    update_sym_extensions(args.symext)

# We support the user piping in STDIN or a file. To handle both the same way
# we write the contents of the input argument to a temporary physical file,
# then operate on that file
with tempfile.NamedTemporaryFile(delete=False) as tmpfile:
    for line in args.crashdump:
        tmpfile.write(str.encode(line))

try:
    with open(tmpfile.name, 'r') as file:
        # Open given crashdump file and symbolize it
        print(symbolize_crashdump(file, symbols_abs_paths))
finally:
    os.remove(tmpfile.name)

