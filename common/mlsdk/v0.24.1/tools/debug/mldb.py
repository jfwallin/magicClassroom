#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import argparse
import atexit
import logging
import os
import re
import subprocess
import sys


class ArgumentParser(argparse.ArgumentParser):
    """ArgumentParser subclass that provides mldb device selection."""

    def __init__(self, description=None):
        super(ArgumentParser, self).__init__(description=description, formatter_class=argparse.RawTextHelpFormatter)
        self.add_argument(
            "--mldb", dest="mldb_path",
            help="path to mldb. If not specified, uses the mldb in the MLSDK.")

        group = self.add_argument_group(title="device selection")
        group = group.add_mutually_exclusive_group()
        group.add_argument(
            "-a", action="store_const", dest="device", const="-a",
            help="directs commands to all interfaces")
        group.add_argument(
            "-d", action="store_const", dest="device", const="-d",
            help="directs commands to the only connected USB device")
        group.add_argument(
            "-s", metavar="SERIAL", action="store", dest="serial",
            help="directs commands to device with the given serial")

    def parse_args(self, args=None, namespace=None):
        result = super(ArgumentParser, self).parse_args(args, namespace)

        mldb_path = result.mldb_path
        if not mldb_path:
            # point to root of the owning sdk.
            base = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..')
            base = os.path.realpath(base)  # canonicalize, for neatness

            # check for internal build
            mldb_dir = os.path.join(base, 'internal', 'tools', 'mldb')
            if not os.path.isdir(mldb_dir):
                # use external build
                mldb_dir = os.path.join(base, 'tools', 'mldb')

            mldb_path = os.path.join(mldb_dir, 'mldb')
            if sys.platform.startswith("win"):
                mldb_path += ".exe"
            if not os.path.isfile(mldb_path):
                raise AssertionError("Malformed SDK. mldb not found at: %s" % mldb_path)

        # Sanity test. Make sure we can actually run mldb
        try:
            version_str([mldb_path])
        except (OSError, subprocess.CalledProcessError):
            msg = "ERROR: Unable to run mldb executable (tried '{}')."
            if not result.mldb_path:
                msg += "\n       Try specifying its location with --mldb."
            sys.exit(msg.format(mldb_path))

        try:
            if result.device == "-a":
                result.device = get_device(mldb_path=mldb_path)
            elif result.device == "-d":
                result.device = get_usb_device(mldb_path=mldb_path)
            else:
                result.device = get_device(result.serial, mldb_path=mldb_path)
        except (DeviceNotFoundError, NoUniqueDeviceError, RuntimeError):
            # Don't error out if we can't find a device.
            result.device = None

        return result


class FindDeviceError(RuntimeError):
    pass


class DeviceNotFoundError(FindDeviceError):
    def __init__(self, serial):
        self.serial = serial
        super(DeviceNotFoundError, self).__init__(
            'No device with serial {}'.format(serial))


class NoUniqueDeviceError(FindDeviceError):
    def __init__(self):
        super(NoUniqueDeviceError, self).__init__('No unique device')


class ShellError(RuntimeError):
    def __init__(self, cmd, stdout, stderr, exit_code):
        super(ShellError, self).__init__(
            '`{0}` exited with code {1}'.format(cmd, exit_code))
        self.cmd = cmd
        self.stdout = stdout
        self.stderr = stderr
        self.exit_code = exit_code


def quote_path_if_needed(path):
    if os.name == 'nt' or os.name == 'ce':
        if ' ' in path:
            path = '"' + path + '"'
    return path


def get_devices(mldb_path='mldb'):
    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call([mldb_path, 'start-server'], stdout=devnull,
                              stderr=devnull)
    out = split_lines(str(subprocess.check_output([mldb_path, 'devices']), 'utf-8'))

    # The first line of `mldb devices` just says "List of attached devices", so
    # skip that.
    devices = []
    for line in out[1:]:
        if not line.strip():
            continue
        if 'offline' in line:
            continue

        serial, _ = re.split(r'\s+', line, maxsplit=1)
        devices.append(serial)
    return devices


def _get_unique_device(product=None, mldb_path='mldb'):
    devices = get_devices(mldb_path=mldb_path)
    if len(devices) != 1:
        raise NoUniqueDeviceError()
    return MLDevice(devices[0], product, mldb_path)


def _get_device_by_serial(serial, product=None, mldb_path='mldb'):
    for device in get_devices(mldb_path=mldb_path):
        if device == serial:
            return MLDevice(serial, product, mldb_path)
    raise DeviceNotFoundError(serial)


def get_device(serial=None, product=None, mldb_path='mldb'):
    """Get a uniquely identified MLDevice if one is available.

    Raises:
        DeviceNotFoundError:
            The serial specified by `serial` or $ML_SERIAL is not
            connected.

        NoUniqueDeviceError:
            Neither `serial` nor $ML_SERIAL was set, and the number of
            devices connected to the system is not 1. Having 0 connected
            devices will also result in this error.

    Returns:
        An MLDevice associated with the first non-None identifier in the
        following order of preference:

        1) The `serial` argument.
        2) The environment variable $ML_SERIAL.
        3) The single device connected to the system.
    """
    if serial is not None:
        return _get_device_by_serial(serial, product, mldb_path)

    ml_serial = os.getenv('ML_SERIAL')
    if ml_serial is not None:
        return _get_device_by_serial(ml_serial, product, mldb_path)

    return _get_unique_device(product, mldb_path=mldb_path)


def _get_device_by_type(flag, mldb_path):
    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call([mldb_path, 'start-server'], stdout=devnull,
                              stderr=devnull)
    try:
        serial = str(subprocess.check_output(
            [mldb_path, flag, 'get-serialno']).strip(), 'utf-8')
    except subprocess.CalledProcessError:
        raise RuntimeError('mldb unexpectedly returned nonzero')
    if serial == 'unknown':
        raise NoUniqueDeviceError()
    return _get_device_by_serial(serial, mldb_path=mldb_path)


def get_usb_device(mldb_path='mldb'):
    """Get the unique USB-connected MLDevice if it is available.

    Raises:
        NoUniqueDeviceError:
            0 or multiple devices are connected via USB.

    Returns:
        An MLDevice associated with the unique USB-connected device.
    """
    return _get_device_by_type('-d', mldb_path=mldb_path)


def split_lines(s):
    """Splits lines in a way that works even on Windows and old devices.

    Windows will see \r\n instead of \n, old devices do the same, old devices
    on Windows will see \r\r\n.
    """
    # rstrip is used here to workaround a difference between splineslines and
    # re.split:
    # >>> 'foo\n'.splitlines()
    # ['foo']
    # >>> re.split(r'\n', 'foo\n')
    # ['foo', '']
    return re.split(r'[\r\n]+', s.rstrip())


def version_str(mldb_path):
    """Get the version string of mldb.
        @param mldb_path: list of cmdline tokens forming the base mldb cmdline.
    """
    return str(subprocess.check_output([mldb_path[0]] + ['version']), 'utf-8')


def version(mldb_path):
    """Get the version of mldb.
        @param mldb_path: list of cmdline tokens forming the base mldb cmdline.
    """
    vstr = version_str(mldb_path)
    pattern = r'.* version (\d+)\.(\d+)\.(\d+)'
    result = re.match(pattern, vstr.splitlines()[0])
    return [int(result.group(1)), int(result.group(2)), int(result.group(3))]


class MLDevice(object):
    # Delimiter string to indicate the start of the exit code.
    _RETURN_CODE_DELIMITER = b'x'

    # Follow any shell command with this string to get the exit
    # status of a program since this isn't propagated by mldb.
    #
    # The delimiter is needed because `printf 1; echo $?` would print
    # "10", and we wouldn't be able to distinguish the exit code.
    _RETURN_CODE_PROBE = [';', 'echo', '{0}$?'.format(_RETURN_CODE_DELIMITER)]

    # Maximum search distance from the output end to find the delimiter.
    # mldb on Windows returns \r\n even if mldbd returns \n. Some old devices
    # seem to actually return \r\r\n.
    _RETURN_CODE_SEARCH_LENGTH = len(
        '{0}255\r\r\n'.format(_RETURN_CODE_DELIMITER))

    def __init__(self, serial, product=None, mldb_path='mldb'):
        self.serial = serial
        self.product = product
        self.mldb_cmd = [mldb_path]

        if self.serial is not None:
            self.mldb_cmd.extend(['-s', serial])
        if self.product is not None:
            self.mldb_cmd.extend(['-p', product])
        self._linesep = None
        self._features = None

    @property
    def linesep(self):
        if self._linesep is None:
            self._linesep = str(subprocess.check_output(self.mldb_cmd +
                                                        ['shell', 'echo']), 'utf-8')
        return self._linesep

    @property
    def features(self):
        if self._features is None:
            try:
                self._features = split_lines(self._simple_call(['features']))
            except subprocess.CalledProcessError:
                self._features = []
        return self._features

    def has_shell_protocol(self):
        ver = version(self.mldb_cmd)
        assert(ver[0] == 1)
        assert(ver[1] == 0)
        return ver[2] >= 35 and 'shell_v2' in self.features

    def _make_shell_cmd(self, user_cmd):
        command = self.mldb_cmd + ['shell'] + user_cmd
        if not self.has_shell_protocol():
            command += self._RETURN_CODE_PROBE
        return command

    def _parse_shell_output(self, out):
        """Finds the exit code string from shell output.

        Args:
            out: Shell output string.

        Returns:
            An (exit_code, output_string) tuple. The output string is
            cleaned of any additional stuff we appended to find the
            exit code.

        Raises:
            RuntimeError: Could not find the exit code in |out|.
        """
        search_text = out
        if len(search_text) > self._RETURN_CODE_SEARCH_LENGTH:
            # We don't want to search over massive amounts of data when we know
            # the part we want is right at the end.
            search_text = search_text[-self._RETURN_CODE_SEARCH_LENGTH:]
        partition = search_text.rpartition(self._RETURN_CODE_DELIMITER)
        if partition[1] == '':
            raise RuntimeError('Could not find exit status in shell output.')
        result = int(partition[2])
        # partition[0] won't contain the full text if search_text was
        # truncated, pull from the original string instead.
        out = out[:-len(partition[1]) - len(partition[2])]
        return result, out

    def _simple_sync_call(self, cmd, package):
        if package is not None:
            cmd += ['-p', package]
        return self._simple_call(cmd)

    def _simple_call(self, cmd):
        logging.info(' '.join(self.mldb_cmd + cmd))
        return str(subprocess.check_output(
            self.mldb_cmd + cmd, stderr=subprocess.STDOUT), 'utf-8')

    def shell(self, cmd):
        """Calls `mldb shell`

        Args:
            cmd: command to execute as a list of strings.

        Returns:
            A (stdout, stderr) tuple. Stderr may be combined into stdout
            if the device doesn't support separate streams.

        Raises:
            ShellError: the exit code was non-zero.
        """
        exit_code, stdout, stderr = self.shell_nocheck(cmd)
        if exit_code != 0:
            raise ShellError(cmd, stdout, stderr, exit_code)
        return stdout, stderr

    def shell_nocheck(self, cmd):
        """Calls `mldb shell`

        Args:
            cmd: command to execute as a list of strings.

        Returns:
            An (exit_code, stdout, stderr) tuple. Stderr may be combined
            into stdout if the device doesn't support separate streams.
        """
        cmd = self._make_shell_cmd(cmd)
        logging.info(' '.join(cmd))
        p = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = p.communicate()
        if self.has_shell_protocol():
            exit_code = p.returncode
        else:
            exit_code, stdout = self._parse_shell_output(stdout)
        return exit_code, stdout, stderr

    def shell_popen(self, cmd, kill_atexit=True, preexec_fn=None,
                    creationflags=0, **kwargs):
        """Calls `mldb shell` and returns a handle to the mldb process.

        This function provides direct access to the subprocess used to run the
        command, without special return code handling. Users that need the
        return value must retrieve it themselves.

        Args:
            cmd: Array of command arguments to execute.
            kill_atexit: Whether to kill the process upon exiting.
            preexec_fn: Argument forwarded to subprocess.Popen.
            creationflags: Argument forwarded to subprocess.Popen.
            **kwargs: Arguments forwarded to subprocess.Popen.

        Returns:
            subprocess.Popen handle to the mldb shell instance
        """

        command = self.mldb_cmd + ['shell'] + cmd

        # Make sure a ctrl-c in the parent script doesn't kill gdbserver.
        if os.name == 'nt':
            creationflags |= subprocess.CREATE_NEW_PROCESS_GROUP
        else:
            if preexec_fn is None:
                preexec_fn = os.setpgrp
            elif preexec_fn is not os.setpgrp:
                fn = preexec_fn

                def _wrapper():
                    fn()
                    os.setpgrp()
                preexec_fn = _wrapper

        p = subprocess.Popen(command, creationflags=creationflags,
                             preexec_fn=preexec_fn, **kwargs)

        if kill_atexit:
            atexit.register(p.kill)

        return p

    def install(self, filename, replace=False, create_generator=False):
        cmd = ['install']
        if replace:
            cmd.append('-u')
        cmd.append(filename)

        if create_generator:
            # Produce a stdout generator that the caller can read from while the
            # process is running
            full_cmd = self.mldb_cmd + cmd
            logging.info(' '.join(full_cmd))
            popen = subprocess.Popen(full_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            for line in iter(popen.stdout.readline, ''):
                yield line
            popen.stdout.close()
            return_code = popen.wait()
            if return_code:
                stderr = ""
                with popen.stderr:
                    while True:
                        line = popen.stderr.readline()
                        if line == '':
                            break
                        stderr += line
                popen.stderr.close()
                stderr = stderr.encode()  # Return binary output, for consistency with self._simple_call().
                raise subprocess.CalledProcessError(return_code, full_cmd, output=stderr)

            popen.stderr.close()
            popen.stdout.close()
        else:
            # Invoke the command, wait until it completes, then return its output
            return self._simple_call(cmd)

    def uninstall(self, filename, replace=False):
        cmd = ['uninstall']
        cmd.append(filename)
        return self._simple_call(cmd)

    def launch(self, package_name, component_name=None, force_restart=True, debug=False, host_port=7777, env=None, init_str=None):
        cmd = ['launch']
        if debug:
            cmd += ['-d', '-p', str(host_port)]  # -d = launch in gdbserver
        if force_restart:
            cmd.append('-f')
        if env is not None:
            for value in env:
                cmd += ['-v', value]
        if init_str != None:
            cmd += ['-i', init_str]
        assert(package_name is not None)
        cmd.append(package_name)
        if component_name is not None:
            cmd.append(component_name)

        return self._simple_call(cmd)

    def terminate(self, package_name, component_name=None, force=True):
        cmd = ['terminate']
        if force:
            cmd.append('-f')
        assert(package_name is not None)
        cmd.append(package_name)
        if component_name is not None:
            cmd.append(component_name)
        return self._simple_call(cmd)

    def attach(self, package_name, component_name=None, host_port=7777):
        cmd = ['attach', '-p', str(host_port), package_name]
        if component_name is not None:
            cmd.append(component_name)
        return self._simple_call(cmd)

    def ls(self, remote, wide=False, directory=False, package=None):
        cmds = ['ls', remote]
        if wide:
            cmds.append('-l')
        if directory:
            cmds.append('-d')
        return self._simple_sync_call(cmds, package)

    def push(self, local, remote, package=None):
        return self._simple_sync_call(['push', local, remote], package)

    def pull(self, remote, local, package=None):
        return self._simple_sync_call(['pull', remote, local], package)

    def tcpip(self, port):
        return self._simple_call(['tcpip', port])

    def usb(self):
        return self._simple_call(['usb'])

    def reboot(self):
        return self._simple_call(['reboot'])

    def connect(self, host):
        return self._simple_call(['connect', host])

    def disconnect(self, host):
        return self._simple_call(['disconnect', host])

    def forward(self, local, remote):
        return self._simple_call(['forward', local, remote])

    def forward_list(self):
        return self._simple_call(['forward', '--list'])

    def forward_no_rebind(self, local, remote):
        return self._simple_call(['forward', '--no-rebind', local, remote])

    def forward_remove(self, local):
        return self._simple_call(['forward', '--remove', local])

    def forward_remove_all(self):
        return self._simple_call(['forward', '--remove-all'])

    def reverse(self, remote, local):
        return self._simple_call(['reverse', remote, local])

    def reverse_list(self):
        return self._simple_call(['reverse', '--list'])

    def reverse_no_rebind(self, local, remote):
        return self._simple_call(['reverse', '--no-rebind', local, remote])

    def reverse_remove_all(self):
        return self._simple_call(['reverse', '--remove-all'])

    def reverse_remove(self, remote):
        return self._simple_call(['reverse', '--remove', remote])

    def wait(self):
        return self._simple_call(['wait-for-device'])

    def get_props(self):
        result = {}
        output = self._simple_call(['getprop'])
        lines = split_lines(output)
        pattern = re.compile(r'^\[([^]]+)\]: \[(.*)\]')
        for line in lines:
            match = pattern.match(line)
            if match is None:
                raise RuntimeError('invalid getprop line: "{}"'.format(line))
            key = match.group(1)
            value = match.group(2)
            if key in result:
                raise RuntimeError('duplicate getprop key: "{}"'.format(key))
            result[key] = value
        return result

    def get_prop(self, prop_name):
        output = split_lines(self._simple_call(['getprop', prop_name]))
        if len(output) != 1:
            raise RuntimeError('Too many lines in getprop output:\n' +
                               '\n'.join(output))
        value = output[0]
        if not value.strip():
            return None
        return value

    def set_prop(self, prop_name, value):
        self._simple_call(['setprop', prop_name, value])

    def ps(self, as_json=True):
        args = ['ps']
        if as_json:
            args.append('-j')
        return self._simple_call(args)

    def packages(self, as_json=True):
        args = ['packages']
        if as_json:
            args.append('-j')
        return self._simple_call(args)
