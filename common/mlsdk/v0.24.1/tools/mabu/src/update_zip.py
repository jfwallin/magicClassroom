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
import zipfile
from zipfile import ZipFile, ZIP_STORED, ZipInfo


class UpdatingZipFile(ZipFile):
    """
    ZipFile variant that supports updating an existing .zip file by using
    the underlying ZipFile class to construct a temporary file which contains
    changes (added/updated files), tracks which files should be deleted, and
    tracks unchanged files.  Before the receiver is close()'d, the contents of the
    original file are appended to the file.
    """
    def __init__(self, file, compression=ZIP_STORED, allowZip64=False):
        self._original_file = file

        # get handles on the current state
        self._original_infos = {}
        try:
            self._original_fp = open(self._original_file, 'rb')
            try:
                with ZipFile(self._original_fp, 'r') as zf:
                    for info in zf.infolist():
                        self._original_infos[info.filename] = info
            except zipfile.BadZipFile:
                pass
        except OSError:
            self._original_fp = None

        # initialize object on a temporary file
        super(UpdatingZipFile, self).__init__(file + '.tmp', mode='w',
                                              compression=compression,
                                              allowZip64=allowZip64)

    def writestr(self, zinfo_or_arcname, bytes, compress_type=None):
        if isinstance(zinfo_or_arcname, ZipInfo):
            arcname = zinfo_or_arcname.filename
        else:
            arcname = zinfo_or_arcname

        if arcname in self._original_infos:
            del self._original_infos[arcname]

        super(UpdatingZipFile, self).writestr(zinfo_or_arcname,
                                              bytes, compress_type=compress_type)

    def write(self, filename, arcname=None, compress_type=None):
        arcname = arcname or filename

        if arcname in self._original_infos:
            del self._original_infos[arcname]

        super(UpdatingZipFile, self).write(filename,
                                           arcname=arcname, compress_type=compress_type)

    def remove_file(self, zinfo_or_arcname):
        """
        Mark an intent to delete the entry
        """
        if isinstance(zinfo_or_arcname, ZipInfo):
            arcname = zinfo_or_arcname.filename
        else:
            arcname = zinfo_or_arcname

        if arcname in self._original_infos:
            del self._original_infos[arcname]

    def close(self):
        # before closing, append any unmodified contents
        if self._original_infos:
            with self._lock:
                for arcname, zinfo in self._original_infos.items():
                    # point to header + content head
                    self._original_fp.seek(zinfo.header_offset)

                    # update
                    zinfo.header_offset = self.fp.tell()

                    # copy contents over
                    cnt = zinfo.compress_size + len(zinfo.FileHeader(zinfo.extract_version >= zipfile.ZIP64_VERSION))
                    while cnt > 0:
                        to_use = min(1048576, cnt)
                        self.fp.write(self._original_fp.read(to_use))
                        cnt -= to_use

                    self.fp.flush()

                    self.filelist.append(zinfo)
                    self.NameToInfo[zinfo.filename] = zinfo
                    self.start_dir = self.fp.tell()

            self._original_infos.clear()

            # emit directory in same order as contents
            self.filelist.sort(key=lambda a: a.header_offset)

        super(UpdatingZipFile, self).close()

        if self._original_fp:
            self._original_fp.close()
            self._original_fp = None

        if self._original_file:
            os.replace(self.filename, self._original_file)
            self._original_file = None

