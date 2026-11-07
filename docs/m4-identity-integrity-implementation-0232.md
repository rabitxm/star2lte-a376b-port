# M4 r3 — public identity, compatibility declarations, PIF and flash result

Date: 31 October 2026  
Target: Galaxy S9+ G965F, serial `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`  
Donor: SM-A376B BZI6 / Android 17 / SDK 37  
Status: **r3 booted to Android Setup Wizard. Setup Wizard and live Play Integrity
test remain pending. M4 is not accepted.**

## Result

The fresh-flashed G965F now presents the requested public identity while keeping
its real hardware target:

| Check | Live value |
| --- | --- |
| Public model | `ro.product.model=SM-A376B` |
| Public product | `ro.product.name=a37xnaxx` |
| Device codename | `ro.product.device=star2lte` |
| Physical model | `ro.boot.em.model=SM-G965F` |
| Marketing name | Settings global `device_name=Galaxy A37 5G` |
| OS | BZI6 / Android 17 / SDK 37 |
| Boot | `sys.boot_completed=1`, boot animation stopped, SELinux Enforcing |

ADB is enabled by default (`adb_enabled=1`, `ro.adb.secure=0`). Fresh Settings
defaults show `rampart_main_switch_enabled=0`,
`rampart_strict_protection_switch_enabled=0`, `rampart_suw_main_on=0`, and both
Rampart blocking settings are `0`. The user has not yet completed Setup Wizard,
so the live Settings toggle UI and user's post-setup choices still need a
functional test.

NFC compatibility remains visible: Android NFC, HCE/HCEF/eSE/UICC feature flags,
`com.android.nfc_extras`, and `com.samsung.android.nfc.adapter` are present. The
Samsung Bixby agent and wakeup packages are installed/enabled. No additional
NFC RF or Bixby voice/key-remap port was added. The single crash captured after
fresh boot is the known, user-accepted donor issue:

```text
10-31 14:16:51.756 E AndroidRuntime: Caused by: java.lang.IllegalStateException: No matching ComponentResult found!
10-31 14:16:51.756 E AndroidRuntime:   at com.samsung.android.bixby.onboarding.receiver.LockedBootCompletedReceiver.onReceive(SourceFile:104)
```

This is separate from boot health; `sys.boot_completed=1`, SystemServer PID2216
and SystemUI PID6238 were live in the first stability sample. Only this accepted
Bixby crash appeared in the crash buffer captured so far.

## Source changes and verification

| File | Reason | Verification |
| --- | --- | --- |
| `rom/unica/patches/exynos9810_device_stack/customize.sh` | Set the M4/G965F public model to `SM-A376B`; preserve real `star2lte` and physical `SM-G965F`; seed the scoped identity/profile properties. | Boot contract compiled gate; live `getprop` after fresh boot. |
| `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh` | Reapply the A37 identity after donor/vendor restore; retain Samsung Bixby capability declaration; pin NFC HCE/HCEF/eSE/UICC permission XMLs to `vendor_configs_file`. | Finalizer output and actual ext4 image metadata audit. |
| `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/a376b_android17_target_identity.py` | Make Android17 target identity transform deterministic and scoped to M4/G965F/BZI6. | Fixture and boot contract; image verifier. |
| `rom/unica/mods/pif/a376b_android17_pif.py` | Replace reflective write to `Build` final fields with guarded Unsafe writes for donor Android17. | Transform fixture and compiled PIF gate; a live checker request has not run yet. |
| `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/adb_defaults.py` | Seed `device_name=Galaxy A37 5G` only for a fresh M4 Settings DB; align compiled verifier's expected SQL with the seed. | `PASS compiled ADB defaults, no-auth properties and stored-choice lifecycle`; live settings/property check. |
| `rom/scripts/internal/verify_exynos9810_release_defaults.py` | Require the M4 PIF patch-level and first-API profile fields in release defaults. | Compiled release-default gate; live property values. |
| `rom/scripts/internal/verify_exynos9810_boot_contract.sh` | Separate public A37 identity from physical star2lte/G965F hardware assertions. | `Verified Exynos9810 boot contract for star2lte: public SM-A376B/a37xnaxx, hardware SM-G965F, ext4`. |
| `rom/scripts/internal/verify_m4_runtime_image.py` | Check public identity, physical model, NFC/Bixby compatibility, actual image file content/mode/owner/SELinux labels. | Real ext4 system/vendor/ODM verification; four NFC XML labels pass. |

No framework VNDK, Hybrid Vendor composition, RIL, modem, bootloader, EFS or
kernel source was changed. The package uses the prebuilt DS-ACK V1.12 G965F
WIPEONFORK diagnostic zip already selected by the M4 diagnostic profile, based
on the earlier logged `MADV_WIPEONFORK` incompatibility. No new kernel was built.

## Build and artifact audit

The first clean build failed its compiled ADB gate with:

```text
RuntimeError: Unexpected seed SQL count: global
```

The fresh `device_name` seed had been added to the patcher but not to the
compiled checker's `SEEDS` table. Both were aligned; the compiled ADB gate then
passed.

The next package attempt caught two actual image issues. First:

```text
ValueError: M4 Samsung Bixby compatibility declaration missing in .../vendor/etc/floating_feature.xml
```

The vendor baseline restore removed the Bixby setting. The finalizer now reapplies
`SEC_FLOATING_FEATURE_COMMON_SUPPORT_BIXBY=TRUE` after restore. The next real
image check found:

```text
ValueError: M4 image SELinux label mismatch: etc/permissions/android.hardware.nfc.hce.xml security.selinux (26) = "u:object_r:vendor_file:s0\000"
```

The four shared NFC feature overlays inherited the generic `/vendor/etc/permissions`
label. The finalizer now emits exact `vendor_configs_file` metadata for those
files. R3 real vendor-image audit reports `vendor_configs_file` for
`android.hardware.nfc.hce.xml`, `.hcef.xml`, `.ese.xml`, and `.uicc.xml`.

R3 build run: `star2lte-1793426231-65eoO4`  
Full flashable SHA256:
`45d34b63ee275b0123fd7fca6c9e868a7a4d12c6bd8069f9c80dcb7356d33daf`  
Target-files SHA256:
`664dcf2599a17b6f70bc52649f3e1beafbffd86b54452bc39b56478a845ef04a`  
Legacy manifest status: `PASS`, eight ordered ZIP32 stages. All 10 hashes in
`SHA256SUMS` and eight in `SHA256SUMS.legacy` passed readback. The flashable gate
reported `ext4, enforcing, six device payloads`. Actual SYSTEM/VENDOR/ODM ext4
checks and file metadata gates passed. The package's `.complete` marker exists.
Older M4 release folders were pruned under the latest-only rule after r3 passed
package/hash checks; rollback partition images/GPT and scripts remain in the old
flash log folder.

## Flash and device safety record

Before writing, TWRP reported `SM-G965F`, serial `M2_G965F_SERIAL`, bootloader
`G965FXXUHFVG6`. Preflash SHA256 values for EFS, CPEFS, RADIO, RECOVERY, BOTA0,
BOTA1 and BOTA2 are saved in
`logs/20261031-m4-identity-integrity-0232/preflash-protected-partition-sha256.txt`;
postflash values are byte-for-byte identical. Primary and backup GPT sector
snapshots are saved before the write. The audited installer writes SYSTEM,
VENDOR, ODM and BOOT; can repartition to the target layout; and formats
CACHE/OMR/PRELOAD/USERDATA. It contains no EFS/CPEFS/RADIO/RECOVERY/BOTA write.

All eight legacy stages were installed in order from TWRP `/tmp`, each after
independent 64 MiB transfer-chunk SHA256 verification and a remote full-file
hash/size check. Recovery markers reached `next=done`; stage 8 ended with
`Updater process ended with RC=0`. This `/tmp` method also prevents stage 8
from losing its installer ZIP when USERDATA is formatted.

Read-only `e2fsck -fn` returned 0 for SYSTEM, VENDOR, ODM, USERDATA, CACHE, OMR
and HIDDEN. Protected partition hashes match. No EFS content was dumped or
written. Full device and per-stage evidence is under
`logs/20261031-m4-identity-integrity-0232/`.

## Open tests and next actions

The current screen is Setup Wizard and `user_setup_complete=0`. The user is
choosing whether to sign in to Google before the Play Integrity test. Until
then, this report does not claim a live PIF hook or Google server verdict.

The remaining checklist is:

1. Complete Setup Wizard. If a Google account is available, sign in so Play
   Store/checker requests can execute; otherwise skip and leave server verdict
   untested.
2. Verify a normal app reads `Build.MODEL=SM-A376B` and About phone shows
   Galaxy A37 5G while hardware stays star2lte/G965F.
3. Run the existing Play Integrity checker once, save sanitized labels/JSON,
   and verify the new build has no `IllegalAccessException` from
   `PlayIntegrityHooks`. Do not equate PIF toggle-on with PASS.
4. Recheck common keystore key generation/signing, then run one temporary
   attestation key test. If required, change only `remote_provisioning.tee.rkp_only`
   during the controlled trial and restore it immediately; never delete user
   keystore state or invent attestation credentials.
5. Check server Play verdicts. `UNEVALUATED` is not PASS. No keybox or fake verdict.
6. Keep NFC/Bixby compatibility declarations/packages. Don't spend effort
   porting RF or Bixby voice/remap in this scope; record the known accepted
   Bixby receiver crash.

## Rollback

The old attempt0229 release folder was removed per the 26/10 latest-only
retention rule after r3 passed package/hash checks. Its baseline SYSTEM/VENDOR/
ODM/BOOT images, saved GPT and rollback scripts remain at
`logs/20261030-m4-g965f-test-0229/`. For a failed boot/runtime, return to TWRP
and restore those partition images; restore the saved GPT only if the partition
table actually changed. Verify protected partition hashes again.
Never write EFS, CPEFS, RADIO, RECOVERY or BOTA as part of rollback.
