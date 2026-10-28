#!/usr/bin/env python3
"""Fixture packaging tests; no FreeImage build or multi-gigabyte allocations."""

import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile
import unittest

spec = importlib.util.spec_from_file_location("fixtures", Path(__file__).with_name("io64-fixtures.py"))
fixtures = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fixtures)


class FixtureTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.assets = self.root / "assets"
        self.records = fixtures.build(self.assets)
        self.archive = self.assets / fixtures.ARCHIVE
        self.digest = hashlib.sha256(self.archive.read_bytes()).hexdigest()

    def unpack(self):
        destination = self.root / "unpacked"
        records = fixtures.extract(self.archive, destination, self.digest)
        self.assertEqual(records, self.records)
        return destination

    def test_deterministic_assets(self):
        second = self.root / "second"
        fixtures.build(second)
        for asset in self.assets.iterdir():
            self.assertEqual(asset.read_bytes(), (second / asset.name).read_bytes())
        self.assertLess(self.archive.stat().st_size, 128 * 1024)

    def test_checksums(self):
        for line in (self.assets / "SHA256SUMS").read_text().splitlines():
            digest, name = line.split(maxsplit=1)
            self.assertEqual(digest, hashlib.sha256((self.assets / name).read_bytes()).hexdigest())

    def test_round_trip_all_sparse_payloads_and_multilingual_names(self):
        destination = self.unpack()
        paths = [record["path"] for record in self.records]
        self.assertEqual(len(paths), len(set(paths)))
        self.assertGreaterEqual(len(paths), 80)
        self.assertEqual(sum(path.startswith("paths/") for path in paths), 60)
        self.assertIn("paths/emoji-\U0001f9ea/valid image.tif", paths)
        for record in self.records:
            path = destination / record["path"]
            self.assertEqual(path.stat().st_size, record["logical_size"])
            if os.name != "nt":
                self.assertLess(path.stat().st_blocks * 512, 1024 * 1024)
            elif record["logical_size"] > 2**32:
                import ctypes
                from ctypes import wintypes
                attributes = ctypes.windll.kernel32.GetFileAttributesW
                attributes.argtypes = [wintypes.LPCWSTR]
                attributes.restype = wintypes.DWORD
                self.assertTrue(attributes(str(path)) & 0x200, "Windows file must be sparse")

    def test_sparse_holes_are_zero_and_last_byte_survives(self):
        destination = self.unpack()
        record = next(r for r in self.records if r["path"] == "valid/bigtiff-strip-above-4g.tif")
        with (destination / record["path"]).open("rb") as file:
            file.seek(0x80000000)
            self.assertEqual(file.read(32), bytes(32))
            file.seek(-3, 2)
            self.assertEqual(file.read(), bytes(fixtures.RGB))

    def test_boundary_and_malformed_coverage(self):
        large = [r for r in self.records if r["logical_size"] > 2**32]
        self.assertTrue(any(r["expect"] == "decode" for r in large))
        self.assertTrue(any(r["expect"] == "reject" for r in large))
        self.assertEqual({r["format"] for r in self.records}, {"TIFF", "PNG", "BMP"})
        for fmt in ("TIFF", "PNG", "BMP"):
            self.assertEqual({r["expect"] for r in self.records if r["format"] == fmt}, {"decode", "reject"})
        self.assertTrue(any("exif_user_comment" in r for r in self.records))
        self.assertTrue(any("big-endian" in r["path"] for r in self.records))
        known = [r for r in self.records if "known_behavior" in r]
        self.assertEqual(len(known), 4)
        self.assertTrue(all(r["expect"] == "reject" for r in known))

    def test_corruption_rejected_before_creating_files(self):
        self.archive.write_bytes(self.archive.read_bytes() + b"corruption")
        destination = self.root / "unpacked"
        with self.assertRaisesRegex(ValueError, "SHA-256"):
            fixtures.extract(self.archive, destination, self.digest)
        self.assertFalse(destination.exists())

    def test_existing_output_is_never_overwritten(self):
        destination = self.unpack()
        with self.assertRaises(FileExistsError):
            fixtures.extract(self.archive, destination, self.digest)

    def test_rejects_software_release_tags(self):
        for tag in ["3.19.29", "v3.19.29", "test-fixtures-io64-v0", "../v1", "v1\n"]:
            with self.subTest(tag=tag), self.assertRaises(ValueError):
                fixtures.build(self.root / "invalid", tag)

    def test_rejects_unsafe_member_names(self):
        for name in ["../outside", "/absolute", "C:/outside", "dir\\outside", "a/../../outside"]:
            with self.subTest(name=name), self.assertRaises(ValueError):
                fixtures.safe_path(self.root, name)

    def test_rejects_links_in_archive(self):
        archive = self.root / "link.tar.gz"
        with tarfile.open(archive, "w:gz") as tar:
            member = tarfile.TarInfo("link"); member.type = tarfile.SYMTYPE; member.linkname = "../outside"
            tar.addfile(member)
        with self.assertRaisesRegex(ValueError, "Unsupported archive member"):
            fixtures.extract(archive, self.root / "unsafe", hashlib.sha256(archive.read_bytes()).hexdigest())

    def test_native_tar_compatibility(self):
        if os.name == "nt":
            self.skipTest("Windows uses the extractor that explicitly marks files sparse")
        tar = shutil.which("tar")
        if not tar:
            self.skipTest("No system tar")
        destination = self.root / "native"
        destination.mkdir()
        subprocess.run([tar, "-xzf", str(self.archive), "-C", str(destination)], check=True)
        manifest = json.loads((destination / "manifest.json").read_text(encoding="utf-8"))
        for record in manifest["fixtures"]:
            path = destination / record["path"]
            self.assertEqual(path.stat().st_size, record["logical_size"])
            self.assertLess(path.stat().st_blocks * 512, 1024 * 1024)
            with path.open("rb") as file:
                for extent in record["extents"]:
                    file.seek(extent["offset"])
                    self.assertEqual(hashlib.sha256(file.read(extent["length"])).hexdigest(), extent["sha256"])


if __name__ == "__main__":
    unittest.main(verbosity=2)
