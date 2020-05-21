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

from build_model import BuildSpec, BuildException
from config import PROJ_EXT, PACKAGE_EXT, COMP_EXT, CONFIG_EXT, OPTION_EXT
from project_vars import *
from projects import MultiVariable, PRIVATE_SUFFIX

SCOPE_EXPORT = 'export'

# zero-width joiner, indicates where lines were merged via '\'
ZWJ = u'\u200d'

def parse_list(s):
    """
    Parse a string as a space-separated list of items,
    accounting for quoting.
    :param s:
    :return: list of items
    """
    in_quote = False
    keep_quotes = False
    items = []
    item = ''

    for ch in s:
        if in_quote:
            if ch == '"':
                in_quote = False
                if not keep_quotes:
                    continue
                else:
                    keep_quotes = False
            item += ch
        else:
            if ch == '"':
                in_quote = True
                if item:
                   keep_quotes = True
                else:
                    keep_quotes = False
                    continue
            if ch == ' ' or ch == '\t':
                if item:
                    items.append(item)
                    item = ''
                continue
            if ch == '#':
                # comment
                break
            item += ch

    if item:
        items.append(item)

    return items


# a line is a definition, with scope, optional specs and commas, followed by a comment, either, or none
LINE = re.compile(r'\s*(?:(?P<scope>[a-zA-Z]+)\s+)?((?P<id>([A-Za-z_-][A-Za-z0-9_.,-]*))\s*=\s*(?P<value>.*?)\s*)?$')

EMBEDDED_COMMENTS = re.compile(r'#.*?\u200d')

# make the spec representing "all"
ALL_SPEC = BuildSpec('', no_defaults=True)
# the spec representing "default"
DEFAULT_SPEC = None


def parse_vars(fp, ext, label, errors, warnings):
    """
    Parse content from the filelike @fp and return a map
    of variable names to MultiVariable.
    :param fp: filelike for text
    :type fp: file
    :param ext: the file extension
    :param label: label for errant lines (e.g. filename)
    :param errors: updated list of errors
    :param warnings: updated list of warnings
    :rtype: Dict[str, MultiVariable]
    """
    num = 0

    # variable mappings as MultiVariables
    vars = {}
    # :type: Dict[str, MultiVariable]

    def line_error(text):
        errors.append((label, num, text))

    def line_warn(text):
        warnings.append((label, num, text))

    def process(line):
        """
        :type  line: str
        """
        # replace comments with a space, and strip
        if '#' in line and ZWJ in line:
            line = EMBEDDED_COMMENTS.sub(' ', line)
        line = line.replace(ZWJ, ' ').strip()
        if not line or line.startswith('#'):
            return

        # debug("checking line: " + line)

        # all lines should be simple assignments
        match = LINE.match(line)
        if not match:
            line_error("unexpected syntax in line")
            return

        # validate the LHS
        var_spec = match.group('id')
        if not var_spec:
            return  # comment

        # rename deprecated settings
        var_spec = var_spec.replace(SIGNER, MLCERT)

        if '.' in var_spec:
            idx = var_spec.index('.')
            var = var_spec[0:idx]
            specs_str = var_spec[idx + 1:]

            if specs_str == 'default':
                specs = [DEFAULT_SPEC]
            else:
                specs = specs_str.split(',')
                for spec in specs:
                    if not spec:
                        line_error("Unexpected empty specialization in '{0}'".format(var_spec))

            if var in UNSPECIALIZABLE_VARS:
                line_error("setting cannot be specialized in '{0}'".format(var_spec))

        else:
            var = var_spec
            specs = [ALL_SPEC]

        if var not in ALL_VARS:
            line_warn("unrecognized project setting '{0}'".format(var))

        # make sure the variables match the project type (which will be
        # lost shortly afterward because the options/configs/etc. are
        # merged in)
        scope = match.group('scope')
        suffix = PRIVATE_SUFFIX if ext == PROJ_EXT else ''
        if scope:
            if scope != SCOPE_EXPORT:
                line_error("unexpected scope (only 'export' known): '{0}'".format(scope))
            elif var in ALWAYS_EXPORTED_VARS:
                line_warn("this setting is always exported: '{0}'".format(var))
            elif var not in PROJ_EXPORTS and var in ALL_VARS:
                line_error("cannot export this setting: '{0}'".format(var))
            else:
                if ext != PROJ_EXT:
                    if ext == PACKAGE_EXT:
                        line_warn("'export' is meaningless in a package, for: '{0}'".format(var))
                    else:
                        line_warn("'export' is redundant; all settings are exported: '{0}'".format(var))
                suffix = ''
        else:
            if var in ALWAYS_EXPORTED_VARS:
                suffix = ''

        var_scoped = var + suffix

        # convert value to a list
        try:
            value = parse_list(match.group('value'))
        except Exception as e:
            line_error(e.message)

        mv = vars.setdefault(var_scoped, MultiVariable())

        for spec in specs:
            try:
                exist = mv.set(spec, value)

                if exist:
                    line_error("redefining setting '{0}'".format(var_spec))
            except BuildException as e:
                line_error(str(e))

    num = 0
    curline = ''
    for line in fp.readlines():
        line = line.strip()
        curline += line
        num += 1

        if line.endswith('\\'):
            # continuation
            curline = curline[:-1] + ZWJ
            continue

        process(curline)
        curline = ''

    process(curline)

    return vars
