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

from platforms._base import Platform


NAME='linux'

class Handler(Platform):
    def __init__(self, name):
        Platform.__init__(self, name)
        from config import HOSTOS
        from config import OS_LINUX
        self._is_host = HOSTOS == OS_LINUX

    def format_program_file(self, name):
        """ Tell what the filename is for a program. """
        return name

    def format_shared_file(self, name):
        """ Tell what the filename is for a shared library. """
        return "lib" + name + ".so"

    def format_static_file(self, name):
        """ Tell what the filename is for a static library. """
        return "lib" + name + ".a"

    def is_buildable(self):
        # no cross tools
        from config import HOSTOS, OS_LINUX
        return HOSTOS == OS_LINUX
