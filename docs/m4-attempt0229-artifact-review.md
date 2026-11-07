# M4 attempt0229 artifact review

Completed 30 October 2026. This is a complete host package; it is not yet an
accepted device build. Build0229 exited 0 after the package-staged flow reran
compiled, image and flashable-zip gates for the completed work tree.

The run is `rom/out/releases/M4/star2lte-1793349848-jzZbHB/`. It uses the
existing fork, M4 G965F Android17/BZI6 profile, Hybrid Vendor VNDK33 and the
G965F hardware/RIL inputs. Its selected kernel is the diagnostic DS-ACK V1.12
G965F WIPEONFORK Enforcing package, without KernelSU; this is permitted by the
recorded WIPEONFORK runtime evidence and does not change the production kernel
selection.

The flashable ZIP SHA256 is
`f90b29a1ae4d488c1384b3676d25631c512ddeb172eb966861293be674fe57b5` and its
size is 5,204,413,980 bytes. The target-files ZIP SHA256 is
`76f615679c13d72c5f3aaa951da036ec274265dd986c32ae34acb931497f0238` and its
size is 5,603,392,155 bytes. Both are in `SHA256SUMS`; `.complete` is present.
Eight ordered signed ZIP32 sideload stages were generated and checked for CRC,
OTA signature, classic ZIP32 bounds, transfer order and final-stage behavior.
Their hashes are in `SHA256SUMS.legacy`; the manifest records the source system
stream hash. The signed full-package verifier reports ext4, Enforcing and six
device payloads.

The final SYSTEM, VENDOR and ODM ext4 images passed `e2fsck` and the M4 image
gate. It confirmed 50 hash-locked SYSTEM files including the AOD payload
`b374094fe87c6ab9f231096ddad4cd1bb0945be74c3247c9a441af8a3262b9f3`, target
identity values, SELinux/fs_config metadata, and preserved native32 vendor
closure. Compiled gates passed for ADB/no-RSA defaults and stored user-choice
behavior, Play Integrity Fix integration, donor-native runtime closure, and
the rebuilt BZI6 camera output. These host checks do not establish that every
device function works at runtime.

The signed BZI6 Bixby donor APK remains unchanged. On the earlier installed M4
image it logged `java.lang.IllegalStateException: No matching ComponentResult
found!` at `logs/20261030-m4-remaining/identity-bixby/crash-buffer.txt:16`.
The user chose to keep the signed donor and accept this receiver crash. No
attempt to resign or replace the APK is included.

## Device result — 31 October 2026

The verified device was G965F `M2_G965F_SERIAL`, identified in recovery as
`SM-G965F` with bootloader `G965FXXUHFVG6`. Fresh installation completed all
eight signed ZIP32 stages. The initial streamed stage-2 attempt failed with
`failed to write to adb host: Success` and `Updater process ended with signal:
7`; the partial SYSTEM write was restored byte-for-byte before switching to
chunked transfer and local TWRP installation. The first stage-8 run could not
unmount `/sdcard` (`umount: can't umount /sdcard: Device or resource busy`) and
reported `E9810: Failed to format cache/omr/preload/data.` A retry from TWRP
tmpfs then found a stale extraction entry and failed with
`unzip: can't read standard input: Not a typewriter` / `E9810: Failed to
extract kernel installer.` Clearing only `/tmp/exynos9810/kernel` allowed
stage 8 to complete with `Updater process ended with RC=0`, marker
`next=done`, and a new USERDATA filesystem UUID. The source fix for future
packages is patch0227; attempt0229 itself does not contain that fix.

After install, SYSTEM/VENDOR/ODM/BOOT/CACHE/OMR/HIDDEN hashes, both GPT copies,
and protected CPEFS/RADIO/RECOVERY readbacks were recorded in
`logs/20261030-m4-g965f-test-0229/postinstall-device-verification.json`.
SYSTEM/VENDOR/ODM read-only `e2fsck -fn` checks exited 0. BOOT remains the
baseline DS-ACK image; no kernel, vendor, radio or bootloader image was changed
by a diagnostic kernel build. EFS hash changed between prewrite and post-attempt
observations (`f74eaf38…` to `2f4d830a…`) despite no updater command targeting
EFS; the cause is unknown, so this is recorded as an unexplained observation
and no EFS write/restore was attempted.

The phone booted Android 17 / SDK 37 / One UI 90000 with
`sys.boot_completed=1`, `ro.product.model=SM-G965F`,
`ro.product.device=star2lte`, and bootloader `G965FXXUHFVG6`. SystemServer,
SystemUI and Bluetooth PIDs stayed stable during the captured two-minute
sample; window state reported systemReady/systemBooted and no ANR. Captured
logcat had no `Fatal signal`, linker, SELinux denial, or the earlier
`RANGING` permission fatal; the known Bixby fatal is documented below. Setup
Wizard has since been completed. The device is currently on the Rampart
Auto Blocker settings page; the visible switch reads `Tắt` and its USB behavior
explanation is present. Screenshot evidence is retained in
`logs/20261031-m4-postsetup-0230/rampart-settings.png`.

ADB policy is active on the live boot: global `adb_enabled=1`,
`development_settings_enabled=1`, `ro.adb.secure=0`, and
`persist.sys.usb.config=sec_charging,adb`. Secure settings show
`rampart_main_switch_enabled=0`, `rampart_blocked_adb_cmd=0`, and
`rampart_blocked_usb_data_transfer=0`; the live Rampart screen shows `Tắt`
with the USB behavior explanation. Compiled image gates for keyless ADB and
stored user-choice behavior passed. Runtime ON/OFF toggle was not tested.
Wireless ADB is enabled in the image and remains a separate audit item.

The user accepts Bluetooth as PASS; phone pairing and two incoming OPP files
were directly tested, and the current boot shows Bluetooth ON/zero crashes.
A2DP/AVRCP/HFP audio was not separately measured and is not a blocker under
that acceptance. M4 remains under test: NFC tag behavior is unresolved because
the only physical trial so far placed the tag at the phone bottom, which is not
confirmed to overlap the antenna area. `service call nfc 6` reports state3 (ON),
while `dumpsys nfc` returns `FAILED_TRANSACTION`; earlier logcat recorded
`NFA_EeSetDefaultProtoRouting: Bad ee_handle` and a T3T route failure. App-level
networking, SIM calls/SMS/data, camera capture, GNSS, thermal behavior, and the
Auto Blocker functional toggle also remain open. Brightness was visually
confirmed to work. The signed BZI6 Bixby receiver crash remains an accepted
limitation by user choice; its APK stays unchanged.

Live DT/sysfs inspection corrected the earlier I2C attribution: repeated
`14370000.hsi2c` NO-ACK messages belong to
`/hsi2c@14370000/mfc-charger@3b`, not NFC. The NFC controller is
`/hsi2c@108E0000/sec-nfc@27`. See `docs/m4-nfc-functional-test-0231.md` and
`logs/20261031-m4-postsetup-0230/nfc-diagnostic.txt`.

Build and stage logs are in `logs/20261030-m4-remaining/`; install, rollback,
partition checks and postboot evidence are in
`logs/20261030-m4-g965f-test-0229/`.

Patch/report coverage: 0225 fixes donor partition identity; 0226 validates the
exact AOD payload installed by the later module; and
`rom/scripts/internal/verify_m4_runtime_image.py` includes the exact
`framework-res.apk` metadata needed by the final ext4 image. The reason,
affected files and verification are recorded in `docs/patch-matrix.md` and
`sources.lock.json`. Patch0227 clears only the temporary kernel extraction
directory before unzip; the clean tmpfs retry verified the procedure, and the
patch records it for future builds. Rollback is by restoring the previous
retained M4 package or captured G965F partition images; this installer does not
write EFS, CPEFS, RADIO, RECOVERY, BOTA or bootloader partitions.
