# Third-party (vendored) library versions

Tracks what's actually bundled under `Source/` against upstream, so drift gets
caught before it turns into a CVE backlog or a silent ARM64EC-class bug.

**Update this before every release** (same step as bumping
`FREEIMAGE_RELEASE_SERIAL` in `Source/FreeImage.h`, see
[danoli3/FreeImage#112](https://github.com/danoli3/FreeImage/pull/112)) and
whenever a vendored library is re-synced.

| Library | Path | Vendored | Latest upstream | Status | Version source |
|---|---|---|---|---|---|
| OpenEXR | `Source/OpenEXR` | 3.3.14 | 3.4.15 (2026-08-21) | behind 3.4.x — 3.3.14 IDManifest fixes are in; ARM64EC SIMD guards are a local patch ([#2344](https://github.com/AcademySoftwareFoundation/openexr/pull/2344) is not in upstream 3.3.x) | `OpenEXRConfig.h` |
| Imath | `Source/Imath` | 3.1.12 | 3.2.3 (2026-08-20) | outdated | `ImathConfig.h` |
| LibDeflate | `Source/LibDeflate` | 1.18 | 1.26 (2026-08-22) | outdated | `libdeflate.h` |
| LibJPEG (IJG) | `Source/LibJPEG` | 10.0 (25-Jan-2026) | 10.0 (25-Jan-2026) | up to date | `jversion.h` |
| LibOpenJPEG | `Source/LibOpenJPEG` | 2.5.4 | 2.5.4 | up to date | `opj_config_private.h` |
| LibPNG | `Source/LibPNG` | 1.6.58 | 1.6.58 | up to date | `png.h` |
| LibRawLite | `Source/LibRawLite` | 0.22.2 | 0.22.2 (2026-07-16) | up to date | `libraw/libraw_version.h` |
| LibTIFF4 | `Source/LibTIFF4` | 4.7.1 | 4.7.2 | partial — `TIFFLIB_VERSION_STR` is still 4.7.1; PixarLog and OJPEG have the 4.7.2 integer-overflow hardening only | `tiffvers.h` |
| LibWebP | `Source/LibWebP` | 1.6.0 (30-Jun-2025) | 1.6.0 | up to date | `NEWS` |
| LibJXR | `Source/LibJXR` | unversioned jxrlib snapshot | n/a — unmaintained (last active fork: [4creators/jxrlib](https://github.com/4creators/jxrlib)) | no upstream to track | `README` |
| ZLib | `Source/ZLib` | 1.3.2 | 1.3.2 | up to date | `zlib.h` |

Vendored column rechecked against `Source/` on 2026-09-22. The upstream
column is still the 2026-09-14 pass (IJG libjpeg checked against ijg.org,
which has no repo), except libpng 1.6.58 and libtiff 4.7.2 were confirmed
still current.

## Known follow-ups

- OpenEXR is 3.3.14, still behind upstream 3.4.15. The ARM64EC guards are
  already in `IlmImf/ImfSimd.h`, `OpenEXRCore/internal_dwa_simd.h`, and
  `OpenEXRCore/internal_zip.c`. Upstream 3.3.x does not have that fix, so
  the next re-vendor has to re-apply it or move to a tag that includes it.
- LibTIFF is not a full 4.7.2 sync. The `TIFFDirectory` cursor move and the
  `tif_getmaxcompressionratio` decompression-bomb hook were left out.
