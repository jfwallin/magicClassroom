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

import collections
import os
import subprocess
import sys

from functools import reduce


def dict_compare(lhs, rhs):
    """
    Compare two dictionaries and return information about their differences.

    :param lhs: First dictionary to compare.
    :type lhs: dict

    :param rhs: Second dictionary to compare.
    :type rhs: dict

    :return: The keys added and removed from the 2nd dictionary, the identical keys between the two, and each differing entry.
    :rtype: tuple[set[str], set[str], set[str], dict[str: tuple]
    """
    lhsKeys = set(lhs.keys())
    rhsKeys = set(rhs.keys())
    commonKeys = lhsKeys.intersection(rhsKeys)

    added = lhsKeys - rhsKeys
    removed = rhsKeys - lhsKeys
    same = set(x for x in commonKeys if lhs[x] == rhs[x])
    diff = {x: (lhs[x], rhs[x]) for x in commonKeys if lhs[x] != rhs[x]}

    return added, removed, same, diff


def dict_intersection(lhs, rhs):
    """
    Compare two dictionaries and construct a new one containing all the items common between the two.

    :param lhs: First dictionary to compare.
    :type lhs: dict

    :param rhs: Second dictionary to compare.
    :type rhs: dict

    :return: Dictionary containing the interesction of all elements common to the inputs.
    :rtype: dict
    """
    added, removed, same, diff = dict_compare(lhs, rhs)

    return {x: lhs[x] for x in same}


def deduplicate_dicts(all_dicts_to_consider):
    """
    Remove all shared entries from a list of dictionaries and put them in their own dictionary.

    :param all_dicts_to_consider: All dictionaries to be deduplicated.
    :type all_dicts_to_consider: list[dict]

    :return: Dictionary containing all the deduplicated data from the input dictionaries.
    :rtype: dict
    """
    common_data = reduce(dict_intersection, all_dicts_to_consider)
    return_data = dict()

    for key, value in common_data.items():
        return_data[key] = value
        for spec_data in all_dicts_to_consider:
            del spec_data[key]

    return return_data


def recursively_merge_dicts(lhs, rhs):
    """
    Recursively merge two dictionaries in a valid json format.

    :param lhs: First dictionary to consider.
    :type lhs: dict

    :param rhs: Second dictionary to merge the first against.
    :type rhs: dict

    :return: The merged result of the two inputs.
    :rtype: dict
    """
    for key, value in rhs.items():
        if isinstance(value, collections.Mapping):
            tmp = recursively_merge_dicts(lhs.get(key, {}), value)
            lhs[key] = tmp

        elif isinstance(value, list):
            tmp = lhs.get(key, [])

            # Don't allow duplicate entries.
            for item in value:
                if item not in tmp:
                    tmp.append(item)

            lhs[key] = tmp

        else:
            lhs[key] = rhs[key]

    return lhs


def remove_all_instances_from_list(data_list, item):
    """
    Remove all occurances of an item from a list.

    :param data_list: List to be modified.
    :type data_list: list

    :param item: Item to remove from the input list.
    :type item: Any
    """
    try:
        while True:
            data_list.remove(item)
    except Exception as e:
        pass


def is_special_build_spec_name(spec_name):
    return spec_name[0] in ["(", "["]


def deconstruct_mabu_build_spec(build_spec):
    """
    Taking a complete mabu build spec as an input, break it down into its individual segments.

    :param build_spec: Complete mabu build spec.
    :type build_spec: str

    :return: Tuple of segments: (config, platform, compiler, arch)
    :rtype: tuple[str, str, str, str]
    """
    config, platform, compiler, arch = build_spec.split("_", 3)

    return config, platform, compiler, arch


def spec_name_matches_criteria(spec_name, criteria):
    """
    Compare a build spec name against a set of criteria.

    :param spec_name: Fully-qualified build spec name.
    :type spec_name: str

    :param criteria: List of spec segments to test the input build spec against.
    :type criteria: list[str]

    :return: True if the spec matches any of the criteria.
    :rtype: bool
    """
    # Custom build specs specific to the JSON should not be considered.
    if is_special_build_spec_name(spec_name):
        return False

    # No criteria means nothing to test against, so the spec name automatically passes.
    if not criteria:
        return True

    segments = deconstruct_mabu_build_spec(spec_name)

    # Check each segment of the spec name against the criteria.
    for segment in segments:
        if segment in criteria:
            return True

    # None of the segments matched!
    return False


def invoke_mabu(mabu_path, project_file_path, mlsdk_path, mltools_path, *args):
    """
    Run mabu with a given set of arguments.

    :param mabu_path: Path to the mabu entry file.
    :type mabu_path: str

    :param project_file_path: Path to the project file to have mabu load.
    :type project_file_path: str

    :param mlsdk_path: Optional path to the mlsdk root.
    :type mlsdk_path: str or None

    :param mltools_path: Optional path to the mltools root.
    :type mltools_path: str or None

    :param args: Supplemental arguments to pass to mabu.
    :type args: str

    :return: Tuple of invokation data: (return code, output, errors)
    :rtype: tuple[int, str, str]
    """
    if not isinstance(args, list):
        args = list(args)

    cmd = [
        sys.executable,
        mabu_path,
        project_file_path,
    ]

    cmd.extend(args)

    mabuEnv = dict(os.environ)

    if mlsdk_path:
        mabuEnv["MLSDK"] = mlsdk_path

    if mltools_path:
        mabuEnv["MLTOOLS"] = mltools_path

    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=mabuEnv)
    out, err = proc.communicate()

    return proc.returncode, out, err


def copy_file(from_file, to_file):
    """
    Copy the contents of a file and write them to another file.
     
    :param from_file: Path to the file to be read.
    :type from_file: str
    
    :param to_file: Path to the file to be written.
    :type to_file: str
    """
    with open(from_file, "rb") as rf:
        original_data = rf.read()

        with open(to_file, "wb") as wf:
            wf.write(original_data)


def backup_file(file_path, max_allowed_backup_count):
    """
    Backup the specified file.
    
    :param file_path: Path to the file to be backed up.
    :type file_path: str
    
    :param max_allowed_backup_count: Maximum number of backups allowed for the file.
    :type max_allowed_backup_count: int
    """

    # Do nothing when backups are disabled.
    if max_allowed_backup_count < 1:
        return

    file_path = os.path.abspath(file_path)

    ops = []

    # Figure out the filenames for the backups.
    for index in range(0, max_allowed_backup_count):
        from_file = file_path if index == 0 else "{}.{}.bak".format(file_path, index)
        to_file = "{}.{}.bak".format(file_path, index + 1)
        ops.append((from_file, to_file))

    # Reverse the order of the backup operations to avoid corrupting any existing backups.
    ops = reversed(ops)

    # Process the backup operations.
    for from_file, to_file in ops:
        if os.access(from_file, os.F_OK):
            copy_file(from_file, to_file)
