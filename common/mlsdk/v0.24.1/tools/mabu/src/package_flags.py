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
import zipfile

import diags
from config import SIGN_PRIVKEY, SIGN_CERT
from project_vars import PKGFLAGS
from utils import as_boolean


class PackageFlags:
    def __init__(self, signer=None, tail_data_args=[], verbose=False):
        """
        :param signer: filepath for dev certificate
        :param tail_data_args: extra arguments to add_tail_data.py
        :param verbose: report verbose progress
        :param silent: whether to report any issues
        """
        self._manifest = None
        self._signer = signer
        self.tail_data_args = tail_data_args
        self.verbose = verbose

        self.debuggable = None

        self.allow_zip64 = True
        self.zip_compress_level = 5

        self.arcname_prefix = ''

        self.taildata_version = 3

        self.validate_manifest = True
        self.enforce_manifest = True
        self.strict_manifest = True

        self.update_manifest_developer_id = False

        self.use_hard_links = True

        self.min_api_level = None

        self.package_cache_dir = '.'

    @property
    def manifest(self):
        """ :rtype: manifest.Manifest """
        return self._manifest

    @manifest.setter
    def manifest(self, mf):
        """ :type: mf: manifest.Manifest """
        self._manifest = mf

    @property
    def signer(self):
        return self._signer

    @signer.setter
    def signer(self, path):
        self._signer = path

    @property
    def cert_file(self):
        return os.path.splitext(self.signer)[0] + SIGN_CERT

    @property
    def privkey_file(self):
        return os.path.splitext(self.signer)[0] + SIGN_PRIVKEY

    def set_from_project(self, project, silent=False):
        def warn(msg):
            if not silent:
                diags.warn(msg)

        self._manifest = project.manifest

        self.arcname_prefix = ''

        self.package_cache_dir = project.package_cache_path.resolve()

        flag_handlers = FlagHandlers(self, warn)

        for opt in list(project.combined[PKGFLAGS]):
            equ = opt.find('=')
            if equ <= 0:
                warn("Invalid package flag '{0}', ignoring".format(opt))
                continue
            name, value = opt[0:equ].strip(), opt[equ + 1:].strip()

            if hasattr(flag_handlers, name):
                getattr(flag_handlers, name)(name, opt, value)
            else:
                diags.warn("Unsupported flag in PKGFLAGS: '{0}', ignoring".format(name))

    def pyzip_compression_level(self):
        if self.zip_compress_level <= 3:
            return zipfile.ZIP_STORED
        else:
            return zipfile.ZIP_DEFLATED

    def is_manifest(self, local_path):
        if self._manifest:
            return os.path.split(self._manifest.path)[1].lower() == os.path.split(local_path)[1].lower() \
                and os.path.samefile(self._manifest.path, local_path)
        return False


class FlagHandlers(object):
    def __init__(self, flags, warn):
        self.flags = flags
        self.warn = warn

    @staticmethod
    def define_bool(optname, fieldname):
        def handle(self, name, opt, value):
            f = as_boolean(value, None)
            if f is None:
                self.warn("Expected boolean value in PKGFLAGS: '{0}={1}', "
                          "ignoring".format(name, opt))
            else:
                setattr(self.flags, fieldname, f)

        setattr(FlagHandlers, optname, handle)

    @staticmethod
    def define_int(optname, fieldname):
        def handle(self, name, opt, value):
            try:
                l = int(value)
                setattr(self.flags, fieldname, l)
            except ValueError:
                self.warn("Expected integer value in PKGFLAGS: '{0}={1}', "
                          "ignoring".format(name, opt))

        setattr(FlagHandlers, optname, handle)

    @staticmethod
    def define_str(optname, fieldname):
        def handle(self, _, _2, value):
            setattr(self.flags, fieldname, value)

        setattr(FlagHandlers, optname, handle)


FlagHandlers.define_bool('debuggable', 'debuggable')
FlagHandlers.define_bool('zip64', 'allow_zip64')
FlagHandlers.define_int('zipCompressLevel', 'zip_compress_level')
FlagHandlers.define_int('tailDataVersion', 'taildata_version')
FlagHandlers.define_bool('manifestValidation', 'validate_manifest')
FlagHandlers.define_bool('manifestEnforcement', 'enforce_manifest')
FlagHandlers.define_bool('manifestStrict', 'strict_manifest')
FlagHandlers.define_bool('useHardLinks', 'use_hard_links')
FlagHandlers.define_str('minApiLevel', 'min_api_level')
