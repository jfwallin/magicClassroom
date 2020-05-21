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

import build_env
import defaults
import diags
import platforms
from build_model import Composition, Literal, Path
from build_vars import CC, CXX, CPP, AS, AR, RANLIB, STRIP, OBJCOPY
from defaults import DEVICE_DEFAULT
from diags import debug
from run_vars import runtime_values, SPEC
from toolchains._base import Toolchain, version_compare
from toolchains._gcc_like import GCCLike


class GCC(GCCLike):
    # initialize to earliest supported, then
    # let detection update to latest incrementally
    VERSION_GCC_LATEST = '4.8'

    def __init__(self, name, version, driver, platform=None):
        GCCLike.__init__(self, name, version, driver=driver, platform=platform)

    def _add_importlib_option(self, type_args, proj):
        type_args.append(Composition(
                Literal('-Wl,--out-implib,'),
                Path(proj.outdir_path(),
                     build_env.platform().format_importlib_file(proj.output_name))))

    @classmethod
    def can_user_override(cls):
        return cls._may_target_toolchain('gcc')

    @classmethod
    def discover_toolchain(cls, driver_or_path, platform=None, arch=None, preferred=False, user_override=False):
        debug("GCC: looking for {}".format(driver_or_path))

        path = cls.find_driver_path(driver_or_path)
        if not path:
            return None

        # user overrides need to abort
        reporter = user_override and diags.error or diags.debug

        # run to verify sanity and check its version
        try:
            comp = cls.invoke_tool_or_script([path, '-v'])

            # ignore stray intl chars
            comp.stdout = str(comp.stdout, encoding='ascii', errors='ignore')
            comp.stderr = str(comp.stderr, encoding='ascii', errors='ignore')
        except OSError as e:
            # e.g. PermissionError, from trying to run Cygwin softlink
            reporter("GCC: failed to invoke '{}', error '{}'".format(path, e))
            return None

        if comp.returncode != 0:
            reporter("GCC: failed to run '{} -v': {}".format(path, comp.stderr))
            return None

        output = comp.stdout + comp.stderr

        if "Apple LLVM" in output or 'clang' in output:
            # ah, an impostor; let clang detection figure it out
            debug("GCC: the tool '{}' is actually clang".format(path))
            return None

        # e.g. gcc version 4.8.5 (Ubuntu 4.8.5-4ubuntu1~14.04.2)
        # e.g. gcc version 8.1.0 (Ubuntu 8.1.0-5ubuntu1~14.04)

        # e.g. gcc version 4.9.x 20150123 (prerelease) (GCC)
        m = re.match(r'.*?version\s+(\d+)(\.\d+)(\.x|\.\d+).*', output, re.S)
        if not m:
            reporter("GCC: did not find version information in output of {}:\n{}".format(
                path, output))
            return None

        # it's GCC but might not be for this platform
        arch = GCCLike.get_target_arch(platform)

        if platform == platforms.lumin.NAME:
            # we always see this in ports
            if arch not in output:
                exe = os.path.split(path)[1]
                if arch in exe:
                    reporter("GCC: the tool '{}' does not target this platform".format(path))
                else:
                    reporter("GCC: the tool '{}' does not target this platform "
                             "(use e.g. '{}-{}' instead)".format(path, GCCLike.get_target_triple(platform), exe))
                return None
        else:
            # _not_ device -- and we might not see the architecture at all in a host tool
            device_arch = GCCLike.get_target_arch(platforms.lumin.NAME)
            if device_arch in output:
                reporter("GCC: the tool '{}' does not target this platform".format(path))
                return None

        major_minor_version = m.group(1) + m.group(2)
        major_version = m.group(1)
        debug("GCC: matched {}".format(major_minor_version))

        newer = False
        # FIXME: handle device-targeting versions independently
        if platform != platforms.lumin.NAME and version_compare(major_minor_version, cls.VERSION_GCC_LATEST) > 0:
            cls.VERSION_GCC_LATEST = major_minor_version
            newer = True

        full_name = 'gcc-' + major_minor_version

        major_name = 'gcc-' + major_version
        if newer or preferred or major_name not in Toolchain.all(platform):
            # map e.g. 'gcc-5' to this one
            Toolchain.register_alias(major_name, full_name, platform)

        if newer or preferred:
            # map e.g. 'gcc' to this one
            Toolchain.register_alias('gcc', full_name, platform)

        # register the toolchain
        tc = GCC('gcc-' + major_minor_version, major_minor_version, path, platform)
        Toolchain.register(tc, force=True)

        return tc

    @classmethod
    def discover(cls):
        """
        Try to locate and register GCC toolchains for the host living on the PATH.
        """

        def is_valid_gcc_match(tool):
            if 'gcc' in (runtime_values.get(SPEC) or ''):
                # yes, the user is explicitly targeting what they call 'gcc'
                return True

            if 'gcc' in tool.lower() or 'g++' in tool.lower():
                # yes, it's named after what we consider gcc
                return True

            return False

        temp_entry = None
        if not Toolchain.find('gcc'):
            # First, register a fake entry for better error reporting, in case nothing
            # is found below (to get an error "no gcc-4.8 on PATH" rather than
            # "unknown build target segment").
            name_and_driver = 'gcc-' + GCC.VERSION_GCC_LATEST
            tc = GCC(name_and_driver, GCC.VERSION_GCC_LATEST, name_and_driver)
            Toolchain.register(tc, force=True)
            temp_entry = tc
            Toolchain.register_alias('gcc', name_and_driver)

        pre_scan = Toolchain.all()

        # Look for generic version, preferring it so it overwrites the aliases if found
        cls.discover_toolchain('gcc', preferred=temp_entry is not None)

        # look for other versioned executables
        for tool in cls.find_tools_matching(r'^gcc-(\d+)(\.\d+)?'):
            cls.discover_toolchain(tool)

        # remove the placeholder if we discovered real stuff
        if temp_entry and Toolchain.all() != pre_scan:
            Toolchain.remove(temp_entry)

        # register known SDK tools for Lumin OS targeting
        Toolchain.register(GCC('gcc-4.9', '4.9', 'aarch64-linux-android-gcc-4.9', platform=DEVICE_DEFAULT), force=True)
        Toolchain.register_alias('gcc', 'gcc-4.9', DEVICE_DEFAULT)

    def detect(self):
        GCCLike.detect(self)

        if self._platform != platforms.lumin.NAME:
            # resolve full path if needed, and/or
            # clean up the driver name based on a sole CXX override
            err = self.register_tool_based_on_driver(
                CC, [self._driver.replace('g++', 'gcc')])
            if err:
                return err

            # primary executables based on `driver`
            driver_file = os.path.split(self._driver)[1]

            # look for sibling executables based on the driver name;
            # try to ignore prefixes at the start or versions stuck on the end
            # by replacing what-looks-like gcc with the expected sibling tool
            base = 'gcc' if 'gcc' in driver_file \
                else 'g++' if 'g++' in driver_file \
                else os.path.splitext(driver_file)[0]

            err = self.register_tool_based_on_driver(
                CXX, [driver_file.replace(base, 'g++'), driver_file.replace(base, base + '++')],
                self._driver)
            if err:
                return err

            # look for standalone preprocessor
            err = self.register_tool_based_on_driver(
                CPP, [driver_file.replace(base, 'cpp'), 'cpp'])
            if err:
                return err

            # look for standalone assembler
            err = self.register_tool_based_on_driver(
                AS, [driver_file.replace(base, 'as'), 'as'])
            if err:
                return err

            # look for archiver
            err = self.register_tool_based_on_driver(
                AR, [driver_file.replace(base, 'ar'), driver_file.replace(base, 'gcc-ar'), 'gcc-ar', 'ar'])
            if err:
                return err

        else:
            err = self.add_cross_toolchain_tool(CC, 'gcc')
            if err:
                return err
            err = self.add_cross_toolchain_tool(CXX, 'g++')
            if err:
                return err
            err = self.add_cross_toolchain_tool(CPP, 'cpp')
            if err:
                return err
            err = self.add_cross_toolchain_tool(AS, 'as')
            if err:
                return err
            err = self.add_cross_toolchain_tool(AR, 'gcc-ar')
            if err:
                return err
            err = self.add_cross_toolchain_tool(RANLIB, 'ranlib')
            if err:
                return err
            err = self.add_cross_toolchain_tool(STRIP, 'strip')
            if err:
                return err
            err = self.add_cross_toolchain_tool(OBJCOPY, 'objcopy')
            if err:
                return err

        return None


def discover():
    Toolchain.register_toolchain_class(GCC)
    GCC.discover()
