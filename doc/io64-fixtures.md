# Large-file test data releases

The `Large-file test fixture assets` workflow publishes synthetic images under
`test-fixtures-io64-v1`, a **prerelease for test data**, separate from FreeImage
software versions. Merging the generator/workflow PR into `master` validates the
bundle on Linux, macOS and Windows, then publishes it once. Pull requests only
validate packaging; they cannot create tags or releases.

Release assets are `freeimage-io64-fixtures.tar.gz`, `manifest.json`,
`SHA256SUMS` and `README.md`. No binary images are checked into the repository.
The sparse archive contains real TIFF files with data beyond 2 GiB and 4 GiB,
including classic TIFF, BigTIFF, both byte orders, and high-offset Exif and
thumbnail data. Malformed examples include truncated headers/directories/strips,
invalid offsets, overflowing directory counts and zero dimensions. Small PNG
and BMP files exercise fallback loaders. Nested multilingual paths contain
both valid and malformed inputs.

The archive is generated directly from sparse extents, without creating large
files on the publisher's disk. Its manifest records intended decode/reject
results, pixel colors, logical lengths and hashes for the stored extents.
These are test expectations for the separate loader regression work; publishing
the corpus does not change FreeImage or assert that every existing loader
already rejects every malformed input correctly.

The initial corpus has 91 cases, roughly 30 GiB of logical data in a small
archive. Four malformed strip/offset cases return a bitmap on the unmodified
PR #197 head; their `known_behavior` entries record the tested commit and API.
Their intended result stays `reject`. Publishing these fixtures does not hide
those failures or include unrelated codec fixes.

After publication, Actions and developers can download the archive and checksum
file with `gh release download`:

```sh
gh release download test-fixtures-io64-v1 --repo danoli3/FreeImage \
  --pattern freeimage-io64-fixtures.tar.gz --pattern SHA256SUMS
python scripts/tests/io64-fixtures.py extract freeimage-io64-fixtures.tar.gz \
  --sha256-file SHA256SUMS --output-dir io64-fixtures
```

The Python extractor checks the archive hash, preserves holes, marks files sparse
on Windows, and verifies payload extents after extraction. Use a fresh output
directory. The extracted files have many GiB of logical size but use little
physical disk space. GNU tar and macOS bsdtar also support this archive. Avoid
tools that materialize the holes. Future loader CI should pin the archive's
SHA-256 from the reviewed bundle, in addition to using the versioned tag.

Re-running the workflow for a published version checks identical checksums and
leaves that release untouched. Different contents require a new fixture tag,
such as `test-fixtures-io64-v2`, selected through **Run workflow** on `master`.
Software release tags such as `3.19.29` are rejected. Incomplete drafts or tags
without releases require inspection instead of automatic replacement. Fixture
tags do not match the numeric software-release build triggers and are never
marked as the latest software release.

Local verification and generation require only Python 3.10 or newer:

```sh
python scripts/tests/test-io64-fixtures.py
python scripts/tests/io64-fixtures.py build --output-dir /tmp/io64-assets
```

All inputs are created from scratch and released under CC0-1.0. There are no
customer images, downloaded third-party samples or committed large binaries.
