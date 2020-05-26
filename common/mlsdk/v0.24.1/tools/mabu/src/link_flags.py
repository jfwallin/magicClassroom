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
import diags
from project_vars import LINKFLAGS
from utils import as_boolean


class LinkFlags:
    def __init__(self):
        self.ignore_unused_shlibs = None

    def set_from_project(self, project, silent=False):
        def warn(msg):
            if not silent:
                diags.warn(msg)

        flag_handlers = FlagHandlers(self, warn)

        for opt in list(project.combined[LINKFLAGS]):
            equ = opt.find('=')
            if equ <= 0:
                warn("Invalid link flag '{0}', ignoring".format(opt))
                continue
            name, value = opt[0:equ].strip(), opt[equ + 1:].strip()

            if hasattr(flag_handlers, name):
                getattr(flag_handlers, name)(name, opt, value)
            else:
                diags.warn("Unsupported flag in LINKFLAGS: '{0}', ignoring".format(name))


class FlagHandlers(object):
    def __init__(self, flags, warn):
        self.flags = flags
        self.warn = warn

    @staticmethod
    def define_bool(optname, fieldname):
        def handle(self, name, opt, value):
            f = as_boolean(value, None)
            if f is None:
                self.warn("Expected boolean value in LINKFLAGS: '{0}={1}', "
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
                self.warn("Expected integer value in LINKFLAGS: '{0}={1}', "
                          "ignoring".format(name, opt))

        setattr(FlagHandlers, optname, handle)

    @staticmethod
    def define_str(optname, fieldname):
        def handle(self, _, _2, value):
            setattr(self.flags, fieldname, value)

        setattr(FlagHandlers, optname, handle)


FlagHandlers.define_bool('ignoreUnusedShlibs', 'ignore_unused_shlibs')
