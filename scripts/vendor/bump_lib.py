#!/usr/bin/env python3
"""
Bumps one of FreeImage's bundled third-party libraries to a new upstream
release: downloads the release tarball, wholesale-replaces the vendored
source directories, and regenerates the matching CMakeLists.txt source
file list.

This only handles the mechanical part of a vendor bump (fetch + replace +
regenerate the file list). You still need to: build, run the test suite,
check for upstream API/behavior changes that affect Source/FreeImage/Plugin*.cpp,
and check for new/removed/renamed top-level source directories that this
library's entry in libs.json doesn't yet know about.

Usage:
    python3 scripts/vendor/bump_lib.py webp \\
        --url https://github.com/webmproject/libwebp/archive/refs/tags/v1.6.0.tar.gz \\
        --version 1.6.0

    # or, once libs.json has a "url_template" for a library:
    python3 scripts/vendor/bump_lib.py webp --version 1.6.0

See scripts/vendor/libs.json for the per-library config (source subdirs to
replace, root metadata files to copy, and the CMakeLists.txt variable name
each library's source list is stored in).
"""
import argparse
import json
import re
import subprocess
import sys
import tarfile
import urllib.request
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
LIBS_JSON = Path(__file__).resolve().parent / "libs.json"


def load_libs():
    with open(LIBS_JSON) as f:
        return json.load(f)


def download_and_extract(url, dest_dir):
    tarball = dest_dir / "download.tar.gz"
    print(f"Downloading {url}")
    urllib.request.urlretrieve(url, tarball)
    with tarfile.open(tarball) as tf:
        tf.extractall(dest_dir)
    # the tarball has exactly one top-level directory; find it
    entries = [p for p in dest_dir.iterdir() if p.is_dir()]
    if len(entries) != 1:
        sys.exit(f"Expected exactly one extracted top-level dir, found: {entries}")
    return entries[0]


def replace_subdirs(extracted_root, vendor_dir, subdirs):
    # Merge-copy (overwrite matching names, keep anything local-only like
    # FreeImage's own .vcxproj files) rather than rm -rf + copy, so files
    # that only exist in this repo (not upstream) are never deleted.
    # Files upstream has since removed will linger locally - `git status`
    # after running will show what upstream's diff touched; delete stale
    # files by hand if a `git rm` is warranted.
    for sub in subdirs:
        src = extracted_root if sub == "." else extracted_root / sub
        dst = vendor_dir if sub == "." else vendor_dir / sub
        if not src.exists():
            sys.exit(f"Expected subdir '{sub}' not found in upstream release - "
                      f"library layout may have changed, check manually.")
        dst.mkdir(parents=True, exist_ok=True)
        subprocess.run(f"cp -R '{src}/.' '{dst}/'", shell=True, check=True)
        print(f"  merged into {dst.relative_to(REPO_ROOT)}")


def copy_root_files(extracted_root, vendor_dir, root_files):
    for name in root_files:
        src = extracted_root / name
        if src.exists():
            dst = vendor_dir / name
            subprocess.run(["cp", str(src), str(dst)], check=True)
            print(f"  updated {dst.relative_to(REPO_ROOT)}")
        else:
            print(f"  note: upstream no longer ships '{name}' at the root "
                  f"(may have been renamed, e.g. README -> README.md - check manually)")


def collect_source_files(vendor_dir, subdirs, extensions):
    files = []
    for sub in subdirs:
        base = vendor_dir if sub == "." else vendor_dir / sub
        for ext in extensions:
            files.extend(base.rglob(f"*{ext}"))
    rel = sorted(
        "./" + str(p.relative_to(REPO_ROOT)) for p in files
    )
    return rel


def format_cmake_list(var_name, files, indent="        ", width=96):
    lines = [f"    set({var_name}"]
    cur = indent
    for f in files:
        candidate = cur + f + " "
        if len(candidate) > width:
            lines.append(cur.rstrip())
            cur = indent + f + " "
        else:
            cur = candidate
    if cur.strip():
        lines.append(cur.rstrip())
    lines.append("    )")
    return "\n".join(lines)


def splice_cmake_block(cmake_text, var_name, new_block):
    """Replaces `set(<var_name> ... )` (balanced parens) with new_block."""
    marker = f"set({var_name}"
    start = cmake_text.find(marker)
    if start == -1:
        sys.exit(f"Could not find 'set({var_name}' in CMakeLists.txt")
    # find the matching close paren by depth counting from marker's own '('
    depth = 0
    i = start + len("set(") - 1  # position of the '(' after 'set'
    for j in range(i, len(cmake_text)):
        if cmake_text[j] == "(":
            depth += 1
        elif cmake_text[j] == ")":
            depth -= 1
            if depth == 0:
                end = j + 1
                break
    else:
        sys.exit(f"Could not find matching ')' for set({var_name}")
    # extend start back to the beginning of its line for clean replacement
    line_start = cmake_text.rfind("\n", 0, start) + 1
    return cmake_text[:line_start] + new_block + cmake_text[end:]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("lib", help="library key in scripts/vendor/libs.json (e.g. webp, libpng, libtiff)")
    parser.add_argument("--version", required=True, help="new version, e.g. 1.6.0")
    parser.add_argument("--url", help="override the download URL (else built from libs.json url_template)")
    parser.add_argument("--skip-cmake", action="store_true", help="only replace files, don't touch CMakeLists.txt")
    args = parser.parse_args()

    libs = load_libs()
    if args.lib not in libs:
        sys.exit(f"Unknown lib '{args.lib}'. Known: {', '.join(libs)}")
    cfg = libs[args.lib]

    url = args.url or cfg["url_template"].format(version=args.version)
    vendor_dir = REPO_ROOT / cfg["vendor_dir"]

    import tempfile
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        extracted_root = download_and_extract(url, tmp)

        print(f"Replacing source subdirs under {vendor_dir.relative_to(REPO_ROOT)}")
        replace_subdirs(extracted_root, vendor_dir, cfg["source_subdirs"])

        if cfg.get("root_files"):
            print("Updating root metadata files")
            copy_root_files(extracted_root, vendor_dir, cfg["root_files"])

    if not args.skip_cmake and cfg.get("cmake_var"):
        print(f"Regenerating {cfg['cmake_var']} in CMakeLists.txt")
        files = collect_source_files(vendor_dir, cfg["source_subdirs"], cfg.get("extensions", [".c", ".h", ".cpp", ".hpp"]))
        new_block = format_cmake_list(cfg["cmake_var"], files)
        cmake_path = REPO_ROOT / "CMakeLists.txt"
        text = cmake_path.read_text()
        text = splice_cmake_block(text, cfg["cmake_var"], new_block)
        cmake_path.write_text(text)
        print(f"  wrote {len(files)} entries")

    print()
    print("Done. Remaining manual steps:")
    print("  1. Check `git status` for new/removed top-level dirs this script doesn't know")
    print("     about yet (update scripts/vendor/libs.json's source_subdirs if so).")
    print("  2. Configure + build: cmake -S . -B build -DBUILD_TESTS=OFF && cmake --build build")
    print("  3. Functionally test load/save round-trips for this format.")
    print("  4. Check the release notes for API/behavior changes affecting")
    print(f"     Source/FreeImage/Plugin*.cpp.")


if __name__ == "__main__":
    main()
