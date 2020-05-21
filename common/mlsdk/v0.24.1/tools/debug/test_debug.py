#!/usr/bin/env python3
"""Unit tests for debug.py script

Needs to be run with python 3, the python of the mlsdk can be used.
E.g. on Windows:
..\python3\python.exe test_debug.py
"""
import logging
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile
import time
import unittest
import mldb
import json
import debug

logger = logging.getLogger("test_debug")


class TestUtils:
    @staticmethod
    def remove_if_exists(path):
        if os.path.exists(path):
            if os.path.isdir(path):
                shutil.rmtree(path)
            else:
                os.remove(path)

    @staticmethod
    def time_ms():
        """ Returns current time in milliseconds using monotonic clock. Starting
            point is undefined
        """
        return int(round(time.monotonic() * 1000))

    @staticmethod
    def wait_for(test, timeout_ms=5000):
        """ Waits for the given test condition (function) to return true, checking periodically
            (every 30ms) until a maximum of timeout_ms. Raises exception if times out. Default
            timeout is 5 seconds
        """
        start_ms = TestUtils.time_ms()
        while True:
            if test():
                    return
            if (TestUtils.time_ms() - start_ms) >= timeout_ms:
                    raise Exception('wait timed out')
            time.sleep(0.030)


class TestDebugScript(unittest.TestCase):

    @staticmethod
    def check_output(*args, ignore_failure=False, **kwargs):
        """ similar to subprocess.check_output
            but prints stderr/stdout on failure.

            Also returns stdout as string, subprocess.check_output returns a byte array in Python 3.
        """
        try:
            output = subprocess.check_output(*args, **kwargs)
        except subprocess.CalledProcessError as e:
            if ignore_failure:
                if e.output is None:
                    return ""
                return e.output.decode('utf-8')

            if hasattr(e, 'stderr') and e.stderr is not None and len(e.stderr) > 0:
                print("stderr:'%s'" % e.stderr.decode('utf-8'))

            if e.output is not None and len(e.output) > 0:
                print("stdout:'%s'" % e.output.decode('utf-8'))

            raise e

        return str(output, 'utf-8')  # required in python 3

    @classmethod
    def run_mldb(clazz, args, ignore_failure=False):
        output = TestDebugScript.check_output([TestDebugScript.mldb_path] + args, stderr=subprocess.STDOUT, ignore_failure=ignore_failure)
        return output

    @classmethod
    def run_mabu(clazz, args):
        output = TestDebugScript.check_output([TestDebugScript.mabu_path] + args, env=TestDebugScript.environ, stderr=subprocess.STDOUT)
        return output

    @classmethod
    def run_debug(clazz, args):
        all_args = [TestDebugScript.debug_path, "--mldb", TestDebugScript.mldb_path] + args
        output = TestDebugScript.check_output(all_args, env=TestDebugScript.environ, stderr=subprocess.STDOUT)
        return output

    @classmethod
    def setUpClass(clazz):
        """ Setup done once per class """
        mydir = os.path.dirname(os.path.realpath(__file__))
        # We use the shell mabu/debug scripts by default, but on plain windows we need to
        # use mabu.cmd and debug.cmd instead
        script_suffix = ""  # TODO remove this if it is not needed
        program_extension = ""
        if sys.platform == 'win32':
            script_suffix = ".cmd"
            program_extension = ".exe"

        TestDebugScript.environ = os.environ.copy()
        TestDebugScript.environ.pop('PYTHONPATH', None)  # don't let this script's environ affect the python scripts we invoke (mabu, debug)
        TestDebugScript.mabu_path = os.path.join(mydir, "..", "..", "mabu%s" % script_suffix)
        TestDebugScript.mabu_path = os.path.realpath(TestDebugScript.mabu_path)

        if not os.path.exists(TestDebugScript.mabu_path):
            raise AssertionError("mabu not found. Expected it here: {}".format(TestDebugScript.mabu_path))

        TestDebugScript.proj_path = os.path.join(mydir, "../../samples/SimpleNativeApp")
        TestDebugScript.proj_path = os.path.realpath(TestDebugScript.proj_path)
        if not os.path.exists(TestDebugScript.proj_path):
            raise AssertionError("example project not found. Expected it here: {}".format(TestDebugScript.proj_path))

        # Paths to the package zip and the executable (probably don't exist yet)
        TestDebugScript.pkgzip = os.path.join(TestDebugScript.proj_path, ".out/SimpleNativeApp/SimpleNativeApp.mpk")
        TestDebugScript.exe = os.path.join(TestDebugScript.proj_path, ".out/debug_lumin_clang-3.8_aarch64/SimpleNativeApp")

        TestDebugScript.pkgname = "com.magicleap.sample.simplenativeapp"

        TestDebugScript.debug_path = os.path.join(os.path.dirname(TestDebugScript.mabu_path), "debug%s" % script_suffix)
        if not os.path.exists(TestDebugScript.mabu_path):
            raise AssertionError("debug not found. Expected it here: {}".format(TestDebugScript.debug_path))

        TestDebugScript.debug_path = os.path.join(os.path.dirname(TestDebugScript.mabu_path), "debug%s" % script_suffix)
        if not os.path.exists(TestDebugScript.mabu_path):
            raise AssertionError("debug not found. Expected it here: {}".format(TestDebugScript.debug_path))

        # Support for running this test from the git workspace. Such a workspace
        # will not have everything the built mlsdk does. E.g. it lacks headers in the
        # include
        include_dir = os.path.join(mydir, "../../include")
        include_dir = os.path.realpath(include_dir)
        if not os.path.exists(include_dir):
            # Running out of the git workspace

            # Sanity check. Make sure the developer actually got the mabu submodule
            mabu_py = os.path.join(mydir, "../mabu/src/mabu.py")
            mabu_py = os.path.realpath(mabu_py)
            if not os.path.exists(mabu_py):
                raise AssertionError("mabu python script not found. Looks like you forgot to specify --recursive when you cloned the respository. Do 'git submodule update --init --recursive'")

            # Make sure we've been pointed to a full MLSDK (for everything else other than mabu and debug)
            if 'MLSDK' not in os.environ:
                raise AssertionError("Looks like you're running the test out of the MLSDK git repository. You need to point me to an official MLSDK installation by setting the env var 'MLSDK'")
            mlsdk_dir = os.environ['MLSDK']
            if not os.path.exists(mlsdk_dir):
                raise AssertionError("MLSDK environment variable is defined to a non-existent location: {}".format(mlsdk_dir))

            ml_api_h = os.path.join(mlsdk_dir, "include/ml_api.h")
            if not os.path.exists(ml_api_h):
                raise AssertionError("Didn't find expected MLSDK header file: {}".format(ml_api_h))

        else:
            mlsdk_dir = os.path.join(mydir, "../..")
            mlsdk_dir = os.path.realpath(mlsdk_dir)

        output = TestDebugScript.run_mabu(["-q", "--version"])  # -q to avoid default target
        print(output)

        mldb_internal_path = os.path.join(mlsdk_dir, "internal/tools/mldb/mldb%s" % program_extension)
        mldb_external_path = os.path.join(mlsdk_dir, "tools/mldb/mldb%s" % program_extension)

        if not os.path.exists(mldb_internal_path):
            if not os.path.exists(mldb_external_path):
                raise AssertionError("Didn't find mldb. Expected it here: {}".format(TestDebugScript.mldb_external_path))
            else:
                TestDebugScript.mldb_path = mldb_external_path
                TestDebugScript.cert_path = os.path.join(mlsdk_dir, 'ml_internal.cert')
        else:
            TestDebugScript.mldb_path = mldb_internal_path
            TestDebugScript.cert_path = os.path.join(mlsdk_dir, 'internal/ml_internal.cert')

        if not os.path.exists(TestDebugScript.cert_path):
            raise AssertionError("Didn't find signing certificate. Expected it here: {}".format(TestDebugScript.cert_path))

        output = TestDebugScript.run_mldb(["version"])  # DTOOLS-707 is done
        print(output)

    @classmethod
    def tearDownClass(clazz):
        """ Cleanup done once per class """
    def setUp(self):
        """ Setup done once per test method """
    def tearDown(self):
        """ Cleanup done once per test method """

    def test_launch(self):
        print("\r")  # unittest oddity, to get first output line in right place

        # Convenience aliases
        pkgzip = TestDebugScript.pkgzip
        pkgname = TestDebugScript.pkgname
        exe = TestDebugScript.exe
        proj_path = TestDebugScript.proj_path

        TestUtils.remove_if_exists(pkgzip)  # in case it's there from previous run/build)
        TestDebugScript.run_mabu(["-t", "debug_lumin", "--clean", os.path.join(proj_path, "SimpleNativeApp.package")])
        self.assertFalse(os.path.exists(exe))
        # TestDebugScript.run_mabu(["-t", "debug_lumin", os.path.join(proj_path, "SimpleNativeApp.package")])
        TestDebugScript.run_mabu(["-t", "debug_lumin", "-s", os.path.join(proj_path, TestDebugScript.cert_path), os.path.join(proj_path, "SimpleNativeApp.package")])
        self.assertTrue(os.path.exists(exe))
        self.assertTrue(os.path.exists(pkgzip))

        # Ensure a clean slate        
        # Fails if there might was no previous installation.
        TestDebugScript.run_mldb(["uninstall", pkgname], ignore_failure=True)

        gdb_cmd_file = tempfile.NamedTemporaryFile("w+", delete=False)
        try:
            with gdb_cmd_file:
                gdb_cmd_file.write("break main\n")
                gdb_cmd_file.write("continue\n")
                gdb_cmd_file.write("set confirm off\n")
                gdb_cmd_file.write("quit\n")
                gdb_cmd_file.flush()

            for i in range(0, 3):
                if i == 0:
                    args = ["--deploy-mpk", pkgzip, exe]
                    print("Launching debug session with deploy-mpk and verifying outcome. Will take a bit...", end="")
                elif i == 1:
                    args = ["--deploy", "--package", pkgzip, exe]
                    print("Launching debug session with deploy and verifying backward compatibility. Will take a bit...", end="")
                elif i == 2:
                    args = ["--package", pkgname, exe]
                    print("Launching debug session without deploy and verifying outcome. Will take a bit...", end="")

                sys.stdout.flush()  # because we skip newline in previous print (end="")

                args += ["--exec", gdb_cmd_file.name, "--no_console"]
                output = TestDebugScript.run_debug(args)

                # There's a lot of output from the debug script and from gdb.
                # The stuff we care about is at the end. There we should see the
                # result of continuing to the breakpoint at main() we set. We
                # should find something like
                #
                #   Breakpoint 1, main () at /home/jdoe/mlsdk/samples/SimpleNativeApp/main.cpp:40
                #   40      MLLifecycleCallbacksEx callbacks{};
                #
                # Depending on the size of the console, the breakpoint output
                # line might get split across two lines
                #
                next_line_is_code = False
                next_line_is_bkpt_part2 = False
                lines = output.split('\n')
                success = False
                for line in lines:
                    if next_line_is_code:
                        self.assertTrue(re.search("MLLifecycleCallbacksEx callbacks{};", line) is not None, output)
                        success = True
                        break
                    if next_line_is_bkpt_part2:
                        self.assertTrue(re.search("^ *at .*[/\\\\]main\.cpp:", line) is not None, output)
                        next_line_is_code = True
                    elif re.search("^Breakpoint 1, main \(.*[/\\\\]main\.cpp:[\d]+", line) is not None:
                        next_line_is_code = True
                    elif re.search("^Breakpoint 1, main \(", line) is not None:
                        next_line_is_bkpt_part2 = True

                self.assertTrue(success, output)
                print("done")
        finally:
            os.remove(gdb_cmd_file.name)
            TestDebugScript.run_mldb(["uninstall", pkgname], ignore_failure=True)

        return

    def test_attach(self):
        print("\r")  # unittest oddity, to get first output line in right place

        # Create a copy of the example project, and add a sleep call. The kali
        # example app crashes for me after a few seconds. Might be related to any
        # number of valid or invalid things. We don't want to rely on any of that.
        # We don't need a successfully running Kali app. We need *any* app that
        # runs indefinitely...doing whatever. An app that sleeps--that will do!
        pkgname = TestDebugScript.pkgname
        proj_path = TestDebugScript.proj_path
        proj_path_parts = os.path.split(proj_path)
        new_proj_path = os.path.join(proj_path_parts[0], proj_path_parts[1] + "_sleep")
        TestUtils.remove_if_exists(new_proj_path)  # in case it's there from previous run/build)
        shutil.copytree(proj_path, new_proj_path)
        main_content = '''
#include <unistd.h>
#include <stdio.h>
#include <ml_lifecycle.h>
#include <ml_logging.h>

int main() {
    MLLifecycleCallbacksEx callbacks{};
    MLLifecycleInitEx(&callbacks, nullptr);
    for (int loop = 0; ;loop++) {
        ML_LOG_TAG(Info, "SimpleNativeApp_sleep", "Hello from the loop!");
        sleep(1);
    }
}
'''
        # overwrite the sample main.cpp with our sleep loop.
        main_cpp = os.path.join(new_proj_path, "main.cpp")
        with open(main_cpp, "w") as file:
            file.write(main_content)

        proj_path = new_proj_path
        pkgzip = os.path.join(proj_path, ".out/SimpleNativeApp/SimpleNativeApp.mpk")
        exe = os.path.join(proj_path, ".out/debug_lumin_clang-3.8_aarch64/SimpleNativeApp")

        TestUtils.remove_if_exists(pkgzip)  # in case it's there from previous run/build)
        TestDebugScript.run_mabu(["-t", "debug_lumin", "--clean", os.path.join(proj_path, "SimpleNativeApp.package")])
        self.assertFalse(os.path.exists(exe), "does exist: " + exe)
        # TestDebugScript.run_mabu(["-t", "debug_lumin", os.path.join(proj_path, "SimpleNativeApp.package")])
        TestDebugScript.run_mabu(["-t", "debug_lumin", "-s", os.path.join(proj_path, TestDebugScript.cert_path), os.path.join(proj_path, "SimpleNativeApp.package")])
        self.assertTrue(os.path.exists(exe), "does not exist: " + exe)
        self.assertTrue(os.path.exists(pkgzip), "does not exist: " + pkgzip)

        # Terminate any already running instance of the application, fails
        # if there was no running process to kill
        TestDebugScript.run_mldb(["terminate", "-f", pkgname], ignore_failure=True)

        # Fails if there might was no previous installation.
        TestDebugScript.run_mldb(["uninstall", pkgname], ignore_failure=True)

        TestDebugScript.run_mldb(["install", pkgzip])

        TestDebugScript.run_mldb(["launch", "-f", pkgname])  # note: we're not specifying -d, which would launch the app in gdbserver

        # Let the application get to the sleep. One second should be plenty
        time.sleep(1)

        gdb_cmd_file = tempfile.NamedTemporaryFile("w+", delete=False)
        try:
            with gdb_cmd_file:
                gdb_cmd_file.write("backtrace\n")    # print stack
                gdb_cmd_file.write("monitor exit\n")  # tell gdbserver to quit
                gdb_cmd_file.write("set confirm off\n")
                gdb_cmd_file.write("quit\n")
                gdb_cmd_file.flush()

            print("Launching debug session and verifying outcome. Will take a bit...", end="")
            sys.stdout.flush()  # because we skip newline in previous print (end="")
            output = TestDebugScript.run_debug(["--attach", "--package", pkgname, "--exec", gdb_cmd_file.name, exe, "--no_console"])
        finally:
            os.remove(gdb_cmd_file.name)

        # There's a lot of output from the debug script and from gdb. The
        # stuff we care about is at the end, and is the result of running the
        # backtrace command
        lines = output.split('\n')

        stack = []
        for line in lines:
            if re.search("^#[0-9]+ +0x[0-9a-f]{16,16} in .+()", line) is not None:
                stack.append(line)
            pass

        # backtrace gdb command will print aproximately the following. It will at
        # least have the bottom two entries. Verify that.
        #
        # 0  0x0000002005e9fd28 in nanosleep () from target:/system/lib64/libc.so
        # 1  0x0000002005e72aa8 in sleep () from target:/system/lib64/libc.so
        # 2  0x0000005555d7eff0 in main ()

        self.assertTrue(len(stack) >= 2, output)
        self.assertTrue(re.search("^#[0-9]+ +0x[0-9a-f]{16,16} in sleep ()", stack[-2]) is not None, output)
        self.assertTrue(re.search("^#[0-9]+ +0x[0-9a-f]{16,16} in main ()", stack[-1]) is not None, output)

        # Cleanup
        shutil.rmtree(new_proj_path)
        TestDebugScript.run_mldb(["uninstall", pkgname], ignore_failure=True)

        print("done")
        return

    def package_mldb_tests(self, device, test_package_name):
        print("Testing with package %s" % test_package_name)
        device.ls("/documents/C1", wide=False, directory=True, package=test_package_name)
        test_file = __file__
        device.push(test_file, "/documents/C1/push_test_file", package=test_package_name)
        with tempfile.TemporaryDirectory() as tmpdir:
            device.pull("/documents/C1/push_test_file", tmpdir, package=test_package_name)
            ref_size = os.path.getsize(test_file)
            pull_size = os.path.getsize(os.path.join(tmpdir, 'push_test_file'))
            self.assertEqual(ref_size, pull_size)

    # Test some mldb.py commands.
    #
    # Currently many commands are not even executed, some just executed but
    # we do not validate their effect.
    def test_cmds(self):
        try:
            device = mldb.get_device(mldb_path=TestDebugScript.mldb_path)
            # regression test
            self.assertEqual('ml', device.get_prop('ro.product.brand'))

            device.usb()
            time.sleep(2.0) # sleep to avoid race condition for the wait call
            device.wait()

            devices_json = device.packages(as_json=True)
            package_list = json.loads(devices_json)
            debuggable_packages = [p for p in package_list if p['debuggable']]
            if len(debuggable_packages) == 0:
                print("No installed packages, skipping package related tests")
            else:
                test_package_name = debuggable_packages[0]["package"]
                self.package_mldb_tests(device, test_package_name)

            device.forward("tcp:10011", "tcp:10003")
            device.forward_list()
            device.forward_remove("tcp:10011")
            device.forward_no_rebind("tcp:10011", "tcp:10003")
            device.forward_remove_all()

            device.reverse("tcp:10020", "tcp:10030")
            device.reverse_list()
            device.reverse_remove("tcp:10020")
            device.reverse_no_rebind("tcp:10020", "tcp:10030")
            device.reverse_remove_all()

            device.wait()
            props = device.get_props()
            self.assertTrue('ro.product.brand' in props)

            self.assertEqual('ml', device.get_prop('ro.product.brand'))

            # Note: can only set specific attrbutes, others are ignored.
            device.set_prop('is_ignored', 'stuff')

            device.ps(as_json=False)
            device.ps(as_json=True)

            # Commands not tested at all (explicitly)
            # device.install(filename, replace=False, create_generator=False)
            # device.uninstall(filename, replace=False)
            # device.launch(package_name, component_name=None, force_restart=True, debug=False, host_port=7777)
            # device.terminate(package_name, component_name=None, force=True)
            # device.attach(package_name, component_name=None, host_port=7777)
            # device.push(local, remote)
            # device.pull(remote, local)
            # device.tcpip(port)
            # device.reboot()
            # device.connect(host)
            # device.disconnect(host)
        except subprocess.CalledProcessError as e:
            print("stdout: %s", e.output)
            try:
                if e.stderr is not None:
                    print("stderr: %s", e.stderr)
            except AttributeError:
                pass  # ignoring 'CalledProcessError' object has no attribute 'stderr'
            raise(e)


if __name__ == '__main__':
    random.seed(0)
    suite = unittest.TestLoader().loadTestsFromName(__name__)
    unittest.TextTestRunner(verbosity=3).run(suite)
