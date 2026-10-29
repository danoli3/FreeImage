#!/usr/bin/env python3
"""Exercise real nested configure/build processes under a non-English environment."""
import argparse
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tarfile
import tempfile

# The test also displays the opt-out's Chinese output. Python otherwise uses
# the runner's legacy code page when its stdout is redirected to the CI log.
sys.stdout.reconfigure(encoding="utf-8")

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--generator")
parser.add_argument("--exercise-msvc-environment", action="store_true",
                    help="exercise the MSVC environment branch with a non-MSVC host compiler")
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
environment = dict(os.environ, VSLANG="2052", DOTNET_CLI_UI_LANGUAGE="zh-CN")
use_msvc = os.name == "nt" or args.exercise_msvc_environment


def run(command, *, success=True):
    result = subprocess.run(command, env=environment, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)
    # Decode only for display; markers below are checked against the raw bytes.
    print(result.stdout.decode("utf-8", errors="replace"), end="", flush=True)
    if (result.returncode == 0) != success:
        raise RuntimeError(f"Unexpected exit status {result.returncode}: {command}")
    return result.stdout


with tempfile.TemporaryDirectory(prefix="freeimage codec output ") as temporary:
    root = Path(temporary)
    source = root / "fixture"
    source.mkdir()
    (source / "CMakeLists.txt").write_text('''cmake_minimum_required(VERSION 3.20)
project(CodecOutputProbe LANGUAGES CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/check-environment.cmake")
add_executable(CodecOutputProbe main.cpp other.cpp)
add_custom_target(check_environment ALL
    COMMAND "${CMAKE_COMMAND}" "-DEXPECTED_VSLANG=${EXPECTED_VSLANG}"
        "-DEXPECTED_DOTNET=${EXPECTED_DOTNET}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check-environment.cmake")
install(TARGETS CodecOutputProbe RUNTIME DESTINATION bin)
''', encoding="utf-8")
    (source / "check-environment.cmake").write_text('''if(NOT "$ENV{VSLANG}" STREQUAL "${EXPECTED_VSLANG}" OR
   NOT "$ENV{DOTNET_CLI_UI_LANGUAGE}" STREQUAL "${EXPECTED_DOTNET}")
    message(FATAL_ERROR "Unexpected child language: $ENV{VSLANG}/$ENV{DOTNET_CLI_UI_LANGUAGE}")
endif()
message(STATUS "Child language: $ENV{VSLANG}/$ENV{DOTNET_CLI_UI_LANGUAGE}")
''', encoding="utf-8")
    (source / "value.h").write_text("#define PROBE_VALUE 42\n", encoding="utf-8")
    (source / "main.cpp").write_text('''#include <cstdio>
#include "value.h"
int other();
int main() { std::printf("Probe value: %d\\n", PROBE_VALUE); return other(); }
''', encoding="utf-8")
    other_source = '''#ifdef _MSC_VER
__declspec(deprecated)
#endif
int old_function() { return 0; }
int other() { return old_function(); }
'''
    (source / "other.cpp").write_text(other_source, encoding="utf-8")
    archive = root / "Source" / "ThirdParty" / "probe.tar.gz"
    archive.parent.mkdir(parents=True)
    with tarfile.open(archive, "w:gz") as output:
        output.add(source, arcname="probe")
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    module = (repo / "cmake" / "FreeImageBundledCodecs.cmake").as_posix()
    simulate = "set(MSVC TRUE)" if args.exercise_msvc_environment else ""
    (root / "CMakeLists.txt").write_text(f'''cmake_minimum_required(VERSION 3.20)
project(CodecOutputParent LANGUAGES CXX)
{simulate}
include("{module}")
set(_codec_prefix "${{CMAKE_BINARY_DIR}}/installed")
freeimage_bundled_project(output_probe probe.tar.gz {digest}
    ARGS "-DEXPECTED_VSLANG=${{EXPECTED_VSLANG}}"
         "-DEXPECTED_DOTNET=${{EXPECTED_DOTNET}}")
''', encoding="utf-8")
    build = root / "build"
    generator = ["-G", args.generator] if args.generator else []
    # Reconfigure the same tree to catch stale language overrides on opt-out.
    for index, english in enumerate((True, False, True) if use_msvc else (True,)):
        expected = ("1033", "en-US") if english and use_msvc else ("2052", "zh-CN")
        command = ["cmake", "-S", str(root), "-B", str(build), *generator,
                   f"-DEXPECTED_VSLANG={expected[0]}", f"-DEXPECTED_DOTNET={expected[1]}",
                   "-DFREEIMAGE_CODEC_BUILD_JOBS=2"]
        if index:
            command.append(f"-DFREEIMAGE_CODEC_ENGLISH_MESSAGES={'ON' if english else 'OFF'}")
        run(command)
        build_command = ["cmake", "--build", str(build), "--config", "Release",
                         "--target", "freeimage_output_probe"]
        output = run(build_command)
        marker = f"Child language: {expected[0]}/{expected[1]}".encode()
        if marker not in output:
            raise RuntimeError("Nested build diagnostics were not forwarded")
        if os.name == "nt" and english and b"was declared deprecated" not in output:
            raise RuntimeError("The nested MSVC compiler did not emit an English warning")
        executable = build / "installed" / "bin" / ("CodecOutputProbe.exe" if os.name == "nt" else "CodecOutputProbe")
        if b"Probe value: 42" not in run([str(executable)]):
            raise RuntimeError("Initial build produced the wrong executable")
        # Ninja must still recognize /showIncludes after language normalization.
        extracted = build / "codec-build" / "output_probe" / "src" / "freeimage_output_probe"
        (extracted / "value.h").write_text("#define PROBE_VALUE 43\n", encoding="utf-8")
        binary_dir = (build / "codec-build" / "output_probe" / "build-msvc-en"
                      if english and use_msvc else Path(f"{extracted}-build"))
        wrapper = ["cmake", f"-DFREEIMAGE_CODEC_BINARY_DIR={binary_dir}",
                   "-DFREEIMAGE_CODEC_CONFIG=Release", "-DFREEIMAGE_CODEC_BUILD_JOBS=2",
                   f"-DFREEIMAGE_CODEC_ENGLISH_MESSAGES={'ON' if english and use_msvc else 'OFF'}",
                   "-P", str(repo / "cmake" / "BuildBundledCodec.cmake")]
        run(wrapper)
        compiled = binary_dir / ("Release/CodecOutputProbe.exe" if os.name == "nt" and args.generator != "Ninja" else "CodecOutputProbe")
        if os.name == "nt" and args.generator == "Ninja":
            compiled = binary_dir / "CodecOutputProbe.exe"
        if b"Probe value: 43" not in run([str(compiled)]):
            raise RuntimeError("Header dependency was lost after changing the diagnostic language")
        (extracted / "other.cpp").write_text("#error FREEIMAGE_CODEC_EXPECTED_FAILURE\n", encoding="utf-8")
        if b"FREEIMAGE_CODEC_EXPECTED_FAILURE" not in run(wrapper, success=False):
            raise RuntimeError("Compiler errors were not forwarded")
        # Restore the fixture for the next configure/build of the same tree.
        (extracted / "other.cpp").write_text(other_source, encoding="utf-8")
        (extracted / "value.h").write_text("#define PROBE_VALUE 42\n", encoding="utf-8")
        print(f"English messages {'ON' if english else 'OFF'}: configure, build, header rebuild and failure propagation passed")
