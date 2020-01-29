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
Generate Makefiles from the project model.
"""
import os

import build_env
from build_model import Variable, Buildable, Path, Command, Rule, EnvironmentVariable, InfoCommand, Include, \
    VariableDefinition, Text, Directive, split_by_length, Composition
from config import TOPLEVEL_MAKEFILE_NAME
from diags import debug
from kinds._base import Kind
from project_vars import KIND, EXTENSIONS, SRC_EXTENSIONS
from run_vars import SPEC, HOST, runtime_values, MAKE
from utils import mkdir_p, host_path


HEADER = """
# this turns off the suffix rules built into make
.SUFFIXES:

# this turns off the RCS / SCCS implicit rules of GNU Make
% : RCS/%,v
% : RCS/%
% : %,v
% : s.%
% : SCCS/s.%

# If a rule fails, delete $@.
.DELETE_ON_ERROR:

ifeq ($(VERBOSE),)
ECHO=@
else
ECHO=
endif

ifeq ($(QUIET),)
INFO=@echo
else
INFO=@:
endif

ifeq ($(VERBOSE),)
SPAM=@: \#
else
SPAM=@echo
endif
"""


class MakeGen(object):
    def __init__(self, proj_map, comp_map, scope):
        """
        :type proj_map: Dict[str,projects.Project]
        :type comp_map: Dict[str,projects.Project]
        """
        self._proj_map = proj_map
        self._comp_map = comp_map

        self._scope = scope

        self._mkfiles = []
        """ :type: List[Tuple[str,Tuple[Node|str,Tuple[List[Directive|Rule]]]]] """

        self._included = []
        """ :type: List[str] """

        self._dfs = []
        """ :type: list[projects.Project] """

        self._top = None
        """ :type: str """

    def create_build_schedule(self, projects):
        """
        Derive a build schedule, based on the actual project dependency ordering,
        and the build scope.  This resolves all the projects along the way.

        :param projects: the projects passed on the command line
        :type projects: list[projects.Project]
        :rtype: list[ProjectInfo]
        """

        def perform_build_resolved_project(proj):

            # the project kind is responsible for the work
            if proj.is_component():
                # a component is not built
                return False

            # resolve options to raw target flags
            proj.resolve_settings(build_env.config())

            if proj.is_package():
                # a package is handled outside
                return False

            return True

        schedule = []
        """ :type: list[ProjectInfo] """

        # go through every possible project, and resolve it, then determine whether
        # we should generate its makefiles

        handled = set()

        for proj in projects:
            # get dependencies...
            dfs = proj.dependency_list(self._proj_map, self._comp_map)

            # reverse them (leaf nodes first)
            dfs.reverse()

            # iterate projects, resolving their variables, and see which are buildable
            to_build = [sub for sub in dfs + [proj] if perform_build_resolved_project(sub)]

            if proj not in handled:
                if proj in to_build:
                    info = ProjectInfo(proj)
                    info.dfs = dfs
                    info.write_makefiles = self._scope <= 0
                    schedule.append(info)
                handled.add(proj)

            for sub in dfs:
                if sub not in handled:
                    if sub in to_build:
                        info = ProjectInfo(sub)
                        info.dfs = sub.dependency_list(self._proj_map, self._comp_map)
                        info.write_makefiles = self._scope >= 0
                        schedule.append(info)
                    handled.add(sub)

        # go back through and patch up the list of built dependent projects
        proj_map = {}
        for info in schedule:
            proj_map[info.proj] = info
            if info.proj.all_paths(EXTENSIONS) or info.proj.all_paths(SRC_EXTENSIONS):
                info.has_extensions = True
                info.extension_makefile = Path(info.proj.outdir_path(), info.proj.name + '.extensions.mk')

        debug("Generating project '{0}'...".format(proj.name))
        for info in schedule:

            # gather the rules (needed even if we don't build, since it derives
            # build artifacts used by dependency rules and the packager)
            kind = Kind.find(info.proj.locals[KIND])
            if kind:
                """ :type kind: build_model.Kind """
                info.rules = kind.generate(info.proj, build_env)

            info.extension_dependencies = []
            for dep in info.dfs:
                dep_info = proj_map.get(dep)
                if dep_info:
                    if dep_info.has_extensions and dep_info.write_makefiles:
                        info.extension_dependencies.append(dep)

        return schedule

    def generate(self, projects):
        """
        Generate the projects, from leaves up to the root.
        :type projects: List[projects.Project]
        :param scope: 0 for all, -1 for local, 1 for refs
        """
        schedule = self.create_build_schedule(projects)

        self._included = []

        for info in schedule:
            if info.write_makefiles:
                self._generate_one(info)
                self._dfs.append(info.proj)

        # make a main makefile that invokes the others
        self._generate_top([info.proj for info in schedule])

    def _generate_one(self, info):
        """
        Generate one project, whose dependent projects have already been
        generated.
        :type info: ProjectInfo
        """
        proj, rules, ext_dependency_projects = info.proj, info.rules, info.extension_dependencies

        # HACK: get the "main" rule -- what the project really does
        main_rules = [rule for rule in rules if isinstance(rule, Rule)] [0 : 1]

        ext_proj_deps = [Buildable(proj.name + '-all') for proj in ext_dependency_projects]

        # get the "main" output -- which the first rule should target
        orig_targets = [rule.buildable for rule in main_rules]

        rules.append(Text(''))

        rules.append(self._create_clean_rule(info))

        project_deps = ext_proj_deps + orig_targets

        build_rule = Rule(None, Buildable(proj.name_var() + '-all'),
                          [Buildable('-make-directories')]
                          + project_deps)
        build_rule.multi_target = True
        rules.append(build_rule)

        if info.has_extensions:
            self._add_ext_invoke_rules(info, build_rule, rules)

            # write extension support to an independent file
            ext_rules = self._create_ext_rules(proj)
            debug("extension rules: {}", ext_rules)
            self._mkfiles.append((proj.name + '.extensions', (info.extension_makefile, ext_rules)))
            self._included.append(info.extension_makefile)

        debug("rules: {}", rules)
        mks = Path(proj.outdir_path(), proj.name + '.mk')
        self._mkfiles.append((proj.name, (mks, rules)))
        self._included.append(mks)

    @staticmethod
    def _add_ext_invoke_rules(info, build_rule, rules):
        """
        Add rules needed to invoke an extension makefile
        :type info: ProjectInfo
        :param build_rule:
        :param rules:
        """
        proj = info.proj

        # invoke a sub-make to build any projects using extensions;
        # such extension makes must use non-parallel invocations (which
        # can only be done when invoking a sub-make)
        ext_prebuild = Buildable(proj.name_var() + '-prebuild')
        ext_postbuild = Buildable(proj.name_var() + '-postbuild')

        # Make sure build serializes order of prebuild, targets, and postbuild
        build_rule.order_only_dependencies[:] = [Buildable('-make-directories')] \
            + [ext_prebuild] \
            + build_rule.dependencies[1:] \
            + [ext_postbuild]

        build_rule.dependencies[:] = []

        for target_suffix in ('prebuild', 'postbuild'):
            target = Buildable(proj.name_var() + '-' + target_suffix)
            ext_target = Buildable(proj.name_var() + '-ext-' + target_suffix)

            # don't say "nothing to be done for..."
            rules.append(Directive('.PHONY:', target, ext_target))

            extension_build_rule = Rule(None, target, [], [])
            extension_build_rule.multi_target = True
            rules.append(extension_build_rule)

            make_args = [Variable(MAKE, None),
                         '--no-print-directory',
                         '-f', info.extension_makefile,
                         ext_target]
            run_make_command = Command(make_args)
            extension_build_rule.commands.append(run_make_command)

        # mark every file we suspect of being generated via SRC_EXTENSIONS
        # as dependent on its makefile
        for proj, mk, _, to_path in proj.generated_sources:
            dep_rule = Rule(proj, Buildable(Path(proj.base_path(), to_path)), [mk])
            rules.append(dep_rule)

    @staticmethod
    def _create_ext_rules(proj):
        """
        Create rules, for an extension makefile project wrapper,
        containing the logic for prebuild, postbuild, and clean rules
        that may be augmented by extension makefiles
        :type proj: projects.Project
        :return: list of new rules
        """
        rules = []

        header = Text(HEADER)
        rules.append(header)

        extensions = proj.all_paths(EXTENSIONS)
        src_extensions = [path for (path, _, _, _) in proj.combined[SRC_EXTENSIONS]]

        safe_name = proj.name_var()

        rules.append(VariableDefinition('PROJECT', '=', safe_name))

        # export $(PROJECT)_SRCS as list of pre-expanded source file paths
        srcs_paths = [Path(proj.base_path(), path) if proj else path
                      for (proj, path) in proj.all_original_source_files()]
        srcs_list = Composition(*srcs_paths, sep=' ')
        rules.append(VariableDefinition(safe_name + '_SRCS', '=', srcs_list))

        # export $(PROJECT)_BUILD_SRCS as list of source file paths built by tools
        all_srcs_paths = [Path(proj.base_path(), path) if proj else path
                      for (proj, path) in proj.all_source_files()]
        all_srcs_list = Composition(*all_srcs_paths, sep=' ')
        rules.append(VariableDefinition(safe_name + '_BUILD_SRCS', '=', all_srcs_list))

        # export $(PROJECT)_OBJS as list of post-build object file paths
        objs_paths = proj.all_object_files()
        objs_list = Composition(*objs_paths, sep=' ')
        rules.append(VariableDefinition(safe_name + '_OBJS', '=', objs_list))

        # export $(PROJECT)_OUTPUT_FILE as fully-resolved artifact path
        rules.append(VariableDefinition(safe_name + '_OUTPUT_FILE', '=',
                                        Path(proj.outdir_path(), proj.output_filename()),
                                        export=True))

        rules.append(Text(''))

        # make hooks for prebuild/postbuild/clean
        for target_name in ('prebuild', 'postbuild', 'clean'):
            ext_target = Buildable(proj.name_var() + '-ext-' + target_name)

            # this, plus "@:" before, suppress "nothing to be done for..."
            rules.append(Directive('.PHONY:', ext_target))

            ext_rule = Rule(None, ext_target, [])
            ext_rule.multi_target = True
            ext_rule.commands.append(Command("@:"))
            rules.append(ext_rule)

        saw_mks = set()
        for path in extensions:
            resolved = path.resolve()
            if resolved not in saw_mks:
                saw_mks.add(resolved)
                rules.append(Include('include', path))

        for path in src_extensions:
            resolved = path.resolve()
            if resolved not in saw_mks:
                saw_mks.add(resolved)
                rules.append(Include('include', path))

        return rules

    def _generate_top(self, handled):
        """
        Generate the top-level makefile that includes all the
        others and builds them, and contains the top-level

        :param handled: projects generated
        :type handled: list[projects.Project]
        """
        self._top = None
        if not handled:
            return

        directives_rules = []

        # put the main makefile at the shallowest output directory,
        # but make it hidden so it won't conflict with a project named 'build'
        for proj in handled:
            if proj.is_component():
                continue

            mk = Path(proj.outdir_path(), TOPLEVEL_MAKEFILE_NAME + '.mk')
            if not self._top or len(str(self._top)) > len(str(mk)):
                self._top = mk

        self._add_toplevel_rules(directives_rules)

        self._add_create_directories_rule(directives_rules)

        self._add_project_includes(directives_rules)

        self._add_build_rules(directives_rules)

        self._mkfiles.append((None, (self._top, directives_rules)))

    def _add_toplevel_rules(self, directives_rules):
        header = Text(HEADER)
        directives_rules.append(header)

        directives_rules.append(Rule(None, Buildable('all'), [Buildable('prebuild'), Buildable('build'), Buildable('postbuild')]))

        prebuild_rule = Rule(None, Buildable('prebuild'), [])
        prebuild_rule.multi_target = True
        directives_rules.append(prebuild_rule)

        postbuild_rule = Rule(None, Buildable('postbuild'), [])
        postbuild_rule.multi_target = True
        directives_rules.append(postbuild_rule)

        clean_rule = Rule(None, Buildable('clean'), [
            Buildable(proj.name_var() + '-clean') for proj in self._dfs
            ])
        clean_rule.multi_target = True
        directives_rules.append(clean_rule)

        full_external_dep_map = dict()

        # Fill a dictionary with the external dependencies for each project.
        for proj in self._dfs:
            external_deps = proj.get_all_external_dependencies()
            for dep in external_deps:
                resolved_dep_path = dep.makefile_str()
                if resolved_dep_path not in full_external_dep_map:
                    full_external_dep_map[resolved_dep_path] = dep

        # Add empty rules for each external project dependency.
        for _, dep in full_external_dep_map.items():
            directives_rules.append(Rule(None, Buildable(dep), []))

        # expose the project-type variables
        self._define_project_type_vars(directives_rules, 'PROGRAM')
        self._define_project_type_vars(directives_rules, 'SHARED')
        self._define_project_type_vars(directives_rules, 'IMPLIB')
        self._define_project_type_vars(directives_rules, 'STATIC')

        # expose tool prefix
        directives_rules.append(VariableDefinition('COMPILER_PREFIX', '=', runtime_values.get('COMPILER_PREFIX', "")))
        directives_rules.append(VariableDefinition('LINKER_PREFIX', '=', runtime_values.get('LINKER_PREFIX', "")))

        directives_rules.append(Text(''))

    @staticmethod
    def _define_project_type_vars(directives, name):
        directives.append(VariableDefinition(name + '_PREFIX', '=', runtime_values[name + '_PREFIX']))
        directives.append(VariableDefinition(name + '_EXT', '=', runtime_values[name + '_EXT']))

    @staticmethod
    def _create_clean_rule(info):
        """
        :type info: ProjectInfo
        :return: build_model.Rule
        """

        proj = info.proj
        outfiles = proj.all_outputs()

        target = Buildable(proj.name_var() + '-clean')
        clean_rule = Rule(None, target, [])
        clean_rule.add_command(InfoCommand("Cleaning {0}...".format(proj.name)))
        clean_rule.multi_target = True

        # split a potentially long command line into pieces
        file_batches = split_by_length(outfiles)
        rm_args = [Variable("RM", "rm"), "-rf"]

        # Create a command for each batch of files.
        for batch in file_batches:
            clean_rule.add_command(Command(rm_args + batch))

        if info.has_extensions:
            make_args = [Variable(MAKE, None),
                         '--no-print-directory',
                         '-f', info.extension_makefile,
                         Buildable(proj.name_var() + '-ext-clean')]
            run_make_command = Command(make_args)
            clean_rule.commands.append(run_make_command)

        return clean_rule

    def _add_build_rules(self, directives_rules):
        # depend on each subproject
        deps = []

        for proj in self._dfs:
            if proj.is_component():
                continue
            deps.append(Path(proj.name_var() + '-all'))

        # ensure the projects are built in this strict order
        build_rule = Rule(None, Buildable('build'), order_dependencies=deps)

        directives_rules.append(build_rule)

    def all_outputs(self):
        """
        Get all the output Paths we expect to create from the project
        (objects, dependencies, packages, copied assets, etc.)
        :rtype: list[Path]
        """
        outs = set()

        for project in self._proj_map.values():
            outputs = project.all_outputs()
            outs = outs.union(outputs)

        outs_l = list(outs)
        outs_l.sort()

        return outs_l

    def all_output_directories(self):
        """
        Get all the output directories we expect to need --
        in addition to the basic .out + build-spec directory,
        this includes paths inside that, corresponding to
        relative paths for source files, data files, etc.
        :rtype: list[Path]
        """
        directories = set()

        # NOTE: we flatten the directories to strings and put them in a set
        # here to avoid reporting the same physical directory more than once,
        # e.g. when multiple projects are built into the same dir,
        # which ends up spewing "*** Ignoring multiple rules for ..../dir" warnings
        for out in self.all_outputs():
            try:
                # prefer removing the last segment before collapsing variables,
                # to avoid making broken results from $(OUTDIR)/$(OTHERDIR)/thing
                dirpath = out.directory.value()
            except AttributeError:
                # else flatten the whole thing
                dirpath = os.path.split(out.value())[0]
            directories.add(dirpath)

        directories_s = list(directories)
        directories_s.sort()

        return directories_s

    def _add_create_directories_rule(self, directives_rules):
        """
        Create all the output directories we are aware of.  These are
        not just the directories we've already created to house
        the *.mk files, but also any required by object files in
        nested directories.
        :param directives_rules:
        :return:
        """

        outdirs = self.all_output_directories()

        make_directories_rule = Rule(None, Buildable('-make-directories'),
                                     [Path(d) for d in outdirs])
        directives_rules.append(make_directories_rule)

        for dir in outdirs:
            make_dir_rule = Rule(None, Buildable(dir), [])
            make_dir_rule.add_command(Command(['@mkdir', '-p', dir]))
            directives_rules.append(make_dir_rule)

    def _add_project_includes(self, directives_rules):
        # include the other generated Makefiles
        for mk in self._included:
            directives_rules.append(Include('include', mk))

    def gather_vars(self):
        """
        Gather the variable definitions for the makefiles
        :rtype: Dict[str,Dict[str,build_model.Node]]
        """

        var_map = {}

        for proj, (_, directives_rules) in self._mkfiles:
            mk_vars = self._gather_vars(directives_rules)
            var_map[proj] = mk_vars

        return var_map

    def write(self):
        """
        Write the generated Makefiles to disk
        :return: the top.level.mk makefile path
        """

        var_map = self.gather_vars()

        for proj, (mkv, directives_rules) in self._mkfiles:
            mk = mkv.value()
            mdir = os.path.dirname(mk)

            # I changed this...
            if os.path.exists(mdir) and not os.path.isdir(mdir):
                os.unlink(mdir)
            mkdir_p(mdir)

            with open(host_path(mk), 'wt', buffering=4096, encoding='utf-8') as fp:
                debug("Writing {0} ...".format(mk))

                print("# Generated Makefile -- DO NOT EDIT!\n", file=fp)

                self._write_vars(fp, var_map[proj])

                for ent in directives_rules:
                    print(ent.makefile_str(), file=fp)

        for _, project in build_env.projects().items():
            for path, text in project.generated_files.items():
                out_path = path.resolve()
                mkdir_p(os.path.split(out_path)[0])
                with open(out_path, 'wt', buffering=4096, encoding='utf-8') as fp:
                    debug("Writing {0} ...".format(out_path))
                    print(text, file=fp)

        return self._top

    def _gather_vars(self, rules):
        """
        Iterate the rules, extract variable references, and place definitions
        of them into a map.
        :type rules: List[build_model.Rule]
        :rtype: Dict[str, build_model.Node]
        """
        mk_vars = {}
        seen_vars = set()

        for rule in rules:
            if isinstance(rule, VariableDefinition):
                seen_vars.add(rule.name)

        for rule in rules:
            def add_var(thing):
                if thing is None:
                    return self

                is_var = isinstance(thing, Variable)
                is_env = isinstance(thing, EnvironmentVariable)
                if is_var or is_env:
                    if thing.name not in seen_vars:
                        if thing.raw_value is not None:
                            defn = VariableDefinition(thing.name, '=', thing)
                            mk_vars[thing.name] = defn
                        seen_vars.add(thing.name)

                return self

            rule.visit(add_var)

        if SPEC not in mk_vars:
            mk_vars[SPEC] = VariableDefinition(SPEC, '=', str(build_env.spec()))

        if HOST not in mk_vars:
            mk_vars[HOST] = VariableDefinition(HOST, '=', runtime_values[HOST])

        return mk_vars

    def _write_vars(self, fp, vars):
        """
        Print variable definitions into the given file.
        :type fp: file
        :type vars: List[build_model.VariableDefinition]
        """

        names = sorted(vars.keys())
        for name in names:
            print(vars[name].makefile_str(), file=fp)
        print(file=fp)


class ProjectInfo(object):
    def __init__(self, proj):
        """ :type proj: projects.Project """
        self.proj = proj
        """ :type: projects.Project """

        # the order to visit subprojects
        self.dfs = []

        # makefile rules (must be generated even if we don't write them)
        self.rules = []

        # do we write this project's makefiles?
        self.write_makefiles = None

        # does the project use EXTENSIONS?  If so,
        # dependent projects need to depend on its '-all'
        # target rather than its raw build artifacts,
        # to ensure build ordering.
        self.has_extensions = False

        self.extension_makefile = None

        # the build dependencies which use extensions
        # (only projects with written subprojects)
        self.extension_dependencies = []

    def __repr__(self):
        return self.proj.name
