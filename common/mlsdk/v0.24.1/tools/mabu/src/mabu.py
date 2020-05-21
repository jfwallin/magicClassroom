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

"""
Magic Leap project builder frontend.
"""
import subprocess
import sys
import textwrap
import threading
import os
import traceback

import diags
import build_env
from args import process_arguments
from create_cmake_toolchain import create_cmake_toolchain
from printers import print_tools, print_components, print_options, print_configs, print_resolved_projects, \
    print_build_vars, print_project_files, print_indexer_config, print_invoke_commands, print_outputs, \
    print_output_directories, print_layout_directories, print_package_outputs, print_signing_data, print_invoke_vars
from build_model import BuildException, dump_build_objects
from config import NAME
from export_json import export_json
from makegen import MakeGen
from package import layout_or_package_project, clean_package_project, resign_package, get_resigned_package_output
from factory import load_projects_and_dependencies
from utils import get_make_env_block, user_relpath, format_command_line


def make_projects(opts, env):
    if not env.toolchain():
        diags.error("failed to locate any tools for {} -- verify the installation and PATH".format(env.spec()))

    err = env.toolchain().detect()
    if err:
        diags.error(err, die=False)

    mg = MakeGen(env.projects(), env.components(), opts.build_scope)
    mg.generate(env.user_projects())
    main_proj = mg.write()

    if err:
        sys.exit(1)

    if not build_env.platform().is_buildable():
        diags.error("cannot build for '{0}' on this host; use a different '-t' argument".format(build_env.spec().platform))
        return

    if not opts.dry and main_proj:
        run_make(opts, main_proj)


def _run_make(opts, main_proj, rule):
    from run_vars import runtime_values, MAKE, MAKEARGS

    args = [runtime_values[MAKE], rule, "-f", main_proj.value()]

    if opts.quiet:
        args.append("--quiet")

    args += runtime_values[MAKEARGS]

    msg = format_command_line(args)
    if opts.verbose:
        print(msg)
    else:
        diags.debug("Running " + msg)

    env = get_make_env_block()

    cmp = subprocess.run(args, env=env, universal_newlines=True)
    return cmp


def run_make(opts, main_proj):
    rule = 'all'
    if opts.rebuild:
        _run_make(opts, main_proj, 'clean')
        rule = opts.no_package and 'build' or 'all'
    elif opts.clean:
        rule = 'clean'
    elif opts.no_package:
        rule = 'build'
    elif opts.no_build:
        return

    cmp = _run_make(opts, main_proj, rule)
    if cmp.returncode:
        sys.exit(1)


def usage():
    print("""
usage: {0} [-t TARGET] [-OPTIONS] PROJECT... [VARIABLE=value...]

Builds and packages Magic Leap projects.

For .package projects, the output is an .mpk for Lumin targets and a 
data layout for host targets.  For .mabu projects, the output is a
set of build artifacts (programs, libraries).  (Pass the .package if
you are unsure.) 

Run {0} --help-target for information about build targets.

Run {0} --help-projects for information about projects.

Run {0} --help-vars for information about variables.

Run {0} --help-commands for information about controlling mabu.

Run {0} --help for full information on available options.
""".format(NAME))


def help_projects():
    print("""
{0} is driven by project files.
 
SYNTAX:
  Projects are simple text files with 'SETTING=value [value2...]' lines.
    
  Lines may be continued using '\\' as the final character on a line.
  
  Comments are preceded with '#' and may appear inside continued lines
  (terminating at newline).
  
TOP-LEVEL PROJECTS:
  *.mabu files define build artifacts (applications, shared/static libraries).
  
  *.package files corral a group of *.mabu files.  For device builds, 
  build artifacts are incorporated from *.mabu file outputs.  For all builds, 
  files from DATAS are also contributed to a data layout that is accessible 
  both on host and device.  (DATAS may also bring in executables if they
  are built externally.)
    
  Projects reference each other by the REFS setting.  Usually you want
  to build *.package for host or device.
  
AUXILLIARY PROJECTS:
  *.comp files define components, which are like mixins to other projects.
  The USES setting adds these to a project (see --print-components).
  
  *.option files define compiler-independent abstractions for cross-platform
  builds, which are referenced by the OPTIONS setting.  For example, 
  'optimize/on' applies appropriate flags for gcc, clang, and Visual Studio
  (see --print-options).
""".format(NAME))


def help_vars():
    print("""
{0} provides some $(VARIABLES) which projects can use.  Additional variables 
can also be provided on the command line.

For example:

  mabu myproject EXTERNAL_SOURCES=.../path/to/external/sources

will fill in a variable in myproject.mabu setting that can be used like this:  
  
  SRCS=$(EXTERNAL_SOURCES)/entry.cpp
  INCS=$(EXTERNAL_SOURCES)

NOTE: mabu does not consume or pass through environment variables by default.

BUILTIN VARIABLES:
  MLSDK: path to the detected Lumin SDK, which is discovered relative
      to mabu's location if not overridden on the command line.
         
  MLTOOLS: path to toolchains.  Defaults to $(MLSDK)/tools/toolchains.
  
  MLCERT: path to a developer signing certificate (.cert or .privkey),
      generated by 'certgen', used to sign device packages.

  SPEC: the expanded build target specification expanded from '-t'
  
  HOST: the current running host OS: win64, osx, linux64
  
  <KIND>_PREFIX, <KIND>_EXT: target-specific filename prefix and extension
      for build artifacts.  KIND is one of PROGRAM, STATIC, SHARED, IMPLIB.
      ("static_shared" projects are considered SHARED.)
""".format(NAME))


def help_spec():
    print("""
mabu can build for Lumin or your host.  Use "-t TARGET" to choose.
The TARGET is a string consisting of configuration, platform, toolchain, 
and architecture segments in the form SEGMENT[_SEGMENT[_SEGMENT[_SEGMENT]]].

Typically you will only provide the platform and configuration in TARGET.
mabu will fill in the rest with appropriate defaults.  The fully expanded
string is called the "build target specification".

Default: debug_host (also try: release_device)

Use "{0} --print-target -t TARGET" to see the expanded specification.
""".format(NAME))

    print("Available segments:")
    isatty = False
    try:
        # MSVC detection can be veeeery slow... provide a progress message, unless dumping to a file
        if os.isatty(sys.stdout.fileno()):
            print("(detecting tools ...)", end='')
            sys.stdout.flush()
            isatty = True
    except OSError:
        pass

    target_info = textwrap.indent(dump_build_objects(), prefix='    ')
    if isatty:
        # clear progress message
        print('\r', end='')
    print(target_info)


def help_commands():
    print("""
The typical workflow is to incrementally build and package the projects
passed on the command line.  One of the following limits the scope of work:

  -b | --build      build the project and its dependencies
  -c | --clean      clean the project and its dependencies
  -r | --rebuild    clean then build the project and its dependencies
  -l | --layout     for *.package projects, create the data layout
  -p | --package    for *.package projects, build the package

Alternately, mabu can provide information about the build toolchain and 
the structure of your projects or perform other miscellaneous operations.

Some information commands:

* report which build tools mabu will use for a given target (--print-tools)   
* report which options are available in OPTIONS settings (--print-options)   
* report which components are available for USES (--print-components)   
* report the fully-parsed structure of projects (--print-resolved)
* report the build variables passed on to "make" (--print-build-vars)

Other top-level commands:

* take an existing .mpk and re-sign it with a different certificate
(usually to prepare for submission to the ML Developer Portal, by a 
party who cannot regenerate the .mpk from scratch) (--re-sign)
* export the structure of a set of projects to JSON for use by IDE
integrations (--export-json)
""".format(NAME))


def main():
    # Save a copy of the original environment path since mabu may overwrite it with its own set of paths.
    original_path = os.environ["PATH"]

    # process command line arguments and variable=value settings
    opts = process_arguments()

    if opts.help_spec:
        help_spec()

    if opts.help_projects:
        help_projects()

    if opts.help_vars:
        help_vars()

    if opts.help_commands:
        help_commands()

    if opts.print_spec:
        # print the spec
        print(build_env.spec())

    if opts.print_tools:
        # print the tools
        print_tools()

    # below this line, we need to be sure a user-provided configuration is valid
    config = build_env.find_config(build_env.spec().config)
    if not config:
        # fatal error
        diags.error("no configuration registered for '{0}'".format(build_env.spec().config))

    if opts.print_components:
        print_components()

    if opts.print_options:
        print_options(opts)

    if opts.print_configs:
        print_configs()

    if opts.print_signing_data:
        print_signing_data(opts)

    if opts.resign_mpk:
        if not build_env.platform().uses_packages():
            diags.error("--re-sign only makes sense for device targets")
        resign_package(opts, get_resigned_package_output(opts.resign_mpk))
        sys.exit(0)

    if opts.create_cmake_toolchain:
        if build_env.platform().is_host():
            diags.error("--create-cmake-toolchain only applies to device targets")
        create_cmake_toolchain(opts.quiet, opts.verbose, opts.create_cmake_toolchain)
        sys.exit(0)

    # read user projects and any dependencies
    if not opts.projects:
        if not opts.version and (not opts.dry or opts.print_build_vars):
            usage()
        sys.exit(0)

    if opts.outdir:
        for proj in opts.projects:
            base = os.path.splitext(os.path.basename(proj))[0]
            if base not in opts.outdirs:
                opts.outdirs[base] = opts.outdir

    load_projects_and_dependencies(opts.projects, opts.outdirs, opts.set_options, config)

    if opts.manifest_file:
        for pkg in build_env.packages().values():
            pkg.manifest_file = os.path.abspath(opts.manifest_file)

    if opts.print_resolved_projects:
        print_resolved_projects(opts.projects)

    if opts.print_build_vars:
        print_build_vars(opts.build_scope)

    if opts.print_project_files:
        # Print the files in the build, then exit.
        print_project_files(opts.build_scope)
        sys.exit(0)

    if opts.print_indexer_config:
        # Detect the toolchain since it needs to be setup in order to extract the command search paths.
        err = build_env.toolchain().detect()
        if err:
            diags.error(err, die=False)
        # Print the config, then exit.
        print_indexer_config(opts.build_scope)
        sys.exit(0)

    if opts.export_json:
        # The toolchain is needed for excluding tool vars.
        err = build_env.toolchain().detect()
        if err:
            diags.error(err, die=False)
        export_json(opts.build_scope, opts.export_json, opts.projects)
        sys.exit(0)

    invoke_file_paths = []
    if opts.print_invoke_commands or opts.print_invoke_vars or opts.invoke:
        err = build_env.toolchain().detect()
        if err:
            diags.error(err, die=False)

        import invoke
        invoke_models = invoke.generate_invoke_models(opts)

        if opts.print_invoke_vars:
            print_invoke_vars(opts, invoke_models)

        if opts.print_invoke_commands or opts.invoke:
            invoke_file_paths = [model.write_script() for model in invoke_models]

            if opts.print_invoke_commands:
                print_invoke_commands(invoke_file_paths)

        if not opts.invoke:
            sys.exit(0)

    if not opts.dry:
        make_projects(opts, build_env)

        if (opts.clean or opts.rebuild) and opts.build_scope <= 0:
            for proj in build_env.user_projects():
                clean_package_project(opts, proj)

        if (not opts.no_layout or not opts.no_package) and opts.build_scope <= 0:
            for proj in build_env.user_projects():
                try:
                    layout_or_package_project(opts, proj)
                except BuildException as e:
                    diags.error(str(e))
                except Exception as e:
                    traceback.print_exc()
                    diags.error(str(e))

    if opts.print_outputs:
        # print the source -> dest file mappings
        print_outputs(opts.create_package, opts.build_scope)
    elif opts.print_output_directories:
        print_output_directories(opts.build_scope)
    elif opts.print_layout_directories:
        print_layout_directories(opts.build_scope)
    elif opts.print_package_outputs:
        if not (build_env.platform().uses_packages() or opts.create_package):
            diags.warn('this target does not create package files (without --create-package)')
        else:
            print_package_outputs(opts.build_scope)
    elif not opts.quiet and opts.build_scope <= 0:
        outdir = None
        multiple = False
        for proj in build_env.user_projects():
            out_rel = user_relpath(proj.output_directory, os.getcwd())
            if not outdir:
                outdir = out_rel
            elif outdir != out_rel:
                multiple = True
                break
        if not multiple and len(build_env.user_projects()) > 1:
            diags.info("all output in '{0}'".format(outdir))
        else:
            for proj in build_env.user_projects():
                out_rel = user_relpath(proj.output_directory, os.getcwd())
                diags.info("'{0}' output in '{1}'".format(proj.name, out_rel))

    if opts.invoke and invoke_file_paths:
        # Restore the original path so any changes to it by the user aren't lost.
        os.environ["PATH"] = original_path

        if not opts.quiet:
            print("-" * 78)

        # Launch each invokable project found.
        for file_path in invoke_file_paths:
            print("[Invoking {}]".format(file_path))

            # Catch the output in real time and print it as it becomes available.
            # Doing this is necessary to keep the launched process from printing
            # its output before python is able to flush its own output buffers.
            # For instance, this problem because apparent when running mabu with
            # "--invoke" from a Java application.
            def stream_output(in_pipe, out_pipe):
                while True:
                    # Block until a line of text is ready to be consumed from the program.
                    line = in_pipe.readline()

                    if not line:
                        break

                    line = line.decode("utf-8", "replace")

                    out_pipe.write(line)

            proc = subprocess.Popen([file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            output_thread = threading.Thread(target=stream_output, args=(proc.stdout, sys.stdout))
            error_thread = threading.Thread(target=stream_output, args=(proc.stderr, sys.stderr))

            # Start capturing output.
            output_thread.start()
            error_thread.start()

            # Run the program.
            proc.wait()

            # Wait for the output threads to complete.
            output_thread.join()
            error_thread.join()

            if proc.returncode != 0:
                raise MabuInvokeError(proc.returncode)


class MabuInvokeError(Exception):
    def __init__(self, return_code):
        Exception.__init__(self, "Invoke command returned non-zero result.")
        self.return_code = return_code


if __name__ == '__main__':
    try:
        main()
    except MabuInvokeError as exc:
        sys.exit(exc.return_code)
    except KeyboardInterrupt as exc:
        if diags.is_debug():
            traceback.print_exc()
        print("*** Interrupted ***", file=sys.stderr)
        sys.exit(5)
    except Exception as exc:
        if True or diags.is_debug():
            traceback.print_exc()
        else:
            print("fatal error encountered: run with --debug for more details: " + str(exc), file=sys.stderr)
        sys.exit(1)

