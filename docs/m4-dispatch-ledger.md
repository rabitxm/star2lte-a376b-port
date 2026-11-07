# M4 BZI6 dispatch ledger — checkpoint0141, 26/10/2026

Attempt03 clean-build evidence: BZI6 NFC and donor-camera dispatch passed; the
build then failed at `customize.sh:294` while replacing missing legacy
`createSpBlob(Ljava/lang/String;[B[BJ)[B`. Patch0134 preserves the donor's
Android17 Keystore-based create/unwrap path for exact M4 G965F SDK37 and keeps
the Android16 crypto adapter for M1/M2. See patch matrix0134 and
`logs/20261026-m4-spblob-0134/`. Keystore hardware security and lockscreen
credential behavior still require runtime verification. Attempt04 then failed
at the P2P adapter because its Android16 DEX signature occurs0 times in BZI6;
patch0135 uses a strict decoded-Smali anchor for M4 while retaining the Seh
callback. Exact M4, wrong-scope, M2 dispatch and mutated-anchor fixtures pass.
See `logs/20261026-m4-p2p-0135/p2p-dispatch-test.txt`. Attempt05 passed that branch, then reached the stagefright camera guard; patch0136 closed that source dispatch. Attempt06 passed P2P and stagefright, then stopped at DVFS: `logs/20261026-m4-camera-0136/m4-build-attempt06.txt:940` reports `Missing BZI6 class descriptor: U1/w.smali`. Patch0137 pins the exact BZI6 SDHMS APK/class, maps M4 to `smali/n5/i0.smali`, and preserves M2's AZH4 path. Attempt07 exposed an unset module-local source path; patch0138 uses the fork cache helper. Attempt08 verified the source APK and SDHMS literal in the real build, then stopped at `m4-build-attempt08.txt:1005` because camera metadata had already changed the libstagefright hash. Patch0139 pins and composes both binary edits; positive/negative fixtures and all129 patch locks pass. Attempt09 is next. See `logs/20261026-m4-media-0139/stagefright-composition-test.txt`.

This ledger separates source adaptation evidence from final image and runtime
acceptance. It supplements `m4-donor-dispatch-audit.md`; the old Android16-only
gate list is an inventory, not permission to rename every guard. A separate M4 donor
config and scoped G965F/no-KSU profile exist. Clean build attempt01 stopped at an
Android16-only NFC path guard before images; patch0132 adapts that path. M2
attempt56 stays the control. Checkpoints0130–0131 add the donor-native
runtime/image closure, BZI6 ADB adapter and profile/cache switch.

| Area | Reviewed implementation/evidence | Remaining gate before M4 acceptance |
| --- | --- | --- |
| Input/cache |0128 adds the BZI6 donor registry;0131 adds the scoped M4 profile and real gen_config M2→M4→M2/cache-marker test; each M4 selection enters donor hash | Full work-tree/compiled-cache switch test remains before build |
| VNDK33 policy |0109, host and actual BZI6 secilc compilation on DS-ACK M2 | Compiler used-N; neverallow/live Enforcing load and final image checks remain |
| SIM |0110+0112 preserve hash-checked runtime detector in no-debug decode | Target SS/DS radio logs; no invented static donor count |
| Optional camera claims |0111 removes only absent classifier/ARDoodle claims, retains GPPM | Final work/image gate and camera runtime |
| Hardware mDNIe |0113 adapts BZI6 methods and removed logging outline; both donors assemble | Final compiled services/framework and real display/accessibility tests |
| APE/SoftAP |0114–0115 cover BZI6 APE, country, power, sharing-lite; composed fixtures assemble | Final JAR/Settings and Wi-Fi/hotspot runtime |
| WLAN RF/11ax query |0116 empties only three donor RF-table literals, preserves HAL/coexistence code, verifies booster000 and adapts chip query | This closes the source dispatch gap; it does not validate RF behavior, antenna count, channels or regulatory compliance on hardware |
| Signature integration |0117 adds BZI6 try-range context; pinned three classes, fuzz0 application, existing platform identity guard and DEX assembly pass | Full package install/boot behavior with the selected certificate remains untested |
| Legacy SehLights |0118 preserves BZI6 LogicalLight, split locked/unchecked API, mute state and standard ILights; forwards the extended brightness Parcel argument | Full framework/HAL integration, brightness, auto-brightness, AOD, mute/unmute and LED tests remain |
| Rear fingerprint |0119 pins BZI6 framework/service/SystemUI methods, checks the pinned BiometricSetting methods and Settings substitution; source/DEX fixtures pass for both donors | Final built framework/UI and actual enrollment/unlock still need M4 tests |
| OMX/media |0120 pins BZI6 ELF and four encoder methods, including present vexfwk; one HEVC branch and four color-format defaults adapted; compiled gate tested | Full artifact gate and actual video/photo/MotionPhoto/HEVC behavior remain untested |
| Product/display features |0121–0124 pin BZI6 shipping/HDR/HFR/brightness, preserve runtime sensor detection and select native fixed resolution with a separate M4 flag; composed DEX fixtures and compiled DEX/XML gate pass | Full artifact and real HDR/brightness/60Hz/resolution tests; new profile must select the reviewed M4 flag |
| DEX class relocation |0123 adds exact-descriptor BZI6/F/M4 scope, rejects missing/duplicate definitions; moved actual Settings class fixture passes | Other generic SPF/mod branches still require full module preflight; unchanged-value or missing-method failures remain blocking |
| Native ABI/runtime |0129–0130 add separate BZI6 native32/runtime locks, donor camera/audio/graphics V7/Tethering set, vendor_kg and aconfig-v2 staging; ext4 SYSTEM/VENDOR fixtures and M2 byte regression pass | Ready M4 profile and full runtime/vendor/image gates remain; Android17 hardware behavior needs device logs |
| Camera app dispatch |0133 skips legacy One UI8 `PackageUtil` Smali hooks only for the exact M4/G965F/SDK37 profile; BZI6 `SamsungCamera.apk` matches existing runtime lock SHA and finalization retains its donor app/server pair | Runtime camera launch, preview, photo/video, lens/scene features and final camera patch compatibility remain unverified |
| ADB/Auto Blocker | BZI6 APK/JAR decoded, patched and rebuilt with fork Apktool; compiled defaults/no-RSA/stored-choice gate passes; M2 output byte-identical; donor controls/observer retained | Fresh-data defaults and live ADB/Auto Blocker toggle tests remain mandatory |
| NFC |0132 keeps exact BZI6 SDK37 `com.android.nfcservices.capex` and `SecNfc.apk`, locks both through SYSTEM ext4, and rejects mutated fixture; all122 patches replay on pinned HEAD | `SecNfc.apk` references Samsung HIDL2.0 and AIDL while target advertises HIDL2.0 only; runtime service startup, tag read/write, eSE/card behavior and Samsung UI remain unverified |
| eSE baseline |0125 pins four BZI6 classes and preserves target-none substitutions; source/DEX gate passes, wrong COS rejects | Actual eSE/NFC behavior on M4 remains untested; no new COS inferred |
| Retained Wi-Fi UI |0126 pins13 classes and checks compiled canonical bodies with a separate M4 flag; old S22 UI patches omitted after input validation | Actual M4 scan/icon behavior; target capability stays Wi-Fi5 |
| Wi-Fi Direct callback |0135 selects the reviewed BZI6 `setupIface` Smali adapter for exact M4 G965F/SDK37; replaces only standard callback registration and retains separate Seh registration | Verify built JAR, Smart View/Wireless DeX discovery and P2P group behavior on M4 runtime |
| Stagefright ABI |0136 permits only absent 32-bit stagefright for exact arm64-only M4 when lib64 exists; patches donor lib64 model property and uses fork ABORT helper | Verify packed image contains patched 64-bit donor library and test video metadata/runtime; any missing 64-bit file fails build |
| Stagefright camera/HEVC composition |0139 verifies the camera-patched intermediate hash, proves reversing one model key recovers the donor, then checks the combined HEVC output hash | Verify final packed libstagefright hash and OMX behavior in attempt09/device runtime |
| SDHMS DVFS class |0137 pins the BZI6 SDHMS APK and decoded `n5/i0` class; exact M4 replaces its forced A37 SSRM policy literal while AZH4 M2 continues through `o5/w` | Verify actual rebuilt APK/JAR and final target policy on M4; source fixture alone does not establish runtime DVFS behavior |
| Face/optional Settings/Bixby |0127 wires selected M4 omission/control hiding, WINK,211+4 search providers and36+1 top-level keys; source/DEX/selection fixtures pass | Full compiled XML/work/boot contracts and remaining optional modules/Knox paths require review |
| Final vendor/boot identity | VNDK33, F blobs and locked radio bridge remain the selected hardware baseline | Add explicit M4 guards in finalization/boot/release checks; donor RILD APEX must not enter final vendor |

M1/M2 profiles and their diagnostic/bootstrap guards remain deliberately
restricted. A new M4 source/config must not borrow `STAR2LTE_M2_*` or
`EXYNOS9810_M2_*` as a shortcut through those guards. Target eSE remains at the
M2 baseline; no new COS or target vendor metadata is inferred from donor text.

The native32 and donor-native runtime/image slices plus the scoped M4 profile are
implemented through0133. Attempt01 exposed a stale Android16 NFC path, which
attempt02 passed after0132. Attempt02 then exposed a missing One UI8 camera class;
0133 keeps the locked BZI6 Android17 app on M4. The next clean build must
validate target HAL composition, later camera dispatch and complete image gates.
After that, remaining module/Knox dispatches, work/compiled-cache switching and
final boot/vendor/image contracts stay open.

Evidence under `logs/20261025-m4-preflight/`:

- `wlan-rf-method-audit.txt`, `wlan-rf-a17-test.txt` — actual no-debug methods,
  wrong-feature/preimage rejection and composition with0114–0115.
- `core-dispatch-preflight.json` — original dry-run failures: signature M2
  `1 out of 7 hunks FAILED`; M2 backlight `2 out of 8 hunks FAILED`.
- `signature-bzi6-test.txt` — new patch fuzz0, configured-cert equality and
  android-package guard checks, affected package-manager DEX assembly.
- `lights-bzi6-method-diff.txt`, `lights-bzi6-test.txt` — split API/mute-state
  review, self-member resolution, Parcel argument order, DEX assembly.
- `replay-through-0118.txt` — all108 textual source patches replay from the
  pinned commit;92 resulting files match the active fork byte for byte.

No kernel build/change, partition write, live setting write, firmware export or
device operation occurred during patches0116–0118. Patch reasons, affected
files, verification and rollback are recorded in `patch-matrix.md`.

Checkpoint0120 evidence in the same preflight folder:

- `fingerprint-method-audit.txt`, `fingerprint-ui-audit.txt`,
  `fingerprint-bzi6-test.txt`: BZI6's SystemUI true register is v3 rather than
  AZH4's v16. It is preserved for unrelated flags. Rear/capacitance enums and
  source/Settings consistency are checked; both donors' fixtures assemble.
- `media-encoder-methods.txt`: BZI6 vexfwk_service exists and uses default
  register v2; MotionPhoto/GPPM/Camera use v4. Camera's class moved to DEX4.
  MotionPhoto still has no legacy configVideoEncoderParameters method.
- `media-stagefright-bzi6/`: actual LLVM21 symbols/relocations/disassembly.
  HEVC GOT slot0x280878 is used before CBZ at0xebdbc; its taken target0xec69c
  reaches the HDR10Plus log/assert path. The four-byte NOP is the only native
  change; expected full output SHA is pinned. Other checks remain.
- `media-bzi6-test.txt`: all four actual encoder classes assemble; the compiled
  gate reads the resulting DEX and native hash, and rejects an unpatched native
  or a real donor vex APK with its stale default. These are fixture APKs,
  not complete signed/rebuilt M4 packages or runtime codec acceptance.
- `replay-through-0120.txt`:110 source patches replay;93 files are byte-identical
  to the active fork. The previous108 locked hashes are unchanged.

At checkpoint0120 the fingerprint/OMX source fixtures were complete; the
product/display gap listed then is addressed by0121–0124 below. Native32/
runtime composition, optional UI/eSE choices, final contracts and source-profile
switching remain open before a complete M4 build.


Checkpoint0124 evidence is under `logs/20261026-m4-product-display/`:
`shipping-hdr-test.txt`, `display-fixed-test.txt`, `smali-dispatch-test.txt`,
`fixed-config-test.txt` and `replay-through-0124.txt`. All114 patches replay;
95 text files match and the prior110 hashes are unchanged. See
[m4-product-display.md](m4-product-display.md) for input provenance, helper
composition, original-rule error lines, compiled-gate negatives and limits.
No device operation or full M4 build occurred. The product gate is wired into
the existing release-default verifier; it has passed fixture artifacts only.


Checkpoint0128 evidence: `logs/20261026-m4-profile-preflight/`, especially
`ese-bzi6-test.txt`, `wifi-ui-test.txt`, `settings-limits-test.txt`,
`search-m2-regression.txt`, `boot-selection-test.txt`, `source-selection-test.txt`
and `replay-through-0128.txt`.118 patches /101 matching text files;114 previous
hashes unchanged. Full report: [m4-source-selection.md](m4-source-selection.md).
The isolated source config fixture uses explicit environment selections; it
must not be described as a ready M4 kernel/runtime profile or a full ROM build.


Checkpoint0129: see [m4-native32-runtime.md](m4-native32-runtime.md) and local
logs/20261026-m4-runtime.119 patches replay to102 matching files; all118 previous
hashes unchanged. Actual restore dispatch and image entrypoint are tested with
private fixture trees/images. This is not a full M4 build or runtime result.


## Checkpoint0130 — BZI6 runtime provider closure

BZI6 cameraserver imports graphics common V7 and the AudioSystem symbols
provided by BZI6 `libaudiobase.so`; these files and the A37 SamsungCamera app
are hash-locked and retained as one donor-native set. This avoids the already
identified S22 camera server imports that BZI6 `libaudioclient.so` cannot
provide. M4 retains BZI6 graphics V7 and Tethering CAPEX; S22 graphics V6 and
Android16 Tethering remain M2-only choices. BZI6 vendor_kg UID/GID5959 and the
four system aconfig-v2 tables are verified; missing vendor tables are staged as
empty v2 output because the donor has no vendor aconfig source.

The existing restore/finalization/release/image paths now select these changes
only under strict M4 BZI6 scope. Positive and negative runtime-helper fixtures,
128MiB ext4 image checks, and byte-identical M2 metadata regression pass.
Patch0130 also adapts the existing project ADB helper to BZI6's actual
`.locals 6` AdbService lifecycle while preserving the existing `adb_enabled`
observer callback and donor Rampart/SecSettings/SUW controls. `services.jar` and
SettingsProvider source hashes are pinned separately. A DEX039 fixture passes
compiled no-RSA properties, one-time default seeds and stored-setting read; M2
output remains byte-identical. This does not establish fresh boot defaults or
live toggles, or that A17 init/aconfigd, Tethering/BPF, camera HAL or AudioSystem
reaches runtime success. No full M4 build or device boot exists. See
`docs/m4-native32-runtime.md` for file list and evidence.



## Checkpoint0140 — BZI6 USB Host Restrictor DEX relocation

Attempt09 passed the composed Stagefright camera/HEVC adapter and SDHMS path,
then failed at `m4-build-attempt09.txt:1002` because fork code requested the
absent `UsbHostRestrictor$8.smali`. The actual BZI6 class is `$11`: it is the
unique decoded handler containing the expected method and both mode strings.
Patch0140 keeps the prior `$8` donor route and selects `$11` only for the exact
M4 G965F SDK37 BZI6 profile. Class hash, uniqueness and contents are recorded
in `logs/20261026-m4-usb-host-0140/usb-host-descriptor-review.txt`; shell
syntax and patch replay checks pass. Attempt10 confirmed the change in the complete fork run: the log resolved `$11`
and replaced both mode constants. It then stopped at line1027 of the same log on
the absent AOSP RescueParty class; Patch0141 preserves only the exact BZI6 donor
SecRescueParty implementation, which is hash-pinned and registered by
SystemServer. Its recovery/data-erasure behavior still requires runtime review.
Attempt11 is next.


## Checkpoint0141 — BZI6 Samsung RescueParty dispatch

Attempt10's next failure was `Missing BZI6 class descriptor:
com/android/server/RescueParty.smali` at log line1027. BZI6 replaces this with a
Samsung `SecRescueParty` class; `SystemServer` registers its observer. The
reviewed class includes donor-specific app-data erase and recovery actions, so
Patch0141 retains the donor implementation for exact M4 scope rather than
applying the AOSP `isDisabled()` patch to an unrelated class. Its pinned class
hash and call-site fixture pass. This defers runtime rescue-policy acceptance;
it does not disable or validate those actions. Attempt11 passed this dispatcher and then failed at vendor mismatch warning; Patch0142 is now in place, and attempt12 must prove the full path.


## Checkpoint0142 — BZI6 vendor mismatch warning

Attempt11 stopped at the fork warning patch: expected lambda6 did not match
BZI6 lambda7 (m4-build-attempt11.txt:1060), and the old handler hunk did not
match BZI6 lambda4 (line1138). Patch0142 pins both exact BZI6 Smali hashes and
adapts only warning class ID3. It leaves the diagnostic log, UID alert class
ID2, and handler cases0–2 intact. M4 dispatch remains strict to G965F/BZI6/SDK37.

Files: rom/scripts/internal/apply_modules.sh and the new
rom/unica/patches/miscs/a376b_android17/0001-disable-bzi6-vendor-mismatch-warning.patch.
Validation: positive/negative content checks against attempt11 decoded classes,
patch apply, Apktool services.jar assembly, bash -n, and 132-patch source-locked
replay pass. Fixture and assembly logs are in
logs/20261026-m4-vendor-warning-0142/. Attempt12 confirmed the dispatch and reached NFC; attempt15 later rechecked it and found the follow-up guard used the wrong target SDK, corrected by patch0144 below.


## Checkpoint0143 — G965F NFC sidecars

Attempt12 reached the fork NFC customizer after passing patch0142, then failed
at line1039 on absent G965F PHN libnfc_prop_extn.so; libnfc_vendor_extn.so is
also absent. Target PHN core JNI/NCI and Samsung HIDL HAL are present, and their
ELF NEEDED lists do not reference these sidecars.

Patch0143 changes only rom/unica/patches/nfc/customize.sh. Its M4 dispatch is
scoped to G965F/PHN FVG4 + BZI6, checks target JNI and BZI6 sidecar hashes, and
keeps the already-staged donor sidecars only when both target files are absent.
Wrong model, changed core JNI and partial-sidecar cases fail closed. All other
profiles keep the old copy behavior. Positive/negative fixture, ELF audit,
bash -n and 133-patch replay passed, but that fixture used stale target SDK29.
Attempt15 config uses SDK31 and failed at log line1038 because the dispatcher
then fell through. Patch0144 corrects the guard; details and four dispatcher
fixtures are recorded in the next checkpoint.


## Checkpoint0144 — generated target-SDK correction

Attempt15's generated config says `TARGET_PLATFORM_SDK_VERSION="31"` at line22,
but Patch0143's guard required29. The exact-profile branch was skipped and the
existing NFC customizer attempted the absent G965F sidecar at
`m4-build-attempt15.txt:1038`.

Patch0144 changes only `rom/unica/patches/nfc/customize.sh` from target SDK29 to
31 for this exact G965F PHN/BZI6 route. It keeps the hash-pinned JNI and donor
sidecar checks. SDK31 positive, wrong-model, wrong-SDK and mutated-JNI fixtures
pass. Shell syntax and all134 ordered patches replay from the pinned base; all111
replayed files match and prior133 locks are unchanged. Patch SHA256:
`092ce3155bede5598cdbc0d2f8324925350864efd4d4dfa29ee0bda13ac11f47`. Full evidence: `logs/20261026-m4-vendor-warning-0142/`.
Attempt16 confirmed this branch at log line1038, then stopped later in display. NFC runtime is not proven.


## Checkpoint0145 — CoreRune intermediate hash after identity adapter

Attempt16 advanced past NFC; line1038 confirms the corrected sidecar branch. It
then failed at line1080 in the display adapter because `CoreRune.<clinit>` had
been changed earlier by the legacy identity patch. Lines1024–1027 identify the
two exact string replacements. Reversing only those gives donor method hash
`56d6e8ff4f638415b65c1b6afedd07bf5df1f163d98997d3d1dc889338ca6931`; the actual pre-display intermediate hash is `f383d6643c1d420005df733e8b418cb075077bebfd020c73f14f093249ab22f7`.

Patch0145 changes only
`rom/unica/patches/product_feature/a376b_android17_display.json` to pin the
reviewed intermediate method before the fixed-resolution edit. The actual
attempt16 class was composed with donor fixture classes; display application,
15 strict-preimage negatives, ten DEX039 assembly fixtures, compiled product
verification and stale-feature negatives pass. M2 HFR/brightness dispatch is
unchanged. All135 ordered patches replay and 111 files match; previous134 locks
remain. Patch SHA256: `ea73978222dc24df8027cbe8cfc58d701a9641dda472bf661824478f09b24879`. Evidence:
`logs/20261026-m4-vendor-warning-0142/product-display-0145/` and
`series-replay-0145.txt`. Attempt17 full build pending.
