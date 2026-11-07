# SSD cleanup and M1/M2/M4 retention — 26/10/2026

The user requested keeping only the latest completed ROM build for M1, M2, and
M4, deleting older builds, freeing the primary SSD, and continuing the M4 build.
This document records that cleanup and the verified retained artifacts.

## Retained complete ROMs

- M1: `star2lte-1792729136-64VYgG` on the second NVMe. Flashable SHA256 is
  `8803ddf3d22670275b1bc07c08b385dd334e28a4703cf35a52cf74abd4655d6e`; target
  files SHA256 is
  `8691457682d5819ac1eeee3c99625f357867ee1e6621be86fe5279b174f2aa73`.
- M2: `star2lte-1792900635-m9nN7O` on the second NVMe. Flashable SHA256 is
  `f6e4a1069cca283ef32751abace1aedc23222d9ea5eb002d9a78e19475ee5e15`; target
  files SHA256 is
  `fe2a891ca884883290d41efa10338d4e2249469df23f45d5f78e40585386b708`.
- M4 had no completed release before cleanup. A ROM counts as complete only
  when its packaging run contains `.complete`; M4 attempt22 had no such marker.

The M1/M2 retained run manifests were verified previously with
`sha256sum -c SHA256SUMS`. No retained package was modified during this cleanup.

## Removed files

- The older M1 archive at
  `inputs/rom-releases/M1/20261021-g965f-adb17-ba4ed009bd98/`, whose full ROM
  SHA256 was `ba4ed009bd984ab647bd4316700081d8f6db54b592b0f005fac2f2fcc5d3aebb`,
  plus its eleven obsolete aliases under `rom/out/`. Its directory had no
  `.complete` marker and represents an older build than the retained M1 run.
- Fifteen failed M4 scratch trees under `rom/out/` (attempts02–15 and
  `profile0131`) and the failed attempt22 tree under `builds/`.
- Forty-two partial ZIP files within those failed M4 trees, plus the empty
  attempt22 release staging directory.

Build logs, source files, firmware inputs, patch history, replay fixtures, and
the M1/M2 completed packages were preserved. No phone was connected or changed.

## Verification and storage

Before cleanup, both retained M1 and M2 run directories existed and had
`.complete`; no M4 completed marker existed. The removed failed M4 directories
contained no `.complete` markers. The cleanup inventory is
`logs/20261026-storage-cleanup/cleanup-inventory.json`.

Primary SSD free space increased from 10.96 GiB to 190.71 GiB (179.75 GiB
reclaimed). After deleting attempt33 scratch, the second NVMe has about190 GiB free; the primary SSD has about190 GiB free. M4 attempt23 failed at the optional Package Block List patch; attempt24 passed
that gate and Battery Health but failed at the legacy FOTA suggestion path. Attempt24–31
scratch trees were removed after preserving their logs. Attempt28 failed at the
paired SystemUI network-speed patch (`BasicRune.smali:644`), attempt29 at Smart
View (`QpRune.smali:308`), attempt30 at DAAgent (`DAUtility.smali:1947`), and
attempt31 at DeKnox (`TAProxy.smali`). Their build trees were removed only after
confirming no `.complete` and an empty M4 release root. Attempt32 failed at Device
Image and its 12.45GB scratch was removed after preserving logs. Attempt33 then
passed Device Image and failed at the optional Hide My Applist hunks; its 12.51GB
scratch was removed after confirming no `.complete` marker and an empty M4 release
root. Attempt34 failed in the optional Keybox module; its 12.51GB scratch was removed after confirming no `.complete` marker and an empty M4 release root. Attempt35 was interrupted before module execution; its 24KiB partial scratch was removed with logs preserved. Attempt36 uses the mounted second NVMe for its build tree, temp images, and M4 release staging; repository/logs/source remain on the primary SSD.

## Rollback

The removed packages and scratch trees are deleted and not recoverable from
this workspace. The retained M1 and M2 final packages, their checksums, and all
build logs remain available. Rebuild any removed failed M4 state through the
existing fork and pinned source locks; do not restore an older release over the
retained M1/M2 artifacts.


Rule synchronization on resume: `AGENTS.md` and `README.md` now repeat the user's 26/10/2026 latest-only M1/M2/M4 retention directive. Reason: their earlier text still required retaining every candidate. Verification: the dated directive agrees with `docs/release-retention.md`; new-run/no-overwrite publication and hash checks remain required before replacing a retained release. This documentation edit changes no ROM payload.


## Latest continuation: attempt36 diagnostics removed

After finalization/PIF and policy preflights, attempt36's failed diagnostic tree,
its temp files and the disposable source replay were removed. Inventory:
`logs/20261026-m4-vendor-warning-0142/cleanup-attempt36.json`. This reclaimed
15,348,322,304 additional bytes on the second NVMe, leaving204,081,266,688 bytes
free immediately before clean attempt37. The retained complete M1/M2 directories
were checked first; M4 still had no completed release. Attempt37 writes work,
image scratch and release staging to the second NVMe; original input caches,
source, logs and rollback/provenance files remain preserved.


## Final M4 result —27/10/2026

M4 attempt37/source0188 is complete at `/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases/M4/star2lte-1793025724-rBdwLO`. Full signed ZIP SHA256 `b8c70dfded510beed8e14207e0fecbe33e98d3061408c6f10fdc3b1925135f3e`; target-files SHA256 `16246888838d7ec554ff327ee0c2af594ebf053f076017e57de0854e6e589c1b`. Full/target-files SHA256 checks passed again;8/8 legacy ZIP32 stages passed CRC/signature/range/order checks. `.complete`, `SHA256SUMS`, `SHA256SUMS.legacy`, legacy manifest and `artifact-review.json` are retained. This establishes package integrity, not M4 boot or a live Play Integrity verdict. Detailed review: `docs/m4-attempt37-artifact-review.md`.

Three incomplete M4 runs and four disposable replay fixtures were deleted after verification. Only the latest completed M1/M2/M4 run remains; their manifests were preserved. Old target-files was a hard link to the retained final file, so its logical size is not a reclaimed-byte claim. Cleanup proof: `logs/20261026-m4-vendor-warning-0142/cleanup-final-m4.json`. Available: primary190.53 GiB; secondary161.36 GiB.

## M4-0219 RTC gate and latest-only cleanup — 29/10/2026

M4-0219 package `star2lte-1793258408-cX3l8d` completed, passed artifact hash
checks and passed the isolated RTC runtime gate on G965F. The retained full
flashable SHA256 is
`13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`; target
files SHA256 is
`fb8b7e7a20580726c15dd701cb71d27fef83ecde6e2211b3db279062bd61a4a7`. Eight
ZIP32 stages and their manifest were checked. Details:
`docs/m4-rtc-label-0218.md` and the run-local `artifact-review.json`.

Following the current latest-only rule, older complete package
`star2lte-1793251536-X0X1kH` was removed only after M4-0219's package and RTC
gates passed. Its 18-file pre-delete inventory is
`logs/20261029-m4-retention-cleanup/cleanup-inventory.json`; result is
`cleanup-result.json`. The removed package contained 16,138,588,327 logical
bytes. NVMe available bytes changed from 157,034,229,760 to 173,172,887,552
(net increase 16,138,657,792). One M4 release directory remains. All source,
build/runtime logs, patch provenance and rollback evidence outside the deleted
release payload remain preserved. M4 is not overall accepted; no thermal load
or actuator test was run during this cleanup.
