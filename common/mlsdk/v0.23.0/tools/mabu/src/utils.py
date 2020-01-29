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

import glob
import hashlib
import inspect
import os
import shutil
import sys
import re

import zlib


def mkdir_p(path):
    try:
        path = host_path(path)
        if not path or os.path.isdir(path):
            return
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        import errno
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def get_actual_filepath(name):
    """
    Fix capitalization of a full path -- os.path.canonpath doesn't do it
    :param name:
    :return:
    """
    if not ':' in name and not '\\' in name:
        return name
    dirs = name.split('\\')
    # disk letter
    test_name = [dirs[0].upper()]
    for d in dirs[1:]:
        test_name += ["%s[%s]" % (d[:-1], d[-1])]
    res = glob.glob('\\'.join(test_name))
    if not res:
        #File not found
        return None
    return res[0]


def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate


@static_vars(scriptDirs={})
def get_script_dir(object):
    """
    Find the directory where the script is running which defines @object
    From http://stackoverflow.com/questions/3718657/how-to-properly-determine-current-script-directory-in-python/22881871#22881871
    :return: path
    """
    path = get_script_dir.scriptDirs.get(object, None)
    if not path:
        if getattr(sys, 'frozen', False): # py2exe, PyInstaller, cx_Freeze
            path = os.path.abspath(sys.executable)
        else:
            path = inspect.getabsfile(object)
            path = os.path.realpath(path)

        # Windows needs real case
        path = get_actual_filepath(path)

        path = os.path.dirname(path)
        get_script_dir.scriptDirs[object] = path

    return path


def as_boolean(s, if_empty=False):
    """
    Convert a string to a boolean, where '0' or 'f' or 'false' are false,
    '1' or 't' or 'true' are true, and if_empty is used for the empty string
    :param s: string
    :type s: str
    :param if_empty: default value if 's' is blank
    :return:
    """
    if not s:
        return if_empty
    if s.lower() in ['0', 'f', 'false']:
        return False
    if s.lower() in ['1', 't', 'true']:
        return True
    raise Exception("expected 0, 1, t, true, f, or false for boolean value, got: '{0}'".format(s))


def which(program):
    """
    Find a program on the path
    :param program: base executable name
    :return: resolved path, or None
    """
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)

    from config import HOSTOS
    from config import OS_WIN
    if HOSTOS == OS_WIN:
        ext = os.path.splitext(fname)[1]
        if not ext:
            fname += ".exe"
            program += ".exe"

    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None


def unix_path(path):
    """
    All paths are Unix-ized internally, since make (even in MinGW)
    expect this format.
    """
    from config import HOSTOS, OS_WIN
    if HOSTOS == OS_WIN:
        if len(path) > 3 and path[0].isalpha() and path[1] == ':' and path[2] in ['/', '\\']:
            #path = '/' + path[0] + '/' + path[2:].replace('\\', '/')
            path = path.replace('\\', '/')

    return path.replace('//', '/')


def host_path(path):
    """
    All paths are Unix-ized internally, but for some purposes
    in Windows we need the drive-lettered path.
    """
    from config import HOSTOS, OS_WIN
    if HOSTOS == OS_WIN:
        if len(path) > 3 and path[0] == '/' and path[1].isalpha() and path[2] == '/':
            return (path[1] + ':' + path[2:]).replace('/', '\\')

    return path


def get_make_env_block():
    """
    Return an environment block for invoking 'make'.
    On Windows, we have a private directory where some MinGW
    tools reside, so those need to be on PATH.
    :rtype: Dict[str,str]
    """
    env = dict(os.environ)
    from config import HOSTOS, OS_WIN, MINGW_DIR, MSYS_DIR
    if HOSTOS == OS_WIN:
        # augment path for Windows
        env['PATH'] = host_path(MINGW_DIR) + os.pathsep + host_path(MSYS_DIR) + os.pathsep + os.getenv('PATH')
        # avoid very slow behavior with MinGW and Active Directory
        # (https://stackoverflow.com/questions/929495/why-is-mingw-very-slow)
        env['LOGONSERVER'] = r'\\LOCALHOST'
    return env


def user_relpath(path, start=os.getcwd()):
    """
    Like os.path.relpath, but meant for user presentation (thus,
    resolves to the original path if the relative one is too long).
    :param path: incoming path
    :param start: base of relative path to construct
    :return: relative path or original path
    """
    try:
        rel = os.path.relpath(path, start)
        if len(rel) >= len(path):
            # too far away
            return path
        return rel
    except Exception:
        # In Windows, paths on different drives cause this
        return path


def uniquify(lst, idfun=None):
    """
    Remove duplicates from @lst (in place)
    :param lst:
    :param idfun: function to determine identity (if not the literal value)
    :return: None
    """
    # from https://www.peterbe.com/plog/uniqifiers-benchmark
    # order preserving
    if idfun is None:
        def idfun(x): return x
    seen = {}
    result = []
    for item in lst:
        marker = idfun(item)
        # in old Python versions:
        # if seen.has_key(marker)
        # but in new ones:
        if marker in seen: continue
        seen[marker] = 1
        result.append(item)

    lst.clear()
    lst += result


def touch(fname, times=None):
    """ Change the timestamps of the file at 'fname'
    :param fname: path to file
    :param times: if set, new times, as per os.utime()
    """
    with open(fname, 'a'):
        os.utime(fname, times)


def copy_safely(src, dest):
    """
    Copy 'src' to 'dest' via a temporary file, to avoid
    conflicts with parallel processes that may be also
    copying the file
    :param src:
    :param dest:
    """
    # copy to a temporary file, to avoid
    # parallel packagers modifying the same file
    temp_loc = dest + '.tmp$' + str(os.getpid())

    # be sure we can overwrite any existing file...
    try:
        os.chmod(temp_loc, 0o666)       # in case it already exists
    except OSError:
        pass

    shutil.copy2(src, temp_loc)

    # and don't preserve read-only-ness in destination
    os.chmod(temp_loc, 0o666)

    try:
        rename_with_retry(temp_loc, dest)
    except OSError as e:
        try:
            # print("cleaning up", src)
            os.remove(temp_loc)
        except FileNotFoundError:
            pass
        except OSError:
            # couldn't even delete the temporary file??
            raise

        # report the problem
        raise e


def rename_with_retry(src, dest):
    """
    Rename 'src' to 'dest', allowing for other parallel
    processes that may be attempting the same.
    :param src:
    :param dest:
    """

    if src == dest:
        return

    # now, try to get the destination replaced
    # atomically
    last_error = None
    tries = 0
    while tries < 8:
        try:
            os.replace(src, dest)
            # good, got it done
            return
        except PermissionError as e:
            # we only get here on Windows
            # (in POSIX, os.rename deletes an existing dest)
            last_error = e
            try:
                # be sure we can delete the target
                os.chmod(dest, 0o666)
                os.remove(dest)
                # good, can try renaming again now
            except FileNotFoundError:
                # hmm, nothing was there...
                # maybe we deleted it in a parallel
                # packaging step...?
                tries = 0
            except OSError as e:
                # hmm, something worse happened
                raise
        except OSError as e:
            # hmm, something worse happened
            raise

        tries += 1

    # ran out of tries
    raise last_error


def format_command_line(args):
    def safe(s):
        # don't convert lines like 'MYVAR="something already painstakingly quoted"'
        if ' ' in s and '="' not in s:
            s = '"' + s + '"'
        return s

    msg = " ".join([safe(arg) for arg in args if arg])
    return msg


def remove_duplicates(seq, key=lambda x: x):
    seen = set()
    return [x for x in seq if not (key(x) in seen or seen.add(key(x)))]


def test_symlink(base):
    """
    See if we can create soft/symlinks in `base`
    :param base:  path
    :return:  bool
    """
    try:
        testlink = os.path.join(base, ".testsymlink")
        testsrc = os.path.join(base, ".testsrc")

        # make a test file
        with open(testsrc, "w") as f:
            pass

        # remove link if existed
        try:
            os.unlink(testlink)
        except OSError:
            pass

        # try to make a link
        os.symlink(testsrc, testlink)

        # clean up
        try:
            os.unlink(testlink)
            os.unlink(testsrc)
        except OSError:
            pass

        return True

    except NotImplementedError:
        # os.symlink can raise this
        return False
    except OSError:
        # some other error
        return False


def test_link(base):
    """
    See if we can create (hard)links in `base`.
    :param base:  directory
    :return:  bool
    """
    try:
        testlink = os.path.join(base, ".testlink")
        testsrc = os.path.join(base, ".testsrc")

        # make a test file
        with open(testsrc, "w") as f:
            pass

        # remove link if existed
        try:
            os.unlink(testlink)
        except OSError:
            pass

        # try to make a link
        os.link(testsrc, testlink)

        # clean up
        try:
            os.unlink(testlink)
            os.unlink(testsrc)
        except OSError:
            pass

        return True

    except NotImplementedError:
        # os.symlink can raise this
        return False
    except OSError:
        # some other error
        return False


def path_to_filename(path):
    """
    Convert a :path: into a filename, without blowing up the Windows path
    length, by combining the base filename with the sha1 of the full path,
    e.g. c:/path/to/baz.x --> baz.x-49347348934890

    We shorten the 40-char SHA1 digest to 16 chars, which should be sufficient
    for distinguishing directories (e.g. git uses sha1 and abbreviates
    to 8 chars).  This prevents us from eating 41 chars of path, only
    taking 17.
    :type path: str
    :rtype: str
    """
    dir, filename = os.path.split(path)
    digest = hashlib.sha1(os.path.abspath(dir).encode('utf-8')).hexdigest()
    return filename + '-' + digest[0:16]


def hash_file_contents(path):
    """
    Read the contents of the file at :path: and return a hash of the
    contents.  This is intended only as a rough guess of whether a file
    has changed and is not used for security purposes.
    :param path: filepath
    :return: hash of contents as hex string
    """
    try:
        # use SHA-1 to hash the crc32's of 64k chunks
        # (hashing the entire contents of a large file is much slower)
        sha1 = hashlib.sha1()
        with open(path, 'rb') as f:
            while True:
                chunk = f.read(65536)
                if not chunk:
                    break
                crc32 = zlib.crc32(chunk)
                sha1.update(bytes(hex(crc32), 'ascii'))
        return sha1.hexdigest()
    except OSError:
        return None


def hash_buffer_contents(buffer):
    """
    Hash the contents of the buffer.  This is intended only as a rough guess of whether a file
    has changed and is not used for security purposes.
    :param buffer: contents to hash
    :return: hash of contents as hex string
    """
    sha1 = hashlib.sha1()
    crc32 = zlib.crc32(buffer)
    sha1.update(bytes(hex(crc32), 'ascii'))
    return sha1.hexdigest()


class PathCache(object):
    """
    Read and cache contents found on the PATH
    """
    @staticmethod
    def instance():
        if not hasattr(PathCache, '_instance'):
            PathCache._instance = PathCache()
        return PathCache._instance

    def __init__(self):
        # ordered list of (directory, set(file))
        self._dir_to_entries = []

        from config import HOSTOS, OS_WIN, OS_OSX, MINGW_DIR, MSYS_DIR

        path = os.environ.get("PATH", os.defpath).split(os.pathsep)

        if HOSTOS == OS_WIN:
            # these entries will be visible at runtime
            path.insert(0, os.path.abspath(MINGW_DIR))
            path.insert(0, os.path.abspath(MSYS_DIR))

        visited = set()
        for directory in path:
            try:
                canon_dir = os.path.realpath(directory)
                if canon_dir not in visited:
                    visited.add(canon_dir)

                    ents = os.listdir(canon_dir)

                    if HOSTOS == OS_WIN or HOSTOS == OS_OSX:
                        self._dir_to_entries.append((canon_dir, set([ent.lower() for ent in ents])))
                    else:
                        self._dir_to_entries.append((canon_dir, set(ents)))
            except OSError:
                # ignore
                pass

    @staticmethod
    def _extensionize_file(file):
        files = [file]

        from config import HOSTOS
        from config import OS_WIN
        if HOSTOS == OS_WIN:
            # mirror logic in shutil.which(), to use PATHEXT for
            # files not already using a known extension
            pathext = os.environ.get("PATHEXT", ".EXE").split(os.pathsep)
            if any(file.lower().endswith(ext.lower()) for ext in pathext):
                files = [file]
            else:
                # make variant for each possibility, and add "no extension" as a fallback for scripts
                files = [file + ext.lower() for ext in pathext] + [file]

        return files

    def which(self, file):
        # if user passed a full path, match it against the cache first
        exp_dir, file = os.path.split(file)
        if exp_dir:
            exp_dir = os.path.realpath(exp_dir)

        files = self._extensionize_file(file)

        # do fast search through cache
        scanned_exp_dir = False
        for direc, ents in self._dir_to_entries:
            if not exp_dir or exp_dir == direc:
                scanned_exp_dir |= (exp_dir == direc)
                for file in files:
                    if file in ents:
                        return os.path.join(direc, file)

        # if a full path came in, not from the path, search it now
        if exp_dir and not scanned_exp_dir:
            for file in files:
                path = os.path.join(exp_dir, file)
                if os.path.exists(path) and os.access(path, os.F_OK | os.X_OK):
                    return path

        return None

    def which_match(self, regex, all_filename_matches=False):
        """
        Return a list of paths in the cache that match the given regex.
        :param pattern: string regex for a filename matcher.  (Extensions
        for Windows are handled as in `which`)
        :param all_filename_matches: if true, continue searching PATH even
        if a normal invocation would stop at the first match
        :return: list of paths
        """
        seen = set()
        matches = []

        templates = self._extensionize_file("__base__")
        file_patterns = ["(" + re.escape(template).replace("__base__", regex) + ")" for template in templates]
        file_pattern = re.compile("|".join(file_patterns))

        for direc, ents in self._dir_to_entries:
            for ent in ents:
                if file_pattern.match(ent) and not ent in seen:
                    matches.append(os.path.join(direc, ent))
                    if not all_filename_matches:
                        seen.add(ent)

        return matches
