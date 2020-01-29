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

import architectures
import build_env
import build_vars
import project_vars
import run_vars
import platforms
from abc import ABCMeta

from build_model import Command, Path, Composition, InfoCommand, Buildable, Variable, Literal
from defaults import arch_default
from link_flags import LinkFlags
from run_vars import COMPILER_PREFIX, LINKER_PREFIX
from toolchains._base import Toolchain


class GCCLike(Toolchain):
    __metaclass__ = ABCMeta

    def __init__(self, name, version=None, driver=None, platform=None):
        Toolchain.__init__(self, name, version=version, driver=driver, platform=platform)

        self._targetMacVersion = None

    def detect(self):
        # Set the command search paths for the build commands
        return None

    def create_assembler_commands(self, env, proj, kind, source_path, obj_path):
        """
        Create command(s) that generate the object file for an assembly file.
        :type env: build_env
        :type proj: projects.Project
        :type kind: kinds._base.Kind
        :type source_path: build_model.Path
        :type obj_path: build_model.Path
        :rtype: List[build_model.Command]
        """
        arch_name = build_env.spec().arch
        user_asmflags = proj.combined[project_vars.ASMFLAGS]
        inc_args = [Composition("-I", inc) for inc in proj.combined[project_vars.INCS]]
        def_args = [Composition("-D", defn) for defn in proj.combined[project_vars.DEFS]]

        #
        # GNU assembly
        #
        args = [
            self.tool_var(build_vars.CXX),

            # The output object MUST be specified BEFORE the input source file,
            # otherwise ml will silently fail to create it!
            Composition("-o", obj_path),

            # make a single object
            "-c", source_path,
        ] \
        + user_asmflags \
        + inc_args \
        + def_args

        if self._platform != platforms.lumin.NAME:
            args.append("-m64" if arch_name == architectures.x64.NAME else "-m32")

        assemble_cmd = Command(args)
        assemble_cmd.search_paths = self._command_search_paths

        return [
            InfoCommand("[{0}] Assembling '{1}'...".format(proj.name, source_path.filename)),
            assemble_cmd
        ]

    def get_unique_artifacts(self, proj):
        """
        Get project artifacts specific to a toolchain.
        :type proj: projects.Project
        :return: List of project artifacts for the toolchain.
        :rtype: List[build_model.Path]
        """
        artifacts = []

        if self._platform == platforms.lumin.NAME:
            artifacts.append(Path(proj.outdir_path(), "{}.sym".format(proj.output_name)))

        return artifacts

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
        arch_name = build_env.spec().arch
        platform_name = build_env.spec().platform
        is_cxx = proj.is_cxx_file(str(source_path)) or proj.is_objcpp_file(str(source_path))

        args = [
            Variable(COMPILER_PREFIX, ""),
            is_cxx and self.tool_var(build_vars.CXX) or self.tool_var(build_vars.CC),

            # make a single object
            "-c", source_path,
            "-o", obj_path,
        ]

        if platform_name != platforms.lumin.NAME:
            args.append("-m64" if arch_name == architectures.x64.NAME else "-m32")

        # dependency rules
        depfile = proj.get_dependency_output(obj_path)
        args += [ "-MD", "-MP", "-MF", depfile ]

        cppflags = []

        # include paths
        cppflags += [Composition("-I", inc) for inc in proj.combined[project_vars.INCS]]

        # macro definitions
        cppflags += [Composition("-D", defn) for defn in proj.combined[project_vars.DEFS]]

        # remaining flags
        cppflags += proj.combined[project_vars.CPPFLAGS]

        args.append(self._variable_arg(proj, build_vars.CPPFLAGS, cppflags))

        if is_cxx:
            cxxflags = proj.combined[project_vars.CXXFLAGS]
            args.append(self._variable_arg(proj, build_vars.CXXFLAGS, cxxflags))
        else:
            cflags = proj.combined[project_vars.CFLAGS]
            args.append(self._variable_arg(proj, build_vars.CFLAGS, cflags))

        compile_cmd = Command(args)
        compile_cmd.search_paths = self._command_search_paths

        # ensure the generated .d file has a rule for the source
        # file, so if/when the source file is moved around,
        # to avoid fatal "No rule to build <old file location>" errors
        srcdep_cmd = Command(['echo', source_path, ':', '>>', depfile])

        return [
            InfoCommand("[{0}] Compiling '{1}'...".format(proj.name, source_path.filename)),
            compile_cmd,
            srcdep_cmd
        ]

    def _gather_libs(self, model, libpaths):
        """
        Create arguments to link in the given static and shared libraries.
        :type model: kinds._base.LinkModel
         :rtype: List[str]
        """
        platform = build_env.platform()

        # The library grouping is only needed for certain platforms (linkers),
        # to ensure symbols can be found globally between static and shared libs.
        requires_lib_group = (model.shlibs or model.stlibs) and platform.name != platforms.osx.NAME

        # Ability to avoid linking unneeded libraries only in GNU linkers
        # (macOS does not use the GNU linker)
        requires_as_needed = model.link_flags.ignore_unused_shlibs and model.shlibs and platform.name != platforms.osx.NAME

        dirs = []
        libs = []

        seen_dirs = set()

        def ensure_libdir(libdir):
            resolved_dir = libdir.resolve()
            if resolved_dir not in seen_dirs:
                seen_dirs.add(resolved_dir)
                dirs.append(Composition("-L", libdir))

        # add user paths
        for libpath in libpaths:
            ensure_libdir(libpath)

        # if the option is used, allow the linker to find symbols
        # in any one of the libraries (rather than typical Unix order,
        # which requires strict client -> dependent file ordering)
        if requires_lib_group:
            libs += ["-Wl,--start-group"]

        # set up static libs first
        if model.stlibs:
            if platform.name != platforms.osx.NAME:
                libs.append("-Bstatic")

                # so that static libs can read contents from shared libs
                libs += ["-Wl,--no-as-needed"]

            for ent in model.stlibs:
                ldir, lib = ent
                if ldir:
                    libs.append(Path(ldir, platform.format_static_file(lib.makefile_str())))
                else:
                    libs.append(Composition("-l", lib))

        # then supply shared libs
        if model.shlibs:
            if platform.name != platforms.osx.NAME:
                libs.append("-Bdynamic")

            if requires_as_needed:
                libs += ["-Wl,--as-needed"]

            for ent in model.shlibs:
                ldir, lib = ent
                if ldir:
                    ensure_libdir(ldir)

                libs.append(Composition("-l", lib))

            if requires_as_needed:
                libs += ["-Wl,--no-as-needed"]

        if requires_lib_group:
            libs += ["-Wl,--end-group"]

        return dirs + libs

    def _create_executable_link_commands(self, model, type_args, label):
        """
        Create command(s) that generate an executable file for a
        given list of object files, static libraries, and shared libraries.

        The commands will run within the $(proj_OUTPUT) directory,
        so will not be full paths.  This is both to reduce the command
        line length for Windows and to ensure that shared libraries
        are linked by their target-side path (AOSP doesn't support -Wl,-rpath).
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        arch_name = build_env.spec().arch

        outpath = model.outpath
        proj = model.proj
        outdir = proj.outdir_path()

        # N.B.: the "stdc++" component should take care of this
        # is_cxx = proj.has_cxx_content(env)
        is_cxx = False

        args = [
            Variable(LINKER_PREFIX, ""),
            is_cxx and self.tool_var(build_vars.CXX) or self.tool_var(build_vars.CC),

            # make an executable
            "-o", outpath.relative_to(outdir)
        ]

        if build_env.platform().name != platforms.lumin.NAME:
            args.append("-m64" if arch_name == architectures.x64.NAME else "-m32")

        args += type_args

        # objects
        obj_args = [obj.relative_to(outdir) for obj in model.objs]

        # bundle excess arguments into response files (only object files for now)
        self._make_response_file_as_needed(proj, outdir, outpath, obj_args)

        args.extend(obj_args)

        # libraries and paths
        libs = self._gather_libs(model, proj.combined[project_vars.LIBPATHS])

        args.append(self._variable_arg(proj, build_vars.LIBS, libs))

        # add linker flags (even if we target CC or CXX, use these)
        ldflags = proj.combined[project_vars.LDFLAGS]

        args.append(self._variable_arg(proj, build_vars.LDFLAGS, ldflags))

        link_cmd = Command(args)
        link_cmd.cwd = outdir
        link_cmd.search_paths = self._command_search_paths

        commands = [
            InfoCommand("[{0}] {2} '{1}'...".format(
                proj.name, self._friendly_name(outpath), label)),
            link_cmd
        ]

        self.add_strip_commands(commands, outpath)

        return commands

    def create_program_link_commands(self, model):
        """
        Create command(s) that generate an executable file for a
        given list of object files, static libraries, and shared libraries.

        The commands will run within the $(proj_OUTPUT) directory,
        so will not be full paths.  This is both to reduce the command
        line length for Windows and to ensure that shared libraries
        are linked by their target-side path (AOSP doesn't support -Wl,-rpath).
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
        type_args = [Literal("-shared")]

        proj = model.proj

        from config import OS_WIN
        from config import HOSTOS

        if build_env.spec().platform == platforms.win.NAME:
            # ensure an import lib is generated, since we promise it
            self._add_importlib_option(type_args, proj)

        commands = self._create_executable_link_commands(model,
                                                         type_args,
                                                         "Linking shared object")

        if build_env.platform().is_host() and HOSTOS == OS_WIN:
            # a *.lib file also comes out on Windows
            lib_path = Path(proj.outdir_path(),
                            build_env.platform().format_importlib_file(
                                proj.output_name))
            model.artifact_rule.add_aux_buildable(Buildable(lib_path))

        return commands

    def add_strip_commands(self, commands, out_path):
        # if platform is lumin then add commands to make a copy of the output and strip the debug symbols
        if self._platform == platforms.lumin.NAME:
            sym_file = out_path.change_extension(".sym")
            commands += [
                Command([self.tool_var(build_vars.OBJCOPY), "--only-keep-debug", out_path, sym_file]),
                Command([self.tool_var(build_vars.OBJCOPY), "--add-gnu-debuglink", "{}".format(sym_file), out_path, out_path]),
                Command([self.tool_var(build_vars.STRIP), "--strip-unneeded", out_path])
            ]

        return commands

    def create_static_link_commands(self, model):
        """
        Create command(s) that generate an static library file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """

        proj = model.proj
        outpath = model.outpath
        outdir = proj.outdir_path()

        # ar appends items to archives, so we need to wipe it out to
        # avoid keeping e.g. renamed objects
        del_command = Command([Variable('RM', 'rm'), '-f', outpath])

        args = [
            self.tool_var(build_vars.AR),

            # make a static library w/index
            "crs", outpath.relative_to(outdir),
        ]

        # objects
        obj_args = [obj.relative_to(outdir) for obj in model.objs]

        # bundle excess arguments into response files (only object files for now)
        self._make_response_file_as_needed(proj, outdir, outpath, obj_args)

        args.extend(obj_args)

        # NOTE: ignoring static/shared libraries for static link

        # add archiver flags
        arflags = proj.combined[project_vars.ARFLAGS]

        args.append(self._variable_arg(proj, build_vars.ARFLAGS, arflags))

        link_cmd = Command(args)
        link_cmd.cwd = outdir
        link_cmd.search_paths = self._command_search_paths

        return [
            InfoCommand("[{0}] Linking static library '{1}'...".format(
                        proj.name, self._friendly_name(outpath))),

            del_command,
            link_cmd,

            # force timestamp change, in case nothing changed
            # (else, we keep running the archiver over
            # and over when nothing has changed).
            Command([Variable('TOUCH', 'touch'), outpath]),
        ]

    @staticmethod
    def get_target_arch(platform):
        arch_map = {'x86': 'i686', 'x64': 'x86_64', 'aarch64': 'aarch64'}
        arch = arch_default(platform)
        return arch_map[arch]

    @staticmethod
    def get_target_triple(platform, with_variant=False):
        arch = GCCLike.get_target_arch(platform)
        triple = arch + ('-none' if with_variant else '') + '-linux-android'
        return triple

    def add_cross_toolchain_tool(self, var, tool, rel_path='.', rename_tool=None):
        """
        Create tool mappings for cross toolchain.
        :type var: tool variable
        :type tool: base tool name
        :type rel_path: path relative to MLSDK/tools/toolchains to search
        :type rename_tool: if set, use this native name instead of guessing
        :rtype: str for error or None for detected
        """
        user_override = self.get_user_override(var)
        if user_override:
            err = self.register_tool_based_on_driver(
                var, [user_override], fallback=user_override)
            return err

        from config import HOSTOS, OS_WIN

        if rename_tool:
            tool_name = rename_tool
        else:
            triple = self.get_target_triple(self._platform)
            tool_name = triple + '-' + tool if triple not in tool else tool

        mltools = run_vars.runtime_values.get(run_vars.MLTOOLS)
        bin_dir = os.path.join(mltools or '', 'bin')

        tool_path = os.path.join(bin_dir, rel_path, tool_name)
        if not os.path.exists(tool_path):
            if HOSTOS == OS_WIN:
                if os.path.exists(tool_path + ".exe"):
                    tool_path += ".exe"

        if not os.path.exists(tool_path):
            return "Tool {0} does not exist".format(tool_path)

        # keep paths in Unix-friendly format
        tool_path = os.path.normpath(tool_path).replace('\\', '/')

        self._tools[var] = tool_path

        return None

