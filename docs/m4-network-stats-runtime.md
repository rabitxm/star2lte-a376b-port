# M4 network statistics and transport API diagnostic — 28 October 2026

Reason: source0196 passes real IpConnectivityMetrics phase500 (24ms), then
StatsPullAtomService on android.bg crashes with
`NoSuchMethodError: No virtual method querySummary(Landroid/net/NetworkTemplate;JJI)Landroid/app/usage/NetworkStats;`.
Evidence: logs/20261027-m4-l4s-test/fast-006-crash.txt:3–5.
Actual installed services.jar method-ID audit also finds two other query
variants and NetworkTemplate.Builder's default ctor/setTransportType absent.

## Source0197 scope and semantics

Extend the existing diagnostic APEX builder with explicit `--stats-compat`,
which retains195's real native provider and implies196 L4S. Normal ROM
profiles/build locks and retained complete ROM packages remain unchanged.
Only the native library and two Java JARs differ from the S22 payload:

- Keep the S22 NetworkStatsManager and original constructor/old methods.
  Donate exact BZI6 querySummary/querySummaryForDevice/queryTaggedSummary
  overloads, sanitizeQueryFlags and augmentation setter. Add donor boolean
  and mask constant. The existing constructor calls the setter(true), which
  preserves mFlags and initializes the new augmentation state. Flags outside
  mask0x3 still throw; valid flags retain actual polling and augmentation bits.
- Donate the shared BZI6 NetworkTemplate/Builder, NetworkIdentity/Builder and
  NetworkIdentitySet unit. It captures actual NetworkCapabilities transports,
  validates generic builders, matches transport bitsets and preserves them
  through Parcel and version8 disk records. The donor reader accepts old
  versions including7 and uses actual legacy-type deduction for missing bits.
- Retain the old Template constructor with zero generic bitset for legacy
  rules, the old Identity constructor with donor transport deduction, the old
  IdentitySet writer routed to v8, and the old TYPE_TEST field. All original
  method signatures/static kinds and field types remain present.
- Donate actual deduceTransportTypeForLegacyNetworkType and intArrayToSet
  utility methods. Dependency audit caught the second method before any
  candidate was installed. No NetworkStatsService implementation or Binder
  interface is replaced; the unit is shared through the existing boot JARs.

S22's synthetic IMS identity builder follows the same legacy setType path as
BZI6's corresponding service implementation; no speculative extra transport
copy or service rewrite is made. Old Binder interfaces/NetworkStats client
remain byte-identical in the generated DEX semantics. Hardware radio/vendor
and the existing diagnostic BOOT/kernel are unchanged by this new delta.

## Locks and PC verification

Private inputs/results: logs/20261028-m4-network-stats/.

| Artifact | SHA256 |
| --- | --- |
| Original S22 framework-connectivity-t.jar | 2953c4396e9a9548e00be6b5ed610b8b62e0ea01da78543d9ec8d76f09487830 |
| BZI6 framework-connectivity-t.jar | fae20cc45231ef0fc887db4e5363c62a939798d6fbe8731b7064f2f035839393 |
| Resulting framework-connectivity.jar | cb000f867b4a969954b157f50a1c07920fc047a3232ee51086dc0fb0ace541f3 |
| Resulting framework-connectivity-t.jar | bd65d527e221beec24f329ff0cb980a52c8a024075fb004121062bb1314bb7e8 |
| Diagnostic APEX (67,911,680 bytes) | f17fa222e1f127d349e61b592c705c127f682ad4d22dda164c03397857862cc6 |

All2161 main/t classes round-trip; exactly8 intended classes change from196,
with no old method/static kind or field type removed. Label spelling is
normalized without changing branch-target identity, quoted strings or field
descriptors; negative branch/field-type checks pass. Existing SOURCE-only
annotations do not require adding unused annotation classes.

Three composition outputs have identical hash; two complete final fixed-lock
runs pass. Payload75 metadata records, AVB/APKv3 and8192-byte payload ZIP
offset verify. The L4S-only196 path still produces its original hash.
All385 installed-services references into donor network provider classes now
have no missing donor-declared signatures. All36 introduced references were
audited: no missing provider member remains; standard java.lang.Long.compare
is an explicit runtime-fixture check. This scoped scan does not certify all
app/framework clients or live traffic accounting.

Initial PC gates rejected custom Smali label spelling during round-trip;
control-flow-preserving normalization corrected that comparison. They also
caught a missing CollectionUtils.intArrayToSet dependency. The first output
of each calibrated lock failed only the earlier output hash gate. Failed PC
outputs were never installed. Final source187-patch replay gives130 identical
files from the pinned fork, with all186 prior hashes unchanged.

## Runtime fixture and rollback

Source: support/diagnostics/m4-network-stats/M4NetworkStatsFixture.smali and
its README. Run only as an app_process child after the diagnostic module is
active. In-memory tests cover actual augmentation flags/invalid flags,
transport builder rejection, Wi-Fi positive/cellular negative matching, both
legacy constructors, Parcel consumption/matching and v7/v8 IdentitySet IO.
No PID1, Binder service call or persistent-statistics write is part of the
fixture. Only exit0 plus its explicit PASS marker passes.

The directory paths were read from actual S22 NetworkStatsService.Dependencies:
getLegacyStatsDir uses Environment data/system/netstats; getOrCreateStatsDir
uses ApexEnvironment(com.android.tethering).getDeviceProtectedDataDir/netstats
when configured. Both exist on the designated recovery device. Because the
new writer may produce v8 records that the old reader cannot safely consume,
the trial snapshots both exact directories first, including file bytes, UID,
GID, mode, SELinux and mtime. These snapshots are private; no unrelated app
data, EFS or modem image is read/restored by that helper.

Trial: logs/20261028-m4-stats-test/ on the verified G965F only. Stage the
fixture with shell_data_file label, apply the already measured vendor/logger,
V6/libui, S22 resolver and existing WIPEONFORK/probe13 BOOT, then the new APEX.
Every stage verifies identity/hash/scope and protected partitions. No data
format or kernel build. Watcher captures crash/system/state first, runs the
fixture in short ADB windows, and tries a guarded recovery reboot on fatal.

On failure collect CACHE/ANR/pstore/mmap and fixture outputs before rollback;
restore original BZI6 APEX pair/libui and removeV6, full BOOT/VENDOR plus exact
netstats snapshot. Keep trial-created stats as private evidence before
restoring. SYSTEM rollback is file-level; full raw ext4 identity is not
claimed. Across Android/recovery EFS/ODM changes are recorded separately from
strict per-write comparisons. Device outcome and final restoration are recorded below.


## Actual0197 runtime result

G965F runtime fixture passed exit0: `M4-STATS-API-SELFTEST PASS flags transport
Parcel v7/v8 legacy-entry-points`. Evidence fixture-1-PASS.txt and the recovery
copy result/fixture-selftest.{txt,exit} in logs/20261028-m4-stats-test. The old
five missing methods are covered by the actual child; full boot is not passed.

A fresh SystemServer native crash follows, not the earlier Java API failure:
`signal 6 (SIGABRT)` in ActivityManager thread, source0197 fresh tombstone_00
(process uptime94s). Frame01 is libmeminfo BpfMapRO::abortOnMismatch+464 at
0x199e4; frame02 ReadProcessGpuUsageKb+140 at0x15c54. Debug.getGpuTotalUsageKb,
MemInfoReader and AppProfiler are the real caller chain. Disassembly identifies
the abort as the negative-fd branch, not proof of key/value map-size mismatch.
Exact BPF_OBJ_GET errno is still unmeasured. Old tombstones in the same folder
refer to earlier graphics/native-provider trials and are not new197 failures.

Captured136 files including CACHE/ANR/pstore/mmap/tombstones. A separate guarded
worker requested recovery successfully; no manual-key reset was required for
this capture. Android boot_completed was never1. Full BOOT and VENDOR restored
and read-back hashes match cc618abc and025f7314 baselines. Per-write protected
partition comparisons pass. SYSTEM's exact original APEX pair/libui are back
and additiveV6 is absent; file-level restoration does not claim raw SYSTEM
ext4 identity. Both narrow netstats directories are restored with exact byte,
owner/mode/SELinux/mtime comparison. The interrupted final VENDOR rollback was
resumed and completed on28 October; rollback_pending=false. Device left TWRP
before the next optional-map meminfo diagnostic. No fresh format/kernel build.
