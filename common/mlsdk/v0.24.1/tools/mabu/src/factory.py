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
This provides a factory to parse projects, components, etc. into data structures
"""
import os
from io import StringIO

import build_env
import kinds
from build_model import Path
from config import PROJ_EXT, COMP_EXT, CONFIG_EXT, OPTION_EXT, PACKAGE_EXT, ALL_EXTS
from diags import warn, error, debug
from projects import MultiVariable, PRIVATE_SUFFIX, is_wild_source
from project_parser import parse_vars
from project_vars import *


_ext_labels = {
    PROJ_EXT: 'project(s)',
    PACKAGE_EXT: 'package(s)',
    COMP_EXT: 'component(s)',
    CONFIG_EXT: 'configuration(s)',
    OPTION_EXT: 'option(s)',
}


class ProjectFactory(object):
    """
    Factory / parser for projects.
    """

    @staticmethod
    def for_path(path, name=None, output=None):
        """
        Create a project factory, referencing a file on disk.
        :param path: full path to project, whose directory is
        the base, and whose base filename is the name
        :param name: if specified, the recorded project name
        (else, the basename of the path)
        :return: new ProjectFactory
        """
        base, basename = os.path.split(path)
        if not name:
            name, ext = os.path.splitext(basename)
        else:
            ext = os.path.splitext(basename)[1]
        return ProjectFactory(name, ext, base, path, output=output)

    @staticmethod
    def for_memory(name, base, output=None):
        """
        Create a project created in memory.
        :param name: name of project
        :param base: base path to project contents
        :param output: user-specified output directory
        :return: new ProjectFactory
        """
        root, ext = os.path.splitext(name)
        if not ext:
            ext = PROJ_EXT
        return ProjectFactory(root, ext, base, name, output=output)

    def __init__(self, name, ext, base, label, output=None):
        self._name = name
        self._ext = ext
        self._base = base
        self._label = label
        from projects import Project
        self._project = Project(self._label, name, base, output=output)
        self._errors = []
        self._warnings = []

    @property
    def extension(self):
        return self._ext

    def parse(self, fp):
        """
        Parse content from the filelike @fp.
        :param fp: filelike for text
        :rtype: dict[str,projects.MultiVariable]
        """
        vars = parse_vars(fp, self._ext, self._label, self._errors, self._warnings)
        self._project.defs.update(vars)
        self._project.flush_scopes()

        return

    def file_error(self, text):
        self._errors.append((self._label, None, text))

    def file_warning(self, text):
        self._warnings.append((self._label, None, text))

    def cleanup_config_vars(self):
        defs = self._project.defs
        base_config = defs.get(BASE)

        # Resolve BASE to a list.
        if isinstance(base_config, MultiVariable):
            base_config = base_config.derive(build_env.spec())

        if base_config:
            if len(base_config) == 0:
                self.file_error("BASE cannot be blank")
            elif len(base_config) > 1:
                self.file_error("only one string expected for BASE, got: {}".format(base_config))
                defs[BASE] = base_config[0]
            else:
                defs[BASE] = base_config[0]

    def merge_vars(self):
        """
        Merge all the variables in the project, presumably expanded and merged
        from the configuration and its options, using the current build spec
        to select the result.

        The project defs consist of name -> MultiVariable entries.  After
        this call, the project will contain well-defined
        values.
        """
        allowed_vars = extension_to_vars[self._ext]
        spec = build_env.spec()

        if self.extension not in [PACKAGE_EXT, PROJ_EXT, COMP_EXT]:
            spec = None

        self._project.flatten_defs(spec)
        defs = self._project.defs

        for scope in [PRIVATE_SUFFIX, '']:
            # KIND is not a list
            varname = KIND + scope
            kind = defs.get(varname)
            if kind is not None:
                if len(kind) == 0:
                    self.file_error("KIND cannot be blank")
                elif len(kind) != 1:
                    # make sure the KIND is known
                    self.file_error("only one string expected for KIND, got: {0}".format(kind))
                    defs[varname] = kind[0]
                else:
                    defs[varname] = kind[0]

            # OUTNAME may not provide multiple items (in one project),
            # but we keep it as a list so we can override
            varname = OUTNAME + scope
            outname = defs.get(varname)
            if outname:
                if len(outname) == 0:
                    self.file_error("OUTNAME cannot be blank")
                elif len(outname) > 1:
                    self.file_error("only one string expected for OUTNAME, got: {}".format(outname))

            # OUT may not provide multiple items (in one project),
            # but we keep it as a list so we can override
            varname = OUT + scope
            out = defs.get(varname)
            if out is not None:
                if len(out) == 0:
                    self.file_error("OUT cannot be blank (use '.'?)")
                elif len(out) > 1:
                    self.file_error("only one string expected for OUT, got: {0}".format(out))

        # resolve REFS/USES/OPTIONS to actual paths
        if REFS in allowed_vars:
            self._fetch_refs(defs, self._project)
        if USES in allowed_vars:
            self._fetch_uses(defs, self._project)
        if OPTIONS in allowed_vars:
            self._fetch_options(defs, self._project)

        self._fetch_configs(self._project)
        self._project.flush_scopes()

    def _fetch_refs(self, defs, project):
        # ensure all the referenced projects are resolved
        all_refs = defs.get(REFS, [])
        for ref in all_refs:
            # canonicalize the name, if needed
            if ref.endswith(PROJ_EXT):
                self.file_warning("project name ('{1}') should not include extensions in REFS in {0}"
                                  .format(project.name, ref))
            else:
                ref += PROJ_EXT

            # see if it's pre-registered (e.g. testing)
            if ref in build_env.projects():
                path = ref
            else:
                # construct expected path
                path = os.path.realpath(os.path.join(project.base, ref))
                if not os.path.exists(path):
                    self.file_error("cannot locate project '{1}' referenced in REFS of project {0}; "
                                    "it should live relative to {2}"
                                    .format(project.name, ref, project.base))
                    continue

            build_env.add_search_path(os.path.dirname(path))
            project.refs.append(path)

    def _fetch_uses(self, defs, project):
        # ensure all the referenced components are resolved
        all_uses = defs.get(USES, [])
        for comp in all_uses:
            # canonicalize the name, if needed
            if comp.endswith(COMP_EXT):
                self.file_warning("component name ('{1}') should not include extensions in USES in {0}"
                                  .format(project.name, comp))
            else:
                comp += COMP_EXT

            # see if it's pre-registered (e.g. testing)
            if comp in build_env.components():
                path = comp
            else:
                # these are found in our components or user components

                # (note: components are loaded on demand, since they refer to existing
                # options and configs and must be loaded last)
                path = build_env.find_component_on_path(project, comp)

                if not path:
                    self.file_error("cannot locate component '{1}' referenced in USES of "
                                    "project {0};\n{2}"
                                    .format(project.name, comp, build_env.format_component_search_path_error(project.base)))
                    continue

            project.uses.append(path)

        debug("project '{0}' defs: {1}".format(self._project.name, self._project.defs))

    def _fetch_options(self, defs, project):
        if not project.options:
            # ensure all the referenced options are resolved
            all_options = defs.get(OPTIONS, [])
            all_options_local = defs.get(OPTIONS + PRIVATE_SUFFIX, [])

            # If MultiVariable, extract the list.
            if isinstance(all_options, MultiVariable):
                all_options = all_options.derive(build_env.spec())

            if isinstance(all_options_local, MultiVariable):
                all_options_local = all_options_local.derive(build_env.spec())

            for option in all_options + all_options_local:
                # canonicalize the name, if needed
                if option.endswith(OPTION_EXT):
                    self.file_warning("option name ('{1}') should not include extensions in OPTIONS in {0}"
                                      .format(project.name, option))
                    option = os.path.splitext(option)[0]

                # Options can only be pre-registered.
                if option in build_env.options():
                    option_proj = build_env.find_option(option)
                    path = option_proj.path

                    if path not in project.options:
                        project.options.append(path)
                        project.direct_options.append(path)

                        self._fetch_options(option_proj.defs, option_proj)

                        for sub_option_path in option_proj.options:
                            if sub_option_path not in project.options:
                                project.options.append(sub_option_path)

    def _fetch_configs(self, project):
        build_spec = build_env.spec()
        if build_spec:
            current_config = build_spec.config
            config_proj = build_env.find_config(current_config)

            if config_proj.path not in project.configs:
                project.configs.append(config_proj.path)
                project.direct_configs.append(config_proj.path)

                self._fetch_options(config_proj.defs, config_proj)

                for sub_option_path in config_proj.options:
                    if sub_option_path not in project.options:
                        project.options.append(sub_option_path)

    def validate(self):
        """
        Verify that settings and files/directories referenced by the project are valid.

        Adds warnings/errors to the receiver if problems are detected.
        """
        proj = self._project

        if self._ext not in extension_to_vars:
            # will fail elsewhere
            return

        allowed_vars = extension_to_vars[self._ext]

        if KIND in allowed_vars:
            from kinds._base import Kind
            kind = proj.locals.get(KIND)
            if kind is not None:
                # make sure the KIND is known
                if kind not in Kind.all():
                    self.file_error("unknown project KIND '{0}'; should be one of: {1}".format(kind, ", ".join(Kind.all())))
            else:
                self.file_error("KIND is not set; should be one of: {0}".format(", ".join(Kind.all())))
                proj.defs[KIND + PRIVATE_SUFFIX] = kinds.program.NAME
                proj.flush_scopes()

        for name in proj.defs:
            # get the base name
            test_name = name
            if test_name.endswith(PRIVATE_SUFFIX):
                test_name = test_name[:-len(PRIVATE_SUFFIX)]

            # see if it's not allowed (note: totally unknown vars or undesired exports already checked)
            if test_name not in allowed_vars and test_name in ALL_VARS:
                self.file_error("this kind of file may not define '{0}'".format(test_name))

        # validate existence of referenced files and directories
        if self._ext in [PROJ_EXT, COMP_EXT, PACKAGE_EXT]:
            for var in [SRCS, OBJS, ASSETS, SRC_EXTENSIONS, EXTENSIONS]:
                for f in proj.fullpaths(var):
                    if not os.path.isfile(f):
                        self.file_error("{0}: no such file '{1}'".format(var, f))

            for var in [INCS, LIBPATHS]:
                for d in proj.fullpaths(var):
                    if not os.path.isdir(d):
                        self.file_warning("{0}: no such directory '{1}'".format(var, d))

    def _process_mapping(self, source, target):
        """
        Process a mapping from DATAS where :source: and :target:
        are canonicalized.  :target: is None if it was not specified
        by the user.
        :param factory: factory for reporting errors
        :param source: the source
        :param target: the destination
        :return: tuple of source path and destination path;
        destination ends in '/' IFF the source is a wildcard
        """
        # canonicalize
        source = source.replace('\\', '/')
        if target:
            target = target.replace('\\', '/')

        source_path = Path.parse(source)
        if not target and (source_path.is_absolute() or '..' in str(source_path)):
            # avoid baking in absolute paths into packages
            # (which also deletes the source during a clean!)
            # or dotty paths that aren't secure
            self.file_warning("source is outside project ({0}); assuming target directory is at root "
                              "(add ': ./' to suppress this warning)".format(source))
            target = './'

        if is_wild_source(source):
            # validate wildcard restrictions
            if not target:
                target = './'
            elif '*' in target or '?' in target:
                self.file_error("Sorry, cannot use wildcards in destination ({0} : {1})".format(
                    source, target))

                return None
            elif not target.endswith('/'):
                self.file_warning("Wildcard sources should specify a target ending in '/' ({0} : {1})"
                                  .format(source, target))
                target += '/'

        elif source.endswith('/'):
            # validate directory expansion
            if not target:
                target = './'
            elif not target.endswith('/'):
                self.file_warning("Directory sources should specify a target ending in '/' ({0} : {1})"
                                  .format(source, target))
                target += '/'

        else:
            # single file copy
            if not target:
                # files go to same source location
                target = source
            elif target.endswith('/'):
                # targeting a directory with same filename as source
                target = target + os.path.split(source)[1]
            else:  # source -> renamed target
                pass

        if '../' in target and build_env.platform().uses_packages():
            # uhoh, this is fishy -- make sure it's not something like
            # "$(MYVAR)/../elsewhere" which may resolve to something legal
            target = os.path.normpath(target)
            # re-canonicalize...
            target = target.replace('\\', '/')

            if '../' in target:
                self.file_error("Cannot place files outside a layout or package ({0} : {1})"
                                .format(source, target))
                return None

        return source, target

    def _process_mappings_list(self, lst):
        """
        Convert mappings in the form:

        src ':' dst

        or

        src    # dst == src, unless full/.. path, then basename

        to a list of tuples of (src, dst).

        This may leave wildcards and directories in place.
        Those are handled in the PackageUpdater.
        :param lst: flat list
        :return: list of tuples
        """
        mappings = []
        length = len(lst)

        i = 0
        while i < length:
            source = lst[i]
            target = None

            has_target = (i + 1) < length and lst[i + 1] == ':'
            if has_target:
                if (i + 2) < length and lst[i + 2] != ':':
                    target = lst[i + 2]
                    if target == '.':
                        target = './'

                    i += 3
                else:
                    self.file_error("No destination found following ' : '")
                    # stop processing, since the syntax is borked
                    return mappings

            else:
                if source == ':':
                    self.file_error("No source found preceding ' : '")
                    # stop processing, since the syntax is borked
                    return mappings

                i += 1

            entry = self._process_mapping(source, target)
            if not entry:
                continue

            mappings.append(entry)

        return mappings

    def _process_mappings(self, lst):
        if isinstance(lst, MultiVariable):
            map = MultiVariable()
            for key, val in lst.items():
                map.set(key, self._process_mappings_list(val))
            return map
        else:
            return self._process_mappings_list(lst)

    def _process_src_extensions(self, lst):
        if isinstance(lst, MultiVariable):
            return lst

        tuples = []
        length = len(lst)
        i = 0
        while i < length:
            mk = lst[i]
            if i + 1 >= length or lst[i + 1] != ':':
                self.file_error("Expected ' : ' after makefile")
                break
            if i + 2 >= length:
                self.file_error("Missing source extension")
                break

            src_ext = lst[i + 2]
            if src_ext.startswith('%'):
                src_ext = src_ext[1:]

            if '/' in src_ext or '\\' in src_ext:
                self.file_error("Got an unexpected path in source extension: " + src_ext)
                break

            if i + 3 >= length or lst[i + 3] != '->':
                self.file_error("Expected ' -> ' after source extension")
                break
            if i + 4 >= length:
                self.file_error("Missing destination extension")
                break

            dst_path, dst_ext = os.path.split(lst[i + 4])
            if '..' in dst_path:
                self.file_error("Cannot generate code outside project")
                break

            if dst_ext.startswith('%'):
                dst_ext = dst_ext[1:]

            i += 5

            tuples.append((
                Path(self.project().base_path(), mk),
                src_ext,
                dst_ext,
                dst_path and Path.parse(dst_path) or None
            ))
            # please see `print_resolved_projects` for another client of this

        return tuples

    def convert_to_type(self, var):
        proj = self.project()
        listval = proj.defs.get(var)
        if listval is not None:
            if var.startswith(DATAS):
                # DATAS is a list of mappings -- make it into a list of tuples
                processed = self._process_mappings(listval)
            elif var.startswith(SRC_EXTENSIONS):
                # SRC_EXTENSIONS is structured too -- make into a list of tuples
                processed = self._process_src_extensions(listval)

            proj.defs[var] = processed

    def project(self):
        return self._project

    def errors(self):
        return self._errors

    def warnings(self):
        return self._warnings

    def report(self, report_error=error):
        for wrn in self.warnings():
            warn(wrn[2], file=wrn[0], line=wrn[1])

        any = False
        for err in self.errors():
            report_error(err[2], file=err[0], line=err[1], die=False)
            any = True

        proj_ext = self.extension
        if any:
            report_error("one or more {0} (*{1}) failed to load"
                  .format(_ext_labels.get(proj_ext), proj_ext))


def load_projects_and_dependencies(projects, outdirs, set_options, config):
    """
    Load projects named in list and populate build_env.{user_projects,projects,packages}
    :param projects: List of project paths
    :param outdirs: map of project name to output directory
    :param config: base configuration
    :type config: projects.Project
    """
    for prj in projects:
        prj_proj = load_project_and_dependencies(prj, outdirs, set_options=set_options, base=config)
        if prj_proj:
            build_env.user_projects().append(prj_proj)


def is_binary_content(content):
    textchars = bytearray({7, 8, 9, 10, 12, 13, 27} | set(range(0x20, 0x100)) - {0x7f})
    return bool(content.translate(None, textchars))


def load_project(path, name=None, text=None,
                 output=None,
                 validate=True, register=True,
                 builtin=True, filter_non_mabu=False):
    """
    Load a project -- from disk if @text is None, else from the string @text.
    Merge in the base configuration, if provided.
    Expand the resulting project's OPTIONS, and flatten
    all definitions for projects and components.

    Returns any warnings or errors in the returned ProjectFactory.
    Does not load or resolve REFS or USES.
    :param path: path to project
    :param name: name of project, if specified (uses basename of @path otherwise)
    :param text: if not None, the text to load for the project (uses file at @path otherwise)
    :param validate: if False, do not validate the new project
    :param register: if False, do not register the new project
    :param builtin: mark whether the directory is considered builtin,
    to decide whether to report conflicts
    :return: ProjectFactory
    """
    from build_model import BuildException

    factory = ProjectFactory.for_path(path if text else os.path.normpath(path), name=name, output=output)
    project = factory.project()
    proj_ext = factory.extension

    if text is None:
        with open(path, 'rb') as fp:
            text = fp.read()

        text = text.decode('utf-8', errors='replace')
        stripped = text.strip()
        if stripped:
            ch_first = ord(stripped[0])
            if ch_first == 0xfeff or ch_first == 0xfffe:
                # ignore BOMs
                stripped = stripped[1:]

        invalid_char_count = len([ch for ch in text if ord(ch) == 0xfffd])

        # ignore obvious non-mabu files (binaries)
        # (these appear as files whose converted text has a lot of 0xfffd
        # replacement chars)
        if filter_non_mabu:
            if invalid_char_count > min(64, len(stripped) // 4):
                return None

            # ignore XML files (e.g. nuget.config)
            if stripped.startswith("<?xml") or stripped.startswith("<!--"):
                return None

    factory.parse(StringIO(text))

    if proj_ext in [PROJ_EXT, PACKAGE_EXT, COMP_EXT]:
        # merge everything for this build
        factory.merge_vars()
    elif proj_ext in [CONFIG_EXT]:
        factory.cleanup_config_vars()

    for structured in [DATAS, SRC_EXTENSIONS]:
        factory.convert_to_type(structured)
        factory.convert_to_type(structured + PRIVATE_SUFFIX)

    project.flush_scopes()

    if validate:
        factory.validate()

    if register:
        if proj_ext == PROJ_EXT:
            build_env.add_project(project)      # cannot be builtin
        elif proj_ext == PACKAGE_EXT:
            build_env.add_package(project)      # cannot be builtin
        elif proj_ext == COMP_EXT:
            build_env.add_component(project, builtin)
        elif proj_ext == CONFIG_EXT:
            build_env.add_config(project, builtin)
        elif proj_ext == OPTION_EXT:
            build_env.add_option(project, builtin)
        else:
            raise BuildException("unknown project file extension '{0}', please use one of {1}" \
                                .format(proj_ext, ", ".join("'" + x + "'"
                                                            for x in ALL_EXTS)))

    return factory


def load_project_and_dependencies(path, outdirs, base=None, name=None, set_options=None):
    """
    Load a project and resolve all its variables, as in load_project().

    If any warnings or errors are detected, dump them, and raise BuildException on error.

    If no errors are found, recursively load REFS and USES.
    :param path: path to project
    :param outdirs: map of project name to user-specified output directory
    :param base: base config
    :type base: projects.Project
    :param name: name of project, if specified (uses basename of path otherwise)
    :return: top-level project for path
    :rtype: projects.Project
    """

    def find_output(proj):
        if not outdirs:
            return None
        out = outdirs.get(proj)
        if out:
            return out

        # match pathy references
        proj_name = os.path.splitext(os.path.basename(proj))[0]
        proj_name = proj_name.replace(' ', '_')

        if proj_name in outdirs:
            return outdirs[proj_name]

        # for key in outdirs:
        #     if proj_name == os.path.basename(key):
        #         return outdirs[key]

        return None

    factory = load_project(path, name, output=find_output(path))
    if not factory:
        return None

    project = factory.project()

    if set_options:
        project.locals[OPTIONS] += set_options

    factory.report()

    # ensure all the referenced projects are loaded
    for ref in project.refs:
        if ref in build_env.projects():
            continue

        load_project_and_dependencies(ref, outdirs, base=base, set_options=set_options)

    # ensure all the referenced components are loaded
    for use in project.uses:
        if use in build_env.components():
            continue

        # components
        load_project_and_dependencies(use, outdirs, base=base, set_options=set_options)

    return project

