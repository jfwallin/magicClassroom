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

import os
import shutil
import tempfile
import textwrap
import zipfile
import re
import sys

import build_env
import diags
import package_updater
import project_vars
import update_zip
from build_model import Path, BuildException
from config import DEFAULT_PACKAGE_BINARY_DIR, BUILT_PACKAGE_EXT, MANIFEST_FILE
from data_layout import DataLayoutArranger
from manifest import Manifest
from package_flags import PackageFlags
from package_updater import PackageUpdateScheduler, PackagingState, FileSigner, TailDataApplier
from run_vars import runtime_values, MLCERT, SIGNER
from utils import mkdir_p, user_relpath
from package_utils import is_elf
from taildata import remove_modsig, pop_tail_data, dump_tail_data_struct


class Packager(object):
    """
    This class creates a package from the files and cached_state file
    """
    CACHE_FILE_NAME = 'package.mf'

    def __init__(self, pkgflags, build_mappings, data_mappings, output_file, cache_dir):
        self._mapping = None
        self._pkgflags = pkgflags
        """ :type: package_flags.PackageFlags """
        self._output_file = output_file
        self._build_mappings = build_mappings
        self._data_mappings = data_mappings
        self._cert_path = pkgflags.signer
        self._verbose = pkgflags.verbose

        self._cache_file = os.path.join(cache_dir, self.CACHE_FILE_NAME)

        try:
            mkdir_p(cache_dir)
        except OSError as e:
            diags.error("failed to create package cache directory: {0}".format(
                e, file=cache_dir
            ))

    @staticmethod
    def _ensure_directory(dest, dirs):
        if dest in dirs:
            return
        dirs.add(dest)
        mkdir_p(dest)

    def _delete_package(self):
        try:
            if os.path.exists(self._output_file):
                os.remove(self._output_file)
        except FileNotFoundError:
            # allow for parallel builds
            pass
        except PermissionError:
            pass

    def clean(self):
        """
        Performs a clean on the package and all copied files.
        """
        schedule = PackageUpdateScheduler(self._build_mappings, self._data_mappings, self._pkgflags,
                                          self._output_file, self._cache_file)

        steps, cache, _ = schedule.generate(clean=True)

        # delete the whole .mpk
        self._delete_package()

        # clean up any transformed files
        state = PackagingState(self._pkgflags, None)
        for step in steps:
            step.perform(state)

        # delete the cache
        try:
            os.remove(self._cache_file)
        except PermissionError as e:
            diags.warn("failed to delete package cache file: {0)".format(e), file=self._cache_file)
        except OSError:
            pass

        return steps, cache

    def run(self):
        """
        Create and run a package updating schedule.
        Return the schedule for testing.
        :rtype: tuple[list[package_updater.PackagingStep],package_updater.PackageCachedState,bool]
        """
        schedule = PackageUpdateScheduler(self._build_mappings, self._data_mappings, self._pkgflags,
                                          self._output_file, self._cache_file)

        # analyze the old and new state and get the steps needed
        steps, cache, recreate = schedule.generate()

        if not steps and not recreate:
            if self._verbose:
                diags.info("no changes detected, leaving previous '{0}'".format(
                    user_relpath(self._output_file)))
            return steps, cache, recreate

        if recreate:
            self._delete_package()

        output_dir = os.path.split(self._output_file)[0]
        try:
            mkdir_p(output_dir)

        except OSError as e:
            diags.error("failed to create package directory '{0}': {1}".format(output_dir, e))

        with update_zip.UpdatingZipFile(
                self._output_file,
                compression=self._pkgflags.pyzip_compression_level(),
                allowZip64=self._pkgflags.allow_zip64) as package:

            state = PackagingState(self._pkgflags, package)

            try:
                for step in steps:
                    step.perform(state)

            except zipfile.LargeZipFile:
                # we only get here if package/allowZip64/off option passed
                diags.error("sorry, either an individual file or the total archive size exceeds 2Gb, "
                            "which is not supported with the current packaging options")

        if self._verbose:
            diags.info("{0} package file '{1}'".format(recreate and "generated" or "updated", self._output_file))

        if self._cert_path:
            # apply tail data for dev cert, and sign the package
            tailer = TailDataApplier()
            signer = FileSigner()
            taildata_needed = tailer.configure_for(self._output_file, self._pkgflags)
            signing_needed = signer.configure_for(self._output_file, self._pkgflags)

            if taildata_needed:
                # apply tail data in-place
                tailer.perform(self._output_file)
                cache.package_flags[package_updater.X_TAILDATA] = tailer.flags()

            if signing_needed:
                # sign in-place
                signer.perform(self._output_file)
                cache.package_flags[package_updater.X_SIGNED] = signer.flags()

        # write the cache
        try:
            mkdir_p(os.path.split(self._cache_file)[0])
            cache.save(self._pkgflags.verbose)

        except OSError as e:
            diags.error("failed to save package cache: " + str(e))

        return steps, cache, recreate


def layout_or_package_project(opts, project):
    """
    When building .packages, we either make a layout for host
    builds with all the files mentioned in DATAS,
    or, make a layout of ELF files and construct an .mpk for target builds.
    (A full layout or an .mpk may be forced with various flags.)
    :type opts: Namespace
    :type project: projects.Project
    """
    if not project.is_package():
        if opts.layout or opts.package:
            # explicit request
            diags.warn("requested --layout or --package but the project '{0}' is not a package".format(project.name))
        return

    # ensure any changes to variables/manifests are resolved
    project.update_output_dirs()

    pkg_layout_path = project.package_layout_path.resolve()
    cache_dir = project.package_cache_path.resolve()

    make_mpk = not opts.no_package and opts.create_package

    # validate certificate
    mlcert = _ensure_mlcert(project, opts) if make_mpk and not opts.no_sign else None

    pkgflags = PackageFlags(mlcert, opts.tail_data_args, opts.verbose)
    pkgflags.set_from_project(project)

    if not opts.no_layout:
        # make a runtime or ELF layout
        if opts.verbose:
            print("[{0}] Laying out DATAS into '{1}'...".format(
                project.name,
                user_relpath(pkg_layout_path)))

        mappings = project.get_package_mappings(opts.create_package, 0)

        # warnings not printed here

        if mappings.data_errors:
            for error in mappings.data_errors:
                diags.error(error, die=False)
            sys.exit(1)

        if opts.create_package:
            # for device builds, only keep ELF files -- no point keeping
            # copies of data files
            mappings.data_mapping = [(src, dest) for (src, dest) in mappings.data_mapping
                                     if dest.startswith("./" + DEFAULT_PACKAGE_BINARY_DIR + "/")
                                     or dest.startswith(DEFAULT_PACKAGE_BINARY_DIR + "/")]

        arranger = DataLayoutArranger(mappings.data_mapping, opts.verbose, pkg_layout_path, cache_dir,
                                      use_links=pkgflags.use_hard_links)
        arranger.run()

    if not make_mpk:
        return

    # build an .mpk
    package_path = project.package_archive_path.resolve()

    if not opts.quiet:
        print("[{0}] Creating package '{1}'...".format(
            project.name,
            user_relpath(package_path)))

    if mlcert:
        if not os.path.isfile(pkgflags.cert_file):
            raise BuildException("expected to find a certificate named '{0}'".format(pkgflags.cert_file))
        if not os.path.isfile(pkgflags.privkey_file):
            raise BuildException("expected to find a private key file '{0}' in the same directory as '{1}'".format(
                os.path.split(pkgflags.privkey_file)[1], pkgflags.cert_file))

    # fetch file mappings
    mappings = project.get_package_mappings(opts.create_package, 0)

    if pkgflags.validate_manifest:
        manifest_warnings, manifest_errors = project.manifest.validate(opts, project, pkgflags, mappings)
    else:
        manifest_warnings, manifest_errors = [], []

    mf_path = user_relpath(project.manifest.path)

    warnings = mappings.data_warnings + manifest_warnings
    for warning in warnings:
        diags.warn(warning, mf_path)

    errors = mappings.data_errors + manifest_errors
    for error in errors:
        diags.error(error, mf_path, die=False)

    if manifest_errors:
        if opts.verbose:
            diags.error("manifest validation failed; "
                        "correct any issues in '{}' and build again".format(mf_path))
        else:
            diags.error("manifest validation failed; "
                        "pass '-v' for more details if needed, "
                        "correct any issues in '{}', "
                        "and build again".format(mf_path))
    if mappings.data_errors:
        sys.exit(1)

    # then package!
    packager = Packager(pkgflags, mappings.build_mapping, mappings.data_mapping,
                        package_path, cache_dir)

    try:
        packager.run()
    except KeyboardInterrupt as e:
        try:
            os.remove(package_path)
        except OSError:
            pass
        raise e


def clean_package_project(opts, project):
    """
    Clean the output of a package project.
    :type opts: Namespace
    :type project: projects.Project
    """
    if not project.is_package():
        if opts.layout or opts.package:
            # explicit request
            diags.warn("requested --layout or --package but the project '{0}' is not a package".format(project.name))
        return

    mappings = project.get_package_mappings(opts.create_package, 0)

    pkg_layout_path = project.package_layout_path.resolve()
    cache_dir = project.package_cache_path.resolve()

    arranger = DataLayoutArranger(mappings.data_mapping, opts.verbose, pkg_layout_path, cache_dir)
    arranger.clean()

    # perform project clean
    package_path = project.package_archive_path.resolve()

    pkgflags = PackageFlags(None, None, opts.verbose)
    pkgflags.set_from_project(project)

    packager = Packager(pkgflags, mappings.build_mapping, mappings.data_mapping,
                        package_path, cache_dir)
    packager.clean()


def _ensure_mlcert(project, opts):
    """
    Establish the runtime MLCERT value, passed to packaging steps.
    :param project:
    :param opts:
    :return: path to base of .cert/.privkey
    """
    # env var or command line arg?
    mlcert = runtime_values.get(MLCERT)
    if not mlcert:
        mlcert = runtime_values.get(SIGNER)  # old name
    if not mlcert and project:
        # look for .package setting
        project_mlcert = project.defs.get(project_vars.MLCERT)
        if project_mlcert:
            if len(project_mlcert) > 1:
                diags.error("multiple values specified for MLCERT")
                return

            mlcert_val = Path.parse(str(project_mlcert[0]))
            if not mlcert_val.is_absolute():
                mlcert_val = Path(project.base, mlcert_val)

            mlcert = mlcert_val.resolve()

    if mlcert:
        mlcert = os.path.abspath(mlcert)
        runtime_values[MLCERT] = mlcert

    elif build_env.platform().uses_packages() and not opts.allow_unsigned:
        lines = ["Missing a developer certificate ('-s ...')."] + \
                textwrap.wrap(
                    "Can't install an unsigned package on device. "
                    "For more information, see https://developer.magicleap.com/learn/guides/application-signing.", width=78)
        diags.error("\n".join(lines))

    return mlcert


def get_resigned_package_output(input_package_path):
    return os.path.splitext(input_package_path)[0] + '.resigned' + BUILT_PACKAGE_EXT


def resign_package(opts, output_package_path):
    """
    Recreate the .mpk with an alternate signature and taildata.
    :type opts: Namespace
    :param output_package_path: path for rewritten mpk (may be same as input)
    """
    input_package_path = opts.resign_mpk
    if not os.path.isfile(input_package_path):
        diags.error("no existing built package: {}".format(input_package_path))

    # validate certificate
    mlcert = _ensure_mlcert(None, opts)

    extract_dir = tempfile.mkdtemp(suffix='-mpk')
    cache_dir = tempfile.mkdtemp(suffix='-cache')

    try:
        pkgflags = PackageFlags(mlcert, opts.tail_data_args, opts.verbose)
        pkgflags.package_cache_dir = cache_dir
        # print(build_env.spec().config)
        # HACK: not reading or consuming projects
        pkgflags.debuggable = 'debug' == build_env.spec().config

        if not opts.quiet:
            diags.info("Re-signed package {} debuggable".format(pkgflags.debuggable and "WILL BE" or "WILL NOT BE"))
            if (hasattr(opts,
                        'default_spec') and opts.default_spec) or 'debug' not in opts.spec and 'release' not in opts.spec:
                diags.info("(use '-t lumin_debug' or '-t lumin_release' to specify your intent.)")

        if mlcert:
            if not os.path.isfile(pkgflags.cert_file):
                raise BuildException("expected to find a certificate named '{0}'".format(pkgflags.cert_file))
            if not os.path.isfile(pkgflags.privkey_file):
                raise BuildException("expected to find a private key file '{0}' in the same directory as '{1}'".format(
                    os.path.split(pkgflags.privkey_file)[1], pkgflags.cert_file))

        if not opts.quiet:
            if opts.verbose:
                print("Extracting '{0}' to '{1}'...".format(user_relpath(input_package_path), extract_dir))
            else:
                print("Extracting '{0}'...".format(user_relpath(input_package_path)))

        try:
            build_mappings, data_mappings = _extract_mpk(input_package_path, extract_dir)
        except IOError as e:
            diags.error("failed to extract package: {}", str(e))
            return

        # we expect this to appear here
        manifest_path = os.path.join(extract_dir, MANIFEST_FILE)
        if os.path.isfile(manifest_path):
            pkgflags.manifest = Manifest()
            pkgflags.manifest.load(manifest_path)
            pkgflags.update_manifest_developer_id = True    # we wrote the original developer_id, so, must change it

        if not opts.quiet:
            print("Removing signing data...")

        for (path, _) in build_mappings:
            remove_modsig(path)
            pop_tail_data(path)

        if not opts.quiet:
            print("Re-signing into '{0}'...".format(user_relpath(output_package_path)))

        packager = Packager(pkgflags, build_mappings, data_mappings,
                            output_package_path, cache_dir)
        try:
            packager.run()
        except KeyboardInterrupt as e:
            try:
                os.remove(output_package_path)
            except OSError:
                pass
            raise e
    except BuildException as e:
        diags.error(str(e))
    finally:
        shutil.rmtree(extract_dir, ignore_errors=True)
        shutil.rmtree(cache_dir, ignore_errors=True)


def fetch_signing_data(opts, input_package_path):
    """
    Extract the .mpk and return the main taildata as well as taildata for each entry
    which would be expected to have it.

    This also fetches the developer certificate and sets `taildata.devcert` with it.
    :rtype: Tuple(taildata, Dict(string, taildata))
    """
    if not os.path.isfile(input_package_path):
        diags.error("no existing built package: {}".format(input_package_path))
        return None, None

    # dump the toplevel data
    main_taildata, _ = package_updater.find_tail_data(input_package_path)

    extract_dir = tempfile.mkdtemp()

    entry_taildata = {}
    try:
        if not opts.quiet:
            if opts.verbose:
                print("Extracting '{0}' to '{1}'...".format(user_relpath(input_package_path), extract_dir))
            else:
                print("Extracting '{0}'...".format(user_relpath(input_package_path)))

        try:
            build_mappings, _ = _extract_mpk(input_package_path, extract_dir)
        except IOError as e:
            diags.error("failed to extract package: {}", str(e))
            return  # notreached

        for (path, relpath) in build_mappings:
            taildata, _ = package_updater.find_tail_data(path)
            entry_taildata[relpath] = taildata

    finally:
        shutil.rmtree(extract_dir, ignore_errors=True)

    return main_taildata, entry_taildata


def _extract_mpk(mpk_path, extract_path):
    """
    Extract an existing .mpk and return the list of detected build and data files.
    :param mpk_path:
    :param extract_path:
    :return: two lists of tuples (for input to output files, for builds and data mappings)
    :rtype: list[tuple[str,str]], list[tuple[str,str]]
    """
    with zipfile.ZipFile(mpk_path, 'r') as zf:
        file_list = zf.namelist()
        zf.extractall(extract_path)

    build_mappings = []
    data_mappings = []

    for entry in file_list:
        path = os.path.join(extract_path, entry)
        if os.path.isfile(path):
            if is_elf(path):
                build_mappings.append((path, entry))
            else:
                data_mappings.append((path, entry))

    return build_mappings, data_mappings


def print_signing_data(label, taildata, verbose):
    if taildata:
        print("Signing data for {}:".format(label))
        dump_tail_data_struct(taildata, verbose)
        print()
    else:
        print("(no signing data present in {})".format(label))

