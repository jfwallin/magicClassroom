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

import sys

# initialize as early as possible
DEBUG='--debug' in sys.argv


def _format(s, type=None, file=None, line=None):
    from config import NAME
    if file and line:
        fmt = "{FILE}:{LINE}: "
    elif file:
        fmt = "{FILE}: "
    else:
        fmt = ""
    if type:
        fmt = "{TYPE}: " + fmt
    fmt = NAME + ": " + fmt

    msg = fmt.format(FILE=file, LINE=line, TYPE=type) + s
    return msg


def error(s, file=None, line=None, **kwargs):
    """
    Print fatal error and die.
    """

    die = True
    if 'die' in kwargs:
        die = kwargs['die']
        del kwargs['die']

    print(_format(s, 'error', file, line), file=sys.stderr)
    if die:
        sys.exit(1)


def warn(s, file=None, line=None, *args, **kwargs):
    """
    Print warning.
    """
    # see error() above for more.

    if 'die' in kwargs:
        # ignore
        del kwargs['die']

    print(_format(s, 'warning', file, line), file=sys.stderr, *args, **kwargs)


def info(s, file=None, line=None, is_stderr=False, *args, **kwargs):
    """
    Print info.
    """
    print(_format(s, None, file, line), *args, **kwargs, file=is_stderr and sys.stderr or sys.stdout)


def noop(*args, **kwargs):
    """
    Do nothing.
    """
    pass


def set_debug(val):
    global DEBUG
    DEBUG = val


def is_debug():
    global DEBUG
    return DEBUG


def debug(s, file=None, line=None, *args, **kwargs):
    """
    Print debug info, if mabu.DEBUG.
    """
    if DEBUG: print(_format(s, None, file, line), *args, **kwargs)
