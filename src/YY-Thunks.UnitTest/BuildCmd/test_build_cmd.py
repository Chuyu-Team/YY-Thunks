"""Behavioral tests of the real Build.cmd, without compiling YY-Thunks.

Run from a Visual Studio developer console:
    python src/YY-Thunks.UnitTest/BuildCmd/test_build_cmd.py -v

Python 3.8+ and an existing MSVC compiler are required. The native stub records
tool invocations and returns exact Windows statuses; Build.cmd itself is copied
unchanged into an isolated tree. No production compiler flags or binaries are
tested by this suite.
"""

import ctypes
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


HERE = Path(__file__).resolve().parent
BUILD_CMD = HERE.parents[1] / "Build.cmd"
STAGES = (
    "msbuild", "cl-object", "FixObj-object", "AppendWeak-object",
    "cl-library", "FixObj-library", "AppendWeak-library", "library",
)


@unittest.skipUnless(os.name == "nt", "Build.cmd requires Windows cmd.exe")
class BuildCmdTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which("cl.exe")
        if compiler is None:
            raise RuntimeError("Run from an existing MSVC developer console (cl.exe is required)")
        cls.tools = tempfile.TemporaryDirectory(prefix="YY Build tools ")
        cls.addClassCleanup(cls.tools.cleanup)
        cls.bin = Path(cls.tools.name)
        executable = cls.bin / "tool_stub.exe"
        compiled = subprocess.run(
            [compiler, "/nologo", "/MT", "/EHsc", "/std:c++17",
             str(HERE / "tool_stub.cpp"), "/Fo" + str(cls.bin / "tool_stub.obj"),
             "/Fe" + str(executable), "/link", "/INCREMENTAL:NO"],
            cwd=cls.bin, capture_output=True, text=True, check=False,
        )
        if compiled.returncode:
            raise RuntimeError(compiled.stdout + compiled.stderr)
        for tool in ("msbuild.exe", "cl.exe", "LibMaker.exe"):
            shutil.copyfile(executable, cls.bin / tool)

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="YY Build space ")
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.source = self.root / "source space"
        (self.source / "src").mkdir(parents=True)
        shutil.copyfile(BUILD_CMD, self.source / "src/Build.cmd")
        self.caller = self.root / "caller space"
        self.caller.mkdir()

    def run_build(self, platform="x86", stage="", code=0, occurrence=1):
        log = self.root / "tools.log"
        log.write_text("", encoding="ascii")
        cwd_log = self.root / "cwd.txt"
        wrapper = self.root / "run.cmd"
        wrapper.write_text(
            '@echo off\n'
            'call "%YY_BUILD_TEST_SCRIPT%"\n'
            'set "result=%errorlevel%"\n'
            'cd >"%YY_BUILD_TEST_CWD%"\n'
            'exit /b %result%\n',
            encoding="ascii",
        )
        env = {
            **os.environ,
            "PATH": str(self.bin) + os.pathsep + str(Path(os.environ["SystemRoot"]) / "System32"),
            "Platform": platform,
            "YY_BUILD_TEST_SCRIPT": str(self.source / "src/Build.cmd"),
            "YY_BUILD_TEST_CWD": str(cwd_log),
            "YY_BUILD_TEST_LOG": str(log),
            "YY_BUILD_TEST_STAGE": stage,
            "YY_BUILD_TEST_CODE": str(code),
            "YY_BUILD_TEST_OCCURRENCE": str(occurrence),
        }
        # cmd's dynamic ERRORLEVEL must not be shadowed by an inherited variable.
        env.pop("ERRORLEVEL", None)
        result = subprocess.run(
            [os.environ["COMSPEC"], "/d", "/u", "/c", str(wrapper)], cwd=self.caller,
            env=env, capture_output=True, text=True, encoding="utf-16-le", check=False,
        )
        self.assertEqual(Path(cwd_log.read_text(encoding="utf-16-le").strip()), self.caller,
                         "Build.cmd must restore its caller's current directory")
        return ctypes.c_int32(result.returncode).value, log.read_text().splitlines(), result.stdout + result.stderr

    def test_all_architectures_complete_the_expected_builds(self):
        for platform, count, definitions in (("x86", 7, 5), ("x64", 6, 3), ("arm", 3, 0), ("arm64", 2, 0), ("", 7, 5)):
            with self.subTest(platform=platform):
                code, calls, output = self.run_build(platform)
                self.assertEqual(code, 0, output)
                self.assertEqual(calls.count("msbuild"), 1)
                for stage in ("cl-object", "FixObj-object", "cl-library", "FixObj-library", "library"):
                    self.assertEqual(calls.count(stage), count, (stage, calls))
                self.assertEqual(calls.count("AppendWeak-object"), definitions)
                self.assertEqual(calls.count("AppendWeak-library"), definitions)
                self.assertEqual(calls[0], "msbuild")
                self.assertEqual(calls[-1], "library")

    def test_every_tool_failure_stops_immediately_and_preserves_its_status(self):
        for platform in ("x86", "x64"):
            for stage in STAGES:
                for status in (7, -1073741819):
                    with self.subTest(platform=platform, stage=stage, status=status):
                        code, calls, output = self.run_build(platform, stage, status)
                        self.assertEqual(code, status, output)
                        self.assertEqual(calls[-1], stage, calls)
                        self.assertEqual(calls.count(stage), 1, calls)

    def test_second_definition_failure_does_not_build_the_library(self):
        for stage in ("AppendWeak-object", "AppendWeak-library"):
            for status in (13, -1073741819):
                with self.subTest(stage=stage, status=status):
                    code, calls, output = self.run_build(stage=stage, code=status, occurrence=2)
                    self.assertEqual(code, status, output)
                    self.assertEqual(calls[-1], stage, calls)
                    self.assertEqual(calls.count(stage), 2, calls)

    def test_a_later_target_failure_does_not_continue_to_the_next_target(self):
        for stage in STAGES[1:]:
            with self.subTest(stage=stage):
                code, calls, output = self.run_build(stage=stage, code=19, occurrence=3)
                self.assertEqual(code, 19, output)
                self.assertEqual(calls[-1], stage, calls)
                self.assertEqual(calls.count(stage), 3, calls)

    def test_existing_output_directories_and_repeated_calls_are_successful(self):
        (self.source / "objs/x86").mkdir(parents=True)
        (self.source / "Lib/5.0.2195.0/x86").mkdir(parents=True)
        first = self.run_build()
        second = self.run_build()
        self.assertEqual(first[0], 0, first[2])
        self.assertEqual(second[0], 0, second[2])
        self.assertEqual(first[1], second[1])

    def test_unicode_workspace_paths_preserve_failures_and_caller_directory(self):
        self.root = self.root / "\u4e2d workspace"
        self.source = self.root / "\u4e2d source space"
        (self.source / "src").mkdir(parents=True)
        shutil.copyfile(BUILD_CMD, self.source / "src/Build.cmd")
        self.caller = self.root / "\u4e2d caller space"
        self.caller.mkdir()
        code, calls, output = self.run_build(stage="AppendWeak-object", code=-1073741819)
        self.assertEqual(code, -1073741819, output)
        self.assertEqual(calls[-1], "AppendWeak-object", calls)


if __name__ == "__main__":
    unittest.main()
