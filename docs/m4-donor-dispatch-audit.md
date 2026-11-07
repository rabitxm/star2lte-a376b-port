# M4 donor and dispatch audit — 25/10/2026

Purpose: record the Android17 BZI6 evidence before adding an M4 source profile or
reusing Android16 donor-only branches. This is preflight evidence only; no ROM build or device write occurred during
this audit. M1/M2 source caches and retained packages remain unchanged.

## Verified BZI6 identity and API layers

| Evidence source | Observed value | Meaning for M4 |
| --- | --- | --- |
| `system/system/build.prop` lines 32–42 | PDA `A376BXXU5BZI6`, SDK 37, sdk_full 37.0, release 17 | Source framework SDK must be 37. |
| same file line 181 | One UI 90000 | Android17 donor identity. |
| `vendor/build.prop` lines 24–26 | vendor release 14 / SDK 34 | Do not treat vendor SDK as framework SDK. |
| `vendor/build.prop` lines 43–44 | `ro.product.first_api_level=36`, `ro.board.first_api_level=34` | Source product shipping API 36 and source board API 34; keep target hybrid VNDK33 policy separate. |
| same file line 56 | `ro.zygote=zygote64` | Donor is 64-bit zygote. |
| `odm/etc/build.prop` | ODM CPU ABI list arm64-v8a; no 32-bit ABI | Consistent with the A37 Android16 profile’s 64-bit application ABI. |
| `system/system/system_ext/apex/com.android.vndk.v34.apex` | Present | Stock donor includes VNDK34; M4 must still resolve to the required fork Hybrid Vendor VNDK33 on star2lte. |

Exact source hashes: system floating_feature.xml `76f9e27c92500562fe4163366c07b71e2ac41e510a57b643500e3a7b68aa1989`, vendor floating_feature.xml `59324c8572e9668db0d29cb09e7ab0247a6abff9a736d71fb447317d44933521`, system build.prop `5e25db79a1ba2ef3150b994c4c4920c7ded8f4369d9695f2ffc0261369d055e8`, vendor build.prop `ea579326a2ac418280334e75d38c31600ccee3e2de7014eacf7346ca5a76c0ce`.

## Android16 versus Android17 source floating features

The A37 AZH4 system floating feature file has 270 entries; BZI6 has 278. BZI6
adds 8 entries, removes none, and changes 3 existing values. The changes are
camera UW distortion timing, filter provider version, and framework frame split
count. The direct feature values currently consumed by the fork and present in
both versions remain the same: dual-speaker support, MDNIE mode, auto-brightness
control level, HFR default/mode/supported rates. BZI6 values are respectively
`TRUE`, `37905`, `5`, `120`, `2`, `60,120`. BZI6 also reports optical display
fingerprint through `SEC_FLOATING_FEATURE_BIOAUTH_CONFIG_FINGERPRINT_FEATURES`.
The vendor floating-feature file reports the same MDNIE/HFR defaults but
`LCD_CONFIG_CONTROL_AUTO_BRIGHTNESS=3`; consumers must use the system/product
source where the existing M2 config used value 5.

Some fork source variables do not have a direct `SEC_FLOATING_FEATURE` entry,
including eSE COS, SIM tray count and WLAN backoff. BZI6
`framework.jar` directly embeds RecordAlive version
`07010` in both `SemMultiMicManager.isSupported()` and `isSupported(I)`, and
`floating_feature.xml` gives SIOP policy `siop_a37x_s5e8845`; the donor APK
contains `dvfs_policy_default.xml` in the SamsungDeviceHealthManagerService
resource table.
Do not infer these from Android SDK or from a same-model product label. Trace
them to their BZI6 config/package/ELF source before adding them to an M4 profile.
Runtime hardware support still requires logs where applicable.

## Additional BZI6 feature evidence

The source feature record is persisted in
`logs/20261025-m4-preflight/config-feature-audit.txt`, which includes SHA256s
for every inspected APK/XML/config. Direct evidence now confirms: RecordAlive
`07010` in BZI6 framework methods; ACH ringtone=false and virtual vibration
sound=false in framework Rune code; dual-speaker=true in both floating-feature
files; system floating-feature MDNIE/HFR/brightness, optical fingerprint,
eSIM slotswitch and SIOP policy values; the DVFS default
policy resource in SamsungDeviceHealthManagerService; SystemUI cutout protection
false; PhotoEditor_Full; CameraX implementation/service; and the SecureElement
and framework strings `JCOP6.5U` / `NXP_SKU`.

Two optional implementations are absent from BZI6's system tree: camera action classifier
(`libVideoClassifier.camera.samsung.so` and
`libtensorflowLite2_11_0_dynamic_camera.so`), ARDoodle (`etc/ardoodle` and
`lib_pet_detection.arcsoft.so`). Their floating-feature claims are absent too. The current
M2 A376B branches already remove the first two claims when implementation is
absent. M4 must add an A17-specific dispatch to those same fail-closed checks
and verify the compiled floating-feature output; no A17 replacement feature
was found in this audit. Correction after checking the complete extracted cache:
although `CAMERA_CONFIG_GPPM_SOLUTIONS` is absent, `GlobalPostProcMgr.apk`
**is present** (5,485,571 bytes, SHA256
`409b23d911d63addc7ead0abae8771df5782474428c31eff746bb684388d488b`), with
its default-permission and privileged-permission XML files. Keep this package;
absence of the floating-feature key does not prove absence of implementation.

Decoded BZI6 `semwifi-service.jar` (SHA256
`978ba8cb40f93d76c75552deb67c1492475ce53b462a9ceb31c0fea2f198ab69`) provides
additional donor-only evidence: `SemFrameworkFacade.isMBOSupported()` returns
true; `SemWifiServiceImpl.getNumOfWifiAnt()` returns2; and
`SemSoftApConfiguration` reports SoftAP 11ax=true, 6GHz=false, dual-AP=false,
OWE=false, power-save=true, priority-traffic=false, concurrency=true,
Wi-Fi-sharing-lite=false, and 5GHz-country-based=false. Its string also reports
backoff=true and SAR-backoff=false. These are A37 donor defaults/capabilities,
not proof of S9+ chipset support. Do not copy 11ax, antenna count, or RF/backoff
settings to the target; M4 target behavior must continue to follow the
star2lte HAL/blob and reviewed target configuration. This resolves those
specific BZI6 donor defaults only, not the fork's detailed target RF/backoff
configuration or runtime capability.

The BZI6 `telephony-common.jar` (SHA256
`ec739c16fb1cd235404dee39181ee15c2bfaf16a3c8f1d8ba5179abddf1f0383`)
confirms `UiccController.onSimTraySwap()` sends `waterproof=true`, matching the
target's existing support. BZI6 `TelephonyFeatures.isOneTray()` is dynamic: it
reads active modem count and `ril.simslottype1/2`; no static BZI6 property or
CSC value sets the donor tray-count feature. The star2lte Hybrid Vendor
`init.baseband.rc` also consumes the target's `ro.vendor.multisim.simslotcount`
at runtime. M4 should keep the A17 runtime tray-detection method and validate
it from target radio logs instead of replacing it with a hard-coded donor
value. BZI6 `SemWifiCoexManager.CUSTOM_BACKOFF_TYPE` is
`CAM_BACK -1 -1 -1 9 -1 -1`, the same source value as AZH4; this resolves the
donor string, while target RF settings remain specific to star2lte.

The eSE metadata needs target-aware handling. BZI6 `vendor/etc/libese-nxp.conf`
sets `NXP_OS_VERSION=0x03`, whose adjacent comment maps to JCOP 5.2, while
`framework.jar` and `SecureElement.apk` initialize the donor COS/vendor to
`JCOP6.5U` / `NXP_SKU`. The stock G965F `framework.jar` (SHA256
`094305c5cefd472c6a6638cccf9a3ec784a8cdf4757de768735094f9f0b5f0d9`) contains
`JCOP5.2U` in a multi-COS lookup in `SemService`; this is not proof that
JCOP5.2U is the selected COS of this target.
The stock G965F also includes the eSE permission, SecureElement app and
`esecomm` libraries; its stock NFC configuration uses `/dev/sec-nfc`, and the
extracted G965F vendor has no `libese-nxp.conf`. BZI6's `UtilExtension` checks
`ro.boot.product.hardware.sku`, eSE feature support and country before enabling
SecureElement. The M4 Hybrid Vendor restore discards donor vendor files, so the
BZI6 `libese-nxp.conf` value is not a final-image input. However, the live
G965F boot SKU was read over ADB on the verified G965F M2 device: both
`ro.boot.product.hardware.sku` and `ro.boot.hardware.sku` are empty, as is
`ro.security.ese.cosname.default`. BZI6's SKU-based detection therefore needs
explicit review for this target. Setting the target COS to `none` clears
BZI6's COS field and sets `SemService.supportReeSpi=false`; this is a behavior
choice, not a neutral metadata substitution. Do not infer a target COS/vendor
from a lookup-list entry or claim runtime eSE support without service evidence.

The source RIL feature list, listed donor SoftAP defaults, waterproof-tray
support, dynamic SIM tray detection, and the fork audio inputs are resolved.
The static donor tray-count metadata, detailed target WLAN RF/backoff
configuration and target eSE adaptation remain open.

## Radio and RILD APEX source dispatch

BZI6's extracted vendor includes `vendor/apex/com.samsung.android.rild.signed.apex`
(SHA256 `bd6d9a4b025e095decc2c900b5dc4119b63000c846b3c35fcafd88dfee942aef`).
The fork's normal Exynos9810 path does not carry that donor vendor tree through:
`customize.sh` replaces the complete work vendor tree with the embedded
Exynos9810 VNDK33 baseline, and `exynos9810_boot_restore/customize.sh` repeats
that replacement after module application. For `star2lte`, target vendor delta
explicitly keeps the known VNDK33 tree. The embedded baseline has no RILD APEX;
the finalization module then restores the fork's coherent N770F IRadio 1.6
bridge files as a unit. Thus BZI6's added RILD APEX and A37 RIL/Wi-Fi hardware
blobs are donor evidence only, not inputs to the final target vendor under the
reviewed path. The final M4 image still needs an explicit negative check for
the donor RILD APEX and positive checks for the locked bridge files/hashes.

The BZI6 framework's `TelephonyFeatures.RIL_FEATURES` constant is exactly
`onebinary` (`framework.jar`, decoded smali class
`com/android/internal/telephony/TelephonyFeatures.smali`, field and initializer).
It does not include `satellite_carrier`. This resolves the source RIL feature
list for a future M4 profile. It does not establish target radio capability or
replace the target bridge. Static BZI6 `ro.multisim.simslotcount` and
`ro.vendor.multisim.simslotcount` values were not found in extracted build
properties/config text; BZI6 init consumes those properties at runtime, so the
source SIM tray-count and waterproof-tray flags remain unresolved pending an
authoritative donor config source.

The BZI6 RIL/Wi-Fi selected-file delta against AZH4 is recorded in
`logs/20261025-m4-preflight/vendor-a16-a17-radio-delta.txt`. The per-donor
hardware binaries differ, reinforcing that only the star2lte/fork vendor and
the locked radio bridge should reach the target image.

## ADB and Auto Blocker source-layout audit

The existing fork adapter passed its database-seed and `AdbService$Lifecycle`
source-smali preflights against BZI6. `AutoBlockerPreferenceController`, its
live rampart readers, settings-change broadcast, description updater, Settings
preference XML and `AdbSettingsObserver` remain present. Evidence and exact APK/
JAR hashes are in `logs/20261025-m4-preflight/adb-defaults-audit.txt`. This
supports using the current M2 adapter for M4 source layout; the compiled M4
image must still prove the ADB properties/DEX and a fresh-data runtime test must
prove toggle behavior. This audit does not mark M4 ADB acceptance PASS.

## 64-bit donor runtime with target-native 32-bit compatibility

BZI6 has a 64-bit-only runtime APEX, while the Exynos9810 VNDK33 vendor layer
still requires selected 32-bit runtime libraries. The existing merger was run
in scratch with the actual BZI6 runtime APEX and locked S22 APEX inputs. It
preserved every original A37 runtime payload file, overlaid the same 17 reviewed
32-bit paths, passed payload AVB and container V3 signature verification, and
produced candidate SHA256
`b6b559bf52dfb07d4c8f1d073b037f1d319b0366e3a732c3b56bbbb55583cdaf`.
The scratch candidate was removed after the audit. Exact record:
`logs/20261025-m4-preflight/native32-apex-compose.json`.

This proves only that the existing composition path can package a BZI6 64-bit
runtime with the selected legacy 32-bit support files. It does not prove that
the resulting 32-bit linker or libraries run under Android 17, nor that a
M4-specific cache/lock is wired into the build. M4 needs its own pinned BZI6
runtime/Apex lock and actual-system linker/runtime checks before profile use.

## Existing fork M2-only gates

A static scan of `rom/target/star2lte`, `rom/unica/patches`, `rom/unica/mods`,
and `rom/scripts/internal` found 23 files containing the A376B Android16 donor name, M2 milestone, or M2-only flags. These checks mix donor-specific method patches, boot-contract/release gates, feature omissions, and user-selected M2 boot-test limits; they cannot safely be globally renamed. M4 must review each branch against BZI6 before the first build.

Files and matching gate counts:
- `rom/target/star2lte/config.sh` — 2 occurrences
- `rom/target/star2lte/profiles/m1-g965f-no-ksu.sh` — 2 occurrences
- `rom/target/star2lte/profiles/m2-g965f-a376b-android16-no-ksu-init-debug.sh` — 1 occurrences
- `rom/target/star2lte/profiles/m2-g965f-a376b-android16-no-ksu-pstore-trace.sh` — 1 occurrences
- `rom/target/star2lte/profiles/m2-g965f-a376b-android16-no-ksu.sh` — 4 occurrences
- `rom/unica/patches/camera/customize.sh` — 2 occurrences
- `rom/unica/patches/saiv/customize.sh` — 2 occurrences
- `rom/unica/patches/legacy/customize.sh` — 5 occurrences
- `rom/unica/patches/signature/customize.sh` — 1 occurrences
- `rom/unica/patches/exynos9810_device_stack/customize.sh` — 4 occurrences
- `rom/unica/patches/product_feature/customize.sh` — 24 occurrences
- `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/customize.sh` — 4 occurrences
- `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_runtime_compat.py` — 1 occurrences
- `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh` — 16 occurrences
- `rom/unica/mods/choidujour/customize.sh` — 1 occurrences
- `rom/unica/mods/deknox/customize.sh` — 1 occurrences
- `rom/unica/mods/setupwizard/customize.sh` — 1 occurrences
- `rom/unica/mods/batteryhealth/customize.sh` — 1 occurrences
- `rom/unica/mods/paradigm/customize.sh` — 1 occurrences
- `rom/unica/mods/settings/customize.sh` — 1 occurrences
- `rom/scripts/internal/verify_exynos9810_boot_contract.sh` — 2 occurrences
- `rom/scripts/internal/apply_modules.sh` — 18 occurrences
- `rom/scripts/internal/verify_exynos9810_release_defaults.py` — 4 occurrences

## A17 class-layout spot checks (structural only)

Apktool v3.0.2-SNAPSHOT decoded the actual A16 and BZI6 `services.jar`, plus
A16/BZI6 `framework.jar`, `gamemanager.jar`, and `semwifi-service.jar` into
`/tmp/m4-bzi6-smali` (local scratch, not release input). Structural fixtures
PASS for both generations on the exact files/signatures below:

- `HdmVendorController` has no no-arg constructor in either services.jar;
  `TAProxy.smali` is absent in both. This supports retaining the two existing
  shipping-API skips after adding an explicit A17 guard.
- `CoreRune.FW_DYNAMIC_RESOLUTION_CONTROL` field and initializer assignment,
  `RefreshRateConfig.dumpProductFeature(PrintWriter,String,boolean)`, the
  GameDisplayListener constructor, `VrrManager` constructor and
  `isVariableRefreshRateSupported()` are present in both A16 and BZI6.
- BZI6 `SemWifiApChipSupportInfo.isSupportedSoftAp11ax()`,
  `SemWifiCoexManager.addBackoff(String,String)`, and the donor's
  `SemSoftApConfiguration` power-save feature output are present.

These checks establish class/method presence only. They do not validate exact
patch preimages, register behavior, compiled output, ABI compatibility, or
runtime HAL behavior; each actual M4 edit still needs source-hash/preimage checks
and compiled validation. Exact framework JAR SHA256 values are in
`logs/20261025-m4-preflight/smali-dispatch-audit.txt`.

## Android 17 runtime and Hybrid VNDK33 SELinux preflight

On the G965F M2 Android16 runtime using the DS-ACK 4.9.337 kernel,
BZI6 `linker64 --list /system/bin/init` exited0 and resolved the A17 init
dependencies. A harmless A17 shell command printed `A17_SH_EXEC_OK` and exited0;
the tests reported no `FATAL: kernel too old` or `SIGILL`. The A17 `secilc`
binary started but was invoked with unsupported `--version`, returning exit1
and exact output `secilc: unrecognized option '--version'`; that is not a valid
policy test. A later valid BZI6 secilc run compiled the adapted split CIL on
the same verified DS-ACK runtime with exit0. It used the fork's `-N` flag, so
neverallow checks were disabled. Host and device outputs are both1,663,784
bytes but have different hashes; semantic equality is unverified. The exact
inputs/results are in `runtime-secilc-compile.txt/json`. The scratch directory
has been removed after preserving the output. These runtime checks do not run PID1, load sepolicy, prove the
native32 layer, or prove an Android17 boot. Raw result/source hashes and cleanup
state are recorded in `logs/20261025-m4-preflight/runtime-linker-device.txt`.

The first complete host-side compile of the staged BZI6 system + Hybrid VNDK33
failed on an undefined compatibility attribute at
`system_ext/etc/selinux/mapping/33.0.cil:359`:
`(expandtypeattribute (attiqi_app_33_0) true)`. The base hybrid
`plat_pub_versioned.cil` lacked 16 expanded aliases required by BZI6's version33
mapping. With `system_ext` staged at the fork verifier's actual path, the next
conflict was the vendor `genfscon` for
`/proc/sys/vm/compaction_proactiveness` (`proc_compaction`) against the A17
system_ext label (`proc_compaction_proactiveness`). The old vendor telephony
allow also named `telephony_prop`, while BZI6 maps
`ro.telephony.sim_slots.count` to `telephony_config_prop`.

Patch0109 adds only the missing versioned typeattribute declarations,
retargets that one vendor_init property rule to the existing
`telephony_config_prop_33_0` alias, and removes the exact stale vendor genfscon
line so Android17's system_ext label remains authoritative. It preserves the
KnoxZT allow because BZI6 `system_ext_sepolicy.cil` defines `knoxzt_service`.
The adapter is scoped to BZI6 SDK37/release17 + target VNDK33 + M4/star2lte,
runs after the existing embedded policy finalizer, and changes only disposable
WORK_DIR files. Its self-test, repeated-apply idempotence, shell/Python syntax,
and the fork's complete staged SELinux compiler all pass. No on-device policy
was loaded; runtime Enforcing acceptance remains pending. Full hashes/results:
`logs/20261025-m4-preflight/selinux-vndk33-adaptation.txt`.

## Historical decision and next gate — before checkpoint0131

At the time of this initial audit, no M4 donor config/profile had been created. The source RIL feature list and
listed donor SoftAP defaults are resolved; direct A17 evidence covers several
optional feature omissions, the ADB adapter passes source-layout preflight, and
BZI6 runtime APEX composition with the selected 32-bit layer packages
successfully. Patch0110 handles the unavailable static SIM count by preserving
the exact reviewed runtime detector; patch0111 handles only the verified
classifier/ARDoodle absence and retains GPPM. Still open are target WLAN
RF/backoff adaptation, runtime acceptance of the native32 layer and eSE behavior.
Target eSE settings stay at the M2 baseline for the initial M4 port, with no
invented COS/vendor metadata. The next work is to
review each M2-only gate against BZI6, add only explicit A17 dispatches
supported by source hashes/preimages, then finish the profile. Checkpoint0131 has since added a scoped M4 profile and
verified the M2→M4→M2 source/config/cache switch. Full module/Knox review and final
work/image gates still precede any build.

## Feature adapters implemented after the audit

Patch0112 corrects the textual SIM detector hash to the fork's actual
`--no-debug-info` mode; it preserves the same BZI6 runtime method. Raw debug
decodes are deliberately rejected. This mismatch was found before M4 build.

Patch0113 adds the scoped BZI6 hardware-mDNIe path. The reviewed S9 sysfs
methods retain CHANGE_DISPLAY_COLOR permission checks. BZI6's removed
DeviceIdleController logging outline is replaced with its append/toString/
Slog.i body; the remaining outline and donor methods are hash checked.
Patch0114 adapts APE/personalization, including Qbox message5 semantics and
Settings availability. BZI6 SUPPORT_APE is ordinal37, not AZH4 ordinal35;
its shell uses v1/v13/v14 instead of p2/v10/v11. Patch0115 adapts the three
SoftAP branches: country fallback only for missing/NA HAL data, disabled
power-save initializer/command and sharing-lite guarded by actual sharing
support and subsequent donor carrier exclusions. Power ordinal29 remains
the same, but its branch label changes :pswitch_c→:pswitch_e.

Actual no-debug AZH4 and BZI6 fixtures pass independently and in module order;
DEX assembly of all affected fixture folders passes with the existing fork
writer's API29/Dex039 settings. The standalone Smali CLI at API37 failed with
`IllegalArgumentException: dexVersion must be within [0, 999]`; inspecting
SmaliBuilder.java confirmed its existing cap to29, and using that exact writer
mode passes. This is a test-harness distinction, not evidence of Android17
runtime failure. No toolchain change was made.

The negative checks reject altered Settings preimages, wrong SDK and
sharing-lite-before-power without partial writes. Command-table comparison
confirms that only APE and power-save entries change. These checks do not
establish full artifact/Dalvik verification, RF correctness, hotspot behavior
or display behavior on a booted M4. Remaining M2-only dispatches, target
RF/backoff and final release gates are still open. Detailed reason/files/
verification/rollback records: `docs/patch-matrix.md`, patches0112–0115.

## Checkpoint0118 — WLAN, signature and lights dispatch

0116 now adapts the source target WLAN policy: booster000, three A37 table
literals removed through the empty guard, and driver-based SoftAP11ax query.
Other coex code stays byte-identical; RF behavior still needs hardware tests.

Original mandatory preflights report `1 out of 7 hunks FAILED` for AZH4
signature and `2 out of 8 hunks FAILED` for AZH4 backlight, preserved in
`core-dispatch-preflight.json`.0117 changes the signature try-range context,
pins all three PM inputs and retains configured-key/android identity guards.
0118 adapts BZI6's LogicalLight, split LightImpl, mute/cache and SehLights
Parcel; four classes and the backlight diff are pinned before decoded writes.

New core fixtures assemble to DEX039 and reject altered scope/preimages.
Logs: `signature-bzi6-test.txt`, `lights-bzi6-test.txt`. All108 patches replay;
92 files match byte for byte. Final framework and M4 runtime remain untested.
See `m4-dispatch-ledger.md` for current gates. Rear fingerprint, OMX/media,
other product features and profile/release wiring remain mandatory work.

## Checkpoint0120 — rear fingerprint and OMX/media

0119 adds an explicit BZI6 rear-fingerprint branch with method pins across
framework/services/SystemUI and the pinned compatibility BiometricSetting
client. Settings is checked after optical→rear SPF substitution. BZI6 LsRune
uses true register v3, unlike AZH4's v16; only the two display/optical stores
are changed, preserving all other uses. Both donor fixtures assemble.

BZI6 vexfwk_service.apk is present (18,601,311 bytes; SHA256
205b751416fb376deef67d574f877ddb7095e9c5b90e90a736b2d5ef5875dd32).
Its encoder default uses v2, unlike MotionPhoto/GPPM/Camera v4; Camera's class
moved to DEX4.0120 therefore requires and patches all four actual encoders,
instead of borrowing AZH4's missing-vex skip. MotionPhoto's old transcoder
method is still absent and is verified with a class hash.

LLVM21 disassembly and relocation identify the HEVC assertion branch at
0xebdbc in BZI6 Stagefright, with HEVC GOT slot0x280878 and taken target0xec69c.
Only that four-byte instruction is NOPed; the full input/output hashes and
PT_LOAD/context checks are pinned. This preserves the scope of the existing
fork media adjustment and does not certify HDR/HEVC support. The new compiled
release gate reads real DEX methods from all four APKs and checks native hash.
Positive fixture and stale-native/donor-vex negatives pass.

Evidence: `fingerprint-bzi6-test.txt`, `media-encoder-methods.txt`,
`media-bzi6-test.txt`, `media-stagefright-bzi6/`, and
`replay-through-0120.txt`.110 patches replay,93 files match. No M4 profile,
full artifact or runtime test yet. Remaining product/display-feature branches
and profile/runtime/release contracts are listed in `m4-dispatch-ledger.md`.
