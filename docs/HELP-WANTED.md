# Help wanted: A37 Android16 on Exynos9810 — camera and persistent metadata

Current status, 24 October 2026: M1 S22/Android16 boots on G965F. Diagnostic M2
A376BXXS5AZH4 (SDK36.1/OneUI80500) reaches Android with Enforcing. Latest tests
preserve aconfig metadata through reboot, make platform-init exit0, and produce
rear/front preview and JPEG4032x3024 /2640x1980 with full PC decode. SystemServer
and cameraserver retain their PIDs through uptime397.34s. Diagnostic edits are
rolled back and verified; M2 remains in TWRP. No complete production artifact,
G965N or Android17 acceptance is claimed.

## Proven sequence

1. Calibrated probe13 records vendor BoringSSL reboot_on_failure. Exact A37
   runtime linker reproduces MADV_WIPEONFORK EINVAL on pinned DS-ACK.
2. Isolated upstream-semantics WIPEONFORK backport, calibrated on M1, makes
   system/vendor BoringSSL self-tests exit0 on M2.
3. A37 Tethering APEX rejects kernel4.9 at its Android25Q4/kernel5.10 gate.
   Retained S22 Tethering APEX passes bpfloader; hotspot/API acceptance is open.
4. Exact retained S22 system64 graphics common V6 interface resolves the
   compatibility libgralloctypes linker failure.
5. Read-only metadata prevents flag storage loading, omits manifest RANGING
   permission and makes AppOps abort. Initial tmpfs proves causality; patch0100
   retains the fork's existing CACHE bind after vendor restore and mirrors donor
   labels. Two boots preserve marker hash/inode and RANGING remains dangerous.
6. The legacy vendor has zero aconfig packages. A valid empty v2 container
   (no A37 vendor flags) passes independent upstream readers and actual donor
   platform-init exits0. Unknown/nonempty vendor content is refused.
7. Native A37 cameraserver with the fork's existing 8-byte zero-max-buffer fix
   preserves framework/audio ABI. The retained Camera APK recognizes exact80500
   using V8_0 features. Adding the pinned system32 GLES1 wrapper resolves legacy
   gralloc loading. Rear/front basic preview and still capture pass.

Evidence, hashes, reasons/files/checks/rollback:
[camera/storage report](m2-camera-metadata-fixes.md),
[initial metadata causal test](m2-metadata-runtime.md),
[kernel provenance](m2-runtime-kernel-fix.md), [probe13](probe13-report.md).
Selected text/JSON evidence is under support/evidence. Firmware, photos,
images, raw logs, credentials and physical identifiers stay local.

## Help needed now

- Integrate the complete proven boot combination into explicit M2 build inputs
  and acceptance checks through the existing fork. Patches0099-0101 cover camera
  and storage; they do not silently select the diagnostic kernel, retained S22
  Tethering module or system64 V6 input as normal production defaults.
- Build a new independently retained full ROM and validate its final image,
  fresh-data boot and mandatory ADB/Auto Blocker defaults plus functional switches.
- Audit CACHE clearing/recovery/OTA/staged flag update behavior and other metadata
  consumers. Two normal reboots alone are not storage lifecycle acceptance.
- Validate video/audio, telephoto, flash, advanced camera modes and Gallery return.
  Missing optional camera vendor tags still produce warnings. No audio hardware
  pass follows from resolving the cameraserver/audio ABI mismatch.
- Validate hotspot, DHCP/DNS/IPv4/IPv6/data saver, radio/IMS and remaining hardware
  independently on F before any authorized N test. No self-test, permission gate
  or SELinux enforcement was bypassed to reach Android.

## Reproduction and constraints

Extend Eend15/Unofficial-Un1ca-9810 Android-16 at
`2b25ae6fc3098650be249bd1898e21e563cd5df1`. tools/prepare_fork.sh applies
91 active text patches numbered through0101; omitted numbers are deliberate.
All91 replay to72 touched paths matching the modified fork. Kernel diagnostics
and recorder source have their separately documented provenance.

Use Hybrid Vendor VNDK33 and variant-correct F/N hardware/RIL blobs, keeping the
fork's locked radio bridge where needed. Production kernel remains original
duhansysl DS-ACK V1.12 08/05/2026 enforcing/no-KernelSU. The successful diagnostic
uses the separately proven WIPEONFORK backport. Do not write EFS/CPEFS, modem or
bootloader. G965N has not been flashed; no M4 transition is justified yet.

Start with README.md, this brief, docs/repo-map.md and docs/patch-matrix.md.
The source snapshot excludes generated/proprietary outputs and retains role
aliases instead of physical serials. Acquire firmware/toolchains independently
and verify local targets before operations.

## Previous M2 boot blocker — resolved on attempt53, 24 October 2026

Attempt52 failed before boot completion with `Missing permission definition for permission "android.permission.RANGING" associated with app op 151`; init could not create `/metadata/aconfig` on read-only `/metadata`. Attempt53 confirms its scoped 16MiB tmpfs lets fresh-data aconfig initialize: `RANGING` is dangerous, RangingService registers, SystemServer stays alive and boot completes. This only verifies current-boot volatile metadata, not persistence or production acceptance.

## Current gate — attempt53 runtime

A one-time user authorization covered the exact attempt53 package hash on G965F `M2_G965F_SERIAL`, fresh USERDATA formatting and boot. All eight install stages passed and the first fresh boot reached `sys.boot_completed=1`. Fresh-data checks show `adb_enabled=1`, `development_settings_enabled=1`, `ro.adb.secure=0`, and Rampart logs `disabledRampart=true`. Attempt53 used the runtime-diagnostic profile, which does not enable the optional patch0095 CACHE logger; its absence is expected for this build and not evidence of patch failure. USB gadget init also logs `init.samsungexynos9810.usb.rc:382 ... No such device` while host ADB remains reachable. Next acceptance items: inspect actual Settings switches and test user changes persist without a forcing boot script; test camera/audio/radio/Wi-Fi and the rest of device hardware; keep metadata persistence and G965N/Android17 explicitly open. Do not reuse attempt53's one-time write authorization for a new package.
