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


class SdkShimParser(object):
    def __init__(self, text, mlsdk, version, host):
        self._var_map = {'MLSDK': mlsdk, 'MLSDK_VERSION': version, 'HOST': host}

        # parse line by line
        for line in text.split('\n'):

            # remove trailing and leading whitespace
            line = line.strip()

            # toss out blank lines or lines starting with “#”
            if len(line) == 0 or line[0] == '#':
                continue

            # split into variable name and value around “=” and trim whitespace around each
            equ = line.find('=')

            variable = line[0 : equ].strip()
            value = line[equ + 1:].strip()

            # if the variable name contains variable expansions “$(...)”
            # these may only reference implicit variables or variables defined
            # in earlier lines, e.g. “$(FOO_$(HOST))”.
            # Such expansions should be performed at this step to define a concrete variable name.

            # place the expanded variable name and its unprocessed text into a variable ⇒ value map, stored as strings
            variable = self.expand_text(variable)
            self._var_map[variable] = value

            diags.debug("Defining",variable,'=',value)

    def expand(self, var):
        # look up a desired variable in the map (if missing, this is an error)

        if var not in self._var_map:
            raise KeyError("no such variable " + var)

        # places the unprocessed text of the variable into a string buffer
        var_val = self._var_map[var]
        expanded = self.expand_text(var_val)

        # enable for caching
        # self._var_map[var] = expanded

        # if a variable is treated as a path list, split it at “;”  (for all hosts)
        return expanded.split(';')

    def extract_variable(self, text, start):
        # handle nested parens and such
        parens = 1
        pos = start
        while pos < len(text):
            ch = text[pos]
            pos += 1
            if ch == '(':
                parens += 1
            elif ch == ')':
                parens -= 1
                if parens == 0:
                    break
        return text[start : pos - 1], pos

    def expand_text(self, var_val):
        # loop:
        while True:
            # look for $(...) references in the buffer
            exp_index = var_val.find('$(')
            if exp_index < 0:
                # if none, break loop
                break

            exp_var, exp_end = self.extract_variable(var_val, exp_index + 2)

            # expand the variable
            exp_var = self.expand_text(exp_var)

            # look up the unprocessed text of the named variable
            exp_val = self._var_map.get(exp_var)
            if not exp_val:
                # if variables are missing, warn to the user log and assume the value is the empty string
                diags.warn("Missing variable", exp_var," -- assuming empty")
                exp_val = ''

            # replace references to $(...) with the value
            var_val = var_val[0 : exp_index] + exp_val + var_val[exp_end:]

        return var_val

