# %BANNER_BEGIN%
# ---------------------------------------------------------------------
# %COPYRIGHT_BEGIN%
#
# Copyright (c) 2019 Magic Leap, Inc. (COMPANY) All Rights Reserved.
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
if DEBUG:
    sys.argv.remove('--debug')


def _format(s, kind, *args):
    from config import NAME
    if kind:
        fmt = kind + ": "
    else:
        fmt = ''

    fmt = NAME + ": " + fmt + s
    msg = fmt.format(*args)
    return msg


def error(s, *args, **kwargs):
    """
    Print fatal error and die.
    """

    die = True
    if 'die' in kwargs:
        die = kwargs['die']
        del kwargs['die']

    print(_format(s, 'error', *args), file=sys.stderr)
    if die:
        sys.exit(9)


def warn(s, *args, **kwargs):
    """
    Print warning.
    """
    # see error() above for more.
    print(_format(s, 'warning', *args), file=sys.stderr, **kwargs)


def info(s, *args, is_stderr=False, **kwargs):
    """
    Print info.
    """
    print(_format(s, None, *args), **kwargs, file=sys.stderr)


def set_debug(val):
    global DEBUG
    DEBUG = val


def is_debug():
    global DEBUG
    return DEBUG


def debug(s, *args, **kwargs):
    if DEBUG: print(_format(s, None, *args), file=sys.stderr, **kwargs)

