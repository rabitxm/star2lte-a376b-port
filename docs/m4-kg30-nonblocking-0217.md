# M4 KG30 nonblocking startup lookup — patch0217

Updated 29/10/2026. This is the first repair unit in the post0217 plan.

## Evidence and cause

The M4-0217 first-boot trace shows SystemServer blocked for about67 seconds and
then killed by the framework watchdog. The original ANR is
`logs/20261029-m4-validation-0217/firstboot-watchdog-anr.txt:277–295`: the main
thread is in `FutureTask.get()` through
`KnoxGuardNative.executeMethodWithTimeOut()` and
`performTAApiCallWithRetry()` while `KnoxGuard30Service` reads the TA state in
its constructor. The worker stack at lines254–269 is in
`ServiceManager.waitForService()` while holding the KG30 controller lock. The
G965F Hybrid Vendor VNDK33 image has no KG30 AIDL provider declaration or init
service, so that lookup cannot complete on this hardware stack.

The donor controller already defines a structured fail-closed response:
`generateFailCloseResponse(0xff02)`. `getTAStateData()` maps an unsuccessful
response to its existing state5 and the utility writes that state to
`knox.kg.state`. The repair preserves those paths. It does not report TA success
or set an unlocked state.

## Change

Patch0217 changes only the lookup in
`KnoxGuard30VendorController.getService()` from
`ServiceManager.waitForService()` to the Android17 donor's existing
`ServiceManager.checkService()`. If the service is absent or its binder is dead,
the existing null branch reaches the `0xff02` error response. Binder conversion,
`pingBinder()` validation, `safeCall()` lock/finally cleanup and the native
retry policy remain pinned to the donor source: at most3 calls with100ms delays;
each missing-service call now completes instead of waiting25 seconds.

Affected source files:

- `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`
- `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/a376b_android17_kg30_nonblocking.py`
- `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/a376b_android17_kg30_nonblocking.lock.json`
- `rom/scripts/internal/verify_m4_runtime_image.py`
- `patches/fork/0217-avoid-bzi6-kg30-service-wait-on-m4.patch`
- `patches/fork/series` and `sources.lock.json`

The helper is pinned to the exact BZI6 `services.jar` SHA256
`efb433ade1059327c8a591731839899e7f6a6da1d7e7895f6e5bc996a182170e` and
decoded controller class. It rejects any profile other than M4 / BZI6 / SDK37 /
star2lte / G965F. The ext4 image verifier checks that the compiled
`system/framework/services.jar` contains the reviewed bytecode and retains
mode0644, owner0:0 and the donor's `system_file` context.

## Host verification

- The pinned donor class changed from SHA256
  `9286fe8e7ba8797c1da5dbc9dfac67f5304fde69c5b40954c82db3a59e8a5fdd` to
  `7cc6d0437cc75c808e80348b77f879055d6fd7d80b505675b097df31ebdf507c`; no other
  KG30 controller method changed.
- `apply-work` and repeat `verify-work` pass; a wrong donor SHA and wrong M4
  scope both fail closed.
- Apktool assembles a donor fixture and the decoded DEX confirms exactly one
  `checkService()` lookup, no `waitForService()` in `getService()`, the existing
  `0xff02` branch, state5 mapping and bounded retry contract.
- The same decode/apply/build/decode sequence passes against the already built
  M4-0217 `services.jar`. The temporary compiled fixture SHA256 is
  `761953a28ec20e06ab5b0cd4a0de25e3af21ddc6cc949070dcce9fbb4ac4604f`.
- Patch0217 applies to an isolated fixture reconstructed from the0216 preimage;
  its four changed/added fork files replay byte-identically. `bash -n`, Python
  syntax, source-lock JSON and all recorded patch/helper/lock hashes pass.

## Runtime result — build0218

Patch0217 is integrated in the signed M4-0218 package at
`rom/out/releases/M4/star2lte-1793251536-X0X1kH`. The full ZIP SHA256 is
`58526b9c2d628efd74ac2dfd11039bcb4ba1cd583c6b504f76b84ee575dd159d`; target
files SHA256 is
`e9c05a11dd783183d9a2703a6af7902cea9fe8bdf51edc64eb97ce35d2db2035`.
The package checksum manifests and all eight legacy ZIP32 stage checks passed.

On the authorized G965F (`M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`), a
fresh install/format reached `sys.boot_completed=1`, SDK37 / One UI90000, with
SELinux Enforcing. Three cold boots completed; the first SystemServer PID stayed
stable for120s, the next cold-boot PID stayed stable for60s, and the final boot
remained stable for60s. The captured `/cache/unica` logger reached post-fs,
post-fs-data and boot-completed. Across the collected logcat there were0
`WATCHDOG KILLING SYSTEM PROCESS`,0 `ANR in system_server` and0 `Fatal signal`
entries.

The vendor provider remains absent. Runtime attempts log
`ServiceManager returned null service`, followed by `KgTaResponse errorCode :
15728643`; the lookup returns promptly, and the existing fail-closed path is
retained. `knox.kg.state` did not provide a valid state in this observation, so
this is not a KnoxGuard-open or service-present PASS. Provider-present,
binder-death/reconnect, and an available-but-unresponsive provider (which still
uses the donor25-second remote-call timeout) remain untested.

The next isolated unit is the RTC label repair in
[M4 RTC patch0218](m4-rtc-label-0218.md). No kernel, vendor blob, radio,
EFS/CPEFS, modem or bootloader content changed during the KG30 unit.

## Rollback

Remove0217 from `patches/fork/series` and its entry from `sources.lock.json`,
reverse the M4 finalization helper call and image verifier additions, then
rebuild `services.jar` from the pinned BZI6 donor. Keep M4-0218 available as the
runtime rollback until later candidate gates pass.
