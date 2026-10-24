# Trạng thái thực thi — 21/10/2026

## Cập nhật 21/10/2026 — flash fresh G965F đã PASS

8/8 phần ADB17 + explicit TWRP Format Data: UUID mới, Settings/adb_keys/probe
cũ đã mất trước boot. Fresh boot165s, reboot46s theo watcher; mặc định trước
mọi test writes: provisioned0/setup0, adb/development1, Rampart master/strict/
blocked0/SUW0. Keyless USB CNXN/shell PASS cả hai boot. Auto Blocker GUI bật/tắt
có tác dụng thật; USB debugging callback PASS, GUI còn chờ sau initial setup.
Máy vẫn Welcome, đã dọn helper và restore stayawake0; N không nhắm tới.

Root cause data cũ: mke2fs báo USERDATA mounted nhưng optional format che lỗi.
Patch0018 source + archive guard + behavioral/replay checks PASS, chưa đóng
vào gói đang chạy. Báo cáo hiện hành: docs/m1-fresh-report.md. Lỗi USB sớm còn
mở; kernel/Hybrid Vendor/RIL không đổi, không suy sang A37/N.


**M1 G965F đã cài và boot thành công; người dùng đã xác nhận.** ADB đúng máy F
báo Android16/SDK36/OneUI80000, kernel DS-ACK V1.12 May nguyên bản, boot_completed1,
bootanim stopped và ảnh Welcome. Xem [báo cáo runtime](docs/m1-boot-report.md).
Đây là đối chứng S22, chưa phải A37 OneUI9/Android17. G965N không flash.

## Kết quả thực

- Fork hiện tại Android-16 commit2b25ae6fc3098650be249bd1898e21e563cd5df1, 19 patch/30 file
  replay byte-identical; 41 submodule và4464 LFS payload đã kiểm SHA/size.
  Bộ công cụ host build PASS; apktool185 tests không fail/error/skip.
- S22EUX S901BXXSOGZH3 và G965F/PHN G965FXXUHFVG4 tải/trích/MD5/metadata/CRC PASS.
  Dùng Hybrid Vendor VNDK33, blob/cấu hình F riêng và cầu nối radio của fork.
- Compile attempt5/package attempt8 PASS; 503 archive/2937ELF được kiểm tĩnh.
  Outputext4, SELinux compile, model/order, CRC/CMS signature và kernel hash PASS.
- Kernel dùng gói duhansysl V1.12 ngày08/05/2026 Enforcing-OneUI7 không KernelSU,
  SHA9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072.
  Không build/sửa kernel hoặc đổi version. BOOT thực sau cài khớp repack RAM55MiB;
  kernel/DTB/ramdisk đúng payload khóa.
- FullZIP5479600816B SHA6a0096cee0750d0d47dcaa0605bac7b65d362c0c7c40950814683d427fb745eb
  bị TWRP9 đọc size modulo2^32, từ chối trước updater; GPT được kiểm chưa đổi
  ở lượt đó. Mode --legacy-sideload mở rộng chính script đóng gói của fork,
  chia8 ZIP32 <2GiB; giữ range/data order, Brotli round-trip, CRC/CMS PASS.
  Live native updater RAM test PASS sai thứ tự/no-write và partialranges.
- Theo ủy quyền trực tiếp, đã cài8/8 phần lên F, marker từng phần thành công;
  SYSTEM/VENDOR/ODM/CACHE thực9500/1300/700/600MiB; USERDATA format ext4.
  GPT primary/backup CRC và table khớp; prefix entry1–17 không đổi.
- User xác nhận “đã boot thành công”; watcher thấy boot_completed ở165s.
  Uptime176.21s và342.48s đều boot_completed1; runtimeVNDK33 và APEXv33 hiện diện.
  Radio1.6/bridge2.0 hai slot và HAL camera/audio/Wi-Fi/Bluetooth/sensors đăng ký.
  Đây là kiểm đăng ký, chưa phải test chức năng.

## Tồn đọng phải giữ nguyên

- Audit trước boot **FAIL vì EFS đổi raw hash**; CPEFS/RADIO/RECOVERY/BOTA0/1/2
  không đổi. Chưa chứng minh EFS chỉ thay metadata. Sau khi được báo và hỏi
  backup, user trả “tiếp tục đi, máy dev thì backup làm gì”: đã tiếp tục boot
  theo ngoại lệ ghi efs-boot-exception.json; không restore, không đổi audit PASS.
- getenforce=Enforcing nhưng su có avc permissive1; ADBroot/debuggable1/secure0.
  Chưa nghiệm thu policy/security release. Không tự setenforce hoặc đổi policy.
- USB/ADB ngắt/kết nối; log init USB UDC báo No such device và symlink File exists.
  Đã thu logcat/dmesg ban đầu, snapshot HAL và logcat sau boot; dmesg bổ sung thất bại
  khi transport mất. Không kết luận máy reboot từ ADB mất.
- Hai SIM ABSENT; baseband đọc được nhưng gọi/data/IMS và toàn bộ chức năng phần
  cứng còn chưa test. Một RIL exit0 trong boot, sau đó daemon/HAL chạy trở lại.
- Hai snapshot logcat không có mẫu fatal exception/signal/ANR hoặc VINTF incompatible;
  dmesg ban đầu không có mẫu kernel panic/BUG/Call trace. Phạm vi chỉ log đã thu.

## Mốc tiếp theo — xem cập nhật M2 bên dưới

Gate boot M1F đã được user xác nhận. Chưa flash milestone tiếp theo hoặc N.
Cần hoàn thiện kiểm chức năng và xử lý/đánh giá tồn đọng M1 trước lượt thử tiếp;
M2 cần donorA37 Android16, M4 mới dùng A37Android17, giữ một biến mỗi mốc.
Đầu vào A37Android17 đã kiểm tĩnh release17/SDK37/OneUI90000/framework37,
chưa build/boot; A37Android16 và nguồn blob/CSC N còn mở.

Evidence chính: docs/m1-boot-report.md, docs/m1-artifact-review.md,
logs/20261020-m1-g965f/flash-01/{legacy-status,preboot-audit,efs-boot-exception,
boot-status,boot-analysis,postboot-analysis}.json. Raw log và binary chỉ cục bộ,
bỏ Git; không commit/phân phối firmware, không viết build system mới.


## Rule ADB cho các build sau

User đã chốt rule lâu dài trong AGENTS.md/PLAN: default Auto Blocker OFF,
USB debugging ON, USB ADB không hỏi RSA key, giữ công tắc/description thật.
Patch0017 common finalization + release verifier đã rebuild SettingsProvider/services
và qua compiled/negative/release/class-preservation checks. Default USB mới
sec_charging,adb theo trạng thái đã cấu hình thành công ở M1; hiệu quả USB cần
log candidate. Chi tiết docs/adb-defaults.md, logs/20261020-adb-defaults.
Candidate17 đã flash fresh trên F ngày21/10/2026 và qua default/keyless tests;
chi tiết m1-fresh-report.md. Source có thêm patch0018 cho lần package sau.
Không thay kernel/RIL/EFS hoặc ghi N trong lượt này.

## Hiện tại — M1 đã lưu, M2 chờ file đầu vào

User yêu cầu chuyển sang build M2 trên PC và giữ mọi ROM M1/M2/M4/M5 không ghi
đè. M1 ADB17 đã archive đủ10ZIP/16.888.429.178byte, SHA/size nguồn-đích PASS:
inputs/rom-releases/M1/20261021-g965f-adb17-ba4ed009bd98/.
Patch0019 retention có7fixtures PASS, replay19patch/30file byte-identical PASS.
ROM archive/running chứa17patch; checkout19, không đồng nhất hai trạng thái.

M2 chưa build: chỉ có A37Android17 trong dự án. User đang tải Full Files
SM-A376B/XXV/A376BXXS5AZH4 Android16 từ SamFW. Trang ghi OneUI8.5, cần xác minh
payload. Chưa tạo config/profile A376B16, chưa có M2/M4/M5 hoàn chỉnh; thư mục
trống dự phòng không phải ROM. Tiếp tục kiểm hash/CRC/SDK/OneUI khi file đến,
thêm donor song song S22 và build qua fork; kernel/vendor/radio giữ contract.
Xem docs/m2-build.md và docs/release-retention.md. Không thực hiện ghi điện
thoại trong lượt chuẩn bị này; quyền M1 không tự mở rộng sang M2/N.

## Cập nhật — M2 build sạch đã bắt đầu

Gói user tải ở Downloads đã MD5/SHA/CRC PASS, cả6Odin MD5 PASS, trích sudo exit0,
metadata sáu partition + CRC607 archive PASS. Payload AZH4 là Android16 SDK36.1,
OneUI80500. Patch20 LOCAL input,21 reviewed ADB lifecycle và22 donor selector/
cache/config đã kiểm; replay22patch/33file PASS, S22 config không đổi.
Build sạch M2 F đang chạy (logs/20261021-m2-build/attempt1.txt), kernel/Hybrid
Vendor/radio contract giữ nguyên. M1 archive đủ10ZIP chỉ đọc. Chưa có ROM M2
hoàn chỉnh hoặc log boot M2; không tự mở rộng quyền flash từ M1.

## Build M2 attempt2

Attempt1 FAIL camera prepare ABI (attempt1.txt:65); patch0023 giữ ABI A376B và
chỉ thêm null-manager guard, actual S22/A376B compiled fixture PASS. Replay23/
34file PASS, attempt2 clean đang chạy. Full A376B services.jar + ADB rebuild
riêng PASS, không sửa minSdk/SDK. Chưa có full M2 ZIP hoặc ghi thiết bị.

## Build M2 attempt3

Attempt2 FAIL inherited optional action-classifier claim, không phải kernel.
Patch0024 chỉ xóa claim không có implementation ở AZH4; five fixtures PASS,
replay24patch/35file PASS. Attempt3 clean đang chạy; chưa có full ZIP hoặc boot M2.
Warnings Bluetooth patterns/unsafe lens guard được giữ trong bugs.md để kiểm
runtime. Source Rampart/SUW key bindings đã review; không coi là test thiết bị.

## M2 AZH4 SehLights patch and build9

Attempt8's exact SehLights patch failures are documented in `docs/bugs.md`.
Patch0030 adds a guarded A376B16 AZH4 adaptation using the donor's VintfHalCache;
actual donor services.jar rebuilt with Apktool in preflight, evidence and SHA256
in `logs/20261021-m2-build/sehlights-a376b-preflight.txt`. Patch record/hash is
in `patches/fork/0030...patch` and `sources.lock.json`. Clean attempt9 is running.
No M2 ZIP, device flash, or hardware-light runtime test is claimed yet.


M2 attempt9 was interrupted mid-overlay and produced no package; attempt10 clean
passed the A376B SehLights adapter and Face omission, then failed at
`attempt10.txt:1000` because AZH4 MotionPhoto lacks the legacy
`configVideoEncoderParameters(...)` method. Patch0031 now guards only that edit
for the exact A376B16 profile; strict upstream behavior remains for other donors.
Recheck method/patch replay, then clean build11. No M2 ZIP/flash/runtime claim.

M2 attempt11 passed the MotionPhoto method guard and applied the donor
EncoderFilter format edit, then stopped at `attempt11.txt:1003-1005`: AZH4 has
no 32-bit `system/lib/libstagefright.so`, and its 64-bit file matches neither
known legacy signature. The G965F target vendor exposes OMX video codecs, so no
blind native patch or Android10 system-library substitution is made. Evidence:
`logs/20261021-m2-build/media-stagefright-a376b-review.json`. Awaiting direction:
port by disassembly, omit only the legacy OMX path with explicit limitations, or
keep M2 blocked. No ZIP, flash, or runtime claim.


## Current continuation — 21/10/2026, after attempt22 interruption

Attempt22 was interrupted while decoding SamsungCamera; no live build and no M2 ZIP. Stagefright disassembly port is statically verified: HEVC HDR10Plus assertion branch at 0xe5654 is changed to NOP, isolated output unchanged elsewhere. Patch0040 GameDisplayListener line-mode edit now passes actual helper execution plus Apktool rebuild. Audit patch0030 had malformed diff metadata; repaired record and lock SHA now replay PASS for all 40 patches / 40 source files. Shell and fork patch-layout checks pass.

Next blocker is preflighted: Settings dynamic-resolution patch spans 57 files and fails against AZH4 with 63 error lines. Awaiting user choice: port full WQHD/FHD/HD or temporary fixed-resolution M2. No fallback has been applied. Whole-ROM compiled ADB/defaults, release checks, package/provenance audit and device validation remain pending. M1 retained packages remain untouched. No phone operations performed during this continuation. Evidence: logs/20261021-m2-build/continuation-static-audit.json.


## M2 fixed resolution selected — 21/10/2026

User selected temporary fixed resolution for the M2 boot test. Patch0041 sets the existing dynamic-resolution target flag false only for the M2/A376B Android16 profile; no generic resolution/Settings/SystemUI switcher patches run in that profile. The fork's native star2lte baseline is 1440x2960 with ro.sf.lcd_density=560 and ro.sf.init.lcd_density=560; it remains selected without forced-size boot scripts. Clear the inherited WQHD/FHD/HD feature claim early and in both final system/vendor floating_feature.xml files; final audit rejects a nonempty switcher claim. The donor Settings screen-resolution controller remains unavailable; actual panel resolution/visibility requires runtime verification.

Scope: target/profile files and existing device-stack feature edits/audit only. M1 remains dynamic; wrong-donor/milestone fixed mode is rejected. Config fixtures pass M1=true, M2=false and invalid profile rejection. All 41 source patches replay to 40 byte-identical files; shell syntax passes. Mandatory ADB defaults/compiled code checks stay required. Next clean build23 with full ROM packaging requested through the existing fork; no device actions.


## Current M2 checkpoint — product-feature preflight0042–0048

Clean attempt23 exited1 at HDR branch (`attempt23.txt:1053`); no ZIP M2. Fixed-resolution selection remains1440x2960/density560, target60Hz, no switching UI. Scoped patches cover exact DEX relocation/HDR controllers, rear capacitive FP, AZH4 HFR thresholds, actual HW mDNIe methods, identical StrongBox false return, Wi-Fi backoff and consistent late Face omission. Replay48patches/44files and layout154 PASS; local actual donor preflights pass those edits. Whole build/compiled release/runtime gates remain open.

Current unpatched blocker is Wi-Fi80211AX UI. Even exact descriptor relocation fails (Settings8/9 resolved +16errors, SystemUI3/3 +6errors); one lambda absent. User preference question pending: port complete Wi-Fi UI now or temporary donor UI for M2 boot, potentially showing Wi-Fi6 scan labels. No bypass applied. Separate APK assembly/ADB audit is ongoing. Incomplete debug work tree must not be packaged. M1 archives unchanged; no phone actions this continuation.


Checkpoint0048 verification completed: all six actual-tree artifacts (framework.jar, services.jar, BiometricSetting, SettingsProvider, SecSettings, SystemUI) rebuild through the fork's Apktool pipeline with exit0. After applying the existing ADB defaults recipe to this diagnostic tree and rebuilding services/SettingsProvider, the verifier reports `PASS compiled ADB defaults, no-auth properties and stored-choice lifecycle`. Compiled rear-FP constants and actual restored mDNIe methods pass independent disassembly checks; standard Face variant audit passes. These are diagnostic-tree static results, not a clean whole-ROM release pass or runtime result. Evidence `product-feature-checkpoint48.json`, `compiled-hardware-preflight.json`, `adb-current-compiled-verify.txt`, `face-current-compiled-audit.txt`. All10 retained M1 manifest files still exist with original sizes; no archive writes. Current Wi-Fi UI question remains pending; no further clean build/ZIP/flash.


## M2 donor Wi-Fi UI selected — 21/10/2026

User explicitly selected temporary donor Wi-Fi UI for M2. Patch0049 adds `STAR2LTE_M2_KEEP_DONOR_WIFI_UI=true` in M2 and resets it false in M1; the existing product_feature recipe validates M2/A376B16/star2lte/Wi-Fi5 scope and skips only the two old 80211AX UI diffs for Settings/SystemUI. It leaves source metadata, target Wi-Fi flags, driver/HAL/blob, custom-backoff adaptation and all other Wi-Fi recipe branches intact. Donor scan labels/icons may still show Wi-Fi6; that is a known temporary UI limitation, not S9+ hardware support. No extra UI bypasses are authorized by this selection.

Actual branch fixtures PASS selected0patch calls / strict2calls / wrong-donor and wrong-milestone rejection; M2-to-M1 flag reset PASS. All49 audit patches replay to44 byte-identical source files. Clean attempt24 is running through `make_rom.sh --clean-work-dir --build-rom-zip`, log `logs/20261021-m2-build/attempt24.txt`. Full release gates, retained run packaging and M2 runtime remain pending. No phone actions; pinned kernel/Hybrid Vendor/radio unchanged.


## M2 continuation after Wi-Fi UI decision — patches0050–0053

Attempt24 clean stopped at `attempt24.txt` with `Method "getMainInstance()Lcom/samsung/android/hardware/display/RefreshRateConfig;" not found ... SemInputFeatures.smali`. This was a local0044 regression: its METHOD loop variable leaked into SMALI_PATCH replaceall's method check. Patch0050 renames/unsets the loop selector and checks methods only for operations with a method argument; field-only replaceall with an unrelated caller METHOD passes, missing real methods still fail.

Diagnostic continuation then logged the user-selected donor Wi-Fi UI and stopped at `product-tail-after-input.txt:94-95`, `SemWifiInjector.smali:1641: patch does not apply`. Patch0051 ports the existing target APE/personalization=false behavior on actual AZH4: removes only the SHA-checked APE construction region, verifies no external label references, keeps null guards, disables the exact L4s default, disables the Qbox APE observer, and splits only SUPPORT_APE ordinal35 from a shared SQE branch. The moved Settings develop/ApePreferenceController is disabled and its reset seed becomes0; old ape map entries are verified absent. No scan icon/label changes. Four classes change; unknown enum fails before writes. Native/Settings runtime needs logs.

Patch0052 ports SoftAP target differences after old AX/country/power/sharing diffs fail. The AX feature1 case uses AZH4's existing public driver query, with null fallbackfalse. Country fallback applies only for missing/NA HAL values and preserves permission enforcement, valid HAL results and Binder identity cleanup on success/exception. Power support reads the driver string instead of compiledtrue; worker initialization is disabled as in the original fork, and only SQE power command ordinal29 is split to unsupported. Sharing-lite is conditional on sharing and retains carrier/model exclusions. Strict method hashes and command mappings are checked; unused callback classes retained without synthetic-ID guessing. Country/power/sharing fixtures and no-partial-write rejection pass. Actual product-feature diagnostic remainder passes. Separate jar assembly succeeds; actual-work-tree jar rebuild is also being checked. Runtime remains open.

SAIV then stopped at `remaining-modules-preflight.txt:5` missing `vendor/etc/midas/moire_detection/moire_detection.tflite`; A376B stores the identical model under VslMesDetector, SHA256 `88e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1`, which matches the older model exactly. Patch0053 verifies/copies the A376B bytes to the path still referenced by donor Midas config, using explicit parent/file metadata. A later missing generic hs_segmenter.info is addressed by validating/copying AZH4's intact two named info/model pairs. SAIV diagnostic preflight now passes; PhotoEditor runtime still needs logs.

All53 patches replay to47 byte-identical source files. Diagnostic continuations use the existing fork module/helper entry points and do not mark .completed or package the partial tree. After remaining module preflights, a fresh full build is still required. Mandatory ADB defaults/compiled audit, pinned duhansysl kernel and Hybrid Vendor/radio remain required. No phone actions or completed M2 ZIP; M1 archive untouched.

## M2 continuation — 22/10/2026

Clean attempt41 ended exit1 at Settings search factory (`attempt41.txt:1489`). Patches0071–0076 handle reviewed Settings/search/MultiSound/setup adaptations and temporary optional-feature omissions; replay76/58files PASS, search/MultiSound/setup class DEX assembly PASS. Settings tail, supported Settings hooks, tweaks/SystemUI, wallpaper and Weather diagnostic continuations PASS. Final vendor restore/finalization diagnostic is running in `logs/20261021-m2-build/finalstages76.txt`. Staging remains incomplete; no M2 release ZIP and no phone action. Next: finish final-stage preflight, clean build, compiled mandatory ADB defaults/control checks, kernel/vendor/radio audits and retained package publication.

Latest M2 state — 22/10/2026: final-stage diagnostic ended exit1 at `finalstages76.txt:90` (missing system32 graphics interface). Audit finds a wider ABI mismatch: A37 has arm64-only app/framework/runtime, while hybrid vendor includes ELF32 audio/camera/OMX services and selects zygote64_32. Native32 runtime/namespace work is required; app32 versus app64 scope awaits user. See docs/m2-abi-review.md. No ABI check bypass or payload swap, no attempt42, no complete M2 ZIP; M1 archive not modified. Patches0071–0076 remain replay-verified diagnostic adaptations, including provisional optional Settings omissions.

Current M2 state — 22/10/2026 (supersedes the preceding pending-decision entry): user confirmed app64-only and hiding five unsupported optional controls. Patches0077–0080 prepare pinned native32 runtime while preserving all original A37 runtime files and the fork radio wrapper, remove unsupported eSIM MEP declaration, adapt SecureFolder namespace and scope S22-specific final workarounds. Replay80/61files PASS; diagnostic final-last80 exit0. Clean attempt42 is running detached through the original fork pipeline. Full rebuild, compiled mandatory ADB/Auto Blocker checks and release/package audits remain pending. No complete M2 ZIP or phone action. See docs/m2-abi-review.md and docs/m2-build.md.

Latest: clean42 failed at Photo Editor inventory using S22 path (attempt42.txt:2393). Patch0081 recognizes AZH4 PhotoEditor_Full; donor/stage hashes match and inventory helper PASS. Replay81/61files PASS; remaining finalization diagnostic exit0. Clean attempt43 now running detached via original pipeline. No completed M2 package or phone action. M1 archive10 ZIP hashes/sizes rechecked PASS.

Attempt43 compiled partial audits: rebuilt SettingsProvider fresh-data seed calls/SQL PASS (settingsprovider-compiled43.json): global adb_enabled/development_settings_enabled=1, Rampart defaults0, INSERT OR IGNORE. Actual rebuilt SecSettings resource XML absence audit PASS for all5 omitted optional controls, with Bixby positive-control XML present (optional-controls-compiled43.json). These do not replace the pending full services lifecycle/boot/release checks. Build continues APK/JAR rebuild; no completed M2 ZIP or phone action.

Attempt43 ended exit1 after29min10sec at the compiled boot contract: missing UN1CA Bixby remapper in system/system/framework/services.jar. All APK/JAR rebuilds completed; final wallpaper and actual rebuilt SystemUI Brief/Edge Lighting gates PASS. Earlier0036 intentionally skipped the incompatible A37 key-event remapper, but Settings controls and USER keylayout mapping remained; gate correctly blocks this inconsistent feature. User has been asked whether to omit/hide remap with source keylayout retention for M2 or port it fully; no choice assumed and no further Bixby patch applied.

Independent read-only release-default checks PASS in release-defaults-independent43.txt: pinned native32 payload/zygote, selected Face omission, no-KernelSU runtime, compiled ADB properties/seeds/stored-choice lifecycle, Infinity and PIF defaults. Its Bixby resource-presence check is only UI inventory and does not prove functional remapping; the full boot contract remains FAIL. No complete M2 ZIP and no phone action. Current compiled tree retained; .completed/release publication not bypassed.

Current: user confirmed omit/hide M2 Bixby remap, original keylayout.0083 source/compiled gates prepared, replay83/62files PASS. Exact reviewed delta applied to attempt43 decoded Settings and2 keylayouts; existing fork resume attempt44 running detached with rebuild+all gates+unique package. Old compiled Settings rejection PASS. No complete M2 ZIP, no phone action.

Attempt44 rebuild was deliberately stopped via its own process group after1min54sec, before gates/package, to use existing fork UN1CA_APKTOOL_JOBS=4 on72 logical CPUs/53GiB available RAM. Its trap misleadingly prints Build completed on SIGTERM; attempt44.interrupted is authoritative, no exit record or package completion exists. Attempt45 is the same original --resume-after-mods --build-rom-zip with4 existing Apktool jobs, detached; no build-system/payload change.

Attempt45 reached SELinux compile and failed on stale S22 CIL allow `otapreopt_slot -> vendor_apex_file` (`plat_sepolicy.cil:78269`); patch0084 removes only this undeclared A37-domain rule. Actual helper, Exynos9810 verifier, full staged `secilc` and84-patch replay PASS. Attempt46 resumed the existing fork packaging pipeline and exited0. All compiled/release checks PASS, including mandatory ADB/Auto Blocker lifecycle, Hybrid Vendor VNDK33/radio, no-KSU pinned DS-ACK, enforcing SELinux, ext4 images, ABI and keylayout/Settings gates. Release retained at `rom/out/releases/M2/star2lte-1792642788-JFvPsK/`; both ZIPs SHA256-check and pass `7z t`. Flashable SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`; target-files SHA256 `d4c3b69fe96f39888f15b3671633b16db6258f99f620263794a7805ebcbb5efe`. PC build/package complete; M2 flash/boot/runtime validation remains pending and no phone was touched. M1 archive remains untouched. Full details in `docs/m2-build.md` and `logs/20261021-m2-build/attempt46.txt`.


Latest M2 runtime status — 22/10/2026: one-time user-authorized fresh flash on G965F (`M2_G965F_SERIAL`) completed via eight existing legacy ZIP32 stages because TWRP rejects the >4GiB Zip64 package. Stage08 retry2 exited RC=0 and formatted fresh USERDATA; package hash remains `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`. The phone repeatedly restarted during system boot and is now in TWRP. Read-only logs are saved under `logs/20261022-m2-g965f-flash-01/`. `/proc/last_kmsg` only shows repeated bootloader starts ending in `Starting kernel...`; pstore has no console ramoops, and no Android failure signature was captured. A single controlled boot retry never exposed G965F through ADB in75s; host saw Samsung MTP `04e8:6860`, while only G965N recovery remained in `adb devices`. Root cause undetermined; M2 boot/runtime acceptance FAIL, not accepted. G965N and M1 archive untouched; no further flash/format/reboot. See `docs/m2-build.md`.


Continuation audit 22/10/2026: comparison of retained M1 versus M2 flashable ZIP entries confirms `boot.img`, `exynos9810/ramdisk.img`, `kernel.zip`, `install` support/preflight/layout/vendor_bootfix payloads are byte-identical where present; only `exynos9810/format_partitions.sh` differs among the compared installer entries. Exact SHA evidence: `logs/20261022-m2-g965f-flash-01/m1-m2-installer-comparison.json`. This does not prove the on-device BOOT was installed identically and does not establish a cause. Current PC ADB inventory is empty; waiting for G965F TWRP connection to read and unpack actual BOOT and compare against the known-good M1 boot evidence. No kernel change or device write made during this audit. The fork already contains optional early/late diagnostic logger paths; their suitability must be reviewed before any diagnostic-image proposal.


Recovery audit02 — 22/10/2026: G965F reconnected and verified as SM-G965F / G965FXXUHFVG6 / TWRP3.7.0_9-0. Read-only full BOOT capture (57,671,680 bytes) matches the M1 flash03 known-good BOOT **byte for byte**, SHA256 `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Kernel/DTB/ramdisk hashes match M1 independently. Samsung legacy header is not accepted by the AOSP unpack tool (`UnicodeDecodeError` decoding header); used the established M1 verifier offsets to inspect it instead. Nine installed early userspace files (init, bootstrap linker/libraries, vendor fstab pair) match the staged M2 tree; SYSTEM/VENDOR mounted `ro,noload` for hashes then unmounted. Existing recovery-mounted data only contains lost+found/media, with no Android system/misc/tombstone or bootloop logger directories. Evidence: `boot-audit-recovery-02/{boot-comparison.json,early-files-comparison.json,recovery.log,last-kmsg.bin,pstore/}`. No partition writes or reboot in this audit.

Ramdisk `/init` is the unchanged statically linked Android29 first-stage binary. This is inventory only: compatibility with the A37 second-stage init remains unproven and must be diagnosed by early-boot logs. Existing `patch_bootlogger.sh` must not be invoked as-is: it replaces PID1 `/init`, while current `bootlogger_init.c` explicitly implements a standalone logger that must never replace/chain Android init. That source/installer mismatch makes it unsuitable as an unreviewed diagnostic shortcut. No logger image has been installed.


Repeat fresh M2 flash02 (22/10/2026, direct user request): all eight hash-verified existing ZIP32 stages installed sequentially on G965F serialM2_G965F_SERIAL; each updater RC=0 with correct next-stage state. Stage08 ran from RAM without retries. USERDATA UUID changed from `d9175a13-acb8-4ea3-9c85-14333b55afe9` to `74d8755a-7aff-4d40-93da-20b57bda9b97`; preboot read-only mount shows only lost+found. Actual BOOT SHA remains `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Fresh install verified; system boot watcher now running. Evidence: `logs/20261022-m2-g965f-flash-02/`. Runtime result pending; no N actions or kernel changes.


Flash02 runtime follow-up: user confirmed the fresh test still bootloops. System ADB did not become available during the observation window, so Android logcat/dmesg could not be retrieved. User was asked to return G965F to TWRP for post-failure read-only capture. All eight installer stages RC=0, UUID-change and empty-data proof remain valid; installation success does not imply boot success.


Flash02 postboot capture complete: G965F verified again in TWRP (`SM-G965F`, `G965FXXUHFVG6`, serialM2_G965F_SERIAL). Raw read-only logs in `logs/20261022-m2-g965f-flash-02/postboot-recovery/`. 2MiB last-kmsg trace records repeated normal boots 07:31–07:34 UTC then recovery; line22392 is `Starting kernel...`. Pstore only has pmsg-ramoops-0 (16,372 bytes), with adbd FunctionFS/USB endpoint shutdown messages, including `remote usb: read terminated (message): Cannot send after transport endpoint shutdown`; this does not establish a system-boot cause or which boot produced every record. No console-ramoops/panic/Android init failure captured. USERDATA still has the new UUID74d8755a-7aff-4d40-93da-20b57bda9b97 and only lost+found/media after recovery creates its media directory. Fresh install did not resolve the bootloop; runtime gate remains FAIL, root cause unconfirmed. Phone left in TWRP; no additional write/reboot during this capture.


Patch0086 / early diagnostic prepared on PC: new optional offline helper `rom/scripts/internal/prepare_star2lte_boot_diagnostic.py`, no automatic build activation. Only legacy BOOT command-line field changed; pinned kernel/ramdisk/DTB/tail unchanged. Diagnostic SHA `a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9`, rollback SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Wrong-input/no-output, output-collision and syntax checks PASS. No image flashed. Review: `docs/m2-early-diagnostic-review.md`; actual pstore/fatal behavior needs device evidence. Await user authorization for this distinct BOOT diagnostic plus rollback, G965F only.


Early diagnostic test01 authorized and installed on verified G965F on22/10/2026. BOOT full readback matches a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9. Only command-line field changed; no kernel/DTB/ramdisk replacement or data format. Boot observation in progress; preserve captures then restore pinned baseline. Evidence `logs/20261022-m2-early-diag/test-01/`. AGENTS.md now reflects user's project flash authorization; F-first/N-reference plan stays.


Early diagnostic test01 completed22/10/2026: user confirmed bootloop and returned G965F to TWRP. Device identity and diagnostic BOOT SHA were verified; `/proc/last_kmsg`, `/proc/sec_log`, pstore and recovery logs captured under `logs/20261022-m2-early-diag/test-01/postboot/`. No console-ramoops or M2 fatal/panic signature recovered. Logs contain recovery kernel `Linux version 4.9.118-g36e6ddff22b9`; fstab errors (`/etc/recovery.fstab`) and hung-task lines belong to a recovery trace and are not evidence of M2 failure. Diagnostic effectiveness unproven, root cause unresolved. Original BOOT restored and entire partition readback SHA verified `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; no post-rollback reboot. Phone left TWRP. No kernel change, no data format, no N operation. Result JSON in test01. Next investigation must improve capture before/through early kernel/init rather than repeat unchanged fresh installs.


Patch0087 / explicit PID1 pmsg probe02: optional raw-syscall AArch64 monitor in `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`, prepare helper extended for cpio. Original init preserved as /init.real; probe keeps PID1 then execs original with argv/env, child copies selected kmsg errors to pmsg only, no FS mounts/partition access. TWRP fork/wait/exec/env self-test PASS, non-PID1 refusal13. Existing DS-ACK MagiskBoot repacks RAM files. Independent CPIO audit preserves all other data/metadata; kernel/DTB/cmdline unchanged. BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926. Diagnostic only: timing/SELinux can affect capture; compare results cautiously. Plan: one verified G965F boot under current project authorization, capture then rollback baseline cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d. Evidence `logs/20261022-m2-early-diag/pid1-probe-02/`.


Probe02 installed under current project flash authorization on verified G965F; readback full BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926 PASS. One system boot requested. No system ADB observed in the initial90-second window; waiting for user-assisted return to TWRP to capture pmsg/last_kmsg/sec_log and restore baseline. No new success/root-cause claim; probe effectiveness unknown until logs are collected. Probe remains installed pending recovery access. Patch0087 reverse/apply preflight PASS.


Probe02 completed: verified G965F returned to TWRP; probe BOOT hash matched. Captured pstore (empty), last_kmsg/sec_log/recovery/reset reason under `logs/20261022-m2-early-diag/pid1-probe-02/postboot/`. No M2-PID1 marker or M2 fatal recovered; absence does not distinguish pre-init failure from unavailable pmsg/sysfs/dev nodes or recorder failure. Restored baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d with entire-partition readback PASS, no reboot. Phone TWRP.

Patch0088 is an unflashed follow-up prototype: isolated monitor mount namespace for only tmpfs/sysfs so original init mount table stays untouched. RAM preflight FAILED `NAMESPACE stage=4 errno=016` (sysfs mount EBUSY), exit20; parent mount table SHA unchanged. Do not flash this prototype; no diagnostic image03 created/installed. Evidence `logs/20261022-m2-early-diag/pid1-probe-03/namespace-selftest.txt`. Root cause still unresolved; preserve pinned kernel and baseline BOOT.

22/10/2026 probe04 and probe05 completed on G965F. Probe04 pmsg and probe05 CACHE persistence passed in TWRP, but neither captured an M2 PID1 runtime marker after the bootloop. Probe05 CACHE file still held only its TWRP self-test line. Full BOOT readback after each rollback matches pinned baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; phone is in TWRP. Bootloader logs show `Starting kernel...` but no Android kernel/init failure line. Offline A37 init bootstrap dependency closure resolved all 35 needed names in 36 scanned ELF files; this excludes a simple missing bootstrap library, not other init/SELinux failures. M2 runtime gate remains FAIL; no G965N flash or kernel change. Evidence and limits in `docs/m2-build.md` and `docs/bugs.md`.

23/10/2026: M1 control fresh boot PASS and probe06 recorder calibration PASS (`/cache/m2-early-boot-probe06.log` has PID1 marker and init lines). M2 fresh install 8/8 PASS on G965F, USERDATA UUID new, protected partition hashes unchanged during install. M2 with same calibrated recorder bootlooped: 240 s no ADB; CACHE contains 23 PID1 markers but no true init/fatal line, last selected kmsg ~0.84 s. Exact reset cause still unknown. Diagnostic BOOT rolled back to pinned baseline SHA `cc618abc...776ed01d` full readback; G965F remains in TWRP with M2 system/data. G965N untouched. No kernel build/edit justified.

23/10/2026: Bước chẩn đoán probe07 hoàn tất trên hai G965F song song. Máy đối chứng mới M1_G965F_SERIAL cài M1 8/8, boot completed với CACHE marker/heartbeat. Máy M2 M2_G965F_SERIAL vẫn bootloop: 240 s không ADB, CACHE 19 PID1 markers nhưng không heartbeat. Cả hai BOOT đã restore baseline SHA `cc618abc...776ed01d` full readback; hiện đều ở TWRP. Chưa có log fatal xác định nguyên nhân M2; chưa sửa kernel/vendor/radio. G965N chưa tác động.

23/10/2026: Probe08 đã chạy đủ trên hai G965F. M1 boot thành công, 17 heartbeat đến ms1761; M2 bootloop 240s không ADB, CACHE 19 PID1 markers và tối đa một heartbeat mỗi segment. Cả hai BOOT đã restore baseline SHA `cc618abc...776ed01d` full readback, đều ở TWRP. Chưa có fatal line xác định root cause; M2 gate FAIL, G965N chưa tác động.

23/10/2026: Probe09 hoàn tất. M1 control serial `M1_G965F_SERIAL` boot completed; M2 serial `M2_G965F_SERIAL` không có ADB sau 240s. M2 CACHE ghi `M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1`, không có `M2-INIT-EXEC-FAILED`, nên exec `/init.real` đã thành công ở các segment được ghi. pmsg 1817 byte được thu trong phiên TWRP sau bootloop nên adbd/USB records không gán được cho M2; chưa có panic/fatal M2 xác định. Thu CACHE/last_kmsg/pmsg và SHA lưu ở `logs/20261023-probe09/`. Hai BOOT restore baseline `cc618abc...776ed01d`, full-partition readback PASS; hai máy ở TWRP. M2 root cause còn mở; không sửa kernel/vendor/radio và G965N chưa tác động.

23/10/2026: Read-only USERDATA audit M2 hoàn tất: `/data` chỉ có lost+found và media/TWRP; không có Android tombstone/ANR/DropBox/bootstat/logd. pmsg hiện tại là từ phiên TWRP, không chứng minh adbd M2 đã chạy. Inventory + recovery logs lưu tại `logs/20261023-m2-postboot-userdata/`. Tiếp theo dùng hook logger/init-debug có sẵn của fork, gói calibration M1 trước rồi M2; chưa đổi kernel.

23/10/2026: M1 logger calibration build0095 đã cài trên G965F phụ `M1_G965F_SERIAL`; tám stage RC=0, USERDATA ext4 được format, Android boot completed SDK36/OneUI80000. ADB không RSA được xác nhận trực tiếp (`ro.adb.secure=0`, key dir rỗng, host status `device`); USB debugging=1 và mọi seed Rampart=0. Logger đầu boot ghi được logcat và marker CACHE nhưng `getprop/dmesg/proc` trong file tổng hợp rỗng; chạy logger trễ trong CACHE tạo đủ dữ liệu, nên timing/service-context cần rà soát. EFS raw hash đổi; CPEFS/RADIO/RECOVERY/BOTA giữ nguyên, installer/kernel không có lệnh ghi EFS; không restore/ghi EFS. M1 report/evidence: `docs/m1-boot-report.md`, `logs/20261023-m1-init-debug/flash-01/`. M2 build attempt45 đã fail `plat_sepolicy.cil:78269`; attempt46 chỉ bị guard FaceunlockFix variant từ chối resume. Attempt47 đang clean-build theo fork để chụp đủ CIL và tái hiện policy gate. G965N chưa bị tác động.

23/10/2026: M2 attempt47 clean build kết thúc exit0, policy/ADB/ABI/image/kernel gates passed. Release mới được giữ tại `rom/out/releases/M2/star2lte-1792735195-f3GTW7/`; SHA flashable `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`, target-files `14ccb6675bcae1ffeddd0b41f4488e3b2bd0aaa4f8c3db00970b0c74c4d474da`. Clean CIL gate qua; log xác nhận Hybrid Vendor VNDK33, cầu nối N770F IRadio1.6 và DS-ACK V1.12 duhansysl no-KSU. So với M2 trước, BOOT image giống; SYSTEM/VENDOR/ODM hashes đổi. Chưa flash package mới, M2 runtime vẫn FAIL từ các lượt trước; G965N không bị tác động.

23/10/2026 — M2 attempt47 fresh flash completed on G965F `M2_G965F_SERIAL`, package `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`. All 8 ZIP32 stages RC=0; fresh USERDATA UUID confirmed; protected hashes passed preboot; BOOT baseline matched before reboot. Android ADB did not complete within 240 seconds, user returned device to TWRP. Captured last_kmsg/pstore/recovery; no specific Android or kernel fatal cause. M2 runtime FAIL / root cause open. See `docs/m2-build.md`, `docs/bugs.md`, and `logs/20261023-m2-attempt47/flash-01/`. Next: M1 calibration of existing init/vendor + system boot logger flags, then M2 diagnostic build; no kernel change without evidence.

23/10/2026: M2 attempt48 logger build fresh-flashed on G965F `M2_G965F_SERIAL`; all 8 stages RC=0, fresh USERDATA UUID, protected raw hashes unchanged, BOOT baseline. Device bootlooped and returned to TWRP. No `/cache/unica` or `/data/bootloop-logs`; captured pstore/last_kmsg are recovery-associated (last_kmsg line 14416 reboot,adb, line 14624 restart command adb), no attributable M2 fatal/phase marker. No kernel change. Next: attempt49 with existing `EXYNOS9810_ENABLE_VENDOR_BOOT_TRACE=true` plus current loggers; verify vendor trace payload, then test fresh on same G965F. See `docs/m2-build.md`, `docs/bugs.md`, `logs/20261023-m2-attempt48-init-debug/`.


23/10/2026: M2 attempt49 vendor-trace profile built and fresh-installed on G965F `M2_G965F_SERIAL`. Build `06f83d57...31cb34d`, 8/8 ZIP32 stages RC=0, USERDATA fresh UUID, BOOT baseline and protected hashes unchanged during install. Vendor init rc passed final-tree/SELinux/fs metadata checks. Reboot watch 240s: ADB target absent; returned to TWRP and postboot capture complete, with no attributable Android error in retained logs; read-only TWRP mount verified the installed vendor trace rc is present, but not executed. EFS hash differed from attempt48 before attempt49 writes, remained stable during attempt49; no EFS write/restore. G965N and M1 control untouched. See `docs/m2-build.md`, `docs/bugs.md`, `logs/20261023-m2-attempt49-vendor-trace/`.

23/10/2026: Attempt49 postboot capture in TWRP found no attributable Android fatal or phase marker. `/proc/last_kmsg` is 2 MiB and ends at the bootloader-to-kernel handoff (68 `Starting kernel...` entries, last at line 38096); pstore contains recovery-associated pmsg only (951 B), with console-ramoops/CACHE logger absent. Captured sec_log line 4537 is a TWRP FunctionFS mount error, not Android M2 evidence. BOOT remains baseline. EFS hash changed after failed boot (from `76b914…` to two repeated read-only device results `a0892e…`); timing unknown, no EFS write/restore. M2 cause unresolved; no kernel/radio change. Evidence: `logs/20261023-m2-attempt49-vendor-trace/postboot/`.

23/10/2026 follow-up: Confirmed attempt49 already included patch0095: build log lines 1712–1713 records metadata for both vendor logger files, and read-only TWRP mounts found init.debug.rc and init_debug_log.sh installed. This is consistent with attempt48 missing logger payload, but does not explain attempt49 lack of CACHE. Static read-only /system/bin/init audit found Android36 AArch64 PIE + bootstrap linker and all listed dependencies on SYSTEM; runtime loading/SELinux remains unverified. Probe09 line3 reports M2-INIT-EXEC-PIPE-EOF; no Attempted to kill init line exists, so panic is not established. Same-flags attempt50 would duplicate attempt49. Evidence in logs/20261023-m2-attempt49-vendor-trace/postboot/system-init-elf.txt.

23/10/2026 diagnostic result: Step1 linker test failed reproducibly on DS-ACK 4.9.337: A37 linker aborts with arc4random data MADV_WIPEONFORK failed: Invalid argument (exit 134) while M1 native linker lists its init (exit 0). This explains the missing second-stage init/post-fs CACHE markers before init or sepolicy can be evaluated. Probe10 not run because its precondition (linker passes) failed. No kernel/vendor/radio/EFS writes or flash/reboot. Evidence: logs/20261023-m2-attempt49-vendor-trace/postboot/.

23/10/2026 follow-up runtime diagnostic: On the installed M2 SYSTEM mounted ro,noload in TWRP, the A37 linker reproduced the same exact MADV_WIPEONFORK EINVAL abort for init, sh and secilc (exit134). On the same G965F CPU, a temporary M1 Android16 bootstrap linker+bionic overlay in TWRP tmpfs successfully listed A37 init/sh and ran a harmless A37 shell command (exit0). This is evidence for a M2-only bootstrap compatibility overlay candidate, not proof that init SELinux setup/second stage will boot. Probe10 remains gated; no init mode, policy compile, flash, reboot, partition write, kernel/vendor/radio/EFS change occurred. TWRP mounts were cleaned up; M2 remains in recovery and M1 control remains booted. Evidence: `logs/20261023-m2-linker-runtime-step1/runtime-test.txt`.

24 October 2026 — next diagnostic completed: actual M2/A37 secilc+CIL compiles
successfully in read-only recovery chroot and on booted M1 pinned DS-ACK,
producing identical policy SHA2564725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b.
No live policy load. Probe10 signal decoding was wrong (127 instead of WSTOPSIG),
causing reproducible PTRACE_CONT errno5; the apparent compile stall is unreliable.
Probe11 correction source passes signal-forward/attach selftests on both kernels,
but no probe11 BOOT has been built/flashed. M2 remains TWRP with baseline BOOT.
Current collaboration brief: docs/HELP-WANTED.md; shareable evidence: support/evidence/.


24/10/2026 — probe11 and passive probe12 completed. Probe11 fixes the wait-status
bug and captures compiler exit0 plus kernel SELinux initialization; passive
probe12 reproduces the loop without PID1 ptrace (14 starts, 11 load-policy
markers, 10 superblock-init markers). No explicit fatal/panic or second-stage
start is captured. Both BOOT rollbacks verify the pinned full-partition hash
cc618abc...776ed01d; M2 is in TWRP, M1 stays booted, N untouched. EFS changed
across boot/recovery, without an EFS write/restore; other measured protected
hashes match. Logger permission/visibility across policy activation is the
next instrumentation gate. Source and selected evidence are prepared for the
GitHub assistance snapshot; images, firmware and raw logs stay local.
Live API inspection now reports the repository as public; visibility is not
changed by this update.


24/10/2026 — offline recorder access check: user-space libsepol against the
actual compiled M2 policy returns `requested permission append denied by
te-rule` for kernel -> unlabeled:file, and likewise denies write. Recovery
reports both saved probe11/12 CACHE files with an unlabeled xattr. Runtime
monitor SID/enforcement/inode labels remain unconfirmed; this is a recorder
visibility hypothesis, not a captured AVC or proven bootloop cause. M1 DS-ACK
has no pmsg node, unlike recovery. No live policy load, edit, enforcement
change, device write or reboot was performed. See
`diagnostics/offline-recorder-policy.md` and selected query evidence.
