# M2 integrated runtime diagnostic build — 24 October 2026

User approved the next proposed step: integrate the tested combination into an
explicit M2 profile, build a new retained artifact through the existing fork,
then fresh-install/test the authorized G965F. This is an integrated diagnostic
build, not a replacement for the selected production kernel or N acceptance.
No new build system, kernel compilation or firmware download is introduced.

## Source changes and provenance

Patch0102 adds `target/star2lte/profiles/m2-g965f-a376b-android16-runtime-diagnostic.sh`.
It sources the existing M2 profile and opts into hash-locked runtime inputs.
M1 resets the new flag false and retains original kernel ZIP SHA9a6bad44....
Earlier bootstrap swapping is explicitly false; the A37 bootstrap is retained.
The existing work-dir hash now includes the runtime flag/kernel hash/metadata
flag so a cached regular tree cannot be confused with the diagnostic profile.

The original installer first writes a kernel-less template then installs a
DS-ACK archive. Staging a diagnostic boot.img alone would be overwritten.
`prepare_m2_runtime_kernel.py` therefore extends that existing archive path:

- Original duhansysl ZIP SHA256
  `9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072`.
- Already calibrated G965F Image SHA256
  `92b14be170225954476869d1fd7700f9c2c92d98eb6447d0181c50a11e80fcdd`.
- Generated diagnostic ZIP SHA256
  `4cea63f84d5add494bfcc2f9fff105848806a930a24aca975dc261bf6720e20a`.
- Only G965F-kernel BSDIFF data and an explicit G965F-only installer guard differ.
  All DTBs and all other entry contents are preserved. Unknown inputs fail;
  exclusive output refuses overwriting an archive. The archive stays local.
- Kernel source is duhansysl commit914eabe7b6799ab0eb159360a5f9731dd50d0f1d,
  config SHAe71a7e50519aec68eb2208a0a25f4eb1319f18501e00183832be4fc016bc85b6,
  plus the existing separate `patches/kernel/series`0000–0002. Full compiler,
  source/image provenance and GPL source link: [kernel report](m2-runtime-kernel-fix.md).
  No version-string or configuration change is made here.

Recovery calibration executes the actual bundled BSDIFF decoder and MagiskBoot,
using files in a dedicated /tmp directory only. The predicted installed BOOT is
`55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`,
57,671,680 bytes. Kernel matches the calibrated Image; original ramdisk SHA is
`a93fe3a2c4f47705fe089ecf24853ab576d4e57ba043d15182165d5ace345862`,
DTB SHA20917e427ea02fd219bff84afa119593b5225dec1b7b0347481f9e2596b7cc9b,
and cmdline/header fields except kernel size/checksum are unchanged. It contains
no probe13 wrapper. This calibration itself writes no BOOT partition.

`a376b_runtime_compat.py` and its adjacent lock stage two verified retained S22
system files after the existing restoration steps:

| Path under SYSTEM system/ | SHA256 |
| --- | --- |
| apex/com.google.android.tethering_compressed.apex | `601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf` |
| lib64/android.hardware.graphics.common-V6-ndk.so | `010bf814b8e62ac739ff228fd6cd84477d40279c6e1c86892f064d750ee4d652` |

Both come from the already verified SM-S901B/EUX extraction, not a new source.
Existing metadata registration assigns modes/labels. Strict scope requires M2,
A376B16, star2lte, G965F, ext4, no KernelSU and the persistent metadata flag;
N, mixed bootstrap mode, wrong hashes and unknown destinations are rejected.

The existing compiled release-default verifier additionally checks final runtime
hashes/kernel selection, exact cameraserver, compiled80500 Camera branch,
canonical metadata bind/labels and zero-flag vendor container. The mandatory
ADB/Auto Blocker compiled checks remain in place. Patches0099–0101 provide the
camera/metadata/GLES1 changes tested in [the prior report](m2-camera-metadata-fixes.md).

## Execution and acceptance

Prebuild checks pass staging idempotence, wrong-model/input rejection, profile
reset and syntax. Full source replay and final image/installer/runtime checks
are recorded below as execution completes. Clean attempt51 runs the existing
`make_rom.sh --clean-work-dir --build-rom-zip`, with a new exclusive release run.
The previous working tree is temporary; completed ROM archives are retained.

No fresh-install success or new boot acceptance is asserted by starting a build.
Before writes: select G965F serialM2_G965F_SERIAL in TWRP, verify model/bootloader,
package/stage hashes and scope, capture protected-partition hashes/USERDATA UUID
and retain rollback inputs. Compare actual installed BOOT/components and final
image inputs before boot. Fresh-data defaults must be observed before changing
settings; ADB/Auto Blocker switches need real effect and persistence checks.
M1/G965N are not part of this test. No EFS/CPEFS/radio/modem/bootloader write.

Local execution evidence: `logs/20261024-m2-integrated-build/`.


## Compiled and source gates completed

93 fork patches replay to78 touched source paths, byte-identical. The final
compiled verifier passes A37 app64/native32 policy, selected kernel/runtime
inputs,80500 Camera branch, canonical persistent metadata/zero-flag vendor,
mandatory no-auth ADB defaults/stored-choice lifecycle and the existing release
gates. Actual ext4 verification follows during packaging. A small real ext4
fixture also confirms the debugfs parser accepts correct metadata and rejects
a wrong camera SELinux label; it contains no proprietary payload.

The exact actual kernel build config is now shared as text at
`support/kernel/g965f-runtime-build.config.txt`, SHAe71a7e50...16bc85b6,
for source/config reproduction. No kernel binary is exported. Ordinary source
stock_defconfig shown by /proc/config.gz is not substituted for this real config.

The existing fork also downloads optional Galaxy Store applications during a
clean build. The pre-rebuild SmartSuggestions input was retained locally with
its hash in smart-suggestions-input.json. This records provenance; starting the
same commands at a later date is not a claim of bit-identical optional app inputs.


## attempt51 image-verifier correction

attempt51 compiled gates passed but image audit stopped:
`ValueError: Image owner mismatch: system/bin/cameraserver`.
The newly added verifier wrongly required0:0 for every selected file. Both
verified donor fs_config inputs and final fork metadata specify0:2000 for
cameraserver (mode0755); image ownership followed that metadata. Correct the
verifier to require donor-matched0:2000 for this executable and0:0 for the other
selected files. Do not change a correct image to satisfy an incorrect audit.
Repackage the already compiled/verified work tree using the existing
--package-staged path after rerunning gates; no APK rebuild or phone write.


## attempt52 fresh install and boot result

The source correction for attempt51's owner audit was verified in corrected real-ext4 fixtures, then `--package-staged` completed attempt52. Both actual SYSTEM and VENDOR ext4 images passed byte, mode, ownership and SELinux checks; `cameraserver` is UID0/GID2000. The signed enforcing flashable ZIP passed the fork's six-payload verifier. Full ZIP SHA256 `61bc3696dfe40939611356285f63ee09f96d0110dc56af4c65d3346dcbebea12`; target-files SHA256 `f62277de3b449868fe1fe33832aa4cfe7b6dd2adc1976d6d7a2d321d66fa791b`. Eight TWRP32 stages passed CRC, signature and ZIP32 checks.

Fresh install on authorized G965F `M2_G965F_SERIAL` passed all eight stages and fresh USERDATA verification; UUID changed and the pre-format marker disappeared. Measured EFS, CPEFS, RADIO, RECOVERY and BOTA0/1/2 byte hashes were identical before and after. BOOT matched the calibrated installed hash `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`.

The 240-second boot watch observed ADB only briefly at 189.8s and 228.0s, with no `sys.boot_completed=1`. Captured logcat gives the fatal at line74018:

```text
10-24 09:12:33.433  2036  2036 E System  : java.lang.IllegalStateException: Missing permission definition for permission "android.permission.RANGING" associated with app op 151
```

The source of that fatal is explicit earlier in the same capture: `init: Command 'mkdir /metadata/aconfig 0775 root system' ... failed: Read-only file system` at line4590 and the corresponding post-fs failure at line6042. The Aconfig flag table is therefore unavailable and the framework skips the donor RANGING permission declaration, while AppOps requires it. This explains why the earlier build's donor permission-only check was insufficient. No AppOps failure was bypassed.

After capture, TWRP read pmsg-ramoops-0 and attempt52 BOOT was restored and read back as baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Exact protected partition hashes still match. M1 and G965N were untouched.

## attempt53 corrective variable

Patch0104 adds the previously boot-tested 16MiB volatile tmpfs mount at `/metadata` before donor aconfigd creates maps. It is only activated by the explicit G965F M2 diagnostic profile; ordinary profiles remain unchanged. The generated init file is hash-locked to `822b51c7e9d748bceed9aab50a504efd0b43e7c1ca15aa62bdab24bf95222357`, and its donor input is `78d6bb311aa7126384da4b59780d4a87d9b321c4fd211d04e1339fbd8975447b`. A fixture confirms apply order and idempotence. The source patch applies over the files introduced in0102 and the full series lock contains its hash.

Attempt53 is rebuilding through the existing fork with a unique retained output. It will be image-audited, installed fresh on the same authorized G965F, and checked for `ranging_permission_enabled`, a dangerous RANGING permission, stable SystemServer and boot completion. The tmpfs is volatile; persistence and production acceptance remain unproven.

## attempt53 build completed; runtime pending

Patch0104 adds the opt-in 16MiB `/metadata` tmpfs hook before donor aconfig initialization. The clean attempt53 build through the existing fork completed with exit0. Compiled ADB/no-auth properties and stored-choice lifecycle, Exynos9810 contract, staged SELinux policy compilation, and runtime-image gates passed. Actual ext4 SYSTEM/VENDOR/ODM verification passed, including the generated `aconfigd.rc` SHA `822b51c7e9d748bceed9aab50a504efd0b43e7c1ca15aa62bdab24bf95222357`; final signed package verification reports enforcing/ext4/six device payloads.

Retained output: `rom/out/releases/M2/star2lte-1792833533-dLCdbp/`. Flash ZIP SHA256 `f72be9be303e34fd5f625c5fd3d50e0590589e62b85386d8816f97f9758343ef`; target-files SHA256 `e0a047c606ce915253c6c9a31a3862d13aa2de01826ffc43f559c64541280392`. The DS-ACK V1.12 WIPEONFORK enforcing/no-KSU diagnostic package selected for this isolated runtime diagnosis is recorded in the build log; this does not replace the production duhansysl package. Eight signed TWRP32 stages passed CRC, signature, SHA and ZIP32 checks.

Read-only preflight on G965F serial alias M2 confirms `SM-G965F`, bootloader `G965FXXUHFVG6`, TWRP `3.7.0_9-0`, device `star2lte`; the full package, target-files, BOOT and all stage hashes match. Updater script contains no writes to EFS, CPEFS, RADIO, RECOVERY or BOTA. Exact-size raw reads of those protected partitions and BOOT completed, and BOOT remains baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Evidence is in `logs/20261024-m2-attempt53/read-only-preflight.json`.

The user then granted a one-time exception for exactly this attempt53 package on the same G965F, including fresh USERDATA format and boot. It was not extended to any other package or to G965N. Execution and runtime results follow.

## attempt53 one-time fresh install and runtime result

The installer completed all eight recovery stages with RC=0. Stage 8 reports
`Formatting cache, omr, preload and data...` and `script succeeded`. USERDATA UUID
changed from `e7303818-969c-4747-82ea-c7c129d01ed1` to
`e9858dbe-3a0a-4176-8d84-bed40b751e95`; the pre-format marker was absent afterward.
Exact-size raw hashes for EFS, CPEFS, RADIO, RECOVERY and BOTA0/1/2 match before
and after. BOOT matches diagnostic image hash
`55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`. No protected
partition or modem/bootloader was written. Evidence is retained in
`logs/20261024-m2-attempt53/`.

Fresh first boot completed: the watcher saw ADB at about 189.7 seconds and
`ANDROID_BOOT_COMPLETED=1` at 196.4 seconds. The running kernel identifies as
`4.9.337-DS-ACK-V1.12-G965F-08.05.2026`, SELinux is Enforcing, and
`system_server` and SurfaceFlinger remain present. The captured logs contain no
`Missing permission definition for permission "android.permission.RANGING"`
or fatal SystemServer exception. Runtime `dumpsys package android` lists
`android.permission.RANGING` as dangerous, and logcat records
`RangingService: Registering Ranging service`.

Patch0104's diagnostic effect is confirmed: `/metadata` is a 16 MiB tmpfs and
`/metadata/aconfig/{boot,flags,maps}` plus storage records exist on fresh data.
This demonstrates current-boot aconfig initialization, not persistence across
reboot; the mount is volatile. Fresh ADB settings show `adb_enabled=1`,
`development_settings_enabled=1`, and `ro.adb.secure=0`; Rampart logged
`disabledRampart=true` at locked boot. These support the required defaults and
no-RSA configuration on this first boot. User-toggle persistence/behavior across
manual changes remains a separate acceptance check.

Attempt53 used `m2-g965f-a376b-android16-runtime-diagnostic.sh`, which enables
the runtime-kernel diagnostic but does not set `EXYNOS9810_ENABLE_INIT_DEBUG`.
The separate `m2-g965f-a376b-android16-no-ksu-init-debug.sh` profile carries that
flag. Therefore the intended CACHE init logger was not enabled for this attempt:
`/vendor/etc/init/init.debug.rc` and `/vendor/bin/init_debug_log.sh` are absent,
and `/cache/unica` was not created. Patch0095 provides the final vendor hook,
but attempt53 did not test it; the earlier test plan incorrectly assumed this
profile enabled it. Boot milestones came from host ADB/logcat. If another
logger-dependent probe is needed, use a reviewed combined profile and require
both files in the final ext4 VENDOR image before flashing.

The captured dmesg also includes USB gadget init failures; for example,
`init.samsungexynos9810.usb.rc:382` reports `Unable to write to file
'/config/usb_gadget/g1/UDC' ... No such device`. ADB is reachable on this host
after boot and `ro.adb.secure=0`. Check this against direct USB behavior and the
S9+ controller property before any USB-stack patch. Full logs remain local under
`logs/20261024-m2-attempt53/`.

Attempt53 proves a fresh G965F M2 Android16 first boot through SystemServer and
boot completion under this diagnostic configuration. It does not prove full
hardware acceptance, user-toggle lifecycle, persistent metadata, G965N
compatibility or Android17/M4 readiness. Keep this diagnostic package distinct
from production duhansysl kernel releases; M1 and G965N remain untouched.
