#!/usr/bin/env python3
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

import argparse
import json
import os
import shutil
import sys
import tempfile
import traceback
import mfit

from mfit import MfitError, util


def _setup_args():
	"""Setup program command line arguments."""
	parser = argparse.ArgumentParser()
	mode_group = parser.add_mutually_exclusive_group()

	parser.add_argument(
		"-p", "--project",
		dest="project",
		action="store",
		type=str,
		help="Mabu project (or package) to load for exporting or updating.",
	)
	parser.add_argument(
		"-s", "--spec",
		dest="spec",
		action="append",
		type=str,
		help="Build spec to use in the export/import process. " \
			"May be passed more than once to handle multiple specs. " \
			"These MUST be fully-qualified mabu build specs! " \
			"(e.g., debug_win_msvc-2015_x64, release_lumin_clang-3.8_aarch64, ...)",
	)
	parser.add_argument(
		"--num-backups",
		dest="num_backups",
		action="store",
		type=int,
		default=0,
		help="Maximum number of backups allowed when backups are enabled. When set to any number less than 1, " \
			"backups will be implicitly disabled."
	)
	parser.add_argument(
		"--mlsdk",
		dest="mlsdk",
		action="store",
		type=str,
		help="Optional path to the MLSDK root."
	)
	parser.add_argument(
		"--mltools",
		dest="mltools",
		action="store",
		type=str,
		help="Optional path to the MLTOOLS root."
	)

	mode_group.add_argument(
		"-e", "--export-json",
		dest="export_json",
		action="store",
		type=str,
		help="Export a JSON file with the complete contents of a project hierarchy.",
	)
	mode_group.add_argument(
		"-i", "--import-json",
		dest="import_json",
		action="store",
		type=str,
		help="Import a JSON file with any relevant changes to apply to the any of the files in the " \
			"hierarchy defined by the specified project file.",
	)

	return parser.parse_args()


def _handle_export(mabu_path, output_file_path, project_file_path, build_specs, num_backups, mlsdk_path, mltools_path):
	"""
	Handle the process of exporting project JSONs from mabu for each provided build spec.

	:param mabu_path: Path to mabu.py.
	:type mabu_path: str

	:param output_file_path: Path to the final output JSON file.
	:type output_file_path: str

	:param project_file_path: Path to the project to have mabu load for exporting.
	:type project_file_path: str

	:param mlsdk_path: Optional path to the MLSDK root.
	:type mlsdk_path: str or None

	:param mltools_path: Optional path to the MLTOOLS root.
	:type mltools_path: str or None

	:param build_specs: List of build specs to pass to mabu.
	:type build_specs: list[str]
	"""
	temp_dir_path = tempfile.mkdtemp()
	spec_json_file_paths = []
	json_file_data = []

	print("Retrieving JSON data from mabu...")

	for spec in build_specs:
		print("Running mabu for spec: {}".format(spec))
		temp_json_file_path = os.path.join(temp_dir_path, spec + ".json")

		ret_code, output, errors = util.invoke_mabu(mabu_path, project_file_path, mlsdk_path, mltools_path, "-t", spec, "--export-json", temp_json_file_path)

		if ret_code != 0:
			raise MfitError("... failed to run mabu for: project({}) spec({})\n\t{}".format(project_file_path, spec, errors.decode("utf-8")))

		if errors:
			errors = errors.decode("utf-8")
			sys.stderr.write(errors)

		spec_json_file_paths.append(temp_json_file_path)

	# Read the JSON file data for each spec.
	for file_path in spec_json_file_paths:
		with open(file_path, "r") as f:
			json_file_data.append(json.load(f))

	# Remove the JSON files and their temp directory.
	shutil.rmtree(temp_dir_path)

	merged_json_data = {}

	print("Merging JSON data...")

	# Merge all the JSON data into a single dictionary.
	for data in json_file_data:
		util.recursively_merge_dicts(merged_json_data, data)

	print("Deduplicating JSON data...")

	# Process the JSON data to deduplicate anything that is shared by all projects.
	json_root = mfit.JsonRoot(merged_json_data)
	json_root.deduplicate_spec_data("(common)", [])
	merged_json_data = json_root.resolve_to_dict()

	output_dir_path = os.path.dirname(output_file_path)

	if not os.access(output_dir_path, os.F_OK):
		os.makedirs(output_dir_path)

	print("Saving JSON file: {}".format(output_file_path))

	util.backup_file(output_file_path, num_backups)

	# Dump the combined JSON to a file.
	with open(output_file_path, "w") as f:
		json.dump(merged_json_data, f, ensure_ascii=False, indent=4, sort_keys=True)


def _handle_import(input_file_path, num_backups):
	"""
	Handle the process of importing a single JSON, splitting it into multiple, and having mabu use each
	one to update the specified project file.

	:param input_file_path: Path to the input JSON file.
	:type input_file_path: str
	"""
	if not os.access(input_file_path, os.F_OK):
		raise MfitError("Failed to find input JSON file: {}".format(input_file_path))

	print("Loading JSON...")

	with open(input_file_path, "r") as f:
		input_json_data = json.load(f)

	if "specs" not in input_json_data:
		raise MfitError("No build spec data in JSON file: {}".format(input_file_path))

	json_root = mfit.JsonRoot(input_json_data)
	configs, platforms, compilers, archs = json_root.get_all_spec_segments()

	print("Building common data...")

	# Generate a global common spec if one does not already exist.
	if "(common)" not in input_json_data["specs"]:
		json_root.deduplicate_spec_data("(common)", [])

	# Generate common specs for only specific subsets of the build specs available.
	for segment in configs + platforms + compilers + archs:
		json_root.deduplicate_spec_data("[{}]".format(segment), [segment])

	def save_mabu_project(project_name, output_file_path, specs):
		def get_def_name_translation(def_name):
			translation = {
				"ARFLAGS": "export ARFLAGS",
				"ARFLAGS.LOCAL": "ARFLAGS",

				"ASMFLAGS": "export ASMFLAGS",
				"ASMFLAGS.LOCAL": "ASMFLAGS",

				"CFLAGS": "export CFLAGS",
				"CFLAGS.LOCAL": "CFLAGS",

				"CPPFLAGS": "export CPPFLAGS",
				"CPPFLAGS.LOCAL": "CPPFLAGS",

				"CXXFLAGS": "export CXXFLAGS",
				"CXXFLAGS.LOCAL": "CXXFLAGS",

				"DEFS": "export DEFS",
				"DEFS.LOCAL": "DEFS",

				"EXTENSIONS": "export EXTENSIONS",
				"EXTENSIONS.LOCAL": "EXTENSIONS",

				"INCS": "export INCS",
				"INCS.LOCAL": "INCS",

				"KIND.LOCAL": "KIND",

				"LDFLAGS": "export LDFLAGS",
				"LDFLAGS.LOCAL": "LDFLAGS",

				"LIBPATHS": "export LIBPATHS",
				"LIBPATHS.LOCAL": "LIBPATHS",

				"OBJS": "export OBJS",
				"OBJS.LOCAL": "OBJS",

				"OPTIONS": "export OPTIONS",
				"OPTIONS.LOCAL": "OPTIONS",

				"OUT": "export OUT",
				"OUT.LOCAL": "OUT",

				"OUTNAME": "export OUTNAME",
				"OUTNAME.LOCAL": "OUTNAME",

				"PKGFLAGS": "export PKGFLAGS",
				"PKGFLAGS.LOCAL": "PKGFLAGS",

				"SHLIBS": "export SHLIBS",
				"SHLIBS.LOCAL": "SHLIBS",

				"SRC_EXTENSIONS": "export SRC_EXTENSIONS",
				"SRC_EXTENSIONS.LOCAL": "SRC_EXTENSIONS",

				"SRCS": "export SRCS",
				"SRCS.LOCAL": "SRCS",

				"STLIBS": "export STLIBS",
				"STLIBS.LOCAL": "STLIBS",
			}
			return translation.get(def_name, def_name)

		def get_formatted_def_value(def_name, def_value):
			def format_list_string(data_list):
				out_string = " \\\n\t".join(sorted(data_list))
				return "\\\n\t" + out_string + "\n" if len(data_list) > 1 else out_string

			if def_name in ["REFS", "USES", "OPTIONS", "OPTIONS.LOCAL"]:
				return format_list_string([original_value for _, original_value in def_value.items()])

			elif def_name in ["SRC_EXTENSIONS", "SRC_EXTENSIONS.LOCAL"]:
				out_list = []

				for data in def_value:
					make_path = data["make_path"]
					src_ext = data["src_ext"]
					dest_path = data["dest_path"]
					dest_ext = data["dest_ext"]
					out_list.append("{} : %{} -> {}/%{}".format(make_path, src_ext, dest_path, dest_ext))

				return format_list_string(out_list)

			elif def_name == "DATAS":
				out_list = []

				for data in def_value:
					src_file = data["src_file"]
					dest_file = data["dest_file"]
					out_list.append("\"{}\" : \"{}\"".format(src_file, dest_file))

				return format_list_string(out_list)

			elif isinstance(def_value, list):
				return format_list_string(def_value)

			return def_value

		def get_specialization(spec_name):
			return ".{}".format(spec_name) if spec_name else ""

		out_spec_data = {}

		# Fill in the data for each spec.
		for spec_name, spec in specs.items():
			proj = spec.project_data.get(project_name, None)
			out_proj_data = {}

			if not proj:
				proj = spec.package_data.get(project_name, None)

			if proj:
				# The common spec name does not have a specific designation in mabu project files.
				if spec_name == "(common)":
					spec_name = ""

				# Special specs should have the brackets removed from their names.
				elif util.is_special_build_spec_name(spec_name):
					spec_name = spec_name[1:-1]

				for key, value in proj.__dict__.items():
					out_proj_data[key] = value

			out_spec_data[spec_name] = out_proj_data

		output_directory = os.path.dirname(output_file_path)

		# Create the output directory.
		if not os.access(output_directory, os.F_OK):
			os.makedirs(output_directory)

		util.backup_file(output_file_path, num_backups)

		# Write the output file.
		with open(output_file_path, "w") as f:
			output_lines = []

			# Write the data for each spec.
			for spec_name, proj_data in out_spec_data.items():
				specialization = get_specialization(spec_name)

				sorted_keys = sorted(proj_data)

				# Write each value for the current spec.
				for key in sorted_keys:
					value = get_formatted_def_value(key, proj_data[key])
					key = get_def_name_translation(key)
					output_lines.append("{}{} = {}\n".format(key, specialization, value))

			f.writelines(output_lines)

	proj_files = { name: path for name, path in json_root.files.items() if os.path.splitext(path)[1] in [".mabu", ".package"] }

	for name, file in proj_files.items():
		print("Saving project file: {}".format(file))
		save_mabu_project(name, file, json_root.specs)


def main():
	"""Program entry point."""
	args = _setup_args()

	# Do some initial error checking.
	if sys.version_info[0:2] < (3, 5):
		raise MfitError("Must be run with Python 3.5 or greater.")

	_mabu_path = os.path.abspath(os.path.join(__file__, "..", "..", "..", "src", "mabu.py"))

	print("Working directory: {}".format(os.getcwd()))

	# Verify that mabu exists at the expected location.
	if not os.access(_mabu_path, os.F_OK):
		raise MfitError("Failed to find mabu at path: {}".format(_mabu_path))

	# Handle the JSON export operation.
	if args.export_json:

		# Error checking specific to JSON export.
		if not args.project:
			raise MfitError("A mabu project file must be specified through the -p/--project option.")
		elif not args.spec:
			raise MfitError("At least one build spec must be provided through the -s/--spec option.")

		args.spec = sorted(set(args.spec))

		_handle_export(_mabu_path, os.path.abspath(args.export_json), args.project, args.spec, args.num_backups, args.mlsdk, args.mltools)

	# Handle the JSON import operation.
	elif args.import_json:
		_handle_import(os.path.abspath(args.import_json), args.num_backups)

	# Unknown operation.
	else:
		raise MfitError("An operation must be specified through either -i/--import-json or -e/--export-json.")


if __name__ == "__main__":
	try:
		main()
	except MfitError as e:
		print("mfit-runner: error:", e.message, file=sys.stderr)
		sys.exit(1)
	except Exception as e:
		traceback.print_exc()
		sys.exit(1)