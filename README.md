# A376B One UI → Galaxy S9+ (star2lte)

Porting project for Galaxy S9+ Exynos 9810: SM-G965N is the primary target,
SM-G965F is tested first. Final goal: Galaxy A37 SM-A376B One UI 9 / Android 17.
This repository shares source patches, diagnostics and evidence for assistance.
It does not contain a working Android 17 ROM or firmware images.

**Current status — 24 October 2026:** M1 (S22/Android16) boots on G965F. M2 A37 A376BXXS5AZH4 (Android16 / SDK36.1 / OneUI80500) reaches Android under Enforcing with the documented diagnostic controls. The latest tests retain aconfig data across reboots, make platform aconfigd exit0, and produce rear/front preview buffers plus JPEG photos that fully decode on the PC. The camera ABI, OneUI80500 enum and missing native32 GLES1 fixes are now in the fork patch series.

A complete ROM containing the whole tested combination has not been packaged. Video/audio, connectivity, OTA/CACHE lifecycle, fresh-data ADB defaults/switches and remaining hardware tests are open. All live diagnostic edits and BOOT were rolled back and verified; M2 remains in TWRP. M1/G965N were untouched, production kernel selection and retained releases stay unchanged. See [camera and persistent metadata report](docs/m2-camera-metadata-fixes.md), [kernel diagnostic provenance](docs/m2-runtime-kernel-fix.md) and [current assistance brief](docs/HELP-WANTED.md).

## Source reproduction

The existing build system is
[Eend15/Unofficial-Un1ca-9810](https://github.com/Eend15/Unofficial-Un1ca-9810),
branch Android-16, pinned commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1`. The 91 active text patches in
`patches/fork/series` extend that fork; they are numbered through 0101.

```bash
bash tools/prepare_fork.sh
# Read docs/build-environment.md and docs/m2-build.md before building.
# Initialize dependencies / acquire firmware separately, as the fork requires.
```

The helper refuses an existing destination and only clones/applies patches.
Builds remain in the fork's `rom/` checkout. Donor S22 configuration is retained;
A37 Android16 profiles are added alongside it. No firmware or device operations
are performed by the preparation helper. Source provenance is recorded in
sources.lock.json, firmware.lock.json and tools.lock.json.

## Fixed project decisions

- Hybrid Vendor VNDK33 from the fork plus correct F/N hardware and RIL blobs;
  use the fork's ABI/hash-locked radio bridge when needed.
- Original duhansysl DS-ACK V1.12 08/05/2026 Enforcing-OneUI7 without KernelSU;
  kernel changes need specific runtime evidence and a separate review.
- Preserve modem, bootloader, EFS and CPEFS. G965N remains unflashed.
- Default Auto Blocker off, USB debugging on, USB ADB without RSA authorization
  on development builds. Preserve functional Settings switches and descriptions;
  seed defaults only for new settings, with no boot loop overriding user choices.
- Retain completed M1/M2/M4/M5 artifacts independently; no overwrite on rebuild.
  Artifacts stay local and are never published in this source repository.

## Reading guide

- [Assistance brief / current evidence](docs/HELP-WANTED.md)
- [Working rules](AGENTS.md), [plan](docs/PLAN.md), [chronological status](STATUS.md)
- [Fork map](docs/repo-map.md), [source provenance](docs/sources.md)
- [Patch matrix](docs/patch-matrix.md), [bugs and logs](docs/bugs.md)
- [M2 build history](docs/m2-build.md), [hardware differences](docs/hw-diff.md)
- [M1 fresh boot report](docs/m1-fresh-report.md), [ADB rule](docs/adb-defaults.md)
- [Diagnostic sources and test limits](diagnostics/README.md)
- [Selected shareable evidence](support/evidence/)

All build/runtime claims are scoped to the documented donor, target and artifact.
Older chronological notes may describe superseded decisions; current AGENTS.md
and the dated assistance brief explain the active state. A successful G965F
boot does not establish G965N compatibility or Android17 support.

## Sharing

Proprietary Samsung firmware, generated output, ROM/BOOT images, EFS dumps,
credentials, signing keys and raw device logs are excluded. The exported GitHub
snapshot substitutes physical device identifiers with role aliases and includes
only allowlisted text/source files. Upstream code and kernel retain their
respective licenses and credits; this handoff does not relicense third-party code.

## Runtime kernel diagnostic — 24 October 2026

Probe13 reproduced the M2 vendor BoringSSL restart trigger as an A37 runtime
APEX linker abort: `arc4random data MADV_WIPEONFORK failed: Invalid argument`
(exit134), followed by init's `reboot_on_failure`. A separately built, narrowly
scoped DS-ACK V1.12 backport passed real fork/zero-fill/KEEPONFORK checks and
made the exact A37 linker `--list` exit0 on the M1 G965F control under enforcing.
M1 has been restored to original BOOT. The audited diagnostic BOOT cleared both vendor/system BoringSSL self-tests on
M2. Subsequent tests reached Android, retained metadata through reboot and captured
rear/front photos. All test writes have been rolled back; a complete production
M2 artifact and hardware acceptance remain pending.
This is not a production kernel/package change. Details and hashes:
[diagnostic report](docs/m2-runtime-kernel-fix.md).

The M2 test now clears the previous BoringSSL/linkerconfig failures: both
BoringSSL self-tests exit0 and linkerconfig generates its config. It then reaches
a separate A37 Tethering gate: `Android 25Q4 requires kernel 5.10`, causing
`bpfloader` exit7 and `reboot,bpfloader-failed` on DS-ACK4.9.337. The diagnostic
BOOT was restored to its original hash after capture. The older retained S22 Tethering APEX then cleared that loader gate, and
the later V6/metadata test reached Welcome. Hardware and module acceptance
remain pending; see the updated runtime and metadata reports.
