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


## Live follow-up after user acceptance

The user confirms ADB/Auto Blocker behavior is stable and reports no observed
basic hardware fault. The read-only attempt53 follow-up found that the installed
canonical and secondary vendor init hashes were both
`f299c805...d74bc8`, while the image-audited canonical init with the CACHE bind
was `88deb866...e7be1a6`. `vendor_bootfix.sh` overwrote the canonical file from
the secondary during install. At runtime `/metadata` remained tmpfs and
`/cache/metadata` was absent. Patch0105 changes the installer copy to fallback
only, removes the volatile tmpfs staging, and adds final checks for original
donor aconfigd and the canonical CACHE bind. Build and boot verification remain
pending.

The same read-only capture found repeated SystemServer SIGABRTs after first
boot. `bpfloader` logs `gpuMem.bpf: Invalid argument` and
`gpuWork.bpf: Invalid argument`; the pinned system `libmeminfo.so` aborts in
`BpfMapRO::abortOnMismatch` while stats pull calls `ReadPerProcessGpuMem` and
`ReadProcessGpuUsageKb`. The latest captured SystemServer stayed alive after a
restart, but long-run stability remains open. Preserve the production kernel
selection and investigate this Android 16 GPU stats/BPF interface separately.
Evidence: `logs/20261024-m2-followup/`.

## Attempt54 — persistent metadata installer fix and G965F runtime (25 October 2026)

Patch0105 fixes the attempt53 install-time overwrite: `vendor_bootfix.sh` now
copies the secondary init file only when the canonical `etc/init/hw` file is
missing. The M2 runtime compatibility patch also leaves donor `aconfigd.rc`
unchanged instead of staging volatile `/metadata` tmpfs, and requires the
original donor hash plus exactly one `/cache/metadata` bind in the canonical
init. Files: `patches/fork/0105-preserve-cache-metadata-bind-in-installer.patch`,
`rom/platform/exynos9810/installer/repartition/vendor_bootfix.sh`, and
`rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_runtime_compat.py`.
Patch replay, shell syntax, Python syntax, build gates, SELinux policy compile,
three ext4 image checks, and signed flashable ZIP verification passed. The real
vendor image contains canonical init SHA256
`88deb866246c25eef4926ee9b11b5344c5878cbe399bb0f67d6cefeb0e7be1a6`; its
secondary baseline remains `f299c805332262f5eb1f7625e6dbd15e7a8eb0638c0a99f6e8963f0841d74bc8`.
The bundled installer contains the same fallback-only copy condition.

Retained package directory:
`rom/out/releases/M2/star2lte-1792852256-BVoh9H/`.
Flashable ZIP SHA256 is
`3871a5d696ce5ea55a299e723c54d6d6d22fe7ff995205ec3c97deaf7f2562ed`
(5,529,949,917 bytes); target-files ZIP SHA256 is
`d5abb6dc9264e9995b80247ba47555b3612348ef463640bd570fc7fc419d62ff`
(5,928,295,860 bytes). The diagnostic DS-ACK BOOT is packaged; the selected
production kernel remains unchanged. Build log:
`logs/20261024-m2-integrated-build/attempt54.txt`; exit status is 0.

Fresh install on G965F completed all eight verified TWRP32 stages with RC=0.
USERDATA UUID changed; byte hashes for EFS/CPEFS/RADIO/RECOVERY/BOTA0/1/2
matched prewrite values. BOOT matched the packaged diagnostic image. First boot
reached `sys.boot_completed=1` after 204s; second boot reached it after 80.5s.
ADB reported `device`; fresh values were `adb_enabled=1`,
`development_settings_enabled=1`, `ro.adb.secure=0`, Rampart switch `0`, and
SELinux Enforcing. `RangingService` registered and the earlier missing-RANGING
permission failure did not recur. SystemServer PID/start count stayed stable
through a 90-second second-boot sample.

A temporary marker's SHA256 survived one controlled reboot on `/metadata`
mounted from `/dev/block/sda21` as ext4; the marker was then removed. This
verifies one-reboot persistence only, not CACHE clearing, OTA, staged flags or
wider storage lifecycle behavior.

Remaining failures are concrete: logcat lines 7840 and 7848 in
`logs/20261025-m2-attempt54/boot2-logcat.txt` report `gpuMem.bpf` and
`gpuWork.bpf: Invalid argument`; lines 164304–164367 and 181425–181482 show
`com.samsung.android.kgclient` has no matching SELinux seapp context and each
child zygote aborts with SIGABRT. At line 168661 the Bixby agent also throws
`IllegalStateException: No matching ComponentResult found!`. These did not stop
SystemServer from reaching boot completion, but they mean the build is not a
stability or full hardware acceptance result.

The expected cache logger did not run: the retained `build-config.sh` omits
`EXYNOS9810_ENABLE_INIT_DEBUG`, build log `attempt54.txt` has no
“Restoring final Exynos9810 CACHE init logger” entry, and live checks report
`/vendor/etc/init/init.debug.rc` and `/vendor/bin/init_debug_log.sh` missing;
`/cache/unica` is absent. Therefore this test did not validate patch0095's
logger. A future logger probe must use a reviewed combined profile and confirm
both files in the final VENDOR image before flash. Full local evidence is under
`logs/20261025-m2-attempt54/`; it contains raw device logs and stays local.

No M1/G965N, EFS/CPEFS/RADIO/RECOVERY/BOTA or bootloader write occurred. The
selected production kernel remains unchanged.

## Attempt55 — isolate the retained CACHE logger

Patch0106 adds a parallel `m2-g965f-a376b-android16-runtime-init-debug.sh`
profile. Its exported environment differs from attempt54's runtime profile only
in `EXYNOS9810_ENABLE_INIT_DEBUG=false -> true`. The existing vendor logger
from patch0095 is the sole added runtime behavior. Kernel, BPF objects,
hardware/radio blobs and seapp policy are unchanged for this diagnostic.

Affected files: the new profile, M1 profile diagnostic reset, existing work-tree
cache key, config serialization and real-ext4 runtime image verifier. The
logger flags are recorded in build-config and cache identity; final VENDOR
verification requires exact staged logger bytes, modes, UID/GID and SELinux
labels. Valid real-ext4 fixtures pass; missing logger, wrong GID and wrong
label are rejected. Profile delta/reset and cache-key divergence pass. Complete
source replay passes 96 patches / 79 paths byte-identically; old series hunks
that use GNU patch fuzz are recorded in the replay JSON.

The clean build uses the existing fork `make_rom.sh --clean-work-dir
--no-target-files` while retained releases are moved and checksum-verified on
the second NVMe. Packaging will use the existing `--package-staged
--build-rom-zip` only after storage and compiled gates pass. Build/test evidence
is private in `logs/20261025-m2-attempt55/`. No build or boot success is claimed
by this entry.

Read-only follow-up also narrows the kgclient problem: AZH4 already has
`user=vendor_kg seinfo=platform name=com.samsung.android.kgclient domain=kg_app
type=app_data_file levelFrom=user` in `system_ext_seapp_contexts`, and that line
survives staging. Donor VENDOR passwd/group define vendor_kg as UID/GID5959;
installed Hybrid VENDOR lacks those account rows and `id 5959` resolves to
`oem_5959`. This is evidence for account-name/policy integration investigation,
not a reason to broaden SELinux access. Attempt55 intentionally does not mix
that prospective fix into the logger calibration. Live attempt54 stayed
Enforcing with SystemServer PID1716/start_count1 at uptime1299.77s; this is an
observed stability interval, not full hardware acceptance.


### Independent BPF runtime evidence before attempt55 flash

A temporary raw AArch64 test loaded the same return-0 two-instruction program
as three program types on the running attempt54 kernel. Socket-filter control
returned FD3 and a successful verifier log; TRACEPOINT and PERF_EVENT each
returned -22 (EINVAL). All FDs were closed and the temporary executable removed;
no program was attached or pinned and no config/policy/partition was changed.
Source: diagnostics/bpf_prog_type_selftest.c. Evidence:
logs/20261025-m2-attempt55/bpf-prog-type-selftest.txt.

The live kernel has CONFIG_BPF_SYSCALL=y, CONFIG_KPROBES unset and no
CONFIG_BPF_EVENTS. Pinned source include/linux/bpf_types.h gates tracepoint and
perf-event program registration on CONFIG_BPF_EVENTS; kernel/trace/Kconfig
requires probe events for that option. Thus the loader EINVAL is reproducible
with a minimal program and is not explained solely by donor BPF instructions.
This does not prove that enabling the option alone is sufficient: availability
of the actual GPU tracepoints/helpers and libmeminfo map ABI still needs
validation. Attempt55 retains the current diagnostic kernel to isolate logger
behavior. A future kernel diagnostic must have its own provenance and tests.


Attempt55 packaging correction: the first package pass exposed a real config
regeneration bug. `package-first-buildenv.txt:16` recorded INIT_DEBUG=false
after the second buildenv call, and vendor image verification omitted logger
checks. No phone was flashed. That incomplete release was marked rejected.
Patch0106 now also preserves the two explicitly selected diagnostic flags
while buildenv clears old config variables. Actual repeated buildenv snapshots
pass true/true/false/true (M2/M2/M1/M2); source replay now passes96patches /80
paths. The compiled tree already contains the selected logger and all APK/JAR
gates passed; repackaging uses the existing --package-staged path with a new
unique release directory and an explicit post-buildenv true-flag assertion.
This corrects build selection/audit; no additional runtime behavior is added.


## Attempt55 completed — 25 October 2026

The clean compile, corrected staged packaging, actual ext4/compiled gates and
all eight signed ZIP32 install stages passed. The selected final config has
INIT_DEBUG=true. Both logger files have exact bytes, UID/GID, modes and SELinux
labels in final VENDOR; the installed VENDOR was checked again using ro,noload
before reboot. All protected partition byte hashes matched across installation,
USERDATA has a new UUID, and installed BOOT is the expected diagnostic hash.

Retained release: `rom/out/releases/M2/star2lte-1792895273-SYRXCu/`.
Full flashable ZIP: `UN1CA_1.4 (stable)-Unofficial-UN1CA 3.2.1_20261025_star2lte-sign.zip`,
5,530,428,098 bytes, SHA256
`a7a9ad8e54c56e3012497f1c1e4401c01efde62175c5500dce440e450dcecafd`.
The eight signed ZIP32 stages and their manifest are in its `legacy/` directory.
Package BOOT SHA256 is
`6a6fe0cd652b22103e88547254c0b8b777aef9612b40309ab1150fe1d1ee410e`;
installed full BOOT SHA256 is
`55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`.
This is the same existing WIPEONFORK diagnostic kernel/input as attempt54.
No new kernel build or BPF/seapp/radio behavior fix was included.

Fresh boot reached completed1 in180.7s; controlled second boot in79.7s, both
Enforcing. Fresh values are adb_enabled=1, development_settings_enabled=1,
ro.adb.secure=0 and secure rampart_main_switch_enabled=0. The user's prior
functional ADB/Auto Blocker confirmation remains recorded; this trial checked
fresh defaults and USB access rather than repeating every GUI toggle test.
SystemServer stayed PID2057/start_count1 through first-boot uptime662.79s;
second boot PID1688/start_count1 at uptime89.87s,121.61s and191.67s. These are bounded
observations, not long-run stability acceptance. Camera/audio/phone services
were found; full hardware acceptance does not follow from service registration.
A unique temporary /metadata marker preserved its hash across the reboot on
CACHE ext4 (/dev/block/sda21); it was removed and absence verified afterward.

CACHE logger runtime result is PARTIAL, with two successful marker/logcat
captures. Both boots wrote post-fs reached, post-fs-data reached and
boot_completed=1. First-boot start/15s/60s logcat files were807124/1110540/7169624
bytes; second-boot files810198/3708176/15322776 bytes. cmdline and partition
snapshots were also nonempty. However props_start.txt and all three dmesg
snapshots remain0bytes, and init_debug_start.txt contains only64bytes of
headers. The JSON collector's PASS means markers plus logcat only, not all
snapshot classes. Restarting the existing service late reproduced the gap.
Read-only runcon tests under u:r:init:s0 reproduced empty date/getprop output
with rc0 and toybox cat rc1; direct root ADB applets/dmesg produce output.
No matching denial or complete root cause was established. No policy weakening,
service relabel or kernel change was made to conceal this limitation.

Remaining errors in `logs/20261025-m2-attempt55/boot2/logcat-all.txt`:

- Line7879: `Error when loading /system/etc/bpf/gpuMem.bpf: Invalid argument (os error 22)`;
  line7887 repeats this for gpuWork.bpf.
- Line152556: `No match for app with uid 5959` for
  com.samsung.android.kgclient, `username oem_5959`; line153275:
  `Fatal signal 6 (SIGABRT)` in ch_zygote. A second pair is169261/169303.
- Line156141: Bixby agent `IllegalStateException: No matching ComponentResult found!`.

The separate raw BPF test proves TRACEPOINT/PERF_EVENT type EINVAL on the
current config while SOCKET_FILTER succeeds. Donor/staged seapp already has
the kgclient rule; the Hybrid Vendor account-name discrepancy is the next
narrow integration hypothesis. Neither finding is fixed by this logger-only
trial. Keep BPF capability/tracepoint/map ABI and account-name tests separate.

Evidence is local under `logs/20261025-m2-attempt55/`: series-replay.json
(96patches/80paths PASS), logger-final-image-audit.json,
installed-logger-audit.json, install-complete.txt, boot-watch.json,
boot1/runtime-summary.json, boot2-control.json, boot2/runtime-summary.json,
boot2-stability-final.txt and the late/runcon control captures. Raw device logs
and firmware outputs remain excluded from source publication. Phone remains
in Android attempt55; no temporary metadata marker remains. G965N/M1 were
untouched. Production kernel selection and M4/Android17 gates are unchanged.


## Attempt56 preparation — account-name diagnosis, 25 October 2026

Read-only capture from booted G965F confirms `uid=5959(oem_5959)` and
`gid=5959(oem_5959)`, with SELinux Enforcing. The app is installed at
`/system/priv-app/KnoxGuard/KnoxGuard.apk`; no kgclient/kg_app process survived
at capture. `/system_ext/etc/selinux/system_ext_seapp_contexts:35` contains the
exact `user=vendor_kg ... name=com.samsung.android.kgclient domain=kg_app` rule.
The installed Hybrid Vendor has no account entry for name vendor_kg or ID5959.
The pinned A37 donor files match full-file SHA256 and define one exact passwd and
group row for vendor_kg at UID/GID5959; target staging has neither. This
confirms the account lookup mismatch as the reason the observed rule cannot
match, while the next runtime must still prove the fix end to end.

Patch0107 modifies only the existing M2/G965F runtime compatibility stage to
merge these two exact rows after vendor restoration; the rows are hash-pinned
and any duplicate name/ID aborts the build. The image verifier now checks the
actual vendor ext4 passwd/group bytes, owner, mode and SELinux label. Fixture
checks prove the donor hashes, exact rows, idempotence, and collision rejection;
syntax checks pass. The patch is recorded in `patches/fork/series` and
`sources.lock.json`. A new clean build and live boot remain pending; attempt55
is retained as rollback/reference. Files, reason, checks and rollback are in
`docs/patch-matrix.md` under Patch0107. Full read-only captures stay local under
`logs/20261025-kgclient-investigation/`.


## Attempt56 completed — 25 October 2026

Patch0107 addresses only the donor-pinned UID/GID account mapping. Its stated
reason, affected files, checks and rollback are in `docs/patch-matrix.md` under
Patch0107. It modifies the existing fork's M2 runtime adapter and donor lock,
the vendor restore customizer, and the actual-ext4 verifier; the tracked patch
and source-lock entry are in `patches/fork/0107-restore-a376b-vendor-kg-account-for-m2.patch`,
`patches/fork/series` and `sources.lock.json`. It adds no sepolicy allow, kernel,
radio or EFS changes.

Build used the existing `make_rom.sh --clean-work-dir --no-target-files` compile
and existing `--package-staged --build-rom-zip` packaging flow. The profile and
final-image gates passed, including compiled ADB defaults, SELinux policy,
ABI/runtime checks and the actual ext4 SYSTEM/VENDOR/ODM audits. Series replay
passed 97 patches / 80 paths. The actual ext4 VENDOR audit confirms the exact
donor-pinned passwd/group content and `vendor_configs_file` mode/label; the
logger remains present. The signed flashable ZIP is retained at
`rom/out/releases/M2/star2lte-1792900635-m9nN7O/`:

- Flashable ZIP: 5,530,402,150 bytes, SHA256
  `f6e4a1069cca283ef32751abace1aedc23222d9ea5eb002d9a78e19475ee5e15`.
- Target-files ZIP SHA256:
  `fe2a891ca884883290d41efa10338d4e2249469df23f45d5f78e40585386b708`.
- Package BOOT payload SHA256:
  `6a6fe0cd652b22103e88547254c0b8b777aef9612b40309ab1150fe1d1ee410e`.
- All eight legacy sideload stages passed ZIP32/signature/CRC checks. The
  release manifest and source hashes are under its `legacy/` directory.

On the authorized G965F test target, installer preflight rechecked model,
bootloader, TWRP, package/stage hashes and exact raw partition sizes. It saved
the current BOOT before writing; stages1–8 each returned `RC=0`. USERDATA UUID
changed after fresh format. Pre/post raw hashes for EFS, CPEFS, RADIO, RECOVERY
and BOTA0/1/2 are identical. Installed BOOT matches the package diagnostic
hash. The installed VENDOR was then mounted `ro,noload`; passwd/group/logger
bytes, UID/GID, mode and SELinux labels passed before boot.

Both first and controlled second boot reached `sys.boot_completed=1` with
Enforcing SELinux. On both boots `id 5959` reports `uid=5959(vendor_kg)
gid=5959(vendor_kg)`, and the live package process uses `u:r:kg_app`. A scan of
`boot1/logcat-after-boot.txt` and `boot2/logcat-midwatch.txt` found zero
`No match for app with uid 5959` and zero `Fatal signal 6 (SIGABRT)`. The
second boot completed in83.5s; a temporary marker survived on ext4 `/metadata`
and was removed by the test. `system_server` stayed at start_count1. Both
`/cache/unica` captures contain post-fs, post-fs-data and boot-complete markers
and nonempty logcat snapshots. Init-context dmesg and props snapshots remain
empty; the collector PASS applies to the markers/logcat classes only.

The remaining KnoxGuard problem is separate from account lookup. Boot1
`logcat-after-boot.txt:253262,253958` and boot2
`logcat-midwatch.txt:195922,195946` record one
`LockedBootCompletedReceiver` ANR on each boot; boot2 line195989 says ActivityManager
killed the process for `bg anr`. `boot2/kgclient-anr-trace2.txt:90-94` shows the
main thread blocked in `IKnoxGuardManager.getTAInfo`. The same boot has
`TLC_ERROR` at line35040 and `@TimeoutException: getTAInfo - null` at line204112;
`dumpsys knoxguard_service` timed out after10s although `knoxguard_service` is
registered. The package was later restarted and remained running under kg_app,
but the TA path is not accepted as working. Do not disable KnoxGuard or infer a
hardware fix from this trace alone.

Other independent attempt56 errors remain: BPF `EINVAL` is logged at
boot2 lines7875/7883, and Bixby logs `IllegalStateException: No matching
ComponentResult found!` at line171618. The bounded stability monitor was
stopped after reproducing the KnoxGuard ANR; this is not a long-run acceptance.
Attempt55 remains retained. The G965F currently runs attempt56; M1/G965N were
untouched, and no M4/Android17 or production-kernel gate is passed. Private raw
evidence is under `logs/20261025-m2-attempt56/`.
