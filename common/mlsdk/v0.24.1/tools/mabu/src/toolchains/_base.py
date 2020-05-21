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
from abc import abstractmethod, ABCMeta

import build_env
import diags
import utils
from run_vars import runtime_values, SPEC
from build_model import BuildObject, Variable, Command, split_by_length, Composition, Node, BuildSpec, BuildException
from config import HOSTOS, OS_WIN, MSYS_DIR
from diags import debug
from utils import PathCache, uniquify
import subprocess


class Toolchain(BuildObject):
    __metaclass__ = ABCMeta

    _toolchain_classes = []

    """
    A toolchain is the set of tools (compiler, linker, assembler, etc.)
    that processes source and object code to construct build artifacts.
    """
    @classmethod
    def register(cls, bo, force=False):
        """ Register a new instance of a build object.
        An object of the same name must not already be registered. """
        assert issubclass(type(bo), cls)
        name = bo.platformed_name
        if not force:
            assert name not in cls._ents
        debug("Registering {0} named {1}".format(cls.__name__, name))
        cls._ents[name] = bo

    @classmethod
    def remove(cls, name_or_key):
        """ Remove any object or alias with the given name """
        try:
            # search by object
            if name_or_key.platformed_name in cls._ents:
                exist = cls._ents[name_or_key.platformed_name]
                if exist is name_or_key:
                    del cls._ents[name_or_key.platformed_name]
            if name_or_key.name in cls._ents:
                exist = cls._ents[name_or_key.name]
                if exist is name_or_key:
                    del cls._ents[name_or_key.name]
        except AttributeError:
            # search by string
            del cls._ents[cls.find(name_or_key).platformed_name]

    @classmethod
    def find(cls, name, platform=None):
        platform = cls.default_platform(platform)
        aliases = cls._alias.get(platform, {})
        while name in aliases:
            name = aliases[name]

        # toolchains have platform-specific names
        platform_name = platform + '_' + name
        if platform_name in cls._ents:
            return cls._ents[platform_name]

        return None

    @staticmethod
    def get_cache_dir():
        if HOSTOS == OS_WIN:
            return os.path.join(os.environ.get('USERPROFILE', os.environ.get('HOME', '.')), 'AppData', 'Local', 'mabu')
        else:
            return os.path.join(os.getenv('HOME', '.'), '.cache', 'mabu')

    def __init__(self, name, version=None, driver=None, platform=None):
        """
        :param name: build target segment
        :param version: optional specific version (can be changed later)
        :param driver: "driver" executable name (e.g. "gcc-7")
        :param platform: target platform (defaults to host)
        """
        BuildObject.__init__(self, name)

        self._version = version
        self._driver = driver
        self._platform = platform or HOSTOS

        self._tools = {}
        self._command_search_paths = []

    @property
    def version(self):
        return self._version

    @version.setter
    def version(self, v):
        self._version = v

    @property
    def driver_executable(self):
        return self._driver

    @property
    def platformed_name(self):
        return self._platform + '_' + self.name

    @property
    def tools(self):
        """
        Get the tools this toolchain uses, as a mapping of
        variable name (e.g. "CC", "LD") to Node or string.
        :rtype: Dict[str, Node|str]
        """
        return self._tools

    @property
    def command_search_paths(self):
        return self._command_search_paths

    def tool_var(self, name):
        """
        Create a variable for the tool_var 'name'
        :param name:
        :return:
        """
        return Variable(name, self._tools.get(name, 'not-found-' + name))

    @classmethod
    def register_toolchain_class(cls, tc_cls):
        cls._toolchain_classes.append(tc_cls)

    @classmethod
    def toolchain_classes(cls):
        """
        Get all the known toolchain classes.
        :rtype: list[Toolchain]
        """
        return list(cls._toolchain_classes)

    @staticmethod
    def find_tools_matching(pattern_text):
        return PathCache.instance().which_match(pattern_text)

    @classmethod
    def find_driver_path(cls, driver_or_path):
        """
        Given a filename or full path and return the full path.
        :param driver_or_path: bare filename or path
        :return: None or full path
        """
        # verify that the executable exists and/or is a valid full path
        path = PathCache.instance().which(driver_or_path)
        if not path:
            debug("did not find '{}' by full path or on PATH".format(driver_or_path))
            return None

        return path

    @classmethod
    @abstractmethod
    def discover_toolchain(cls, driver_or_path, platform, preferred=False):
        """
        Discover whether a particular toolchain exists, given a primary "driver" executable or path.
        This should register the toolchain on success.
        :param driver_or_path: the driver executable to check (e.g. "gcc", "clang-5.0"), possibly as a path
        :param platform: build platform
        :param preferred: if set, use this as the preferred generic tool if detected
        :rtype: Toolchain
        :return: the registered toolchain or None
        """

    @abstractmethod
    def detect(self):
        """
        Detect the tools for this toolchain's version in the machine or environment.

        This should populate self._tools but only populate missing entries.

        This method tell whether the receiver is available
        for real builds.  Even if this method fails, however,
        the receiver may still be used, e.g. for unit tests.
        :rtype: str for error or None for detected
        """
        raise NotImplementedError

    @classmethod
    def _may_target_toolchain(cls, toolchain):
        """
        When called before the build_env has been initialized,
        see if we may target a given toolchain.  In other words, we
        do a negative check that the user has _not_ provided `-t other-toolchain`.
        :param toolchain: base name of toolchain
        :return: True if the build spec does not specifically target a different toolchain
        """
        spec = runtime_values.get(SPEC)
        if spec:
            # i.e., we've seen `-t`
            try:
                build_spec = BuildSpec(spec, no_defaults=True, ignore_missing=True)
                if build_spec.toolchain is not None and toolchain not in build_spec.toolchain:
                    # the user is not targeting this one
                    return False

            except BuildException:
                # can't parse -- be generous (let the main driver complain)
                return True

        # it's either there or we don't know yet
        return True

    @classmethod
    @abstractmethod
    def can_user_override(cls):
        """
        Tell whether we should consider this class when processing a user CC/CXX override.
        :return:
        """
        raise NotImplementedError

    @staticmethod
    def get_user_override(var):
        # check command-line args
        user = runtime_values.get(var)
        if not user:
            user = os.getenv(var)
        return user

    @classmethod
    def invoke_tool_or_script(cls, args, **kwargs):
        """
        This is intended to emulate the way "make" works, where on Windows, we
        can invoke a bash script if needed.
        :param args: program and arguments
        :return: subprocess.Completion
        """
        try:
            # do fast path for normal cases of actual executables
            comp = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
        except OSError:
            if HOSTOS == OS_WIN:
                # assume the program is a script and try again
                sh_exe = os.path.join(MSYS_DIR, "sh.exe")
                sh_cmdline = utils.format_command_line(["/bin/sh"] + args).replace('\\', '/')
                sh_args = [sh_exe, "-c", sh_cmdline]
                comp = subprocess.run(sh_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
            else:
                raise

        return comp

    def register_tool_based_on_driver(self, var, candidates, fallback=None):
        if var in self._tools:
            # don't overwrite
            return None

        user_override = self.get_user_override(var)
        if user_override:
            candidates = [self.find_driver_path(user_override) or user_override]
        else:
            candidates = list(candidates)
            uniquify(candidates)

        for candidate in candidates:
            path = self.locate_tool_near_driver(candidate)
            if path:
                # canonicalize as Unix-like program path
                path = path.replace("\\", "/")
                self.register_tool(var, path)
                return None

        # how did we come across this name?
        if user_override:
            how = "specified in user override"
            if not os.path.dirname(user_override):
                # we don't search PATH unless it's a bare filename
                how += " on PATH"
        elif self._driver in candidates:
            how = "on PATH"
        else:
            how = "next to '{}' or on PATH".format(self._driver)

        err = "did not find '{}' tool {} {}".format(
            var, " or ".join("'{}'".format(candidate) for candidate in candidates), how)

        if fallback and fallback != candidate:
            # not fatal
            diags.warn(err + "; assuming '{}' will work\n(else pass {}=... to explicitly locate it)".format(fallback, var))
            self.register_tool(var, fallback)
            return None

        return err

    def locate_tool_near_driver(self, file):
        """
        Look for an executable near the driver (i.e. in the same directory) named @name
        :param file: expected executable name
        :return: path or None
        """
        if self._driver:
            cand = os.path.join(os.path.split(self._driver)[0], file)
            path = PathCache.instance().which(cand)
            if path:
                return path

        # not in same dir, so, check PATH
        path = PathCache.instance().which(file)
        return path

    def register_tool(self, var, path):
        # canonicalize for Unix-like program path
        path = os.path.normpath(path).replace('\\', '/')

        self._tools[var] = path

    @property
    def versioned_name(self):
        if self._version and self._version not in self._name:
            return self._name + '-' + self._version
        return self._name

    @abstractmethod
    def get_unique_artifacts(self, proj):
        """
        Get project artifacts specific to a toolchain.
        :type proj: projects.Project
        :return: List of project artifacts for the toolchain.
        :rtype: List[build_model.Path]
        """
        raise NotImplementedError()

    @abstractmethod
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
        raise NotImplementedError()

    @abstractmethod
    def create_compiler_commands(self, env, proj, kind, source_path, obj_path):
        """
        Create command(s) that generate the object file for a C/C++
        file in the given project along with generating dependency
        rules for future builds.
        :type env: build_env
        :type proj: build_model.Project
        :type kind: kinds._base.Kind
        :type source_path: build_model.Path
        :type obj_path: build_model.Path
        :rtype: List[build_model.Command]
        """
        raise NotImplementedError()

    @abstractmethod
    def create_program_link_commands(self, model):
        """
        Create command(s) that generate an executable file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        raise NotImplementedError()

    def create_shared_link_commands(self, model):
        """
        Create command(s) that generate an shared object file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        raise NotImplementedError()

    def create_static_link_commands(self, model):
        """
        Create command(s) that generate a static library file for a
        given list of object files, static libraries, and shared libraries.
        :type model: kinds._base.LinkModel
        :rtype: List[build_model.Command]
        """
        raise NotImplementedError()

    def generate_extra_rules(self, env, proj):
        """
        Override if a particular kind produces other outputs
        which need their own rules.
        :type env: build_env
        :type proj: projects.Project
         :rtype: List[Rule]
        """
        return []

    def _friendly_name(self, path):
        return os.path.split(str(path))[1]

    def _variable_arg(self, proj, basename, value):
        """
        Create a variable named <project name>_<name>
        encompassing options in @value.
        :param proj: project target
        :param basename: base name of variable
        :param value: list of options
        """
        return Variable(proj.name_var() + '_' + basename, Command(value))

    def _make_response_file_as_needed(self, project, cwd, out_path, args):
        """
        Package a long list of arguments into a response file.
        The @project holds the response file as a generated text file.
        The @cwd is where the command is expected to run.
        @out_path is the build artifact used as a base for the response file.
        @args are the incoming args to bake into the response file if needed.
        :type project: projects.Project
        :type cwd: build_model.Path
        :type args: List[Node]
        :type out_path: build_model.Path
        """
        text = ""

        def expand(node_or_str):
            x = node_or_str.resolve() if isinstance(node_or_str, Node) else str(node_or_str)
            return x

        out_args = []

        full_resp_path = out_path.change_extension(".args", append=True)

        def responsify(arglist):
            # expand each element to raw text and place each on a new line
            nonlocal text
            for node in arglist:
                exp = expand(node)
                if text:
                    text += '\n'
                text += exp

        # TODO: reenable this when there's time to expand build variables
        # like CPPFLAGS, CXXFLAGS, LDFLAGS without losing that reference

        # # first, extract any troublesome arguments that `make` on Windows
        # # will mangle all to ?&!*&#$
        # i = 0
        # while i < len(args):
        #     exp = expand(args[i])
        #     if '"' in exp:
        #         responsify([exp])
        #         args[i:i+1] = []
        #     else:
        #         i += 1

        # then take any very long sequence of args into a response file
        # -- DTOOLS-1708 involves bugs in MinGW make/sh which encounter
        # a bug around char 16000, so stop well short of that
        chunks = split_by_length(args, max_cmd_line=10000)

        # was anything response-able because of weird arguments yet,
        # or is the command line already known to be too long?
        if not text and len(chunks) == 1:
            return False

        # use the response file (full path since we don't know what the caller uses as cwd)
        out_args.append(Composition("@", full_resp_path))

        # break up chunks into the response file
        for chunk in chunks:
            responsify(chunk)

        args[:] = out_args

        project.generated_files[full_resp_path] = text
        return True


def version_compare(a, b):
    a_version_bits = a.split(r'.')
    b_version_bits = b.split(r'.')

    # compare major/minor/build by element
    for index in range(min(len(a_version_bits), len(b_version_bits))):
        try:
            diff = int(a_version_bits[index]) - int(b_version_bits[index])
            if diff < 0:
                return -1
            if diff > 0:
                return 1
        except ValueError:
            break

    # if all was equal (or nonnumeric), prefer longer version
    diff = len(a_version_bits) - len(b_version_bits)
    if diff < 0:
        return -1
    if diff > 0:
        return 1

    # finally, just compare strings
    if a < b:
        return -1
    if a > b:
        return 1
    return 0


Toolchain.init_class()


