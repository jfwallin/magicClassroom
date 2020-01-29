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
import xml.parsers.expat

import re

import diags
from run_vars import runtime_values, MLSDK
from build_model import BuildException
from config import DATA_DIR, SCRIPT_DIR
from diags import warn


class Component(object):
    def __init__(self, name, binary_file):
        self._name = name
        self._binary_file = binary_file

    @property
    def name(self):
        return self._name

    @property
    def binary_file(self):
        return self._binary_file


global_known_privileges = []


class Manifest(object):

    def __init__(self):
        self._path = None
        self._package_name = None
        self._components = []
        # :type: List[Component]
        self._developer_id = None

        self._min_api_level = None

        self._privileges = []

    @property
    def path(self):
        return self._path

    def load(self, path, text=None):
        self._path = path
        self._privileges.clear()

        if not text:
            with open(path, 'rb') as f:
                self._load(path, file=f)
        else:
            self._load(path, text=text)

    def _load(self, label, file=None, text=None):
        p = xml.parsers.expat.ParserCreate()

        el_stack = []

        def start_handler(name, attrs):
            if len(el_stack) == 0:
                if name != 'manifest':
                    raise BuildException("unexpected manifest format (expected <manifest> root) in {}".format(label))
                self._package_name = attrs.get('ml:package')
                self._developer_id = attrs.get('ml:developer_id')
            elif len(el_stack) == 1 and name == 'application':
                self._min_api_level = attrs.get('ml:min_api_level')
            elif len(el_stack) == 2 and name == 'component':
                comp = Component(attrs.get('ml:name'), attrs.get('ml:binary_name'))
                self._components.append(comp)
            elif len(el_stack) == 2 and name == 'uses-privilege':
                # check below
                priv = attrs.get('ml:name')
                if priv:
                    self._privileges.append(priv)

            el_stack.append(name)

        def end_handler(_):
            el_stack.pop()

        p.StartElementHandler = start_handler
        p.EndElementHandler = end_handler

        try:
            if file:
                p.ParseFile(file)
            elif text:
                p.Parse(text)

        except xml.parsers.expat.ExpatError as e:
            raise BuildException("failed to parse " + label) from e

        if not self._package_name:
            warn('Expected a <manifest ml:package="..."> attribute in {0}'.format(label))

    def exists(self):
        return os.path.exists(self._path)

    @property
    def package_name(self):
        return self._package_name

    @property
    def components(self):
        return self._components

    @property
    def developer_id(self):
        return self._developer_id

    @developer_id.setter
    def developer_id(self, val):
        self._developer_id = val

    @property
    def min_api_level(self):
        return self._min_api_level

    @property
    def privileges(self):
        return self._privileges

    def validate(self, opts, project, pkgflags, mappings):
        """
        Validate the manifest according to the schame and report a list of warnings and errors.
        This can run validation twice: once in a "lax" mode that supports forward
        compatibility by allowing unknown attributes and elements, then,
        if no errors are found, again in a "strict" mode that complains about
        any unrecognized attributes and elements, and reports those as warnings.

        :param opts: mabu options
        :param project: package project
        :param pkgflags: package_flags.PackageFlags
        :param mappings: projects.PackageMappings
        :return: warnings and errors
        :rtype: (list[str],list[str])
        """
        manifest_warnings, manifest_errors = self._validate_manifest(opts, project, pkgflags, mappings)
        schema_warnings, schema_errors = self.validate_schema(opts.verbose, pkgflags.strict_manifest)
        return manifest_warnings + schema_warnings, manifest_errors + schema_errors

    @staticmethod
    def _validate_manifest(opts, project, pkgflags, mappings):
        """
        Make sure the manifest follows certain rules
        :param opts: mabu options
        :type project: project.Project
        :type pkgflags: package_flags.PackageFlags
        :type mappings: package.PackageMappings
        :rtype: (list[str],list[str])
        """
        warnings = []
        errors = []

        mf = project.manifest
        if not mf:
            return warnings, errors  # hrm, shouldn't have gotten this far

        # perform simple checks
        pkg_id = mf.package_name
        if not pkg_id:
            warnings.append("manifest is lacking a <manifest ml:package> attribute")
        else:
            # make sure package name is legitimate
            pkg_id_regex = r'[a-z0-9_]+(\.[a-z0-9_]+)*(-[a-zA-Z0-9]*)?'
            if not re.fullmatch(pkg_id_regex, pkg_id):
                warnings.append("<manifest ml:package> '{}' does not follow convention (regex: {})".format(
                    pkg_id, pkg_id_regex))
            elif len(pkg_id) > 127:
                warnings.append("<manifest ml:package> is too long (maximum 127 characters)", )

        report_contents = False
        if pkgflags.validate_manifest:
            # make sure all the binaries mentioned in the manifest actually ship
            all_remotes = [remote.replace('\\', '/') for (local, remote)
                           in mappings.build_mapping + mappings.data_mapping if remote]

            for component in mf.components:
                report = errors if pkgflags.enforce_manifest and pkgflags.validate_manifest else warnings
                if not component.name:
                    report.append("<component> missing a ml:name attribute")
                if not component.binary_file:
                    report.append(
                        "<component ml:name=\"{0}\"> missing the ml:binary_name attribute".format(component.name))
                else:
                    # make sure it's recorded
                    bin_found = component.binary_file in all_remotes or \
                                ("./" + component.binary_file) in all_remotes
                    if not bin_found:
                        report.append(
                            "<component ml:name=\"{}\"> has ml:binary_name \"{}\" which is not bundled in this package"
                                .format(component.name, component.binary_file))
                        report_contents = True

            if report_contents:
                report.append("these are the package's files (see --print-outputs):\n"
                              "\t{}".format("\n\t".join(all_remotes)))

            # now, validate the schema
            vwarnings, verrors = project.manifest.validate_schema(verbose=opts.verbose, strict=pkgflags.strict_manifest)
            if pkgflags.enforce_manifest:
                # problems are errors
                warnings += vwarnings
                errors += verrors
            else:
                # problems are just warnings
                warnings += vwarnings + verrors

        return warnings, errors

    def validate_schema(self, verbose=True, strict=True):
        """
        Validate the manifest according to the schame and report a list of warnings and errors.
        This can run validation twice: once in a "lax" mode that supports forward
        compatibility by allowing unknown attributes and elements, then,
        if no errors are found, again in a "strict" mode that complains about
        any unrecognized attributes and elements, and reports those as warnings.

        :return: warnings and errors
        :rtype: (list[str],list[str])
        """
        warnings = []
        errors = []

        warnings += self._validate_privileges()

        root = os.path.join(DATA_DIR, "device")

        try:
            import xmlschema

            with open(os.path.join(root, "manifest-lax.xsd"), 'r') as f:
                schema_lax = xmlschema.XMLSchema(f)
            with open(os.path.join(root, "manifest.xsd"), 'r') as f:
                schema_strict = xmlschema.XMLSchema(f)

        except ImportError as e:
            diags.error("installation problem, cannot validate manifest: " + str(e), die=False)
            return warnings, errors

        except IOError as e:
            diags.error("failed to locate manifest schema; cannot validate manifest: " + str(e), die=False)
            return warnings, errors

        def respell(msg):
            msg = msg.replace(" a un", " an un")  # fix grammar error
            msg = msg.replace('failed', 'issues')  # "failed validating" -> "issues validating"
            return msg

        def massage(exc, verbose):
            if not verbose:
                msg = getattr(exc, 'reason')
                if msg:
                    return [msg]

            # not an XMLSchemaChildrenValidationError?
            msg = str(exc)
            msg = respell(msg)

            if not verbose:
                # only show the low-level reason
                msg_lines = msg.split('\n')
                reasons = [msg for msg in msg_lines if "Reason: " in msg]
                if reasons:
                    return [reason.replace("Reason: ", "") for reason in reasons]

            # fall through
            return [msg]

        if strict:
            # must match exactly
            with open(self.path, 'rb') as f:
                for error in schema_strict.iter_errors(f):
                    errors += massage(error, verbose)

        else:
            # lax validation: make sure required stuff is present...
            with open(self.path, 'rb') as f:
                for error in schema_lax.iter_errors(f):
                    errors += massage(error, verbose)

            if not errors:
                # ... and if that passes, just warn about unexpected content
                # found by the strict scan
                with open(self.path, 'rb') as f:
                    for error in schema_strict.iter_errors(f):
                        warnings += massage(error, False)

        return warnings, errors

    def _validate_privileges(self):
        global global_known_privileges
        if not global_known_privileges:
            privs_dat = os.path.join(DATA_DIR, 'device/privileges.dat')
            try:
                with open(privs_dat, 'rb') as f:
                    import pickle
                    global_known_privileges[:] = pickle.load(f)

            except IOError as e:
                diags.warn("cannot open '{}': <uses-privilege> elements cannot be verified\n{}".format(privs_dat, e))
                return []

        messages = []
        for priv in self._privileges:
            if priv not in global_known_privileges:
                messages.append('unknown privilege in <uses-privilege ml:name="{}">'.format(priv))
        return messages
