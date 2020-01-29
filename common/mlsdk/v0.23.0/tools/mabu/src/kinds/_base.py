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

from build_model import BuildObject, Path, Rule, Buildable, Directive, Include
from diags import debug
from link_flags import LinkFlags


class Kind(BuildObject):
    """
    A kind is the type of a build artifact.
    """
    def __init__(self, name):
        BuildObject.__init__(self, name)

    def generate(self, proj, env):
        """ Generate the rules for the project based on this kind
        :type proj: projects.Project
        :type env: build_env
         :rtype: List[Rule]
        """
        raise NotImplementedError()

    def generate_object_rules(self, env, proj):
        """ Generate the rules to build object files for the sources in the project.
        :type env: build_env
        :type proj: projects.Project
         :rtype: List[Rule]
        """
        rules = []
        toolchain = env.toolchain()
        for src_proj, src in proj.all_source_files():
            # output is always in our project
            obj_path = proj.get_object_output(src_proj, src)
            obj = Buildable(obj_path)

            src_path = proj.get_source_path(src_proj, src)

            rule = Rule(proj, obj, [Buildable(src_path)])
            rules.append(rule)

            # commands...
            if proj.is_c_family_file(src):
                cmds = toolchain.create_compiler_commands(env, proj, self, src_path, obj_path)
                rule.add_commands(cmds)

                rules.append(Include("-include", proj.get_dependency_output(obj_path)))
            elif proj.is_asm_file(src):
                cmds = toolchain.create_assembler_commands(env, proj, self, src_path, obj_path)
                rule.add_commands(cmds)

        return rules

    class LinkModel:
        """
        Represents the model of a link step.
        """
        def __init__(self, proj, artifact_rule, outpath, objs, stlibs, shlibs):
            """
            :type proj: build_model.Project
            :type artifact_rule: build_model.Rule
            :type outpath: build_model.Path
            :type objs: List[build_model.Path]
            :type stlibs: List[tuple[build_model.Path,str]]
            :type shlibs: List[tuple[build_model.Path,str]]
            """
            self.proj = proj
            self.artifact_rule = artifact_rule
            self.outpath = outpath
            self.objs = objs
            self.stlibs = stlibs
            self.shlibs = shlibs

            self.link_flags = LinkFlags()
            self.link_flags.set_from_project(proj)

    def generate_project_rules(self, proj, env, outfile):
        """
        Generate the Rules that build the project (link) and its
        dependencies (object files).

        The rules must always place the primary project artifact
        first in the list (see kinds.shared).
        :param proj: project
        :type proj: projects.Project
        :param env: build environment
        :type env: build_env
        :param outfile: output path
        :type outfile: Node | str
        :rtype: List[build_model.Rule]
        """
        outpath = Path(proj.outdir_path(), outfile)
        bld = Buildable(outpath)

        debug("Buildable for {0} is {1}".format(proj.name, bld))

        # the program's rule builds the artifact from object files,
        # shared objects, and static libraries
        objs = proj.all_object_files()
        stlibs = proj.all_static_libraries()
        shlibs = proj.all_shared_implibs()

        # stlib/shlib dependencies only rely on ones for which we
        # know the absolute paths
        if not proj.is_static():
            import build_env
            platform = build_env.platform()

            def full_path(ldir, name, cvt):
                if not ldir:
                    return None
                return Path(ldir, name).change_filename(cvt(name.makefile_str()))

            stlibs_deps = [full_path(ldir, lib, platform.format_static_file) for ldir, lib in stlibs]
            stlibs_deps = [x for x in stlibs_deps if x]
            shlibs_deps = [full_path(ldir, lib, platform.format_importlib_file) for ldir, lib in shlibs]
            shlibs_deps = [x for x in shlibs_deps if x]
        else:
            # static/shared libs are not linked into static
            stlibs_deps = []
            shlibs_deps = []

        artifact_rule = Rule(proj, bld, objs + stlibs_deps + shlibs_deps)
        rules = [ artifact_rule ]

        # link it...
        model = self.LinkModel(proj, artifact_rule, outpath, objs, stlibs, shlibs)
        cmds = self._create_link_commands(model)
        artifact_rule.add_commands(cmds)

        # remember which artifacts are created
        proj.artifacts = [ artifact_rule.buildable.path ] + \
            [buildable.path for buildable in artifact_rule.aux_buildables ]

        # create any other rules
        rules += env.toolchain().generate_extra_rules(env, proj)

        # and create rules for the object files themselves
        rules += self.generate_object_rules(env, proj)

        return rules

    def _create_link_commands(self, model):
        raise NotImplementedError()


Kind.init_class()
