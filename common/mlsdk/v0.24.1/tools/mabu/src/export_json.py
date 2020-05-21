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

import hashlib
import json
import os
import re
import stat
import subprocess
import sys

import build_env
import diags
import platforms
import project_vars
from build_model import Variable, Command, VariableDefinition, Path, Node
from config import HOSTOS, OS_WIN, OS_OSX, PROJ_EXT, COMP_EXT
from diags import error, warn
from makegen import MakeGen
from projects import MultiVariable, PRIVATE_SUFFIX
from run_vars import runtime_values, MLSDK
from utils import format_command_line


def export_json(scope, output_file_path, explicit_projects):
    """
    Print the indexer config variables contributing to this project.
    :param scope: 0=all, -1=local, 1=refs
    :param output_file_path: Path to the output JSON file.
    :param explicit_projects: List of project files the user specified in the command line.
    """
    env = build_env

    mg = MakeGen(env.projects(), env.components(), scope)
    mg.generate(env.user_projects())
    var_map = mg.gather_vars()

    toolchain = env.toolchain()

    # Tool command names should not be included.
    excluded_var_names = [x for x in toolchain.tools]

    # Add known variable names that are not explicitly associated with any toolchain.
    excluded_var_names.extend([
        "DEPGEN",
        "HOST",
        "PYTHON",
        "RM",
        "SPEC",
        "TOUCH",
    ])

    all_packages = set()
    all_projects = set()
    all_components = set()
    all_configs = set()
    all_options = set()
    project_key_lookup_by_name = dict()
    project_key_lookup_by_path = dict()

    section_explicit_projects = set()
    section_files = dict()
    section_project_resolved_outputs = dict()
    section_vars = dict()
    section_packages = dict()
    section_projects = dict()
    section_components = dict()
    section_configs = dict()
    section_options = dict()

    def get_file_reference_key_or_value(value):
        if value in project_key_lookup_by_path:
            return project_key_lookup_by_path[value]
        elif value in project_key_lookup_by_name:
            return project_key_lookup_by_name[value]
        return value

    def cacheProjectFiles(proj):
        components = proj.dependency_list(None, env.components())
        configs = [env.find_config_from_full_path(x) for x in proj.configs]
        options = [env.find_option_from_full_path(x) for x in proj.options]

        if proj.is_package():
            all_packages.add(proj)
        else:
            all_projects.add(proj)

        all_components.update(set(components))
        all_configs.update(set(configs))
        all_options.update(set(options))

    # Collect a complete listing of each project, component, option, and config involved in the build.
    for _, proj in env.projects().items():
        cacheProjectFiles(proj)

    for proj in env.user_projects():
        cacheProjectFiles(proj)

    for comp in all_components:
        configs = [env.find_config_from_full_path(x) for x in comp.configs]
        options = [env.find_option_from_full_path(x) for x in comp.options]
        all_configs.update(set(configs))
        all_options.update(set(options))

    all_files = all_packages | all_projects | all_components | all_configs | all_options

    # Fill in the project files by creating mappings of [unique_name] <-> [project_path/project_name].
    for proj in all_files:
        # The project unique names need to be consistent across all build specs, so hashing the project path seems
        # to be the simplest way of accomplishing that.
        hasher = hashlib.md5(proj.path.encode("utf-8"))
        hashed_path = hasher.hexdigest()
        unique_name = "@({})".format(hashed_path)

        if unique_name in section_files:
            error("Project path hashing collision: {}, {}".format(proj.path, section_files[unique_name]))

        # Don't add packages to the name lookup table since their names can conflict with project files.
        # NOTE: This is a temporary solution since it may be a problem in other case where project names can be identical.
        if not proj.is_package():
            if proj.name in project_key_lookup_by_name:
                warn(
                    "Project \"{}\" already in the name lookup table\n\t> {}\n\t+ {}".format(
                        proj.name,
                        section_files[project_key_lookup_by_name[proj.name]],
                        proj.path,
                    )
                )
            else:
                project_key_lookup_by_name.update({ proj.name: unique_name })

        project_key_lookup_by_path.update({ proj.path: unique_name })
        section_files.update({ unique_name: proj.path })

    # Fill in the variable mappings.
    for proj_name, proj_vars in var_map.items():

        # Discard the vars from the top-level since everything it contains will already exist in the project vars.
        if not proj_name:
            continue

        proj_unique_name = project_key_lookup_by_name[proj_name]
        proj_vars_section = dict()

        # Create a dictionary of all project variables.
        for var_name, var_data in proj_vars.items():

            # Discard tool variables.
            if var_name in excluded_var_names:
                continue

            value = get_file_reference_key_or_value(var_data.rhs().value())
            proj_vars_section.update({ "$({})".format(var_name): value })

        # Add the project variables to the vars section.
        section_vars.update({ proj_unique_name: proj_vars_section })

    def get_value_for_project_def(def_name, def_value, proj):
        if def_name.endswith(PRIVATE_SUFFIX):
            # Strip the def name private suffix for this function so it's easier to parse.
            def_name = def_name[:-len(PRIVATE_SUFFIX)]

        if isinstance(def_value, MultiVariable):
            # MultiVariables need to be resolved based on the spec before they can be exported.
            def_value = def_value.derive(env.spec())

        def get_project_dict(proj):
            out_data = {}

            for ref in def_value:
                ref_file = ref

                if not ref_file.endswith(PROJ_EXT):
                    ref_file += PROJ_EXT

                file_key = None

                if ref in env.projects():
                    file_key = ref
                else:
                    ref_file_full_path = os.path.abspath(os.path.join(proj.base, ref_file))
                    if os.access(ref_file_full_path, os.F_OK):
                        file_key = ref_file_full_path

                if file_key:
                    file_key = get_file_reference_key_or_value(file_key)
                    out_data[file_key] = ref

            return out_data

        def get_component_dict():
            out_data = {}

            for comp in def_value:
                comp_file = comp

                if not comp_file.endswith(COMP_EXT):
                    comp_file += COMP_EXT

                file_key = None

                if comp in env.components():
                    file_key = comp
                else:
                    comp_file_full_path = build_env.find_component_on_path(None, comp_file)
                    if comp_file_full_path:
                        file_key = comp_file_full_path

                if file_key:
                    file_key = get_file_reference_key_or_value(file_key)
                    out_data[file_key] = comp

            return out_data

        def get_option_dict():
            out_data = {}

            for option in def_value:
                file_key = None

                if option in env.options():
                    file_key = env.find_option(option).path

                if file_key:
                    file_key = get_file_reference_key_or_value(file_key)
                    out_data[file_key] = option

            return out_data

        if def_name == "REFS":
            return get_project_dict(proj)
        elif def_name == "USES":
            return get_component_dict()
        elif def_name == "OPTIONS":
            return get_option_dict()
        elif def_name == "DATAS":
            return [
                {
                    "src_file": str(x[0]),
                    "dest_file": str(x[1]),
                }
                for x in def_value
            ]
        elif def_name == "SRC_EXTENSIONS":
            return [
                {
                    "make_path": str(x[0]),
                    "src_ext": str(x[1]),
                    "dest_ext": str(x[2]),
                    "dest_path": str(x[3]),
                }
                for x in def_value
            ]
        elif def_name in ["OUT", "OUTNAME", "MLCERT", "SIGNER"]:
            return def_value[0] if def_value else None
        elif isinstance(def_value, list):
            return [str(x) for x in sorted(set(def_value))]

        return str(def_value) if def_value else None

    def fill_section_data(projects, section_data_map):
        for proj in projects:
            proj_vars_section = dict()

            for def_name, def_value in proj.defs.items():

                def_value = get_value_for_project_def(def_name, def_value, proj)

                # Discard empty values.
                if def_value:
                    proj_vars_section.update({ def_name: def_value })

            proj_key = get_file_reference_key_or_value(proj.path)

            section_data_map.update({ proj_key: proj_vars_section })

    # Get all the explicit projects.
    for proj_path in explicit_projects:
        proj_key = get_file_reference_key_or_value(proj_path)

        section_explicit_projects.add(proj_key)

    # Get the output paths to all the projects.
    for proj in all_projects:
        proj_key = get_file_reference_key_or_value(proj.path)
        build_platform = env.platform()

        # Format the project output filename.
        output_filename = str(proj.output_filename())
        if output_filename:
            resolved_path = os.path.abspath(os.path.join(proj.output_directory, output_filename))

            section_project_resolved_outputs.update({ proj_key: resolved_path })

    # Get the output paths to all the packages.
    for proj in all_packages:
        proj_key = get_file_reference_key_or_value(proj.path)
        resolved_path = proj.package_archive_path.resolve()

        section_project_resolved_outputs.update({ proj_key: resolved_path })

    fill_section_data(all_packages, section_packages)
    fill_section_data(all_projects, section_projects)
    fill_section_data(all_components, section_components)
    fill_section_data(all_configs, section_configs)
    fill_section_data(all_options, section_options)

    # Setup the root objects for the JSON file.
    spec_data = {
        str(env.spec()): {
            "project_resolved_outputs": section_project_resolved_outputs,
            "vars": section_vars,
            "package_data": section_packages,
            "project_data": section_projects,
            "component_data": section_components,
            "config_data": section_configs,
            "options_data": section_options,
        }
    }

    json_root_data = {
        "explicit_projects": sorted(section_explicit_projects),
        "files": section_files,
        "specs": spec_data,
    }

    output_directory = os.path.dirname(output_file_path)

    # Create the output directory if it doesn't already exist.
    if not os.access(output_directory, os.F_OK):
        os.makedirs(output_directory)

    # Dump the JSON to a file.
    with open(output_file_path, "wt", encoding='utf-8') as f:
        json.dump(json_root_data, f, ensure_ascii=False, indent=4, sort_keys=True)
        print("Exported project files to: {}".format(output_file_path))
