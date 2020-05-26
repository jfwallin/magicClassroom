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
import os
import shutil
from json import JSONDecodeError

import build_env
import diags
import utils
from build_model import BuildException
from diags import info
from project_vars import DATAS
from utils import mkdir_p, user_relpath, copy_safely

# JSON object names (Elements)
E_DEST = 'dest'
E_SRC = 'src'

# Flags for DataLayoutEntry._state
F_SAME = 0
F_CHANGED = 1
F_NEW = 2
F_MISSING = 3


def is_linked(path):
    try:
        st = os.stat(path)
        return st.st_nlink > 1
    except OSError:
        return False


class DataLayoutEntry(object):
    """
    This represents a single file assigned to a layout,
    which was either copied or hardlinked from some @_local_src into the
    layout at @_layout_path.
    """
    @staticmethod
    def from_json(cache_file, ent):
        def ensure(test):
            if not test:
                raise ValueError("{}: expected data layout cache entry ({})".format(cache_file, ent))

        ensure(E_SRC in ent and E_DEST in ent)

        de = DataLayoutEntry(ent[E_SRC], ent[E_DEST])
        return de

    def __init__(self, src, dest):
        self._local_path = src
        self._layout_path = dest
        self._state = F_NEW

    def __repr__(self):
        ent = self.to_json()
        ent['state'] = self._state
        return str(ent)

    def to_json(self):
        return {
            E_SRC: self._local_path,
            E_DEST: self._layout_path
        }

    @property
    def local_path(self):
        return self._local_path

    @property
    def layout_path(self):
        return self._layout_path

    def is_dirty(self, layout_dir, use_links):
        is_changed = not os.path.exists(self._local_path)
        if not is_changed:
            try:
                dest_path = os.path.join(layout_dir, self._layout_path)
                dest_stat = os.stat(dest_path)
                src_stat = os.stat(self._local_path)

                if dest_stat.st_ino == src_stat.st_ino and \
                   dest_stat.st_dev == src_stat.st_dev:
                    # same exact file (hard link)
                    #
                    # It's "changed", however, if we don't want to use links.
                    is_changed = not use_links

                elif src_stat.st_size != dest_stat.st_size:
                    is_changed = True

                else:
                    # see if the contents changed
                    # (timestamp could be same even with changes)
                    src_hash = utils.hash_file_contents(self._local_path)
                    dest_hash = utils.hash_file_contents(dest_path)
                    is_changed = src_hash != dest_hash

            except (FileNotFoundError, NotADirectoryError):
                is_changed = True
            except PermissionError:
                # can't tell
                pass

        return is_changed


class DataLayoutCachedState(object):
    """
    This is the model of a package which has been previously generated
    or which is being updated, which is used to do incremental updates.

    File format:
    JSON encoding of a list of items, each corresponding to some package entry:
    [
      {
        "dest": "...layout-relative path...",
        "src": "...local path to original source...",
      }
    ]
    """
    def __init__(self, cache_file):
        self._cache_file = cache_file
        self._entries = {}
        """ :type: Dict[tuple[str,str],DataLayoutEntry] """

    def load(self, verbose, clean):
        if not os.path.exists(self._cache_file):
            return

        if verbose:
            diags.info("reading data layout cache from '{}'".format(self._cache_file))

        def warn(msg):
            diags.warn(msg, file=self._cache_file)

        try:
            with open(self._cache_file, 'rt', encoding='utf-8', errors='replace') as f:
                content = f.read()
                ents = content and json.loads(content) or []

            if not isinstance(ents, list):
                raise ValueError("unexpected format for data layout cache")

            for ent in ents:
                de = DataLayoutEntry.from_json(self._cache_file, ent)
                self._entries[(de.local_path, de.layout_path)] = de

        except JSONDecodeError as e:
            if not clean:
                warn("corrupt data layout cache, rebuilding ({})".format(e))
                self._entries.clear()

        except ValueError as e:
            if verbose and not clean:
                warn("outdated (?) data layout cache, rebuilding ({})".format(e))
            self._entries.clear()

        except IOError:
            # no such file, etc.
            pass

    def save(self, verbose):
        ents = []

        for path, pe in self._entries.items():
            ent = pe.to_json()
            ents.append(ent)

        ents.sort(key=lambda ent: ent[E_SRC])
        with open(self._cache_file, 'wt', encoding='utf-8') as f:
            json.dump(ents, f, indent=1, sort_keys=True)

        if verbose:
            diags.info("wrote data layout cache to '{}'".format(self._cache_file))

    def entries(self):
        """ :rtype: Dict[tuple[str,str],PackageEntry] """
        return self._entries

    def find_by_layout_path(self, layout_path):
        for (local, arc), ent in self._entries.items():
            if arc == layout_path:
                return ent
        return None


class DataLayoutState(object):
    def __init__(self, can_link, verbose, layout_dir):
        self.can_link = can_link
        self.verbose = verbose
        self.layout_dir = layout_dir
        self.dirs = set()
        """ :type: set(str) """

    def ensure_directory(self, dest):
        if dest in self.dirs:
            return
        self.dirs.add(dest)

        # delete any file in the way (in case user changes output file <--> directory)
        if os.path.isfile(dest):
            os.unlink(dest)

        mkdir_p(dest)

    def delete(self, dest):
        if os.path.isdir(dest):
            # in case user changed output directory -> file
            shutil.rmtree(dest)

            # no longer a created directory
            self.dirs.remove(dest)
        else:
            os.unlink(dest)


class DataLayoutStep(object):
    def __init__(self, entry):
        self.entry = entry

    def perform(self, state):
        # override
        pass


class UpdateDataFileStep(DataLayoutStep):
    def __init__(self, entry):
        DataLayoutStep.__init__(self, entry)

    def perform(self, state):
        src, dest = self.entry.local_path, os.path.join(state.layout_dir, self.entry.layout_path)

        if not os.path.exists(src):
            diags.error("{0}: no such file '{1}'".format(DATAS, src), die=False)
            return False

        state.ensure_directory(os.path.dirname(dest))

        linked_this_file = False

        if state.can_link:
            # note: no point doing os.path.exists() since this already does that...

            # reset assuming failure case
            linked_this_file = False

            # we can get weird behavior if parallel processes are linking at
            # the same time, so perform some retries
            for i in range(8):
                try:
                    os.link(src, dest)
                    # did it!  no more failure case.

                    if state.verbose:
                        info("linked '{0}' into '{1}'".format(
                            user_relpath(src),
                            user_relpath(dest)
                        ))
                    linked_this_file = True
                    break
                except FileExistsError:
                    try:
                        # delete the link and loop back
                        os.unlink(dest)
                    except OSError:
                        pass
                except NotImplementedError as e:
                    # ok, no worries (but why is `state.can_link` set?)
                    break
                except OSError as e:
                    # likely "Invalid cross-device link"
                    if state.verbose:
                        diags.warn("cannot link file, must copy: " + str(e))
                    break

        if not linked_this_file:
            # okay, need to explicitly copy
            if state.verbose:
                info("copying '{0}' into '{1}'...".format(
                    user_relpath(src),
                    user_relpath(dest)
                ))

            try:
                copy_safely(src, dest)
            except OSError as e:
                diags.error("failed to copy '{0}' into '{1}': {2}".format(
                    user_relpath(src),
                    user_relpath(dest),
                    str(e)), die=False)
                return False

        return True


class DeleteDataFileStep(DataLayoutStep):
    def __init__(self, entry):
        DataLayoutStep.__init__(self, entry)

    def perform(self, state):
        dest = os.path.join(state.layout_dir, self.entry.layout_path)

        try:
            if state.verbose:
                diags.info("cleaning '{0}'".format(user_relpath(dest)))

            state.delete(dest)
            return True
        except FileNotFoundError:
            # allow for parallel builds
            return True
        except PermissionError as e:
            diags.warn("failed to delete: {0}".format(e), file=dest)
            return False


class DataLayoutScheduler(object):
    """
    Create a list of DataLayoutSteps that will update the layout
    from its current state.
    """
    def __init__(self, data_mapping, layout_dir, cache_file, verbose=False, use_links=None):
        self._data_mapping = data_mapping
        self._layout_dir = layout_dir
        self._cache_file = cache_file
        self._verbose = verbose
        self._use_links = use_links

    def _process_file(self, local_src, layout_path, clean,
                      entries, entries_keys_seen, layout_paths_seen, steps):

        # canonicalize path
        layout_path = os.path.normpath(layout_path).replace('\\', '/')

        # do not put absolute paths inside an .mpk
        if layout_path.startswith('/') and build_env.platform().uses_packages():
            layout_path = layout_path[1:]

        ent_key = (local_src, layout_path)
        ent = entries.get(ent_key)
        """ :type: DataLayoutEntry """

        diags.debug("processing file {}".format(ent_key))

        entries_keys_seen.add(ent_key)
        layout_paths_seen.add(layout_path)

        if clean:
            dest = os.path.join(self._layout_dir, layout_path)
            if ent and os.path.exists(dest):
                steps.append(DeleteDataFileStep(ent))
            return

        if not ent:
            ent = DataLayoutEntry(local_src, layout_path)
            entries[ent_key] = ent

        if ent.is_dirty(self._layout_dir, self._use_links):
            steps.append(UpdateDataFileStep(ent))
        elif self._verbose:
            diags.info("skipping unmodified file '{}'".format(local_src))

    def generate(self, clean=False):
        """
        :rtype: tuple[list[DataLayoutStep],DataLayoutCachedState]
        """
        # read the cache
        cached_state = DataLayoutCachedState(self._cache_file)

        cached_state.load(self._verbose, clean)

        entries_keys_seen = set()
        """ :type: Set[tuple[str,str]] """

        layout_paths_seen = set()
        """ :type: Set[str] """

        entries = cached_state.entries()
        """ :type: Dict[tuple[str,str],DataLayoutEntry] """

        steps = []

        # track state of files in layout
        for local_src, layout_path in self._data_mapping:
            self._process_file(local_src, layout_path, clean,
                               entries, entries_keys_seen, layout_paths_seen, steps)

        # remove files no longer present
        diags.debug("Considering delete steps: saw entries: {};\n"
                    "saw layout paths: {}"
                    .format(entries_keys_seen, layout_paths_seen))
        for key, uent in dict(entries).items():
            if key not in entries_keys_seen:
                if clean or key[1] not in layout_paths_seen:
                    if not clean:
                        diags.debug("Scheduling delete of {}".format((key,uent)))
                    steps.append(DeleteDataFileStep(uent))
                del entries[key]

        # keep files in name order
        def file_order(step):
            return step.entry.local_path

        steps.sort(key=file_order)

        return steps, cached_state


class DataLayoutArranger(object):
    """
    This arranges DATAS content from .packages into the layout for
    runtime usage on host.

    This isn't done for device packaging, since the output goes directly
    (or, indirectly -- via a secured copy of a binary) into a zip file.
    """
    CACHE_FILE_NAME = 'layout.mf'

    def __init__(self, data_mappings, verbose, layout_dir, cache_dir, use_links=None):
        self._data_mappings = data_mappings
        self._verbose = verbose
        self._layout_dir = layout_dir

        self._cache_file = os.path.join(cache_dir, self.CACHE_FILE_NAME)

        try:
            mkdir_p(cache_dir)
        except OSError as e:
            diags.error("failed to create data layout cache directory: {0}".format(
                e, file=cache_dir
            ))

        if use_links is None:
            self._can_link = utils.test_link(self._layout_dir)
        else:
            self._can_link = use_links and utils.test_link(self._layout_dir)

    def clean(self):
        """
        Performs a clean on all copied DATAS.
        """
        scheduler = DataLayoutScheduler(self._data_mappings, self._layout_dir, self._cache_file)
        steps, _ = scheduler.generate(clean=True)

        state = DataLayoutState(self._can_link, self._verbose, self._layout_dir)

        failed = False
        for step in steps:
            if not step.perform(state):
                failed = True

        if failed:
            diags.warn("some files could not be cleaned from the layout")

        # delete the cache
        try:
            os.remove(self._cache_file)
        except PermissionError as e:
            diags.warn("failed to delete package cache file: {0)".format(e), file=self._cache_file)
        except OSError:
            pass

    def run(self):
        """
        Lay out the data by copying or linking it into a layout.
        """
        mkdir_p(self._layout_dir)

        scheduler = DataLayoutScheduler(self._data_mappings, self._layout_dir, self._cache_file,
                                        verbose=self._verbose, use_links=self._can_link)
        steps, cache = scheduler.generate(clean=False)

        state = DataLayoutState(self._can_link, self._verbose, self._layout_dir)

        failed = False
        for step in steps:
            if not step.perform(state):
                failed = True

        if failed:
            raise BuildException("some source files were missing from the layout")

        # write the cache
        try:
            mkdir_p(os.path.split(self._cache_file)[0])
            cache.save(self._verbose)

        except OSError as e:
            diags.error("failed to save package cache: " + str(e))

        return steps

