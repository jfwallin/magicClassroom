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
import json
import shutil
import stat
import subprocess

import sys
import zipfile
from json import JSONDecodeError

import diags
import os
import run_vars
import utils
from build_model import BuildException
from config import SIGN_ALG, HOSTOS, OS_WIN, BUILT_PACKAGE_EXT
from package_flags import PackageFlags
from package_utils import *

from project_vars import DATAS
from taildata import set_taildata_version, is_file_signed, has_taildata
from utils import format_command_line, path_to_filename

# JSON object names (cache toplevel)
C_PKGFLAGS = 'pkgflags'
C_ENTRIES = 'entries'

# JSON object names (Elements)
E_PKGPATH = 'pkgpath'
E_SRC = 'src'
E_SIZE = 'size'
E_HASH = 'hash'
E_COMPRESSION = 'compression'

E_XFRM_PATH = 'xfrm_path'
E_XFRM_FLAGS = 'xfrm_flags'
E_XFRM_SIZE = 'xfrm_size'
E_XFRM_HASH = 'xfrm_hash'

# Flags for PackageEntry._state
F_SAME = 0
F_CHANGED = 1
F_NEW = 2
F_MISSING = 3

# Keys of xfrmflags
X_SIGNED = 'signed'
X_TAILDATA = 'taildata'
X_MANIFEST = 'manifest'
X_MXS_DIGEST = 'mxs_digest'

# arguments for manifest transformation
XM_DEVID = 'developer_id'
XM_CERT_HASH = 'cert_hash'   # we depend on its contents
XM_MIN_API_LEVEL = 'min_api_level'

# arguments for signing
XS_CERT = 'cert'
# (do not need privkey since it is always parallel to .cert, thus redundant)
XS_ALG = 'alg'

# arguments for taildata
XT_SBOX = 'sbox'
XT_DEBUGGABLE = 'debuggable'
XT_DEVCERT = 'devcert'
XT_ARGS = 'args'
XT_VERSION = 'version'


class Transformer(object):
    def __init__(self):
        self._flags = {}
        self._verbose = False

    def flags(self):
        return self._flags

    def configure_for(self, source_path, pkgflags):
        self._verbose = pkgflags.verbose
        # override and return True if the transformer is needed
        return False

    def perform(self, staging):
        # override to do the work of the transformer
        pass


class FileSigner(Transformer):
    def __init__(self):
        Transformer.__init__(self)
        self._cert_file = None
        self._privkey_file = None

    def configure_for(self, source_path, pkgflags):
        # signing: depends on whether a certificate is supplied
        if not pkgflags.signer:
            return False

        if os.path.splitext(source_path)[1].lower() == '.sym':
            if pkgflags.verbose:
                diags.info("not signing .sym file '{0}'".format(source_path))
            return False

        if is_file_signed(source_path) or has_taildata(source_path):
            if pkgflags.verbose:
                diags.warn("incoming file is already secured; cannot sign '{}'".format(source_path))
            return False

        Transformer.configure_for(self, source_path, pkgflags)

        self._cert_file, self._privkey_file = pkgflags.cert_file, pkgflags.privkey_file

        new_flags = None
        if self._cert_file:
            new_flags = { XS_CERT: self._cert_file }

        self._flags = new_flags

        return True

    def perform(self, staging):
        """
        Sign the given file.
        """
        signer_tool = run_vars.runtime_values[run_vars.SIGN_FILE]

        # sign with developer cert
        args = [
            signer_tool,
            SIGN_ALG,
            self._privkey_file,
            self._cert_file,
            staging
        ]

        if self._verbose:
            diags.info("running signer tool: {0}".format(format_command_line(args)))

        cmp = subprocess.run(args,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
        if cmp.returncode != 0:
            if HOSTOS == OS_WIN:
                # if there is no content, we may be lacking DLLs
                msg = "" if cmp.stdout.strip() + cmp.stderr.strip() else \
                    "\nBe sure the MSVC 2015 redistributable is installed:\n" \
                    "https://www.microsoft.com/en-us/download/details.aspx?id=48145"
            else:
                msg = ""

            diags.error("signing failed with error {0}:\n{1}{2}{3}".format(
                hex(cmp.returncode), cmp.stdout, cmp.stderr, msg
            ))
            return

        if self._verbose:
            diags.info("signed '{}'".format(staging))


class TailDataApplier(Transformer):
    def __init__(self):
        Transformer.__init__(self)
        self._args = None

    def configure_for(self, source_path, pkgflags):
        # tail_data: depends on debuggable, cert flag, and tail data args
        if not pkgflags.debuggable and not pkgflags.signer and not pkgflags.tail_data_args:
            return False

        ext = os.path.splitext(source_path)[1].lower()
        if ext == '.sym':
            if pkgflags.verbose:
                diags.info("not adding tail data to .sym file '{}'".format(source_path))
            return False

        if is_file_signed(source_path) or has_taildata(source_path):
            if pkgflags.verbose:
                diags.info("not adding tail data to secured file: '{}'".format(source_path))
            return False

        Transformer.configure_for(self, source_path, pkgflags)

        self._args = []
        new_flags = {}

        # set sandbox
        self._args += ['--sbox', 'USER']
        # not configurable, but different from earlier releases, so force re-run
        new_flags[XT_SBOX] = 'USER'

        if pkgflags.debuggable:
            self._args.append('--debuggable')
            new_flags[XT_DEBUGGABLE] = pkgflags.debuggable

        if ext == BUILT_PACKAGE_EXT:
            if pkgflags.signer:
                self._args.append('--devcert')
                self._args.append(pkgflags.cert_file)
                new_flags[XT_DEVCERT] = pkgflags.cert_file

        if pkgflags.tail_data_args:
            self._args += pkgflags.tail_data_args
            new_flags[XT_ARGS] = pkgflags.tail_data_args

        new_flags[XT_VERSION] = taildata_module().ML_TAIL_DATA.ML_TAIL_DATA_LATEST_VERSION

        self._flags = new_flags
        return True

    def perform(self, staging):
        """
        Apply tail data
        """
        tail_data = taildata_module()
        args = [
                sys.executable,
                tail_data.__file__
            ] + \
            self._args + \
            [
                staging
            ]

        if self._verbose:
            diags.info("setting tail data: {0}".format(format_command_line(args)))

        try:
            fs = os.stat(staging)

            # If the file is read-only force it to be writable.
            if fs.st_mode & stat.S_IWRITE == 0:
                os.chmod(staging, fs.st_mode | stat.S_IWRITE)
        except NotImplemented:
            pass
        except OSError:
            pass

        # run, but stifle embedded prints, only printing on error
        myenv = os.environ.copy()
        myenv['PYTHONPATH'] = os.pathsep.join(sys.path)

        cmp = subprocess.run(args,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True,
                             env=myenv)
        if cmp.returncode != 0:
            diags.error("setting tail data failed with error {0}:\n{1}{2}".format(
                hex(cmp.returncode), cmp.stdout, cmp.stderr
            ))
            return False

        if self._verbose:
            diags.info("applied tail data to '{}'".format(staging))

        return True


class ManifestUpdater(Transformer):
    """
    If signing, update the manifest.xml:

    * to include the <manifest ml:developer_id> attribute
    copied from the developer certificate's Subject CN field, if missing

    * to inject <application ml:min_api_level> attribute
    specified from any .package option, if missing

    Include the alternate copy of the manifest.xml in the package.
    """
    def __init__(self):
        Transformer.__init__(self)
        self._manifest = None
        self._developer_id = None
        self._min_api_level = None

    def configure_for(self, source_path, pkgflags):
        # updating manifest: depends on whether a certificate is supplied
        if not pkgflags.manifest:
            return False

        self._manifest = pkgflags.manifest

        Transformer.configure_for(self, source_path, pkgflags)

        self._flags = {}

        if pkgflags.signer:
            updated_developer_id = self._configure_developer_id(pkgflags)
        else:
            updated_developer_id = False

        updated_min_api_level = self._configure_min_api_level(pkgflags)

        return updated_developer_id or updated_min_api_level

    def _configure_developer_id(self, pkgflags):
        try:
            from OpenSSL import crypto
        except ImportError:
            if not pkgflags.manifest.developer_id:
                diags.warn("Installation problem: cannot update <manifest ml:developer_id=...> (missing newer SSL library).  "
                           "Please edit the manifest to reflect your certificate's subject common name, "
                           "else the package will fail to install.", file=pkgflags.manifest.path)
            return False

        try:
            # fetch the common name from the certificate
            try:
                with open(pkgflags.cert_file, 'rt') as f:
                    buffer = f.read()

                cert = crypto.load_certificate(crypto.FILETYPE_PEM, buffer)
            except ValueError as e:
                diags.error("failed to read certificate; verify it is correctly generated: {}".format(e))
                return False

            # The certificate name in a developer cert is just the user's developer UUID.
            # The distribution cert has the developer UUID (or short name) plus '.' plus the application UUID.
            dev_id = cert.get_subject().CN
            diags.debug("CN from cert is: {}".format(dev_id))
            self._developer_id = dev_id

            # no need if it is already provided and matching
            if pkgflags.manifest.developer_id == self._developer_id:
                return False

            if pkgflags.manifest.developer_id and not pkgflags.update_manifest_developer_id:
                # note: we do not modify the manifest in a mismatch, unless required (e.g. re-signing)
                diags.warn("<manifest ml:developer_id=\"{}\"> does not match "
                           "subject common name in your certificate, \"{}\"; leaving manifest alone "
                           "(if this is not what you want, edit or remove the attribute)".format(
                             pkgflags.manifest.developer_id, self._developer_id),
                           file=pkgflags.manifest.path)
                self._developer_id = pkgflags.manifest.developer_id
                return False

            cert_hash = utils.hash_buffer_contents(bytes(buffer, 'ascii'))
        except crypto.Error as e:
            diags.warn("failed to fetch developer ID from certificate:".format(e.args))
            for arg in e.args:
                diags.warn("{0}".format('\n'.join([': '.join(a) for a in arg])))
            return False

        self._flags[XM_DEVID] = self._developer_id
        self._flags[XM_CERT_HASH] = cert_hash

        return True

    def _configure_min_api_level(self, pkgflags):
        if not pkgflags.min_api_level:
            if not pkgflags.manifest.min_api_level:
                # this attribute is not required in the schema (yet),
                # but it is required semantically to avoid broken apps
                if not self._min_api_level:
                    if pkgflags.validate_manifest and pkgflags.enforce_manifest and pkgflags.strict_manifest:
                        diag_func = diags.error
                    else:
                        diag_func = diags.warn

                    diag_func('missing <application ml:min_api_level=...> attribute\n'
                              '(This tells the device, Developer Portal, and App Store which APIs your program\n'
                              'is using, in order to prevent installing the package on incompatible devices.\n'
                              'See $MLSDK/include/ml_platform.h for the latest level.)',
                              file=pkgflags.manifest.path)
            return False

        self._min_api_level = pkgflags.min_api_level

        # no need if it is already provided and matching
        if pkgflags.manifest.min_api_level == self._min_api_level:
            return False

        if pkgflags.manifest.min_api_level:
            # note: we do not modify the manifest in a mismatch,
            # since the user may be intentionally doing this
            diags.warn("<application ml:min_api_level=\"{}\"> does not match "
                       "build-time option, which specifies \"{}\"; leaving manifest alone "
                       "(if this is not what you want, edit or remove the attribute)".format(
                         pkgflags.manifest.min_api_level, self._min_api_level),
                       file=pkgflags.manifest.path)
            self._min_api_level = pkgflags.manifest.min_api_level
            return False

        self._flags[XM_MIN_API_LEVEL] = self._min_api_level

        return True

    def perform(self, staging):
        """
        Change the manifest for packaging
        """
        if self._verbose:
            diags.info("transforming manifest")

        import xml.etree.ElementTree
        xml.etree.ElementTree.register_namespace('ml', 'magicleap')
        tree = xml.etree.ElementTree.parse(self._manifest.path)
        root = tree.getroot()
        assert root.tag == 'manifest'  # tested in Manifest already, just sanity

        # elementtree is not too smart here -- need to use this expanded version,
        # not 'ml:...' to make this work (see https://stackoverflow.com/questions/14853243/parsing-xml-with-namespace-in-python-via-elementtree)
        # (and QName doesn't work either...)
        if self._developer_id:
            root.attrib['{magicleap}developer_id'] = self._developer_id
            if self._verbose:
                diags.info("set <manifest ml:developer_id=\"{}\">".format(self._developer_id))

        if self._min_api_level:
            application = root.find('application')
            if application is not None:
                application.attrib['{magicleap}min_api_level'] = str(self._min_api_level)
                if self._verbose:
                    diags.info("set <manifest ml:min_api_level=\"{}\">".format(self._min_api_level))

        tree.write(staging, encoding='UTF-8')


class LineConverter(Transformer):
    def __init__(self):
        Transformer.__init__(self)
        self._content = None

    def configure_for(self, source_path, pkgflags):
        Transformer.configure_for(self, source_path, pkgflags)
        try:
            with open(source_path, 'rb') as f:
                self._content = f.read()
        except OSError:
            return False
        return b'\r\n' in self._content or b'\r' in self._content

    def perform(self, staging):
        lf_converted = self._content.replace(b'\r\n', b'\n')
        with open(staging, 'wb') as f:
            f.write(lf_converted)


class PackageEntry(object):
    """
    This represents a single entry destined for an .mpk, which in
    the simplest case is copied from some @_local_path into the
    .mpk at @_pkg_rel_path.

    For secure binaries, files may be modified according
    to steps in @_xfrms.  Such files may have a staging copy
    placed at @_xfrmed_path to avoid modifying external files
    (e.g. from MLSDK).  But to avoid unnecessary copying, the
    original file may be modified in-place.  In such cases,
    @_xfrmed_path is the same as @_local_path.

    Between package rebuilds, incremental updates are nominally
    supported by comparing @_xfrm_flags and changes in previous
    files from the markers in @_local_size, @_local_hash, and
    @_xfrmed_size, @_xfrmed_hash.

    """

    @staticmethod
    def from_json(cache_file, ent):
        def ensure(test):
            if not test:
                raise ValueError("{}: expected package build cache entry ({})".format(cache_file, ent))

        ensure(E_SRC in ent and E_PKGPATH in ent and E_COMPRESSION in ent)

        pe = PackageEntry(ent[E_SRC], ent[E_PKGPATH], ent[E_COMPRESSION])
        pe._local_size = ent.get(E_SIZE, -1)
        pe._local_hash = ent.get(E_HASH, None)

        if E_XFRM_FLAGS in ent and E_XFRM_PATH in ent:
            xfrm_flags = ent[E_XFRM_FLAGS]
            ensure(isinstance(xfrm_flags, dict))
            pe._xfrm_flags = xfrm_flags
            pe._xfrmed_path = ent[E_XFRM_PATH]
            pe._xfrmed_size = ent[E_XFRM_SIZE]
            pe._xfrmed_hash = ent.get(E_XFRM_HASH, None)

        return pe

    def __init__(self, local_path, pkg_rel_path, compression):
        self._local_path = local_path
        self._pkg_rel_path = pkg_rel_path
        self._compression = compression

        self._local_size = -1
        self._local_hash = None
        self._local_state = F_NEW

        self._xfrmed_path = local_path
        self._xfrm_flags = {}
        self._xfrms = []
        """ :type: List[package_updater.Transformer] """
        self._xfrmed_size = -1
        self._xfrmed_hash = None
        self._xfrmed_state = F_NEW

    def __repr__(self):
        ent = self.to_json()
        ent['state'] = self._local_state
        ent['xfrm_state'] = self._xfrmed_state
        return str(ent)

    def to_json(self):
        ent = {
            E_SRC: self._local_path,
            E_PKGPATH: self._pkg_rel_path,
            E_SIZE: self._local_size,
            E_HASH: self._local_hash,
            E_COMPRESSION: self._compression
        }
        if self._xfrmed_path != self._local_path:
            ent[E_XFRM_FLAGS] = self._xfrm_flags
            ent[E_XFRM_PATH] = self._xfrmed_path
            ent[E_XFRM_SIZE] = self._xfrmed_size
            ent[E_XFRM_HASH] = self._xfrmed_hash

        return ent

    @property
    def transformed_path(self):
        return self._xfrmed_path

    def reset_transformed_path(self):
        self._xfrmed_path = self._local_path

    @staticmethod
    def _file_state(path, size, hash, arc_size=None):
        """
        See what effective state a file is in
        :param path: source file path
        :param size: last detected size
        :param hash: last detected hash
        :param arc_size: current size in .mpk or None
        :return: new state, new size, new moddate, new hash
        """
        try:
            st = os.stat(path, follow_symlinks=True)

            st_size = st.st_size
            st_hash = utils.hash_file_contents(path)

            if size < 0:
                # no state
                return F_NEW, st_size, st_hash

            elif st_hash != hash or st_size != size or (arc_size and arc_size != st_size):
                return F_CHANGED, st_size, st_hash

            else:
                return F_SAME, st_size, st_hash

        except FileNotFoundError:
            return F_MISSING, -1, None

        except IOError as e:
            diags.warn(str(e))
            return F_CHANGED, -1, None

    def set_transforms(self, pkgflags, zip_info):
        """
        Update self._local_state / self._xfrmed_state / self._xfrms according to the local file
        and pkgflags, compared to the previous state.
        :type pkgflags: PackageFlags
        :type zip_info: current ZipInfo
        """
        rebuild = False

        # remember what we last did
        old_flags = dict(self._xfrm_flags)
        old_compression = self._compression

        # Check the original and transformed source file for changes.
        #
        # If we modified the original file in-place -- when
        # _local_path == _xfrmed_path -- then check the
        # changes against what we recorded after modifying the file,
        # else, check against the original source file.

        arc_size = zip_info and zip_info.file_size

        state, size, hash = self._file_state(
            self._local_path, self._local_size, self._local_hash,
            self._local_path == self._xfrmed_path and arc_size)  # check .mpk entry size only if not transformed

        self._local_state = state
        self._local_size = size
        self._local_hash = hash

        if self._local_path != self._xfrmed_path:
            state, size, hash = self._file_state(
                self._xfrmed_path, self._xfrmed_size, self._xfrmed_hash,
                arc_size)  # always check archive size for transformed file

        self._xfrmed_state = state
        self._xfrmed_size = size
        self._xfrmed_hash = hash

        self._xfrms.clear()
        self._xfrm_flags.clear()

        # some transforms only apply to security-modified files
        if is_secure_file(self._local_path):
            tailer = TailDataApplier()
            if tailer.configure_for(self._local_path, pkgflags):
                self._xfrms.append(tailer)
                self._xfrm_flags[X_TAILDATA] = tailer.flags()

            signer = FileSigner()
            if signer.configure_for(self._local_path, pkgflags):
                self._xfrms.append(signer)
                self._xfrm_flags[X_SIGNED] = signer.flags()

        elif pkgflags.is_manifest(self._local_path):
            mfxfrm = ManifestUpdater()
            if mfxfrm.configure_for(self._local_path, pkgflags):
                self._xfrms.append(mfxfrm)
                self._xfrm_flags[X_MANIFEST] = mfxfrm.flags()

        # see if transformers changed
        if old_flags != self._xfrm_flags or old_compression != self._compression:
            rebuild = True

        # make a transform file if we are modifying the source
        # and the file is outside the generated tree
        if self._xfrms:
            if self._xfrmed_path != self._local_path:
                # already made one
                xfrm_path = self._xfrmed_path
            else:
                # write to a copy
                xfrm_path = os.path.join(
                    pkgflags.package_cache_dir,
                    path_to_filename(self._local_path or self._pkg_rel_path))
            self._xfrmed_path = xfrm_path

        else:
            # leave any _xfrmed_path as it was, to be deleted eventually
            pass

        if rebuild:
            self._xfrmed_state = F_CHANGED

    def is_dirty(self):
        return self._local_state != F_SAME or self._xfrmed_state != F_SAME

    @property
    def transforms(self):
        return list(self._xfrms)

    def transform(self, state):
        """
        If needed, transform the source for packaging
        :type state: PackagingState
        """
        if not self._xfrms:
            if self._xfrmed_path:
                try:
                    os.remove(self._xfrmed_path)
                except OSError as e:
                    diags.warn("failed to remove obsolete file", file=self._xfrmed_path)
                self._xfrmed_path = None
                self._xfrmed_size = -1
                self._xfrmed_hash = None
            return

        staging = self._xfrmed_path
        if staging != self._local_path:
            try:
                if os.path.exists(staging):
                    os.chmod(staging, 0o777)
                    os.remove(staging)
            except OSError:
                pass
            shutil.copyfile(self._local_path, staging)

        for xfrm in self._xfrms:
            xfrm.perform(staging)

        # Update the sizes/hashes
        state, size, hash = \
            self._file_state(self._xfrmed_path, self._xfrmed_size, self._xfrmed_hash)

        self._xfrmed_state, self._xfrmed_size, self._xfrmed_hash = state, size, hash

        if self._local_path == self._xfrmed_path:
            # we just modified both together
            self._local_state, self._local_size, self._local_hash = state, size, hash

    @property
    def local_path(self):
        return self._local_path

    @property
    def zip_source(self):
        """
        Get the appropriate source path
        """
        return self._xfrmed_path or self._local_path

    @property
    def zip_path(self):
        """
        Get the path inside the zip
        """
        return self._pkg_rel_path

    @property
    def local_state(self):
        return self._local_state

    @property
    def transformed_state(self):
        return self._xfrmed_state


class PackageCachedState(object):
    """
    This is the model of a package which has been previously generated
    or which is being updated, which is used to do incremental updates.

    File format:
    JSON encoding of a list of items, each corresponding to some package entry:
    [
      {
        "pkgpath": "...package-relative path...",
        "src": "...local path to original source...",
        "xfrmpath": "...local source path for transformed file...",
        "xfrmflags": { "signed" : <...>, "taildata": <...> ... },  # actions and settings used to transform (X_...)
        "size": <size in bytes>,
        "moddate": <modification date in epoch>,
        "hash": <SHA-1 hash>
      }
    ]
    """
    def __init__(self, cache_file):
        self._cache_file = cache_file
        self._entries = {}
        """ :type: Dict[tuple[str,str],PackageEntry] """
        self._pkg_flags = {}

    @property
    def package_flags(self):
        return self._pkg_flags

    @package_flags.setter
    def package_flags(self, flags):
         self._pkg_flags = flags

    def load(self, verbose, clean):
        if not os.path.exists(self._cache_file):
            return

        def warn(msg):
            diags.warn(msg, file=self._cache_file)

        try:
            with open(self._cache_file, 'rt', encoding='utf-8', errors='replace') as f:
                content = f.read()
                toplevel = content and json.loads(content) or []

            if not isinstance(toplevel, dict):
                raise ValueError("unexpected format for package build cache")

            self._pkg_flags = toplevel.get(C_PKGFLAGS)
            if self._pkg_flags is None:
                raise ValueError("no package flags found")

            ents = toplevel.get(C_ENTRIES, {})
            if ents is None:
                raise ValueError("no package entry list found")

            for ent in ents:
                pe = PackageEntry.from_json(self._cache_file, ent)
                self._entries[(pe.local_path, pe.zip_path)] = pe

        except JSONDecodeError as e:
            if not clean:
                warn("corrupt package build cache, rebuilding ({})".format(e))
                self._entries.clear()

        except ValueError as e:
            if verbose and not clean:
                warn("outdated package build cache, rebuilding ({})".format(e))
            self._entries.clear()

        except IOError:
            # no such file, etc.
            pass

    def save(self, verbose):
        ents = []
        toplevel = { C_PKGFLAGS : self._pkg_flags, C_ENTRIES : ents }

        for path, pe in self._entries.items():
            ent = pe.to_json()
            ents.append(ent)

        ents.sort(key=lambda ent: ent[E_SRC])
        with open(self._cache_file, 'wt', encoding='utf-8') as f:
            json.dump(toplevel, f, indent=1, sort_keys=True)

        if verbose:
            diags.info("wrote package build cache to '{}'".format(self._cache_file))

    def entries(self):
        """ :rtype: Dict[tuple[str,str],PackageEntry] """
        return self._entries

    def find_by_pkgpath(self, pkgpath):
        for (local, arc), ent in self._entries.items():
            if arc == pkgpath:
                return ent
        return None


class PackagingState(object):
    def __init__(self, pkgflags, zf):
        self.pkgflags = pkgflags
        """ :type: package_flags.PackageFlags """
        self.zipfile = zf
        """ :type: update_zip.UpdatingZipFile """


class PackagingStep(object):
    """
    A single step in the packaging
    """
    def perform(self, state):
        pass


class PackageEntryStep(PackagingStep):
    """
    A step that manipulates a PackageEntry
    """
    def __init__(self, entry, path=None):
        PackagingStep.__init__(self)
        self._entry = entry
        """ :type: PackageEntry """
        self._path = path or entry.zip_path

        # accessible attribute for sorting
        self.order = 0

    @property
    def entry(self):
        return self._entry

    @property
    def zip_path(self):
        return self._path

    def perform(self, state):
        pass


class TransformFileStep(PackageEntryStep):
    """
    Run transforms on the given file.
    """
    def __init__(self, entry):
        PackageEntryStep.__init__(self, entry)

    def perform(self, state):
        """
        :type state: PackagingState
        """
        self._entry.transform(state)


class CleanupTransformedFileStep(PackageEntryStep):
    """
    Clean (delete) the given file if it has a transformed copy
    """
    def __init__(self, entry):
        PackageEntryStep.__init__(self, entry)

    def perform(self, state):
        """
        :type state: PackagingState
        """
        if self._entry.zip_source != self._entry.local_path:
            try:
                os.remove(self._entry.zip_source)
            except FileNotFoundError:
                # allow for parallel builds
                pass
            except PermissionError:
                pass


class BaseWriteArchiveFileStep(PackageEntryStep):
    """
    Add/update the file to the archive
    """
    def __init__(self, entry):
        PackageEntryStep.__init__(self, entry)

    def perform(self, state):
        """
        :type state: package_updater.PackagingState
        """
        state.zipfile.write(filename=self._entry.zip_source,
                            arcname=self._entry.zip_path,
                            compress_type=state.pkgflags.pyzip_compression_level())


class AddArchiveFileStep(BaseWriteArchiveFileStep):
    """
    Add the file to the archive
    """
    def __init__(self, entry):
        BaseWriteArchiveFileStep.__init__(self, entry)

    def perform(self, state):
        if state.pkgflags.verbose:
            diags.info("adding package entry '{0}'".format(self._entry.zip_path))
        BaseWriteArchiveFileStep.perform(self, state)


class UpdateArchiveFileStep(BaseWriteArchiveFileStep):
    """
    Update the existing file to the archive
    """
    def __init__(self, entry):
        BaseWriteArchiveFileStep.__init__(self, entry)

    def perform(self, state):
        if state.pkgflags.verbose:
            diags.info("updating package entry '{0}'".format(self._entry.zip_path))
        BaseWriteArchiveFileStep.perform(self, state)


class RemoveArchiveFileStep(PackageEntryStep):
    """
    Remove the given file from the archive
    """
    def __init__(self, entry):
        if isinstance(entry, PackageEntry):
            PackageEntryStep.__init__(self, entry)
        else:
            PackageEntryStep.__init__(self, None, entry)

    def perform(self, state):
        """
        :type state: PackagingState
        """
        if state.pkgflags.verbose:
            diags.info("removing package entry '{0}'".format(self._path))
        state.zipfile.remove_file(self._path)


class PackageUpdateScheduler(object):
    """
    This class creates a list of required changes given the previous and desired
    new state of a package.

    Whenever a package is successfully built, a @cache_file is written
    describing each @PackageEntry and its source file and metadata, any transformations
    the file required (signing, tail data) and its metadata, and the archive name
    and compression level.

    Before regenerating a package, this class examines the differences
    between the state implied by that cache file and the new state
    (defined by @build_mapping, @data_mapping, and @pkgflags), and
    provides a list of @PackageSteps telling how to update the archive.
    """

    def __init__(self, build_mapping, data_mapping, pkgflags, output_file, cache_file):
        self._build_mapping = build_mapping
        self._data_mapping = data_mapping
        self._pkgflags = pkgflags
        """ :type: package_flags.PackageFlags """
        self._output_file = output_file
        self._cache_file = cache_file

        self._zip_infos = {}

        # sync system with prefs
        set_taildata_version(pkgflags.taildata_version)

        self._recreate = not os.path.exists(self._output_file) or \
            not os.path.exists(self._cache_file)

        # read the current zip state
        try:
            with zipfile.ZipFile(self._output_file, 'r') as zf:
                for info in zf.infolist():
                    self._zip_infos[info.filename] = info
        except OSError:
            self._recreate = True
        except zipfile.BadZipFile as e:
            diags.warn("{0}: {1}".format(self._output_file, e))
            self._recreate = True

    def process_archive_candidate(self, local_src, arcname, zip_info, clean,
                                  entries, keys_seen, pkgpaths_seen, steps):

        if not arcname:
            return

        # canonicalize path
        arcname = arcname.replace('\\', '/')
        if arcname.startswith('/'):
            arcname = arcname[1:]

        arcname = self._pkgflags.arcname_prefix + arcname

        ent_key = (local_src, arcname)
        ent = entries.get(ent_key)
        """ :type: PackageEntry """

        diags.debug("processing archive file {}".format(ent_key))

        keys_seen.add(ent_key)
        pkgpaths_seen.add(arcname)

        if clean:
            if ent:
                steps.append(CleanupTransformedFileStep(ent))
            return

        found_in_zip = zip_info is not None
        if not ent:
            ent = PackageEntry(local_src, arcname, self._pkgflags.pyzip_compression_level())
            entries[ent_key] = ent

        ent.set_transforms(self._pkgflags, zip_info)

        if ent.is_dirty() or not found_in_zip:
            if ent.transforms:
                steps.append(TransformFileStep(ent))
            elif ent.transformed_path != ent.local_path and not ent.transforms:
                # dropped transforms, so remove transformed file
                steps.append(CleanupTransformedFileStep(ent))
                ent.reset_transformed_path()

            cls = found_in_zip and UpdateArchiveFileStep or AddArchiveFileStep
            steps.append(cls(ent))

        elif self._pkgflags.verbose:
            diags.info("not repackaging unmodified file '{}'".format(local_src))

    def generate(self, clean=False):
        """
        Generate a schedule.
        :rtype: tuple[list[package_updater.PackagingStep],package_updater.PackageCachedState,bool]
        """
        # read the cache
        cached_state = PackageCachedState(self._cache_file)

        cached_state.load(self._pkgflags.verbose, clean)

        steps = []

        entries_keys_seen = set()
        """ :type: Set[tuple[str,str]] """

        pkgpaths_seen = set()
        """ :type: Set[str] """

        entries = cached_state.entries()
        """ :type: Dict[tuple[str,str],PackageEntry] """

        def process(local_src, arcname):
            return self.process_archive_candidate(
                local_src, arcname, self._zip_infos.get(arcname),
                clean,
                entries, entries_keys_seen, pkgpaths_seen,
                steps)

        failed = False

        # examine built files
        for src, out_path in self._build_mapping:
            if not clean and not os.path.exists(src):
                diags.error("did not build file '{}'".format(src), die=False)
                failed = True
                continue

            process(src, out_path)

        # examine DATAS files (which may have ELFs)
        for src, out_path in self._data_mapping:
            if not clean and src and not os.path.exists(src):
                diags.error("{0}: no such file '{1}'".format(DATAS, src), die=False)
                failed = True
                continue

            process(src, out_path)

        if failed:
            raise BuildException("some source data files were missing")

        # remember what files we expect to generate
        known_arcnames = [zip_path for (local_src, zip_path) in entries]

        # remove files no longer present
        diags.debug("Considering delete steps: saw entries: {};\n"
                    "current archive paths: {}".format(
                        "\n".join(str(x) for x in entries_keys_seen),
                        "\n".join(str(x) for x in known_arcnames)))

        for key, uent in dict(entries).items():
            if key not in entries_keys_seen:
                if clean:
                    diags.debug("Scheduling transform cleanup of {}".format((key, uent)))
                    steps.append(CleanupTransformedFileStep(uent))
                elif key[1] not in pkgpaths_seen:
                    diags.debug("Scheduling removal of {}".format((key, uent)))
                    steps.append(RemoveArchiveFileStep(uent))
                del entries[key]

        if not clean:
            # remove obsolete files in .mpk (maybe user added them?)
            unknown_arcnames = set(self._zip_infos.keys())
            unknown_arcnames.difference_update(known_arcnames)
            diags.debug("Scheduling removal of obsolete archive members: {}"
                        .format(list(unknown_arcnames)))
            for name in unknown_arcnames:
                steps.append(RemoveArchiveFileStep(name))

        # keep files in name order
        def file_order(step):
            if isinstance(step, PackageEntryStep):
                return step.zip_path
            else:
                return ''

        steps.sort(key=file_order)

        # and ensure steps are in a reasonable order,
        # with any digest work after other transforms
        for step in steps:
            if isinstance(step, TransformFileStep):
                step.order = 0
            elif isinstance(step, RemoveArchiveFileStep):
                step.order = 2
            elif isinstance(step, BaseWriteArchiveFileStep):
                step.order = 3
            else:
                step.order = 0  # e.g. delete, etc.

        steps.sort(key=lambda step: step.order)

        return steps, cached_state, self._recreate

