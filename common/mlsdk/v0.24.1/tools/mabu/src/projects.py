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
Parse projects into data structures
"""
import os, stat

import copy

import build_env
import build_vars
import kinds
from build_model import BuildException, Path, Variable, PathLiteral, BuildSpec, Literal, Composition, Node
from config import PROJ_EXT, PACKAGE_EXT, MANIFEST_FILE, DEFAULT_OUT_DIR, OPTION_EXT, CONFIG_EXT, COMP_EXT
from defaults import format_package_file
from diags import debug, error
from manifest import Manifest
from project_vars import *
from run_vars import MLSDK, runtime_values
from utils import unix_path, remove_duplicates, path_to_filename, mkdir_p

#
# Internal variable suffix marking a variable as being
# used only privately.  This is the inverse of "export ..." in source.
#
PRIVATE_SUFFIX = '.LOCAL'


def is_wild_source(src):
    return '*' in src or '?' in src or ('[' in src and ']' in src)


def _uniquify_to_end(lst):
    """
    Remove any duplicates in 'lst', favoring those at the end
    :param lst:
    :return: new list
    """
    rev = list(lst)
    rev.reverse()
    rev = _uniquify_to_start(rev)
    rev.reverse()
    return rev


def _uniquify_to_start(lst):
    """
    Remove any duplicates in 'lst', favoring those at the start
    :param lst:
    :return: new list
    """
    if len(lst) <= 1:
        return lst

    seen = set()
    out = []
    for item in lst:
        if item in seen:
            continue
        out.append(item)
        seen.add(item)
    return out


def _uniquify_options_to_start(lst):
    """
    Remove command line options in 'lst', favoring those at the start.
    Honor the notion that options ('-foo', '/O') may have arguments
    in succeeding locations.
    :param lst:
    :return: new list
    """
    if len(lst) <= 1:
        return lst

    seen = set()
    out = []
    cur = []

    def flush():
        if not cur:
            return
        opt = tuple(cur)
        cur.clear()
        if opt in seen:
            return
        out.append(opt)
        seen.add(opt)

    def add(it):
        if it[0] in ['-', '/']:
            flush()
        cur.append(it)

    for item in lst:
        add(item)

    flush()
    flat = [item for sublist in out for item in sublist]
    return flat


def _merge_setting(orig, add):
    if isinstance(orig, MultiVariable):
        orig = orig.derive(build_env.spec())
    if isinstance(add, MultiVariable):
        add = add.derive(build_env.spec())

    if not orig:
        return add
    if not add:
        return orig

    if isinstance(orig, MultiVariable):
        merged = copy.copy(orig)
        merged.update(add)
        return merged
    elif isinstance(orig, dict):
        merged = dict(add)
        merged.update(orig)
        return merged
    elif isinstance(orig, list):
        return _uniquify_options_to_start(orig + add)
    else:
        if orig != add:
            raise AssertionError("should not merge two items")
        return orig


class MultiVariable(object):
    """
    This tracks a variable, e.g. SRCS, and all the
    build-specific values it may have, e.g. SRCS.msvc
    or SRCS.debug_host or the like.

    Such subvalues are fetched by .get(str|BuildSpec).

    The final value of the variable is the one that
    accomodates all the non-conflicting configuration
    elements from build_env.

    The build specifications attached to variables are
    treated as shorthands, as they are in e.g. "--target spec"
    options, but there is an ordering between less-specific
    and more-specific variables that determines which are used
    and which are not used.

    For example, if SRCS.debug and SRCS.debug_msvc are both defined,
    then both are combined for debug_win_msvc_x64 builds.  Only
    SRCS.debug is used for debug_gcc_* builds.  Neither is used for
    any release_* build.

    Also, for example, say SRCS.msvc and SRCS.debug_host are defined.
    Since they encompass different aspects of the build specification,
    their values are combined in debug_win_msvc_x64 builds, whereas
    only one (or none) of SRCS.msvc and SRCS.gcc is ever used at a time.

    The plain SRCS is considered not to define any elements,
    so it is combined with every build-specific value.

    SRCS.default, on the other hand, is used only when
    no *other* build-specific values apply.

    The way to independently specify all aspects of variables
    is to provide full build specifications (SRCS.debug_win_msvc_x64)
    and not define plain SRCS.
    """

    def __init__(self):
        self._mapping = {}
        # :type: Dict[BuildSpec,object|List[object]]

        # BuildSpecs sorted by number of segments
        self._sorted = []
        # :type: List[BuildSpec]

    def __repr__(self):
        return str(self._mapping)

    def __iter__(self):
        all = [x for lst in self._mapping.values() for x in lst]
        return iter(all)

    def items(self):
        return self._mapping.items()

    @staticmethod
    def _spec(spec, ignore_missing=True):
        """
        Get the BuildSpec for the input, where missing segments are not filled in.
        :param spec: the specification to match, or None to match "default"
        :type spec: str | BuildSpec
        :rtype: BuildSpec
        """
        if spec is not None and not isinstance(spec, BuildSpec):
            spec = BuildSpec(spec, no_defaults=True, ignore_missing=ignore_missing)

        return spec

    def get(self, spec, ignore_missing=True):
        """
        Fetch the value of this variable defined
        explicitly for this specification.  Does not resolve aliases.
        :param spec: the specification to match, or None to match "default"
        :type spec: str | BuildSpec
        :return:
        """
        spec = self._spec(spec, ignore_missing)
        return self._mapping.get(self._spec(spec))

    def set(self, spec, value):
        """
        Set the value of this variable defined
        explicitly for this specification.  Does not resolve aliases.
        :param spec: the specification to match, or None to match "default"
        :type spec: str | BuildSpec
        :type value: List[object] | object
        :return: previous value
        """
        spec = self._spec(spec, ignore_missing=False)
        exist = self._mapping.get(spec)
        self._mapping[spec] = value
        self._sorted = None
        return exist

    def update(self, other):
        """
        merge the data from other into receiver
        """
        if isinstance(other, MultiVariable):
            # go spec-by-spec and merge
            for spec, value in other._mapping.items():
                self._mapping[spec] = _merge_setting(self._mapping.get(spec), value)
        else:
            # other is a plain setting, so we apply it to "all"
            all_spec = BuildSpec('', no_defaults=True)
            self._mapping[all_spec] = _merge_setting(self._mapping.get(all_spec), other)

    def _ensure_sorted(self):
        """
        Establish self._sorted as the ordering of BuildSpecs
        in self._mapping from least-specific to most-specific.
        Any "default" entry goes to the end.
        :return:
        """
        if not self._sorted:
            self._sorted = list(self._mapping.keys())
            self._sorted.sort(key=lambda a: (a and a.segments() or 5))

    def derive(self, spec):
        """
        Get the value of the variable derived from all the resolved BuildSpecs
        that match the given spec, from least-specific to most-specific.
        :param spec: the specification to match, or None to match "default"
        :type spec: str | BuildSpec
        :return: combined data or []
        """
        # get the fully-qualified BuildSpec here
        if spec is not None and not isinstance(spec, BuildSpec):
            spec = BuildSpec(spec, ignore_missing=True)

        # resolve elements to aliases
        if spec:
            spec = spec.resolve_aliases()

        data = None

        from project_parser import DEFAULT_SPEC, ALL_SPEC

        specializations_matched = []

        # go in order of specificity
        self._ensure_sorted()

        for aspec in self._sorted:
            # get a copy of the specific value first
            value = self._mapping[aspec]

            aspec = aspec and aspec.resolve_aliases(exclude_toolchain=True) or DEFAULT_SPEC

            if not value:
                alias_value = self._mapping.get(aspec)
                if alias_value is not None:
                    value = alias_value

            accept = False
            if aspec is DEFAULT_SPEC:
                # default (used when no other specializations match)
                if data is None or not specializations_matched:
                    accept = True
            elif aspec == ALL_SPEC:
                accept = True
            elif not spec or aspec.is_compatible(spec):
                accept = True
                specializations_matched.append(aspec)

            if accept:
                if data is None:
                    data = copy.copy(value)
                elif isinstance(value, dict):
                    data.update(value)
                else:
                    data += value

        return data or []


class PackageMappings(object):
    def __init__(self, build_mapping=[], data_mapping=[], data_warnings=[], data_errors=[]):
        self.build_mapping = build_mapping
        self.data_mapping = data_mapping
        self.data_warnings = data_warnings
        self.data_errors = data_errors


class Project(object):
    """
    This is the data model for a project.

    The name() and base() reflect the project name and base directory.

    The defs() reports all the variable=value definitions parsed.

    Other methods report values synthesized from the literal values in defs()
    and the base directory.
    """
    def __init__(self, path, name, base, output=None):
        self._path = path

        self._name = name
        self._base = unix_path(base)

        self._ext = os.path.splitext(path)[1]

        self._base_path_var = None

        self._outdir_root_path = None
        """ :type: str """
        self._outdir_path_var = None
        """ :type: build_model.Node """
        self._layout_path_var = None
        """ :type: build_model.Node """
        self._package_archive_path_var = None
        """ :type: build_model.Node """
        self._package_cache_path_var = None
        """ :type: build_model.Node """

        self._user_output = None
        """ :type: str """
        self._user_output_name = None
        """ :type: str """

        self._manifest_file = None
        """ :type: str """
        self._manifest = None
        """ :type: manifest.Manifest """

        # definitions
        self._defs = {}
        """ :type: dict[str,list|object] """
        # local definitions (subset of self._defs, sharing values)
        self._locals = None
        """ :type: dict[str,list|object] """
        # exported definitions (subset of self._defs, sharing values)
        self._exports = None
        """ :type: dict[str,list|object] """

        self._refs = []
        """ :type: list[str] """
        self._uses = []
        """ :type: list[str] """
        self._configs = []
        """ :type: list[str] """
        self._options = []
        """ :type: list[str] """
        self._direct_configs = []
        """ :type: list[str] """
        self._direct_options = []
        """ :type: list[str] """

        # combined definitions (the sum total applicable to the project,
        # via locals, REFS and USES; may be in a different format than the
        # raw values, such as for SRCS)
        self._combined = None
        """ :type: dict[str,list|object] """

        self._artifacts = []

        # list of project, makefile, input source, generated source tuples
        self._gen_src_list = []
        """ :type: list[Tuple(Project, Path, Path, Path)] """

        self._generated_files = {}
        """ :type: dict[Path,str] """

        self.output_name = None
        self.output_directory = output

    def __repr__(self):
        return self.name + self._ext + ' in ' + self._base

    def update_output_dirs(self):
        """
        Update the root output directory, the directory for the layout,
        and the package output and cache directories, whenever
        OUT or SPEC are changed.
        :param output: user-provided override or None for default
        """
        # output directory
        self._layout_path_var = None
        self._package_archive_path_var = None
        self._package_cache_path_var = None

        # base of output
        if self._user_output:
            out_root = Path.parse(self._user_output)
        else:
            out_root = Path(self.base_path(), DEFAULT_OUT_DIR)

        self._outdir_root_path = out_root.resolve()

        if self._ext == PACKAGE_EXT:
            # package builds are special:
            # the layout directory is shared amongst all builds
            # (usually links to source files found elsewhere)
            # but the package cache / output need to be build-specific
            package_segment = self.package_name

            if self._user_output:
                # user explicitly wants to share directories
                out_dir = out_root
                self._layout_path_var = out_root
                self._package_archive_path_var = out_root

                # But still keep cache inside build-specific subdir,
                # avoiding using the same structure as non-package builds
                # in case the package and an executable share names.
                # Also, make directory name unique in case multiple
                # .packages with the same filename but different directories
                # are being built.
                self._package_cache_path_var = Path(
                    out_root,
                    '.cache',
                    build_env.spec_var(),
                    path_to_filename(os.path.splitext(self._path)[0]),
                )
            else:
                # multiple projects may share same outdir, so distinguish
                # them by package name to avoid conflicts
                out_dir = Path(out_root, package_segment)
                self._layout_path_var = out_dir
                self._package_archive_path_var = out_dir
                self._package_cache_path_var = Path(out_dir, build_env.spec_var())

        else:
            # build-specific content
            out_dir = Path(out_root, build_env.spec_var())
            self._layout_path_var = None
            self._package_archive_path_var = None
            self._package_cache_path_var = None

        outdir_var = self.name_var() + '_' + build_vars.OUTPUT
        self._outdir_path_var = Variable(outdir_var, out_dir)

    class DefaultValueMap(dict):
        """
        Wraps a dictionary of project settings, returning the type-specific
        default if a given key is not provided.

        Raises AssertionError if any keys in 'illegal' are requested
        (specifically for use with Project.combined, where path-specific
        variables must not be referenced since they are no longer
        project-relative).
        """
        @staticmethod
        def default(key):
            if key == KIND: return None
            return []

        def __getitem__(self, item):
            if item not in self:
                return self.default(item)
            return dict.__getitem__(self, item)

    @staticmethod
    def _default_map():
        map = Project.DefaultValueMap()
        return map

    @property
    def locals(self):
        """
        Get a view of the local variables.
        :rtype: Dict[str,object]
        """
        if not self._locals:
            locs = self._default_map()

            for key, val in self._defs.items():
                if self._ext == PROJ_EXT:
                    if not key.endswith(PRIVATE_SUFFIX):
                        continue
                    key = key[:-len(PRIVATE_SUFFIX)]
                locs[key] = val

            self._locals = locs

        return self._locals

    @property
    def exports(self):
        """
        Get a view of the exported variables.
        :rtype: Dict[str,object]
        """
        if not self._exports:
            exps = self._default_map()

            for key, val in self._defs.items():
                if key in ALL_VARS and (key not in PROJ_EXPORTS and key not in ALWAYS_EXPORTED_VARS):
                    continue
                if key.endswith(PRIVATE_SUFFIX):
                    continue
                exps[key] = val

            self._exports = exps

        return self._exports

    @property
    def combined(self):
        """
        Get a view of the combined settings for variables.
        This is populated after self.resolve_settings().

        Note that some settings have a different representation
        when combined:

        SRC_EXTENSIONS: tuples of (makefile Path, source extension, dest extension)

        SRCS: tuples of (originating Project or null for component, Path)

        :rtype: dict[str,object]
        """
        return self._combined

    def flush_scopes(self):
        """
        Call to ensure self.locals() and self.exports() is up-to-date.
        """
        self._locals = None
        self._exports = None

    @property
    def path(self):
        """
        Get the resolved and unique path to the project file.
        :return: str
        """
        return self._path

    @property
    def name(self):
        """
        Get the project name (basename of file)
        :return:  str
        """
        return self._name

    @name.setter
    def name(self, name):
        self._name = name

    @property
    def base(self):
        """
        Get the base directory for the project (where its .proj was read)
        :return:  str
        """
        return self._base

    @base.setter
    def base(self, path):
        """
        Set the base directory for the project (where its .proj was read)
        """
        self._base = path

    @property
    def output_name(self):
        """
        Get the basename of the project output.
        :return: Returns OUTNAME if defined, self._name otherwise.
        :rtype: str
        """
        return self._user_output_name or self._name

    @output_name.setter
    def output_name(self, outname):
        """
        Set the basename of the project output (None to reset to default)
        """
        self._user_output_name = outname
        self.update_output_dirs()

    @property
    def output_directory(self):
        """
        Get the output directory for the project (usually under its location, and annotated
        with a build spec or package)
        :return:  str
        """
        return self._outdir_path_var.resolve()

    @output_directory.setter
    def output_directory(self, path):
        """
        Set the output directory for the project (None to reset to default)
        :return:  str
        """
        self._user_output = path
        self.update_output_dirs()

    @property
    def output_root_directory(self):
        """
        Get the output directory for the project (usually under its location, like .out)
        :return:  str
        """
        return self._outdir_root_path

    def output_filename(self):
        """
        Get the filename of the project output.
        :return: Returns OUTNAME if defined, self._name otherwise.
        :rtype: str
        """
        basename = self.output_name

        build_platform = build_env.platform()

        if self.is_program():
            output_filename = build_platform.format_program_file(basename)
        elif self.is_shared():
            output_filename = build_platform.format_shared_file(basename)
        elif self.is_static():
            output_filename = build_platform.format_static_file(basename)
        elif self.is_package():
            output_filename = format_package_file(basename)
        else:
            return None

        return output_filename

    @property
    def defs(self):
        """
        :return: all the definitions in this project
         :rtype: dict[str, str]
        """
        return self._defs

    @property
    def refs(self):
        """
        Get the resolved paths to referenced projects from REFS
        :rtype: List[str]
        """
        return self._refs

    @property
    def uses(self):
        """
        Get the resolved paths to referenced projects from USES
        :rtype: List[str]
        """
        return self._uses

    @property
    def configs(self):
        """
        Get the resolved paths to all referenced projects from CONFIG
        :rtype: List[str]
        """
        return self._configs

    @property
    def options(self):
        """
        Get the resolved paths to all referenced projects from OPTIONS
        :rtype: List[str]
        """
        return self._options

    @property
    def direct_configs(self):
        """
        Get the resolved paths to direct referenced projects from CONFIG (does not include indirect references from other projects)
        :rtype: List[str]
        """
        return self._direct_configs

    @property
    def direct_options(self):
        """
        Get the resolved paths to direct referenced projects from OPTIONS (does not include indirect references from other projects)
        :rtype: List[str]
        """
        return self._direct_options

    def get_all_external_dependencies(self):
        """
        Get all external component, config, and option dependencies as Node objects.

        :rtype: list[build_model.Node]
        """
        components = [x.path for x in self.dependency_list(None, build_env.components())]
        extensions = [path.resolve() for path in self.all_paths(EXTENSIONS)]
        return self._uniquify_dependencies(components + self.options + self.configs + extensions)

    @staticmethod
    def _uniquify_dependencies(dependencies):
        mlsdk_path = os.path.abspath(runtime_values[MLSDK])

        all_deps = []

        extra_dependencies = sorted(set(dependencies))
        for dep_file in extra_dependencies:
            if dep_file.startswith(mlsdk_path):
                # Any files in the MLSDK path should be made relative to the $(MLSDK) variable.
                dep_file = dep_file[len(mlsdk_path):]
                dep_file = Composition(Literal("$(MLSDK)"), Path(dep_file))
            else:
                dep_file = Path(dep_file)

            all_deps.append(dep_file)

        return all_deps

    def dependency_list(self, proj_map, comp_map):
        """
        Get a list of all the project names this one depends on, in top-down order.
        This does not include the receiver.
        :param proj_map: Map[str, projects.Project]
        :param comp_map: Map[str, projects.Project]
        :rtype: list[projects.Project]
        """
        return self._make_dependency_list(proj_map, comp_map, [], [])

    def _make_dependency_list(self, proj_map, comp_map, deps, stack):
        """
        Get a list of all the project names this one depends on.
        :param proj_map: Map[str, projects.Project]
        :rtype: Collection[projects.Project]
        """

        def add_dep(proj):
            if proj not in deps:
                if proj in stack:
                    raise BuildException("project '{0}' depends on itself through the path: {1}"
                                         .format(proj.name, " -> ".join([p.name for p in stack])))

                deps.append(proj)
                for dep in proj._make_dependency_list(proj_map, comp_map, deps, stack):
                    add_dep(dep)
                return True
            return False

        stack.append(self)

        if proj_map:
            for ref in self.refs:
                try:
                    proj = proj_map[ref]
                except KeyError as exc:
                    raise BuildException("did not load project '{0}' referenced from REFS in project '{1}'"
                                         .format(ref, self.name)) from None

                add_dep(proj)

        if comp_map:
            for comp in self.uses:
                try:
                    proj = comp_map[comp]
                except KeyError:
                    raise BuildException("did not load component '{0}' referenced from USES in project '{1}'"
                                     .format(comp, self.name)) from None

                add_dep(proj)

        stack.pop()

        return deps

    def _full(self, path):
        """
        Convert a Path to its full, resolved path.
        :type path: build_model.Path
        :rtype: str
        """
        path = path.resolve()

        # combine with project's base dir if not a full path
        path = os.path.join(self._base, path)

        # remove trailing slash
        # if path and path[-1] in ['/', '\\']:
        #     path = path[:-1]

        return path

    def name_var(self):
        """
        Get the name of the project, made safe for use as a variable.
        :return:  str
        """
        safe = self.name.replace(' ', '.').replace('/', '.')
        if self.extension == COMP_EXT:
            # distinguish same-named projects and components
            return safe + "_comp"
        else:
            return safe

    def base_path(self):
        """
        Returns a Node for the base directory of the project.
        :rtype: build_model.Node
        """
        if not self._base_path_var:
            self._base_path_var = Variable(self.name_var() + '_' + build_vars.BASE, PathLiteral(self._base))
        return self._base_path_var

    def outdir_path(self):
        """
        Returns a Node for the output directory of the project (with a build spec or package segment)
        :rtype: build_model.Node
        """
        return self._outdir_path_var

    def _get_output_relative_path(self, path):
        """
        Get the output filepath for the given Path -- assumed to be either
        absolute or relative to a project variable -- in the receiver's
        output directory.

        For example:
        $(proj_BASE)/foo/bar/baz.o --> $(proj_OUTPUT)/foo/bar/baz.o
        $(otherproj_BASE)/foo/bar/baz.o --> $(proj_OUTPUT)/otherproj/foo/bar/baz.o
        $(proj_BASE)/../foo.o --> $(proj_OUTPUT)/_full_path_to_foo.o
        /full/path/to/whee.o --> $(proj_OUTPUT)/_full_path_to_whee.o
        :type path: Path
        :return: Path
        """

        if not path.is_absolute() and not path.has_parent():
            return Path(self.outdir_path(), *path.segments)

        # Handle full paths specially -- things already in our project
        # are reduced to their same relative path in _OUTPUT.
        # Things from other projects are moved to <our_OUTPUT>/<orig relative path>.
        # Other full paths are reduced to_a_single_string_like_this to avoid
        # making unnecessarily deep directories.
        #
        first_seg = path.segments[0]
        if first_seg == self.base_path():
            suffix = path.segments[1:]
            new_path = Path(self.outdir_path(), *suffix)

        elif isinstance(first_seg, Variable) and first_seg.name.endswith('_BASE'):
            # another project's file
            new_path = Path(self.outdir_path(), first_seg.name[:-len('_BASE')], *path.segments[1:])
        else:
            new_path = None

        if not new_path or new_path.has_parent():
            # must be a full path or an otherwise suspicious one
            suffix_str = self.sanitize_path(self._base, path)
            new_path = Path(self.outdir_path(), suffix_str)

        return new_path

    @staticmethod
    def sanitize_path(base, path):
        """
        Convert a path with suspicious segments into a single segment that
        combines the canonical segments of the path's absolute location,
        e.g. $(FOO)/../bar where FOO=c:/path/to/baz.ext --> baz.ext-8346234978243
        :type path: projects.Path
        :rtype: str
        """
        # a full path with e.g. $(MLSDK) or raw filesystem
        src = path.resolve()
        try:
            src = os.path.abspath(os.path.join(base, src))
        except IOError:
            pass

        encoded = path_to_filename(src)
        return encoded

    def get_source_path(self, source_project, source_path):
        """
        Get the actual full source path for a relative 'source_path' mentioned
        in built project 'source_project'.

        Note that proj is None for components, which are not built.
        :type source_project: projects.Project
        :type source_path: Path
        :rtype: Path
        """
        if source_project and not source_path.is_absolute():
            return Path(source_project.base_path(), *source_path.segments)
        else:
            return source_path

    def get_object_output(self, source_project, source_path):
        """
        Get the output filepath for the object file for source_path
        (a Path inside this project or another)
        :type source_project: Project
        :type source_path: Path
        :param private: if True, inject the receiver's name into the project's output directory
        :return: build_model.Node
        """
        if source_path.is_absolute():
            # output dir is constructed specially for absolute paths
            new_path = source_path
        elif source_project and source_project != self:
            new_path = Path(self.base_path(), 'obj.' + self.name + '-' + source_project.name, *source_path.segments)
        else:
            # put objects in project-unique directory
            new_path = Path(self.base_path(), 'obj.' + source_project.name, *source_path.segments)

        objpath = self._get_output_relative_path(new_path)
        return objpath.change_extension(".o", append=True)

    def get_dependency_output(self, obj_path):
        """
        Get the output filepath for the dependencies of an object file
        (a Path inside the output directory)
        :type obj_path: Path
        :return: build_model.Node
        """
        return obj_path.change_extension(".d")

    def is_cxx_file(self, file):
        """
        Tell whether the filename represents a C++ source file.
        :type file: str | build_model.Path
        :rtype: bool
        """
        return os.path.splitext(str(file))[1].lower() in ['.cc', '.c++', '.cxx', '.cpp']

    def is_c_file(self, file):
        """
        Tell whether the filename represents a C source file.
        :type file: str
        :rtype: bool
        """
        return os.path.splitext(str(file))[1].lower() in ['.c']

    def is_asm_file(self, file):
        """
        Tell whether the filename represents an assembly file.
        :type file: str
        :rtype: bool
        """
        return os.path.splitext(str(file))[1].lower() in [".s", ".asm"]

    def is_objc_file(self, file):
        """
        Tell whether the filename represents an Objective-C file.
        :type file: str
        :rtype: bool
        """
        return os.path.splitext(str(file))[1].lower() in [".m"]

    def is_objcpp_file(self, file):
        """
        Tell whether the filename represents an Objective-C++ file.
        :type file: str
        :rtype: bool
        """
        return os.path.splitext(str(file))[1].lower() in [".mm"]

    def is_c_family_file(self, src):
        return self.is_c_file(src) or self.is_cxx_file(src) \
               or self.is_objc_file(src) or self.is_objcpp_file(src)

    def has_cxx_content(self, env):
        for src in self.locals[SRCS]:
            if self.is_cxx_file(src):
                return True

        # static[-shared] libraries' sources are directly linked
        dfs = self.dependency_list(env.projects(), env.components())
        for dep in dfs:
            if dep.is_static():
                if dep.has_cxx_content(env):
                    return True

        return False

    @staticmethod
    def _create_path(dep, inc):
        path = Path.parse(inc)
        if not path.is_absolute():
            path = Path(dep.base_path(), *path.segments)
        return path

    def _all_paths(self, var, path_maker, scope=0, force_exports=False):
        """
        Get the Paths for all entries for 'var' from this project's
        locals and all its dependencies' exports.

        The search is modified by scope.

        NOTE: this will be incorrect if 'var' may be
        provided by OPTIONS, since we only consider the local
        values of the setting for the receiver.
        :param var: variable name
        :param path_maker: a function that maps Project, str arguments
        into a single entry or a list of entries (usually a resolved path,
        but package outputs return a tuple)
        :param scope: the project scope to search (local=-1, all=0, refs=1)
        :param force_exports: if true, consider receiver's exports (else, only the locals)
        :rtype: List[Path] or List[tuple[Path,Path]]
        """
        def get(defs, var):
            coll = defs.get(var, [])
            if isinstance(coll, dict):
                return coll.items()
            elif var != DATAS:
                if coll and isinstance(coll[0], tuple):
                    coll = [t[0] for t in coll]
            return coll

        if scope <= 0:
            paths = [path_maker(self, path) for path in get(self.locals, var)]
            if force_exports:
                paths += [path_maker(self, path) for path in get(self.exports, var)]
        else:
            paths = []

        if scope >= 0:
            dfs = self.dependency_list(build_env.projects(), build_env.components())
            for dep in dfs:
                paths += [path_maker(dep, path) for path in get(dep.exports, var)]

        return paths

    def all_original_source_files(self):
        """ Generate list of the user-provided sources --
        private SRCS and ref's/use's exported SRCS -- indicating where
        each came from.

        These sources precede any SRC_EXTENSIONS conversions.

        :return: List of tuples of project and relative source path
        :rtype: list[(Project, Path)]
        """
        srcs = list(self.all_source_files())
        gen_srcs = self.generated_sources
        if gen_srcs:
            back_mapping = {}
            for proj, _, in_path, out_path in gen_srcs:
                back_mapping[(proj, out_path)] = (proj, in_path)
            srcs = [back_mapping.get(entry, entry) for entry in srcs]
        return srcs

    def all_source_files(self):
        """ Generate list of the buildable source files used by the project --
        private SRCS and ref's/use's exported SRCS -- indicating where
        each came from.

        By default (before_src_extensions=False), these are the sources _after_
        SRC_EXTENSIONS has been applied.  Otherwise, supply the incoming
        sources.

        :return: List of tuples of project and relative source path
        :rtype: list[(Project, Path)]
        """
        return self.combined[SRCS]

    def all_object_files(self):
        """ Generate the object files for the sources in the project.
        :type env: build_env
        :rtype: list[Path]
        """
        # note: we always make a private object copy of sources
        def object_path(proj, src):
            return self.get_object_output(proj, Path.parse(src))

        # objects are those from the project, then any manually-specified OBJS
        return [object_path(proj, src) for proj, src in self.combined[SRCS]] \
               + self.combined[OBJS]

    def all_shared_implibs(self):
        """ Generate the paths for the shared library import libs the project
        must link against.

        Note: the filename segment of the Path is the project name
        (which one alters via Platform.format_static_file() to form the filename).
         :rtype: List[Path]
        """

        # get local SHLIBS
        #
        def sh_implib(comp, path_str):
            path = Path.parse(path_str)
            if not path.is_absolute():
                if len(path) == 1:
                    # bare name
                    return None, path
                path = Path(comp.base_path(), *path.segments)
            return path.directory, path.filename

        locals = [sh_implib(self, lib) for lib in self.combined[SHLIBS]]

        # get the import libs from referenced projects, whose
        # actual output path we know...
        def ref_implib(proj):
            path = Path(proj.outdir_path(), proj.output_name)
            return path.directory, path.filename

        dfs = self.dependency_list(build_env.projects(), build_env.components())
        refs = [ref_implib(proj) for proj in dfs if proj.is_shared()]

        if not self.is_static():
            # and any exported
            exp_projs = [proj for proj in dfs if not proj.is_component()]
            exps = []
            for exp_proj in exp_projs:
                for lib in exp_proj.exports[SHLIBS]:
                    exps.append(sh_implib(exp_proj, lib))
                if not exp_proj.is_static():
                    break

            # and those from used components, which are formed
            # from Paths (but whose filenames must be twiddled)
            comps = [proj for proj in dfs if proj.is_component()]
            uses = [sh_implib(comp, lib) for comp in comps for lib in comp.exports[SHLIBS]]
        else:
            exps = []
            uses = []

        return _uniquify_to_end(locals + refs + exps + uses)

    def all_static_libraries(self):
        """ Gather information about the static libraries linked into the project.

        These are Tuples of a Path directory and the project name
        (which one alters via Platform.format_static_file() to form the filename).

        :rtype: list[tuple[build_model.Path, build_model.Node]]
        """

        # get local STLIBS
        #
        def st_lib(proj, path_str):
            path = Path.parse(path_str)
            if not path.is_absolute():
                if len(path) == 1:
                    # bare name
                    return None, path
                path = Path(proj.base_path(), *path.segments)
            return path.directory, path.filename

        locals = [st_lib(self, lib) for lib in self.combined[STLIBS]]

        dfs = self.dependency_list(build_env.projects(), build_env.components())

        # get the static libs from referenced projects
        def lib(proj, name):
            path = Path(proj.outdir_path(), name)
            return path.directory, path.filename

        refs = [lib(proj, proj.output_name) for proj in dfs if proj.is_static()]

        if not self.is_static():
            # and any exported
            exp_projs = [proj for proj in dfs if not proj.is_component()]
            exps = []
            for exp_proj in exp_projs:
                for lib in exp_proj.exports[STLIBS]:
                    exps.append(st_lib(exp_proj, lib))
                if not exp_proj.is_static():
                    break

            # and those from used components, which are formed
            # from Paths (but whose filenames must be twiddled)
            comps = [proj for proj in dfs if proj.is_component()]
            uses = [st_lib(comp, lib) for comp in comps for lib in comp.exports[STLIBS]]
        else:
            exps = []
            uses = []

        return _uniquify_to_end(locals + refs + exps + uses)

    @staticmethod
    def _gather_wildcard_datas(proj, src_path, target_path, src_absolute):
        """
        Take a src_path with wildcard characters in it, and a deduced target_path,
        then resolve the wildcards, and return a list of (source Path, target Path)
        entries as if the user entered these manually.
        :param proj:
        :param src_path:
        :param target_path:
        :param src_absolute:
        :return: List[tuple[Path, Path]]
        """
        import glob

        raw_src_glob = src_glob = src_path.resolve().replace('\\', '/')

        def hide_brackets(path):
            # "For a literal match, wrap the meta-characters in brackets.
            # For example, '[?]' matches the character '?'."

            # can't use replace here, else we double-expand things
            tmp = ''
            for ch in path:
                if ch == '[':
                    tmp += '[[]'
                elif ch == ']':
                    tmp += '[]]'
                else:
                    tmp += ch
            return tmp

        # ignore brackets if they match an extant directory
        brack_pos = max(src_glob.find('['), src_glob.find(']'))
        if brack_pos >= 0:
            slash = src_glob.find('/', brack_pos)
            if slash > brack_pos:
                test_src_glob_dir = src_glob[0:slash]
                if os.path.isdir(test_src_glob_dir):
                    src_glob = hide_brackets(test_src_glob_dir) + src_glob[slash:]

        # get the prefix up to the first wildcardy segment
        def pos_or_end(ch):
            pos = src_glob.find(ch)
            if pos < 0:
                pos = len(src_glob)
            return pos

        star_pos = pos_or_end('*')
        quest_pos = pos_or_end('?')
        wild_pos = min(star_pos, quest_pos)

        slash = raw_src_glob.rfind('/', 0, wild_pos)
        raw_src_glob_prefix = raw_src_glob[0:slash + 1]

        ents = []

        try:
            glob_entries = glob.glob(src_glob, recursive=True)
        except Exception:
            # try again
            src_glob = hide_brackets(src_glob)
            try:
                glob_entries = glob.glob(src_glob, recursive=True)
            except Exception as e:
                raise BuildException("syntax error in wildcard (" + src_glob + ")")

        for ent in glob_entries:
            if not os.path.exists(ent) or os.path.isdir(ent):
                continue

            ent_target_path = target_path

            if src_absolute:
                ent_source_path = Path.parse(ent)
            else:
                source_suffix = ent[len(proj.base):]
                ent_source_path = Path(proj.base, source_suffix)

            target_suffix = ent[len(raw_src_glob_prefix):]
            ent_target_path = Path(ent_target_path, target_suffix)

            ents.append((ent_source_path, ent_target_path))

        return ents

    @staticmethod
    def _gather_directory_datas(proj, src_root, target_path, src_absolute):
        """
        Take a :src_root: representing a directory, and a deduced :target_path:,
        then add every entry inside, and return a list of (source Path, target Path)
        entries as if the user entered these manually.

        This differs from wildcards in that we fetch dotfiles (except '.' and '..',
        of course).
        :param proj:
        :param src_root:
        :param target_path:
        :param src_absolute:
        :return: List[tuple[Path, Path]]
        """
        ents = []

        for dirpath, _, filenames in os.walk(src_root):
            for filename in filenames:
                ent = os.path.join(dirpath, filename)
                ent_target_path = target_path

                if src_absolute:
                    ent_source_path = Path.parse(ent)
                else:
                    source_suffix = ent[len(proj.base):]
                    ent_source_path = Path(proj.base, source_suffix)

                target_suffix = ent[len(src_root):]
                ent_target_path = Path(ent_target_path, target_suffix)

                ents.append((ent_source_path, ent_target_path))

        return ents

    def all_data_files(self, scope=0, clean=False):
        """ Generate the map of the data files used by the package --
        from the DATAS in the package and all its REFS and USES.

        This expands wildcards and directories to their
        recursive mappings.  We ignore dotfiles (clients can
        always explicitly
        :param scope: 0 for all, -1 for local, 1 for refs
        :return: list of of input full paths and output package paths, list of warnings, list of errors
        :rtype: list[tuple[Path, Path]], list[Str], list[Str]
        """
        warnings = []
        errors = []

        def data_paths(proj, src_target):
            src, target = src_target

            # get expected types (sometimes src_target[0] holds
            # a Node, sometimes a str... FIXME)
            src_absolute = False
            if not isinstance(src, Node):
                is_src_directory = src.endswith('/')
                src_suffix = Path.parse(src)
                if src_suffix.is_absolute():
                    src_absolute = True
                    src_path = src_suffix
                else:
                    src_path = Path(proj.base, src_suffix)
                src = src_path.resolve()
            else:
                src_path = src
                src_absolute = src_path.is_absolute()
                src = src.resolve()
                is_src_directory = src.endswith('/')

            if not is_src_directory and os.path.isdir(src):
                if not clean:
                    try:
                        simple_src = src_target[0].relative_to(proj.base_path())
                    except AssertionError:
                        simple_src = src_target[0]
                    errors.append('in DATAS ({0} : ...), the source resolves to a directory ({1}) '
                                  'and should end in a slash'.format(
                                    simple_src, src))

            target_path = Path.parse(target)

            if is_wild_source(src):
                ents = self._gather_wildcard_datas(proj, src_path, target_path, src_absolute)
                if not ents:
                    warnings.append('no entries matched wildcard in DATAS ({0} : {1})'.format(
                                      src_target[0], src_target[1]))

                return ents
            elif is_src_directory:
                try:
                    # exclude slash from tested path, to detect directory/file-ness properly
                    st = os.stat(src[0:-1])
                    if not stat.S_ISDIR(st.st_mode):
                        errors.append('source is not a directory in DATAS ({0} : {1})'.format(
                            src_target[0], src_target[1]))
                        return []
                except OSError as e:
                    errors.append('source directory does not exist or is inaccessible in DATAS ({0} : {1})'.format(
                        src_target[0], src_target[1]))
                    return []
                ents = self._gather_directory_datas(proj, src, target_path, src_absolute)
                return ents
            else:
                if not os.path.exists(src):
                    errors.append("DATAS: no such file '{0}'\n(in entry {1} : {2})" \
                                  .format(src, src_target[0], src_target[1]))
                return src_path, target_path

        def flatten_list(lst):
            l = []
            for item in lst:
                if isinstance(item, list):
                    l += item
                else:
                    l.append(item)
            return l

        if scope == 0:
            datas = flatten_list([data_paths(self, path) for path in self.combined[DATAS]])
        else:
            datas = flatten_list(self._all_paths(DATAS, data_paths, scope))

        def is_duplicate_target(src_dest):
            return src_dest[1]

        return remove_duplicates(datas, is_duplicate_target), warnings, errors

    def all_paths(self, var, scope=0, force_exports=False):
        """ Generate list of the Paths for files in this project
        and its dependencies.
        For DATAS, this yields only the source paths.
        :param var: the variable to fetch
        :param scope: 0 for all, -1 for local, 1 for refs
        :param force_exports: if true, consider receiver's exports (else, only the locals)
         :rtype: list[Path]
        """
        def full_path(proj, src):
            """
             :type proj: projects.Project
            """
            if isinstance(src, tuple):
                src = src[0]

            path = Path.parse(src)
            if path.is_absolute():
                return path
            else:
                return Path(proj.base_path(), path)

        return self._all_paths(var,
                               full_path,
                               scope=scope,
                               force_exports=force_exports)

    def fullpaths(self, var):
        """
        Fetch full resolved paths to file references in a given variable
        (local only) -- used for validation
        :return: the absolute paths to filenames for the given variable
        :rtype: list[str]
        """
        return [path.resolve() for path in self.all_paths(var, scope=-1, force_exports=True)]

    @property
    def artifacts(self):
        """
        :rtype: list[build_model.Path]
        """
        return self._artifacts

    @artifacts.setter
    def artifacts(self, artifacts):
        """
        :type artifacts: list[build_model.Path]
        """
        self._artifacts = artifacts

    @property
    def generated_sources(self):
        """
        Get the list of generated sources: project, user .mk, input SRCS, generated source
        :rtype: list[tuple[projects.Project,build_model.Path,build_model.Path,build_model.Path]]
        """
        return self._gen_src_list

    def all_outputs(self):
        """
        Get Paths for all files generated by the receiver
        :rtype: tuple[list[Path]
        """
        outs = []
        if not self.is_component():
            toolchain = build_env.toolchain()
            out_path = self.outdir_path()

            if self.is_package():
                outs.append(self.package_archive_path)

            for art in self.artifacts:
                outs.append(art)

            for artifact in toolchain.get_unique_artifacts(self):
                outs.append(artifact)

            for proj, src in self.all_source_files():
                obj_path = self.get_object_output(proj, src)
                outs.append(obj_path)

                if self.is_c_family_file(src):
                    outs.append(self.get_dependency_output(obj_path))

            for proj, _, gen_in_path, gen_out_path in self.generated_sources:
                outs.append(Path(proj.base_path(), gen_out_path))

        # layout's files
        out_files, _, _ = self.all_data_files()
        for _, dest in out_files:
            outs.append(Path(out_path, dest))

        # remove dups
        outs = remove_duplicates(outs)

        return outs

    @property
    def extension(self):
        return self._ext

    def is_program(self):
        return self.locals[KIND] == kinds.program.NAME

    def is_shared(self):
        return self.locals[KIND] == kinds.shared.NAME

    def is_static(self):
        return self.locals[KIND] in [kinds.static.NAME, kinds.static_shared.NAME]

    def is_component(self):
        return self._ext == COMP_EXT

    def is_package(self):
        return self._ext == PACKAGE_EXT

    @property
    def package_name(self):
        """
        Return the base package filename for a project,
        using the user-defined OUTNAME or project name.
        :rtype: str
        """
        return self.output_name

    @property
    def manifest(self):
        if not self._manifest:
            self._manifest = Manifest()
            try:
                self._manifest.load(self.manifest_file)
            except IOError:
                pass
            self.update_output_dirs()
        return self._manifest

    @property
    def manifest_file(self):
        return self._manifest_file or os.path.join(self.base, MANIFEST_FILE)

    @manifest_file.setter
    def manifest_file(self, file):
        self._manifest_file = file
        self._manifest = None

    @property
    def package_layout_path(self):
        """
        Get the directory where a package's runtime layout appears
        (only set for host builds)
        :return: Path
        """
        return self._layout_path_var

    @property
    def package_archive_path(self):
        """
        Get the path to a package's archive (.mpk)
        (only set for device builds)
        :return: Path
        """
        if not self._package_archive_path_var:
            return None
        return Path(self._package_archive_path_var, format_package_file(self.package_name))

    @property
    def package_cache_path(self):
        """
        Get the directory where we cache data about the build
        (unique per build spec) (only set for device builds)
        :return: Path
        """
        return self._package_cache_path_var

    def flatten_defs(self, spec):
        """
        Flatten the MultiVariable definitions into single values.
        :type spec: build_model.BuildSpec
        """
        vars = copy.copy(self._defs)
        self._defs.clear()

        for var, mv in vars.items():
            base = var[:-len(PRIVATE_SUFFIX)] if var.endswith(PRIVATE_SUFFIX) else var
            if not isinstance(mv, MultiVariable):
                value = mv
            elif spec is not None or base in UNSPECIALIZABLE_VARS:
                value = mv.derive(spec)
            else:
                value = mv
            self._defs[var] = value

        self.flush_scopes()

    def get_package_mappings(self, force_package, scope):
        """
        Get the mappings of input files to output files from the receiver,
        modulated by scope.

        This assumes the receiver and dependencies have been
        resolved or processed by MakeGen.

        For a package project, this consists of DATAS, where all wildcards
        and directory references are expanded.

        For any other project, it is the collection of expected
        build artifacts.

        NOTE: static library artifacts have None as their package output paths.
        NOTE: generated files have None as their local paths.
        :param force_package: get outputs if an .mpk is built, even if on host
        :param scope: 0 for all, -1 for local, 1 for refs
        :return: mappings info
        :rtype: PackageMappings
        """
        env = build_env

        build_mapping = []
        data_mapping = []
        warnings = []
        errors = []

        # actual DATAS
        datas, sub_warnings, sub_errors = self.all_data_files(scope)
        warnings += sub_warnings
        errors += sub_errors

        for src, dest in datas:
            data_mapping.append((src.value(), dest.value()))

        # build artifacts
        if scope <= 0:
            output_dir = self.output_directory
            for art in self.artifacts:
                path = art.value()
                out = os.path.join('bin', os.path.relpath(path, output_dir)) if not self.is_static() else None
                build_mapping.append((path, out))

        if scope >= 0:
            dfs = self.dependency_list(env.projects(), env.components())
            for project in dfs:
                output_dir = project.output_directory
                for art in project.artifacts:
                    path = art.value()
                    # static libs are not emitted into a package
                    out = os.path.join('bin', os.path.relpath(path, output_dir)) \
                        if not project.is_static() or not self.is_package() else None
                    build_mapping.append((path, out))

        if self.is_package() and scope <= 0 and (env.platform().uses_packages() or force_package):
            # the manifest is an output file for a package
            if self.manifest.exists():
                data_mapping.append((self.manifest_file, MANIFEST_FILE))
            elif env.platform().uses_packages():
                errors.append("No manifest.xml found, expected {}".format(os.path.normpath(self.manifest_file)))

        # remove duplicates
        build_mapping = remove_duplicates(build_mapping)
        data_mapping = remove_duplicates(data_mapping)

        return PackageMappings(build_mapping, data_mapping, warnings, errors)

    @staticmethod
    def _uniquify_options(opts):
        """
        Remove options coming from the same category, where the category is
        the longest prefix of an option before '/'.

        E.g. debug/on debug/off opts/full --> debug/off opts/full
        :param opts: List[str]
        :return: new opts
        """
        opt_map = {}
        opt_list = []
        for opt in opts:
            if '/' in opt:
                idx = opt.rfind('/')
                category, name = opt[0:idx], opt[idx+1:]
                oidx = opt_map.get(category)
                if oidx is not None:
                    opt_list[oidx]  = None     # remove the earlier dupe
                opt_map[category] = len(opt_list)   # store this entry's location

            opt_list.append(opt)

        return [x for x in opt_list if x]

    def resolve_settings(self, config):
        """
        Expand the project by replacing all settings with their concrete
        build flags.  Such flags override the values of any concrete flags
        already in the project.
        :param config: the base configuration
        :type config: projects.Project
        :return: flag indicating whether anything changed
        :rtype: boolean
        """
        if self._combined:
            # already handled
            return False

        # make a map for the combined settings
        self._combined = self._default_map()

        # local or always-exported settings are
        # the basis of settings
        for key, val in self._defs.items():
            if key.endswith(PRIVATE_SUFFIX):
                key = key[:-len(PRIVATE_SUFFIX)]
            elif key not in ALWAYS_EXPORTED_VARS and key != OUTNAME:
                continue

            # WARNING: _combined temporarily contains non-canonical data
            self._combined[key] = copy.deepcopy(val)

        # pre-merge the config to fetch non-option settings
        if config:
            self._pre_merge(config)

        # fetch all the applicable OPTIONS
        changed = self._merge_options(config)

        # merge settings from REFS and USES
        self._merge_refs_uses(config)

        # convert SRCS
        self._convert_srcs()

        # expand custom source extensions
        self._expand_src_extensions()

        # see if we changed the OUTNAME
        if OUTNAME in self.combined:
            outnames = self.combined[OUTNAME]
            if outnames:
                # entries are combined left-to-right from config/options/uses/projects,
                # so use the first one (override)
                self.output_name = outnames[0]

        # see if we retargeted the OUT
        # (from a component -- direct OUT settings handled in factory)
        if OUT in self.combined:
            outdirs = self.combined[OUT]
            if outdirs:
                # entries are combined left-to-right from config/options/uses/projects,
                # so use the first one (override)
                self.output_directory = outdirs[0]
        else:
            # update vars if e.g. package manifest was set
            self.update_output_dirs()

        if changed:
            debug("project '{1}' defs after merge: {0}".format(self.combined, self.name))
        return changed

    def _merge_options(self, config):
        """
        Resolve the final OPTIONS from the config, platform, and project kind
        with those from REFS and USES into final list, applying option category overrides
        to reduce to the smallest list that favors the user project over the config
        or other sub-projects.
        :type config: projects.Project
        """
        spec = build_env.spec()

        opts = self._all_options(config)
        if isinstance(opts, MultiVariable):
            opts = opts.derive(spec)

        # weed out any with the same prefix
        opts = self._uniquify_options(opts)

        if not self.is_component() and not self.is_package():
            # inject the project type's options
            kind = self.locals[KIND]
            from kinds._base import Kind
            if kind in Kind.all():
                opts[0:0] = ['kinds/' + kind]

            if build_env.spec():
                # inject the platform options
                opts[0:0] = ['platforms/' + build_env.spec().platform]

        # we pre-merge, so, reverse the list
        opts.reverse()
        changed = False
        for opt in opts:
            # get the project for the option...
            opt_proj = build_env.find_option(opt)
            if not opt_proj:
                error("unknown option '{0}' referenced in project '{1}'".format(opt, self.name))
                continue

            # flatten a copy
            opt_proj = copy.deepcopy(opt_proj)  # copied to avoid trampling on the original
            opt_proj.flatten_defs(spec)

            # and merge settings
            changed |= self._pre_merge(opt_proj)

        return changed

    def _merge_refs_uses(self, config):
        """
        Inject setting contributions from REFS and USES
        :type config: projects.Project
        """
        dfs = self.dependency_list(build_env.projects_and_packages(), build_env.components())
        configs_options = [p for p in dfs if p.extension in [CONFIG_EXT, OPTION_EXT]]
        for var in extension_to_vars[self._ext]:
            if var == KIND:
                continue

            if var == DATAS:
                # the combined values for these are List[Tuple[Tuple[Project,Path],str]]
                vals = self._resolve_datas(config, dfs, var)
            elif var in PATH_VARS:
                # the combined values for these are List[Tuple[Project,Path]]
                vals = self._resolve_paths(config, dfs, var)
            elif var == STLIBS or var == SHLIBS:
                # these are only inherited from .option or .config,
                # since they must be interpreted specially
                # from REFS and USES
                vals = self._resolve_setting(config, configs_options, var)
            else:
                vals = self._resolve_setting(config, dfs, var)

            self._combined[var] = vals

    def _resolve_setting(self, config, dfs, var):
        """
        Resolve all the values for settings named 'var' from the given 'config',
        the dependency list 'dfs', and the receiver.
        :type config: projects.Project
        :type dfs: list[projects.Project]
        :type var: str
        """
        vals = config and copy.copy(config.locals[var]) or Project.DefaultValueMap.default(var)

        for dep in dfs:
            vals = _merge_setting(vals, dep.exports[var])

        vals = _merge_setting(vals, self._combined[var])
        return vals

    def _resolve_paths(self, config, dfs, var):
        """
        Resolve all the paths for settings named 'var' from the given 'config',
        the dependency list 'dfs', and the receiver.
        :type config: projects.Project
        :type dfs: list[projects.Project]
        :type var: str
        :return: List[Path]
        """
        vals = Project.DefaultValueMap.default(var)

        if config and var in config.locals:
            vals = [ self._create_path(config, path) for path in config.locals[var]]

        for dep in dfs:
            dep_vals = [ self._create_path(dep, path) for path in dep.exports[var]]
            vals += dep_vals

        # for path-based variables, user settings override others
        my_vals = [ self._create_path(self, path) for path in self.locals[var]]
        vals[0:0] = my_vals

        return vals

    def _resolve_datas(self, config, dfs, var):
        """
        Resolve all the paths for settings named 'var' from the given 'config',
        the dependency list 'dfs', and the receiver.

        These DATAS are maintained in tuples (<source path str>, <dest path str>),
        and converted to tuples in the form:  (<source Path>, <dest path str>).

        :param config:
        :param dfs:
        :param var:
        :return: List[Tuple[Path, str]]
        """

        vals = []

        if config and var in config.exports:
            vals = [ (self._create_path(config, f), Path.parse(t)) for f, t in config.exports[var] ]

        for dep in dfs:
            dep_vals = [ (self._create_path(dep, f) , Path.parse(t)) for f, t in dep.exports[var] ]
            vals = _merge_setting(vals, dep_vals)

        cmb_vals = [ (self._create_path(self, f), Path.parse(t)) for f, t in self.combined[var] ]
        vals = _merge_setting(vals, cmb_vals)

        # update exports for dependency checks
        self.exports[var] = vals

        return vals

    def _all_options(self, config):
        """
        Gather all the OPTIONS values for this project, which are
        the configuration's, plus all those from the dependency list,
        then those defined locally.
        :param config: the configuration's project
        :return: List[Node | str]
        """

        if config:
            vals = list(config.locals[OPTIONS])
        else:
            vals = []

        dfs = self.dependency_list(build_env.projects(), build_env.components())
        for dep in dfs:
            vals = _merge_setting(vals, dep.exports[OPTIONS])

        vals = _merge_setting(vals, self.locals[OPTIONS])

        return vals

    def _pre_merge(self, other):
        """
        Iterate the definitions in 'other' and prepend them to the
        corresponding combined definitions in the receiver.

        Return True if any changes were made.
        :type other: projects.Project
        :rtype: boolean
        """

        allowed = extension_to_vars[self._ext]
        changed = False
        for key, value in other.defs.items():
            if key not in allowed:
                continue

            if isinstance(value, MultiVariable):
                value = value.derive(build_env.spec())

            orig = self._combined.get(key)
            merged = _merge_setting(value, orig)

            if merged != orig:
                debug("{0}: merged [{1}].{2}={3} into project".format(self.name, other.name, key, str(value)))
                self._combined[key] = merged
                changed = True

        return changed

    def _convert_srcs(self):
        """
        Convert the original SRCS -- list[str] -- to the combined
        form of (Project, Path)
        """
        def source_path(proj, src):
            path = Path.parse(src)
            if path.is_absolute():
                return None, path
            else:
                return proj, path

        self.combined[SRCS] = self._all_paths(SRCS, source_path)

    def _expand_src_extensions(self):
        """
        Interpret the combined SRC_EXTENSIONS variable to update the
        combined SRCS to those we expect the extension makefile to generate.
        """
        out_srcs = []

        ext_map = {}
        for (mk, src, dst, pfx) in self.combined[SRC_EXTENSIONS]:
            ext_map[src] = (dst, pfx, mk)

        self._gen_src_list = []

        changed = False
        for project, src in self.combined[SRCS]:
            path, name = src.directory, src.filename
            base, ext = os.path.splitext(str(name))
            if ext in ext_map:
                # converted
                dst_ext, dst_pfx, mk = ext_map[ext]
                cvt_suffix = Path.join(path, base + dst_ext)
                cvt_path = Path.join(dst_pfx, cvt_suffix)
                out_srcs.append((project, cvt_path))

                # remember how we made these files
                self._gen_src_list.append((project, mk, src, cvt_path))

                changed = True
            else:
                # normal source
                out_srcs.append((project, src))

        if changed:
            debug("Converted SRCS='{0}' to SRCS='{1}'".format(self.combined[SRCS], out_srcs))

        self.combined[SRCS] = out_srcs

    @property
    def generated_files(self):
        """
        Get the mapping of fullpaths (usually inside outdir_path) to contents
        :rtype: dict[Path, str]
        """
        return self._generated_files
