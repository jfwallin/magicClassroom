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
Provides the model for the build, including
the build specification, the notion of configurations,
generic options, targets, toolchains, architectures, etc.
"""
import importlib
import os
import re
import textwrap
from abc import ABCMeta, abstractmethod

import defaults
import diags
import run_vars
from config import PROJ_EXT, PACKAGE_EXT, HOSTOS, OS_OSX, OS_WIN, OS_LINUX
from diags import debug
from utils import unix_path, host_path


class BuildException(Exception):
    """
    Exception used for build model-specific
    problems.
    """
    pass


class BuildObject(object):
    def __init__(self, name):
        self._name = name

    def __str__(self):
        return self.__class__.__name__ +": " + self._name

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, name):
        self._name = name

    # define these fields here for code completion;
    # the actual copies are class-specific instances

    _ents = {}
    # Maps a specific name to an instance
    # type: Dict(str, T <= BuildObject)

    _alias = {}
    # Maps aliases to other aliases or specific names
    # type: Dict(str, str)

    @classmethod
    def init_class(cls):
        # each class has a unique copy
        cls._ents = {}

        cls._alias = {}
        # :type: Dict(str,Dict(str,str))

    @classmethod
    def _iterate_modules_in_package(cls, pkg, cb):
        """
        Iterate the given package @pkg, load all the modules
        present, and invoke @cb.
        :param pkg: a build object package
        :type pkg: module
        :param cb: callback, taking a module name and full module path
        """
        paths = pkg.__path__
        pkg_name = pkg.__name__
        for path in paths:
            ents = os.listdir(path)
            for ent in ents:
                name, ext = os.path.splitext(ent)
                if not ent[0] in "._" and ext.startswith('.py'):
                    cb(name, pkg_name + '.' + name)

    @classmethod
    def register_directory(cls, pkg):
        """
        Iterate the given package @pkg, load all the modules
        present, and register their contents as instances of @cls.
        :param cls:
        :param pkg:
        :type pkg: module
        """

        def register_objects(name, pkg_path):
            pre_defs = cls.all()
            mod = importlib.import_module(pkg_path)

            if hasattr(mod, 'Handler'):
                # easiest path is to define Handler(...) class.
                handler_cls = getattr(mod, 'Handler')
                instance = handler_cls(name)
                cls.register(instance)
            elif hasattr(mod, 'discover'):
                # module can discover its own instances and register them
                handler_func = getattr(mod, 'discover')
                handler_func()
                post_defs = cls.all()
                if pre_defs == post_defs:
                    diags.warn(
                        "ignoring {0} {1} ('discover()' did not make any {0}.register() or {0}.alias() calls)".format(
                            cls.__name__, name))
                # debug("registered {0}: {1}".format(cls.__name__, name))
            else:
                # otherwise, clients must invoke code manually.
                post_defs = cls.all()
                if pre_defs == post_defs:
                    diags.warn(
                        "ignoring {0} {1} (no 'Handler' class defined and no {0}.register() or {0}.alias() calls made)".format(
                            cls.__name__, name))
                # debug("registered {0}: {1}".format(cls.__name__, name))

        cls._iterate_modules_in_package(pkg, register_objects)

    @classmethod
    def default_platform(cls, platform):
        """
        Get the default platform for the purpose of distinguishing
        host and device aliases or tools.
        :param platform: None, 'host', 'device', or any host or device platform
        :return: canonical host or device
        """
        if not platform or platform == 'host' or platform in [OS_WIN, OS_OSX, OS_LINUX]:
            return HOSTOS

        # resolve device platform
        from platforms._base import Platform
        plat = Platform.find(platform)
        if plat:
            return plat.name

        return platform

    @classmethod
    def names(cls):
        """ Get the names of all proper build objects """
        return cls._ents.keys()

    @classmethod
    def all(cls, platform=None):
        """ Get the names of all build objects and aliases """
        # take the name of the build object, not the key,
        # which may be prefixed with "<platform>_"
        platform = cls.default_platform(platform)
        objs = list(bo.name for bo in cls._ents.values())
        platform_keys = cls._alias.get(platform, {})
        return objs + list(platform_keys.keys()) + list(platform_keys.values())

    @classmethod
    def register(cls, bo):
        """ Register a new instance of a build object.
        An object of the same name must not already be registered. """
        assert issubclass(type(bo), cls)
        try:
            name = bo.platformed_name
        except AttributeError:
            name = bo.name
        assert name not in cls._ents
        debug("Registering {0} named {1}".format(cls.__name__, name))
        cls._ents[name] = bo

    @classmethod
    def remove(cls, name_or_key):
        """ Remove any object or alias with the given name """
        try:
            name_or_key = name_or_key.platformed_name
        except AttributeError:
            try:
                name_or_key = name_or_key.name
            except AttributeError:
                pass
        del cls._ents[name_or_key]

    @classmethod
    def register_alias(cls, from_, to, platform=None):
        """
        Register an alias.
        :param from_: the base name of a thing
        :param to: the substitute name for that thing
        :param platform: the platform that the replacement targets
        """
        platform = cls.default_platform(platform)
        debug("Registering {0} alias {1} for {2} on {3}".format(cls.__name__, from_, to, platform))
        cls._alias.setdefault(platform, {})[from_] = to

    @classmethod
    def aliases(cls):
        """ Get the all the aliases
        :rtype: Dict[str,str]
        """
        return dict(cls._alias)

    @classmethod
    def find(cls, name, platform=None):
        platform = cls.default_platform(platform)
        aliases = cls._alias.get(platform, {})
        while name in aliases:
            name = aliases[name]
        if name in cls._ents:
            return cls._ents[name]

        return None


class Node(object):
    """
    This is the base class for content which comprises
    the generated build file (makefiles, here).
    """
    __metaclass__ = ABCMeta

    def __repr__(self):
        return self.value()

    def _ensure_node(self, seg):
        if isinstance(seg, Node):
            return seg
        # wrap it
        return Literal(seg)

    def makefile_str(self):
        """
        Expand the receiver for this project to the value as
        it appears in a Makefile.
        :rtype: string
        """
        return self.value()

    @abstractmethod
    def value(self):
        """
        Expand the receiver for this project to a full string.
        It depends on context which version you want.

        Note: str(Node) may be an acceptable alternative, since it
        represents the "pre-expanded" value.
        :rtype: string
        """

    @abstractmethod
    def resolve(self):
        """
        Flatten the receiver to a full string without variables.
        :rtype: string
        """

    @abstractmethod
    def visit(self, visitor):
        raise NotImplementedError()


class Buildable(Node):
    """
    A target of a build.  Targets are usually files.
    """
    def __init__(self, path, no_escape=False):
        """
        :param path:
         :type path: str | Node
        """
        Node.__init__(self)
        self._path = isinstance(path, Node) and path or Literal(path)
        self._no_escape = no_escape

    def __repr__(self):
        return str(self._path)

    @property
    def path(self):
        return self._path

    def visit(self, visitor):
        if isinstance(self._path, Node):
            return self._path.visit(visitor)
        return self

    def makefile_str(self):
        return str(self._path) if self._no_escape else self._path.makefile_str()


class Rule(Node):
    """
    A build rule for a project.  Semantically it represents the
    way a main Buildable (and zero or more auxiliary Buildables)
    are created based on changes to dependent Buildable.

    Ordered dependencies are used to ensure build order, when e.g.
    extension makefiles add additional dependencies that are not
    otherwise known to make.

    The Commands are used to construct the Buildable.
    """
    def __init__(self, proj, buildable, dependencies=[], order_dependencies=[]):
        """
        :param proj: a Project
        :param buildable: a Buildable
        :param dependencies: List[Buildable]
        :param order_dependencies: List[Buildable]
        """
        Node.__init__(self)
        from projects import Project
        assert not proj or isinstance(proj, Project)
        assert isinstance(buildable, Buildable)
        self._project = proj
        self._buildable = buildable
        self._buildables = []
        self._dependencies = dependencies or []
        self._order_only_dependencies = order_dependencies or []
        self._commands = []
        self._multi_target = False

    def __repr__(self):
        hdr = str(self._buildable) + \
              " ".join([str(x) for x in self._buildables]) + " <- " + \
              ", ".join([str(dep) for dep in self._dependencies]) + \
              " | " + " , ".join([str(dep) for dep in self._dependencies])
        return hdr + '\n\t'.join([''] + [''] + [str(cmd) for cmd in self._commands] + [''])

    @property
    def project(self):
        """ The project that owns this rule.
        :rtype: build_model.Project """
        return self._project

    @property
    def buildable(self):
        """ The primary buildable constructed by this rule.
        :rtype: build_model.Buildable """
        return self._buildable

    def add_aux_buildable(self, buildable):
        """ Auxiliary buildables constructed by this rule.
        :type buildable: build_model.Buildable
        """
        assert isinstance(buildable,Buildable)
        self._buildables.append(buildable)

    @property
    def aux_buildables(self):
        """ Auxiliary buildables constructed by this rule.
        :rtype: List[build_model.Buildable]
        """
        return self._buildables

    @property
    def dependencies(self):
        """ :rtype: List[build_model.Buildable] """
        return self._dependencies

    @property
    def order_only_dependencies(self):
        """ :rtype: List[build_model.Buildable] """
        return self._order_only_dependencies

    @property
    def commands(self):
        """ :rtype: List[build_model.Command] """
        return self._commands

    def add_command(self, command):
        """
        :type command: build_model.Command
        """
        if command:
            self._commands.append(command)
            command.rule = self

    def add_commands(self, commands):
        """
        :type commands: List[build_model.Command]
        """
        for command in commands:
            self.add_command(command)

    @property
    def multi_target(self):
        return self._multi_target

    @multi_target.setter
    def multi_target(self, v):
        self._multi_target = v

    def makefile_str(self):
        lines = []

        target_str = self.buildable.path.makefile_str()
        aux_str = ' '.join([aux.path.makefile_str() for aux in self.aux_buildables])
        dep_str = ' '.join([dep.makefile_str() for dep in self.dependencies])

        # If a rule generates more than one output, and both are
        # listed (e.g., there are aux_buildables), we need to
        # force the rule to be a pattern rule, so that make
        # will generate commands only once for the set of outputs.
        # Without the pattern, the rule is multiplexed,
        # leading to multiple e.g. link attempts on the
        # same file in different threads.
        name = self._project and self._project.name
        if aux_str:
            non_variable_refs = re.compile(r'(?<!\$\()' + re.escape(Path(name).makefile_str()))
            target_str = non_variable_refs.sub('%', target_str)
            aux_str = non_variable_refs.sub('%', aux_str)

        if self._project:
            # always depend on the project file itself to force rebuilds
            # when the project file changes
            dep_str += ' '
            proj_path = Path(self._project.base_path(), self._project.name +
                             (PACKAGE_EXT if self._project.is_package() else PROJ_EXT))

            dep_str += proj_path.makefile_str()
            external_deps = self._project.get_all_external_dependencies()

            for dep in external_deps:
                dep_str += " {}".format(dep.makefile_str())

        ordered_dep_str = ''
        if self.order_only_dependencies:
            ordered_dep_str += ' | ' + (' '.join([dep.makefile_str() for dep in self.order_only_dependencies]))

        lines.append(target_str + ' ' + aux_str + (':: ' if self._multi_target else ': ') + dep_str + ordered_dep_str)

        for command in self.commands:
            if isinstance(command, InfoCommand):
                s = '\t$(INFO) '
            else:
                s = '\t$(ECHO) '
            s += command.makefile_str()
            lines.append(s)

        return '\n'.join(lines) + '\n'

    def visit(self, visitor):
        ret = visitor(self)
        if ret is None:
            return
        self._buildable.visit(visitor)
        for dep in self._dependencies:
            dep.visit(visitor)
        for command in self._commands:
            command.visit(visitor)
        return ret


class Literal(Node):
    """
    This is a literal string.
    """

    # pattern describes characters not already escaped with \
    UNSAFE_PATTERN=re.compile(r'(?!<\\)([ ()";|\[\]])')
    # pattern describes dollar signs not used as variable expansions
    DOLLAR_PATTERN=re.compile(r'(\$(?!\())')
    # pattern finds variable expansions
    VARIABLE_PATTERN=re.compile(r'\$\$\\\(([^)]+)(\\\))')

    def __init__(self, value):
        Node.__init__(self)
        self._value = str(value)

    def __eq__(self, other):
        return isinstance(other, Literal) and self._value == other._value

    def __hash__(self):
        return hash(self._value)

    def __lt__(self, other):
        return self._value < str(other)

    def __bool__(self):
        return len(self._value) > 0

    def value(self):
        """
        Expand the receiver for this project to a full string.
        :rtype: string
        """
        return self._value

    def resolve(self):
        return self._value

    def makefile_str(self):
        s = str(self)

        # escape dollars
        s = s.replace('$', '$$')

        # escape "unsafe" chars that a shell may get freaked out by
        s = Literal.UNSAFE_PATTERN.sub(r'\\\1', s)

        # after this, variable substitutions are like $$\(MLSDK\) -- fix those
        s = Literal.VARIABLE_PATTERN.sub(r'$(\1)', s)
        return s

    def visit(self, visitor):
        return visitor(self)


class PathLiteral(Literal):
    """
    This is a literal string appearing in a Path.
    """
    def __init__(self, value):
        Literal.__init__(self, value)

    def makefile_str(self):
        s = str(self)
        s = s.replace('$', '$$')
        s = s.replace('\\', '/')
        s = Literal.UNSAFE_PATTERN.sub(r'\\\1', s)
        return s


class Variable(Node):
    """
    This represents a variable whose value known at generation time.

    The value might be an Node or a string on input but is always
    stored as a Node.
    """
    def __init__(self, name, value):
        Node.__init__(self)
        self._name = name
        self._value = self._ensure_node(value) if value is not None else None

    def __repr__(self):
        return "$(" + self._name + ")"

    def __str__(self):
        return repr(self)

    def __eq__(self, other):
        return isinstance(other, Variable) and self._name == other.name and self._value == other._value

    def __hash__(self):
        return hash(tuple([self._name, self._value]))

    def __lt__(self, other):
        return isinstance(other, Variable) and \
               (self._name < other.name
                or (self._name == other.name and self._value < other._value))

    @property
    def name(self):
        return self._name

    def value(self):
        if self._value is None:
            diags.warn("Variable '{0}' is undefined.\n"
                       "Ensure it is spelled correctly or pass it as "
                       "'{0}=...value...' on the command line.".format(self._name))
            return str(self)
        return self._value.value()

    @property
    def raw_value(self):
        return self._value

    @raw_value.setter
    def raw_value(self, val):
        self._value = val

    def makefile_str(self):
        # don't expand self as an expansion of itself
        if isinstance(self._value, Variable) and self._value.name == self._name:
            return self.value()

        s = self._value.makefile_str() if self._value else ''
        return s

    def resolve(self):
        if self._value is None:
            return ''
        return self._value.resolve()

    def visit(self, visitor):
        ret = visitor(self)
        if ret is None:
            return
        if self._value:
            self._value.visit(visitor)
        return ret


class EnvironmentVariable(Node):
    """
    This represents a variable whose value is fetched from the
    environment.
    """
    def __init__(self, name):
        Node.__init__(self)
        self._name = name

    def __repr__(self):
        return "$(" + self._name + ")"

    def __str__(self):
        return repr(self)

    def __eq__(self, other):
        return isinstance(other, EnvironmentVariable) and self._name == other._name

    def __hash__(self):
        return hash(self._name)

    @property
    def name(self):
        return self._name

    def value(self):
        return os.getenv(self._name, repr(self))

    @property
    def raw_value(self):
        return self.value()

    def visit(self, visitor):
        return visitor(self)

    def makefile_str(self):
        s = self.value()
        s = s.replace('\\', '/')
        s = Literal.UNSAFE_PATTERN.sub(r'\\\1', s)
        return s


class Composition(Node):
    """ Represents a composition of other strings or Expandables. """
    def __init__(self, *components, sep=''):
        Node.__init__(self)
        self._sep = sep
        self._segments = [self._ensure_node(node) for node in components]

    def __repr__(self):
        return self._sep.join([repr(s) for s in self._segments])

    def __hash__(self):
        return hash(tuple(self._segments))

    def __lt__(self, other):
        if isinstance(other, Composition):
            for a, b in zip(self._segments, other._segments):
                if a == b:
                    continue
                if a < b:
                    return True
            return False
        return str(self) < str(other)

    def __eq__(self, other):
        if isinstance(other, Composition):
            return self._sep == other._sep and self._segments == other._segments
        return str(self) == str(other)

    def __len__(self):
        return len(self._segments)

    def _eval(self, ent):
        """
        :type ent: Node|str
        """
        try:
            return self._eval(ent.value())
        except AttributeError:
            return str(ent)

    @property
    def segments(self):
        return self._segments

    def value(self):
        exps = [ent.value() for ent in self._segments]
        path = self._sep.join(exps)
        return path

    def makefile_str(self):
        exps = [ent.makefile_str() for ent in self._segments]
        path = self._sep.join(exps)
        return path

    def _resolve(self, joiner):
        """
        Flatten the receiver to a full string without variables.
        :return: str
        """
        def resolve(v):
            try:
                return v.resolve()
            except:
                return v.value()

        exps = [resolve(ent) for ent in self._segments]
        return joiner.join(exps)

    def resolve(self):
        return self._resolve(self._sep)

    def visit(self, visitor):
        ret = visitor(self)
        if ret is None:
            return
        for seg in self._segments:
            seg.visit(visitor)
        return ret


class Path(Composition):
    """ Represents a path whose entries are canonically separated by '/',
    even on Windows, where we honor MinGW patterns.
    """
    def __init__(self, *segments, sep='/'):
        new_segs = []
        for seg in segments:
            if not isinstance(seg, Node):
                if '\\' in seg:
                    new_segs += seg.split('\\')
                elif '/' in seg:
                    new_segs += seg.split('/')
                elif seg or not new_segs:       # don't add empty segs in the middle; the first, though, means full path
                    new_segs.append(seg)
            else:
                new_segs.append(seg)

        Composition.__init__(self, *new_segs, sep=sep)

    def _ensure_node(self, seg):
        if isinstance(seg, Node):
            return seg
        # wrap it
        return PathLiteral(seg)

    @staticmethod
    def parse(path):
        """
        Parse a path from a string.
        :rtype: Path
        """
        if isinstance(path, Path):
            return path

        if '\\' in path:
            segs = path.split('\\')
        else:
            segs = path.split('/')

        # convert variable-looking things into Variables
        def convert(seg):
            open_pos = seg.find('$(')
            close_pos = seg.find(')', open_pos+2)
            if open_pos >= 0 and close_pos >= 0:

                def varof(var):
                    # get a well-known variable if possible, else,
                    # leave an undefined Variable.
                    return Variable(var, run_vars.runtime_values.get(var))

                # base case
                if open_pos == 0 and close_pos == len(seg) - 1:
                    name = seg[open_pos+2:close_pos]
                    return varof(name)

                # more complex -- alternates between plain tet and variables

                # split around '('
                pieces = re.split(r'\$\(|\)', seg)
                cur = None
                idx = 0
                while idx < len(pieces):
                    if pieces[idx]:
                        if (idx % 2) == 0:
                            piece = Literal(pieces[idx])
                        else:
                            piece = varof(pieces[idx])
                        if not cur:
                            cur = piece
                        elif not isinstance(cur, Composition):
                            cur = Composition(cur, piece)
                        else:
                            cur = Composition(*cur.segments, piece)
                    idx += 1
                return cur

            return seg

        cvt_segs = [convert(seg) for seg in segs]

        path_obj = Path(*cvt_segs)
        # preserve any directory path
        if path and path[-1] in ('/', '\\'):
            path_obj.segments.append(Literal(''))

        return path_obj

    @property
    def segments(self):
        return self._segments

    def is_absolute(self):
        """
        Deduce whether the receiver is absolute.
        For now, assume that a path whose first segment is a Variable,
        an empty string, or a Windows drive or UNC path are absolute.
        :rtype: bool
        """
        if not self._segments:
            return False

        first = self._segments[0]
        if not first:
            # Unix absolute path
            return True

        if isinstance(first, Variable):
            # variable: check its value -- if undefined, assume full path
            return not first._value or os.path.isabs(first.resolve())

        if isinstance(first, Path):
            return first.is_absolute()

        first_str = str(first)
        if first_str.startswith('//') or first_str[-1] == ':':
            return True

        return isinstance(first, Variable) or isinstance(first, EnvironmentVariable)

    def has_parent(self):
        """
        Tell if the receiver has a '..' segment
        """
        for seg in self._segments:
            if str(seg) == '..':
                return True
        return False

    @staticmethod
    def _base(s):
        if s.endswith('/') or s.endswith('\\'):
            s = s[:-1]
        return s

    def __repr__(self):
        exps = [self._base(repr(ent)) for ent in self._segments]
        path = self._sep.join(exps)
        return unix_path(path)

    def value(self):
        exps = [self._base(ent.value()) for ent in self._segments]
        # remove any blank entries, except the first
        exps = exps[0:1] + [exp for exp in exps[1:] if len(exp)]
        path = self._sep.join(exps)
        return unix_path(path)

    def resolve(self):
        """
        Flatten the receiver to a full path without variables.
        :return: str
        """
        def resolve(v):
            return v.value()

        exps = [resolve(ent) for ent in self._segments]
        path = self._sep.join(exps)
        return unix_path(path)

    def makefile_str(self):
        def mk(ent):
            if isinstance(ent, Variable):
                return str(ent)
            return ent.makefile_str()

        exps = [self._base(mk(ent)) for ent in self._segments]
        path = self._sep.join(exps)
        return path

    @property
    def filename(self):
        """
        Return the final segment, or the filename.
        :return: Node
        """
        return self._segments[-1]

    @filename.setter
    def filename(self, name):
        """
        Set the final segment, or the filename.
        :param: name Node|str
        """
        self._segments[-1] = self._ensure_node(name)

    @property
    def directory(self):
        """
        Return the leading segments, or the directory
        :return: Path
        """
        if len(self._segments) > 1:
            end = self._segments[-1]
            if isinstance(end, Composition) and len(end.segments) > 1:
                return Path(*self._segments[0:-1], end.directory, sep=self._sep)
            else:
                return Path(*self._segments[0:-1], sep=self._sep)
        return None

    def change_extension(self, new_ext, append=False):
        """
        Return a new Path with @new_ext in place or added to the existing
        extension (or appended to an extensionless name).
        :param new_ext: '.foo', '', etc.
        :return: Path
        """
        segs = list(self._segments)
        if len(segs) > 0:
            base, ext =  os.path.splitext(str(segs[-1]))
            segs[-1] = base + (ext + new_ext if append else new_ext)
        return Path(*segs)

    def change_filename(self, new_name):
        """
        Return a new Path with @new_name in place of the existing
        filename.
        :param new_name
        :return: Path
        """
        segs = list(self._segments)
        if len(segs) > 0:
            segs[-1] = self._ensure_node(new_name)
        return Path(*segs, sep=self._sep)

    @staticmethod
    def join(directory, suffix):
        """
        Return a new Path with directory combined with a suffix path,
        where empty directories are ignored, and a full-path suffix
        replaces the directory entirely.

        :param directory: directory, possibly None, representing a Path
        :type directory: Path
        :param suffix: Path for the suffix
        :type suffix: Path|str
        :return: Path
        """
        suffix_path = Path.parse(suffix)
        if not directory or suffix_path.is_absolute():
            return suffix_path

        return Path(directory, suffix_path)

    def relative_to(self, other_path):
        """
        Return a path where the receiver does not include other_path as a
        prefix.  (The assumption is, they are closely related -- this is
        to avoid writing a generic "relative_to" method.)
        :param other_path:
        :return: suffix of receiver excluding other_path
        """
        if isinstance(self._segments[0], Variable) and isinstance(other_path, Variable):
            other_value = other_path.value()
            seg0_value = self._segments[0].value()
            rel = os.path.relpath(other_value, seg0_value)
            if rel != '.':
                return Path(rel, *self._segments[1:], sep=self._sep)
            else:
                return Path(*self._segments[1:], sep=self._sep)
        raise AssertionError("expected simpler path")


class Command(Composition):
    """
    This represents a command to invoke as part of invoking a rule.
    """
    def __init__(self, args):
        """
        :param args: List[str | Variable]
        """
        Composition.__init__(self, *args, sep=' ')
        self._rule = None
        self._winargs = False
        self._cwd = None
        self._search_paths = []

    @property
    def winargs(self):
        return self._winargs

    @winargs.setter
    def winargs(self, flag):
        self._winargs = flag

    @property
    def cwd(self):
        """
        Get the desired current working directory when invoking this command.
        :rtype: Path
        """
        return self._cwd

    @property
    def search_paths(self):
        return self._search_paths

    @cwd.setter
    def cwd(self, cwd):
        """
        Set the desired current working directory when invoking this command.
        If not set, the directory is unchanged when running the command.
        If set, the directory is changed only when running the command.
        :type cwd: Path
        """
        self._cwd = cwd

    @search_paths.setter
    def search_paths(self, search_paths):
        self._search_paths = search_paths

    def makefile_str(self):
        # we don't expand variables inside commands
        def wrap(x):
            s = isinstance(x, Variable) and str(x) or x.makefile_str()

            if self._winargs and os.path.isabs(s):
                s = host_path(s)

            return s

        segs = self._segments

        if self.search_paths:
            if HOSTOS == OS_WIN:
                segs.insert(0, Literal("PATH={}".format(
                    ";".join(self.search_paths + ['%PATH%']))))
            else:
                segs.insert(0, Literal("{0}={1}:${0}".format(
                    "DYLD_LIBRARY_PATH" if HOSTOS == OS_OSX else "LD_LIBRARY_PATH",
                    ":".join(self.search_paths))))

        if self.cwd:
            segs = [ Literal('cd'), self._cwd, Literal('&&') ] + segs

        return self._sep.join([wrap(x) for x in segs])

    @property
    def rule(self):
        """ The rule owning this Command.
        :rtype: Rule """
        return self._rule

    @rule.setter
    def rule(self, rule):
        """
        :type rule: build_model.Rule
        """
        self._rule = rule

    @property
    def args(self):
        """ :rtype: str | Variable """
        return [str(seg) for seg in self._segments]


class InfoCommand(Command):
    """
    This represents a command to print information
    """
    def __init__(self, message):
        """
        :param message: str
        """
        Command.__init__(self, [message])


class Directive(Composition):
    """
    A directive rule for a Makefile.
    """
    def __init__(self, *words):
        Composition.__init__(self, *words)

    def value(self):
        return self.makefile_str()

    def makefile_str(self):
        # we don't expand variables inside commands,
        # but must escape literal words with spaces inside

        tokens = []

        def wrap(x):
            s = x.makefile_str()

            tokens.append(s)
            return None

        for node in self._segments:
            node.visit(wrap)
        return " ".join(tokens)

    def resolve(self):
        return self._resolve(" ")


class Include(Directive):
    """
    Represents an include directive, specifically.
    """
    def __init__(self, *words):
        Directive.__init__(self, *words)


class Text(Node):
    """
    Raw text for a Makefile.
    """
    def __init__(self, text):
        Node.__init__(self)
        self._text = text

    def value(self):
        return self._text

    def makefile_str(self):
        return self._text

    def resolve(self):
        return self._text

    def visit(self, visitor):
        return visitor(self)


UNSAFE_PATTERN = re.compile(r'( |(?!<\$\()\(|(?!<\$\([^)]+)\))')


class VariableDefinition(Directive):
    """
    Represents an assignment of a name to a value.
    """
    def __init__(self, name, op, value, export=False):
        Directive.__init__(self, name, op, value)
        self._export = export

    def name(self):
        return self._segments[0]

    def op(self):
        return self._segments[1]

    def rhs(self):
        return self._segments[2]

    def makefile_str(self):
        # variable value must be expanded
        var = self.rhs()
        var_str = var.makefile_str()
        if isinstance(var, Variable) and var.name in var_str:
            # but, don't expand if we are FOO=$(FOO)
            var_str = Literal(var.value()).makefile_str()

        return (self._export and 'export ' or '') + self.name().value() + self.op().value() + var_str


class BuildSpec(object):
    """
    This specifies the build platform, as a configuration,
    toolchain, platform, and architecture.
    """
    def __init__(self, spec=None, no_defaults=False, ignore_missing=False, **kwargs):
        """
        :param spec: the specification string
        :param no_defaults: if true, don't fill in unspecified segments
        :param ignore_missing: don't error out on elements with no extant build model
        :param kwargs: dict of overrides for 'config', 'target', 'toolchain', or 'arch'
        """
        self._config = None
        # :type: Config
        self._target = None
        # :type: Platform
        self._toolchain = None
        # :type: Toolchain
        self._arch = None
        # :type: Architecture

        if spec is not None:
            self._parse(spec, no_defaults, ignore_missing)

        if 'config' in kwargs:
            self._config = kwargs['config']
        if 'target' in kwargs:
            self._target = kwargs['target']
        if 'toolchain' in kwargs:
            self._toolchain = kwargs['toolchain']
        if 'arch' in kwargs:
            self._toolchain = kwargs['arch']

    def _parse(self, spec, no_defaults, ignore_missing):
        from platforms._base import Platform
        from toolchains._base import Toolchain
        from architectures._base import Architecture
        import build_env

        def _no_duplicate(seg, cur, type):
            if not cur:
                return seg
            else:
                raise BuildException("multiple {2} segments at '{1}' in build specification '{0}'".
                                     format(spec, seg, type))

        def parse_segment(seg):
            if seg in build_env.configs():
                self._config = _no_duplicate(seg, self._config, "configuration")
            elif seg in Platform.all():
                self._target = _no_duplicate(seg, self._target, "platform")
            elif seg in Toolchain.all():
                self._toolchain = _no_duplicate(seg, self._toolchain, "toolchain")
            elif seg in Architecture.all():
                self._arch = _no_duplicate(seg, self._arch, "architecture")
            else:
                # see if we got a close-but-not-exact toolchain
                try:
                    idx = seg.index('-')
                    if Toolchain.find(seg[0:idx]):
                        self._toolchain = seg
                        return
                except ValueError:
                    pass

                if ignore_missing:
                    return

                # else, assume unknown config
                if not self._config:
                    self._config = seg
                else:
                    if spec == seg:
                        error_msg = "unknown target segment '{}'; " \
                                    "run with --help-target to see all valid target segments".format(spec)
                    else:
                        error_msg = "unexpected segment '{1}' in build target '{0}'; " \
                                    "run with --help-target to see all valid target segments".format(spec, seg)
                    raise BuildException(error_msg)

        if not spec:
            spec = ''
            segs = []
        else:
            segs = spec.split('_', 4)

        for seg in segs:
            parse_segment(seg)

        if no_defaults:
            return

        if not self._config:
            self._config = defaults.CONFIG_DEFAULT
            debug("Using configuration '{0}' as default in '{1}'".format(self._config, spec))

        if not self._target:
            self._target = defaults.TARGET_DEFAULT
            debug("Using platform '{0}' as default in '{1}'".format(self._target, spec))

        if not self._toolchain:
            if self._target in defaults.TOOLCHAIN_DEFAULT:
                name = defaults.TOOLCHAIN_DEFAULT[self._target]
            else:
                platform = Toolchain.default_platform(self._target)
                name = defaults.TOOLCHAIN_DEFAULT[platform]
            self._toolchain = name
            debug("Using toolchain '{0}' as default in '{1}'".format(self._toolchain, spec))

        if not self._arch:
            self._arch = defaults.arch_default(self._target)
            debug("Using architecture '{0}' as default in '{1}'".format(self._arch, spec))

    def __repr__(self):
        return '_'.join([str(x) for x in [self._config, self._target, self._toolchain, self._arch] if x])

    def __hash__(self):
        return hash(self._target) * 57 + (hash(self._arch) * 57 + (hash(self._config) * 57 + hash(self._toolchain)))

    def __eq__(self, other):
        return self._target == other._target \
            and self._config == other._config \
            and self._arch == other._arch \
            and self._toolchain == other._toolchain

    def check_valid(self):
        """
        Tell if all the pieces correspond to known elements
        :raises: BuildException if not
        """
        from toolchains._base import Toolchain
        import build_env

        if self._toolchain not in Toolchain.all():
            # look for likely suspect for better error
            try:
                idx = self._toolchain.index('-')
                if Toolchain.find(self._toolchain[0:idx]):
                    raise BuildException(
                        "toolchain '{0}' is not registered (in build specification '{1}'); "
                        "instead, try:\n{2}".format(self._toolchain, str(self),
                                                    textwrap.indent(dump_build_objects(toolchains=True),
                                                                    prefix='  ')))

            except ValueError:
                pass

        if self._config not in build_env.configs():
            raise BuildException("unrecognized build configuration '{0}'; be sure --path=... or MABUPATH "
                                 "point to {0}.config ".format(self._config))


    @property
    def platform(self):
        """
        :return: platform name
        :rtype: str
        """
        return self._target

    @property
    def arch(self):
        """
        :return: architecture name
        :rtype: str
        """
        return self._arch

    @property
    def config(self):
        """
        :return: configuration name
        :rtype: str
        """
        return self._config

    @property
    def toolchain(self):
        """
        :return: toolchain name
        :rtype: str
        """
        return self._toolchain

    def segments(self):
        """
        Return the number of segments defined.
        :return: int (0 to 4)
        """
        return (self._target and 1 or 0) + \
               (self._arch and 1 or 0) + \
               (self._config and 1 or 0) + \
               (self._toolchain and 1 or 0)

    def is_compatible(self, spec):
        """
        Tell whether the defined segments in the receiver and argument are compatible.

        This compares in a way that the receiver is the project's BuildSpec, and may be more
        than generic than the runtime build target @spec.  For example, to ask
        whether a setting derived from `lumin` (CXXFLAGS.lumin=...) is compatible with
        `debug_lumin_gcc-4.9_aarch64`.
        :rtype: bool
        """
        if self._toolchain and spec.toolchain:
            if self._toolchain != spec.toolchain:
                # toolchain names may be versioned
                self_and_vers = self._toolchain.split('-')  # e.g., 'msvc', '2015' or 'gcc', '4.8'
                other_and_vers = spec.toolchain.split('-')

                # compare names first
                if self_and_vers[0] != other_and_vers[0]:
                    return False

                # else match if versions are compatible in terms of major,
                # or, if the minor is newer
                if len(self_and_vers) >= 2 and len(other_and_vers) >= 2:
                    if self_and_vers[1] != other_and_vers[1]:
                        self_vers_parts = self_and_vers[1].split('.')  # e.g. '4', '8'  or '15', '7'
                        other_vers_parts = other_and_vers[1].split('.')
                        try:
                            # Do not inherit flags crossing major toolchain versions
                            # (we can't tell if the flag is intended for a specific version
                            # or not, and, each major version may have a distinct flag
                            # for a given feature); clients can use e.g.
                            # "CXXFLAGS.gcc-4,gcc-5,gcc-6..." to
                            # explicitly spell out the support for newer versions.
                            if int(self_vers_parts[0]) != int(other_vers_parts[0]):
                                return False
                        except ValueError:
                            return False

                        if len(self_vers_parts) >= 2:
                            try:
                                # Do consider compatible when comparing minor versions.
                                # The major version is the same, from above.
                                self_minor = int(self_vers_parts[1])
                                if len(other_vers_parts) >= 2:
                                    other_minor = int(other_vers_parts[1])
                                else:
                                    other_minor = 0
                                if self_minor > other_minor:
                                    return False
                            except ValueError:
                                return False

                elif len(self_and_vers) == 0 and len(other_and_vers) > 1:
                    # both don't have versions
                    pass
                elif len(self_and_vers) > 1 and len(other_and_vers) == 0:
                    # hmm, unexpected case
                    return False
                else:
                    # neither has a version, and the names matched above
                    pass

        if self._target and spec._target:
            if self._target != spec._target:
                return False

        if self._config and spec._config:
            if self._config != spec._config:
                return False

        if self._arch and spec._arch:
            if self._arch != spec._arch:
                return False

        return True

    def resolve_aliases(self, exclude_toolchain=False):
        """
        Convert any segments to registered items,
        returning a modified copy.
        :param exclude_toolchain: if set, don't resolve a toolchain
        (which is usually versioned)
        :rtype: BuildSpec
        """
        from platforms._base import Platform
        from toolchains._base import Toolchain
        from architectures._base import Architecture

        def resolve(value, cls, platform=None):
            if value:
                new = cls.find(value, platform)
                if new:
                    return new.name
            return value

        new = BuildSpec()

        new._config = self._config
        new._target = resolve(self._target, Platform)
        if not exclude_toolchain:
            new._toolchain = resolve(self._toolchain, Toolchain, new._target)
        else:
            new._toolchain = self._toolchain
        new._arch = resolve(self._arch, Architecture)

        return new


def dump_build_objects(configs=False, targets=False, toolchains=False, archs=False):
    from platforms._base import Platform
    from toolchains._base import Toolchain
    from architectures._base import Architecture

    def sort(lst):
        lst.sort()
        return lst

    def mappings(platform_map, host=HOSTOS):
        the_map = platform_map.get(host, {})
        ents = [ent + ' => ' + key for (ent, key) in the_map.items()]
        return sort(ents)

    if not (configs or targets or toolchains or archs):
        configs = targets = toolchains = archs = True

    import build_env
    s = ''
    indent = '  '
    if configs:
        s += 'Configurations: ' + ', '.join(sort(build_env.configs())) + '\n'
    if targets:
        s += 'Targets: ' + ', '.join(sort(list(Platform.names()))) + '\n'
        aliases = ', '.join(mappings(Platform.aliases()))
        if aliases:
            s += indent + 'aliases: ' + aliases + '\n'
    if toolchains:
        s += 'Toolchains:\n'
        import platforms
        for platform in [HOSTOS, platforms.lumin.NAME]:
            names = [name[len(platform)+1:] for name in Toolchain.names() if name.startswith(platform)]
            s += indent + 'for ' + platform + ': ' + ', '.join(sort(names)) + '\n'
            aliases = ', '.join(mappings(Toolchain.aliases(), platform))
            if aliases:
                s += indent + indent + platform + ' aliases: ' + aliases + '\n'

            s += indent + indent + 'detection status:\n'
            for name in names:
                tc = Toolchain.find(name, platform=platform)
                if not tc:
                    err = "not registered"
                else:
                    err = tc.detect()
                if not err:
                    s += indent + indent + indent + name + ': detected'
                else:
                    s += indent + indent + indent + name + ': ' + err
                s += '\n'

    if archs:
        s += 'Architectures: ' + ', '.join(sort(list(Architecture.names())))
    return s


def split_by_length(arguments, max_cmd_line=500):
    """
    Split a list of arguments, which will eventually be expanded into
    a command line, into chunks that do not exceed @max_cmd_line.
    :type arguments: List[Node]
    :param max_cmd_line: the maximum number of chars to allow (taking expansion into account)
    :return: List[List[Node]]
    """
    cur_cmd_line = 0

    args = []
    arg_batches = []

    # Generate batches of files to remove to avoid overrunning the max command line length.
    for arg in arguments:
        args.append(arg)
        exp = arg.resolve() if isinstance(arg, Node) else str(arg)
        cur_cmd_line += len(exp) + 1
        if cur_cmd_line >= max_cmd_line:
            arg_batches.append(args)
            args = []
            cur_cmd_line = 0

    # Add any remaining files as the final batch.
    if args:
        arg_batches.append(args)

    return arg_batches
