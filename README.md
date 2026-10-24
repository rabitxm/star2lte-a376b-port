# A376B One UI → Galaxy S9+ (star2lte)

Porting project for Galaxy S9+ Exynos 9810: SM-G965N is the primary target,
SM-G965F is tested first. Final goal: Galaxy A37 SM-A376B One UI 9 / Android 17.
This repository shares source patches, diagnostics and evidence for assistance.
It does not contain a working Android 17 ROM or firmware images.

**Current status — 24 October 2026:** M1 (S22/Android 16) boots on G965F.
M2 (A37 A376BXXS5AZH4, Android 16 / SDK36.1 / OneUI80500) still bootloops,
but probe13 now captures the explicit restart trigger. All30 retained boots
reach second-stage init;23 capture vendor BoringSSL self-test SIGABRT and its
reboot-on-failure message,22 retain the shutdown command
`reboot,boringssl-self-check-failed`. The earlier policy-load observation
boundary was a recorder access failure, demonstrated on M1 and M2.

The native A37 **runtime APEX linker**, in addition to its bootstrap linker,
reproduces `arc4random data MADV_WIPEONFORK failed: Invalid argument` / exit134
on the pinned DS-ACK kernel. Attempt50's M1 bootstrap overlay does not replace
that runtime APEX. A separate linkerconfig execution error remains to be
resolved; init A37 names the APEX path, so the absence of a system-bin symlink
alone is not a demonstrated cause. No self-test was bypassed or kernel changed.
Both diagnostic BOOT images are rolled back: M1 is booted, M2 is in TWRP.
See [probe13 results](docs/probe13-report.md) and
[the assistance brief](docs/HELP-WANTED.md) for evidence and remaining limits.

## Source reproduction

The existing build system is
[Eend15/Unofficial-Un1ca-9810](https://github.com/Eend15/Unofficial-Un1ca-9810),
branch Android-16, pinned commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1`. The 88 active text patches in
`patches/fork/series` extend that fork; they are numbered through 0098.

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
