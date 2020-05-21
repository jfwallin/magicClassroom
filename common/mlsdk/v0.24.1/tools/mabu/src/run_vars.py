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

""" where the SDK root is """
MLSDK = 'MLSDK'

""" where the toolchain root is """
MLTOOLS = 'MLTOOLS'

""" the search path for mabu extensions """
MABUPATH = 'MABUPATH'

""" the MLSDK host (win64, linux64, osx) """
HOST = 'HOST'

""" build specification (configuration+platform+toolchain+architecture) """
SPEC = 'SPEC'

""" user's signing information (may be unset) """
MLCERT = 'MLCERT'
# older name
SIGNER = 'SIGNER'

""" where the sign-file tool is """
SIGN_FILE = 'SIGN_FILE'

""" location of make binary """
MAKE = 'MAKE'

""" gathered make arguments (variable=value) """
MAKEARGS = 'MAKEARGS'

""" user-specified Visual Studio C/C++ root directory """
VCDIR = 'VCDIR'

""" user-specified compiler prefix (e.g. 'ccache') """
COMPILER_PREFIX = 'COMPILER_PREFIX'

""" user-specified linker prefix (e.g. 'ccache') """
LINKER_PREFIX = 'LINKER_PREFIX'

# This encapsulates variables passed by the user on the
# command line or otherwise detected from args.py,
# using the keys above.
#
# All these values should be passed down to 'make' with the same names.
runtime_values = {}
