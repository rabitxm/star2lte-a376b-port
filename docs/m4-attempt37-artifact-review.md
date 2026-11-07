# M4 attempt37 — build and artifact review

Completed on27October2026: full signed ZIP, target-files and8 signed ZIP32 stages verified. The final package run has `.complete`; `package_exit=0` and `split_exit=0`. This is a host build result; M4 device boot and live Integrity verdict are untested.

## Inputs and composition

- Existing fork base `2b25ae6fc3098650be249bd1898e21e563cd5df1`,178 ordered hash-locked source patches;125 resulting text files replay byte-identically.
- Donor SM-A376B/XXV/A376BXXU5BZI6, Android17 SDK37. M4 G965F profile; M1/M2 donor profiles remain separate.
- Hybrid Vendor VNDK33 with variant-appropriate G965F hardware/radio sources and the fork ABI bridge. No modem/bootloader/EFS operation in this build.
- Kernel is the original duhansysl DS-ACK V1.12 08/05/2026 Enforcing-OneUI7 prebuilt without KernelSU (archive SHA256 `9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072`). M4 keeps this production selection; the M2 WIPEONFORK diagnostic kernel profile is not selected. No kernel rebuild or byte change in this continuation. M4 kernel/runtime acceptance still requires actual device logs.
- Clean module/APK build: attempt37. Final package uses the fork's existing `--package-staged --build-rom-zip` to rerun all final gates after scoped source/metadata corrections, retaining the already rebuilt framework/Settings/PIF output.

## Required and optional behavior

ADB/USB debugging defaults on, Auto Blocker defaults off, no host RSA authorization; compiled controls retain stored user choices and do not force them each boot. Compiled gates are mandatory; M4 live behavior remains untested.

Play Integrity Fix is ported and enabled by default with application, keystore, task hooks and three Settings controls. Exact BZI6 methods and real rebuilt DEX are verified. This proves integration, not a live Play Integrity verdict. No Keybox payload is included.

Paradigm, Setup Wizard UI and Misc Tweaks retain donor behavior as selected by the user. Unsupported KnoxPatch/HMA/Keybox and other unavailable options stay omitted/hidden. Face Unlock, resolution switching and Bixby remap are omitted for this boot candidate; retained Wi-Fi UI may show donor capabilities while hardware stays S9+ Wi-Fi5.

## Verification evidence

All evidence below is in `logs/20261026-m4-vendor-warning-0142/`.

- `replay-result-0188.json`:178 source patches,125 matching text files,138 prior locked hashes unchanged.
- `staged-metadata-0188.json`:43 embedded overlay libraries normalized to0644; every content SHA256 unchanged.
- `image-metadata-preflight-0188.json`:71 runtime/native32 metadata checks pass.
- `actual-image-audit-0188.json`:50 locked native32 files plus22 runtime metadata paths pass in the real images; all three ext4 partition audits pass.
- `m4-package-attempt37-0188.txt`: final compiled boot/wallpaper/OMX/product/eSE/Wi-Fi/Settings/PIF/ABI/runtime/native-app/ADB and SELinux checks, actual ext4 audits and completed target-files. The unsigned full-ZIP step was deliberately terminated (exec session exit143) after detecting an unset DEBUG flag in the resume wrapper.
- `package-environment-check-0188.json`: diagnosis `DEBUG=<unset>` rather than buildenv's release defaultfalse.
- `m4-package-targetfiles-release.txt`: existing fork `build_flashable_zip.sh` packages the same verified target-files into a fresh run with explicit DEBUG=false, Brotli and OTA signing. Completion requires final `package_exit=0`, SHA256SUMS and `.complete`; previous partial run is discarded only after successful verification.

Earlier actual ext4 failures and corrections:0187 requires the existing intentional shared-process HAL labels for exactly mapper2.0/2.1 and libgralloctypes;0188 fixes common-V2 mode0664 imported from the host checkout rather than relaxing the image gate. Kernel, radio bytes and SELinux policy remain unchanged by these two fixes.

## Acceptance still pending

A completed package and valid checksums establish host build integrity only. Fresh-flash/first boot, app initialization, hardware/RIL/IMS behavior, stored ADB/Auto Blocker choices and live Play Integrity result require a separate G965F device test. No M4 flash has occurred in this cleanup/build task. G965N acceptance remains separate.

## Resume-wrapper correction

Reason: the task wrapper sourced the profile/config directly without buildenv, omitting DEBUG=false. Empty DEBUG caused fork conditionals to skip Brotli/signing. This was an orchestration error, not an image or compiled-code failure. The in-progress unsigned ZIP was stopped before publication; its target-files metadata independently confirms Android17/OneUI90000/BZI6/G965F.

Affected files: diagnostic task wrappers `package-m4-attempt37-0188.sh`, `package-m4-targetfiles-release.sh` and `split-m4-attempt37.sh` explicitly export DEBUG=false. No fork source or image is changed by this correction;178 source patches/locks remain the same. The final package run hard-links the verified target-files, reuses the normal existing fork full-OTA builder, and produces Brotli payloads, expected OTA signature and checksums in a new release directory.

Verification: exact isolated/shared config SHA256, strict target-files metadata, fork full-package audit and legacy stage signature/CRC checks. Source fixtures/compiled/image audits already passed on the unchanged target-files. Rollback: delete an incomplete corrected package run and rerun the existing fork command with DEBUG=false; keep the verified target-files and logs.


## Final retained artifacts

Release directory: `/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases/M4/star2lte-1793025724-rBdwLO`.

| Artifact | Bytes | SHA256 |
| --- | ---: | --- |
| Full signed ZIP | 5192379395 | `b8c70dfded510beed8e14207e0fecbe33e98d3061408c6f10fdc3b1925135f3e` |
| Target-files | 5602008927 | `16246888838d7ec554ff327ee0c2af594ebf053f076017e57de0854e6e589c1b` |

Full file: `UN1CA_1.4 (stable)-Unofficial-UN1CA 3.2.1_20261026_star2lte-sign.zip`.
Target-files: `star2lte_1.4 (stable)-Unofficial-UN1CA 3.2.1-target_files.zip`.

Eight ordered legacy packages use `-legacy-01-of-08.zip` through `-legacy-08-of-08.zip`. All8 passed CRC, cryptographic OTA signature verification and classic ZIP32 limits; largest is1194232523 bytes. Source system ranges/data order are preserved. Each stage's hash and manifest hash are in `SHA256SUMS.legacy`; full/target-files hashes are in `SHA256SUMS`. `artifact-review.json` records the source lock/Android17 metadata and stage audit; `build-config.sh` retains the selected build config.

Final evidence: `final-artifact-verification.json`, `verify-final-m4.txt`, `m4-package-targetfiles-release.txt`, `m4-legacy-attempt37.txt`, `cleanup-final-m4.json`, and `final-storage.json` under the evidence directory above.

Cleanup kept the complete M1/M2 runs and this single M4 run. Three incomplete M4 package directories and four disposable text replay fixtures were removed after verifying the new final release. The old target-files hard link was removed while the verified final copy remained. Source, input firmware/tools, build logs and the current clean attempt37 work tree remain. Available space afterwards: primary190.53 GiB; secondary161.36 GiB.
