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
This models the full build environment:
all the projects requested and all the components registered.
"""
import os

import architectures
import diags
import kinds
import platforms
import toolchains

from build_model import BuildSpec, Variable, BuildException
from config import DATA_DIR, CONFIG_EXT, OPTION_EXT, COMP_EXT, PACKAGE_EXT
from diags import warn
from project_vars import BASE
from run_vars import MABUPATH, runtime_values, MLSDK
from utils import static_vars

""" This marks the split between user and builtin paths """
BUILTIN_PATH_SEPARATOR = '...'


_spec = None
""" :type: build_model.BuildSpec """
_spec_var = None
""" :type: build_model.Variable """

_user_projects = []
""" :type: list[projects.Project] """

_projects = {}
""" :type: dict[str,projects.Project] """
_packages = {}
""" :type: dict[str,projects.Project] """
_components = {}
""" :type: dict[str,projects.Project] """
_configs = {}
""" :type" dict[str,projects.Project] """
_options = {}
""" :type: dict[str,projects.Project] """
_configs_full_path = {}
""" :type: dict[str, projects.Project] """
_options_full_path = {}
""" :type: dict[str, projects.Project] """


_path = [BUILTIN_PATH_SEPARATOR,
         os.path.join(DATA_DIR, 'components'),
         os.path.join(DATA_DIR, 'configs'),
         os.path.join(DATA_DIR, 'options')]

_config_keys = None
""" :type: List[str] """
_option_keys = None
""" :type: List[str] """


def discover_user_tool():
    """
    Register a toolchain for any overridden CC or CXX variable, and
    make it the default.

    This is intended to be called _before_ build_env.set_spec() --
    i.e. operating on the runtime_values[SPEC] parsed from the command line --
    to allow the user to override any default toolchain selection.

    (Conversely, detection will not honor a CC or CXX that does not
    match an explicitly stated `-t ...` option.)
    """
    from toolchains._base import Toolchain
    from build_vars import CC, CXX
    from run_vars import runtime_values, SPEC

    driver = runtime_values.get(CC) or runtime_values.get(CXX)
    if not driver:
        return None

    spec = runtime_values.get(SPEC)
    build_spec = BuildSpec(spec, ignore_missing=True)
    platform = Toolchain.default_platform(build_spec.platform)

    for tc_cls in Toolchain.toolchain_classes():
        if tc_cls.can_user_override():
            tc = tc_cls.discover_toolchain(driver, preferred=True, platform=platform, user_override=True)
            if tc:
                diags.debug(
                    "registered toolchain '{}' derived from user toolchain override".format(
                        tc.name))

                # update spec to match
                from run_vars import runtime_values, SPEC
                override_spec = BuildSpec(runtime_values.get(SPEC), toolchain=tc.name)
                runtime_values[SPEC] = override_spec

                return None

    return "could not determine toolchain in user override of CC or CXX ('{}'); please use a full path if needed; " \
           "or perhaps use COMPILER_PREFIX or LINKER_PREFIX if wrapping an existing tool " \
           "(add --debug for more info about the detection process)" \
        .format(driver)


def spec():
    """
    :rtype: build_model.BuildSpec
    """
    global _spec

    return _spec


def set_spec(spec):
    """
    Set the BuildSpec from a string or existing BuildSpec,
    and resolve any aliases to form a canonical BuildSpec.
    :param spec: str|BuildSpec
    :return:
    """
    if not isinstance(spec, BuildSpec):
        spec = BuildSpec(spec)

    global _spec, _spec_var
    _spec = spec.resolve_aliases()
    _spec_var = None

    return _spec


def spec_var():
    import build_vars
    global _spec_var
    if not _spec_var:
        _spec_var = Variable(build_vars.SPEC, str(_spec))
    return _spec_var


def search_path():
    """
    Get the search path where user and built-in configuration and option
    extensions are found.
    :return:
    """
    global _path
    return _path


def set_search_path(path):
    """
    Set the search path where user and built-in configuration and option
    extensions are found.

    The entry BUILTIN_PATH_SEPARATOR may be included to mark the
    division between user and builtin paths.
    """
    global _path
    _path[:] = path
    diags.debug("Changed mabu search path to: " + os.pathsep.join(_path))


def add_search_path(path):
    """
    Add an entry to the search path if it is not already present
    :param path: new path
    """
    paths = search_path()
    if path not in paths:
        set_search_path(paths + [path])


def format_component_search_path_error(proj_base=None):
    """
    Return a string telling the user where components were searched and
    how to fix search errors.
    :return: str
    """
    searched = [p for p in _path if p != BUILTIN_PATH_SEPARATOR]
    if proj_base:
        searched.insert(0, proj_base)
    paths = os.pathsep.join(searched)
    return "searched " + paths + ";\npass '--path=...' or set " + MABUPATH + " to search elsewhere,\n" + \
        "or run 'mabu --print-components' to see which are available"


def iterate_search_path(user_cb, builtin_separator_cb, builtin_cb):
    """
    Iterate the mabu search path, first iterating user paths (--path ... or MABUPATH=...),
    then indicating when the builtins are starting, then iterating the
    builtin paths (local installation, MLSDK, etc).

    If any callback returns False, stop iterating.
    :param user_cb: called for each entry in user-specified path
    :param builtin_separator_cb: called once between user and builtin entries
    :param builtin_cb: called for each entry in builtin path
    """
    ents = search_path()
    is_builtin = False
    for ent in ents:
        if ent == BUILTIN_PATH_SEPARATOR:
            if not builtin_separator_cb():
                return
            is_builtin = False
        elif is_builtin:
            if not builtin_cb(ent):
                return
        else:
            if not user_cb(ent):
                return


def find_component_on_path(project, comp):
    """
    Look for a component file on the mabu path.  The search order is:
    1) Any user --path entries
    2) Project-relative
    3) Any system --path entries.
    :param project: current project
    :param comp: component name
    :return: path or None
    :rtype: str
    """
    if not comp.endswith(COMP_EXT):
        comp += COMP_EXT

    path = None

    def test_entry(base):
        nonlocal path
        cand = os.path.join(base, comp)
        if os.path.exists(cand):
            path = os.path.realpath(cand)
            return False
        return True

    # search project-relative first
    if project:
        cand = os.path.realpath(os.path.join(project.base, comp))
        if os.path.isfile(cand):
            path = cand
            return path

    # else, look in path
    iterate_search_path(test_entry, lambda: True, test_entry)
    return path


def setup_mabupath(user_project_paths, external_mabu):
    """
    Set up the full mabu search path, with user MABUPATH entries overriding
    the MLSDK search path.

    If @external_mabu, also change the "builtin" paths to point to the mabu
    installed in the new MLSDK.
    """

    dyn_paths = []

    # get project paths
    for proj_path in user_project_paths:
        dirp = os.path.dirname(proj_path)
        if dirp not in dyn_paths:
            dyn_paths.append(dirp)

    # add MLSDK components
    mlsdk_components = os.path.join(runtime_values[MLSDK], '.metadata', 'components')
    if os.path.isdir(mlsdk_components):
        dyn_paths.append(mlsdk_components)

    path = search_path()
    path[0:0] = dyn_paths

    mpath = runtime_values.get(MABUPATH)
    if mpath:
        # prefix user overrides
        ents = mpath.split(os.pathsep)
        path[0:0] = [os.path.expanduser(ent) for ent in ents if len(ent) > 0]

    # update/add builtin paths relative to the running mabu or the external
    try:
        index = path.index(BUILTIN_PATH_SEPARATOR)
    except ValueError:
        # wasn't there
        index = len(path)

    if external_mabu:
        data_dir = os.path.join(runtime_values[MLSDK], 'tools/mabu/data')
    else:
        import config
        data_dir = config.DATA_DIR

    path = path[0:index] + [
        BUILTIN_PATH_SEPARATOR,
        os.path.join(data_dir, 'components'),
        os.path.join(data_dir, 'configs'),
        os.path.join(data_dir, 'options')
    ]

    set_search_path(path)


def add_project(proj):
    """
    :type proj: projects.Project
    """
    key = proj.path
    exist = _projects.get(key, None)
    if exist and exist.path != proj.path:
        raise BuildException("project '{0}' already registered at {1}"
                             .format(exist.name, exist.base))

    if diags.is_debug():
        diags.info("Added project '{0}' at '{1}'".format(key, proj.path))
    _projects[key] = proj


def add_package(proj):
    """
    :type proj: projects.Project
    """
    key = proj.path
    exist = _packages.get(key, None)
    if exist and exist.path != proj.path:
        raise BuildException("package '{0}' already registered at {1}"
                             .format(exist.name, exist.base))

    if diags.is_debug():
        diags.info("Added package '{0}' at '{1}'".format(key, proj.path))

    _packages[key] = proj


def add_component(proj, builtin=True):
    """
    :type proj: projects.Project
    """
    key = proj.path
    exist = _components.get(key, None)
    if exist and exist.path != proj.path:
        if builtin:
            return
        raise BuildException("component '{0}' already registered at {1}"
                             .format(exist.name, exist.base))

    if diags.is_debug():
        diags.info("Added component '{0}' at '{1}'".format(key, proj.path))

    _components[key] = proj


def add_config(proj, builtin=True):
    """
    Add a build configuration (a project with OPTIONS and target flags)
    :type proj: projects.Project
    """
    key = proj.name
    full_key = proj.path
    exist = _configs.get(key, None)
    if exist and exist.path != proj.path:
        if builtin:
            return
        raise BuildException("configuration '{0}' already registered at {1}"
                             .format(exist.name, exist.base))

    if diags.is_debug():
        diags.info("Added config '{0}' at '{1}'".format(key, proj.path))

    global _config_keys
    _config_keys = None
    _configs[key] = proj
    _configs_full_path[full_key] = proj


def add_option(proj, builtin=True):
    """
    Add an option (a project with target flags)
    :type proj: projects.Project
    """
    key = proj.name
    full_key = proj.path
    exist = _options.get(key, None)
    if exist and exist.path != proj.path:
        if builtin:
            return
        raise BuildException("option '{0}' already registered at {1}"
                             .format(exist.name, exist.base))

    if diags.is_debug():
        diags.info("Added option '{0}' at '{1}'".format(key, proj.path))

    global _option_keys
    _option_keys = None
    _options[key] = proj
    _options_full_path[full_key] = proj


def user_projects():
    """
    Get the projects the user explicitly passed on the command line.
    :rtype: list[projects.Project]
    """
    return _user_projects


def projects():
    """ Get the map of projects registered by Project.path
    :rtype: dict[str,projects.Project]
    """
    return _projects


def packages():
    """ Get the map of packages registered by Project.path
    :rtype: dict[str,projects.Project]
    """
    return _packages


def projects_and_packages():
    """ Get the map of packages and projects registered by Project.path
    :rtype: dict[str,projects.Project]
    """
    all_em = dict(_packages)
    all_em.update(_projects)
    return all_em


def project_or_package(path):
    try:
        return projects()[path]
    except KeyError:
        return packages()[path]


def components():
    """ Get the map of components registered by Project.path
    :rtype: dict[str,project.Project]
    """
    return _components


def platform():
    """
    Get the platform object from the current specification.
    :rtype: platforms._base.Platform
    """
    from platforms._base import Platform
    return Platform.find(_spec.platform)


def toolchain():
    """
    Get the toolchain object from the current specification.
    :rtype: toolchains._base.Toolchain
    """
    from toolchains._base import Toolchain
    return Toolchain.find(_spec.toolchain, _spec.platform)


def config():
    """
    Get the configuration project from the current specification
    :rtype: projects.Project
    """
    return _configs.get(_spec.config)


def architecture():
    """
    Get the architecture object from the current specification.
    :rtype: architectures._base.Architecture
    """
    from architectures._base import Architecture
    return Architecture.find(_spec.arch)


def configs():
    """
    Get all the registered configurations
    :rtype: List[str]
    """
    global _config_keys
    if not _config_keys:
        _config_keys = list(_configs.keys())
    return _config_keys


def options():
    """
    Get all the registered options
    :rtype: List[str]
    """
    global _option_keys
    if not _option_keys:
        _option_keys = list(_options.keys())
    return _option_keys


def find_config(name):
    """
    Get the configuration project from @name
    :type name: str
    :rtype: projects.Project
    """
    return _configs.get(name)


def find_option(name):
    """
    Get the option project from the given option name
    :type name: str
    :rtype: projects.Project
    """
    return _options.get(name)


def find_config_from_full_path(path):
    """
    Get the configuration project from @name
    :type path: str
    :rtype: projects.Project
    """
    return _configs_full_path.get(path)


def find_option_from_full_path(path):
    """
    Get the option project from the given option path
    :type path: str
    :rtype: projects.Project
    """
    return _options_full_path.get(path)


def cleanup():
    """
    Remove anything registered or added (e.g. by tests)
    :return:
    """
    global _spec
    _spec = None
    _projects.clear()
    _packages.clear()
    _components.clear()


def cleanup_test_configs_options():
    """
    Remove configs and options named 'test-...'
    """
    global _config_keys, _option_keys
    for key in list(_configs.keys()):
        if key.startswith('test-'):
            del _configs[key]
    _config_keys = None

    for key in list(_options.keys()):
        if key.startswith('test-'):
            del _options[key]

    _option_keys = None


def _register_project_directories(exts=[OPTION_EXT, CONFIG_EXT]):
    """
    Iterate the files along directories in the search path,
    load them as mabu projects, and register them.
    """
    builtin = False

    paths = search_path()
    builtin_roots = _get_recursion_barriers(paths)

    for path in paths:
        if path == BUILTIN_PATH_SEPARATOR:
            builtin = True
            continue

        _register_project_directory(path, builtin=builtin, exts=exts, builtin_roots=builtin_roots)


def _get_recursion_barriers(paths):
    """
    When we search directories for configs, components, and options, we
    want to avoid conflating user directories (e.g. project file directories)
    with builtin search paths, else we will register them twice.
    :param paths:
    :return: culled list of builtin paths we should not hit when recursing
    """
    try:
        bi = paths.index(BUILTIN_PATH_SEPARATOR) + 1
    except ValueError:
        bi = len(paths)

    # these are the points where recursion stops --
    # don't descend into MLSDK or other builtin paths
    mlsdk = runtime_values.get(MLSDK)
    barriers = ([mlsdk] if mlsdk else []) + \
               ([p for p in paths[bi:] if os.path.isdir(p)])

    return barriers


@static_vars(complained=set())
def _register_project_directory(path, base='', builtin=True, exts=[OPTION_EXT, CONFIG_EXT], builtin_roots=[]):
    """
    Iterate the option and config files in the directory 'path',
    load them as mabu projects, and register them.
    Use 'base' as a prefix for any recursively detected entries.
    :param path: the directory
    :param base: base path (e.g. for option/foo)
    :param builtin: mark whether the directory is considered builtin,
    to decide whether to report conflicts
    :param exts: the file extensions of projects to register
    :param builtin_roots: paths for builtin mabu contents, where recursion should stop
    """
    if not builtin and base:
        for p in builtin_roots:
            # don't recursively fetch stuff found in other search paths
            try:
                if os.path.samefile(path, p):
                    return
            except:
                # WinError can be thrown by this
                return

    base_prefix = base and base + '/' or base
    try:
        ents = os.listdir(path)
    except OSError:
        if not base and path not in _register_project_directory.complained:
            _register_project_directory.complained.add(path)
            warn("no such directory found in mabu search path: {0}".format(path))
        return

    # recurse down through either empty directories or directories
    # with real mabu projects
    any_mabu = False
    any_non_mabu = False

    for ent in ents:
        if ent[0] in "._" or 'readme' in ent.lower() or ent == '.metadata':
            continue
        full = os.path.join(path, ent)
        if os.path.isfile(full):
            name, ext = os.path.splitext(ent)
            if ext in exts:
                from factory import load_project
                if ext == CONFIG_EXT:
                    # configs are never registered by path
                    if name in _configs:
                        # avoid later dupes if we overrode one
                        continue
                    proj_name = name
                else:
                    proj_name = base_prefix + name

                factory = load_project(full, name=proj_name, builtin=builtin, filter_non_mabu=True)
                if not factory:
                    continue

                report_error = diags.error
                is_real = True

                if ext in [CONFIG_EXT, OPTION_EXT]:
                    # this is a common extension and may not be intended as a mabu
                    # extension; don't fail on load
                    if not builtin and factory.errors():
                        is_real = False
                        report_error = diags.warn

                factory.report(report_error=report_error)
                if is_real:
                    any_mabu = True
            else:
                any_non_mabu = True

    if any_mabu or not any_non_mabu:
        # recurse only when we see directories that contain files expected in exts
        # or which are otherwise empty of innocuous files
        for ent in ents:
            if ent[0] in "._":
                continue
            full = os.path.join(path, ent)
            if os.path.isdir(full):
                _register_project_directory(full, base_prefix + ent, builtin=builtin, exts=exts, builtin_roots=builtin_roots)
    else:
        diags.debug("stopping mabu search at {}".format(path))


@static_vars(inited=False)
def register_builtins():
    """
    Register all built-in components in the system.
    """
    if register_builtins.inited:
        return
    register_builtins.inited = True

    from platforms._base import Platform
    from toolchains._base import Toolchain
    from architectures._base import Architecture
    from kinds._base import Kind

    Platform.register_directory(platforms)
    Toolchain.register_directory(toolchains)
    Architecture.register_directory(architectures)
    Kind.register_directory(kinds)


_inited = False
if not _inited:
    _inited = True
    register_builtins()


@static_vars(inited=False)
def load_configs_options():
    if not load_configs_options.inited:
        load_configs_options.inited = True
        _register_project_directories()

        # TODO: This does not currently take into account hierarchies more than 1 project deep.
        for config_name in configs():
            config_proj = find_config(config_name)

            if BASE in config_proj.defs:
                base_config_name = config_proj.defs[BASE]
                base_config_proj = find_config(base_config_name)

                # Merge the settings from the BASE config into the one referencing it.
                config_proj.resolve_settings(base_config_proj)


def load_all_components():
    _register_project_directories(exts=[COMP_EXT])


def fetch_scoped_projects(scope):
    """
    Get all the projects accessible with the given scope
    :param scope: 0=all, -1=local, 1=refs
    :return: project set
    """
    visited_project_paths = set()
    visited_projects = list()       # keep ordered!

    if scope <= 0:
        for proj in user_projects():
            if proj.path not in visited_project_paths:
                visited_project_paths.add(proj.path)
                visited_projects.append(proj)

    if scope >= 0:
        for proj_path in projects():
            proj = project_or_package(proj_path)

            if proj in user_projects():
                # already handled above
                continue

            if proj.path not in visited_project_paths:
                visited_project_paths.add(proj.path)
                visited_projects.append(proj)

    return visited_projects
