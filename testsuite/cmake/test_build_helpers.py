#!/usr/bin/env python3
"""Exercise the real helper and launcher generator without GDL's dependencies.

Run: python3 testsuite/cmake/test_build_helpers.py
Requires Python 3, CMake, Ninja and C/C++ compilers; never installs packages.
Set CMAKE_COMMAND and CTEST_COMMAND to use tools outside PATH.
"""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

REPO = Path(__file__).resolve().parents[2]
CMAKE = os.environ.get("CMAKE_COMMAND") or shutil.which("cmake") or "cmake"
CTEST = os.environ.get("CTEST_COMMAND") or shutil.which("ctest") or "ctest"


class BuildHelpers(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.have_multi_config = "Ninja Multi-Config" in subprocess.check_output([CMAKE, "--help"], text=True)

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="gdl-build-test-")
        self.addCleanup(self.temp.cleanup)
        # The helper and launchers report physical paths; on macOS the temporary
        # directory is under /var, a symlink to /private/var.
        self.root = Path(self.temp.name).resolve()
        self.source = self.root / "source with spaces"
        self.source.mkdir()
        for directory in ("scripts", "CMakeModules", "src/pro", "resource/maps", "src/plplot/data"):
            (self.source / directory).mkdir(parents=True)
        for filename in ("scripts/build_gdl.sh", "scripts/run-gdl.in", "scripts/run-gdl-config.in",
                         "CMakeModules/GdlBuildTree.cmake"):
            shutil.copy2(REPO / filename, self.source / filename)
        (self.source / "src/pro/example.pro").write_text("procedure version 1")
        (self.source / "resource/maps/example").write_text("map")
        # A tiny executable reports exactly what the real generated launcher passes.
        (self.source / "main.c").write_text(r'''
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char **argv) {
  char cwd[8192];
  const char *names[] = {"GDL_DIR", "GDL_MAPS_DIR", "PLPLOT_LIB", "GDL_PATH",
    "GDL_DLM_PATH", "GDL_STARTUP", "IDL_STARTUP", NULL};
  printf("CONFIG=%s\n", CONFIG);
  printf("CWD=%s\n", getcwd(cwd, sizeof cwd));
  for (int i=0; names[i]; ++i)
    printf("%s=%s\n", names[i], getenv(names[i]) ? getenv(names[i]) : "");
  for (int i=1; i<argc; ++i) printf("ARG=%s\n", argv[i]);
  return argc > 1 ? 23 : 0;
}
''')
        module = self.source / "CMakeModules/GdlBuildTree.cmake"
        (self.source / "CMakeLists.txt").write_text('''cmake_minimum_required(VERSION 3.13.3)
project(LauncherTest C)
enable_testing()
add_executable(gdl main.c)
target_compile_definitions(gdl PRIVATE CONFIG="$<CONFIG>")
set_target_properties(gdl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/custom bin")
include("''' + str(module) + '''")
add_test(NAME launched COMMAND "${GDL_BUILD_LAUNCHER}")
install(TARGETS gdl DESTINATION bin)
file(WRITE "${CMAKE_BINARY_DIR}/helper-data-dir" "$ENV{GDL_DIR}")
''')
        self.build = self.root / "my build"
        self.build.mkdir()
        self.env = {k: v for k, v in os.environ.items() if not k.startswith("WITH_") and k not in (
            "BUILD_DIR", "ROOT_DIR", "INSTALL_PREFIX", "CC", "CXX", "CFLAGS", "CXXFLAGS",
            "LDFLAGS", "CMAKE_GENERATOR", "GENERATOR", "GDL_PATH", "IDL_PATH", "GDL_DIR",
            "GDL_DLM_PATH", "GDL_STARTUP", "IDL_STARTUP", "GDL_BUILD_CONFIG", "GDL_SRC_DIR",
            "DEPS", "GSL_CBLAS_PROVIDER", "PYTHONVERSION", "CMAKE_TOOLCHAIN_FILE")}
        self.env.update(Configuration="Debug", GENERATOR="Ninja", NTHREADS="2",
                        CCACHE_DIR=str(self.root / "ccache"), CMAKE_COMMAND=CMAKE, CTEST_COMMAND=CTEST)

    def command(self, args, cwd=None, env=None, status=0, include_stderr=False):
        result = subprocess.run([str(a) for a in args], cwd=cwd or self.build,
                                env=env or self.env, text=True, capture_output=True)
        self.assertEqual(result.returncode, status, result.stdout + result.stderr)
        return result.stdout + result.stderr if include_stderr else result.stdout

    def helper(self, action, *args, **kwargs):
        return self.command(["bash", self.source / "scripts/build_gdl.sh", action, *args], **kwargs)

    def cache(self):
        return {line.split(":", 1)[0]: line.split("=", 1)[1]
                for line in (self.build / "CMakeCache.txt").read_text().splitlines()
                if not line.startswith(("#", "//")) and ":" in line and "=" in line}

    def mock_tools(self, scripts):
        mocks = self.root / "mock tools"
        mocks.mkdir(exist_ok=True)
        for name, text in scripts.items():
            (mocks / name).write_text(text)
            (mocks / name).chmod(0o755)
        return mocks

    def test_reconfigure_preserves_settings_and_accepts_overrides(self):
        self.helper("config", "-DOPENMP=OFF", "-DX11=OFF", "-DGSL_CBLAS_PROVIDER=GSL",
                    "-DCMAKE_INSTALL_PREFIX=" + str(self.root / "custom install"))
        before = self.cache()
        env = dict(self.env, GDL_DIR="/user/runtime/data")
        del env["Configuration"]
        self.helper("config", env=env)
        after = self.cache()
        for setting in ("CMAKE_BUILD_TYPE", "OPENMP", "X11", "GSL_CBLAS_PROVIDER", "CMAKE_INSTALL_PREFIX"):
            self.assertEqual(before[setting], after[setting], setting)
        self.assertEqual((self.build / "helper-data-dir").read_text(), "/user/runtime/data")
        self.helper("build", env=env)
        self.helper("test", env=env)
        self.helper("install", env=env)
        self.assertTrue((self.root / "custom install/bin/gdl").exists())
        self.assertIn("CONFIG=Debug\n", self.command([self.build / "run-gdl"], env=env))
        self.helper("config", "-DX11=OFF", env=dict(env, WITH_OPENMP="ON", WITH_X11="ON"))
        self.assertEqual(self.cache()["OPENMP"], "ON")
        self.assertEqual(self.cache()["X11"], "OFF")
        self.helper("config", env=dict(env, Configuration="Release", GSL_CBLAS_PROVIDER="AUTO"))
        self.assertEqual(self.cache()["CMAKE_BUILD_TYPE"], "Release")
        self.assertEqual(self.cache()["GSL_CBLAS_PROVIDER"], "AUTO")

    def test_implicit_directory_guards(self):
        for cwd in (self.source / "scripts", self.source / "src/pro"):
            output = self.helper("config", cwd=cwd, status=1)
            self.assertIn("Refusing an implicit build", output)
            self.assertFalse((cwd / "CMakeCache.txt").exists())
        alias = self.root / "source alias"
        alias.symlink_to(self.source / "scripts", target_is_directory=True)
        self.helper("config", cwd=alias, status=1)
        fake_home = self.root / "home"
        fake_home.mkdir()
        self.helper("config", cwd=fake_home, env=dict(self.env, HOME=str(fake_home)), status=1)
        self.helper("config", cwd="/", status=1)

    def test_nonempty_directory_and_redirected_config_log(self):
        (self.build / "valuable-file").write_text("preserve me")
        (self.build / "dependency-files").mkdir()
        self.command(["bash", "-c", 'bash "$1" config >config.log 2>&1',
                      "helper-test", self.source / "scripts/build_gdl.sh"])
        self.assertTrue((self.build / "CMakeCache.txt").exists())
        self.assertIn("Configuring GDL", (self.build / "config.log").read_text())
        self.assertEqual((self.build / "valuable-file").read_text(), "preserve me")
        self.assertFalse((self.build / ".gdl-build-directory").exists())

    def test_prep_is_independent_of_build_directory(self):
        # Mock Homebrew so neither prep nor prep_dryrun can install packages.
        mocks = self.mock_tools({
            "uname": "#!/bin/sh\ncase $1 in -m) echo x86_64;; *) echo Darwin;; esac\n",
            "brew": '#!/bin/sh\n[ "$1" = install ] && printf "%s\\n" "$*" > "$GDL_TEST_CAPTURE"\nexit 0\n',
        })
        capture = self.root / "brew-args"
        fake_home = self.root / "home"
        fake_home.mkdir()
        nonempty = self.root / "nonempty"
        nonempty.mkdir()
        (nonempty / "valuable-file").write_text("preserve me")
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"],
                   HOME=str(fake_home), GDL_TEST_CAPTURE=str(capture))
        for action in ("prep", "prep_dryrun"):
            for cwd in (self.build, fake_home, self.source, self.source / "scripts", nonempty):
                with self.subTest(action=action, cwd=cwd):
                    if capture.exists():
                        capture.unlink()
                    self.helper(action, cwd=cwd, env=env)
                    if action == "prep":
                        self.assertTrue(capture.read_text().startswith("install "))
                    else:
                        self.assertFalse(capture.exists())
                    self.assertFalse(list(self.root.rglob(".gdl-build-directory")))
                    self.assertFalse((self.source / "build").exists())
            destination = self.root / "new build"
            self.helper(action, env=dict(env, BUILD_DIR=str(destination)))
            self.assertFalse(destination.exists())
        self.assertEqual((nonempty / "valuable-file").read_text(), "preserve me")

    def test_prep_fails_only_for_missing_homebrew_formulae(self):
        # brew install may fail after installing everything (e.g. link conflicts).
        mocks = self.mock_tools({
            "uname": "#!/bin/sh\ncase $1 in -m) echo x86_64;; *) echo Darwin;; esac\n",
            "brew": '#!/bin/sh\ncase $1 in\n'
                    '  install) exit "${GDL_TEST_BREW_STATUS:-0}" ;;\n'
                    '  list) [ "$3" != "${GDL_TEST_BREW_MISSING:-}" ] ;;\n'
                    'esac\n',
        })
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"])
        self.helper("prep", env=dict(env, GDL_TEST_BREW_STATUS="1"))
        output = self.helper("prep", env=dict(env, GDL_TEST_BREW_STATUS="1", GDL_TEST_BREW_MISSING="gsl"),
                             status=1)
        self.assertIn("Homebrew could not install: gsl", output)

    def test_prep_retries_pacman_and_stops_when_it_keeps_failing(self):
        mocks = self.mock_tools({
            "uname": "#!/bin/sh\ncase $1 in -m) echo x86_64;; *) echo MINGW64_NT-10.0;; esac\n",
            # Fails until the attempt given in GDL_TEST_PACMAN_OK (never if unset).
            "pacman": '#!/bin/sh\necho call >> "$GDL_TEST_CAPTURE"\n'
                      # Arithmetic expansion ignores the padding of BSD wc on macOS.
                      'n=$(( $(wc -l < "$GDL_TEST_CAPTURE") ))\n[ "$n" -eq "${GDL_TEST_PACMAN_OK:-0}" ]\n',
            "sleep": "#!/bin/sh\nexit 0\n",
            # Stop prep right after the package step; never download bsd-xdr.
            "curl": "#!/bin/sh\nexit 1\n",
        })
        capture = self.root / "pacman-calls"
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"], GDL_TEST_CAPTURE=str(capture))
        output = self.helper("prep", env=env, status=1)
        self.assertEqual(capture.read_text().count("call"), 3)
        self.assertIn("pacman could not install the dependencies", output)
        self.assertNotIn("Downloading", output)
        capture.unlink()
        output = self.helper("prep", env=dict(env, GDL_TEST_PACMAN_OK="2"), status=1)
        self.assertEqual(capture.read_text().count("call"), 2)
        self.assertIn("retrying in 30 seconds", output)
        self.assertIn("Failed to download", output)

    def test_packaging_uses_cached_prefix_and_accepts_override(self):
        prefix = self.root / "cached install"
        override = self.root / "override install"
        for directory in (prefix, override):
            directory.mkdir()
            (directory / "installed-file").touch()
        self.helper("config", "-DCMAKE_INSTALL_PREFIX=" + str(prefix))
        # The 32-bit Windows packaging branch avoids downloading Workbench.
        mocks = self.mock_tools({
            "cygpath": '#!/bin/sh\nprintf "%s\\n" "$2"\n',
            "makensis": '#!/bin/sh\nprintf "%s\\n" "$GDL_INSTALL_DIR" > "$GDL_TEST_CAPTURE"\n',
            "zip": '#!/bin/sh\npwd -P > "$GDL_TEST_CAPTURE"\n',
        })
        capture = self.root / "package-prefix"
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"],
                   GDL_TEST_CAPTURE=str(capture), Platform="i686")
        for action, system in (("pack", "MINGW64_NT"), ("prep_deploy", "Linux")):
            self.mock_tools({"uname": '#!/bin/sh\ncase $1 in -m) echo i686;; *) echo ' + system + ';; esac\n'})
            for settings, expected in (({}, prefix), ({"INSTALL_PREFIX": str(override)}, override)):
                with self.subTest(action=action, settings=settings):
                    self.helper(action, env=dict(env, **settings))
                    self.assertEqual(capture.read_text().strip(), str(expected))
        # Existing installations remain packageable without a CMake cache.
        (self.build / "CMakeCache.txt").unlink()
        self.helper("prep_deploy", env=dict(env, INSTALL_PREFIX=str(override)))
        self.assertEqual(capture.read_text().strip(), str(override))

    def test_cached_settings_tolerate_crlf_cache(self):
        # CMake on Windows writes CMakeCache.txt with CRLF line endings.
        # Pretend to be Linux on every host: prep_deploy must zip, not run hdiutil.
        mocks = self.mock_tools({
            "uname": "#!/bin/sh\ncase $1 in -m) echo x86_64;; *) echo Linux;; esac\n",
            "zip": '#!/bin/sh\npwd -P > "$GDL_TEST_CAPTURE"\n',
            "ctest": '#!/bin/sh\nprintf "%s|" "$@" > "$GDL_TEST_CAPTURE"\n',
        })
        capture = self.root / "capture"
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"], GDL_TEST_CAPTURE=str(capture))
        prefix = self.root / "cached install"
        prefix.mkdir()
        self.helper("config", "-DCMAKE_INSTALL_PREFIX=" + str(prefix), env=env)
        cache = self.build / "CMakeCache.txt"
        cache.write_bytes(cache.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        env = {k: v for k, v in env.items() if k != "Configuration"}
        env["CTEST_COMMAND"] = str(mocks / "ctest")
        self.helper("prep_deploy", env=env)
        self.assertEqual(capture.read_text().strip(), str(prefix))
        self.helper("test", env=env)
        self.assertEqual(capture.read_text(), "-C|Debug|--output-on-failure|")

    def test_configured_tools_take_precedence_over_path(self):
        mocks = self.mock_tools({
            "cmake": "#!/bin/sh\necho Unexpected CMake from PATH >&2\nexit 91\n",
            "ctest": "#!/bin/sh\necho Unexpected CTest from PATH >&2\nexit 92\n",
        })
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"])
        for action in ("config", "build", "test", "install"):
            self.helper(action, env=env)

    def test_windows_cached_gslcblas_does_not_warn(self):
        # Exercise FindGSL's diagnostic independently of installed GSL/BLAS.
        # Symbol probes are stubbed; only provider/path selection is under test.
        modules = self.root / "probe modules"
        modules.mkdir()
        (modules / "CheckCSourceCompiles.cmake").write_text('''
macro(check_c_source_compiles source result)
  if("${result}" STREQUAL "GDL_CUSTOM_CBLAS_WORKS")
    set(${result} TRUE)
  else()
    set(${result} FALSE)
  endif()
endmacro()
''')
        script = self.root / "find-gsl.cmake"
        script.write_text('''cmake_minimum_required(VERSION 3.13.3)
set(CMAKE_MODULE_PATH "''' + str(modules) + '''")
set(GSL_LIBRARY placeholder-gsl)
set(GSL_INCLUDE_DIR "''' + str(self.source) + '''")
include("''' + str(REPO / "CMakeModules/FindGSL.cmake") + '''")
''')
        for platform in ("WIN32", "CYGWIN"):
            for name in ("libgslcblas.dll.a", "libgslcblas.a", "gslcblas.lib", "GSLcblas.lib",
                         "libgslcblas.so.0", "libopenblas.dll.a"):
                with self.subTest(platform=platform, library=name):
                    output = self.command([CMAKE, "-D" + platform + "=ON",
                                           "-DGSLCBLAS_LIBRARY=" + str(self.root / name), "-P", script],
                                          include_stderr=True)
                    self.assertEqual("cannot change the CBLAS bindings" in " ".join(output.split()),
                                     "openblas" in name, output)

    def test_current_directory_workflow_and_reconfigure(self):
        self.helper("config", "-DUSER_SENTINEL=retained")
        self.assertTrue((self.build / "CMakeCache.txt").exists())
        sentinel = self.build / "keep-this-file"
        sentinel.write_text("must survive reconfiguration")
        self.helper("configure")
        self.assertTrue(sentinel.exists())
        self.assertIn("USER_SENTINEL:UNINITIALIZED=retained", (self.build / "CMakeCache.txt").read_text())
        self.helper("build")
        self.helper("test")
        self.assertFalse((self.source / "CMakeCache.txt").exists())
        caller = self.root / "caller"
        caller.mkdir()
        env = dict(self.env, GDL_DIR="/wrong", PLPLOT_LIB="/wrong", GDL_MAPS_DIR="/wrong",
                   IDL_PATH="/user path", GDL_DLM_PATH="/user modules",
                   GDL_STARTUP="/startup", IDL_STARTUP="/idl startup")
        output = self.command([self.build / "run-gdl", "-e", "print, 'hello world'", "", "*"],
                              cwd=caller, env=env, status=23)
        data = self.build / "share/gnudatalanguage"
        for line in (f"GDL_DIR={data}", f"GDL_MAPS_DIR={data}/resource/maps",
                     f"PLPLOT_LIB={self.source}/src/plplot/data",
                     f"GDL_PATH=+{data}/lib:/user path", f"CWD={caller}",
                     "GDL_STARTUP=/startup", "IDL_STARTUP=/idl startup",
                     "ARG=print, 'hello world'", "ARG=", "ARG=*"):
            self.assertIn(line + "\n", output)
        self.assertIn(f"GDL_DLM_PATH=+{self.build}/src/dlm:+{data}/dlm:/user modules\n", output)
        env["GDL_PATH"] = "/gdl user path"
        self.assertIn(f"GDL_PATH=+{data}/lib:/gdl user path\n",
                      self.command([self.build / "run-gdl"], env=env))

    def test_overrides_and_source_root_default(self):
        self.helper("config", cwd=self.source)
        self.assertTrue((self.source / "build/CMakeCache.txt").exists())
        env = dict(self.env, ROOT_DIR="legacy root")
        self.helper("config", env=env)
        self.assertTrue((self.build / "legacy root/build/CMakeCache.txt").exists())
        env["BUILD_DIR"] = "explicit build"
        self.helper("config", env=env)
        self.assertTrue((self.build / "explicit build/CMakeCache.txt").exists())
        self.helper("build", env=env)
        self.helper("test", env=env)

    def test_multi_configuration_and_missing_executable(self):
        if not self.have_multi_config:
            self.skipTest("Ninja Multi-Config needs CMake 3.17 or newer")
        env = dict(self.env, GENERATOR="Ninja Multi-Config")
        self.helper("config", env=env)
        self.command([self.build / "Debug/run-gdl"], env=env, status=127)
        self.helper("build", env=env)
        self.helper("test", env=env)
        self.assertIn("CONFIG=Debug\n", self.command([self.build / "run-gdl"],
                      env=dict(env, GDL_BUILD_CONFIG="Debug")))
        del env["Configuration"] # A cached CMAKE_BUILD_TYPE must not select Debug here.
        self.helper("build", env=env)
        self.assertIn("CONFIG=Release\n", self.command([self.build / "run-gdl"], env=env))
        self.command([self.build / "run-gdl"], env=dict(env, GDL_BUILD_CONFIG="unknown"), status=1)

    def test_explicit_in_source_workflow(self):
        env = dict(self.env, BUILD_DIR=".")
        self.helper("config", cwd=self.source, env=env)
        self.assertTrue((self.source / "CMakeCache.txt").exists())
        self.assertFalse((self.source / "build").exists())
        self.helper("build", cwd=self.source, env=env)
        self.helper("test", cwd=self.source, env=env)
        output = self.command([self.source / "run-gdl"], cwd=self.build, env=env)
        self.assertIn(f"GDL_DIR={self.source}/share/gnudatalanguage\n", output)
        # Legacy direct-CMake reconfiguration can reuse the same source tree.
        self.command([CMAKE, "."], cwd=self.source, env=env)
        self.helper("build", cwd=self.source, env=env)

    def test_real_testsuite_multi_config_fixture(self):
        if not self.have_multi_config:
            self.skipTest("Ninja Multi-Config needs CMake 3.17 or newer")
        suite = self.source / "testsuite"
        suite.mkdir()
        shutil.copy2(REPO / "testsuite/CMakeLists.txt", suite / "CMakeLists.txt")
        (suite / "LIST").write_text("test_call_external.pro\n")
        (suite / "example.grib").touch()
        (suite / "libtest_ce.cpp").write_text("int test_library() { return 0; }\n")
        cmake = self.source / "CMakeLists.txt"
        cmake.write_text(cmake.read_text().replace("project(LauncherTest C)", "project(LauncherTest C CXX)")
                         + "add_subdirectory(testsuite)\n")
        env = dict(self.env, GENERATOR="Ninja Multi-Config")
        self.helper("config", env=env)
        # Exercise the actual add_test declaration, including $<CONFIG> expansion.
        self.command([CTEST, "-C", "Debug", "--output-on-failure", "-R", "^test_build_libtest_ce$"], env=env)
        self.assertTrue(any((self.build / "testsuite/Debug").glob("*test_ce*")))
        tests = json.loads(self.command([CTEST, "-C", "Debug", "--show-only=json-v1"], env=env))["tests"]
        helper_test = next(test for test in tests if test["name"] == "test_build_helpers")
        properties = {prop["name"]: prop["value"] for prop in helper_test["properties"]}
        self.assertIn("CMAKE_COMMAND=" + self.cache()["CMAKE_COMMAND"], properties["ENVIRONMENT"])
        self.assertIn("CTEST_COMMAND=" + self.cache()["CMAKE_CTEST_COMMAND"], properties["ENVIRONMENT"])

    def test_macos_explicit_compiler_does_not_mix_in_llvm_defaults(self):
        scripts = {
            "uname": "#!/bin/sh\ncase $1 in -m) echo x86_64;; *) echo Darwin;; esac\n",
            "brew": "#!/bin/sh\nprintf '/mock/homebrew/%s\\n' \"$2\"\n",
            "xcrun": "#!/bin/sh\necho /mock/sdk\n",
            "cmake": "#!/usr/bin/env python3\nimport json, os, sys\n"
                     "with open(os.environ['GDL_TEST_CAPTURE'], 'w') as f: json.dump(sys.argv[1:], f)\n",
        }
        mocks = self.mock_tools(scripts)
        capture = self.root / "cmake-args.json"
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"], GDL_TEST_CAPTURE=str(capture),
                   CMAKE_COMMAND=str(mocks / "cmake"))
        for args in (("-DCMAKE_CXX_COMPILER=/custom/c++",),
                     ("-D", "CMAKE_C_COMPILER:FILEPATH=/custom/cc"),
                     ("--toolchain", "/custom/toolchain.cmake")):
            self.helper("config", *args, env=env)
            actual = json.loads(capture.read_text())
            self.assertFalse(any("/mock/homebrew/llvm" in arg for arg in actual), actual)
            self.assertTrue(all(arg in actual for arg in args))
        self.helper("config", env=env)
        self.assertIn("-DCMAKE_C_COMPILER=/mock/homebrew/llvm/bin/clang", json.loads(capture.read_text()))

    def test_macos_prep_installs_flexiblas_unless_another_cblas_is_chosen(self):
        mocks = self.mock_tools({
            "uname": "#!/bin/sh\ncase $1 in -m) echo arm64;; *) echo Darwin;; esac\n",
            "brew": "#!/bin/sh\nexit 0\n",
        })
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"])
        for provider, expected in ((None, True), ("flexiblas", True), ("GSL", False), ("ACCELERATE", False)):
            settings = {"GSL_CBLAS_PROVIDER": provider} if provider else {}
            output = self.helper("prep_dryrun", env=dict(env, **settings))
            command = next(line for line in output.splitlines() if line.startswith("brew install "))
            self.assertEqual("flexiblas" in command.split(), expected, provider)

    def test_graphics_and_python_defaults(self):
        mocks = self.mock_tools({
            "brew": "#!/bin/sh\nprintf '/mock/homebrew/%s\\n' \"$2\"\n",
            "xcrun": "#!/bin/sh\necho /mock/sdk\n",
            "cmake": "#!/usr/bin/env python3\nimport json, os, sys\n"
                     "with open(os.environ['GDL_TEST_CAPTURE'], 'w') as f: json.dump(sys.argv[1:], f)\n",
        })
        capture = self.root / "cmake-args.json"
        env = dict(self.env, PATH=str(mocks) + os.pathsep + self.env["PATH"], GDL_TEST_CAPTURE=str(capture),
                   CMAKE_COMMAND=str(mocks / "cmake"))
        for system, python in (("Darwin", "OFF"), ("Linux", "ON")):
            self.mock_tools({"uname": "#!/bin/sh\ncase $1 in -m) echo arm64;; *) echo " + system + ";; esac\n"})
            for settings, expected in (({}, ("-DX11=OFF", "-DWXWIDGETS=ON", "-DPYTHON=" + python)),
                                       ({"DEPS": "headless"}, ("-DX11=OFF", "-DWXWIDGETS=OFF")),
                                       ({"WITH_X11": "ON", "WITH_PYTHON": "ON"}, ("-DX11=ON", "-DPYTHON=ON"))):
                with self.subTest(system=system, settings=settings):
                    self.helper("config", env=dict(env, **settings))
                    actual = json.loads(capture.read_text())
                    for option in expected:
                        self.assertIn(option, actual)

    def test_resource_copy_fallback_refreshes(self):
        data = self.build / "share/gnudatalanguage"
        for name in ("lib", "resource"):
            (data / name).mkdir(parents=True)
        self.helper("config", env=dict(self.env, BUILD_DIR="."))
        self.helper("build")
        self.assertEqual((data / "lib/example.pro").read_text(), "procedure version 1")
        (self.source / "src/pro/example.pro").write_text("procedure version 2")
        self.helper("build")
        self.assertEqual((data / "lib/example.pro").read_text(), "procedure version 2")
        self.assertFalse((data / "lib").is_symlink())

    def test_launcher_literal_paths(self):
        # Compile in a normal path, then generate launchers for a target path
        # containing shell metacharacters. CMake itself cannot build in every
        # such path, but the launcher must not interpret them as shell code.
        self.helper("config")
        self.helper("build")
        binary = self.build / "custom bin/gdl"
        unusual = self.root / "a 'quoted' $dollar `name`"
        unusual.mkdir()
        shutil.copy2(binary, unusual / "gdl")
        script = (self.build / "run-gdl").read_text().replace(str(binary), str(unusual / "gdl"))
        launcher = unusual / "run-gdl"
        launcher.write_text(script)
        launcher.chmod(0o755)
        self.assertIn("CONFIG=Debug\n", self.command([launcher]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
