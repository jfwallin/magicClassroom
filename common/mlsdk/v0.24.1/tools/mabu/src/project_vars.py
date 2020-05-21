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

#################################################
#
# These are the variables users may define in
# .mabu and .package projects.
#
# Any others are not supported.
#
#################################################

# The variables allowed for given file types
import config
from utils import uniquify

#
# Variables used in almost any kind of project
#
KIND = 'KIND'
REFS = 'REFS'
USES = 'USES'
OUT = 'OUT'
OUTNAME = 'OUTNAME'
OPTIONS = 'OPTIONS'

SHARED_VARS= [
    KIND,
    REFS,
    OUT,
]

#
# Variables used in packages (*.package)
#
MLCERT = 'MLCERT'
SIGNER = 'SIGNER'  # old name for MLCERT
ASSETS = 'ASSETS'
DATAS = 'DATAS'
PKGFLAGS = 'PKGFLAGS'  # mabu internal use

PACKAGE_VARS = [
    MLCERT,
    SIGNER,  # old name
    ASSETS,
    DATAS,
    PKGFLAGS,

    REFS,
    OUT,
    OUTNAME,
    USES,
    OPTIONS,
]

#
# Variables used in other projects (KIND = program, shared, static)
#

SRCS = 'SRCS'
OBJS = 'OBJS'
INCS = 'INCS'
DEFS = 'DEFS'
STLIBS = 'STLIBS'
SHLIBS = 'SHLIBS'
LIBPATHS = 'LIBPATHS'
LINKFLAGS = 'LINKFLAGS'  # mabu internal use

SRC_EXTENSIONS = 'SRC_EXTENSIONS'
EXTENSIONS = 'EXTENSIONS'

BUILD_VARS_COMP = [
    SRCS,
    OBJS,
    INCS,
    DEFS,
    OPTIONS,
    USES,
    STLIBS,
    SHLIBS,
    LIBPATHS,
    LINKFLAGS,
    DATAS,
    SRC_EXTENSIONS,
    EXTENSIONS,
]

BUILD_VARS = BUILD_VARS_COMP + [
    OUTNAME
]

#
# Variables derived from INCS, DEFS, OPTIONS, USES which
# users should not need to define in projects --
# these are expected to appear in components and options files.
#

CPPFLAGS = 'CPPFLAGS'
CFLAGS = 'CFLAGS'
CXXFLAGS = 'CXXFLAGS'
ASMFLAGS = 'ASMFLAGS'
LDFLAGS = 'LDFLAGS'
ARFLAGS = 'ARFLAGS'

SPECIFIC_VARS = [
    CPPFLAGS,
    CFLAGS,
    CXXFLAGS,
    ASMFLAGS,
    LDFLAGS,
    ARFLAGS,
    PKGFLAGS,
    LINKFLAGS
]

ALL_VARS = SHARED_VARS + PACKAGE_VARS + BUILD_VARS + SPECIFIC_VARS
uniquify(ALL_VARS)

#
# These variables are allowed in projects
#
PROJ_VARS = SHARED_VARS + BUILD_VARS + SPECIFIC_VARS
uniquify(PROJ_VARS)

#
# These settings may be `export`ed from projects.
#
PROJ_EXPORTS = [ OUT ] + BUILD_VARS + SPECIFIC_VARS

# always exported
PROJ_EXPORTS.remove(DATAS)
PROJ_EXPORTS.remove(USES)

#
# These settings are allowed in components
#
COMP_VARS = [ OUT ] + BUILD_VARS_COMP + SPECIFIC_VARS

#
# These settings are allowed in configurations
#
BASE = "BASE"

CONFIG_VARS = BUILD_VARS + SPECIFIC_VARS + [ BASE ]

#
# These settings are allowed in options
#
OPTION_VARS = BUILD_VARS + SPECIFIC_VARS
OPTION_VARS.remove(OPTIONS)

#
# These settings cannot be specialized per-target
#
UNSPECIALIZABLE_VARS = [ KIND, MLCERT, SIGNER ]

#
# These settings are always exported from *.mabu REFS
#
ALWAYS_EXPORTED_VARS = [ DATAS, ASSETS, REFS, USES ]

#
# These settings depend on project-relative filepaths
# so must be resolved before combining
#
PATH_VARS = [ SRCS, OBJS, INCS, LIBPATHS, ASSETS, MLCERT, SIGNER, OUT ]

extension_to_vars = {
    config.PROJ_EXT: PROJ_VARS,
    config.PACKAGE_EXT: PACKAGE_VARS,
    config.COMP_EXT: COMP_VARS,
    config.CONFIG_EXT: CONFIG_VARS,
    config.OPTION_EXT: OPTION_VARS,
}
