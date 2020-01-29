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

import diags
from config import ENV_LAB_ROOT, NAME, HOSTOS, OS_WIN
from finder import find_the_lab, default_install_path

from utils import format_command_line
from workspace import get_driver_workspace

SHOWED_HELP = False


class MyHelpAction(argparse.Action):
    def __init__(self, option_strings, dest, **kwargs):
        argparse.Action.__init__(self, option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        global SHOWED_HELP
        SHOWED_HELP = True
        parser.print_help()
        print()
        # remainder of code will invoke "help" automagically


def parse_arguments():
    parser = ArgumentParser(prog=NAME, description='''Control a Magic Leap Lab instance from the command line.''',
                            add_help=False)

    parser.add_argument("-h", "--help", action=MyHelpAction, nargs=0, help="""
        Show help.
    """)

    parser.add_argument("--lab_root", dest="root", help="""
        Point to The Lab installation to use (default is the last 
        launched instance or {}).
    """.format(default_install_path()))

    parser.add_argument("-o", "--output", dest="output", help="""
        Redirect output to the given file.
        """)

    parser.add_argument(dest='commands',
                        metavar='command [arguments...] [-- command [arguments ...]]',
                        type=str,
                        nargs=argparse.REMAINDER,
                        help="""
        Specify one or more commands (and arguments) for The Lab.
        Use 'help' as a command to list known commands.
    """)

    parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help="""
        Verbose output.
        """.format(default_install_path()))

    # Used to force the workspace _for the driver_
    parser.add_argument("-workspace", dest="workspace", help=argparse.SUPPRESS)

    # process options
    opts, left = parser.parse_known_args(sys.argv[1:])

    if not opts.root:
        env_root = os.getenv(ENV_LAB_ROOT)
        if env_root:
            opts.root = env_root

    return opts, left


def get_out_files(base_output):
    out_out = base_output + ".out"
    err_out = base_output + ".err"
    return out_out, err_out


def process_print(base_output):
    out_out, err_out = get_out_files(base_output)

    # spit out any errors...
    if os.path.exists(err_out):
        with open(err_out, 'rt') as f:
            print(f.read(), file=sys.stderr, end=None)

    # spit out the output...
    if os.path.exists(base_output):
        with open(base_output, 'rt') as f:
            print(f.read(), end=None)
    else:
        # no JSON file, assume stdout has other info (like cmdline arg help)
        if os.path.exists(out_out):
            # exclude logging content before the real meat
            with open(out_out, 'rt') as f:
                print(f.read(), end=None)


if __name__ == "__main__":
    opts, left = parse_arguments()
    lab_root, driver = find_the_lab(opts.root)  # will exit if failed

    cl_workspace = get_driver_workspace(opts.workspace)

    diags.set_debug(opts.verbose or diags.is_debug())
    diags.debug("Using installation at {}", lab_root)

    # send output to a file no matter what, so we can massage the final stdout
    is_temp = False
    if opts.output:
        base_output = os.path.abspath(opts.output)
    else:
        try:
            base_output = tempfile.mktemp(".json", "cldriver")
            is_temp = True
        except IOError as e:
            print("failed to create temp file; use the -output parameter if needed (" + str(e) + ")", file=sys.stderr)
            sys.exit(1)

    diags.debug("sending output to {}", base_output)

    # Windows barfs on multiple args with spaces in one "cmd" command line...
    # just generate/run relative to the possibly spacey root.
    if HOSTOS == OS_WIN:
        script = os.path.join('scripts', 'labdriver_run.cmd')
        args = ['cmd', '/c', script]
    else:
        script = os.path.join('scripts', 'labdriver_run')
        args = [script]

    # script args (must be first)
    args += ['-driver', os.path.relpath(driver, lab_root)]
    args += ['-output', base_output]

    # Eclipse args
    args += ['-data', cl_workspace]

    if SHOWED_HELP and not left and not opts.commands:
        # if only "-help" or "-h" provided, pass this command down,
        # appearing on stdout like "normal" help.
        args += ['-stdio', 'help']
    else:
        args += left + opts.commands

    exp_script = os.path.join(lab_root, script)
    if not os.path.exists(exp_script):
        diags.error("did not find expected file '{}' (this build may be too old or too new)", exp_script)

    diags.debug("launching in {}: {}", lab_root, format_command_line(args))

    try:
        comp = subprocess.run(args, cwd=lab_root, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        print("failed to launch script; please run with --debug: " + str(e), file=sys.stderr)
        sys.exit(1)

    if not opts.output:
        try:
            process_print(base_output)
        finally:
            # remove output-related generated files
            out_out, err_out = get_out_files(base_output)
            to_delete = [out_out, err_out, base_output] if is_temp else [out_out, err_out]
            for path in to_delete:
                try:
                    os.remove(path)
                except IOError:
                    pass

    if comp.returncode == 13:
        log_file = os.path.join(cl_workspace, ".metadata", ".log")
        if os.path.exists(log_file):
            print("The Lab failed to start up.  Details are in: '" + log_file + "'")
        else:
            print("The Lab failed to start up.  Please run again with --debug for more information.")

    sys.exit(comp.returncode)
