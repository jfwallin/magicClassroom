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

from abc import abstractmethod, ABCMeta

from build_model import BuildObject
from config import HOSTOS


class Platform(BuildObject):
    __metaclass__ = ABCMeta

    """
    A platform is the operating system for which code is generated.
    """

    @classmethod
    def _default_platform(cls, platform):
        return HOSTOS

    def __init__(self, name):
        BuildObject.__init__(self, name)
        self._is_host = False

    def is_host(self):
        return self._is_host

    def set_host(self, host):
        self._is_host = host

    @property
    def path_separator(self):
        return '/'

    @abstractmethod
    def format_program_file(self, name):
        """ Tell what the filename is for a program. """
        raise NotImplementedError()

    @abstractmethod
    def format_shared_file(self, name):
        """ Tell what the filename is for a shared library. """
        raise NotImplementedError()

    @abstractmethod
    def format_static_file(self, name):
        """ Tell what the filename is for a static library. """
        return NotImplementedError()

    def format_importlib_file(self, name):
        """ Tell what the filename is for importing a shared library.

        The default is to reference the shared library itself.
        """
        return self.format_shared_file(name)

    def format_object_file(self, name):
        """ Tell what the filename is for object code for this source file. """
        return name + ".o"

    def uses_packages(self):
        """ Tell if the platform uses package files """
        return False

    def is_buildable(self):
        """
        Tell if projects targeting the receiver can build on the current host.
        """
        raise NotImplementedError()


Platform.init_class()