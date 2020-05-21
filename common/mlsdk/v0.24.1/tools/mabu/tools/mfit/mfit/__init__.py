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

from . import util

from abc import abstractmethod
from functools import reduce


class MfitError(Exception):
    """Custom exception for raising errors in the mfit tool."""

    def __init__(self, msg):
        Exception.__init__(self, msg)
        self.message = msg


class JsonNode(object):
    """Base class for all JSON node data wrappers."""
    def __init__(self):
        pass

    @abstractmethod
    def resolve_to_dict(self):
        """
        Resolve the wrapped data back to a standard dictionary.

        :return: JSON-compatible dictionary.
        :rtype: dict
        """
        raise NotImplementedError()


class JsonRoot(JsonNode):
    """Data wrapper for the root of a dictionary containing JSON data."""
    def __init__(self, json_data):
        JsonNode.__init__(self)
        self.files = json_data.get("files", {})
        self.explicit_projects = json_data.get("explicit_projects", [])
        self.specs = { spec_name: JsonSpec(spec_data) for spec_name, spec_data in json_data.get("specs", {}).items() }

    def resolve_to_dict(self):
        """
        Resolve the wrapped data back to a standard dictionary.

        :return: JSON-compatible dictionary.
        :rtype: dict
        """
        output_json_data = {
            "files": self.files,
            "explicit_projects": self.explicit_projects,
            "specs": { spec_name: spec.resolve_to_dict() for spec_name, spec in self.specs.items() }
        }

        # Disregard any empty specs.
        output_json_data["specs"] = { spec_name: spec_data for spec_name, spec_data in output_json_data["specs"].items() if spec_data }

        return { key: value for key, value in output_json_data.items() if value }

    def get_all_spec_segments(self):
        configs = []
        platforms = []
        compilers = []
        archs = []

        for spec_name, _ in self.specs.items():
            if not util.is_special_build_spec_name(spec_name):
                segments = util.deconstruct_mabu_build_spec(spec_name)

                configs.append(segments[0])
                platforms.append(segments[1])
                compilers.append(segments[2])
                archs.append(segments[3])

        return sorted(set(configs)), sorted(set(platforms)), sorted(set(compilers)), sorted(set(archs))


    def deduplicate_spec_data(self, common_spec_name, spec_criteria):
        """
        Deduplicate the build spec data for each project and store the common data in a new spec.

        :param common_spec_name: Name of the common spec to generate.
        :type common_spec_name: str

        :param spec_criteria: List of build spec segments to use for considering the specs for deduplication (empty list for 'all').
        :type spec_criteria: list[str]

        :return: Generated common spec.
        :rtype: :class:`mfit.JsonSpec`
        """
        spec_list = [
            spec_data
            for spec_name, spec_data in self.specs.items()
            if util.spec_name_matches_criteria(spec_name, spec_criteria)
        ]

        common_spec = JsonSpec.deduplicate(spec_list)

        if common_spec:
            # If a common spec already exists, the old one and new one need to be merged.
            if common_spec_name in self.specs:
                old_common_spec = self.specs[common_spec_name].resolve_to_dict()
                new_common_spec = common_spec.resolve_to_dict()
                common_spec = JsonSpec(util.recursively_merge_dicts(old_common_spec, new_common_spec))

            self.specs[common_spec_name] = common_spec

        else:
            common_spec = None

        return common_spec


class JsonSpec(JsonNode):
    """Data wrapper for build specs containing JSON data."""
    def __init__(self, json_data):
        JsonNode.__init__(self)
        self.project_resolved_outputs = json_data.get("project_resolved_outputs", {})
        self.vars = json_data.get("vars", {})
        self.package_data = { project_name: JsonProject(project_data) for project_name, project_data in json_data.get("package_data", {}).items() }
        self.project_data = { project_name: JsonProject(project_data) for project_name, project_data in json_data.get("project_data", {}).items() }
        self.config_data = { project_name: JsonProject(project_data) for project_name, project_data in json_data.get("config_data", {}).items() }
        self.options_data = { project_name: JsonProject(project_data) for project_name, project_data in json_data.get("options_data", {}).items() }
        self.component_data = { project_name: JsonProject(project_data) for project_name, project_data in json_data.get("component_data", {}).items() }

    def __bool__(self):
        checks = [True for _, item in self.__dict__.items() if item]
        return bool(checks)

    def clear(self):
        self.project_resolved_outputs = {}
        self.vars = {}
        self.package_data = {}
        self.project_data = {}
        self.config_data = {}
        self.options_data = {}
        self.component_data = {}


    def resolve_to_dict(self):
        """
        Resolve the wrapped data back to a standard dictionary.

        :return: JSON-compatible dictionary.
        :rtype: dict
        """
        output_json_data = {
            "project_resolved_outputs": self.project_resolved_outputs,
            "vars": self.vars,
            "package_data": { project_name: project.resolve_to_dict() for project_name, project in self.package_data.items() if project },
            "project_data": { project_name: project.resolve_to_dict() for project_name, project in self.project_data.items() if project },
            "config_data": { project_name: project.resolve_to_dict() for project_name, project in self.config_data.items() if project },
            "options_data": { project_name: project.resolve_to_dict() for project_name, project in self.options_data.items() if project },
            "component_data": { project_name: project.resolve_to_dict() for project_name, project in self.component_data.items() if project },
        }
        return { key: value for key, value in output_json_data.items() if value }

    @staticmethod
    def deduplicate(all_specs):
        """
        Deduplicate the data contained within the input list of build specs.

        :param all_specs: Build specs to deduplicate.
        :type all_specs: list[JsonSpec]

        :return: New build spec containing the items common to each of the input build specs.
        :rtype: JsonSpec
        """
        def get_common_vars():
            all_spec_data = [spec.vars for spec in all_specs]

            common_keys = reduce(set.intersection, [set(spec_data.keys()) for spec_data in all_spec_data])
            common_data = dict()

            for key in common_keys:
                data_list = [spec_data[key] for spec_data in all_spec_data]
                sub_common_data = util.deduplicate_dicts(data_list)

                if sub_common_data:
                    common_data.update({ key: sub_common_data })

            return common_data

        def get_common_generic_project_data(all_spec_data):
            common_keys = reduce(set.intersection, [set(spec_data.keys()) for spec_data in all_spec_data])
            common_data = dict()

            for key in common_keys:
                data_list = [spec_data[key] for spec_data in all_spec_data]
                data_list = [proj.__dict__ for proj in data_list]

                sub_common_data = JsonProject.deduplicate(data_list)

                if sub_common_data:
                    common_data.update({ key: sub_common_data })

            return common_data

        def get_common_package_data():
            all_spec_data = [spec.package_data for spec in all_specs]

            return get_common_generic_project_data(all_spec_data)

        def get_common_project_data():
            all_spec_data = [spec.project_data for spec in all_specs]

            return get_common_generic_project_data(all_spec_data)

        def get_common_component_data():
            all_spec_data = [spec.component_data for spec in all_specs]

            return get_common_generic_project_data(all_spec_data)

        def get_common_config_data():
            all_spec_data = [spec.config_data for spec in all_specs]

            return get_common_generic_project_data(all_spec_data)

        def get_common_options_data():
            all_spec_data = [spec.options_data for spec in all_specs]

            return get_common_generic_project_data(all_spec_data)

        if len(all_specs) > 1:
            # Deduplicate data between multiple specs.
            new_spec_data = {}

            common_vars = get_common_vars()
            if common_vars:
                new_spec_data["vars"] = common_vars

            common_package_data = get_common_package_data()
            if common_package_data:
                new_spec_data["package_data"] = common_package_data

            common_project_data = get_common_project_data()
            if common_project_data:
                new_spec_data["project_data"] = common_project_data

            common_component_data = get_common_component_data()
            if common_component_data:
                new_spec_data["component_data"] = common_component_data

            common_config_data = get_common_config_data()
            if common_config_data:
                new_spec_data["config_data"] = common_config_data

            common_options_data = get_common_options_data()
            if common_options_data:
                new_spec_data["options_data"] = common_options_data

            return JsonSpec(new_spec_data)

        elif len(all_specs) == 1:
            # When there is only a single spec, just use it as the common spec.
            spec = all_specs[0]
            common_spec = JsonSpec(spec.resolve_to_dict())

            spec.clear()

            return common_spec

        else:
            # Do nothing if there are no specs to deduplicate.
            return None


class JsonProject(JsonNode):
    """A thin wrapper around JSON project data."""
    def __init__(self, json_data):
        JsonNode.__init__(self)
        self.__dict__.update(json_data)

    def __bool__(self):
        return True if self.__dict__ else False

    def resolve_to_dict(self):
        """
        Resolve the wrapped data back to a standard dictionary.

        :return: JSON-compatible dictionary.
        :rtype: dict
        """
        return self.__dict__

    @staticmethod
    def deduplicate(all_projects):
        """
        Deduplicate all data in the provided list of dictionaries representing projects for all available build specs,
        returning a new project with data common to each.

        :param all_projects: List of project dictionaries.
        :type all_projects: list[dict]

        :return: New project dictionary containing common data of the input list.
        :rtype: dict
        """
        def dedupe_lists(all_data_lists, key):
            common_items = reduce(set.intersection, [set(data_list) for data_list in all_data_lists])

            # Remove the common entries from the data lists.
            for item in common_items:
                for data_list in all_data_lists:
                    util.remove_all_instances_from_list(data_list, item)

            # Remove empty entries from the projects.
            for proj in all_projects:
                if key in proj and not proj[key]:
                    del proj[key]

            return sorted(common_items)

        def dedupe_dicts(all_data_dicts, key):
            common_items = reduce(set.intersection, [set(tuple(data_dict.items())) for data_dict in all_data_dicts])
            common_items = dict(common_items)

            # Remove the common entries from the data dicts.
            for common_key, common_value in common_items.items():
                for data_dict in all_data_dicts:
                    if common_key in data_dict:
                        del data_dict[common_key]

            # Remove empty entries from the projects.
            for proj in all_projects:
                if key in proj and not proj[key]:
                    del proj[key]

            return common_items

        def dedupe_lists_of_dicts(all_data_lists, key):
            common_items = reduce(set.intersection, [set([tuple(x.items()) for x in data_list]) for data_list in all_data_lists])
            common_items = [dict(x) for x in common_items]

            # Remove the common entries from the data lists.
            for item in common_items:
                for data_list in all_data_lists:
                    util.remove_all_instances_from_list(data_list, item)

            # Remove empty entries from the projects.
            for proj in all_projects:
                if key in proj and not proj[key]:
                    del proj[key]

            return common_items

        def dedupe_strings(all_data_strings, key):
            if len(set(all_data_strings)) == 1:

                # We need to remove the entries for the provided key from each version of the project.
                for proj in all_projects:
                    if key in proj:
                        del proj[key]

                return all_data_strings[0]

            return None

        # Keys that represent list items.
        list_keys = [
            "ARFLAGS",
            "ASMFLAGS",
            "CFLAGS",
            "CPPFLAGS",
            "CXXFLAGS",
            "DEFS",
            "DEFS.LOCAL",
            "EXTENSIONS",
            "EXTENSIONS.LOCAL",
            "INCS",
            "INCS.LOCAL",
            "LDFLAGS",
            "LIBPATHS",
            "SHLIBS",
            "SRCS",
            "SRCS.LOCAL",
            "STLIBS",
        ]

        dict_keys = [
            "REFS",
            "OPTIONS",
            "OPTIONS.LOCAL",
            "USES",
        ]

        list_dict_keys = [
            "DATAS",
            "SRC_EXTENSIONS",
            "SRC_EXTENSIONS.LOCAL",
        ]

        # Keys that represent any string items. These are special because they can only be
        # deduplicated if they are all identical.
        string_keys = [
            "KIND",
            "KIND.LOCAL",
            "OUT",
            "OUTNAME",
            "MLCERT",
            "SIGNER",
        ]

        common_keys = reduce(set.intersection, [set(proj.keys()) for proj in all_projects])
        common_data = {}

        for key in common_keys:
            all_items = [proj[key] for proj in all_projects if key in proj]
            common_items = None

            if key in list_keys:
                common_items = dedupe_lists(all_items, key)
            if key in dict_keys:
                common_items = dedupe_dicts(all_items, key)
            elif key in list_dict_keys:
                common_items = dedupe_lists_of_dicts(all_items, key)
            elif key in string_keys:
                common_items = dedupe_strings(all_items, key)

            # Add the common items to the common project data.
            if common_items:
                common_data[key] = common_items

        return common_data
