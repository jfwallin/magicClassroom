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

from build_model import Buildable, Rule, Path
from kinds._base import Kind

NAME = 'shared'


class Handler(Kind):
    def generate(self, proj, env):
        # basic objects + libs -> artifact rules...
        out_name = proj.output_filename()

        rules = self.generate_project_rules(
            proj, env,
            out_name)

        # Add a dependency rule to ensure the import lib is regenerated
        # by rebuilding the shared object if it goes missing.
        #
        # This rule only makes sense if the import lib actually
        # has a different filename than the shared object.
        #
        # Also, note we have the import lib depend on the same inputs
        # as the shared object, not the shared object itself, since
        # there's no guarantee which file will have the newer timestamp.
        lib_name = env.platform().format_importlib_file(proj.output_name)
        if lib_name != out_name:
            lib_path = Path(proj.outdir_path(), lib_name)
            lib_rule = Rule(proj, Buildable(lib_path), rules[0].dependencies)

            rules.append(lib_rule)

        return rules

    def _create_link_commands(self, model):
        import build_env
        return build_env.toolchain().create_shared_link_commands(model)
