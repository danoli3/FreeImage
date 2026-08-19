#!/bin/bash
# Bumps one of FreeImage's bundled third-party libraries to a new upstream
# release: downloads the release tarball, merges it into the vendored
# source directory, and regenerates the matching CMakeLists.txt source
# file list.
#
# This only handles the mechanical part of a vendor bump. You still need
# to: build, run the test suite, check for upstream API/behavior changes
# that affect Source/FreeImage/Plugin*.cpp, and check `git status` for
# new/removed top-level source directories that this library's config
# doesn't yet list (update scripts/vendor/libs/<name>.sh if so).
#
# Usage:
#   scripts/vendor/bump-lib.sh webp --version 1.6.0
#   scripts/vendor/bump-lib.sh libraw --version 0.22.1
#   scripts/vendor/bump-lib.sh <lib> --version <ver> --url <override-url>
#
# Per-library config (source subdirs to merge, root metadata files to
# copy, the CMakeLists.txt variable name, and the download URL template)
# lives in scripts/vendor/libs/<lib>.sh - see that directory for the full
# list of supported libs and notes on any that need manual handling
# instead (OpenEXR, jxrlib).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

usage() {
	echo "Usage: $0 <lib> --version <version> [--url <override-url>]" >&2
	echo "Known libs:" >&2
	for f in "$SCRIPT_DIR"/libs/*.sh; do
		echo "  $(basename "$f" .sh)" >&2
	done
	exit 1
}

[ $# -ge 1 ] || usage
LIB="$1"; shift

VERSION=""
URL_OVERRIDE=""
while [ $# -gt 0 ]; do
	case "$1" in
		--version) VERSION="$2"; shift 2 ;;
		--url) URL_OVERRIDE="$2"; shift 2 ;;
		*) echo "Unknown argument: $1" >&2; usage ;;
	esac
done

CONFIG="$SCRIPT_DIR/libs/$LIB.sh"
[ -f "$CONFIG" ] || { echo "Unknown lib '$LIB' (no $CONFIG)" >&2; usage; }
# shellcheck source=/dev/null
source "$CONFIG"

if [ -n "$URL_OVERRIDE" ]; then
	URL="$URL_OVERRIDE"
elif [ -n "${URL_TEMPLATE:-}" ]; then
	[ -n "$VERSION" ] || { echo "--version is required" >&2; exit 1; }
	URL="${URL_TEMPLATE//\{version\}/$VERSION}"
else
	echo "No URL_TEMPLATE in $CONFIG and no --url given - this library" >&2
	echo "may need manual handling instead. See scripts/vendor/libs/$LIB.sh for notes." >&2
	exit 1
fi

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

echo "Downloading $URL"
curl -sL -o "$TMPDIR/download.tar.gz" "$URL"
mkdir "$TMPDIR/extracted"
tar xzf "$TMPDIR/download.tar.gz" -C "$TMPDIR/extracted"

EXTRACTED_ROOT=$(find "$TMPDIR/extracted" -mindepth 1 -maxdepth 1 -type d)
COUNT=$(echo "$EXTRACTED_ROOT" | wc -l | tr -d ' ')
[ "$COUNT" = "1" ] || { echo "Expected exactly one top-level dir in the tarball, found $COUNT" >&2; exit 1; }

VENDOR_PATH="$REPO_ROOT/$VENDOR_DIR"

echo "Merging source subdirs into $VENDOR_DIR"
# Merge-copy (overwrite matching names, keep anything local-only like
# FreeImage's own .vcxproj files) rather than deleting first, so files
# that only exist in this repo (not upstream) are never removed. Files
# upstream has since removed will linger locally - check `git status`
# after running and `git rm` anything that's genuinely gone upstream.
for sub in $SOURCE_SUBDIRS; do
	if [ "$sub" = "." ]; then
		SRC="$EXTRACTED_ROOT"
		DST="$VENDOR_PATH"
	else
		SRC="$EXTRACTED_ROOT/$sub"
		DST="$VENDOR_PATH/$sub"
	fi
	if [ ! -d "$SRC" ]; then
		echo "Expected subdir '$sub' not found in upstream release - library layout may have changed, check manually." >&2
		exit 1
	fi
	mkdir -p "$DST"
	cp -R "$SRC/." "$DST/"
	echo "  merged into ${DST#"$REPO_ROOT"/}"
done

if [ -n "${ROOT_FILES:-}" ]; then
	echo "Updating root metadata files"
	for f in $ROOT_FILES; do
		if [ -f "$EXTRACTED_ROOT/$f" ]; then
			cp "$EXTRACTED_ROOT/$f" "$VENDOR_PATH/$f"
			echo "  updated $VENDOR_DIR/$f"
		else
			echo "  note: upstream no longer ships '$f' at the root (may have been renamed - check manually)"
		fi
	done
fi

if [ -n "${CMAKE_VAR:-}" ]; then
	echo "Regenerating $CMAKE_VAR in CMakeLists.txt"
	"$SCRIPT_DIR/regen-cmake-list.sh" "$REPO_ROOT/CMakeLists.txt" "$CMAKE_VAR" "$VENDOR_PATH" "$SOURCE_SUBDIRS"
fi

echo
echo "Done. Remaining manual steps:"
echo "  1. Check 'git status' for new/removed top-level dirs this script doesn't know"
echo "     about yet (update scripts/vendor/libs/$LIB.sh's SOURCE_SUBDIRS if so)."
echo "  2. Configure + build: cmake -S . -B build -DBUILD_TESTS=OFF && cmake --build build"
echo "  3. Functionally test load/save round-trips for this format."
echo "  4. Check the release notes for API/behavior changes affecting"
echo "     Source/FreeImage/Plugin*.cpp."
