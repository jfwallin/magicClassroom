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

import json
import os
import shlex
import stat
from json import JSONDecodeError

import build_env
import diags
import platforms
import project_vars
from build_model import Path
from config import HOSTOS, OS_WIN, OS_OSX, OS_LINUX, get_host_segment, get_runtime_library_path_var
from makegen import MakeGen
from run_vars import runtime_values, MLSDK, HOST
from sdk_shim_parser import SdkShimParser
from utils import format_command_line, remove_duplicates


def fetch_mlremote_lib_paths():
    host = get_host_segment()

    sdk_manifest_path = os.path.join(runtime_values[MLSDK], ".metadata", "sdk.manifest")
    try:
        with open(sdk_manifest_path, "rt", encoding="UTF-8", errors="replace") as f:
            sdk_manifest = json.load(f)

            sdk_version = sdk_manifest.get("version")

    except (OSError, JSONDecodeError) as e:
        diags.warn("Failed to parse {}\nPlease verify your setup and installation."
                   "\n({})".format(sdk_manifest_path, e))
        return []

    if not sdk_version:
        diags.warn("Did not detect \"version\" attribute in {}.\nPlease verify your setup and installation."
                   .format(sdk_manifest_path))
        sdk_version = '0.20.0'  # we don't depend on it currently

    sdk_shim_path = os.path.join(runtime_values[MLSDK], ".metadata", "sdk_shim_discovery.txt")
    try:
        with open(sdk_shim_path, "rt", encoding="UTF-8", errors="replace") as f:
            sdk_shim_text = f.read()

        shim_parser = SdkShimParser(sdk_shim_text, runtime_values[MLSDK], sdk_version, host)

    except OSError as e:
        diags.warn("Failed to parse {}, needed for correct launching of host apps under ML Remote."
                   "\nPlease verify your setup and installation (if you pass MLSDK=..., it should point to "
                   "a version for 0.18 or newer, or, pass '-Z' to launch without ML Remote support)."
                   "\n({})".format(sdk_shim_path, e))
        return []

    zi_paths = shim_parser.expand('ZI_SHIM_PATH_' + host)
    return zi_paths


class InvokeModel(object):
    """
    This represents the information needed to invoke a particular project.
    """
    def __init__(self, opts, package, project):
        self._opts = opts

        self._package = package
        """ @type: projects.Project """
        self._project = project
        """ @type: projects.Project """

        # target for scripts is the same as project output dir
        self._output_directory = os.path.normpath(self._project.output_directory)

        self._mlsdk = runtime_values[MLSDK]

        # get some platform-specific stuff out of the way...
        platform = build_env.platform()
        self._is_device_build = (platform.name == platforms.lumin.NAME)

        self._executable_name = self._project.output_filename()

    @property
    def project_path(self):
        return self._project.path

    def generate_json(self):
        model = {
            'project_path': self.project_path,
            'executable_path': self._executable_path,
            'executable_command': self._exe_command,
        }

        self._add_json_vars(model)
        return model

    def _make_commands_win(self):
        cmds = []

        cmds.extend([
            "@echo off",
            "setlocal",
            "pushd \"{}\"".format(self._launch_directory),
        ])

        self._make_specific_commands_win(cmds)

        cmds.extend([
            "popd",
            "exit /b %__RETCODE__%",
        ])

        return cmds

    def _make_commands_posix(self):
        """
        Create bash commands for Unix-like hosts.
        """
        cmds = []
        cmds.extend([
            "#!/usr/bin/env bash",
            "__OLDPWD__=$PWD",
            "cd \"{}\"".format(self._launch_directory),
        ])

        self._make_specific_commands_posix(cmds)

        cmds.extend([
            "cd $__OLDPWD__",
            "exit $__RETCODE__",
        ])

        return cmds

    def write_script(self):
        """
        Write a shell script (.bat/.sh) that can do the invocation work
        :return: path
        """
        filename = "invoke-{}".format(self._package.package_name if self._package else self._project.name)

        if HOSTOS == OS_WIN:
            filename += ".bat"
            cmds = self._make_commands_win()
        else:
            filename += ".sh"
            cmds = self._make_commands_posix()

        # Create the output directory if it doesn't exist.
        if not os.access(self._output_directory, os.F_OK):
            os.makedirs(self._output_directory)

        # Write it out
        out_file_path = os.path.normpath(os.path.join(self._project.output_directory, filename))
        with open(out_file_path, "w") as f:
            f.write("\n".join(cmds))

        # Get the invoke file permissions.
        st = os.stat(out_file_path)

        # Make the invoke file executable.
        os.chmod(out_file_path, st.st_mode | stat.S_IEXEC)

        return out_file_path


class InvokeModelDevice(InvokeModel):
    """
    This represents the information needed to invoke on device.
    """
    def __init__(self, opts, package, project, environ):
        InvokeModel.__init__(self, opts, package, project)

        package_filename = "{}.mpk".format(self._project.package_name)
        self._package_path = os.path.join(self._output_directory, package_filename)
        self._installed_package_name = self._project.manifest.package_name
        self._envsetup_basename_path = os.path.abspath(os.path.join(self._mlsdk, "envsetup"))
        self._device_install_cmd = "mldb install -u \"{}\"".format(self._package_path)

        self._launch_directory = "."
        self._executable_path = self._executable_name

        self._exe_command = ["mldb", "launch", "-f", self._installed_package_name]
        if opts.invoke_args:
            self._exe_command += ["-i", opts.invoke_args]  # one string

    def _add_json_vars(self, model):
        model['install_command'] = self._device_install_cmd
        model['package_path'] = self._package_path

    def _make_specific_commands_win(self, cmds):
        cmds.extend([
            "call \"{}.bat\"".format(self._envsetup_basename_path),
            self._device_install_cmd,
            "set __RETCODE__=%ERRORLEVEL%",
            "if %__RETCODE__% EQU 0 (",
            "\t{}".format(format_command_line(self._exe_command)),
            "\tset __RETCODE__=%ERRORLEVEL%",
            ")",
        ])

    def _make_specific_commands_posix(self, cmds):
        cmds.extend([
            "source \"{}.sh\"".format(self._envsetup_basename_path),
            self._device_install_cmd,
            "__RETCODE__=$?",
            "if [ $__RETCODE__ -eq 0 ]; then",
            "\t{}".format(format_command_line(self._exe_command)),
            "\t__RETCODE__=$?",
            "fi",
        ])


class InvokeModelHost(InvokeModel):
    """
    This represents the information needed to invoke on host.
    """
    def __init__(self, opts, package, project, environ=os.environ):
        InvokeModel.__init__(self, opts, package, project)

        self._launch_directory = self._package.package_layout_path.resolve() if self._package else self._output_directory
        self._executable_path = os.path.join(self._output_directory, self._executable_name)

        self._exe_command = [self._executable_path] + \
                            (self._opts.invoke_args and shlex.split(self._opts.invoke_args) or [])

        self._source_lib_path_var = self._lib_path_var = get_runtime_library_path_var()

        if HOSTOS == OS_OSX:
            # Favor ML_LIBRARY_PATH over an empty/missing DYLD_LIBRARY_PATH,
            # which macOS deletes for our ever-loving security
            # (see https://apple.stackexchange.com/questions/212945/unable-to-set-dyld-fallback-library-path-in-shell-on-osx-10-11-1).
            # (mlvdsetup.sh sets up ML_LIBRARY_PATH)
            if not environ.get(self._lib_path_var) and environ.get('ML_LIBRARY_PATH'):
                if not self._opts.quiet:
                    diags.info("macOS deleted ${}, using $ML_LIBRARY_PATH instead".format(self._lib_path_var))
                self._source_lib_path_var = 'ML_LIBRARY_PATH'

        project_lib_paths = self._project.combined.get(project_vars.LIBPATHS, []) if self._project.combined else []

        dfs = self._project.dependency_list(build_env.projects(), build_env.components())

        ref_lib_paths = [Path(dep_proj.output_directory) for dep_proj in dfs if
                         dep_proj.is_program() or dep_proj.is_shared()]

        all_paths = [path.resolve() for path in project_lib_paths + ref_lib_paths]

        if not self._is_device_build:
            # get the user's current library paths
            env_lib_val = environ.get(self._source_lib_path_var, '')
            if env_lib_val:
                env_lib_paths = env_lib_val.split(os.pathsep)
            else:
                env_lib_paths = []

            # and get any ML Remote / ZI library paths
            if self._opts.mlremote_adjust:
                mlremote_lib_paths = fetch_mlremote_lib_paths()
            else:
                mlremote_lib_paths = []

            all_paths = env_lib_paths + mlremote_lib_paths + all_paths

        self._lib_paths = remove_duplicates([os.path.abspath(path) for path in all_paths])

    def _add_json_vars(self, model):
        model['working_directory'] = self._launch_directory
        model['library_paths'] = self._lib_paths
        model['library_variable'] = self._lib_path_var

    def _make_specific_commands_win(self, cmds):
        if self._lib_paths:
            # no quotes in PATH var
            new_path_var = ";".join(self._lib_paths + ["%PATH%"])
            cmds.append("PATH={}".format(new_path_var))

        cmds.extend([
            format_command_line(self._exe_command),
            "set __RETCODE__=%ERRORLEVEL%",
        ])

    def _make_specific_commands_posix(self, cmds):
        exe_cmd = list(self._exe_command)

        if self._lib_paths:
            # add quotes around the entire definition, not each path, and ensure the
            # quote follows "=" so format_command_line() won't bork it
            exe_cmd.insert(0, '{0}="{1}:${0}"'.format(self._lib_path_var, os.pathsep.join(self._lib_paths)))

        cmds.extend([
            format_command_line(exe_cmd),
            "__RETCODE__=$?",
        ])


def generate_invoke_model(opts, package, proj, is_device_build, environ=os.environ):
    is_device_program_build = is_device_build and proj.is_program()
    is_non_device_package_build = not is_device_build and proj.is_package()
    is_non_program_and_non_package = not proj.is_program() and not proj.is_package()

    if is_device_program_build or is_non_device_package_build or is_non_program_and_non_package:
        # Ignore builds that cannot be invoked.
        return None

    model_factory = (is_device_build and InvokeModelDevice or InvokeModelHost)
    model = model_factory(opts, package, proj, environ)
    return model


def generate_invoke_models(opts):
    env = build_env

    mg = MakeGen(env.projects(), env.components(), opts.build_scope)
    mg.generate(env.user_projects())

    platform = build_env.platform()
    is_device_build = (platform.name == platforms.lumin.NAME)

    # track the current package
    # (we will see, for a .package that REFS .mabu,
    # first the .package, then the .mabu)
    package = None

    models = []

    # Find any invokable projects.
    for proj in build_env.fetch_scoped_projects(0):
        model = generate_invoke_model(opts, package, proj, is_device_build)
        if model:
            models.append(model)
        if proj.is_package():
            package = proj

    if not models:
        diags.error("no invokable projects found.")

    return models
