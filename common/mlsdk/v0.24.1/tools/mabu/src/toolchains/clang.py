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

import re

import os

import diags
import platforms
from build_vars import CC, CXX, AS, AR, RANLIB, CPP, OBJCOPY, STRIP
from defaults import DEVICE_DEFAULT
from diags import debug
from toolchains._base import Toolchain, version_compare
from toolchains._gcc_like import GCCLike


class CLang(GCCLike):
    # initialize to earliest supported, then
    # let detection update to latest incrementally
    VERSION_CLANG_LATEST = '3.8'

    # Apple clang builds follow the Xcode toolchain version, not the
    # underlying clang version, which makes it quite difficult to
    # engineer sane .option files.  Thus, for purposes of our target
    # specifications, we will convert the version number to the
    # corresponding clang version.
    #
    # See https://en.wikipedia.org/wiki/Xcode#10.x_series
    #
    # This is a sorted list which intentionally does not cover
    # every Apple-side version, but just records entries corresponding
    # to changes in the underlying LLVM version.
    APPLE_TO_LLVM_VERSION_MAP = [
        ((0, 0), (3, 3)),
        ((5, 0), (3, 3)),
        ((5, 1), (3, 4)),
        ((6, 0), (3, 5)),
        ((6, 3), (3, 6)),
        ((7, 0), (3, 7)),
        ((7, 3), (3, 8)),
        ((8, 0), (3, 9)),
        ((9, 0), (4, 0)),
        ((9, 3), (5, 0)),
        ((10, 0), (6, 0)),
        ((10, 1), (6, 0)),
        ((10, 2), (7, 0)),
        ((10, 3), (7, 0)),
        ((11, 0), (8, 0)),
        ((11, 1), (8, 0)),
        ((11, 2), (8, 0)),
    ]

    def __init__(self, name, version, driver, platform=None):
        GCCLike.__init__(self, name, version, driver=driver, platform=platform)

    def _add_importlib_option(self, type_args, proj):
        # clang doesn't properly pass through --out-implib -- just cross fingers it will be generated
        pass

    @classmethod
    def detect_canonical_clang_version(cls, text):
        """
        Parse the @text, find the stated version number, and map it to an upstream
        llvm release number.  This is done to ensure we can make .option files
        that have meaningful mappings for all hosts.

        # e.g. clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)
        # e.g. clang version 3.8.1 (branches/release_38)
        # e.g. clang version 5.0.1-svn319952-1~exp1 (branches/release_50)
        # and the outliers:
        # e.g. Apple LLVM version 8.0.0 (clang-800.0.42.1)
        # e.g. Apple LLVM version 10.0.0 (clang-1000.11.45.2)
        # e.g. Apple clang version 11.0.0 (clang-1100.0.33.8)
        # the version does not correspond to any actual upstream LLVM version :-p

        We yield the original version encoded as a "build" suffix so
        the original lineage isn't lost.
        :param text:
        :return: ((major, minor), suffix) or ((0, 0), "")
        """

        m = re.match(r'.*?\s+version\s+(\d+)\.(\d+)(\.\d+)?.*', text, re.S)
        if not m:
            return (0, 0), ''

        text_version = m.group(1) + '.' + m.group(2)
        major, minor = int(m.group(1)), int(m.group(2))
        debug("CLang: matched {}.{}".format(major, minor))

        if "Apple LLVM" in text or "Apple clang" in text:
            # map Apple clang versions from Xcode to clang upstream
            llvm_version = None
            for (amaj, amin), llvm in cls.APPLE_TO_LLVM_VERSION_MAP:
                if amaj == major and minor >= amin:
                    # keep taking newest
                    llvm_version = llvm

            if llvm_version:
                debug("CLang: converted version to canonical {}.{}".format(llvm_version[0], llvm_version[1]))
                return llvm_version, ".apple" + text_version

            diags.warn("The version of Xcode on this system ({}.{}) is too new for mabu, guessing version".
                       format(major, minor))
            return cls.APPLE_TO_LLVM_VERSION_MAP[-1][1], ".apple" + text_version

        # actual clang
        return (major, minor), ''

    @classmethod
    def can_user_override(cls):
        return cls._may_target_toolchain('clang')

    @classmethod
    def discover_toolchain(cls, driver_or_path, platform=None, preferred=False, user_override=False):
        debug("CLang: looking for {}".format(driver_or_path))

        path = cls.find_driver_path(driver_or_path)
        if not path:
            return None

        # user overrides need to abort
        reporter = user_override and diags.error or diags.debug

        # run to verify sanity and check its version
        try:
            comp = cls.invoke_tool_or_script([path, '--version'])

            # ignore stray intl chars
            comp.stdout = str(comp.stdout, encoding='ascii', errors='ignore')
            comp.stderr = str(comp.stderr, encoding='ascii', errors='ignore')

        except OSError as e:
            # e.g. PermissionError, from trying to run Cygwin softlink
            reporter("CLang: failed to invoke '{}', error '{}'".format(path, e))
            return None

        if comp.returncode != 0:
            reporter("CLang: failed to run '{} --version': {}".format(path, comp.stderr))
            return None

        if "GCC" in comp.stdout and not ('clang' in comp.stdout or 'LLVM' in comp.stdout):
            reporter("CLang: this looks like GCC ('{}'):\n{}".format(path, comp.stdout))
            return None

        arch = GCCLike.get_target_arch(platform)

        # it's clang but might not be for this platform
        if platform == platforms.lumin.NAME:
            # we always see this in ports
            # e.g.
            if arch not in comp.stdout:
                exe = os.path.split(path)[1]
                if arch in exe:
                    reporter("CLang: the tool '{}' does not target this platform".format(path))
                else:
                    reporter("CLang: the tool '{}' does not target this platform "
                             "(use e.g. '{}-{}' instead)".format(path, GCCLike.get_target_triple(platform), exe))
                return None
        else:
            # _not_ device -- and we might not see the architecture at all in a host tool
            device_arch = GCCLike.get_target_arch(platforms.lumin.NAME)
            if device_arch in comp.stdout:
                reporter("CLang: the tool '{}' does not target this platform".format(path))
                return None

        (major, minor), suffix = cls.detect_canonical_clang_version(comp.stdout)
        if major == 0:
            reporter("CLang: did not find version information in output of {}:\n{}".format(
                path, comp.stdout + comp.stderr))
            return None

        major_version = str(major)
        major_minor_version = str(major) + '.' + str(minor)
        full_version = major_minor_version + suffix

        newer = False
        # FIXME: handle device-targeting versions independently
        if platform != platforms.lumin.NAME and version_compare(full_version, cls.VERSION_CLANG_LATEST) > 0:
            cls.VERSION_CLANG_LATEST = full_version
            newer = True

        # update aliases
        full_name = 'clang-' + major_minor_version + suffix

        major_minor_name = 'clang-' + major_minor_version
        if full_name != major_minor_name:
            if newer or preferred or major_minor_name not in Toolchain.all(platform):
                # map e.g. 'clang-5.1' to 'clang-5.1.apple9.0.0'
                Toolchain.register_alias(major_minor_name, full_name, platform)

        major_name = 'clang-' + major_version
        if newer or preferred or major_name not in Toolchain.all(platform):
            # map e.g. 'clang-5' to this one
            Toolchain.register_alias(major_name, full_name, platform)

        if newer or preferred:
            # map e.g. 'clang' to this one
            Toolchain.register_alias('clang', full_name, platform)

        # register the toolchain
        tc = CLang(full_name, major_minor_version + suffix, path, platform)
        Toolchain.register(tc, force=True)

        return tc

    @classmethod
    def discover(cls):
        """
        Try to locate, detect, and register clang toolchains for the host living on the PATH.
        """
        temp_entry = None
        if not Toolchain.find('clang'):
            # First, register a fake entry for better error reporting, in case nothing
            # is found below (to get an error "no clang-3.8 on PATH" rather than
            # "unknown build target segment").
            name_and_driver = 'clang-' + CLang.VERSION_CLANG_LATEST
            tc = CLang(name_and_driver, CLang.VERSION_CLANG_LATEST, name_and_driver)
            Toolchain.register(tc, force=True)
            temp_entry = tc
            Toolchain.register_alias('clang', name_and_driver)

        pre_scan = Toolchain.all()

        # Look for generic version, preferring it so it overwrites the aliases if found
        cls.discover_toolchain('clang', preferred=temp_entry is not None)

        # look for other versioned executables
        for tool in cls.find_tools_matching(r'^clang-(\d+)(\.\d+)?$'):
            cls.discover_toolchain(tool)

        # remove the placeholder if we discovered real stuff
        if temp_entry and Toolchain.all() != pre_scan:
            Toolchain.remove(temp_entry)

        # Lumin OS target (no detection)
        Toolchain.register(CLang('clang-3.8', '3.8', 'clang', platform=DEVICE_DEFAULT), force=True)
        Toolchain.register_alias('clang', 'clang-3.8', DEVICE_DEFAULT)

        # new experimental toolchain
        Toolchain.register(CLang('clang-8.0', '8.0', 'clang-8', platform=DEVICE_DEFAULT))
        Toolchain.register_alias('clang-8', 'clang-8.0', DEVICE_DEFAULT)

    def detect(self):
        GCCLike.detect(self)

        # primary executables based on `driver`
        driver_file = os.path.split(self._driver)[1]

        # look for sibling executables based on the driver name;
        # try to ignore prefixes at the start or versions stuck on the end
        # by replacing what-looks-like clang (or gcc, if on macOS and user passed CC=gcc)
        # with the expected sibling tool
        base = 'clang' if 'clang' in driver_file \
            else 'gcc' if 'gcc' in driver_file \
            else 'g++' if 'g++' in driver_file \
            else os.path.splitext(driver_file)[0]

        if base == 'gcc' or base == 'g++':
            c_tool = driver_file.replace('g++', 'gcc')
            cpp_tool = driver_file.replace('gcc', 'g++')
        else:
            c_tool = driver_file.replace('++', '')
            cpp_tool = driver_file.replace(base, base + '++')

        if self._platform != platforms.lumin.NAME:
            # resolve full path if needed, and/or
            # clean up the driver name based on a sole CXX override
            err = self.register_tool_based_on_driver(
                CC, [c_tool])
            if err:
                return err

            err = self.register_tool_based_on_driver(
                CXX, [cpp_tool],
                self._driver)
            if err:
                return err

            # look for standalone preprocessor next to compiler
            err = self.register_tool_based_on_driver(
                CPP, [driver_file.replace(base, 'cpp'), 'cpp'])
            if err:
                return err

            # find assembler:
            # -- don't use llvm-as (that's for bitcode, not .o files)
            # -- no need to be fussy about assembler version matching clang[++] version
            # -- some versions of clang act as assemblers on their own, so clang itself is a valid match
            # (not a "fallback", though, to avoid warning message for the rare use of assembly)
            err = self.register_tool_based_on_driver(AS, ['as', self._tools[CC]])
            if err:
                return err

            # find archiver; don't use llvm-ar (for bitcode, not .o files)
            err = self.register_tool_based_on_driver(
                AR, [driver_file.replace(base, 'ar'), 'ar'])
            if err:
                return err
        else:
            rel_path = os.path.join('..', 'llvm-8', 'bin') if '8.0' == self.version else '.'

            cross_name_c_tool = None
            cross_name_cpp_tool = None

            if rel_path != '.':
                cross_name_c_tool = self.get_target_triple(self._platform) + '-clang'
                cross_name_cpp_tool = self.get_target_triple(self._platform) + '-clang++'

            err = self.add_cross_toolchain_tool(CPP, 'cpp', rel_path=rel_path)
            if err:
                return err
            err = self.add_cross_toolchain_tool(CC, c_tool, rel_path=rel_path, rename_tool=cross_name_c_tool)
            if err:
                return err
            err = self.add_cross_toolchain_tool(CXX, cpp_tool, rel_path=rel_path, rename_tool=cross_name_cpp_tool)
            if err:
                return err
            err = self.add_cross_toolchain_tool(AS, 'as', rel_path=rel_path)
            if err:
                return err
            err = self.add_cross_toolchain_tool(AR, 'gcc-ar', rel_path=rel_path)
            if err:
                return err
            err = self.add_cross_toolchain_tool(RANLIB, 'ranlib', rel_path=rel_path)
            if err:
                return err
            err = self.add_cross_toolchain_tool(OBJCOPY, 'objcopy', rel_path=rel_path)
            if err:
                return err
            err = self.add_cross_toolchain_tool(STRIP, 'strip', rel_path=rel_path)
            if err:
                return err

        return None

def discover():
    Toolchain.register_toolchain_class(CLang)
    CLang.discover()

