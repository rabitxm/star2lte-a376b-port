# M4 Android17 native connectivity provider diagnostic —27 October2026

Reason: after V6 and complete mapper fallback repair, SurfaceFlinger initializes
physical display0 but bootanimation/zygote cannot preload libandroid.so:
`cannot locate symbol "AConnectivityNative_getNetworkBlockedReason"`.
The paired S22 Tethering/Resolver passes loader/netd exports but cannot provide
this newer public native API. Replacing libandroid_net alone does not fix the
direct libandroid.so import. See docs/m4-bpf-runtime-audit.md.

## Exact composition and scope

Patch0195 adds only a diagnostic builder and independent lock to the existing
fork vendor-restore module. It calls the existing native32 tool wrapper and
uses the fork's ext4/e2fsdroid/AVB/signapk tools plus explicitly pinned existing
local native32 signing identity. It does not add a ROM build system, choose
itself in a profile, replace normal M4 runtime locks or flash anything.

Keep S22 Tethering version360743340 and change only
`lib64/libcom.android.tethering.connectivity_native.so` inside its payload:

| Input/output | SHA256 |
| --- | --- |
| Original S22 CAPEX | 601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf |
| BZI6 CAPEX providing real native library | 540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d |
| Original S22 native library | d0c794cedcd1cf6a8f7639a5ae044a9a7cea827e9aee32f4b8e6f15a353b792c |
| Real BZI6 native library | 70399052a18d74bfb639242300a5141dfbc15e30f731b277bd1d4f87543b5264 |
| Composed, locally signed APEX | ca9e7b7c76f85baf306b37cdcbe74b4e03721bc2520e869a9ecd7d1e7a517916 |

Output is67,911,680bytes. It uses the existing local diagnostic certificate
and payload key, not the original Samsung/Google signing identity. The actual
BZI6 function performs getsockopt and returns negative errno on failure; no
constant-success stub or fabricated blocked-network result is added. Its
behavior and the special socket option on the diagnostic kernel still require
runtime validation. Old Java/JAR/BPF behavior is not accepted as complete A17
networking merely because this native provider loads.

## PC verification

Private evidence: `logs/20261027-m4-network-native/`.

- Exactly one payload file changes. Every other file/link and all75 mode,
  UID/GID and SELinux metadata records match the S22 payload. Manifests,
  classpath tables and other nonsignature container metadata remain identical.
- Payload AVB and container APKv3 verify; stored payload offset8192 is4KiB
  aligned. Three independent clean builds, including the actual source helper,
  produce identical SHA256 and size. The helper requires the locked output.
- Native consumer audit finds no remaining S22 APEX user of the three old
  generated C++ default-implementation exports removed by BZI6. Its additional
  binder imports are supplied by the installed hash-matched BZI6 binder_ndk.
  This is static evidence; CFI/callback/kernel/API behavior is still open.
- Builder preflights caught `searching for label "/priv-app/ServiceConnectivityResourcesGoogle@360743340"` caused by unnecessary hyphen escaping in
  file-context stems. Removing that escape preserved the exact target label.
  A second check rejected lost+found UID/GID1000 versus original0; the existing
  native32 recipe's explicit root ownership normalization corrected it.
  Failed build logs remain retained; no failed build was installed.
- 185 ordered source patches replay to128 byte-identical files;184 older hashes
  remain unchanged. No kernel or normal ROM profile changed for0195.

## Device test and rollback contract

The next trial inherits the already measured0189/0190 vendor/logger, existing
WIPEONFORK/probe13 BOOT, S22 resolver, additive V6 and0194 libui. Only one SYSTEM
APEX file changes from the previous comparison. Guard serial/model/bootloader,
partition size/path, old/new APEX hashes and metadata, plus exact libui/V6/
resolver hashes. Check netd/framework/services bytes and measured partitions
unchanged before/after this write. Keep exact original M4 APEX and full
BOOT/VENDOR rollback files. No data format, new kernel or direct ODM/EFS/
CPEFS/modem write is part of the diagnostic.

On failure, collect fresh CACHE/mmap/pstore before restoring both BZI6 APEX
files, original libui bytes/metadata, remove V6, restore full BOOT/VENDOR.
SYSTEM rollback is file-level; raw ext4 identity is not claimed. Across-boot
EFS/ODM hash changes are tracked separately from strict per-write comparisons.
No new complete ROM is published or substituted for retained M1/M2/M4 releases.
The current device result belongs to `logs/20261027-m4-native-provider-test/`
and will be appended after capture. The normal M4 build still selects native
BZI6 Tethering until explicit build/real-image integration is implemented.

## First runtime observations — completion not accepted

The guarded one-APEX delta passes, including unchanged BOOT/VENDOR/resolver/
V6/libui/netd/framework/services and per-write protected partition checks.
At225seconds Android ADB appears briefly, allowing a9.6MiB logcat and kernel
log capture. SELinux reports Enforcing. The captured log proceeds into real
SystemServer and BootAnimation without the prior native-symbol preload fatal.
`sys.boot_completed` is still unset. The user confirms that the screen is
showing boot animation and initially keeps it undisturbed.

Later short ADB windows show uptime410s, then102s on a subsequent capture,
establishing a later kernel reboot. Fast capture obtains system_server PID1812
on a later boot and reaches:

```text
fast-001-system.txt:1821 D SystemServerTiming: OnBootPhase_500_com.android.server.ssrm.CustomFrequencyManagerService$Lifecycle
fast-001-state.txt: bootanimation running; zygote running; sys.usb.state=sec_charging,adb; boot_completed unset; SELinux Enforcing
```

The last captured phase is an investigation boundary, not proof that CFMS is
the root cause. Short USB windows and `settings ... Broken pipe (32)` do not
establish disabled adb_enabled or an Auto Blocker regression. The existing
USB gadget log also contains `Unable to write ... /config/usb_gadget/g1/UDC:
No such device` and missing gadget HAL declarations; these require correlation
with the complete new logs before a change. No forced-ADB loop or new property
version spoof was added.

Private evidence: android-logcat-private.txt, android-dmesg-private.txt,
extended/fast captures and watch state under
`logs/20261027-m4-native-provider-test/`. The helper source is now patch0195;
normal M4 profile integration and full real-image/compiled/fresh-boot acceptance
remain open. Source185 replays to128 matching text files with184 previous
patch hashes unchanged.

Historical checkpoint before the recovery result below: user had been asked to return only the designated
G965F to TWRP so CACHE/pstore/ANR can be collected and diagnostic components
rolled back. At that earlier checkpoint no rollback of the last candidate was claimed.
Do not overwrite EFS/CPEFS/modem or erase data while retrieving evidence.

## Recovery evidence and completed rollback — source0196

The designated G965F returned to verified TWRP. All CACHE logger files, seven
ANR traces, pmsg and probe13 mmap were copied to the PC with a65-file SHA256/
size manifest. Fresh mmap generations44/45 belong to this trial; generation45
filled its1MiB slot and reports213 dropped frames, so it is not a complete
kernel transcript. Older slots are not new evidence.

The full log explicitly completes CFMS:
`extended-logcat-0171.txt:66321 ...CustomFrequencyManagerService$Lifecycle took to complete: 5105ms`.
It then records the actual fatal exception, repeated across SystemServer PIDs:

```text
fast-004-crash.txt:3 java.lang.NoSuchMethodError: No static method getL4sDeveloperOption(Landroid/content/Context;)I in class Landroid/net/ConnectivitySettingsManager;
fast-004-crash.txt:4 at com.android.server.connectivity.IpConnectivityMetrics.onBootPhase(...:129)
fast-004-crash.txt:12 FATAL EXCEPTION IN SYSTEM PROCESS: main
```

All seven ANR main-thread stacks wait in FutureTask.get ->
KnoxGuardNative.executeMethodWithTimeOut -> performTAApiCallWithRetry ->
getTAStateResponse -> KnoxGuard30Service constructor. The logs contain
`KG30.KnoxGuardTA30Native: @TimeoutException: getTAStateResponse - null` and
`KgTaResponse detail : TLC_ERROR`. This is a real preceding TA compatibility
stall; it does not justify blaming CFMS or disabling KnoxGuard/SELinux. The
subsequent fatal Java API mismatch is a separate, directly evidenced blocker.

Rollback of that trial is complete: both original BZI6 APEX files, original
libui metadata, V6 removal, full BOOT SHA256 cc618abc... and full VENDOR
025f7314... verify. Per-write protected hashes are unchanged. Across the real
Android/recovery interval EFS and ODM raw hashes differ, recorded separately
in `across-boot-hash-changes.json`; no direct write/restore to either was issued.
SYSTEM restoration is file-level, not a raw ext4 identity claim. Device stayed
in TWRP after rollback; no fresh-data format.

Patch0196 adds an explicit `--l4s-compat` option to the existing diagnostic
APEX helper. It retains0195's real BZI6 native provider and adds only four
actual BZI6 L4S method bodies plus five constants to two S22 Java classes.
Getter reads Settings.Global `l4s_developer_option`: missing=AUTOMATIC2,
`"1"`=ENABLED1, other present values=DISABLED0. Setter preserves donor0/1/2
handling and IllegalArgumentException for invalid input. No fixed-success or
synthetic API result is introduced. SOURCE-retained annotations do not affect
runtime calls and are not added as new classes.

Verification: all1684 classes and every original method/annotation survive
DEX round-trip, with only the equivalent omitted static-false encoding for
two unchanged fields. Actual installed services.jar SHA256
c05208d9a8f06fbe88caa2576dbac613ed6cd0564195f8bed1fb5ea154774618 has
seven ConnectivitySettingsManager method-id references across all its DEX;
all resolve, including the new getter and setter. This limited audit is not
complete Java/networking ABI acceptance.

Both donors use DEX039. The bundled smali tool rejects API36 output with
`dexVersion must be within [0, 999]`; its API28 path cannot preserve hidden-API
restrictions. API29 selects the actual DEX039 format with those restrictions,
without altering any Android SDK property. Multi-thread assembly produced
nonidentical DEX layouts; sorted input plus one assembly thread makes three
clean builds byte-identical. All failed PC outputs remain private and were
never installed. Final JAR SHA256:
91e443b8a6705ac3f20d986b70b449ceee604a821a43cbdb80ed07a2c4b4b4c7.
Final diagnostic APEX SHA256:
5639fb701cd90455fad7b7925097229bde36fe314cf968976353f764a12e0b7d
(67,911,680bytes). Both changed payload files, all75 metadata records,
AVB/APKv3 and payload offset8192 verify. Native-only default rebuild still
produces0195 ca9e7b7c...; normal M4 profile remains unchanged.

186 patches replay from the pinned fork to129 identical text files; all185
previous patch hashes remain unchanged. Private evidence:
`logs/20261027-m4-network-java/`. Next bounded device trial is isolated in
`logs/20261027-m4-l4s-test/`, with original rollback payloads and refreshed
recovery identity/hashes; its outcome must be recorded separately.

## Source0196 bounded runtime result — new network Java boundary

The guarded trial passes all writes/protected comparisons, using the same
existing diagnostic BOOT and restored0189/0190 vendor/logger, V6,194 libui
and S22 resolver. Only the Tethering Java additions differ from the prior
native-provider comparison. No data format or new kernel build was issued.

ADB appears at131.3s, with intermittent short windows. Runtime now directly
proves that the formerly fatal L4S phase executes successfully:

```text
fast-003-system.txt:1656 D SystemServerTiming: OnBootPhase_500_com.android.server.connectivity.IpConnectivityMetrics
fast-003-system.txt:1660 D SettingsProvider: PUT_ret(/global/l4s_developer_option), callingPackage:android
fast-003-system.txt:1661 V SystemServerTiming: OnBootPhase_500_com.android.server.connectivity.IpConnectivityMetrics took to complete: 24ms
```

The next fatal is a different missing Java API, on the background thread:

```text
fast-006-crash.txt:3 FATAL EXCEPTION IN SYSTEM PROCESS: android.bg
fast-006-crash.txt:4 java.lang.NoSuchMethodError: No virtual method querySummary(Landroid/net/NetworkTemplate;JJI)Landroid/app/usage/NetworkStats; in class Landroid/app/usage/NetworkStatsManager;
fast-006-crash.txt:5 at com.android.server.stats.pull.StatsPullAtomService$$ExternalSyntheticLambda5.queryNetworkStats(...:46)
```

boot_completed remains unset; Enforcing and bootanimation/zygote running were
observed. The subsequent short ADB window allowed a verified bootloader check
and `adb reboot recovery` request. The same device arrived in verified TWRP
without requiring another manual key reset. CACHE/ANR/pstore/mmap were copied
with a66-file manifest; mmap generation46 is new and reports zero dropped
frames. Two ANRs are new compared with the previous capture. The finite CACHE
logger and individual short ADB buffers are not a complete system transcript.

The read-only actual-services audit was expanded to all three network Java
provider JARs and their known S22 superclass chains, with196 L4S included.
Among385 method-ID references into BZI6-defined network provider classes, five
BZI6-declared signatures are absent from the retained S22 provider:

- NetworkStatsManager.querySummary(NetworkTemplate,long,long,int)
- NetworkStatsManager.querySummaryForDevice(NetworkTemplate,long,long,int)
- NetworkStatsManager.queryTaggedSummary(NetworkTemplate,long,long,int)
- NetworkTemplate.Builder.<init>()
- NetworkTemplate.Builder.setTransportType(int)

No donor-only class reference was found within that limited services method
scan. This does not certify inherited external ancestors, fields, other
framework/app consumers, Binder transactions or Parcelable wire semantics.
Private reproducible scanner/result:
`logs/20261027-m4-network-java/audit-services-network.py` and
`services-network-abi-audit.json`.

Next change must cover these actual API consumers and their dependency closure,
not stub statistics, discard flags or replace whole Java/Binder families on
assumption. The new donor query uses sanitizeQueryFlags and a separate
mAugmentWithSubscriptionPlan field absent from S22; donor NetworkTemplate
builder/serialization also needs explicit old/new comparison. Keep the pinned
kernel, HAL/vendor/radio architecture and195 native API intact while resolving
that Java interface boundary. Live network accounting remains unaccepted.

Private device evidence: `logs/20261027-m4-l4s-test/`. Final rollback PASS: original BZI6 APEX pair/libui and V6 removal at file
level; full BOOT cc618abc... and full VENDOR025f7314... match original hashes.
All per-write protected comparisons pass. EFS/ODM again differ across the real
Android/recovery interval, recorded separately after excluding the controlled
BOOT probe write. No direct writes/restores to EFS/ODM were issued. The device
is left in verified TWRP, without another reboot or data format. See
operation-status.json, system-rollback/result.json,
network-pair-boot-rollback/result.json and vendor-rollback-summary.txt for the
completed checkpoint.
