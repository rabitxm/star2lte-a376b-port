# M4 post-boot runtime repair — 28 October 2026

## State

Patch0199 integrates the measured network/meminfo composition into the explicit M4 G965F diagnostic profile after vendor restoration. Patch0200 applies the user-approved omission of only the BZI6-unsupported DEX thermal special node. Patch0201 routes the failing FaceWidget callback initialization to the existing main-looper handler with generation and plugin-identity guards. All three are source-level and fixture-verified; 0201 SystemUI APK fixture also passes Apktool build/decode roundtrip. No ROM was rebuilt or flashed in this execution segment. The retained attempt37/source0188 package remains untouched.

The live G965F `M2_G965F_SERIAL` was seen in TWRP with bootloader G965FXXUHFVG6. BOOT readback SHA256 was `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`, matching the documented baseline. A batched full-partition hash command timed out after returning BOOT, so other partitions were not treated as reverified; no write occurred.

## Patch0199

Reason: measured fixes 0195–0198 had no final integration point in the M4 diagnostic profile, and end-of-build vendor restores could remove staged runtime outputs. Files: explicit M4 diagnostic profile, runtime compatibility verifier, postboot composition helper, finalization script, and image verifier. The helper accepts only BZI6/M4/G965F/SDK37 with the explicit diagnostic flags; it validates donor marker and input hashes, runs the existing APEX and meminfo builders, and stages only the two reviewed outputs after restores. Normal M4 profile is unchanged.

Verification: fixture built from the actual extracted BZI6 donor produced Tethering APEX SHA256 `f17fa222e1f127d349e61b592c705c127f682ad4d22dda164c03397857862cc6` and `libmeminfo.so` SHA256 `b19a1c37a8ebed72676739a12ab46be31dd3c74d4ba1c605f655a62661d70fe9`. `py_compile`, `bash -n`, `git diff --check`, and patch replay against saved preimage files passed. The compiled APK and actual ext4 image gates are implemented but not run because no new ROM image was built. Rollback is removal of the explicit profile flag and reversal of0199; normal M4 donor assets remain.

## Patch0200: SDHMS DeX special branch

The exact runtime failure is `logs/20261028-m4-meminfo-test/sdhms-first-crash.txt:2`: `No enum constant com.sec.android.sdhms.thermal.overheatcontrol.overheatcomplex.OverheatComplexType.DEX`. The user authorized omitting the DEX-specific branch. The BZI6 parser enum has no DEX member. The transformation removes exactly `<DEX formula="SKIN" temp="470" />` from the S9 policy input, requiring one exact line and pinned source/output hashes. It preserves every other byte in the SIOP file and copies the distinct locked `ssrm_default.xml`; it does not copy the SIOP file into both asset names.

Verification produced SIOP SHA256 `94a74d1c278207fbd59eb595db12ad337ccf1a8b8a39797451745e16923347be` (10181 bytes) from source `8e30ff47bec85ea6526b839df5057ee3ea0769a017ef9a9e3573dea16bf23503` (10227 bytes). XML parser and no-DEX checks passed; SSRM is unchanged at `6784ecc18f0326c0599b9278fbd2865c56e3c4d58f2e6ca1f5250280ef78cb96`. The APK source pin is `53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`. Shell/Python syntax and patch replay fixture passed.

This does not establish thermal runtime safety. Compiled APK/ext4 image checks, sensor freshness, policy selection, limiter registration, actuator readback, light-load and cooldown runtime checks are pending. DeX-specific overheat behavior is explicitly omitted and must not be called accepted. Ordinary AP/GPU/skin/charging/camera policy remains.

## Patch0201: FaceWidget lifecycle threading

The fresh crash at `logs/20261028-m4-meminfo-test/systemui-first-crash.txt:3–20` records `$Proxy9.setPluginFaceWidgetCallback` invoked from `PluginFaceWidgetManager$6.handleMessage` on `HandlerThread.run`; the nested cause is `IllegalStateException: Method setCurrentState must be called on the main thread`. The AOD setter's Dagger construction creates `FaceWidgetPluginLifeCycle`, whose lifecycle registry enforces main-thread use. The existing `$5` UI handler is constructed with `Looper.getMainLooper()`.

Patch0201 increments a connection generation on connect/disconnect. The BG handler routes initialization to UI; UI checks generation and raw plugin identity before directly invoking the existing initialization handler on main. Stale queued callbacks are dropped. It does not mute the lifecycle assertion. Three BZI6 classes are SHA-pinned; positive work fixture, changed-preimage guards, Apktool build and decode roundtrip all pass. Files/reason/rollback are recorded in patch and lock.

The entire one-time plugin-wrapper initialization/getter phase now executes synchronously on main to preserve callback-before-getter ordering. This can affect startup latency and still needs runtime measurement. No device boot, 15-minute stability, lock/unlock, AOD, Infinity or reconnect acceptance has run.

## Remaining plan

SystemUI error: `logs/20261028-m4-meminfo-test/systemui-first-crash.txt:15` — `IllegalStateException: Method setCurrentState must be called on the main thread`. Patch0201 source/fixture repair is implemented; runtime remains pending.

## Patch0202: NFC APEX JNI pairing

The fresh loader error was `logs/20261028-m4-postboot-repair/fast-018-crash.txt:5`: `dlopen failed: library "libnfc-nci.so" not found: needed by /system/lib64/libnfc_nci_jni.so in namespace clns-shared-9`. The first static candidate was to stage PHN's matching provider. Before doing that, the actual BZI6 `NfcNciApex.apk` and G965F PHN `NfcNci.apk` were decoded and their `NativeNfcManager` declarations compared. BZI6 has54 declared native methods; PHN has72. Concrete signature differences include BZI6 `doEnableDiscovery(IZZZ[B[BZ)V` versus PHN `doEnableDiscovery(IZZZZZ)V`, and BZI6 `doAbort(Ljava/lang/String;)V` versus PHN `doAbort()V`. Therefore adding the PHN provider beside the PHN JNI would only hide the current load error while leaving a mismatched Java/JNI contract.

The chosen source repair uses the JNI already embedded in the hash-pinned BZI6 NFC APEX, which is packaged with the BZI6 Java classes, and removes the PHN global `system/lib64/libnfc_nci_jni.so` shadow only in the explicit M4/G965F diagnostic profile. The S9 SLSI vendor service, firmware/RF data, Samsung `libnfc_sec_jni.so`, and target hardware stack remain selected. This avoids copying old PHN native dependencies or replacing the target HAL.

Verification: the source helper extracted the nested APEX payload and confirmed embedded JNI SHA256 `e312f4fd01044e2bb3ee26900df73090a4a657ed6292d4e0e056fa73b23e5d81`; PHN JNI SHA is pinned at `ed74c2fc3c76a835d0fe88043366b3e8c86a920c503cf247355cc84f94efa042`. The final ext4 gate now rejects a system-level JNI shadow and validates the embedded APEX JNI hash. Source/profile syntax checks pass. Device classloader namespace, JNI registration, service registration, RF/tag, HCE/eSE/UICC tests have not run; no NFC runtime PASS is claimed. Rollback: reverse0202 and rebuild original M4 work tree; no firmware cache or device partition is modified.

The statements above describe the source-repair segment before packaging. The subsequent compiled run and device result are recorded below.

## Attempt0207 package and fresh device test

The complete M4 diagnostic package was built in isolated run `star2lte-1793162155-h9hgDY` on the secondary NVMe. Package and target-files SHA256, all eight ZIP32 stages, signatures and CRCs passed; the run's `artifact-review.json` records the exact hashes and build profile. The retained M4 attempt37 package was not replaced.

G965F serial `M2_G965F_SERIAL` was confirmed in TWRP as `Galaxy S9+`, bootloader `G965FXXUHFVG6`. All eight installer stages completed in order; stage08 ended with `script succeeded: result was [1.000000]`. The installed partition layout matches the audited target sizes: BOOT 57,671,680; SYSTEM 9,961,472,000; VENDOR 1,363,148,800; ODM 734,003,200; CACHE 629,145,600 bytes. USERDATA is 114,905,559,040 bytes after fresh formatting. EFS, CPEFS, RADIO, RECOVERY, BOTA0, BOTA1 and BOTA2 hashes match the preflash values exactly.

The stage08 hash is `249313d61dfe3d08e3a7e57eeaeaa93c581b3f4feed2c7cab040489d2e4492e9`, matching the legacy manifest, local package and device copy. BOOT readback after install is retained at `logs/20261028-m4-repair-build/boot-attempt0207-postflash.img` (SHA256 `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`).

The TWRP installer also emitted the exact warning `/tmp/exynos9810/vendor_bootfix.sh[255]: can't create temporary file : No such file or directory`; it then logged `E9810 vendorfix: done` and the full stage08 installer succeeded. The observed excerpt is preserved in `logs/20261028-m4-repair-build/vendorbootfix-warning-snippet.txt`; the complete recovery log still needs to be copied from the device and the warning diagnosed.

After `twrp reboot system`, ADB left recovery at 10 seconds and stayed absent for the full 300-second observation. The host still sees Samsung USB `04e8:6863` RNDIS only and receives `192.168.19.193/24` with the phone gateway at `192.168.19.172`; `adb connect 192.168.19.172:5555` returns `Connection refused`. This suggests USB tethering mode is enumerated, but does not prove Android reached boot-complete. `logs/20261028-m4-repair-build/boot-attempt0207.txt` ends with `boot_completed_not_observed_within=300s`; Android boot is not confirmed. Once the device is returned to TWRP, collect CACHE/pstore, recovery log and any staged init/runtime diagnostics before changing or rolling back partitions. No protected partition was written. No M4 acceptance is claimed; attempt37 remains the retained completed M4 release.

## 28/10 verifier integration repair (patch0204)

The first source build passed `a376b_android17_postboot_diagnostic.py verify` for
both measured outputs, then the generic compiled release gate failed with the
exact line `ValueError: Final M4 runtime input mismatch:
system/system/apex/com.google.android.tethering_compressed.apex`. The donor-native
runtime manifest correctly pins the original BZI6 APEX, while this explicitly
selected diagnostic profile intentionally composes the separately hash-locked
Tethering output. The verifier had not connected those two checks.

Patch0204 changes only `rom/scripts/internal/verify_exynos9810_release_defaults.py`.
For the ordinary M4 profile it retains exact donor verification. For the
postboot exception it first requires the reviewed star2lte/M4/SM-G965F/runtime
profile tuple, lets the generic verifier skip only the Tethering APEX hash, then
requires `a376b_android17_postboot_diagnostic.py verify` to validate both the
exact APEX and `libmeminfo.so` output hashes. Other runtime inputs remain pinned.
The compiled release-default gate passed on the current work tree after this
change. The final build/image gate is still running; device runtime and fresh
flash remain pending. Rollback: reverse patch0204; the diagnostic profile then
fails closed at the generic donor hash check.

## 28/10 ext4 SystemUI image-path repair (patch0205)

Target-files reached actual ext4 image verification after the build tree passed.
The exact failure was `ValueError: M4 compiled SystemUI or bundled Apktool is
missing`. The SystemUI APK is present in the work tree at
`system/system/system_ext/priv-app/SystemUI/SystemUI.apk`; this profile folds
system_ext into system, but the image verifier assumed a separate partition
root. Patch0205 makes the expected path follow
`TARGET_OS_BUILD_SYSTEM_EXT_PARTITION` and maps the folded image path to the
existing SystemUI metadata contract. Both separate and folded layouts remain
supported. Python/shell syntax and patch/lock integrity checks pass; rebuilding
and running real image/package gates remain pending.

## 28/10 BZI6 camera gate scope repair (patch0206)

The real package preflight failed with `star2lte: final Samsung Photo/QR repair is not applied` after actual system, vendor and ODM ext4 gates passed. The M4 finalizer intentionally retains the BZI6 Android17 SamsungCamera/cameraserver pair, while the old final matrix check unconditionally required the One UI8 Photo/QR Smali transform. Patch0206 scopes that check: under the exact M4/star2lte/SM-G965F/BZI6/runtime-diagnostic tuple it runs the existing compiled BZI6 camera/OMX verifier; all other configurations keep the legacy Photo/QR check. It also saves the original build codename before iterating starlte/star2lte/crownlte, so the strict scope check is not affected by the loop variable.

Affected files: `rom/scripts/internal/verify_exynos9810_camera_matrix.sh`, `patches/fork/0206-verify-bzi6-native-camera-for-m4.patch`, `patches/fork/series`, `sources.lock.json`, `docs/patch-matrix.md`. Verification: Bash syntax; M4-scoped camera matrix PASS including `PASS compiled BZI6 OMX: native output hash and four actual DEX encoder defaults`; patch reverse/apply and lock/hash replay; then full target-files and flashable package gates. No camera payload is changed by this gate repair. Rollback removes0206 from series/source lock and reverses its verifier change. Repackaging and device test remain pending.

## Attempt0207 diagnosis and patch0207

The complete CACHE logger proves this was a repeatable `netd` linker failure, not an early init death: `init_debug_post_fs.txt` says `post-fs reached`, and `init_debug_post_fs_data.txt` says `post-fs-data reached`. The final captured cycle is `logs/20261028-m4-repair-build/recovery-after-attempt0207/cache-unica/logcat_live.txt:377916`: `CANNOT LINK EXECUTABLE "/system/bin/netd": library "libnetd_resolv.so" not found: needed by main executable`; line377920 records init's `reboot_on_failure` shutdown. The same failure repeats at lines356966 and367439. The installed `/system/bin/netd` DT_NEEDED list includes `libnetd_resolv.so`.

Inspection of the installed Tethering APEX, verified against the installed file hash, found no `lib64/libnetd_resolv.so`. The S22 base APEX also lacks it. The pinned BZI6 Tethering APEX contains the DSO (SHA256 `ccfa0d2f6c1d296daf7f5ce3d9087d70243d795c8dce508eafac1cf46fcfb849`) and its `apex_manifest.pb` field7 (`provideNativeLibs`) exports it. This explains why `netd` could not resolve the library from the composed S22-based APEX. AOSP's APEX manifest schema documents field7 as `provideNativeLibs`, and linkerconfig uses that manifest declaration to expose APEX-provided public native libraries ([AOSP apex_manifest.proto](https://android.googlesource.com/platform/system/apex/+/refs/heads/master/proto/apex_manifest.proto), [Android 16 linkerconfig](https://android.1.googlesource.com/platform/system/linkerconfig/+/refs/tags/android-16.0.0_r4)).

Patch0207 adds only that hash-pinned BZI6 DSO and the corresponding APEX manifest export to the diagnostic composition. The builder verifies the expected five changed payload files (the existing native DSO, two reviewed Java jars, the new resolver DSO, and manifest), 76 UID/GID/mode/SELinux metadata entries, ext4 roundtrip, AVB, container v3 signature, and 4KiB payload alignment. Two independent runs produced the exact locked APEX SHA256 `03a0fbcdd952df34795a6b3648583ae7feb6b49ab785a3cd074db85f7fb2a2ba` (67,911,680 bytes). Normal M4 profile, Hybrid Vendor, target radio and kernel selection are unchanged. This establishes packaging correctness only; the next isolated M4 build and device boot must verify linkerconfig exposes the provider and `netd` stays running. Rollback is reverse0207 and rebuild the previous diagnostic artifact.

## Attempt0208 build integration failure and patch0208

The clean attempt0208 build completed work_dir patch/module application through the postboot composition helper. The network APEX builder and image integrity checks ran, but `a376b_android17_postboot_diagnostic.py:149` then failed with `KeyError: PosixPath('system/system/apex/com.google.android.tethering_compressed.apex')`. `verify()` intentionally returns string-keyed JSON paths, while `apply()` still indexed with `Path` keys. Patch0208 changes those two report lookups to `str(APEX_REL)` and `str(MEMINFO_REL)`; it does not change staged bytes. The prepared work_dir contains the source/module stage and can be validated/resumed after the helper fix without reimporting donor firmware. Build log: `logs/20261028-m4-repair-build/attempt0208-build.txt`. No device writes occurred in this failed run.

## Attempt0208 fresh-flash result and patch0209 diagnosis

Attempt0208 package `star2lte-1793169123-BgSkRT` completed all eight signed ZIP32 stages and was fresh-flashed to the verified G965F `M2_G965F_SERIAL`. All eight stages returned RC=0 after retrying stage08 with the read-only inspection mounts removed. The first two stage08 retries failed only because Codex's own `/tmp/m4-vendor-ro` and `/tmp/m4-system-ro` mounts were still present; the successful retry completed `vendorfix` and the package installer. Fresh USERDATA UUID changed. EFS/CPEFS/RADIO/RECOVERY/BOTA0/1/2 hashes matched the pre-write baseline exactly; BOOT readback matched the package's diagnostic DS-ACK image. The full hashes and per-stage logs are retained under `logs/20261028-m4-repair-build/attempt0208/`.

After `twrp reboot system`, the handset returned to TWRP in a repeatable software reboot loop. CACHE evidence is under `logs/20261028-m4-repair-build/attempt0208/postboot-recovery/`. Init reached both `post-fs` and `post-fs-data`; `init_debug_post_fs.txt` and `init_debug_post_fs_data.txt` confirm these milestones. The decisive error is `logcat_live.txt:6203-6205`:

```text
apexd: Failed to verify /system/apex/com.google.android.tethering_compressed.apex: Manifest inside filesystem does not match manifest outside it
apexd: Failed to activate packages: Failed to verify ...
```

Because apexd rejected the APEX, the APEX-provided `netbpfload` service was not installed. The fallback declared by `netbpfload.rc` is `/system/bin/false`; consequently init's `bpfloader` service exited with status1 in about9ms and line9030 recorded `reboot_on_failure`. This is not yet evidence that the kernel lacks BPF support. `logcat_live.txt:9030` shows the resulting reboot; repeated later cycles show the same APEX verification failure and bpfloader fallback.

Patch0207 changed the `apex_manifest.pb` inside the APEX payload to export BZI6 `libnetd_resolv.so`, but preserved the duplicate outer `apex_manifest.pb` from the S22 container. Patch0209 updates both copies and adds gates requiring byte identity and the provider declaration. A clean helper run passed the 76-path UID/GID/mode/SELinux check, ext4 roundtrip, AVB verification, container v3 signature, and 8192-byte payload alignment. The signed output is SHA256 `0698d84c1d8306a0dbc30871fccd37505d43ab5a5de397d65319a117065f6853` (67,911,680 bytes); its outer and payload manifests are identical and provide `libnetd_resolv.so`. A second independent build produced the same output. The updated APEX is not yet installed on the phone; next build attempt0209 as a unique release and test apexd activation/netd before treating BPF/networking as resolved. No kernel rebuild, RIL/modem change, or protected-partition write was made.

## Attempt0209 fresh test: APEX activation fixed, resolver TLS closure missing

The isolated release is `star2lte-1793172794-68tWKA`: full signed ZIP SHA256 `da45a70ae9232c82402e93ea61080cc052413c51e5add09af95e2a992e64fbf8`, target-files `be9d9d51822cc607e721fa7a468eca1af0517798f2da3c59eae8e5ec73538d74`. Both checksum manifests passed, eight signed ZIP32 stages passed CRC/signature/range checks, and all eight installer stages returned RC=0 on the verified G965F. Fresh USERDATA UUID changed; BOOT readback matched `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`. All protected EFS/CPEFS/RADIO/RECOVERY/BOTA raw hashes matched immediately before/after installation. The device did not reach boot_completed within300s and returned to TWRP.

New evidence is `logs/20261028-m4-repair-build/attempt0209/postboot-recovery/cache-unica/logcat_live.txt`. Lines54545–54546 record `NetBpfLoad: success.` and bpfloader exit0. Line54555 records the next decisive failure:

```text
CANNOT LINK EXECUTABLE "/system/bin/netd": library "libcrypto.so" not found: needed by /apex/com.android.tethering/lib64/libnetd_resolv.so in namespace com_android_tethering
```

Lines54556–54560 show netd1shot exit1 and `reboot,netd1shot-fail`. Both post-fs and post-fs-data markers exist. Thus0209 resolves the manifest/activation failure, but not netd linking or network acceptance. Some optional Samsung BPF programs still fail to load; the loader nevertheless returns0 in this captured cycle, so they are not the immediate reboot trigger.

## Patch0210: keep the BZI6 resolver with its private TLS providers

The BZI6 resolver DT_NEEDED includes `libcrypto.so` and `libssl.so`; the pinned BZI6 APEX contains both, whereas the S22 base does not. Patch0210 copies exactly this same-APEX pair beside the resolver. No public namespace export is broadened. File scope: `a376b_android17_network_native.py`, its JSON lock, patch series/source lock, and this evidence record. The helper pins crypto SHA256 `2104e13a52b6188843f9d261430e0ba990bb3662b5fa94d7546ee4623aecaef6` and ssl `e09adfa93abbbbccbf5c0bda0f3d8c6418e71dfca1aa0109e93e19f33bfa79e8`; it adds a recursive DT_NEEDED gate requiring each dependency to exist inside the APEX or in its explicit manifest imports.

The selected stats composition was produced twice, byte-identically, at SHA256 `bd17e835c1140b20f18145ff9e42f2068f950d23d3ceec3de14c582aa30af848` /67,911,680 bytes. The full locked run passed78 metadata paths, expected seven payload changes, both matching manifests, ext4 roundtrip, AVB, container v3 and4KiB alignment. A separate static recursive audit checked23 DSOs with no unresolved strong symbols or symbol-version providers. Patch replay with fuzz0 is byte-identical. Base/L4S diagnostic output locks were recomputed with the same TLS closure; only the explicit diagnostic stats profile is selected for this device test.

A temporary read-only TWRP chroot test could not reach netd linking. Both the old and candidate APEX tests abort with exit134 and `arc4random data MADV_WIPEONFORK failed: Invalid argument` on the recovery kernel `4.9.118-g36e6ddff22b9`. This is not a TLS-runtime PASS. Temporary mounts were removed. The installed diagnostic BOOT already contains the separately calibrated WIPEONFORK backport, so the next runtime test uses that existing BOOT without another kernel build.

For the0210 diagnostic device test, only `/system/apex/com.google.android.tethering_compressed.apex` was replaced in installed attempt0209. The original APEX was backed up and verified against hash0698d84c…, the candidate was read back to the PC and matched bd17e835…, mode0644/UID0/GID0/system_file remained correct, and protected raw hashes including BOOT were unchanged. TWRP's chcon returned1 during the first staging attempt; the old APEX was restored/hash-verified before reboot, then the successful staging preserved and verified the original file context without relying on chcon. Detailed evidence and rollback backup are under `logs/20261028-m4-repair-build/attempt0210/device-test/`. Runtime boot/network/SystemUI/NFC/thermal acceptance remains pending until the following observation result.

## Patch0210 device result and namespace-audit correction

The user observed bootloop and returned the device to TWRP. Captured `attempt0210/device-test/postboot-recovery/cache-unica/logcat_live.txt:10629` reports NetBpfLoad success. Line10639 reports `cannot locate symbol "_ZN3fmt3v127vformatENS0_17basic_string_viewIcEENS0_17basic_format_argsINS0_7contextEEE" referenced by "/apex/com.android.tethering/lib64/libnetd_resolv.so"`; line10643 triggers netd1shot reboot_on_failure. The same failure repeats each cycle. TLS library discovery is fixed, but the S22 APEX libbase does not provide the BZI6 resolver's fmt v12 API.

The initial23-DSO static PASS was overbroad and is invalidated: its symbol union included private dependencies of SYSTEM imports as if those providers were visible inside Tethering. The corrected namespace filter rejects exactly two fmt v12 imports for this APEX placement; the SYSTEM placement passes20-DSO static closure. Both audit results are retained. This validates the negative fixture; it does not substitute for live loader acceptance. Original APEX0698d84c… was restored and read back after logs were saved; all protected raw hashes matched before/after rollback, and the device remains in TWRP. No BOOT write or reboot occurred during rollback.

## Patch0211: selected SYSTEM resolver namespace

The selected diagnostic composition now retains the S22 APEX's internal provider group, omits the transplanted BZI6 resolver/TLS files from that APEX, and stages the exact BZI6 resolver at `/system/lib64/libnetd_resolv.so`. It uses the matching SYSTEM BZI6 libbase/crypto/ssl/netdutils group. Exactly one `libnetd_resolv.so` declaration in BZI6 `/system/etc/linker.config.pb` moves from requireLibs(field4) to provideLibs(field3), with every other byte preserved. These fields are specified by [AOSP LinkerConfig protobuf](https://android.googlesource.com/platform/build/soong/+/10363161e/linkerconfig/proto/linker_config.proto). No global namespace isolation is disabled and no success/failure check is bypassed.

Files: existing network APEX helper/lock, postboot composition helper, finalization metadata, and actual ext4 image verifier. The existing fork build now stages/verifies both new SYSTEM paths; it checks14 exact SYSTEM dependency hashes plus the statsd APEX against donor and work bytes. A mutated-libbase fixture is rejected. The selected APEX was produced twice identically at650f75fa946ee5fbe44e38418c4581738e7654abcea527832c97d092d6d7671d /67,911,680 bytes, passes original metadata, expected three-file payload allowlist, AVB/v3/alignment and both manifest checks, and does not export or contain resolver/TLS additions. Resolver hash remains ccfa0d2f…fb849; linker.config output hash is0eb6fef30c067a54638019471c75bb5cb408955b38e4727eebfe61524963c480. Work fixture positive/negative checks and fuzz0 byte-identical patch replay passed. Full ROM packaging/real ext4 image gate and live device boot remain pending for this source revision.

The0211 staging gate initially rejected the new resolver because TWRP created its temporary inode as `unlabeled`; it rolled back the two already-replaced files before any reboot. Recovery's chcon returns1 and cannot be relied on here. A2,576-byte statically linked AArch64 recovery-only tool calls fsetxattr/fgetxattr on exactly the temporary resolver path, sets `security.selinux` to `u:object_r:system_lib_file:s0`, verifies all30 bytes and fsyncs. Its source/binary hash/compile command and device results are retained in attempt0211; it is not included in ROM or BOOT. Actual results: `fsetxattr_rc=0`, `fgetxattr_bytes=30`, `fsync_rc=0`, `label_exact=PASS`.

The successful0211 stage then read back all three candidate files to the PC, verified all hashes and metadata, checked15 pinned providers on the installed SYSTEM, and confirmed unchanged protected raw hashes including BOOT. No USERDATA wipe, vendor/radio or kernel change was needed for this diagnostic iteration. The device was rebooted once with that coherent provider relocation; boot observation result follows.

## Patch0211 runtime result and patch0212 black-display diagnosis — 28 October

The SYSTEM resolver namespace trial reaches `sys.boot_completed=1` while
Enforcing. netd PID1778 and main SystemServer PID2169 (zygote parent1783,
start tick4000) remain stable in the corrected120-second monitor. The first
observer's `BOOT_COMPLETED_BUT_UNSTABLE` is a false classification: it compared
all `pidof system_server` values, including a transient forked child. Preserve
that raw result; the corrected parent/start-time samples are in
`logs/20261028-m4-repair-build/attempt0211/device-test/live-after-observer/main-server-stability-120s.json`.
The netd maps show the resolver, crypto/ssl and libbase in SYSTEM; the separate
S22 APEX dependency group remains intact. This proves initialization, not DNS,
Wi-Fi/mobile-data or hardware acceptance.

The user reports a physically black screen. Both pre-trial screencaps are
black. Wake input changes PowerManager to Awake and the display to ON, but
SetupWizard/SystemUI still cannot draw. The decisive chain is:

- `attempt0212-aod-api/black-display/logcat.txt:62661–62664`:
  `open /dev/ion failed: Permission denied`, then
  `open_and_query_ion:537 ion_open failed with Permission denied`, then
  `GraphicBufferAllocator: Failed to allocate (1440 x 2960) layerCount 1 format 1 usage 20000b00: 5`.
- The actual node is0666 system:system but labeled `u:object_r:device:s0`.
  Neither BZI6 platform contexts nor the final vendor contexts maps `/dev/ion`;
  BZI6 retains the ion_device policy type but newer hardware uses dma_heap.
- A narrowly scoped live `chcon u:object_r:ion_device:s0 /dev/ion` succeeds.
  SELinux remains Enforcing, with no permission or binary change. After wake,
  screencap `attempt0212-aod-api/black-display/ion-label-awake.png` renders the
  Vietnamese Welcome screen, language selector, Start button and status bar.
  No further ion_open/GraphicBuffer allocation failure appears in that trial's
  captured2200-line tail. Physical-panel confirmation is separate.

Patch0212 extends the existing M4 runtime helper/lock to restore exactly one
`/dev/ion    u:object_r:ion_device:s0` platform pathname rule, with a pinned
BZI6 context source. It rejects missing, conflicting, duplicate or symlinked
context files; repeated apply is idempotent. All existing runtime fixtures pass.
The real ext4 checker now extracts this context file, compares bytes and checks
uid0/gid0/mode0644/file_contexts_file. A targeted real16MiB ext4 fixture passes;
this is not a newly completed full ROM image/package.

The actual installed context file changes from a3b099f7…95181d to
a51ada9d…348f7 by exactly37 appended bytes. Source fuzz0 replay matches all3
files. Recovery deployment saves and verifies the original file, writes only
its replacement contents while preserving inode metadata, reads it back to PC,
and verifies unchanged BOOT/EFS/CPEFS/RADIO/RECOVERY/BOTA raw hashes. The three
successful0211 network files are hash-checked unchanged. No wipe, kernel,
vendor or modem write occurs. Rollback restores the saved original context
file; the live-only trial also has an exact reverse-label command.

Persistent reboot validation is running; results follow below. The current
full packaged ROM remains attempt0209;0211/0212 are documented diagnostic
SYSTEM modifications on top of it.

A separate SystemUI issue remains measured, not fixed by0212:
`attempt0212-aod-api/black-display/crash.txt:19` reports
`AbstractMethodError: ...PluginKeyguardStatusView.disconnectPluginOnBackground()`
in the retained S9 Infinity AOD implementation. BZI6's AOD class has that method,
while the retained class lacks it. Do not replace this with an unreviewed no-op
or assume it explains every display issue. The one-time SystemUI restart and
optional Bixby receiver crash remain open for the next compatibility repair.

### Persistent0212 reboot result

The real reboot passes: automatic `/dev/ion` label is
`u:object_r:ion_device:s0`, platform-context hash is a51ada9d…348f7,
`sys.boot_completed=1`, and SELinux stays Enforcing. Main SystemServer PID1842
(zygote parent1453, start tick1547) and netd PID1451 survive the additional
120-second window. Total observer time249.6s includes boot and capture. The
post-reboot screenshot renders Welcome; the user independently confirms the
physical panel shows Welcome. No manual chcon is run on this reboot. The full
captured boot log has zero `open /dev/ion failed`, `ion_open failed` or
`Failed to allocate (1440 x 2960)` strings. This closes the measured black-display
fault. It does not close general hardware/network/thermal acceptance.

SystemUI still restarts once (PID3057 ->4885) due to the known AOD background
API mismatch; the main SystemServer remains stable. Evidence is
`attempt0212-ion-label/device-test/crash.txt:18`. A Bixby receiver crash remains
separate. Leave this device in Android for the next compatibility investigation;
do not rollback the successful0211/0212 boundary fixes merely because the
remaining plugin fault exists. A complete fresh ROM package containing these
changes has not yet been built.

Source handoff audit also exposed historical record defects. The preparation
script now selects -p2 for a/rom/ headers (0202+) and -p1 otherwise.0209's
hand-edited unified-diff hunk lengths were wrong, so GNU patch skipped its
existing report-field hunk; recounting those lengths restores the intended
source without changing any payload.0213 records one already-applied local
import removal in the release verifier, avoiding function-scope importlib
shadowing. All203 patches now replay from pinned HEAD into140 byte-identical
working source files with the preparation script's default GNU patch behavior;
14 historical patches use fuzz. The new0212 patch independently passes fuzz0.
Raw failed audit outputs are retained alongside the corrected full replay.

## Validation follow-up after0212

The subsequent validation does not change ROM/BOOT/policy. It confirms five sleep/wake cycles without another Java fatal and a user-confirmed physical Home action. At uptime1178.75s the main server/netd/SF remain the same; SystemUI4885 has lived1056.75s since the earlier restart. New material failures: Bluetooth30abnormal restarts; Bionic ping/nc EMSGSIZE with rawTCP PASS and a52-byte AF_UNIX control request receiving int32=-90; SDHMS brightness EACCES. NFC/SDHMS dumps both fail Binder transactions, so RF failure is not inferred from dump failure. Compiled installed-equivalent AOD API audit finds12missing abstract methods,10version-guarded and2unguarded; only the disconnect method has a measured crash. Details: [validation](m4-validation-after-0212.md), [current detailed plan](m4-post-0212-validation-plan.md).
