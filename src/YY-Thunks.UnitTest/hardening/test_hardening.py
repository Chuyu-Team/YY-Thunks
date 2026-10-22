"""Run from a VS x86/x64 developer prompt: python test_hardening.py.

Requires cl.exe with /Qspectre support and .NET Framework's C# compiler.
All outputs are temporary; the normal MSTest projects are not needed.
"""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


HERE = Path(__file__).resolve().parent
SCRIPT = HERE.parents[1] / "Build.cmd"


def run(command, **kwargs):
    result = subprocess.run(command, capture_output=True, text=True, errors="replace", **kwargs)
    if result.returncode:
        raise AssertionError(f"{command!r}\n{result.stdout}\n{result.stderr}")
    return result


class HardeningTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(prefix="YY hardening ")
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.root = Path(cls.temporary.name)
        cls.tools = cls.root / "tools"
        cls.tools.mkdir()
        csc = Path(os.environ["WINDIR"]) / "Microsoft.NET/Framework64/v4.0.30319/csc.exe"
        for tool in ("cl", "msbuild", "LibMaker"):
            run([str(csc), "/nologo", "/r:System.Web.Extensions.dll",
                 f"/out:{cls.tools / (tool + '.exe')}", str(HERE / "process_fixture.cs")])

    def check_build_flags(self, platform, hardened, count, source_name="source with spaces"):
        with tempfile.TemporaryDirectory(dir=self.root) as temporary:
            root = Path(temporary)
            source = root / source_name / "src"
            source.mkdir(parents=True)
            script = source / "Build.cmd"
            script.write_bytes(SCRIPT.read_bytes())
            log = root / "commands.jsonl"
            env = {key: value for key, value in os.environ.items()
                   if key.upper() not in ("PLATFORM", "YY_THUNKS_HARDENED")}
            env["PATH"] = str(self.tools) + os.pathsep + str(Path(env["WINDIR"]) / "System32")
            env["YY_HARDENING_LOG"] = str(log)
            if platform is not None:
                env["PLATFORM"] = platform
            if hardened is not None:
                env["YY_THUNKS_HARDENED"] = hardened
            run([env["COMSPEC"], "/d", "/c", str(script)], cwd=root, env=env)
            rows = [json.loads(line) for line in log.read_text(encoding="utf-8").splitlines()]
            commands = [row["args"] for row in rows if row["kind"] == "cl"]
            self.assertEqual(len(commands), count)
            enabled = hardened == "1" and (platform or "x86").lower() in ("x86", "x64")
            for arguments in commands:
                self.assertEqual(arguments.count("/GS-"), int(not enabled))
                for flag in ("/W4", "/GS", "/Qspectre", "/DYY_THUNKS_HARDENED=1"):
                    self.assertEqual(arguments.count(flag), int(enabled))
                self.assertIn("/MT", arguments)
                self.assertEqual(arguments[-1], str(source / "Thunks/YY_Thunks.cpp"))
            self.assertEqual(sum(any(arg.startswith("/FoLib") for arg in args) for args in commands), count // 2)
            self.assertEqual(sum(any(arg.startswith("/Foobjs") for arg in args) for args in commands), count // 2)

    def test_build_flags(self):
        for platform, count in (("x86", 14), ("x64", 12), ("X86", 14), ("X64", 12),
                                ("arm", 6), ("arm64", 4), (None, 14)):
            for hardened in (None, "0", "1", "2"):
                with self.subTest(platform=platform, hardened=hardened):
                    self.check_build_flags(platform, hardened, count)

    def test_build_unicode_source_path(self):
        self.check_build_flags("x64", "1", 12, "source with spaces \u6e2c\u8a66 \U0001f9ea")

    def test_native_startup_and_loaded_dll(self):
        for hardened in (None, "0", "1"):
            for custom in (False, True):
                output = self.root / (("custom" if custom else "original") + str(hardened) + ".exe")
                options = ["/nologo", "/std:c++17", "/EHsc", "/W4", "/Qspectre", "/MT",
                           "/GS" if hardened == "1" else "/GS-", str(HERE / "startup.cpp"),
                           f"/Fe{output}", f"/Fo{self.root / 'fixture.obj'}"]
                if hardened is not None:
                    options.append("/DYY_THUNKS_HARDENED=" + hardened)
                if custom:
                    options.append("/DYY_STARTUP_TEST_CUSTOM")
                run(["cl", *options])
                run([str(output)])
        unicode_directory = self.root / "DLL \u6e2c\u8a66 \U0001f9ea"
        unicode_directory.mkdir()
        dll = unicode_directory / "startup.dll"
        run(["cl", "/nologo", "/std:c++17", "/EHsc", "/W4", "/GS", "/Qspectre", "/MT", "/LD",
             "/DYY_THUNKS_HARDENED=1", "/DYY_STARTUP_TEST_DLL", str(HERE / "startup.cpp"),
             f"/Fe{dll}", f"/Fo{self.root / 'dll.obj'}", "/link", "/ENTRY:DllMainCRTStartupForYY_Thunks",
             f"/IMPLIB:{self.root / 'startup.lib'}"])
        run([str(self.root / "original1.exe"), str(dll)])


if __name__ == "__main__":
    unittest.main()
