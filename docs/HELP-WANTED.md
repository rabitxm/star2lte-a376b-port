# Help wanted: A37 Android16 on Exynos9810 — camera and persistent metadata

Current status, 25 October 2026: diagnostic M2 attempt56 was fresh-installed on G965F and booted twice under Enforcing. Patch0107's vendor_kg account integration is runtime-confirmed: UID5959 resolves to vendor_kg, the package enters kg_app, and the old lookup-miss/zygote-abort pair is absent. CACHE init markers/logcat work, while init-context dmesg/props snapshots remain empty. A separate KnoxGuard TA timeout now reproduces as a LockedBootCompletedReceiver ANR on both boots; BPF type support, Bixby compatibility and extended hardware/stability tests remain open. M1/G965N untouched; Android17 has not been built/tested.

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

- Diagnose missing init-context dmesg/props output without weakening SELinux. Late restart and runcon reproduce it; direct ADB output works.
- Investigate BPF program registration and actual GPU tracepoint/helper/map ABI. Minimal TRACEPOINT/PERF_EVENT loads return EINVAL while SOCKET_FILTER passes; CONFIG_BPF_EVENTS is absent. This is not yet a validated kernel fix.
- Diagnose the remaining KnoxGuard TA path without suppressing KnoxGuard: `TLC_ERROR` and `@TimeoutException: getTAInfo - null` leave `LockedBootCompletedReceiver` blocked in `IKnoxGuardManager.getTAInfo`, causing one ANR per boot. Account-name resolution itself is now verified.
- Diagnose Bixby `No matching ComponentResult found!` as a separate app issue.
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
97 active text patches numbered through0107; omitted numbers are deliberate.
All97 replay to80 touched paths matching the modified fork. Kernel diagnostics
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

## Current gate — attempt56 runtime (25 October 2026)

Patch0107 passed donor-pinned account fixtures, actual ext4 VENDOR audit and
fresh runtime checks. Eight install stages pass; boots complete in about197s and
84s. UID5959 resolves to vendor_kg; no lookup-miss or zygote SIGABRT appears in
the captured logs. Logger markers/logcat pass; dmesg/props remain empty under
init context. KnoxGuard TA requests time out and its LockedBoot receiver ANRs on
both boots; this is the next narrow diagnosis. Full provenance and exact log
references are in the [attempt56 report](m2-integrated-runtime-build.md#attempt56-completed--25-october-2026).
Raw evidence stays local in logs/20261025-m2-attempt56. The same WIPEONFORK
diagnostic kernel is retained; production selection is unchanged. G965N/M4
remain outside completed acceptance.
