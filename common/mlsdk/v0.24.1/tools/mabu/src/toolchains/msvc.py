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
import locale
import os
import subprocess
import sys
import pickle
from abc import abstractmethod
from pickle import PickleError

import architectures
import build_env
import build_vars
import config
import diags
import platforms
import project_vars
from build_model import Path, Composition, Command, InfoCommand, Buildable, BuildException, \
    Variable, Rule
from config import TOOLS_DIR, HOSTOS, OS_WIN
from diags import warn
from run_vars import COMPILER_PREFIX, LINKER_PREFIX
from toolchains._base import Toolchain, version_compare
from utils import mkdir_p, rename_with_retry, format_command_line


class MSVCBase(Toolchain):
    NAME_MSVC_LATEST = None

    def __init__(self, name, version):
        Toolchain.__init__(self, name, version)
        self.tool_bin_path = {}
        self.system_libs_paths = {}
        self.system_include_paths = {}

        self.current_tool_bin_path = ""
        self.current_system_lib_paths = []
        self.current_system_include_paths = []

    @abstractmethod
    def _scan_vs_env(self, build_arch, host_arch):
        """
        Scan for all the environment variables MSVC setup scripts set to build for @build_arch on @host_arch.
        :param build_arch:
        :param host_arch:
        :return: Dict[str,str]
        """
        pass

    # Keep these variables even if they were set before invoking vcvarsall/vsdevcmd
    _reqd_vars = [
        'INCLUDE',
        'LIB',
        'PATH',
        'UCRTCONTENTROOT'
        'UCRTVERSION',
        'UNIVERALCRTSKDIR',
        'VCINSTALLDIR',
        'VCTARGETSPATH',
        'WINDOWSLIBPATH',
        'WINDOWSDKDIR',
        'WINDOWSDKLIBVERSION',
        'WINDOWSDKVERSION',
    ]

    def _get_vs_env(self, build_arch, host_arch):
        # MSVC scripts are very slow -- remember the likely-to-be-invariant information
        # between runs.  This is optimized for a single installation of 2015 and 2017.
        # The cache of information is used only when mabu's version, the detected _vcvars_path,
        # the MSVC version, and the host/target architecture match.
        CACHE_VERSION = 5

        # see if have a cached copy
        cache_dir = self.get_cache_dir()
        cache_file = os.path.join(cache_dir, "{}_{}_{}_{}.dat".format(self.name, build_arch, host_arch, config.VERSION))
        if os.path.exists(cache_file):
            try:
                with open(cache_file, 'rb') as f:
                    info = pickle.load(f)
                    if isinstance(info, tuple) and \
                            len(info) == 3 and \
                            info[0] == CACHE_VERSION and \
                            info[1] == self._vcvars_path and \
                            isinstance(info[2], dict):
                        # new env is the last-detected env but with PATH using a pattern
                        new_env = info[2]

                        # we normalize PATH to full uppercase and add this marker below
                        path_pattern = new_env.get('PATH', '{PATH}')
                        new_env['PATH'] = path_pattern.replace('{PATH}', os.getenv('PATH', ''))
                        return new_env
            except (OSError, PickleError) as e:
                diags.debug("failed to read cache file: {}", str(e))

        # else, do the slow work to scan them
        full_env = self._scan_vs_env(build_arch, host_arch)

        # get diffs from current env (don't apply unrelated env vars from an older detection)
        # and keep uppercase version of PATH to simplify logic below
        def canon_env(k):
            if k.upper() == 'PATH':
                return 'PATH'
            return k

        env = {canon_env(k): v for (k, v) in full_env.items() if k.upper() in self._reqd_vars or os.environ.get(k, None) != v}

        # PATH is a special case: add the new prepended/appended settings to get those related to MSVC setup,
        # but don't embed the current PATH there
        my_path = os.environ.get('PATH', os.environ.get('Path'))  # OS variable may be either case
        vs_path = env.get('PATH')  # we normalized this above
        if my_path is None or vs_path is None:
            # suspicious; don't cache
            diags.debug("cannot detect PATH changes; not caching")
            return env

        try:
            my_index = vs_path.index(my_path)
        except IndexError:
            # suspicious; don't cache
            diags.debug("MSVC setup script modified PATH in an unexpected way; not caching")
            return env

        # save a pattern that preserves the prepended and appended entries
        path_update_pattern = vs_path[0:my_index] + "{PATH}" + vs_path[my_index + len(my_path):]
        env['PATH'] = path_update_pattern

        # and try to save
        try:
            mkdir_p(cache_dir)
            temp_cache_file = cache_file + str(os.getpid())
            with open(temp_cache_file, 'wb') as f:
                pickle.dump((CACHE_VERSION, self._vcvars_path, env), f)

            rename_with_retry(temp_cache_file, cache_file)
        except OSError as e:
            # nope, just ignore
            diags.debug("failed to write cache file: {}", str(e))

        return env

    @staticmethod
    def _get_host_arch():
        # get the PC architecture (avoid platform module, which is expensive)
        import struct
        try:
            size = struct.calcsize('P')
        except struct.error:
            # Older installations can only query longs
            size = struct.calcsize('l')

        host_arch = size == 8 and architectures.x64.NAME or architectures.x86.NAME

        return host_arch

    @staticmethod
    def _get_tool_arch(build_arch, host_arch):
        return "{}_{}".format(build_arch, host_arch)

    def detect(self):
        """
        Detect the desired tools in the machine or environment.

        This should populate self._tools.

        This method tell whether the receiver is available
        for real builds.  Even if this method fails, however,
        the receiver may still be used, e.g. for unit tests.
        :rtype: str for error or None for detected
        """
        # Only when building for Windows should we verify that the selected version of Visual Studio is installed.
        if HOSTOS == OS_WIN and not self.exists():
            return "No Visual Studio installation could be found for toolchain {}".format(self.name)

        host_arch = self._get_host_arch()
        build_arch = build_env.spec() and build_env.spec().arch or host_arch

        if HOSTOS == OS_WIN:
            try:
                self._locate_vc(build_arch, host_arch)
            except BuildException as e:
                return str(e)

        else:
            return "MSVC toolchain only supported on Windows"

        # update full driver path
        self._driver = os.path.join(self.current_tool_bin_path, "cl.exe")

        err = self.register_tool_based_on_driver(build_vars.CC, ["cl.exe"])
        if err:
            return err

        err = self.register_tool_based_on_driver(build_vars.CXX, ["cl.exe"], self._tools[build_vars.CC])
        if err:
            return err

        err = self.register_tool_based_on_driver(build_vars.LD, ["link.exe"])
        if err:
            return err

        err = self.register_tool_based_on_driver(build_vars.AR, ["lib.exe"])
        if err:
            return err

        err = self.register_tool_based_on_driver(build_vars.AS, ["ml{}.exe".format(
            "64" if build_arch == architectures.x64.NAME else "")])
        if err:
            return err

        # get preprocessor from MinGW/msys (we put theses on the local PATH)
        err = self.register_tool_based_on_driver(build_vars.CPP, ['cpp'])
        if err:
            return err

        return None

    def _convert_string_to_dict(self, input_string):
        lines = input_string.replace("\r", "").split("\n")
        output = {}

        for line in lines:
            if "=" in line:
                key, value = line.split("=", 1)
                output[key] = value

        return output

    def _locate_vc(self, build_arch, host_arch):
        tool_arch = self._get_tool_arch(build_arch, host_arch)

        if tool_arch not in self.tool_bin_path or tool_arch not in self.system_libs_paths:
            vs_env = self._get_vs_env(build_arch, host_arch)

            for key, value in vs_env.items():
                # The Visual Studio environment is just an augmentation of the existing system environment at the time
                # the vcvars batch file was called, so it's safe to overwrite the system environment with the new values.
                os.environ[key] = value

            bin_path = None
            system_lib_paths = []
            system_include_paths = []

            def split_path_string_into_list(path_string):
                # Canonicalize all paths to be in a Unix-like format.
                return [path.replace("\\", "/") for path in path_string.split(os.pathsep) if path]

            # The vcvars batch file prepends the "Path" environment variable with the location of the build tools,
            # so we need to search the value to see which path we need to use.
            for key, value in os.environ.items():
                key_lowered = key.lower()

                if key_lowered == "path":
                    sysPaths = value.split(os.pathsep)

                    for path in sysPaths:
                        hypotheical_compiler_path = os.path.join(path, "cl.exe")

                        # Check if the hypothetical path to the compiler is actually real.
                        if os.access(hypotheical_compiler_path, os.F_OK):
                            bin_path = path
                            break

                elif key_lowered == "lib":
                    system_lib_paths.extend(split_path_string_into_list(value))

                elif key_lowered == "include":
                    system_include_paths.extend(split_path_string_into_list(value))

            # Make sure the bin path was found.
            if not bin_path:
                raise BuildException("MSVC binary path not found for toolchain {}".format(self.name))

            self.tool_bin_path[tool_arch] = bin_path
            self.system_libs_paths[tool_arch] = system_lib_paths
            self.system_include_paths[tool_arch] = system_include_paths

        self.current_tool_bin_path = self.tool_bin_path[tool_arch]
        self.current_system_lib_paths = self.system_libs_paths[tool_arch]
        self.current_system_include_paths = self.system_include_paths[tool_arch]

    def get_unique_artifacts(self, proj):
        """
        Get project artifacts specific to a toolchain.
        :type proj: projects.Project
        :return: List of project artifacts for the toolchain.
        :rtype: List[build_model.Path]
        """
        return [
            Path(proj.outdir_path(), "{}.ilk".format(proj.output_name)),
            Path(proj.outdir_path(), "{}.idb".format(proj.output_name)),
            Path(proj.outdir_path(), "{}.pdb".format(proj.output_name)),
        ]

    def create_assembler_commands(self, env, proj, kind, source_path, obj_path):
        """
        Create command(s) that generate the object file for an assembly file.
        :type env: build_env
        :type proj: build_model.Project
        :type kind: kinds._base.Kind
        :type source_path: build_model.Path
        :type obj_path: build_model.Path
        :rtype: List[build_model.Command]
        """
        user_asmflags = proj.combined[project_vars.ASMFLAGS]

        # include paths
        inc_args = [Composition("-I", inc) for inc in proj.combined[project_vars.INCS]]

        # macro definitions
        def_args = [Composition("-D", defn) for defn in proj.combined[project_vars.DEFS]]

        #
        # MSVC assembly
        #
        args = [
            self.tool_var(build_vars.AS),

            # silence!
            "-nologo",

            # The output object MUST be specified BEFORE the input source file,
            # otherwise ml will silently fail to create it!
            Composition("-Fo", obj_path),

            # make a single object
            "-c", source_path,
        ] \
        + user_asmflags \
        + inc_args \
        + def_args

        assemble = Command(args)
        assemble.winargs = True

        return [
            InfoCommand("[{0}] Assembling '{1}'...".format(proj.name, source_path.filename)),
            assemble
        ]

    def create_compiler_commands(self, env, proj, kind, source_path, obj_path):
        """
        Create command(s) that generate the object file for a C/C++
        file in the given project along with generating dependency
        rules for future builds.
        :type env: build_env
        :type proj: projects.Project
        :type kind: kinds._base.Kind
        :type source_path: build_model.Path
        :type obj_path: build_model.Path
        :return: List[build_model.Command]
        """
        #
        # Dependency generation
        #
        args = [
            Variable('PYTHON', sys.executable.replace('\\', '/')),
            Variable('DEPGEN', os.path.join(TOOLS_DIR, HOSTOS, "depgen.py").replace('\\', '/')),

            # input
            source_path,

            # depfile output
            proj.get_dependency_output(obj_path),

            # obj file path
            obj_path,
        ]

        # add the include paths
        for i in proj.combined[project_vars.INCS]:
            args.append(Composition("-I", i))

        # add the macro definitions
        for d in proj.combined[project_vars.DEFS]:
            args.append(Composition("-D", d))

        # add the other explicit preprocessor flags
        args += proj.combined[project_vars.CPPFLAGS]

        is_cxx = proj.is_cxx_file(source_path)
        user_cppflags = proj.combined[project_vars.CPPFLAGS]
        user_cflags = proj.combined[project_vars.CFLAGS]
        user_cxxflags = proj.combined[project_vars.CXXFLAGS]

        # add includes or defines passed as raw compiler flags
        likely_cpp_flags = user_cppflags + user_cflags + (user_cxxflags if is_cxx else [])
        for arg in likely_cpp_flags:
            if arg.startswith("-I") or arg.startswith("-D") or arg.startswith("-std:"):
                args.append(arg)

        depgen = Command(args)

        # include paths
        inc_args = [Composition("-I", inc) for inc in proj.combined[project_vars.INCS] + self.current_system_include_paths]

        # macro definitions
        def_args = [Composition("-D", defn) for defn in proj.combined[project_vars.DEFS]]

        #
        # MSVC compilation
        #
        args = [
            Variable(COMPILER_PREFIX, ""),
            self.tool_var(build_vars.CXX if is_cxx else build_vars.CC),

            # silence!
            "-nologo",

            # C vs. C++ switch
            "-TP" if is_cxx else "-TC",

            # make a single object
            "-c", source_path,
            Composition("-Fo", obj_path),

            # use mspdbserver to allow access to pdb file in parallel builds
            "-FS",

            # name program database after the output, to avoid
            # crossing the streams and littering the world with
            # .pdb files
            Composition("-Fd", Path(proj.outdir_path(), os.path.basename(proj.output_name) + '.pdb')),
        ]

        # remaining user flags
        cppflags = inc_args + def_args
        cppflags += user_cppflags

        # note: we don't share CPPFLAGS directly with the 'cpp' invocation above, since
        # the user CPPFLAGS flags may indeed have msvc specifics in it
        args.append(self._variable_arg(proj, build_vars.CPPFLAGS, cppflags))

        if is_cxx:
            cxxflags = user_cxxflags
            args.append(self._variable_arg(proj, build_vars.CXXFLAGS, cxxflags))
        else:
            cflags = user_cflags
            args.append(self._variable_arg(proj, build_vars.CFLAGS, cflags))

        compile = Command(args)
        compile.winargs = True

        return [
            InfoCommand("[{0}] Compiling '{1}'...".format(proj.name, source_path.filename)),
            depgen,
            compile
        ]

    def _gather_libs(self, model, libpaths):
        """
        Create arguments to link in the given static and shared libraries.
        :rtype: List[str]
        """

        platform = build_env.platform()

        dirs = []
        libs = []

        seen_dirs = set()

        def ensure_libdir(libdir):
            resolved_dir = libdir.resolve()
            if resolved_dir not in seen_dirs:
                seen_dirs.add(resolved_dir)
                dirs.append(Composition("-libpath:", libdir))

        # add user paths
        for libpath in libpaths:
            ensure_libdir(libpath)

        for ent in model.stlibs + model.shlibs:
            ldir, lib = ent
            if ldir:
                ensure_libdir(ldir)

            lib_name = platform.format_importlib_file(lib.makefile_str())
            libs.append(lib_name)

        return dirs + libs

    def _create_executable_link_commands(self, model,
                                         type_args, label):
        proj = model.proj
        outpath = model.outpath
        outdir = proj.outdir_path()

        args = [
            Variable(LINKER_PREFIX, ""),
            self.tool_var(build_vars.LD),

            # quiet!
            "-nologo",

            # output path...
            Composition("-out:", outpath.relative_to(outdir)),

            # architecture
            '-machine:' + build_env.architecture().name,
        ]

        args += type_args

        # objects
        obj_args = [obj.relative_to(outdir) for obj in model.objs]

        # bundle excess arguments into response files (only object files for now)
        self._make_response_file_as_needed(proj, outdir, outpath, obj_args)

        args.extend(obj_args)

        # library paths
        libs = self._gather_libs(model, proj.combined[project_vars.LIBPATHS] +
                                 [Path(p) for p in self.current_system_lib_paths])

        args.append(self._variable_arg(proj, build_vars.LIBS, libs))

        # add user linker flags
        ldflags = proj.combined[project_vars.LDFLAGS]

        args.append(self._variable_arg(proj, build_vars.LDFLAGS, ldflags))

        # ensure the output file's modification date changes
        # even if the MSVC incremental linker finds no change
        # (e.g. when an object file is touched but has no structural
        # difference for the executable)

        linktouch = Variable("LINKTOUCH", os.path.join(TOOLS_DIR, HOSTOS, "linktouch.sh").replace('\\', '/'))
        args = [linktouch, outpath] + args

        link = Command(args)
        link.winargs = True
        link.cwd = outdir

        return [
            InfoCommand("[{0}] {2} '{1}'...".format(
                proj.name,
                self._friendly_name(outpath),
                label

            )),
            link
        ]

    def create_program_link_commands(self, model):
        """
        Create command(s) that generate an executable file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        return self._create_executable_link_commands(model, [], "Linking program")

    def create_shared_link_commands(self, model):
        """
        Create command(s) that generate an shared object file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        commands = self._create_executable_link_commands(model,
                                                         ["-dll"],
                                                         "Linking DLL")

        # This format requires us to use pattern rules (see 'Rule.makefile_str()')
        # but *that* then exposes a bug in make where spaces are involved in
        # paths... let's just be lazy here for now.

        # *.lib and *.exp files also come out on Windows
        # lib_path = Path(model.proj.outdir_path(), build_env.platform().format_importlib_file(model.proj.name))
        # if lib_path != model.outpath:
        #     artifact_rule.add_aux_buildable(Buildable(lib_path))

        # exp_path = Path(model.proj.outdir_path(), model.proj.output_name + '.exp')
        # artifact_rule.add_aux_buildable(Buildable(exp_path))

        return commands

    def generate_extra_rules(self, env, proj):
        """
        Override if a particular kind produces other outputs
        which need their own rules.
        :type env: build_env
        :type proj: projects.Project
         :rtype: List[Rule]
        """
        if proj.is_shared():
            # make a .lib rule that depends on DLL
            dll_path = Path(proj.outdir_path(), env.platform().format_shared_file(proj.output_name))

            lib_path = Path(proj.outdir_path(), env.platform().format_importlib_file(proj.output_name))
            if lib_path != dll_path:
                rule = Rule(proj, Buildable(lib_path), [dll_path])
                return [rule]

        return []

    def create_static_link_commands(self, model):
        """
        Create command(s) that generate a static library file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        proj = model.proj
        outpath = model.outpath
        outdir = proj.outdir_path()

        # LIB appends items to archives, so we need to wipe it out to
        # avoid keeping e.g. renamed objects
        del_command = Command([Variable('RM', 'rm'), '-f', outpath])

        lib_args = [
            self.tool_var(build_vars.AR),

            # quiet!
            "-nologo",

            # make a shared library
            Composition("-out:", outpath.relative_to(outdir)),
        ]

        # objects
        obj_args = [obj.relative_to(outdir) for obj in model.objs]

        # bundle excess arguments into response files (only object files for now)
        self._make_response_file_as_needed(proj, outdir, outpath, obj_args)

        args = list(obj_args)

        # NOTE: ignoring static/shared libraries for static link

        # add archiver flags
        arflags = proj.combined[project_vars.ARFLAGS]

        args.append(self._variable_arg(proj, build_vars.ARFLAGS, arflags))

        link = Command(lib_args + args)
        link.winargs = True
        link.cwd = outdir

        return [
            InfoCommand("[{0}] Linking static library '{1}'....".format(
                proj.name,
                self._friendly_name(outpath)
            )),
            del_command,
            link
        ]


class MSVC2015(MSVCBase):
    def __init__(self, name):
        MSVCBase.__init__(self, name, '14.0')
        self._vcvars_path = None

    @classmethod
    def can_user_override(cls):
        return cls._may_target_toolchain('msvc-2015')

    @classmethod
    def discover_toolchain(cls, driver_or_path='', platform='', preferred=False):
        if platform == platforms.lumin.NAME:
            return None

        vcvarsPath = None
        if driver_or_path:
            path = cls.find_driver_path(driver_or_path)
            if not path:
                return None
            vcvarsPath = os.path.join(os.path.split(path)[0], '..')
        else:
            comnToolsPath = os.getenv("VS140COMNTOOLS")
            if comnToolsPath and os.access(comnToolsPath, os.X_OK):
                vcvarsPath = os.path.abspath(os.path.join(comnToolsPath, "..", "..", "VC"))

        if vcvarsPath and os.access(os.path.join(vcvarsPath, "vcvarsall.bat"), os.F_OK):
            # we can't/don't detect anything but "14.0" here
            tc = MSVC2015('msvc-2015')
            tc._vcvars_path = vcvarsPath

            if not MSVCBase.NAME_MSVC_LATEST:
                MSVCBase.NAME_MSVC_LATEST = tc.name

            Toolchain.register(tc, force=True)
            return tc

        return None

    def exists(self):
        return True if self._vcvars_path else False

    def _scan_vs_env(self, build_arch, host_arch):
        """
        Detect the environment variables that MSVC supplies for us.
        :param build_arch:
        :param host_arch:
        :return: set of all variables detected after invoking vcvarsall.bat
        """
        args = {
            "x64": {
                "x64": "amd64",
                "x86": "amd64_x86",
            },
            "x86": {
                "x64": "x86_amd64",
                "x86": "x86",
            },
        }

        if host_arch not in args:
            raise BuildException("Host architecture {} is not supported".format(host_arch))

        if build_arch not in args[host_arch]:
            raise BuildException("Build architecture {} is not supported".format(build_arch))

        arg_arch = args[host_arch][build_arch]

        cmd = [os.path.join(self._vcvars_path, "vcvarsall.bat"), arg_arch, "&", "set"]
        comp = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)

        if comp.returncode:
            raise BuildException(comp.stdout + comp.stderr)

        return self._convert_string_to_dict(comp.stdout)


class MSVC201x(MSVCBase):
    VERSION_2017_LATEST = None
    VERSION_2019_LATEST = None

    def __init__(self, name, version):
        MSVCBase.__init__(self, name, version)
        self._vcvars_path = None

    @classmethod
    def can_user_override(cls):
        return cls._may_target_toolchain('msvc-2017') or cls._may_target_toolchain('msvc-2019')

    @classmethod
    def discover_toolchain(cls, driver_or_path='', platform='', preferred=False):
        if platform == platforms.lumin.NAME:
            return None

        user_directory = None
        if driver_or_path:
            path = cls.find_driver_path(driver_or_path)
            if not path:
                return None
            # i.e. assume they provided a path to 'cl' -- the vcvars path should be one step up
            user_directory = os.path.realpath(os.path.join(os.path.split(path)[0], '..'))

        vswhere_file_path = os.path.join(
            os.environ.get("ProgramFiles(x86)", '.'),
            "Microsoft Visual Studio",
            "Installer",
            "vswhere.exe"
        )

        if not os.access(vswhere_file_path, os.F_OK):
            diags.debug("did not find vswhere in '{}'".format(vswhere_file_path))
            return None

        cmd = [
            vswhere_file_path,
            "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-format", "json",
        ]

        # vswhere emits JSON in a mystery encoding until Jun 12 2018, where
        # a "-utf8" option was finally added.
        tcs = cls.run_and_decode_vswhere(user_directory, cmd, try_utf8=True, suppress_errors=True)
        if tcs is None:
            tcs = cls.run_and_decode_vswhere(user_directory, cmd, try_utf8=False)

        if not tcs:
            return None

        for tc in tcs:
            Toolchain.register(tc, force=True)

        return tcs[-1]

    @classmethod
    def run_and_decode_vswhere(cls, user_directory, cmd, try_utf8, suppress_errors=False):
        """
        vswhere emitted JSON content in a non-JSON and non-UTF8 format until Jun 2018.
        On non-English locales, the content may have non-ASCII text in the "Description"
        field of a tool entry.  Either subprocess.run() would explode here (using
        universal_newlines=True only allows running with the default locale in Python 3.5)
        or the JSON decoder would explode, not getting proper text (i.e. some Japanese
        locales could emit what looks like '\' in the text).

        So this is called twice, once hoping that the UTF-8 option is available, and decoding that
        way, or, if that fails, running without that parameters and trying to decode
        in different ways.

        :param user_directory: user-provided VS directory
        :param cmd: vswhere command
        :param suppress_errors: if True, don't report if the command failed (e.g. trying to
        use the UTF-8 option)
        :return: toolchain array, or None if something failed
        """
        # Python 3.5 can't ask for an encoding here, so decode later
        out = subprocess.run(cmd + (try_utf8 and ['-utf8'] or []), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if out.returncode != 0 and suppress_errors:
            # likely this is because "-utf8" doesn't exist in this version.
            return None

        vswhere_command_string = format_command_line(cmd)

        if try_utf8:
            encodings = ['utf-8']
        else:
            encodings = [locale.getpreferredencoding(False), "cp1252", "utf-8", "ascii"]

        for encoding in encodings:
            try:
                # The last (or only) encoding is the fallback where we throw caution to the wind
                # and hope the filesystem paths are MS defaults which are always ASCII
                handling = encoding == encodings[-1] and 'replace' or 'strict'
                stdout_text = out.stdout.decode(encoding, handling)
                stderr_text = out.stderr.decode(encoding, handling)
            except UnicodeDecodeError as ue:
                diags.warn("failed to decode content as {} in {}:\n{}".format(
                    encoding, vswhere_command_string, str(ue)))
                continue

            if out.returncode != 0:
                # suppress_errors must be False; delay reporting til here so we can show the text
                # (no need to keep looping)
                diags.debug("failed to invoke {}:\n{}".format(vswhere_command_string,
                                                              stdout_text + stderr_text))
                return

            try:
                tcs = cls.parse_tools(stdout_text, user_directory)
                return tcs
            except json.JSONDecodeError as je:
                # likely we decoded the text but it was mangled (e.g. EUC-JP text has misleading '\' chars)
                diags.debug("failed to decode content as {} in {} ({}):\n{}".format(
                    encoding, vswhere_command_string, str(je),
                    stdout_text + stderr_text))
                continue

        # nothing worked
        return None

    @classmethod
    def parse_tools(cls, json_text, user_directory=None, validate=True):
        """
        Parse JSON data from vswhere and create a list of Toolchains
        :param json_text: text from vswhere stdout
        :param user_directory: if set, user provided a CC override where we want to look
        :param validate: validate that directories exist
        :return: list[Toolchain]
        """
        try:
            json_data = json.loads(json_text)
        except json.decoder.JSONDecodeError as e:
            diags.warn("vswhere provided invalid JSON, cannot detect Visual Studio 2017/2019:\n{}\n{}".format(e, json_text))
            return []

        # Parse the install information.
        tcs = []
        for install_data in json_data:
            full_version = install_data["installationVersion"]

            # Consider all sub-versions of version 15.
            if full_version.startswith("15.") or full_version.startswith("16."):
                version = full_version
                try:
                    segments = version.split('.')
                    version = segments[0] + '.' + segments[1]
                except IndexError:
                    pass

                install_name = install_data["displayName"]
                install_path = install_data["installationPath"]
                common_tools_path = os.path.join(install_path, "Common7", "Tools")

                # ignore this one if user override didn't reference its path
                if user_directory and os.path.realpath(install_path) not in user_directory:
                    continue

                if validate:
                    # Verify the install path exists.
                    if not os.access(install_path, os.X_OK):
                        warn("Found {} with invalid installation path: \"{}\"".format(install_name, install_path))
                        continue

                    # Verify the common tools directory exists within the install path.
                    if not os.access(common_tools_path, os.X_OK):
                        warn("{} does not have the required common tools path: \"{}\"", install_name, common_tools_path)
                        continue

                if version[0:3] == "15.":
                    tc = MSVC201x('msvc-2017-' + version, version)
                    tc._vcvars_path = common_tools_path

                    if not cls.VERSION_2017_LATEST or version_compare(version, cls.VERSION_2017_LATEST) > 0:
                        cls.VERSION_2017_LATEST = version
                        MSVCBase.NAME_MSVC_LATEST = tc.name
                elif version[0:3] == "16.":
                    tc = MSVC201x('msvc-2019-' + version, version)
                    tc._vcvars_path = common_tools_path

                    if not cls.VERSION_2019_LATEST or version_compare(version, cls.VERSION_2019_LATEST) > 0:
                        cls.VERSION_2019_LATEST = version
                        MSVCBase.NAME_MSVC_LATEST = tc.name
                else:
                    # really a bug from not matching the `if` above, but don't kill mabu
                    warn("failed to register MSVC " + version)

                tcs.append(tc)

        return tcs

    def exists(self):
        return True if self._vcvars_path else False

    def _scan_vs_env(self, build_arch, host_arch):
        """
        Detect which env vars are required for MSVC builds
        :param build_arch:
        :param host_arch:
        :return: set of all variables detected after invoking VsDevCmd.bat
        """
        cmd = [
            os.path.join(self._vcvars_path, "VsDevCmd.bat"),
            "-no_logo",
            "-arch={}".format(build_arch),
            "-host_arch={}".format(host_arch),
            "&",
            "set",
        ]
        comp = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)

        if comp.returncode or comp.stdout.startswith("[ERROR:"):
            raise BuildException(comp.stdout + comp.stderr)

        return self._convert_string_to_dict(comp.stdout)


def discover():
    if HOSTOS == OS_WIN:
        Toolchain.register_toolchain_class(MSVC2015)
        Toolchain.register_toolchain_class(MSVC201x)

        MSVC2015.discover_toolchain()
        MSVC201x.discover_toolchain()

    # NOTE: msvc-2015 is always registered directly as is, without a "14.0" suffix, since
    # we have no way to detect this, and it will preserve compatibility with older releases.
    if not Toolchain.find('msvc-2015'):
        # ensure a dummy is available
        Toolchain.register(MSVC2015('msvc-2015'))

    # Select the newest 2017 that we detected (these are placeholders, even if not detected,
    # so the --help-target report mentions the expected name; .detect() can report that
    # it is not actually installed)
    if not MSVC201x.VERSION_2017_LATEST:
        MSVC201x.VERSION_2017_LATEST = '15.0'
        # ensure a dummy is available
        Toolchain.register(MSVC201x('msvc-2017-15.0', '15.0'))

    Toolchain.register_alias("msvc-2017", 'msvc-2017-' + MSVC201x.VERSION_2017_LATEST)

    # Select the newest 2019 that we detected (these are placeholders, even if not detected,
    # so the --help-target report mentions the expected name; .detect() can report that
    # it is not actually installed)
    if not MSVC201x.VERSION_2019_LATEST:
        MSVC201x.VERSION_2019_LATEST = '16.0'
        # ensure a dummy is available
        Toolchain.register(MSVC201x('msvc-2019-16.0', '16.0'))

    Toolchain.register_alias("msvc-2019", 'msvc-2019-' + MSVC201x.VERSION_2019_LATEST)

    if HOSTOS == OS_WIN:
        # When running from Visual Studio 2017 Command Prompt, the "VisualStudioVersion" environment
        # variable will always be present.  If not available, and other environment variables seen
        # in 2015 command prompts are visible, we can use this to encourage mabu to use the appropriate version.
        # (Users can also set this manually to make mabu select the right version, though -t msvc-2015 | msvc-2017
        # is the preferred path.)
        major_version = os.getenv("VisualStudioVersion")
        if not major_version and os.getenv("WindowsSdkDir") and os.getenv("INCLUDE") and os.getenv("LIB"):
            major_version = "14.0"

        default_toolchain = MSVCBase.NAME_MSVC_LATEST or 'msvc-2015'
        if major_version:
            diags.debug("Detected 'VisualStudioVersion={}', selecting default 'msvc' based on that".format(major_version))
            if major_version.startswith("14."):
                default_toolchain = 'msvc-2015'
            elif major_version.startswith("15."):
                default_toolchain = 'msvc-2017'
            elif major_version.startswith("16."):
                default_toolchain = 'msvc-2019'
            else:
                diags.warn("mabu does not recognize this version of Visual Studio yet ({})".format(major_version))
        else:
            diags.debug("No Visual Studio Command Prompt markers detected; using latest detected 'msvc'")

        Toolchain.register_alias("msvc", default_toolchain)

        # set this up for tests (unrelated to detection above)
        if not MSVCBase.NAME_MSVC_LATEST:
            MSVCBase.NAME_MSVC_LATEST = 'msvc-2017'

    else:
        # set this up for tests
        MSVCBase.NAME_MSVC_LATEST = 'msvc-2017'

        # register dummy targets for other OSes (to report "Only works on Windows" rather than "I dunno what 'msvc' is")
        Toolchain.register_alias('msvc-2017', 'msvc-2017-15.0')
        Toolchain.register_alias('msvc', 'msvc-2017')
