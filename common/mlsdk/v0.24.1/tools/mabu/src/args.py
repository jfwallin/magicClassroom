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

import argparse
import os
import re
import shlex
import shutil
import sys
from argparse import ArgumentParser, ArgumentTypeError

import build_env
import diags
import utils
from build_model import BuildException, Path
from build_vars import CXX, CC
from config import NAME, PACKAGE_EXT, get_dev_sdk_path, BUILT_PACKAGE_EXT, get_host_segment, get_runtime_library_path_var
from config import PROJ_EXT, TOOLS_DIR
from config import TOPLEVEL_MAKEFILE_NAME
from config import VERSION
from diags import error, debug, set_debug, warn, info

from run_vars import runtime_values, MLSDK, MLTOOLS, MABUPATH, SPEC, MLCERT, SIGNER, MAKE, MAKEARGS, VCDIR, HOST, \
    SIGN_FILE
from config import HOSTOS, OS_WIN
from config import MSYS_DIR
from config import BUILD_DIR
from config import SCRIPT_DIR
from tool_vars import TOOL_VARS
from toolchains._base import Toolchain


def parse_target(s):
    """
    Parse simple target, ignoring VAR=VALUE
    :return: str
    """
    if '=' not in s:
        return None
    return s


def parse_variable(s):
    """
    Parse VAR=VALUE for argparse; return tuple
    :return:  (var, value)
    """
    idx = s.index('=')
    if idx < 0:
        raise ArgumentTypeError("Expected '=' between VARIABLE and VALUE")
    return s[0:idx], s[idx + 1:]


def report_version():
    print("{0} version {1}".format(NAME, VERSION))
    print("(c) 2018 Magic Leap, Inc.")


class SplitCommandLineAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        if nargs is not None:
            raise ValueError("nargs not allowed")
        super(SplitCommandLineAction, self).__init__(option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, shlex.split(values))


class DeprecatedAction(argparse.Action):
    def __init__(self, option_strings, dest, message, **kwargs):
        super(DeprecatedAction, self).__init__(option_strings, dest, nargs=0, **kwargs)
        self._message = message

    def __call__(self, parser, namespace, values, option_string=None):
        warn("option {} is deprecated: {}".format(option_string, self._message))


def parse_arguments():
    parser = ArgumentParser(prog=NAME, description='''Build and package a Magic Leap project.''')

    parser.add_argument("--debug", dest="debug", action='store_true', help=argparse.SUPPRESS)
    parser.add_argument("--debug-make", dest="debug_make", action='store_true', help=argparse.SUPPRESS)
    parser.add_argument("--version", action="store_true", help="report version of mabu")
    parser.add_argument("-t", "--target", dest="spec", metavar='SPEC',
                        help="override the build target specification (use --help-target for details)")

    parser.add_argument("--path", dest='path', help="add search directories for *.option, *.config, and *.comp files; "
                                                    "separated by '" + os.pathsep + "'")
    parser.add_argument("-X", "--external-mabu-data", dest='external_mabu', action='store_true',
                        help="when MLSDK is overridden on the command line, find builtin *.config/*.component/*.option "
                             "files in the new MLSDK/tools/mabu/data instead of the running instance")

    build_group = parser.add_argument_group('Build options')

    build_group.add_argument("-q", "--quiet", dest="quiet", action='store_true',
                             help="quiet build (no information)")
    build_group.add_argument("-v", "--verbose", dest="verbose", action='store_true',
                             help="verbose build")
    build_group.add_argument("-j", "--jobs", dest="jobs", metavar='N', nargs='?', type=lambda x: x and str(int(x)) or '', const='',
                             help="invoke parallel make, using N processors (or as many as possible with no arg)")
    build_group.add_argument("-k", "--keep-going", dest="keep_going", action='store_true',
                             help="once build starts, keep going after errors occur (passed to 'make')")

    build_group.add_argument("-o", "--out", dest="outdir", metavar='OUTDIR',
                             help="override the output directory; for *{0} projects, the <SPEC> is still included; "
                             "for *{1} projects, this specifies the runtime layout".format(PROJ_EXT, PACKAGE_EXT))
    # NOTE: --out-project handled below in code

    build_group.add_argument("-c", "--clean", dest="clean", action='store_true',
                             help="only clean the project(s)")
    build_group.add_argument("-r", "--rebuild", dest="rebuild", action='store_true',
                             help="clean then build the project(s)")

    build_group.add_argument("-s", "--sign", dest="mlcert", metavar='FILE',
                             help="when packaging, sign with the provided the certificate and paired private key "
                                  "(FILE.cert and FILE.privkey -- use 'certgen' to create); "
                                  "this overrides any MLCERT environment variable or package setting")
    build_group.add_argument("--no-sign", dest="no_sign", action='store_true',
                             help=argparse.SUPPRESS)

    build_group.add_argument("--set-options", dest="set_options", metavar='OPTIONS',
                             help="prepend the given OPTIONS (separated with space, ':', or ';') "
                                  "to those from the projects; intended for IDE integration")

    build_group.add_argument("-m", "--manifest", dest="manifest_file", metavar='FILE',
                             help="when building a {}, override the path to the manifest.xml "
                                  "file, which is otherwise found in the project directory".format(PACKAGE_EXT))

    build_group.add_argument("--add-tail-data-args", dest="tail_data_args",
                             action=SplitCommandLineAction,
                             default=[],
                             help=argparse.SUPPRESS)

    adv_build_group = parser.add_argument_group('Unconventional build options (these are rarely used)')

    adv_build_group.add_argument("-a", "--all", dest="all", action='store_true',
                                 help="build and package the project(s) (default; this is usually what you want)")

    adv_build_group.add_argument("-n", "--dry-run", dest="dry", action='store_true',
                                 help="do nothing but parse project(s)")

    adv_build_group.add_argument("-b", "--build", dest="build", action='store_true',
                                 help="only build the .mabu project(s)")
    adv_build_group.add_argument("-l", "--layout", dest="layout", action='store_true',
                                 help="for {} projects, only lay out runtime data".format(PACKAGE_EXT))
    adv_build_group.add_argument("-p", "--package", dest="package", action='store_true',
                                 help="for {} projects, only create {} package".format(PACKAGE_EXT, BUILT_PACKAGE_EXT))
    adv_build_group.add_argument("--allow-unsigned", dest="allow_unsigned", action='store_true',
                                 help="for {} projects, do not fail if a MLCERT is not specified or detected "
                                 "(this is only for testing; the created {} cannot be installed!)".format(
                                     PACKAGE_EXT, BUILT_PACKAGE_EXT))
    adv_build_group.add_argument("--create-package", dest="create_package", action='store_true',
                                 help="for {} projects, always create {} file, "
                                      "even for builds targeting the host".format(PACKAGE_EXT, BUILT_PACKAGE_EXT))

    adv_build_group.add_argument("--local", dest="local", action='store_true',
                                 help="only build the project(s) mentioned on the command line, "
                                      "not their dependencies (REFS)")
    adv_build_group.add_argument("--refs", dest="refs", action='store_true',
                                 help="only build the dependencies (REFS) referenced from the project(s) "
                                      "mentioned on the command line, not the outputs of the .mabu files themselves")

    adv_build_group.add_argument("--rescan", dest="rescan", action='store_true',
                                 help="if mabu is having trouble locating build tools after installing or removing "
                                      "them, clears cached data in {}".format(Toolchain.get_cache_dir()))

    parser.add_argument(dest='project',
                        metavar='project [VAR=value]',
                        type=str,
                        nargs='?',
                        help='the project name (a path to a *{0} or *{1} file) along '
                             'with variable settings like MLSDK.'.format(PROJ_EXT, PACKAGE_EXT))

    print_group = parser.add_argument_group("Information options")

    print_group.add_argument("--help-target", "--help-spec", dest="help_spec",
                             help="print help about build target specifications (-t ...)",
                             action='store_true')
    print_group.add_argument("--help-projects", dest="help_projects",
                             help="print overview about mabu projects",
                             action='store_true')
    print_group.add_argument("--help-vars", dest="help_vars",
                             help="print overview of build variables",
                             action='store_true')
    print_group.add_argument("--help-commands", dest="help_commands",
                             help="print help about special commands",
                             action='store_true')

    print_group.add_argument("--print-target", "--print-spec", dest="print_spec",
                             help="print the full specification for --target=...", action='store_true')

    print_group.add_argument("--print-components", dest="print_components",
                             help="print the list of available components (for USES) given the current "
                                  "MLSDK, --path=..., and MABUPATH", action='store_true')
    print_group.add_argument("--print-options", dest="print_options",
                             help="print the list of available options (in OPTIONS) given the current "
                                  "MLSDK, --path=..., and MABUPATH", action='store_true')
    print_group.add_argument("--print-configs", dest="print_configs",
                             help="print the list of available configurations (for -t ...) given the current "
                                  "MLSDK, --path=..., and MABUPATH", action='store_true')

    print_group.add_argument("--print-tools", dest="print_tools",
                             help="print the set of tools used for --target=...", action='store_true')

    print_group.add_argument("--print-resolved", dest="print_resolved_projects",
                             help="print the project with all settings resolved from the "
                                  "build, components, and references", action='store_true')
    print_group.add_argument("--print-build-vars", dest="print_build_vars",
                             help="print the set of build variables derived from the project", action='store_true')

    print_group.add_argument("--print-outputs", dest="print_outputs",
                             help="print the set of build artifacts generated by the project and its subprojects "
                                  "(each build artifact is a pair of output file and output-relative file, "
                                  "separated by tab)",
                             action='store_true')
    print_group.add_argument("--print-output-directories", dest="print_output_directories",
                             help="print the output directories for each .mabu project", action="store_true")
    print_group.add_argument("--print-layout-directories", dest="print_layout_directories",
                             help="print the DATAS layout directories for each .package project", action="store_true")
    print_group.add_argument("--print-package-outputs", dest="print_package_outputs",
                             help="print the path to the "+BUILT_PACKAGE_EXT+" for each .package project", action="store_true")
    print_group.add_argument("--print-indexer-config", dest="print_indexer_config",
                             help="print the set of include directories and defines derived from the project", action='store_true')
    print_group.add_argument("--print-invoke-commands", dest="print_invoke_commands",
                             help="print the path to a script created for invoking the project output", action="store_true")
    print_group.add_argument("--print-invoke-vars", dest="print_invoke_vars",
                             help="print JSON for the variables and commands needed for invoking the project output", action="store_true")
    print_group.add_argument("--print-project-files", dest="print_project_files",
                             help="print all the project files involved in the build of a specific project", action="store_true")

    print_group.add_argument("--print-signing-data", dest="print_signing_data", metavar='INPUT_MPK',
                             help="print the signature data applied to files in the given previously-built {ext} "
                             "(this is a diagnostic tool; it can't tell what certificate was used to sign the package)"
                             .format(ext=BUILT_PACKAGE_EXT))

    invoke_group = parser.add_argument_group("Program invocation options")

    invoke_group.add_argument("--invoke", dest="invoke",
                              help="execute .mabu program projects for host or device", action="store_true")
    invoke_group.add_argument("-z", "--mlremote-adjust", dest="mlremote_adjust",
                              help="when invoking programs on host, adjusts the runtime library search path "
                                   "to launch under zero iteration with ML Remote (default); "
                                   "search order is: the environment's library search path ({}), "
                                   "ML Remote libraries, then library paths referenced by the project".format(
                                    get_runtime_library_path_var()),
                              action="store_true", default=True)
    invoke_group.add_argument("-Z", "--no-mlremote-adjust", dest="mlremote_adjust",
                              help="opposite of -z; search order is: "
                                   "the environment's library search path ({}) "
                                   "then library paths referenced by the project".format(
                                    get_runtime_library_path_var()),
                              action="store_false")
    invoke_group.add_argument("--invoke-args", dest="invoke_args",
                              help="arguments (quoted) to pass to an invoked program",
                              action="store")

    special_group = parser.add_argument_group("Special commands",
                                              "These commands bypass the build and packaging steps.")

    special_group.add_argument("--export-json", dest="export_json", metavar="FILE",
                               action="store", default="", type=str,
                               help="export the project and all related files to a JSON file")
    special_group.add_argument("--create-cmake-toolchain", dest="create_cmake_toolchain", metavar="FILE",
                               action="store", default="", type=str,
                               help="create a CMake toolchain for Lumin development into FILE, and exit")
    special_group.add_argument("--re-sign", "--resign", dest="resign_mpk", metavar='INPUT_MPK',
                               help="apply a different signing certificate to the given previously-built {ext}, "
                               "writing a copy with the alternate signatures applied, and exit".format(ext=BUILT_PACKAGE_EXT))

    # process options
    opts, left = parser.parse_known_args(sys.argv[1:])
    # print("left={0}".format(left))

    if opts.outdir and opts.outdir.startswith('ut') and '=' in opts.outdir:
        error('misspelled option; use --out... instead of -o' + opts.outdir)

    if opts.outdir:
        opts.outdir = os.path.abspath(opts.outdir)

    if opts.export_json:
        opts.export_json = os.path.abspath(opts.export_json)

    # check conflicts / redundancies
    _validate_options(opts)

    if opts.project:
        left.insert(0, opts.project)

    # split the target(s) and project(s) from variable=value settings
    vars = {}
    opts.projects = []
    opts.outdirs = {}

    for ent in left:
        if ent.startswith('--out-'):
            if '=' not in ent:
                error("Expected '{0}=<directory>'".format(ent))
            else:
                idx = ent.index('=')
                name = ent[6:idx]
                opts.outdirs[name] = os.path.abspath(ent[idx+1:])
        elif '=' in ent:
            idx = ent.index('=')
            vars[ent[0:idx]] = ent[idx+1:]
        elif not ent or ent[0] == '-':
            warn("Ignoring unknown argument '{0}'".format(ent))
        elif opts.resign_mpk:
            error("--resign does not take .mabu or .package arguments")
        else:
            proj = ent
            if not os.path.isfile(proj):
                proj += PROJ_EXT
                if not os.path.isfile(proj):
                    error("cannot find project '{0}' (should be a file ending in '{1}' or '{2}')".format(
                        ent, PROJ_EXT, PACKAGE_EXT))

            base_name, ext = os.path.splitext(os.path.split(proj)[1])
            if base_name == TOPLEVEL_MAKEFILE_NAME:
                error("Sorry, cannot use '{0}' as a project name: this is reserved for the build system.".format(base_name))
            elif ext != PROJ_EXT and ext != PACKAGE_EXT:
                error("mabu projects should have '{0}' or '{1}' as their extension, but got '{2}'".format(
                    PROJ_EXT, PACKAGE_EXT, base_name + ext))

            opts.projects.append(os.path.realpath(proj))

    return parser, opts, vars


def _validate_options(opts):
    # Reconcile the various build arguments and make sure there are no
    # obvious contradictions or redundancies.
    #
    # If no build option was specified, but any print option was,
    # assume it's a dry run.

    any_build_flags_provided = False
    opts.no_build = False
    opts.no_layout = False
    opts.no_package = False

    if opts.dry:
        opts.no_build = True
        opts.no_layout = True
        opts.no_package = True

    if opts.build:
        any_build_flags_provided = True
        if opts.all:
            warn("redundant to supply both --all and --build")
        elif opts.rebuild:
            warn("redundant to supply both --all and --rebuild")
        elif opts.resign_mpk:
            warn("re-signing is a standalone command")
        opts.no_layout = True
        opts.no_package = True

    if opts.layout:
        any_build_flags_provided = True
        if opts.all:
            warn("redundant to supply both --all and --layout")
        elif opts.resign_mpk:
            warn("re-signing is a standalone command")
        opts.no_build = True
        opts.no_layout = False
        opts.no_package = True

    if opts.package:
        any_build_flags_provided = True
        if opts.all:
            warn("redundant to supply both --all and --package")
        opts.no_build = True
        opts.no_layout = True
        opts.no_package = False

    if opts.resign_mpk:
        any_build_flags_provided = True
        if opts.build or opts.layout or opts.clean:  # tolerate opts.package
            error("cannot supply build flags with --re-sign")
        opts.no_build = True
        opts.no_layout = True
        opts.no_package = False

        if not opts.spec:
            opts.spec = 'release_device'
            opts.default_spec = True

        if not opts.mlcert:
            error("--re-sign requires the -s/--sign argument")

        opts.resign_mpk = os.path.abspath(opts.resign_mpk)

    if opts.clean:
        # clean is a 'build'
        any_build_flags_provided = True
        if opts.all:
            warn("redundant to supply both --all and --clean; use --rebuild")
            opts.rebuild = True
        elif opts.rebuild:
            warn("redundant to supply both --all and --rebuild")
        elif opts.resign_mpk:
            warn("re-signing is a standalone command")

        opts.no_layout = True
        opts.no_package = True

        if opts.build:
            warn("instead of supplying both --build and --clean, use --rebuild")
            opts.rebuild = True
            opts.no_layout = False
            opts.no_package = False

    if opts.rebuild:
        any_build_flags_provided = True
        if opts.resign_mpk:
            warn("re-signing is a standalone command")

    if not any_build_flags_provided:
        for attr in opts.__dict__:
            if attr.startswith('help_') or attr.startswith('print_'):
                if getattr(opts, attr):
                    debug("no build options provided, but print options were; assuming --dry")
                    opts.dry = True
                    break

    if opts.local == opts.refs:
        opts.build_scope = 0
    elif opts.local:
        opts.build_scope = -1
    else:
        opts.build_scope = 1


def process_arguments():
    from run_vars import runtime_values, MAKEARGS

    # handle environment variables
    _process_env_vars()

    # turn off built-in rules and variables,
    # and emit output line-by-line for parallel builds
    runtime_values[MAKEARGS] = ["-r", "-R", "-Oline"]

    # parse the arguments and validate
    parser, opts, vars = parse_arguments()
    set_debug(opts.debug)

    debug("SCRIPT_DIR={0}".format(SCRIPT_DIR))
    debug("BUILD_DIR={0}".format(BUILD_DIR))

    if opts.version:
        report_version()
        if not opts.projects and not opts.dry:
            sys.exit(0)

    if opts.rescan:
        cache_dir = Toolchain.get_cache_dir()
        if os.path.exists(cache_dir):
            diags.info("Removing cached tool information from {}".format(cache_dir))
            shutil.rmtree(cache_dir, ignore_errors=True)

    if opts.spec:
        runtime_values[SPEC] = opts.spec

    if opts.mlcert:
        runtime_values[MLCERT] = opts.mlcert
    if opts.path:
        # combine paths
        exist = runtime_values.get(MABUPATH)
        if exist:
            runtime_values[MABUPATH] = exist + os.pathsep + opts.path
        else:
            runtime_values[MABUPATH] = opts.path

    _process_variables(vars)

    _locate_make()

    _locate_mlsdk(opts.quiet)

    _locate_mltools()

    _locate_sign_file()

    build_env.setup_mabupath(opts.projects, opts.external_mabu)

    build_env.load_configs_options()

    _resolve_toolchain_and_spec(opts)

    add_convenience_variables()

    if build_env.platform().uses_packages():
        # always allow this for such devices (independent of --create-package)
        opts.create_package = True

    if opts.set_options:
        opts.set_options = re.split('[ :;]', opts.set_options)

    # pass these along
    makeargs = runtime_values[MAKEARGS]

    # arguments passed to make need to be escaped the same way
    # they are internally
    def makefile_var(name):
        val = runtime_values[name]
        return '{0}={1}'.format(name, Path.parse(val).makefile_str())

    makeargs.append(makefile_var('MLTOOLS'))
    makeargs.append(makefile_var('MLSDK'))
    if runtime_values.get(MLCERT):
        makeargs.append(makefile_var('MLCERT'))

    if opts.quiet:
        makeargs.append('QUIET=1')
    elif opts.verbose:
        makeargs.append('VERBOSE=1')

    if opts.jobs is not None:
        makeargs.append("-j" + opts.jobs)

    if opts.keep_going:
        makeargs.append("-k")

    if opts.debug_make:
        makeargs.append('--trace')
        if opts.debug:
            makeargs.append('-d')

    debug("MAKE={0}, MLTOOLS={1}, MLSDK={2}, SPEC={3}".format(
         runtime_values[MAKE], runtime_values[MLTOOLS], runtime_values[MLSDK], runtime_values[SPEC]))
    debug("MAKEARGS={0}".format(makeargs))
    debug("projects={0}".format(opts.projects))

    return opts


def _process_env_vars():
    # first, consume any environment variables
    def get_env(var):
        v = os.getenv(var, None)
        if v: runtime_values[var] = v

    # do not take MLSDK from the environment, since it can be wrong
    for env in [MLTOOLS, MABUPATH, SIGN_FILE, MLCERT, SIGNER, VCDIR]:
        get_env(env)


def _process_variables(vars):
    # analyze variables and send unknown ones through to makeargs
    for k, v in vars.items():
        runtime_values[k] = v

        if k in [MLSDK, MLTOOLS, SIGN_FILE, MABUPATH, VCDIR, MAKE] or k in TOOL_VARS:
            continue

        if HOSTOS == OS_WIN:
            # quote user vars so they don't get mangled by MinGW make

            # TODO: be smarter here, since this tramples "smart" escaping of
            # quoted macros and such, but that is a rarer case

            # The backslashes need to be killed early, since they may end up
            # in a variable used anywhere inside the Makefile

            s = v.replace('\\', '/')
        else:
            s = v

        runtime_values[MAKEARGS].append(k + '=' + s)


def add_convenience_variables():
    # expose variables for users
    runtime_values[HOST] = get_host_segment()

    _generate_extensions('PROGRAM', build_env.platform().format_program_file)
    _generate_extensions('SHARED', build_env.platform().format_shared_file)
    _generate_extensions('IMPLIB', build_env.platform().format_importlib_file)
    _generate_extensions('STATIC', build_env.platform().format_static_file)


def _generate_extensions(base, func):
    name = func('$$$')
    runtime_values[base + '_PREFIX'] = name[0:name.index('$$$')]
    runtime_values[base + '_EXT'] = name[name.index('$$$') + 3:]


def _locate_mlsdk(quiet):
    """
    locate MLSDK, if needed -- if not specified on the command line,
    it's where this script is (in SDK layouts), else, if in git,
    we expect to find it in the 'mlsdk_devtools' repo parallel to 'buildsystem'.
    """
    indicator = 'include'

    # This env var is not the preferred determinant since it can be
    # stale from old installations, but fall back to it if needed.
    mlsdk_env = os.getenv('MLSDK')

    path = runtime_values.get(MLSDK)

    if not path:
        # not specified on command line -- guess, assuming we live in $MLSDK/tools/mabu
        path = _locate_running_mlsdk(indicator)

        if not path:
            path = _locate_env_var_mlsdk(indicator)

        elif not quiet:
            # Check whether we picked an MLSDK that differs from a stale MLSDK env var
            env_mismatch = False
            try:
                if mlsdk_env and not os.path.samefile(path, mlsdk_env):
                    env_mismatch = True
            except IOError:
                # nonexistent env var (or local?!) path
                env_mismatch = True

            if env_mismatch:
                warn("Building against mabu's running location, '" + path + "'\n"
                     "instead of the environment variable 'MLSDK=" + mlsdk_env + "'.\n"
                     "(In order to build against a different release, pass MLSDK=" + mlsdk_env + "\n"
                     "on the mabu command line.)")
    else:
        # user overrode it, just use the path they want, assuming it exists
        if not os.path.isdir(path):
            error("No such directory exists for MLSDK=" + path)

    if not path:
        error("Cannot locate the Magic Leap Lumin SDK.\n"
              "Please pass MLSDK=<install path> to mabu or set the MLSDK environment variable.")
    elif not quiet and not os.path.isdir(os.path.join(path, indicator)):
        warn("The Lumin SDK seems incomplete: no subdirectory '" + indicator + "' exists inside '" + path + "'")

    # need a full path here...
    path = os.path.abspath(path)

    # canonicalize for MinGW-type tools
    path = path.replace('\\', '/')

    runtime_values[MLSDK] = path


def _locate_running_mlsdk(indicator):
    """
    Locate MLSDK from where mabu is rrunning.
    This is the home of this script (in SDK layouts), else, if in git,
    we expect to find it in the 'mlsdk' repo parallel to 'buildsystem'.
    """
    path = os.path.realpath(os.path.join(BUILD_DIR, '../../'))
    if os.path.isdir(os.path.join(path, indicator)):
        debug("Detected Lumin SDK relative to " + SCRIPT_DIR)
        return path

    if os.path.isdir(os.path.join(BUILD_DIR, '.git')):
        # for mabu developers, use local layout
        path = get_dev_sdk_path()
        if path and os.path.isdir(os.path.join(path, indicator)):
            debug("Assuming mlsdk mirror is checked out parallel to buildsystem")
            return path

    return None


def _locate_env_var_mlsdk(indicator):
    """
    Locate the SDK from where the env var says.
    """
    mlsdk_env = os.getenv('MLSDK')

    if mlsdk_env and os.path.isdir(mlsdk_env):
        return mlsdk_env

    return None


def _locate_mltools():
    """
    set MLTOOLS, if needed -- by default, it's located inside MLSDK
    """
    if not runtime_values.get(MLTOOLS):
        runtime_values[MLTOOLS] = os.path.join(runtime_values[MLSDK], 'tools', 'toolchains')

    indicator = 'bin'

    path = runtime_values.get(MLTOOLS)
    if not path:
        error("Please define MLTOOLS to point to the Lumin SDK (where '" + indicator + "' lives).")
    elif not os.path.isdir(path):
        error("Cannot locate MLTOOLS at " + path)
    elif not os.path.isdir(os.path.join(path, indicator)):
        # HACK for development
        if not os.path.isdir(os.path.join(path, '.git')):
            error("The location of MLTOOLS is incorrect: should contain '" + indicator + "', not found in " + path)


def _locate_sign_file():
    """
    set SIGN_FILE, if needed -- by default, it's located inside MLSDK
    """
    if not runtime_values.get(SIGN_FILE):
        runtime_values[SIGN_FILE] = os.path.join(runtime_values[MLSDK], 'tools', 'signer', 'sign-file')

    path = runtime_values.get(SIGN_FILE)

    # ensure this is an executable with the expected OS extension
    resolved = utils.which(path)
    if not resolved:
        error("The location of SIGN_FILE is incorrect: did not find " + path)


def _locate_make():
    # locate 'make', if needed
    if not runtime_values.get(MAKE):
        if HOSTOS == OS_WIN:
            runtime_values[MAKE] = os.path.join(MSYS_DIR, 'make')
        else:
            runtime_values[MAKE] = os.path.join(TOOLS_DIR, HOSTOS, 'make')


def _resolve_toolchain_and_spec(opts):
    # Resolve which toolchain to use.  This involves identifying
    # any conflicts in `-t ...` and `CC=...` or `CXX=...`
    # variables (or, allowing `-t` to influence discovery,
    # per Toolchain.__may_target_toolchain()).

    # By this point, build_env has already loaded and discovered
    # toolchains based on heuristic searches.

    # If the user specified an override, register that user tool.
    # If successful, this tweaks SPEC to reflect the actual
    # toolchain, so a default toolchain segment does not override it.
    if CC in runtime_values or CXX in runtime_values:
        err = build_env.discover_user_tool()
        if err:
            error(err)

    # Finalize the spec (also detecting a stock toolchain based on SPEC
    # if not explicitly discovered above)
    _define_spec(opts.quiet, opts.verbose)

    # Then finalize the toolchain by detecting actual runtime executables
    tc = build_env.toolchain()
    if tc:
        err = tc.detect()
        if err:
            error(err)


def _define_spec(quiet, verbose):
    # set up SPEC into a BuildSpec, and canonicalize
    try:
        user_spec = runtime_values.get(SPEC)
        build_env.set_spec(user_spec or '')
        build_env.spec().check_valid()
        final_spec = str(build_env.spec())
        runtime_values[SPEC] = final_spec
        if not quiet:
            if not user_spec:
                info("using default target '-t {0}'".format(final_spec), is_stderr=True)
            else:
                info("using build target '{0}'".format(final_spec), is_stderr=True)
    except BuildException as e:
        error(str(e))
