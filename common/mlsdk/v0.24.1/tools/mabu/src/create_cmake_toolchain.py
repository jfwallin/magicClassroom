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
import copy
import sys
import subprocess
import os

import re

import build_env
import diags
import kinds
import utils
from build_model import BuildSpec
from config import DATA_DIR, OS_WIN, HOSTOS
from run_vars import MLSDK, runtime_values, MAKE

TEMPLATES_DIR = os.path.join(DATA_DIR, 'templates')


def _run_get_output(verbose, args, config=None):
    my_args = list(args)

    # replace generic 'mabu' with another invocation
    if my_args[0] == 'mabu':
        import mabu

        mabu_path = []

        def add_user_path(path):
            mabu_path.append(path)

        build_env.iterate_search_path(add_user_path, lambda: False, lambda _: False)

        # find base.comp last
        mabu_path.append(os.path.join(TEMPLATES_DIR, 'default'))

        # modify the build spec per the supplied config
        if config:
            my_spec = BuildSpec(str(build_env.spec()), config=config)
        else:
            my_spec = build_env.spec()

        my_args[0:1] = [
            sys.executable,
            mabu.__file__,
            '-t',
            str(my_spec),
            '--path',
            os.pathsep.join(mabu_path)
        ]

    # run, but stifle embedded prints, only printing on error
    myenv = os.environ.copy()
    myenv['PYTHONPATH'] = os.pathsep.join(sys.path)

    cmp = subprocess.run(my_args,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         universal_newlines=True, env=myenv)
    if cmp.returncode != 0:
        diags.error("failed to invoke mabu subprocess:\n{}{}".format(
            cmp.stdout, cmp.stderr
        ))
        return None

    if verbose:
        diags.info("{} yielded:\n{}".format(utils.format_command_line(my_args), cmp.stdout))

    return cmp.stdout


def _parse_assignments(text):
    """
    Read a set of VAR=VALUE lines and create a map.
    :param text:
    :rtype: dict[str,str]
    """
    vars = {}
    for line in text.split('\n'):
        line = line.strip()
        try:
            idx = line.index('=')
            var = line[0:idx]
            value = line[idx+1:]

            # remove some mabu escapes
            value = value.replace("'", "")
            value = value.replace("$$", "$")

            vars[var] = value
        except ValueError:
            pass

    return vars


def _canonicalize(path):
    c = os.path.realpath(path)
    if HOSTOS == OS_WIN:
        # realpath doesn't correct capitalization.
        # for our purposes, this is safe.
        c = c.lower()

        # cmake strings interpret slashes, so, forward-slasherize these
        c = c.replace('\\', '/')
    return c


def _fetch_toolchains(verbose, cmake_vars):
    toolchains = _run_get_output(verbose, ['mabu', '-q', '--print-tools'])
    if verbose:
        diags.info("Toolchains:\n{}".format(toolchains))

    tool_vars = _parse_assignments(toolchains)

    if 'CC' not in tool_vars:
        diags.error("Did not detect CC in mabu --print-tools (run with '-v' or '--debug')")
    if 'CXX' not in tool_vars:
        diags.error("Did not detect CXX in mabu --print-tools (run with '-v' or '--debug')")

    cmake_vars['CMAKE_MAKE_PROGRAM'] = _canonicalize(runtime_values[MAKE])
    cmake_vars['CMAKE_C_COMPILER'] = _canonicalize(tool_vars['CC'])
    cmake_vars['CMAKE_CXX_COMPILER'] = _canonicalize(tool_vars['CXX'])
    cmake_vars['CMAKE_AR'] = _canonicalize(tool_vars.get('AR', 'ar'))
    cmake_vars['CMAKE_RANLIB'] = _canonicalize(tool_vars.get('RANLIB', 'ranlib'))

    bin_dir, _ = os.path.split(cmake_vars['CMAKE_C_COMPILER'])
    cmake_vars['TOOLS_BIN'] = bin_dir


def _fetch_settings(verbose, cmake_type, mabu_kind, cmake_vars):
    project = os.path.join(TEMPLATES_DIR, 'empty_{}.mabu'.format(mabu_kind))
    settings = _run_get_output(verbose,
                               ['mabu', '-q', '--print-build-vars', project],
                               config='blank')
    if verbose:
        diags.info("Settings for {}:\n{}".format(mabu_kind, settings))

    if verbose:
        diags.info("Release settings for {}:\n{}".format(mabu_kind, settings))

    prj_vars = _parse_assignments(settings)

    if mabu_kind == kinds.program.NAME:
        # assume the program flags apply to any build (let cmake figure out '-fPIC' and such)

        # record where we found these settings
        base = build_env.find_component_on_path(None, 'base') or os.path.join(TEMPLATES_DIR, 'default', 'base.comp')
        cmake_vars['MABU_BASE'] = base.replace('\\', '/')
        cmake_vars['MABU_SETTINGS'] = settings.replace('\r', '').replace('\n', '\n#  ')
        cmake_vars['MABU_TEMPLATE'] = project

        cppflags = ''
        incs = []
        if 'CPPFLAGS' in prj_vars:
            full_cppflags = prj_vars['CPPFLAGS']
            cppflags = []

            # split out include paths
            cppflag_pieces = full_cppflags.split(' ')
            for piece in cppflag_pieces:
                if piece[0:2].lower() == '-i':
                    incs.append(piece[2:].replace('\\', '/'))
                else:
                    cppflags.append(piece)

        non_inc_cppflags = ' '.join(cppflags)
        inc_cppflags = ' '.join('"' + inc + '"' for inc in incs)

        if 'CFLAGS' in prj_vars:
            _fetch_sysroot(non_inc_cppflags + ' ' + prj_vars['CFLAGS'], cmake_vars, 'CMAKE_C_FLAGS')
        if 'CXXFLAGS' in prj_vars:
            _fetch_sysroot(non_inc_cppflags + ' ' + prj_vars['CXXFLAGS'], cmake_vars, 'CMAKE_CXX_FLAGS')

        cmake_vars['INCLUDE_DIRECTORIES'] = inc_cppflags

    ld_flags = ''
    if 'LIBS' in prj_vars:
        ld_flags = prj_vars['LIBS']

    if 'LDFLAGS' in prj_vars:
        ld_flags += ' ' + prj_vars['LDFLAGS']

    # extract library paths
    if mabu_kind == kinds.program.NAME:
        lib_paths = []
        for ent in ld_flags.split(' '):
            if ent[0:2] == '-L':
                lib_paths.append('"' + ent[2:] + '"')
        cmake_vars['CMAKE_LIBRARY_PATH'] = ' '.join(lib_paths)

    cmake_vars['CMAKE_{}_LINKER_FLAGS'.format(cmake_type)] = ld_flags


def _fetch_sysroot(full_flags, cmake_vars, flag_variable_name):
    """ Splits the --sysroot from the other CFLAGS and CXXFlags, and avoids having multiple conflicting values for --sysroot defined """
    flags = []

    # split --sysroot out from other flags
    flag_pieces = full_flags.split(' ')
    for piece in flag_pieces:
        if piece.startswith('--sysroot='):
            sysroot = piece[len('--sysroot='):].replace('\\', '/')
            if 'CMAKE_SYSROOT' in cmake_vars and cmake_vars['CMAKE_SYSROOT'] != sysroot:
                # This both checks that CFLAGS and CXXFLAGS have the same sysroot, and that each of those variables only has sysroot defined once.
                diags.error('Multiple conflicting --sysroot flags are defined. {} conflicts with {}'.format(cmake_vars['CMAKE_SYSROOT'], sysroot))
            else:
                cmake_vars['CMAKE_SYSROOT'] = sysroot

            # cmake ignores CMAKE_SYSROOT for some reason; keep passing it in CMAKE_..._FLAGS below

        flags.append(piece)

        cmake_vars[flag_variable_name] = ' '.join(flags)


def create_cmake_toolchain(quiet, verbose, path):
    """
    Create a cmake toolchain using toolchains and options detected by mabu.
    :param quiet: whether to suppress info
    :param verbose: whether to dump status info from mabu sub-invocations
    :param path: output path
    """

    templ_path = os.path.join(DATA_DIR, 'templates', 'cmake', 'mlsdk.toolchain.cmake')
    try:
        with open(templ_path, 'r') as f:
            template_contents = f.read()
    except OSError as e:
        diags.error("failed to read template from {}, check installation ({})".format(templ_path, str(e)))

    cmake_vars = {}

    cmake_vars['CMAKE_SYSTEM_PROCESSOR'] = 'aarch64'

    _fetch_toolchains(verbose, cmake_vars)
    _fetch_settings(verbose, 'EXE', 'program', cmake_vars)
    _fetch_settings(verbose, 'SHARED', 'shared', cmake_vars)
    _fetch_settings(verbose, 'STATIC', 'static', cmake_vars)

    contents = template_contents
    for key, val in cmake_vars.items():
        contents = contents.replace('@@' + key + '@@', val)

    # replace $(MLSDK) refs
    contents = contents.replace('$(MLSDK)', '${LUMIN_SDK}')

    # try to replace stray references to the MLSDK path
    # (embedded in the tool paths, for example) with the variable
    mlsdk = _canonicalize(runtime_values[MLSDK])
    contents = contents.replace(mlsdk, r'${LUMIN_SDK}')

    # then replace @@MLSDK@@ last, with the one reference to the path
    contents = contents.replace('@@MLSDK@@', runtime_values[MLSDK])

    with open(path, 'w') as f:
        f.write(contents)

    if not quiet:
        diags.info("Generated a cmake toolchain file to {}.\n"
                   "This toolchain is entirely independent of mabu, but, depends on the current SDK location;\n"
                   "override the MLSDK cmake variable as needed.".format(path))
        diags.info("""Example usage:\n\t{} CMakeCache.txt\n\tcmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE={} [-DMLSDK=...] ..."""
                   .format(HOSTOS == OS_WIN and "del" or "rm", path))

