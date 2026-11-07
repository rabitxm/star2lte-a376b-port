# Ma trận patch và thay đổi

Chưa áp patch ROM nào lên A376B. `patch-inventory.tsv` liệt kê 154 file .patch
với hash và NOT_TESTED; `module-inventory.tsv` liệt kê 22 module patch cấp đầu.
Các nested final stages cũng cần audit; không coi số module cấp đầu là toàn bộ
những thay đổi động trong customize.sh.

| ID | Lý do | File ảnh hưởng | Kiểm chứng | Rollback/trạng thái |
| --- | --- | --- | --- | --- |
| M4-KM4-EFS-001 | RC software KM4 chứa lệnh `mkdir`/`restorecon`/`chown`/`chmod` vào `/efs/DAK`, trái quy tắc EFS của dự án; backend hardware chưa được khôi phục | `unica/patches/exynos9810_device_stack/keymaster4/vendor/etc/init/android.hardware.keymaster@4.0-service.rc`, `exynos9810_boot_restore/customize.sh`, `scripts/internal/verify_m4_runtime_image.py` | Final verifier thật dương/âm; ext4 mẫu + debugfs dương/âm; metadata contract; `bash -n`, `py_compile`, `git diff --check` | Đảo ba sửa trong fork; source-only, chưa build/flash/boot, không nhận là Integrity fix. [Chi tiết](m4-g965f-keymaster-tee-work-20261101.md) |
| ROM-146 | Attempt17 gặp hàm BZI6 `isSupportStrongboxDeviceID()Z` vốn đã trả false, nên `SMALI_PATCH return false` thất bại khi không tạo diff | `unica/patches/product_feature/customize.sh`, `a376b_android17_strongbox.sh` | Class SHA256 dương; class đổi, sai model và thiếu class âm; `bash -n`; replay 136 patch; attempt18 xác nhận log no-op | Patch0146 `83d9bfe17ec14d5e8b6184d7d209ef0305ede864f6e422d5fc0e39b6ba1b8927`; scoped M4/G965F/BZI6; no-op verified |
| ROM-147 | Attempt18 xác nhận BZI6 Midas config trỏ model moire ở đường dẫn cũ, trong khi file hash-matched đã chuyển sang `VslMesDetector` | `unica/patches/saiv/customize.sh`, `a376b_android17_moire.sh` | Config/model hash pins; stage + metadata fixture; changed config/model và sai model âm; `bash -n`; replay 137 patch; attempt19 sẽ kiểm ảnh cuối | Patch0147 `e92a711ba193a9a9aa945c3a11ffa06b878d02b58afca035a8c349314e75e68c`; scoped M4/G965F/BZI6; image inclusion pending |
| ROM-148 | Attempt19 gặp PhotoEditor BZI6 có hai cặp HumanSeg/BGPclassifier mang tên phiên bản, không có file legacy `hs_segmenter.info`/`.tflite` | `unica/patches/saiv/customize.sh`, `a376b_android17_hs_segmenter.sh` | Exact four-file set, SHA and info model paths; staged directory fixture; changed model/wrong model negatives; `bash -n`; replay 138 patch; attempt20 kiểm workdir/image | Patch0148 `810f22ea278fe2fc24b48c6597be187467dfd0f7f318e66d40d707d1b0679b10`; scoped M4/G965F/BZI6; final integration pending |
| ROM-149 | Attempt20 báo thiếu model legacy `smartcropping_2.0`; BZI6 dùng model `system/saiv/imageCropper` và star2lte đã có SCE model trong vendor baseline | `unica/patches/saiv/customize.sh`, `a376b_android17_image_cropper.sh` | Pin donor features/model/library, exact legacy target and vendor hashes; stage/retention fixture; wrong-model/changed-donor/changed-vendor negatives; `bash -n`; replay139 | Patch0149 `7c7a8e940412b7c4ddc8a76ba2bc141ea767a07fd9ce6be51c285a7ebd722307`; scoped M4/G965F/BZI6; attempt21 integration pending |
| DOC-001 | Chốt Hybrid Vendor + DS-ACK prebuilt theo yêu cầu mới | AGENTS.md, docs/PLAN.md, README.md | Đối chiếu chỉ thị người dùng; không thay fork | Hoàn tác tài liệu; DONE |
| DOC-002 | Tránh commit firmware/backup và nhầm baseline | .gitignore, các lock JSON, docs/*, STATUS.md, logs/*/recovery-baseline.txt | Kiểm JSON, ignored paths, audit link/file/log | Hoàn tác file mới; DONE sau validation |
| ROM-001 | Tách nguồn blob/CSC N khỏi mặc định F | target/star2lte/config.sh và finalization/boot contract cần thiết theo repo-map | Provenance/hash blob N, ABI/VINTF, output đúng model, radio/IMS log | Đã thêm override nguồn và model guard; chưa đổi blob/CSC/boot contract N, cần package/inventory/log |
| ROM-002 | Thêm A376B song song S22 | unica/configs và selector tại gen_config_file sau khi chốt thiết kế | Props/SDK/One UI/APEX từ firmware, ma trận smali riêng; M1/M2 phải qua gate | Chưa sửa; không ghi bừa SOURCE_* |
| ROM-003 | Chọn DS-ACK nguyên bản duhansysl cho ext4 | Hook EXYNOS9810_KERNEL_ZIP / EXPECTED_KERNEL_SHA256 đã có; runtime/verifier liên quan | Hash asset, CRC, dựng đúng kernel+DTB F/N, boot-size, DTB/fstab, log | Gói đã tải/CRC+hash PASS; chưa đóng gói |
| HOST-001 | CMake thiếu escape dấu ngoặc ở đường dẫn workspace, log `/bin/sh: 1: Syntax error: "(" unexpected` | external/make.sh, external/patches/android-tools/0001-cmake-quote-mkbootimg-symlink.patch; file con vendor/CMakeLists.mkbootimg.txt | bash -n, áp/check patch idempotent, build android-tools trong đúng đường dẫn gốc | Native android-tools đã build PASS; rollback outer patch và nested patch |
| HOST-002 | URLDecoder đổi dấu + trong Galaxy S9+ thành space khiến test fixture không tồn tại | external/make.sh, external/patches/apktool/0002-fix-file-resource-uri.patch; BaseTest.java của submodule | Gradle build shadowJar có tests; apktool-lib 185 tests/0 fail/0 skip; --check-tools=0 | PASS; rollback outer patch và nested patch |
| HOST-003 | curl không có User-Agent của client bị HTTP403, samloader cùng URL lại thành công | scripts/utils/firmware_utils.sh | GET_LATEST_FIRMWARE trả đúng S22 build, Samsung XML o=16; đã bắt đầu download qua pipeline | PASS metadata; download chưa đồng nghĩa extract/boot |
| ROM-004 | Người dùng chọn gói duhansysl không KernelSU; không được ghép runtime root mặc định hoặc nhận nhầm N trong lượt thử F | platform config, gen_config_file, target/star2lte/config.sh/profiles, finalization stage, release-default verifier | Profile/default/negative-case config PASS; runtime guard và actual cleanup function/metadata/cache PASS trên fixture | Chuẩn bị xong; full ROM/package/boot NOT_TESTED; rollback patch 0003 và tạo lại output đúng profile |
| HOST-004 | Binary f2fs_unpack trong commit đã pin segfault ngay cả --help (exit 139), làm extract S22 dừng ở dòng 106 | external/make.sh, scripts/extract_fw.sh; dùng nguồn f2fs_unpack_src sẵn có, binary sinh trong out/tools | Go build PASS; CLI/invalid-image guard PASS; extract S22 exit 0; metadata phủ đủ năm partition, CRC 524 APK/JAR/APEX PASS; replay bốn patch PASS | Patch 0004; rollback patch và tạo lại output bằng extractor đã xác minh; không thay binary tracked hoặc sửa kernel |
| HOST-005 | m1-build-attempt1.txt:28 báo cp cannot stat file rootfs tạm; chuỗi lệnh debugfs chưa quote đường dẫn workspace | unica/patches/exynos9810_device_stack/customize.sh, hàm COPY_ROOTFS_FILE | Năm file rootfs thực trùng byte/hash với debugfs cat trong path có space/plus/ngoặc; metadata và optional missing case PASS; replay năm patch PASS | Patch 0005; build lại work_dir sinh từ lượt thất bại; không sửa kernel/blob |
| HOST-006 | m1-build-attempt2.txt:58 báo Work dir needs to be set up; GET_PROP cat unquoted paths nên framework tag rỗng dù build.prop có giá trị | scripts/utils/common_utils.sh, hàm GET_PROP | Tám case gồm explicit path/partition/precedence/fallback/missing và actual M1 tag PASS; replay sáu patch PASS | Patch 0006; tạo lại work_dir để bỏ property rỗng do reader cũ; không sửa donor/kernel |
| HOST-007 | attempt3 có xxd No such file cho libbluetooth_jni.so và dừng ở đọc boot legacy; legacy-unpack-error.txt ghi UnicodeDecodeError, EVAL che lỗi dưới set -e | common_utils.sh/EVAL, bluetooth/customize.sh, legacy/customize.sh | EVAL lỗi hiển thị dưới errexit; actual legacy payload khớp byte header, invalid/truncated bị chặn; Bluetooth ELF/hash/metadata từ actual block PASS; boot gốc và DS-ACK hash không đổi; replay bảy patch PASS | Patch 0007; chỉ trích/kiểm dữ liệu có sẵn, tạo lại work_dir; không build/sửa/version-forge kernel |
| HOST-008 | m1-build-attempt4.txt:2695 thiếu tên llvm-readelf trong PATH; LLVM21 đã được cài với tên versioned | scripts/utils/build_utils.sh | Tìm binary qua clang --print-prog-name, source dependency với PATH tối thiểu PASS; verifier ABI native face thực PASS | Patch 0008; reverse và đặt LLVM bin vào PATH thủ công; không bỏ check hoặc cài thêm gói |
| ROM-005 | Module dsack_star2lte_kernel cuối pipeline chép BOOT KernelSU hardcoded, trái profile May gốc không KernelSU | unica/mods/dsack_star2lte_kernel/customize.sh, target/star2lte/profiles/m1-g965f-no-ksu.sh | Nhánh false chép đúng hash template kernel-size0 sẵn có của platform installer; nhánh true giữ hash cũ; symlink/invalid flag bị chặn; cpio16 file không có root-runtime names | Patch 0009; không build/sửa payload kernel; platform installer ghép nguyên ZIP May đã pin. Chưa nghiệm thu ZIP/boot cuối |
| HOST-010 | m1-build-attempt5.txt:2844 chặn package do hash khác; audit tái tạo chính xác hash bằng việc thêm một Python cache do verifier sinh sau modules | scripts/make_rom.sh, GET_WORK_DIR_HASH | Bảy case: cache không đổi hash; source Python, binary asset, Face source, recipe và filesystem vẫn làm đổi hash. Exact real-hash reproduction PASS | Patch 0010 chỉ prune __pycache__, không bỏ guard. Dùng --package-staged chính thức để kiểm/đóng output đã compile; không sửa marker thủ công |
| HOST-011 | m1-build-attempt6.txt: 7z archive path chưa quote, `syntax error near unexpected token (` trong build_full_ota_zip.sh:301 | scripts/internal/build_full_ota_zip.sh, hai lệnh 7z | bash -n; chạy hai lệnh thật trong path có cách/+/ngoặc, đủ member và CRC PASS; package lại | Patch0011, reverse nếu cần; không đổi payload |
| INSTALL-001 | PRINT_ASSERTIONS model branch OR codename star2lte nên G965N có thể vượt metadata F-only | scripts/utils/install_utils.sh | F/FDS/FD accept; N/unknown reject dù shared codename; N-list reject F; target khác giữ fallback. Audit ZIP assert trước prepare_layout | Patch0012, chỉ star2lte khi có model list; không nới assert để thử N |

Không đổi sang kernel tự build hoặc sửa binary để vượt verifier. Không tắt
kiểm tra hash/model/layout chỉ để đóng ZIP. Mọi thay đổi sau này thêm lý do,
bằng chứng, file, cách kiểm chứng và rollback vào ma trận này.

Final M1 package8 exit0; CRC/hash/layout/model-order/CMS audit PASS trong
m1-final-artifact-audit.json và m1-ota-signature-audit.json. Chưa có log boot,
không coi kiểm tĩnh là nghiệm thu runtime hoặc thay đổi kernel cần thiết.

| RECOVERY-TOOLS | Live ls báo /system/bin/mkdir,unzip,chmod không có; /sbin equivalents tồn tại | Patch0013 install-start/end và platform customize | bash-n, live capabilities, final updater paths/guards; không flash ZIP cũ |
| EFS-RO | ro mount có thể replay journal; yêu cầu không ghi EFS | Patch0014 vendor_bootfix EFS mount ro,noload | sh-n và live chỉ đọc hai SIM keys; mount được tháo sau đọc |
| GPT-ABI | sg không link trên TWRP9: CANNOT LINK EXECUTABLE missing libc++ stringstream symbol | Patch0015 M1 profile + existing tool override/hash guard + final verifier | Static ARM64 source/hash pin; missing/hash-negative; real4K GPT readonly; existing script RAM fixture target sizes/prefix1–17 PASS |

Patch0013–0015 giữ nguyên system/vendor/kernel; chỉ công cụ và mã installer.
Nguồn prebuilt sg: https://github.com/evdenis/disk/tree/903e56c2d61e276c9d500bbda7c634842993613c.
Đây là provenance binary; repository không cung cấp recipe tái dựng binary đó.
Không cài module Magisk; chỉ dùng utility sg đã kiểm độc lập.

| RECOVERY-SIZE | TWRP wraps full ZIP size by2^32, rejects before updater | Patch0016 build_flashable_zip.sh + split_legacy_sideload.py | Source full verifier; 8 outputZIP32/CRC/CMS; ordered ranges, raw chunk SHA round-trip; actual updater RAM partial ranges and wrong-order/no-write test PASS | Optional mode only, normal build unchanged; intermediates noBOOT/noUSERDATAformat, final retains original tail |


## ADB-001 / Patch0017 — chính sách lâu dài do user yêu cầu

Lý do: mặc định Auto Blocker OFF / USB debugging ON và ADB USB không hỏi key cho
mọi PC, đồng thời công tắc/description phải hoạt động thật cho mọi bản F/N sau.
File: finalization/customize.sh, helper finalization/adb_defaults.py và
scripts/internal/verify_exynos9810_release_defaults.py; rule ở AGENTS.md/PLAN.
Kiểm: SQLite defaults/retained choice, source idempotence/unknown donor reject,
APK/JAR rebuild gốc, compiled DEX/property verifier + ba negative fixtures,
release checks +15715 classes, replay17patch/28file PASS. Runtime candidate
chưa thử; không suy USB dứt lỗi từ default config. Rollback reverse0017 và rebuild.
Chi tiết: [adb-defaults.md](adb-defaults.md). Không đổi kernel/Hybrid Vendor/RIL.


## INSTALL-FRESH / Patch0018 — không che lỗi format USERDATA

Lý do: flash-03/fresh-format/installer-format-attempt.txt ghi
`USERDATA is mounted; will not make a filesystem here!`, trong khi recovery
kết thúc `Updater process ended with RC=0`. TWRP Format Data lần đầu cũng báo
`Unable to wipe Data.` và UUID giữ nguyên. /proc/mounts có nhiều /sdcard alias
cùng USERDATA; script cũ chỉ tháo /data và đánh dấu format data optional.
File: platform/exynos9810/installer/repartition/format_partitions.sh và
scripts/internal/verify_exynos9810_flashable.sh. Helper mới resolve source,
tháo mọi alias/stacked mount bằng umount thường, kiểm không còn mount; lỗi busy
hoặc mke2fs phải dừng khi format data. Không lazy/force-unmount để bỏ kiểm tra.
Verifier đọc script thực trong ZIP và từ chối kiểu optional cũ.
Kiểm: sh/bash syntax, fixtures stacked/canonical aliases, busy-before-mkfs,
mkfs failure propagation, actual archive guard accept/reject PASS;
replay18patch/29file byte-identical PASS. Evidence flash-03/format-*.json,
patch18-replay.json. Rollback reverse0018, nhưng không dùng RC0/ext4 TYPE làm
bằng chứng data mới. Chưa đóng/flash ZIP chứa0018. Flash-03 giữ nguyên ZIP0017,
đã tháo aliases rồi dùng TWRP Format Data riêng; UUID mới + absence proof PASS.

## RELEASE-RETENTION / Patch0019 — giữ ROM M1/M2/M4/M5

Lý do: user yêu cầu giữ mọi gói hoàn chỉnh, không ghi đè khi đổi/build lại mốc.
Source cũ make_rom.sh xóa stale target-files và `find ... -delete` full ZIP;
signer/splitter ghi thẳng destination. File ảnh hưởng: scripts/make_rom.sh,
scripts/build_flashable_zip.sh, scripts/internal/create_target_files_zip.sh,
scripts/internal/build_full_ota_zip.sh, scripts/internal/split_legacy_sideload.py,
scripts/utils/build_utils.sh, target/star2lte/profiles/m1-g965f-no-ksu.sh.
Thay đổi: mỗi run thư mục mới; staging + exclusive publish; từ chối existing/
symlink; checksum/config/marker và ZIP chỉ đọc; legacy không thay output cũ.

Kiểm chứng: shell/Python syntax, bảy behavioral fixtures PASS (bao gồm hai run
M2 và concurrent publish), actual7z excludes itself, legacy collision trước
work; replay19patch/30file byte-identical PASS. Evidence ở
logs/20261021-release-retention/{retention-fixtures,patch19-replay}.json.
M1 archive10ZIP SHA/size PASS, độc lập byte/hardlink với root out; ROM chứa17patch.
Chưa build M2 vì chưa có donor16. Rollback reverse0019, giữ archive M1 và tránh
chạy wildcard-delete cũ trên output cần giữ. Không đổi kernel/blob/thiết bị.
Chi tiết docs/release-retention.md, docs/m2-build.md.

## INPUT-LOCAL / Patch0020 — đầu vào Odin cục bộ

Lý do: A376B16 đã được người dùng tải, parser cũ vẫn đòi IMEI/SN khi trích.
File: scripts/utils/firmware_utils.sh, scripts/download_fw.sh. Descriptor LOCAL
xóa định danh còn sót từ firmware trước; downloader chỉ dùng import có marker,
không gọi FUS và không xóa import kể cả --force. Marker thiếu thì dừng.
Kiểm: bash syntax, actual parser/downloader với network spy PASS ba case;
replay toàn bộ patch PASS. Evidence local-import-fixtures.json, patch22-replay.json.
Rollback reverse0020 và không dùng descriptor LOCAL. ZIP nguồn không đổi.

## ADB-A376B / Patch0021 — rà soát lifecycle mới trước build

Lý do: adb-donor-preflight.json ghi `Unknown Samsung AdbService lifecycle;
review donor before building`. A376B AZH4 có .locals6, receiver AdbService và
state register p1; M1 dùng v0. File: finalization/adb_defaults.py.
Chỉ nhận hai layout đã đọc; seed SQLite và UI rule giữ nguyên. Compiled verifier
kiểm read/move-result/state cùng register; layout khác vẫn dừng.
Kiểm actual M1/A376B source/idempotence, unknown locals/anchor rejection,
assemble class fixtures và compiled checks PASS; pristine compiled A376B bị
reject đúng. Fixtures dùng DEX039/API32 cho instruction đã đọc, không phải
full services.jar rebuild/SDK hạ cấp hoặc runtime. Evidence adb-reviewed-fixtures.
Rollback reverse0021 và dừng donor mới ở guard cũ. Không thay ROM M1 đã lưu.

## DONOR-A376B / Patch0022 — cấu hình song song và cache riêng donor

Lý do: M2 chuyển donor nhưng giữ ESSI layout và nguồn S22. Cache cũ chỉ dựa
checkout/FS có thể tái dùng work_dir donor khác. File: gen_config_file.sh,
make_rom.sh, profiles/m1-g965f-no-ksu.sh, profile M2 mới và
unica/configs/essi_a376b_android16.sh. Selector UN1CA_DONOR_CONFIG độc lập SSI,
ROM_SOURCE_CONFIG được lưu vào config; hash gồm selector, descriptor, SDK và
.extracted version. Profile M1 reset inherited selector, M2 reuse kernel/tooling F.

Kiểm actual config/profile M1/M2, path traversal reject, inherited selector
reset, actual hash function với donor/config/SDK/version khác PASS. S22 config
SHA256 giữ nguyên 5c2835891d6dd25b836057bc7ab1a96237d2c994e3bbecc175a95a84be5c54c7.
Replay22patch/33file byte-identical PASS. Bluetooth source flags chưa xác định
và không có consumer được ghi none, guard chặn nếu recipe sau thêm consumer;
không gán giá trị S22 hoặc tuyên bố phần cứng A37 đã test. Các flag Wi-Fi có
runtime HAL detection mô tả static/default path; nghiệm thu cần log riêng.
Build M2 đầy đủ đang chạy, compiled/hardware acceptance chưa đạt từ fixtures.
Rollback reverse0022 rồi source M1; giữ archive/releases. Không đổi kernel,
Hybrid Vendor/radio binary hoặc ghi máy. Evidence donor-config-fixtures.json.

## CAMERA-A376B / Patch0023 — giữ ABI prepare của donor

Lý do: logs/20261021-m2-build/attempt1.txt:65 báo `Failed to patch method
"prepare(IIII)V"` trong MultiMicController. A376B dùng prepare(IIIIZ)V,
feature class LO1/d và tham số p5 chọn mode. Thay cả hàm bằng S22 sẽ sai ABI/
obfuscation. File: exynos9810_device_stack/customize.sh, camera_multimic.py mới.
Chỉ chèn guard mManager null vào entry của hai signature đã đọc; giữ toàn bộ
body donor, mode và feature references. Layout/initializer khác vẫn dừng.
Kiểm S22/A376B source body+ABI bảo toàn, idempotence, unknown reject, actual
smali compile/readback PASS; A376B p5 và feature ref còn đúng. Replay23patch/
34file byte-identical PASS. Evidence camera-multimic-fixtures.json.
Rollback reverse0023, dừng M2 ở old guard; không đổi kernel/blob/thiết bị.
Build sạch attempt2 đang chạy; chưa có runtime camera hoặc full ZIP M2.

Bổ sung ADB gate: services-full-rebuild.json ghi apktool_build_exit0,
compiled_lifecycle_readabletrue và read_in_state_register_p1true. Đã rebuild
FULL services.jar A376B với ADB patch, minSdk35 giữ nguyên; Apktool xuất DEX039
hợp lệ, verifier đọc được. Không chỉnh SDK/minSdk thủ công để né probe DEX041.

## CAMERA-CLAIM / Patch0024 — không quảng bá classifier không có implementation

Lý do: attempt2.txt:905 `Missing SPF patches for condition
(SOURCE_CAMERA_CONFIG_ACTION_CLASSIFIER: [], TARGET_CAMERA_CONFIG_ACTION_CLASSIFIER:
[1,0]). Aborting`. Target floating-feature kế thừa S22/legacy quảng bá optional
Single Take classifier; XML AZH4 và hai thư viện implementation đều không có.
File: unica/patches/camera/customize.sh. Chỉ donor config A376B16 + Exynos9810
+ absent source feature/native payload mới xóa riêng target claim; không đổi
SOURCE_* hoặc blob vendor. Native payload/symlink bất ngờ, donor/platform khác
vẫn dùng review guard. Optional action-classification chưa được hỗ trợ ở M2.
Kiểm actual camera block + actual FF helpers, XML copied từ donor; positive
và bốn negative donor/platform/payload/link PASS. Replay24patch/35file PASS.
Evidence camera-classifier-fixtures.json. Rollback reverse0024 và dừng M2 ở
old guard. Attempt3 clean đang chạy, chưa có full ROM/camera runtime.

## M2 camera compatibility follow-ups

- 0025: unica/patches/camera/customize.sh treats donor None/absent Gallery Pet Cluster as disabled when target is also disabled; mismatch with a target version still aborts. Reason: attempt3 line914. Verify: pet-cluster-fixtures.json, replay patch25.
- 0026: same file clears inherited legacy ARDoodle claim only for known AZH4 payload without key/assets/library; unknown inputs remain guarded. Reason: camera preflight documented in ardoodle-preflight-before.txt. Verify: ardoodle-claim-fixtures.json, replay patch26.
- 0027: same file processes stagefright ABI libraries only when present and permits a missing ABI only for A376B XXV LOCAL SDK36. Reason: attempt4 reports missing /system/system/lib/libstagefright.so. Verify: stagefright-abi-fixtures.json, bash -n, replay patch27.

- 0028: unica/patches/dvfs/customize.sh selects the reviewed A376B SDK36
  obfuscated SamsungDeviceHealthManagerService Feature class smali/o5/w.smali;
  other donors keep the upstream smali/U1/w.smali. Reason: attempt5.txt:941.
  Verify: dvfs-obfuscation-review.json, bash syntax, replay patch28.

- M2 attempt8: no new patch. The existing legacy SehLights backport is
  incompatible with AZH4 services.jar (`attempt8.txt:1104-1105`, `:1132-1133`).
  Next change must adapt the existing patch for the reviewed donor, explain
  reason/files/verification, and add an apply/replay fixture before rebuilding.

- 0030: `unica/patches/legacy/customize.sh` selects the AZH4-specific path only
  when ROM_SOURCE_CONFIG is `essi_a376b_android16`; new
  `unica/patches/legacy/lights/services.jar/0002-A376B-legacy-SehLights-backlight.patch`
  updates the backlight caller; new `patch_a376b_sehlights.py` adapts the
  existing backport body to VintfHalCache and preserves old method overloads.
  Reason: attempt8.txt:1104-1105 and :1132-1133; the old Supplier layout does
  not match AZH4. Verify: actual AZH4 decode/strict-transform/backlight apply/
  Apktool rebuild PASS in `sehlights-a376b-preflight.txt`, shell syntax,
  patch-layout and lock hash PASS. Full ROM/build, runtime brightness and HAL
  behavior remain to verify. Rollback by reverting patch0030; no kernel or
  firmware change.

- 0031: `unica/patches/legacy/customize.sh` bỏ qua riêng legacy VideoTranscoder method patch khi profile A376B AZH4 không có method đó; giữ donor implementation và áp dụng EncoderFilter/stagefright patches đã xác minh. Lý do: `attempt10.txt:1000`. File ảnh hưởng: `unica/patches/legacy/customize.sh`. Kiểm chứng: method absence và supported EncoderFilter method trên donor decode, `bash -n`, patch layout/replay, rồi clean build; rollback bằng cách revert patch0031.


- 0032: `rom/unica/patches/legacy/customize.sh` adds a strict A376B AZH4-only Stagefright patch, checks the 64-bit-only layout, and NOPs the HEVC early-exit branch identified by ELF relocation and ACodec disassembly. Other donor paths stay unchanged. Reason: attempt11.txt:1003-1005 plus AZH4 GOT relocation `0x2639a0` -> `MEDIA_MIMETYPE_VIDEO_HEVC` and branch at `0xe5654`. Verification: source hash/disassembly, unique exact signature, NOP patch decode, `bash -n`, reverse dry-run, clean build12 and output audit. Camera/OMX runtime still needs device logs. Rollback patch0032.

- 0033: `rom/unica/patches/legacy/customize.sh` selects `const v4` for GlobalPostProcMgr EncoderFilter only on `essi_a376b_android16`, retaining upstream v3 for others. Reason: attempt12 line 999+ fails expected v3; AZH4 decoded class has v4 constant at 1908. Verify strict donor source, shell syntax and clean build/Apktool assembly. Rollback patch0033.

- 0034: `rom/unica/patches/legacy/customize.sh` guards the optional vexfwk_service EncoderFilter edit only for A376B AZH4 when the source APK is absent, while keeping other donor handling strict. Reason: attempt13.txt:1007 and source inventory. Verify shell and clean build14. Rollback patch0034.

- 0035: `rom/scripts/internal/apply_modules.sh` skips only the existing vendor-mismatch UI warning patch for `essi_a376b_android16`; AZH4 changes the synthetic lambda structure and the UI patch is non-boot-critical. Other donors retain upstream patch. Verify source smali, shell syntax, exact path guard and clean build15. Rollback patch0035.

- 0036: `rom/scripts/internal/apply_modules.sh` skips the optional Bixby remap for A376B16 because AZH4 key dispatch differs (KeyGestureEvent vs expected KeyEvent). Other donor patching remains intact. Reason: attempt15.txt:1038-1044. Verify exact guard, shell syntax and clean build16. Rollback patch0036.

- 0037: `rom/unica/patches/product_feature/customize.sh` skips the HdmVendorController constructor shipping-API substitution for exact A376B16 because donor class has no constructor. Reason: attempt16.txt:1043 and class inspection. Verify shell, reverse patch and clean build17. Rollback patch0037.

- 0038: `rom/unica/patches/product_feature/customize.sh` skips the absent Knox TAProxy API substitution for A376B16 only; other product API changes remain. Reason: attempt17.txt:1045 and donor class inventory. Verify shell/reverse patch/clean build18. Rollback patch0038.

- 0039: `rom/unica/patches/product_feature/customize.sh` adapts only the A376B16 resolution patch dispatch to set CoreRune FW_DYNAMIC_RESOLUTION_CONTROL true while leaving donor VRR resolution flags false. Reason: attempt18.txt:1087-1097 and target config/floating feature. Verify final CoreRune smali/compiled APK. Rollback patch0039.

- 0040: `rom/unica/patches/product_feature/customize.sh` ports the resolution comparison to AZH4 `GameDisplayListener` constructor for exact A376B16; other donors keep upstream class patch. Reason: attempt20.txt:1090 and verified donor smali/target resolution list. Verify build22 and compiled gamemanager.jar; runtime display switching remains open. Rollback patch0040.


Continuation verification update (21/10/2026): audit patch0030 record repaired without implementation changes; all 40 source patches replay byte-identically. Patch0032 before/after disassembly confirms only instruction `0xe5654` is NOP in the isolated audit copy; this is the HEVC HDR10Plus assertion path. Patch0040 now passes the actual fork SMALI_PATCH helper and Apktool jar assembly on decoded AZH4. These are static checks; whole-ROM compiled defaults/package gates remain open. The upcoming generic Settings resolution patch fails preflight and needs a semantic port or an explicit fixed-resolution M2 decision.

- 0041: user-approved temporary fixed native resolution for M2/A376B16. Files: `rom/target/star2lte/profiles/{m1-g965f-no-ksu,m2-g965f-a376b-android16-no-ksu}.sh`, `rom/target/star2lte/config.sh`, existing device-stack customize/finalization scripts. Reason: generic resolution Settings patch is incompatible with AZH4 and user chose fixed resolution. Disable the existing target switcher feature and remove/empty the floating-feature claim in both copies; audit requires absence, using existing native panel/density baseline without boot scripts. Verify profile fixtures, full patch replay, shell syntax, clean build23/final XML/compiled UI and later hardware resolution. Revert0041 to return to resolution-port work.


Product-feature audit records0042–0048 (21/10/2026):

| Patch | Reason and affected fork files | Verification / rollback |
| --- | --- | --- |
|0042|Moved exact DEX classes and missing HDR target branch: `scripts/utils/smali_utils.sh`, `unica/patches/product_feature/customize.sh`.|Actual helper fixtures reject missing/ambiguous classes; HDR recipe preflight. Full assembly/runtime gates remain; revert0042.|
|0043|Optical to rear/capacitive: product_feature recipe and new `fingerprint/a376b_rear.py`; five exact classes in decoded framework/services/SystemUI/pinned BiometricSetting.|Enum/mapper and pinned APK provenance checked; preflight + no-partial-write rejection fixture. Runtime unlock pending; revert0043.|
|0044|AZH4 threshold/dump/VrrManager differences: product_feature recipe.|Actual HFR recipe slices pass through all edits; assembly and60Hz runtime needed; revert0044.|
|0045|Donor mDNIe stub functions: product_feature recipe and new `mdnie/a376b_hw.py`.|Upstream diff + old method hashes, dependencies and register shape checks; actual preflight PASS. Hardware/SELinux runtime needed; revert0045.|
|0046|Already-false StrongBox no-op: `scripts/utils/smali_utils.sh`.|Accept only byte-identical generated constant method after normal validation, A376B16 only; actual helper logs success; revert0046.|
|0047|S22 Wi-Fi backoff diff differs: product_feature recipe.|Actual donor placeholder emptied; parser is skipped by its existing isEmpty branch; native Wi-Fi unchanged, runtime pending; revert0047.|
|0048|Late finalization contradicted Face omission: finalization recipe, make_rom.sh, M1 profile, release verifier, new verify_face_omission.py.|Fixtures reject restored HAL/init and preserve radio; staged preflight + profile reset PASS; mandatory compiled ADB remains. Revert0048.|

Each patch has reason/files/verification in its header and SHA in sources.lock.json. Replay48/44 byte-identical; no package or runtime success implied.


- 0049: user-selected temporary M2 donor Wi-Fi UI. Files: both star2lte profiles and product_feature/customize.sh. Reason: Wifi80211AX Settings/SystemUI diffs fail AZH4 even after exact descriptor relocation; user chose donor UI for boot test. Verification: real branch fixtures selected/strict/invalid-scope, profile reset, shell syntax and replay49/44 PASS; clean attempt24. Rollback0049; native Wi-Fi, kernel and ADB rules unchanged.


- 0050: repair local0044 METHOD leak into replaceall. Files: product_feature/customize.sh, scripts/utils/smali_utils.sh. Reason: attempt24 SemInputFeatures failure. Verify actual helper field-only/global METHOD and missing-method fixtures, shell/replay; revert0050.
- 0051: port AZH4 target APE/personalization disablement. Files: product_feature recipe + wifi/a376b_backend.py. Reason: product-tail-after-input.txt:94-95. Exact region/method/enum checks, no external labels, fixture no partial writes, actual preflight4classes PASS; assembly/full/runtime gates remain; revert0051.
- 0052: port AZH4 SoftAP driver/country/power/sharing-lite paths. Files: product_feature recipe + wifi/a376b_softap.py. Reason: actual diagnostic AX/country failures plus independent power/sharing preflight failures. Preserve public HAL query, permission/Binder cleanup, per-command SQE isolation and carrier exclusions; exact hashes/fixtures/independent jar assembly/actual tail PASS. Clean build and runtime required; revert0052.
- 0053: adapt A376B moire model relocation and named hs_segmenter pairs. File: saiv/customize.sh. Reason: remaining-modules-preflight.txt:5 and saiv-preflight.txt:12. Model SHA match, existing Midas config path, donor model_path validation, SAIV preflight and explicit metadata; no foreign model binary added. Runtime pending; revert0053.

- 0056: `scripts/internal/apply_modules.sh` keeps AZH4's PackageBlockListPolicy/lifecycle intact for exact A376B Android 16 M2 because legacy patch changes removed policy fields and observer; attempt preflight lines75,136. Verification: dispatcher log in attempt27 and full clean module staging; blocklist customization remains omitted for this donor. Revert0056.
- 0057–0058: `unica/mods/choidujour/customize.sh`, `scripts/internal/apply_modules.sh` skip the legacy UN1CA Updates suggestion hook because AZH4 lacks `Settings$FOTASuggestionActivity.smali`; attempt25/26 line1230. Verification: both module hook and duplicate Smali dispatcher path now log the exact scoped skip in attempt27; other donor path remains unchanged. Revert0057/0058.
- 0059: `scripts/internal/apply_modules.sh` keeps AZH4's donor real-time network speed Settings implementation because three legacy classes are absent; attempt27 lines1237–1241. Verification: exact A376B dispatcher guard, shell syntax and replay59; clean build28 running. Revert0059.
- 0060: `scripts/internal/apply_modules.sh` keeps AZH4's donor power-off lock Settings implementation because CSC's legacy `PowerOffLockPreferenceController.smali` is absent; attempt28.txt:1238. Verification: exact A376B CSC patch guard, shell syntax, replay60 and clean attempt29. Revert0060.
- 0061: `scripts/internal/apply_modules.sh` keeps AZH4 donor Secure Folder Settings behavior because the CSC quick-switch patch targets absent classes; attempt29.txt:1239,1241. Verification: donor-scoped patch guard, shell syntax, replay61 and clean attempt30. Revert0061.
- 0062: `scripts/internal/apply_modules.sh` retains AZH4 donor Smart View tile initialization because CSC's QpRune context differs; attempt30.txt:1261. Verification: exact SystemUI patch/profile guard, syntax, replay62 and clean attempt31. Revert0062.
- 0064: `unica/mods/deknox/customize.sh` skips only Knox TAProxy shipping-API substitution when absent in AZH4; attempt32 deknox lines103–108. Verification: exact M2/file absence guard, shell syntax, replay64 and clean attempt33. Revert0064.
- 0065: `scripts/internal/apply_modules.sh` skips the whole optional Nuke Knox module for AZH4 M2, preserving donor Knox as one coherent stack; attempt33.txt:1437–1480 shows multiple stale framework hunks. Verification: exact module/profile guard, shell syntax, replay65 and clean attempt34. Revert0065.
- 0066: `scripts/internal/apply_modules.sh` skips the optional Hide My Applist module on AZH4 M2 after its PackageManager/ActivityStarter patches fail; attempt34.txt:1281–1295. Verification: exact module/profile guard, syntax, replay66 and clean attempt35. Donor behavior retained; revert0066 to port.
- 0067: `scripts/internal/apply_modules.sh` skips optional KeyboxImitationHooks for A376B16 M2 because AZH4 lacks three keystore2/BouncyCastle classes; attempt35.txt:1256–1260. Preserve donor attestation. Verification: module/profile guard, shell, replay67 and clean attempt36. Revert0067 to port.
- 0068: `scripts/internal/apply_modules.sh` skips the optional KnoxPatch spoof/bypass module for AZH4 M2 because attempt36 failed across multiple integrity/attestation call sites (`attempt36.txt:1259–1260,1265–1310,1311–1348,1349–1388`). Verification: exact donor/module guard, `bash -n`, replay68, clean attempt37. AZH4 donor Knox/attestation behavior remains; spoof/bypass feature omitted. Revert0068 to port.
- 0069: `unica/mods/paradigm/customize.sh` skips only the optional adaptive colour-tone feature for exact AZH4 M2 after its display-service patches failed (`attempt37.txt:1261–1340`). Verification: donor-scoped feature guard, shell syntax, replay69, clean attempt38. Donor display behavior retained; adaptive colour-tone add-on omitted. Revert0069 to port.
- 0070: `scripts/internal/apply_modules.sh` skips only Paradigm's standalone EnvironmentAdaptiveDisplay APK patches for AZH4 M2 because attempt39 still decoded the APK after feature-level omission (`attempt39.txt:1333`). Verification: exact donor/path guard, shell syntax, replay70, clean attempt40. Revert0070 to port.
- 0071: `unica/mods/settings/customize.sh`, `a376b_search.py` adapt AZH4's inlined search factory after `attempt41.txt:1489` cannot find the legacy Base constructor. Preserve all 211 donor registrations, append four UN1CA providers through a static utility; four input class hashes checked before writes. Fixture rejects unknown donor without partial writes; factory/helper DEX assembly PASS; actual Settings tail PASS. Runtime search pending.
- 0072: `scripts/internal/apply_modules.sh`, `unica/mods/settings/a376b/0001-Allow-more-than-five-apps.patch` port the two five-app selection gates after `settings-tail71.txt:41`; preserve donor saving, unlink and footer paths. Actual module dispatch/DEX assembly PASS. Six-app runtime selection pending.
- 0073: `unica/mods/setupwizard/customize.sh` ports the reviewed navigation step from S2/f to I2/f.d and Activity.f to Activity.e, with method SHA checks. `postsettings72.txt:2` was the missing class; postsettings73 shows both changes PASS, two-class DEX assembly PASS. Runtime setup navigation pending.
- 0074: `scripts/internal/apply_modules.sh` keeps the donor disclaimer for M2 after `postsettings73.txt:53,96` fail custom disclaimer resource/Activity hunks. Exact source/path guard; shell/replay PASS. Custom branding omitted.
- 0075: `scripts/internal/apply_modules.sh`, `unica/mods/settings/customize.sh` preserve donor behavior for five incompatible optional features (Google Photos spoof, secure windows bypass, downgrade, target SDK bypass, capture detection bypass) and remove their five Extra Settings controls. Evidence: `settings-patches-preflight72.json`, `settings-smali72.txt:18–19`; actual `settings-smali75.txt` and XML parse PASS. Supported hooks remain. Compiled output audit pending; temporary M2 scope can be reverted.
- 0076: `scripts/internal/apply_modules.sh` preserves donor QP text scrolling and sensor-toggle confirmation for M2 after QP layout failures (`postsetup74.txt`) and missing old SensorPrivacyToggleTile (`tweaks-preflight74.json`). Actual SystemUI dispatcher continuation PASS in postsetup76; stepX and previously applied haptics retained. Shell/replay PASS.
- 0077: existing vendor-restore module, A376B donor config, new native32 adapter/lock, existing release-default verifier. Reason: finalstages76.txt:90 and audit show app64-only A37 lacks linker32 while vendor audio/camera/OMX are ELF32. User confirmed app64-only; add only pinned S22 native32 runtime/platform files while preserving all A37 runtime files/64-bit exports, Hybrid Vendor and hardware blobs. Verification:164-node final static namespace candidate audit (47 copied files; fork radio common-V2 wrapper retained); original A37 byte/metadata comparison; AVB+v3+4KiB alignment; independent output hash match; wrong-key rejection with49 stage files unchanged; actual finalstages77 ABI+graphics PASS; replay77. Full build, compiled ADB and runtime remain open. Keys stay ignored; no kernel change.

- 0078: existing boot-restore eSIM cleanup and release verifier remove/check the donor euicc.mep permission XML. Reason: finalstages77.txt:353 rejected its declaration on hardware without eSIM. Verification: final-tail78b eSIM gate and final-last80 package gate; full clean42 pending. Revert0078 to restore donor declaration.
- 0079: existing boot-restore SecureFolder helper handles the AZH4 namespace move using exact method hash and reviewed return-false body for unsupported DeX-PC. Reason: final-tail78b missing legacy class. Verification: securefolder79 helper and DEX assembly PASS; full clean42/runtime pending.
- 0080: existing boot-restore final fixes preserve A37 ObjectCapture/IPService/Rubin behavior rather than apply S22-specific workarounds; package inventory expects RubinVersion38 for this donor. Reason: final-tail78b shows changed implementations without A37 runtime evidence for those workarounds. Verification: final-last80 guards/package gate/native verifier and final Infinity/BOOT modules exit0; clean42/compiled/runtime pending.

- 0081: existing finalization Photo Editor inventory selects PhotoEditor_Full only for A376B; other donors retain PhotoEditor_AIFull. Reason: attempt42.txt:2393 uses the S22 path although donor/stage APK exists under Full. Verify: donor/stage SHA equality91a458916c289f9218e1791c0d934cb40de1f538a72ab6f402c97d77008d412e; actual full inventory helper PASS; shell/replay81 PASS; clean/runtime pending. Revert0081 to restore old path selection.

- 0082: scripts/internal/verify_exynos9810_boot_contract.sh aligns exact A376B/M2/star2lte Face-disabled profile with the already selected omission and donor Knox retention. Reason: boot-contract-preflight43.txt requires removed face-default-sec.rc; old SDP absence rule is S22-specific while0065 retains AZH4 Knox. Invoke existing Face omission verifier, retain other donors' Face file/SDP absence checks, positively require AZH4 SDP class. Verify syntax, replay82/62files and three wrong-profile rejection fixtures PASS; final compiled gate pending. No APK/JAR payload or kernel changes.

- 0083: Settings customize, device-stack/finalization keylayout writers, boot-contract and release-default verifiers. Reason: attempt43.txt:2568 missing intentionally omitted Bixby hook; user chose hidden remap/original keylayout. Remove selector XML for exact A376B M2, reject unexpected app selector, restore703 WINK, retain other donor behavior. Compiled release gate checks seven omitted keys absent, supported games-spoof control present, all703 WINK and original vendor layout SHA f5cacb4896c414de1c8497d00a7d8bc8287c80bbb9b55e3c09d6473587a15dd8. Replay83/62files PASS; old compiled Settings correctly rejected in bixby83-old-compiled-rejection.txt. Existing fork resume44 rebuild/release gates pending. Revert0083 to port remap; no ADB/radio/kernel change.

- 0084: `unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh` removes only the stale `otapreopt_slot -> vendor_apex_file` allow from the merged AZH4 CIL, with exact donor guard and count/absence checks. Reason: `attempt45.txt` failed `secilc` resolving undeclared `otapreopt_slot` at `plat_sepolicy.cil:78269`; the A37 policy does not declare that S22-only domain. Verification: actual helper and Exynos9810 SELinux verifier PASS; `secilc` full staged policy PASS in `selinux84-compile.txt`; 84-patch replay PASS; all release gates and retained M2 package pass in attempt46. This narrows one obsolete allow and preserves enforcing policy; no kernel/radio change.


- 0085: `rom/scripts/internal/split_legacy_sideload.py` moves extraction of the final `legacy_next.prop` state marker before the final-stage installer formats USERDATA. Reason: the final stage may be loaded from `/sdcard`; formatting USERDATA first removes the source package and makes a later `package_extract_file` fail. Verification: generated stage-08 updater script contains one state extraction before `Formatting cache, omr, preload and data...`; Python syntax and generated classic-ZIP CRC/signature checks PASS; actual G965F stage08 retry2 completed with RC=0 and fresh USERDATA format. Scope is the optional legacy sideload helper; full release ZIP/hash unchanged.


Patch0086 / early diagnostic prepared on PC: new optional offline helper `rom/scripts/internal/prepare_star2lte_boot_diagnostic.py`, no automatic build activation. Only legacy BOOT command-line field changed; pinned kernel/ramdisk/DTB/tail unchanged. Diagnostic SHA `a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9`, rollback SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Wrong-input/no-output, output-collision and syntax checks PASS. No image flashed. Review: `docs/m2-early-diagnostic-review.md`; actual pstore/fatal behavior needs device evidence. Await user authorization for this distinct BOOT diagnostic plus rollback, G965F only.


Patch0087 / explicit PID1 pmsg probe02: optional raw-syscall AArch64 monitor in `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`, prepare helper extended for cpio. Original init preserved as /init.real; probe keeps PID1 then execs original with argv/env, child copies selected kmsg errors to pmsg only, no FS mounts/partition access. TWRP fork/wait/exec/env self-test PASS, non-PID1 refusal13. Existing DS-ACK MagiskBoot repacks RAM files. Independent CPIO audit preserves all other data/metadata; kernel/DTB/cmdline unchanged. BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926. Diagnostic only: timing/SELinux can affect capture; compare results cautiously. Plan: one verified G965F boot under current project authorization, capture then rollback baseline cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d. Evidence `logs/20261022-m2-early-diag/pid1-probe-02/`.


Probe02 completed: verified G965F returned to TWRP; probe BOOT hash matched. Captured pstore (empty), last_kmsg/sec_log/recovery/reset reason under `logs/20261022-m2-early-diag/pid1-probe-02/postboot/`. No M2-PID1 marker or M2 fatal recovered; absence does not distinguish pre-init failure from unavailable pmsg/sysfs/dev nodes or recorder failure. Restored baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d with entire-partition readback PASS, no reboot. Phone TWRP.

Patch0088 is an unflashed follow-up prototype: isolated monitor mount namespace for only tmpfs/sysfs so original init mount table stays untouched. RAM preflight FAILED `NAMESPACE stage=4 errno=016` (sysfs mount EBUSY), exit20; parent mount table SHA unchanged. Do not flash this prototype; no diagnostic image03 created/installed. Evidence `logs/20261022-m2-early-diag/pid1-probe-03/namespace-selftest.txt`. Root cause still unresolved; preserve pinned kernel and baseline BOOT.

- 0089: optional PID1 probe source. Reason: inherited sysfs made probe03 stop at `NAMESPACE stage=4 errno=016`; accepting EBUSY after verifying the real pmsg sysfs path permits both inherited and fresh sysfs mounts. Verification: TWRP warm/cold namespace tests, fork/wait/exec/env, unchanged parent mount hash, and pmsg self-test marker at `pmsg-ramoops-0:59` after a recovery reboot. Probe04 BOOT full readback passed; M2 bootloop yielded no runtime marker. Baseline BOOT restored. Normal builds/kernel unaffected.

- 0090: optional PID1 probe source. Reason: pmsg evidence was absent after repeated M2 bootloops; a dedicated CACHE file can retain an early marker across reboots. Child-private mount reads CACHE dev_t from sysfs; only `/cache/m2-early-boot-probe05.log` is written and fsynced. Verification: TWRP warm/cold namespace, fork/wait/exec/env, unchanged parent mount hash, CACHE self-test survived a recovery reboot; BOOT component and full readback hashes passed. After M2 bootloop the file still held only the TWRP test marker, so boot cause remains unknown. Baseline BOOT restored. No G965N/kernel change.

- 0091: hiệu chuẩn recorder CACHE tùy chọn trên M1 đã boot. Lý do: probe05 self-test trên Android báo `NAMESPACE stage=4 errno=002` vì system kernel không có pmsg sysfs; nhánh `EBUSY` có thể ghi nhầm tmpfs riêng. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0091-calibrate-m2-cache-probe-on-m1.patch`. Kiểm chứng: AArch64 static link, M1 runtime namespace/CACHE PASS, M1 boot hoàn tất với probe06 và marker `M2-PID1-PROBE-v4: persistent cache monitor ready` đọc từ `/cache`; audit BOOT giữ kernel/DTB/cmdline/footer. M2 runtime còn chờ log TWRP. Không tích hợp probe vào build ROM bình thường, không sửa kernel.

- 0092: probe07 ghi kmsg không lọc và heartbeat định kỳ vào CACHE để so sánh M1/M2. Lý do: probe06 chỉ lọc một số dòng, không cho thấy ranh giới first-stage init của M2. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0092-unfiltered-kmsg-heartbeat-probe.patch`. Kiểm chứng: AArch64 static build, BOOT component audit, patch replay byte-identical; M1 trên G965F thứ hai boot completed và CACHE có marker/heartbeat; M2 có 19 marker, không heartbeat/ADB sau 240 s; cả hai BOOT rollback full readback PASS. Giới hạn: fsync từng kmsg có thể chậm, không dùng timestamp cuối làm thời điểm reset. Không sửa kernel/vendor/radio.

- 0093: probe08 ghi kmsg theo lô và heartbeat có ms monotonic/số record. Lý do: probe07 fsync mỗi dòng, mốc cuối không đáng tin để kết luận reset. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0093-batch-kmsg-heartbeats.patch`. Kiểm chứng: static AArch64 compile, BOOT component audit, M1 boot completed với 17 heartbeat đến ms1761; M2 19 PID1 markers, tối đa một heartbeat/segment, không ADB sau 240s. Cả hai BOOT rollback full hash PASS. Không chạm kernel/vendor/radio.

- 0094: probe09 báo kết quả `execve("/init.real")` qua pipe `O_CLOEXEC` để phân biệt exec thất bại với exec thành công/parent exit. Lý do: probe08 chỉ cho thấy recorder ngừng sớm, chưa biết lệnh chuyển PID1 có lỗi không. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0094-report-init-exec-through-cache-child.patch`. Kiểm chứng: AArch64 static build, errno `ENOENT` self-test qua pipe PASS, M1 boot completed; M2 ghi `M2-INIT-EXEC-PIPE-EOF`, không có `EXEC-FAILED`, sau đó vẫn 240 s không ADB. Hai BOOT rollback về baseline với full readback PASS. Chưa xác định lỗi sau exec; không sửa kernel/vendor/radio.

- 0095: đưa `init.debug.rc` và `init_debug_log.sh` vào hook vendor cuối sau restore baseline trong `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/customize.sh`. Lý do: build M1 diagnostic trước đó log `Adding Exynos9810 vendor init.debug boot logger` nhưng final `work_dir/vendor` thiếu hai file, vì finalization khôi phục vendor sau hook ban đầu. Files ảnh hưởng: `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/customize.sh`; build gate từ chối gói diagnostic nếu thiếu payload cuối. Kiểm chứng trước build: `bash -n` PASS; full build M1 diagnostic mới phải chứng minh hai file và SELinux/fs_config metadata trong final vendor image, sau đó hiệu chuẩn trên G965F M1 serial `M1_G965F_SERIAL`. Artifact build trước được giữ nguyên nhưng không flash. Không đổi kernel, radio hay image phân vùng ngoài ROM.

- 0096: thêm overlay bootstrap Android 16 chỉ bật qua profile chẩn đoán M2 trên `star2lte` + donor `essi_a376b_android16`. Lý do: A37 linker aborts trên DS-ACK 4.9.337 với `MADV_WIPEONFORK failed: Invalid argument`; trên cùng Exynos9810 CPU, M1 linker/libc/libm/libdl/libdl_android liệt kê A37 init dan chạy shell vô hại. Files ảnh hưởng: `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`, M1 profile để reset flag, profile M2 bootstrap compat. Copy fail-closed theo 5 SHA đã xác minh từ S22 M1; giữ linker SELinux label `system_linker_exec`, bootstrap libs `system_lib_file`. Kiểm chứng: fixture exact hashes/fs_config/file_context PASS, M1=false/M2=true profile PASS, `bash -n`, patch replay 87/63 PASS; chưa clean-build hoặc flash. Bước tiếp theo build artifact riêng, giữ attempt49 và các bản lưu khác.

- 0097: đưa marker hoàn tất stage cuối vào trước điểm format USERDATA trong `scripts/internal/split_legacy_sideload.py`; các stage trung gian giữ thứ tự cũ. Lý do: ZIP cuối có thể nằm trên USERDATA mà chính stage đó format, nên marker ghi sau format có thể không được đọc lại. Files ảnh hưởng: splitter legacy sideload. Kiểm chứng: patch replay byte-identical PASS; kiểm tra fixture/order và flash-stage `next=done` là gate của build/test tiếp theo. Đây đồng bộ hóa hành vi packaging đã dùng trước đó, không thay đổi phạm vi phân vùng.

24/10/2026 — standalone diagnostic correction (outside the 87-patch fork series):
`diagnostics/probe11-wait-status.patch` derives probe11 from the known-broken
probe10 evidence. Reason: wrong wait-stop signal decoding causes PTRACE_CONT
errno5; private procfs was absent. Files: probe11 diagnostic C and test/docs only.
Verification: AArch64 static compile; real SIGCHLD failure reproduction and
correct forward/exit tests PASS on TWRP and DS-ACK; attach selftests PASS.
No probe11 PID1 boot/image test yet. Source handoff helpers only clone/apply the
existing fork or export allowlisted text; they do not replace the build system.

0098 — source handoff reproducibility correction: the prior active87-patch
series omitted five existing checkout files (optional PID1 recorder, diagnostic
preparation helper, M2 init-debug/pstore profiles, and existing battery-health
class-location guard). Record their actual content without editing the local
fork. Verification: canonical GNU patch replay88 and comparison against every
modified/added source path. Some historical patch headers require GNU patch,
so the handoff helper uses --dry-run then --batch --forward, as prior audits did.


24/10/2026 — standalone passive diagnostic (outside the 88-patch fork series):
`diagnostics/bootlogger_pid1_probe12_passive.c` removes PID1 ptrace from the
probe11 boot path while retaining the private CACHE logger. Reason: isolate
tracer influence. Files: the standalone C source, diagnostic documentation and
selected evidence only; normal fork recipes are unchanged. Verification:
static AArch64 compile, actual recovery/M1 namespace tests, exec-error test,
full image/CPIO audit, BOOT readback, bounded boot observation, log capture and
baseline rollback all completed. M2 still bootloops with policy-load markers;
no fatal/panic or second-stage start captured. See probe12-summary.json.
The existing probe11 correction diff now also records SIGCHLD details and
proc-read failures; boot-tested source is retained. `probe13-plan.md` describes
instrumentation acceptance gates only and has not produced another image.


24/10/2026 — standalone probe13, outside the 88-patch fork series. Reason:
verify recorder access across policy activation after probe12 reproduced the
loop without tracing. Files: `diagnostics/bootlogger_pid1_probe13.c`,
`tools/decode_probe13.py`, diagnostic docs and selected evidence. Uses an
already-mapped/preallocated regular CACHE file, explicit read/write/fsync/
msync outcomes and retained proc FD; no raw CACHE sectors or policy changes.
Corrects only the recorder-private kmsg inode label after an observed read
EACCES. Kernel/DTB/cmdline/non-init CPIO and normal fork recipes remain intact.
Verification: actual recovery/DS-ACK namespace/detach/persistence/error tests,
frame corruption rejection, image audit and M1 runtime calibration/BOOT
rollback PASS. M1 confirms kernel-domain VFS read/write denial with surviving
mapped output and successful second-stage boot. M2 test/capture/rollback are
complete:30 second-stage starts,23 BoringSSL SIGABRT/reboot-on-failure records,
22 explicit shutdown commands. Runtime APEX linker EINVAL/exit134 is reproduced
on recovery and pinned DS-ACK. No normal ROM patch was changed. Detailed report:
`docs/probe13-report.md`.

## Standalone kernel diagnostics (separate from ROM patch series)

`patches/kernel/series` contains 0000–0002 for the evidence-backed A37 runtime
APEX WIPEONFORK failure. Reasons, affected files, pinned source, actual config,
compiler, image hashes and rollback are in `docs/m2-runtime-kernel-fix.md`.
Replay of 3 patches / 7 files matches the isolated built source; baseline and
feature compare 10,189 exported CRCs with no changes. M1 feature calibration
passes, while M2 test has been launched and is not yet a final acceptance.
The fork remains at 88 ROM patches; this diagnostic series does not silently
change the normal ROM kernel default or retained M1/M2 images.


24/10/2026 — standalone `diagnostics/m2-metadata-tmpfs.patch`, outside normal fork series. Reason: read-only /metadata prevents donor aconfig tables from loading and RANGING permission registration, causing AppOps systemReady fatal. Affected test file: SYSTEM system/etc/init/aconfigd.rc; adds only tmpfs mount/restorecon before donor early-init aconfig actions. Fixed controls: same WIPEONFORK diagnostic BOOT, retained S22 Tethering APEX and system64 V6 interface as preceding runtime test. Verification: donor flag enabled/default, RANGING registered dangerous, Welcome screenshot, boot_completed1/Enforcing and stable SystemServer; captured unresolved vendor aconfig and camera ABI errors. Full original rc/APEX restoration, newly added V6 deletion and BOOT cc618abc rollback/readback PASS; no protected partition changed during BOOT writes. No production recipe/kernel default changed. Limit: no flag persistence across reboots/OTA and no hardware acceptance. Report: docs/m2-metadata-runtime.md.

## 24 October 2026 — patches0099–0101

| Patch | Reason / affected files | Verification |
| --- | --- | --- |
| 0099 native camera ABI/version | Existing native finalization; two Python adapters. A37 server replaces ABI-incompatible S22 server while preserving reviewed8byte fallback; exact80500 enum compatibility | Pinned input/instruction checks, other bytes unchanged, compiled enum, onlyclasses3.dex change/same signer; rear/front preview/JPEG decode PASS |
| 0100 persistent metadata/container | Existing boot_restore, M1/M2 profiles, two adapters. Reapply lost CACHE bind after vendor restore, mirror20 labels and describe flagless vendor | Idempotence/negative guards, M1 reset, independent upstream readers, actual platform-init exits0, persistent inode/hash through reboot/Enforcing |
| 0101 missing GLES1 wrapper | One existing native32 lock entry for S22 system/lib/libGLESv1_CM.so | Pinned SHA/label, no app32/ART32, actual gralloc/provider startup and rear/front capture |

All91patches/72touched paths replay identically; Bash syntax and source guards
PASS. Runtime fixture SYSTEM/VENDOR changes and BOOT fully rolled back with
hash checks, new CACHE metadata backed up/removed. No full ROM/package yet.
Detailed reasons/files/provenance/tests/limits/rollback:
[report](m2-camera-metadata-fixes.md). M1/G965N, protected radio/EFS partitions,
production kernel choice and retained ROM archives unchanged by these actions.

## Integrated diagnostic build gates — patches0102/0103

0102 reason: installer would replace a staged diagnostic BOOT kernel. Files:
explicit M2 G965F runtime profile, original ZIP preparation helper, runtime lock
and stager, existing boot_restore/release checker/cache identity and M1 reset.
Checks: actual recovery BSPATCH/Image hash and original BOOT components, strict
model/donor/hash rejection, stage idempotence and profile reset. Generated ZIP
and firmware stay excluded. Original production kernel selection unchanged.

0103 reason: staged verification alone cannot prove actual image contents.
Files: existing image verifier and a read-only debugfs helper. Checks: real ext4
file bytes compared to verified staging, selected modes/owners/SELinux labels;
Camera/runtime/storage and mandatory ADB artifacts included. Build/runtime
outcomes pending, not assumed. Rollback: reverse patch/select original profile;
retained artifacts and baseline BOOT kept. No protected partition write added.
Report: docs/m2-integrated-runtime-build.md.

24/10/2026 — Patch0104, `0104-enable-m2-volatile-metadata-diagnostic.patch`. Reason: attempt52 failed at `init` because `/metadata` is read-only, so aconfig did not load and SystemServer raised the RANGING AppOps fatal. Files: existing `a376b_runtime_compat.py` and its hash lock; the opt-in M2 final SYSTEM receives a 16MiB tmpfs mount at `/metadata` before donor aconfigd creates maps. Ordinary profiles are unchanged. Verification: exact donor/generated SHA, apply idempotence/order, patch apply from0102-created source, compiled/final image exact-byte check; attempt53 fresh runtime must verify flag, dangerous RANGING, SystemServer stability and boot completion. Rollback: select normal profile or reverse0104; tmpfs is volatile. No AppOps bypass, kernel, RIL, EFS or G965N change.

Attempt53 execution for patch0104: clean existing-fork build exit0; compiled ADB defaults/no-auth and saved-choice lifecycle, Exynos9810 contract, staged SELinux and real ext4 SYSTEM/VENDOR/ODM checks PASS. Signed flash ZIP and eight recovery ZIP32 stages PASS. After one-time user authorization for exact SHA `f72be9be303e34fd5f625c5fd3d50e0590589e62b85386d8816f97f9758343ef`, fresh install on authorized G965F completed 8/8, formatted USERDATA, preserved protected raw hashes, and first-booted to `sys.boot_completed=1`. Live ADB defaults/no-auth and Rampart-disabled states passed; `/metadata` tmpfs created aconfig and RANGING registered. The selected runtime-diagnostic profile does not enable the optional patch0095 logger, so its absence in VENDOR and `/cache/unica` is expected and not a patch failure. Do not reuse one-time authorization on a different package. Full details and local logs are in `docs/m2-integrated-runtime-build.md`.


## 24 October 2026 — Patch0105: preserve the persistent metadata init hook

Reason: attempt53's on-device installer unconditionally copied the secondary
vendor init over the canonical `etc/init/hw/` file, so the CACHE bind that had
passed the ext4-image audit never reached the running system. The selected
profile also inserted a tmpfs over `/metadata`, masking any persistent bind.
Files: `platform/exynos9810/installer/repartition/vendor_bootfix.sh` and
`unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_runtime_compat.py`.
The installer copy is now fallback-only; the runtime helper keeps original
donor `aconfigd.rc`, rejects a tmpfs `/metadata`, and requires one canonical
`/cache/metadata` bind. Verification so far: patch reverses/reapplies and shell
and Python syntax pass. Before claiming a runtime fix, the next retained build
must pass real ext4 and embedded-installer audits, then a normal reboot must
show `/metadata` backed by CACHE and preserve an aconfig marker's inode/hash.
Rollback: reverse0105 and disable the M2 CACHE metadata-bind flag. No kernel,
vendor blob, RIL, EFS, or modem change.


## 25 October 2026 — Patch0106: select and verify the CACHE logger

Reason: attempt54's runtime profile did not select patch0095's logger. Files:
parallel runtime-init-debug profile; M1 profile flag reset; existing cache key,
config serialization and real-ext4 runtime verifier. The only runtime delta is
EXYNOS9810_ENABLE_INIT_DEBUG=true. Verification: profile environment delta and
M1 reset PASS; true/false cache keys differ; real-ext4 fixtures accept correct
logger bytes/metadata and reject missing file, wrong GID and wrong label;
96-patch /79-path full replay is byte-identical. Clean build/package and
G965F runtime remain pending. Rollback: select runtime-diagnostic profile and
retain both releases; no kernel/BPF/seapp/radio behavior change in this patch.
Detailed report: docs/m2-integrated-runtime-build.md.


Patch0106 packaging follow-up: adding logger fields to config exposed the
existing buildenv clearing step, which removed those profile values before
regeneration. The first package pass was rejected before flash because its
config was false and actual-image logger checks were skipped. Added a narrow
exception for the two explicit logger profile flags in buildenv.sh, plus
actual repeated regeneration tests true/true/false/true. Current complete replay
is96patches /80paths, byte-identical. Repackaging uses the compiled tree and
requires true after buildenv; actual VENDOR logger checks remain mandatory.


Patch0106 final verification:96patches/80paths byte-identical replay PASS;
compiled gates, actual final ext4 and installed ro,noload logger UID/GID/mode/
label/byte audits PASS. Retained attempt55 fresh install8/8 and two Enforcing
boots PASS for markers/logcat only. props/dmesg snapshots are0bytes; their
runtime completeness is NOT accepted. Exact evidence/rollback profile and
remaining independent errors: docs/m2-integrated-runtime-build.md. Same existing
diagnostic BOOT as attempt54; no kernel/BPF/seapp/radio fix was introduced.


## Patch0107 — restore the donor-pinned `vendor_kg` name for M2

Reason: attempt55 logcat lines152556/169261 say UID5959 resolves as `oem_5959`,
so the retained kgclient seapp entry for `vendor_kg` does not match; child
zygotes then abort at lines153275/169303. Live G965F ADB confirms
`uid=5959(oem_5959) gid=5959(oem_5959)`. The staged Hybrid Vendor files lack
UID/GID5959, while the verified A37 donor has exactly
`vendor_kg::5959:5959::/:/bin/sh` and `vendor_kg::5959:`; the compiled rule
already exists at system_ext_seapp_contexts:35.

Files: M2 runtime adapter and locked donor-file hashes, vendor restore
customizer, and actual ext4 image verifier. Scope: the existing G965F M2 runtime
compatibility profile only. The adapter appends only the two donor-pinned rows,
rejects any same-name/ID collision and sets vendor_configs_file metadata. It does
not add a SELinux permission. Donor hash/row, idempotence, conflict rejection,
97-patch replay and actual ext4 checks pass. The unique release was installed
fresh in eight signed ZIP32 stages on the authorized G965F; USERDATA UUID
changed and protected raw hashes matched. Before boot, installed VENDOR was
mounted `ro,noload` and passwd/group/logger bytes, owners, modes and labels
matched. Across two Enforcing boots, `id 5959` resolves to vendor_kg and the
package process enters `u:r:kg_app`; captured logs contain zero `No match for app
with uid 5959` and zero zygote `Fatal signal 6 (SIGABRT)`. A different failure
remains: `TLC_ERROR`/`@TimeoutException: getTAInfo - null` precede a
LockedBootCompletedReceiver ANR on each boot (attempt56 boot1 logcat
253262/253958; boot2 195922/195946; trace2 90-94). This patch does not resolve
the KnoxGuard TA/backend issue. Rollback/reference: retained attempt55 or select
the earlier runtime profile. M1, G965N, radio and kernel are untouched.

## Patch0108 — isolate source firmware cache by locked donor build

Reason: A376B Android16 AZH4 and Android17 BZI6 share model/CSC and the old
fork key (SM-A376B_XXV), so extraction, generated config, work-tree reuse,
module source lookup and target-files provenance could mix donor generations.
Files: scripts/utils/common_utils.sh, firmware download/extract utilities,
workdir/config/target-files/build-hash callers, and existing source customizers
that resolve donor files. The suffix is optional and only affects the configured
source model/CSC; existing M1/S22 and M2/AZH4 profiles remain on their original
cache keys. M4 must lock the exact build suffix A376BXXU5BZI6.

Verification: full base + 97 prior patch replay, patch0108 dry-run/apply,
bash -n on changed shell files, Python syntax on changed Python files, and
selector fixtures: M1=SM-S901B_EUX, M2=SM-A376B_XXV, M4=
SM-A376B_XXV_A376BXXU5BZI6; traversal suffix rejection. Extraction completed with the fork: all six partition trees have non-empty fs_config/file_context metadata and the build-specific marker is exact. Still pending: full Android17 donor/config audit, donor profile, M2→M4→M2 output switch-back, and clean build. Rollback: remove patch0108 and
omit SOURCE_FIRMWARE_CACHE_SUFFIX; no firmware files, kernel or phone changed.

## Patch0109 — adapt BZI6 Android17 policy to Hybrid Vendor VNDK33

Reason: the first complete staged compiler found missing Android17 version33
aliases, a telephony property label change, and a conflicting obsolete vendor
`genfscon`. The affected lines are captured in
`logs/20261025-m4-preflight/selinux-vndk33-adaptation.txt`.

Files: new `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_android17_selinux.py`;
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`;
`patches/fork/0109-adapt-a376b-android17-selinux-vndk33.patch` and
`patches/fork/series`. The adapter is restricted to BZI6 SDK37/release17,
star2lte M4, and VNDK33. It adds 16 missing aliases, converts one
`telephony_prop` allow to the existing VNDK33 `telephony_config_prop` alias, and
removes one exact obsolete duplicate proc label; it retains the KnoxZT rule.

Verification: self-test covers exact preimages, VNDK rejection and idempotence;
actual BZI6 + embedded vendor inputs pass the existing staged `secilc` verifier
after transformation. Shell/Python syntax passes. `tools/prepare_fork.sh` replayed
the complete series from pinned commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`;
the replayed helper self-test passed and its finalizer/helper files match the
active fork tree. No Android17 build, policy load, or flash has occurred. Rollback:
remove patch0109 from `patches/fork/series` and return to the pre-M4 fork tree.

## Patch0110 — preserve BZI6 runtime SIM-tray detection

Reason: the BZI6 build/config cache has no static SIM-tray count, while its
Android17 `TelephonyFeatures.isOneTray()` evaluates active modem count and
`ril.simslottype1/2` at runtime. The previous config comparison cannot safely
invent a donor tray count or replace the donor method with a hard-coded result.
BZI6 `UiccController.sendIntentForTrayProtectionPopup(IZ)V` also reports
`waterproof=true`, matching the target feature flag.

Files: new `rom/unica/patches/product_feature/a376b_android17_sim.py` and the
SIM-count branch in `rom/unica/patches/product_feature/customize.sh`; this patch
file and `patches/fork/series`. It is restricted to source config
`essi_a376b_android17`, milestone M4, `star2lte`, source count `none`, target
count1. It verifies the two exact BZI6 method hashes before preserving runtime
behavior.

Verification: helper self-test PASS; actual BZI6 framework and telephony smali
fixture PASS, including method hashes and dynamic slot/property markers;
`bash -n` and Python syntax pass. Patch dry-run/apply and helper self-test pass
on the already prepared base+0109 replay checkout. No build, policy load,
device write or flash occurred. Runtime SIM behavior still needs target radio
logs after an M4 boot. Rollback: remove patch0110 from `patches/fork/series` and
restore the pre-M4 product-feature customizer.

## Patch0111 — guard BZI6 optional camera claims and retain GPPM

Reason: the complete BZI6 extraction lacks the action-classifier libraries and
ARDoodle assets, while the fork can restore old floating-feature claims. The
Android16-only cleanup did not cover the new donor. A previous audit also
incorrectly classified GPPM as absent; its APK and permission files are present.

Files: `rom/unica/patches/camera/customize.sh`, new
`rom/unica/patches/camera/a376b_android17_features.py`, and
`rom/scripts/internal/verify_exynos9810_release_defaults.py`; patch/series and
the source lock index. M4 support is restricted to BZI6 SDK37, G965F/star2lte,
the isolated source-cache suffix and exact extracted marker. The helper pins
the donor floating-feature XML and GPPM APK, verifies the four absent optional
paths, then enables only classifier/ARDoodle claim cleanup. The final release
gate checks both floating-feature XML files and retained GPPM. No GPPM removal
or eSE change is included.

Verification: actual extracted BZI6 source PASS; final-work fixtures PASS and
reject stale ARDoodle claims, missing GPPM, and wrong milestone. Shell/Python
syntax PASS. All 100 earlier patches replay from the pinned fork commit; 0111
dry-run/apply PASS, all three replayed files equal the active tree, and diff
whitespace checks PASS. Logs: `logs/20261025-m4-preflight/camera-feature-gates.txt`,
`replay-through-0110.txt`, and `replay-0111.txt`. This is source/fixture evidence;
no actual M4 image, build, flash or camera runtime acceptance yet. Rollback:
remove 0111 from series and restore its three fork paths from base+0110.

## Patch0112 — match SIM hash to the fork's no-debug decode

Reason: patch0110's TelephonyFeatures fixture was decoded with debug info,
whereas `rom/scripts/apktool.sh` always uses `--no-debug-info`. The same BZI6
method therefore has a different textual hash and the old guard would reject
the actual build input. Runtime tray logic and the UiccController hash do not
change. Files: `rom/unica/patches/product_feature/a376b_android17_sim.py`,
patch/series and `sources.lock.json`.

Verification: actual no-debug framework/telephony fixture PASS; debug fixture
is rejected with `ValueError: BZI6 method hash changed:
framework.jar/com/android/internal/telephony/TelephonyFeatures.smali isOneTray()Z`.
Parser self-test, syntax and replay PASS. Log: `sim-nodebug-gate.txt` under
`logs/20261025-m4-preflight/`. This fixes a source guard before any M4 build.
Rollback: restore the helper from base+0111; that also restores the known
decode-mode mismatch, so it is not a usable M4 build configuration.

## Patch0113 — restore BZI6 hardware mDNIe methods

Reason: the Android16-only adapter cannot select the BZI6 methods, and BZI6
removed the old DeviceIdleController synthetic logging helper used by the
fork's restored color-blind/negative methods. Files:
`rom/unica/patches/product_feature/mdnie/a376b_hw.py` and the mDNIe branch of
`rom/unica/patches/product_feature/customize.sh`; patch/series and source lock.

The explicit A17 branch is restricted to M4/star2lte/G965F/SDK37/BZI6. It pins
the donor methods, A11yRune initializer, file-existence outline and original
fork mDNIe diff. It restores the reviewed permission-checked sysfs methods,
color-table writer and curtain state, and inlines append/toString/Slog.i
instead of guessing a new synthetic class. AZH4 behavior remains selected by
its existing branch.

Verification: both AZH4/BZI6 fixtures PASS; repeated or wrong-scope input is
rejected before partial writes. All affected fixture Smali folders assemble
to DEX039 with API29, matching the fork's existing SmaliBuilder cap. The
earlier standalone `--api 37` test produced
`java.lang.IllegalArgumentException: dexVersion must be within [0, 999]`;
that invocation does not match the fork and is not a donor runtime failure.
Syntax, patch apply and replay byte comparison PASS. Log:
`logs/20261025-m4-preflight/mdnie-a17-test.txt`. Full artifact compilation,
Dalvik verification and real display behavior remain pending. Rollback:
remove0115,0114,0113 in reverse order if reverting the shared customizer, then
restore the two paths to base+0112.

## Patch0114 — adapt BZI6 APE and connection-personalization dispatch

Reason: BZI6 changes the injector APE region, SUPPORT_APE ordinal35→37 and
SQE register layout; the M2 branch cannot be reused with its old preimages.
Files: `rom/unica/patches/product_feature/wifi/a376b_backend.py` and the APE
branch of `rom/unica/patches/product_feature/customize.sh`; patch/series/lock.

The scoped BZI6 branch pins the constructor region, Qbox observer and message5
handler, shell process method and Settings methods. It nulls the two APE
objects, selects personalization0, disables the unsupported Settings control
and returns Not Supported for only SUPPORT_APE. The new shell body uses the
actual low registers v1/v13/v14; blindly keeping p2 in the BZI6 locals16 method
would be invalid for its ordinary invoke encoding. No target HAL/blob change.

Verification: actual AZH4/BZI6 source fixtures and DEX assembly PASS; only the
APE entry changes in the command table. Wrong SDK and changed late Settings
preimage reject with byte-identical inputs (no partial writes). Syntax and
patch replay PASS. Log: `logs/20261025-m4-preflight/wifi-backend-a17-test.txt`.
No Wi-Fi runtime acceptance or final ROM claim. Rollback: remove0115 before
0114 and restore its two files to base+0113, since0115 expects APE's output.

## Patch0115 — adapt BZI6 SoftAP country/power/sharing-lite branches

Reason: the three A376B-specific hotspot adapters were dispatched only for
Android16, and BZI6's power-saving shell case uses different registers and
branch labels. Files: `rom/unica/patches/product_feature/wifi/a376b_softap.py`
and the four relevant branches in `rom/unica/patches/product_feature/customize.sh`
(country, power helper, power SPF string, sharing-lite); patch/series/lock.

Country fallback still applies only to missing/NA HAL data; access permission,
valid HAL results, debug override and Binder identity cleanup remain. BZI6's
region method is pinned separately. Power disables the unsupported initializer
and changes only command ordinal29 using v1/v13/v14. It pins the shell method
after0114; sharing-lite pins the chip method after power. This explicit order
matches product_feature, preserves the donor's carrier/model exclusions and
does not force a regulatory country or add RF/backoff assumptions.

Verification: both donors pass the composed APE → country → power →
sharing-lite fixture and DEX assembly. Only APE/power command table entries
change; country permission/HAL/Binder cleanup markers remain. Sharing-lite
before power is rejected without writes. Syntax, apply and replay PASS. Log:
`logs/20261025-m4-preflight/wifi-softap-a17-test.txt`. Runtime hotspot channels,
power and sharing behavior still need M4 device tests. Rollback: remove0115
and restore its two fork files from base+0114. No kernel/vendor/radio/partition
or live setting writes were made in this source-adapter work.

Integrated replay checkpoint0115: all105 textual patches apply from the pinned
fork commit, and all87 resulting files match the active fork byte for byte.
The lock list has the same order/count and all105 hashes pass; previous101
hashes are unchanged. Evidence: `replay-through-0115.txt` in the same log folder.

## Patch0116 — BZI6 WLAN target policy and SoftAP11ax chip query

Reason: BZI6 embeds an A37 camera RF table; star2lte's fork config uses custom
backoff `none`. The empty-table adapter/public chip query were Android16-only.
Files: new `rom/unica/patches/product_feature/wifi/a376b_android17_wlan.py`
and backoff/11ax branches in `rom/unica/patches/product_feature/customize.sh`;
patch/series/lock.

The M4/star2lte/F/SDK37/BZI6 helper pins constructor/addBackoff/getter methods,
source/target booster000, empty guard and three literal locations. It empties
only the donor table; all other HAL/coexistence code remains identical. No RF,
country, calibration or antenna value is invented. The 11ax branch pins the
chip selector/query and selects driver capability with a null fallback.

Verification: actual BZI6 fixture passes both modes composed with0114–0115;
wrong source/target/threshold/guard rejects before writes. Coex byte comparison,
limited command-table changes, DEX039 assembly, syntax and patch replay PASS.
Log: `logs/20261025-m4-preflight/wlan-rf-a17-test.txt`. RF/runtime validation
remains open. Rollback: remove0116 and restore its customizer/helper state from
base+0115; later patches do not depend on it.

## Patch0117 — BZI6 signature integration context

Reason: M2 signature preflight reports `1 out of 7 hunks FAILED`. BZI6 retains
the certificate-collection call/v15 receiver/v1 ParsedPackage but renames its
exception range to try_start_10/try_end_10. Files: new BZI6 services.jar patch
and `verify_a376b17_preimages.py` in `rom/unica/patches/signature/`, plus its
customizer; patch/series/lock.

Only that hunk's exception-range context changes relative to M2. The existing
configured-certificate equality, normalization and platform SELinux identity
guard remain. Three complete input classes are pinned before APPLY_PATCH.
Verification: actual BZI6 patch applies with fuzz0; existing identity helper
accepts BZI6 plat_mac_permissions XML; normalization location, both applyPolicy
callers, real signature-array comparison and android/configured-key guards are
checked. Three affected classes assemble to DEX039. Wrong milestone/cache and
each altered class reject before writes. Logs: `core-dispatch-preflight.json`
(before), `signature-bzi6-test.txt` (after) in the same preflight folder.
Full M4 install/boot behavior remains untested. Rollback: remove0117, restore
customizer to base+0116 and remove its two new files. M2/S22 branches remain.

## Patch0118 — BZI6 split lights API and legacy SehLights extension

Reason: M2 backlight preflight reports `2 out of 8 hunks FAILED`. BZI6 uses
LogicalLight/DisplayManagerInternal and splits LightImpl into locked state and
unchecked HAL writes with mMuted/mIsSvcLightChanged. Files: new
`patch_bzi6_sehlights.py`, new BZI6 backlight patch in
`rom/unica/patches/legacy/lights/`, and `rom/unica/patches/legacy/customize.sh`;
patch/series/lock.

Four complete BZI6 classes and the backlight diff are pinned. The diff applies
with fuzz0 to a temporary copy; all transforms finish before decoded-file
writes. LogicalLight/cast and conversion API remain. Four-int methods remain
as wrappers; five-int methods use the fork's color/extended-brightness/mode/on/
off order. State/mute handling remains, with cached extended brightness so
changes sharing an RGB value are not skipped. Mute's color0 write uses
sentinel-1; nonzero-color unmute replays the cached value. Standard ILights
remains; SehLights Parcel sends the supplied extra int after HwLightState
instead of hard-coded-1. Two-argument proxy callers retain a wrapper.
PowerManagerUtil enables the extension without rewriting nearby labels.

Verification: wrong milestone/model and each altered class reject before any
decoded-file writes. Fixture checks old/new APIs, state/mute/cache markers,
self-member resolution, standard HAL path, actual Parcel ordering and the
referenced BZI6 static conversion method. Both affected folders assemble to
DEX039; syntax/replay PASS. Logs: `lights-bzi6-method-diff.txt` and
`lights-bzi6-test.txt`. Brightness/auto-brightness/AOD/LED/mute behavior needs
real M4 runtime tests. Rollback: remove0118, restore legacy customizer to
base+0117 and remove its two new files. M2/kernel/HAL binaries are unchanged.

Checkpoint0118:108 textual patches replay from pinned commit2b25ae6;92 files
match the active fork byte for byte. Previous105 patch hashes are unchanged.
Evidence: `logs/20261025-m4-preflight/replay-through-0118.txt`.

## Patch0119 — BZI6 rear capacitive fingerprint stack

Reason: the existing rear-fingerprint adapter was Android16-only. BZI6 keeps
the two framework enum methods but changes SemBiometricFeature and LsRune
initializers; SystemUI's true register is now v3 rather than v16. Files:
`rom/unica/patches/product_feature/fingerprint/a376b_rear.py` and both relevant
branches in `rom/unica/patches/product_feature/customize.sh`; patch/series/lock.

The new branch is restricted to BZI6/M4/star2lte/F/SDK37 and exact optical-source/
rear-target feature strings. It pins both framework methods, the service and
SystemUI initializers, and the pinned compatibility BiometricSetting config/
constructor. Settings is checked after the generic SPF replacement. It changes
only rear/capacitance and optical/display constants; the true register remains
for unrelated flags. The client APK is the fork's existing star2lte overlay,
SHA256 bc64ca2e5083ae651a57e523eb68496ccf6f9dcaeabfed32c3adc1998acb877c;
no donor APK, HAL or native fingerprint blob is substituted.

Verification: actual no-debug BZI6 and AZH4 fixtures pass; wrong milestone/model
and missing late Settings substitution reject without partial writes. Both
donors' affected folders assemble to DEX039; unrelated true-register assignment
lists remain identical. Syntax and patch replay pass. Logs:
`fingerprint-method-audit.txt`, `fingerprint-ui-audit.txt`,
`fingerprint-bzi6-test.txt` under `logs/20261025-m4-preflight/`. This is source/
assembly evidence; full compiled artifact and enrollment/unlock are untested.
Rollback: remove0119, restore its two files to base+0118;0120 is independent.

## Patch0120 — BZI6 OMX defaults and semantic HEVC branch port

Reason: AZH4's native signature does not exist in the BZI6 ELF, the encoder
class/register layout differs, and vexfwk_service is PRESENT in BZI6. The M2
missing-APK skip cannot be extended. Files: new
`rom/unica/patches/legacy/a376b_android17_media.py`, OMX dispatch in
`rom/unica/patches/legacy/customize.sh` and
`rom/scripts/internal/verify_exynos9810_release_defaults.py`; patch/series/lock.

The scoped helper pins BZI6's 64-bit Stagefright input hash
f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517,
rejects unexpected32, verifies the executable PT_LOAD mapping and exact
comparison/branch context, then NOPs only CBZ w0 at VA/file0xebdbc. LLVM21
relocation shows the preceding GOT load at0x280878 references
MEDIA_MIMETYPE_VIDEO_HEVC; equality takes0xec69c, whose path logs
`Do not enable feature for HDR10Plus recording` and calls the assertion.
The port matches the existing fork's narrow HEVC assertion-path adjustment;
other metadata checks remain. Only4bytes change. Expected output SHA256:
4f36b1c04855ac6fac952a17f350cef93fc83974176f9173ec19790230a5ba58.

Four actual EncoderFilter.configCodec method hashes are pinned before changing
their Message.get default0x7f420888 to0x7f000789: MotionPhoto/GPPM/Camera use
v4, vexfwk uses v2. The Camera class is in DEX4. A pinned MotionPhoto transcoder
still lacks the old configVideoEncoderParameters method; only that absent
legacy edit is omitted. All four APKs, including vexfwk, are required. Validation
finishes before any decoded/native writes. The new release gate checks the
native output hash and disassembles the actual compiled encoder method from
each APK, checking the new boxed default and Message.get usage.

Verification: source fixture, four encoder DEX assemblies and compiled gate
PASS. Wrong scope, modified last/vex method, native preimage or unexpected32
layout reject without partial writes. The compiled gate rejects the unpatched
native and a real donor vex APK with its old default. Native comparison proves
all bytes outside the single instruction unchanged; post-disassembly shows NOP.
Logs: `media-encoder-methods.txt`, `media-bzi6-test.txt`, and
`media-stagefright-bzi6/` under the same preflight folder. Fixture APKs contain
the actual assembled encoder class; this is not a full signed APK/ROM rebuild
or a codec runtime test. A Baksmali CLI option issue was corrected before this
checkpoint: this tool requires separate `--debug-info false` arguments.
Kernel, HAL and vendor codec declarations are unchanged. Rollback: remove0120,
restore legacy/release customizers to base+0119 and delete the helper; rebuild
the work tree rather than hand-editing a retained ROM package.

Checkpoint0120:110 textual patches replay from pinned commit2b25ae6;93 files
match byte for byte and all108 earlier patch hashes are unchanged. Evidence:
`logs/20261025-m4-preflight/replay-through-0120.txt`.

## Patch0121 — BZI6 shipping API and retained HDR controllers

Reason: the original shipping dispatch invokes absent HdmVendorController
constructor/TAProxy edits for BZI6, and the HDR enable branch was A16-only.
Files: `rom/unica/patches/product_feature/customize.sh`, new
`rom/unica/patches/product_feature/a376b_android17_product.py`; patch/series/lock.

The BZI6/F/M4/SDK37 helper changes five classes' compiled shipping36 to26,
keeping framework SDK37. Pin EsecommAdapter, HdmSakManager, EngmodeTimeThread
methods; require PowerManagerUtil after0118 lights. Match the entire local
SystemServer shipping log/parse/threshold block so unrelated eSE startup edits
can coexist. Pin HdmVendorController and require TAProxy absence. HDR pins both
complete BZI6 controller classes, changes only availability3→0, and selects the
existing floating feature; the implementation and setting observers remain.

Verification: real BZI6 fixtures compose with0118; changed method/class and
late TAProxy negatives reject before decoded writes. Five shipping classes
and both HDR controllers assemble to DEX039 with the existing writer. M2
shipping/HDR shell traces are byte-identical to the previous source. Evidence:
`logs/20261026-m4-product-display/shipping-hdr-test.txt`. No device or eSE/HDR
hardware acceptance claim. Rollback: remove later dependent0122/0124 first,
then0121; restore the customizer/helper to the preceding patch series.

## Patch0122 — BZI6 HFR/brightness constants and compiled product gate

Reason: BZI6 moved display classes and removed Rune's initializer SPF string5;
its runtime sensor-type5 must not become profile3. The S22 threshold/dump path
also differs. Files: the same product customizer/helper, new
`rom/unica/patches/product_feature/a376b_android17_display.json`, and
`rom/scripts/internal/verify_exynos9810_release_defaults.py`; patch/series/lock.

The new BZI6 display branch pins14 classes/18 methods at this patch (0124 adds
the unchanged fixed-resolution controller). Replace25 selected const-string
instructions plus two input-service field defaults: default60/mode0, empty
adaptive lists/passive thresholds, brightness3. Pin the three unchanged Rune
sensor-query methods. Power's preimage includes lights+shipping; LsRune's
includes rear fingerprint. No arbitrary register/number or global string
replacement. Only the A17 branch uses the new manifest/helper; M2 dispatch is
preserved. The new release gate disassembles actual DEX methods, checks the
selected constants/fields and final system floating-feature XML, including
shipping26 and both HDR returns.

Verification: real lights→shipping/HDR→rear-FP→display composition passes;
changed preimage/scope/feature negatives reject before writes. All affected
folders in ten fixture artifacts assemble to DEX039. The compiled gate passes
these minimal fixture APKs/JARs and rejects actual donor SemImsRune/HFR2 and
mode2 XML. M2 brightness/HFR shell trace is byte-identical to the original.
Evidence: `display-test.txt`, extended `display-fixed-test.txt` in the same log
folder. These are not full signed artifact/release or display runtime tests.
Rollback: remove0124 first, then0122 and restore its four files to base+0121;
rebuild scratch/work, never alter a retained ROM package in place.

## Patch0123 — Exact-descriptor BZI6 DEX resolution

Reason: generic SPF edits before/around the scoped adapters still use original
DEX folder names, while BZI6 moved Settings classes to DEX3. File:
`rom/scripts/utils/smali_utils.sh`; patch/series/lock. Only the reviewed
A17/BZI6/F/M4/SDK37 scope gains a full exact-descriptor search with missing/
duplicate rejection. The old A16 missing-path fallback is unchanged, as are
method/value validation, no-change failures and all other donors. This does
not declare unreviewed generic substitutions compatible.

Verification: actual BZI6 FingerprintSettingsUtils resolves DEX4→DEX3 and the
expected SPF string alone changes. Wrong milestone/model/SDK, duplicate and
absent descriptors reject before a class edit. A16 fallback still succeeds.
Evidence: `smali-dispatch-test.txt`. Rollback: remove0123 and restore the utility
to base+0122; BZI6 generic callers will again stop at old DEX paths.

## Patch0124 — Separate M4 native fixed-resolution selection

Reason: initial M4 must keep the M2 panel baseline while source controls are
still being ported; enabling the M2-only flag would bypass donor/milestone
boundaries. Files: `rom/target/star2lte/config.sh`,
`rom/target/star2lte/profiles/m1-g965f-no-ksu.sh`,
`rom/unica/patches/exynos9810_device_stack/customize.sh`,
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`,
and the product helper/display JSON above; patch/series/lock.

Add STAR2LTE_M4_FIXED_RESOLUTION restricted to BZI6/M4/F; do not create an M4
profile yet. Keep native1440x2960/density560 and remove switcher advertising in
both floating-feature copies at the existing stack/final stages. Selecting the
M1 base (also used by M2) resets the M4 flag. The product helper now requires
that explicit fixed choice and a false dynamic-resolution target; pin the
unchanged BZI6 controller's unsupported3 method. The compiled gate checks that
return and absence of dynamic-resolution advertising as well as HFR lists.

Verification: M1 dynamic, M2 fixed and M4 fixed config fixtures pass; wrong
milestone/donor/cache/model and a borrowed M2 flag reject. Actual M1/M2 profile
selection clears a stale M4 flag; actual finalizer block selects the right
feature operation. The complete source/DEX/product gate fixture was rerun with
this new scope/controller input:15 class guards/19 methods,13 changed display
classes,25 literal/two field edits. It passes with the earlier lights and FP
changes retained. Evidence: `fixed-config-test.txt`, `display-fixed-test.txt`.
Rollback: remove0124 and restore its six files to base+0123; no retained image,
userdata, vendor blob, radio or kernel was changed by these source edits.

Checkpoint0124:114 textual patches replay from pinned2b25ae6 to95 byte-identical
files; all110 pre-checkpoint patch hashes are unchanged. Evidence:
`logs/20261026-m4-product-display/replay-through-0124.txt`. Shell syntax,
Python/JSON parsing and whitespace checks pass. Full artifact gates and M4
runtime remain pending. Detailed scope/limits: `docs/m4-product-display.md`.

## Patch0125 — Preserve target eSE baseline in BZI6

Reason: BZI6 supportEseHal logs the COS inside a prefixed literal; the generic
exact-string substitution fails. Keep M2's target-none policy without guessing
a target COS/SKU. Files: `rom/unica/patches/ese/customize.sh`, new
`rom/unica/patches/ese/a376b_android17_baseline.py`,
`rom/scripts/internal/verify_exynos9810_release_defaults.py`; patch/series/lock.

Pin four full BZI6 classes and exact counts, finish validation before decoded
writes, then perform only the existing COS/vendor-to-empty substitutions.
Preserve every other instruction, including dynamic SKU/country/feature/native
calls. Compiled checks require empty fields, the blank-COS HAL/REE-SPI paths
and retained detection logic. No hardware eSE acceptance or new COS selection.
Verification: actual source/DEX fixtures; all four altered preimages and wrong
scope/new COS reject without partial writes; actual donor SecureElement fails
the gate; M2 dispatch trace is byte-identical. Evidence: ese-bzi6-test.txt and
ese-original-rule.txt in logs/20261026-m4-profile-preflight. Rollback: remove0125,
restore its customizer/release hook to base+0124 and delete the helper; rebuild
work rather than altering retained images. Later release-hook patches depend
on the current source series and must be removed/rebased first.

## Patch0126 — Explicit retained BZI6 Wi-Fi UI

Reason: old S22 Wi-Fi6 UI patches do not match BZI6, which changed a filter to
Kotlin coroutine code. Initial M4 keeps donor UI with target Wi-Fi5. Files:
`rom/unica/patches/product_feature/customize.sh`, new
`rom/unica/patches/product_feature/wifi/a376b_android17_ui.py` and `.json`,
`rom/target/star2lte/profiles/m1-g965f-no-ksu.sh`, release-default verifier;
patch/series/lock. The M4 flag is distinct; M1/M2 selection resets it.

Pin13 source classes and the old lambda's absence before skipping both S22 UI
patches. The compiled gate checks canonical full class bodies, ignoring only
comments/source markers, hidden-API labels and string-jumbo encoding. No HAL,
11ax/6GHz capability, RF or vendor changes. Verification: all13 classes assemble
and round-trip to the same canonical bodies; removing the real Wi-Fi6 method
fails; scope/changed-input/borrowed-M2-flag negatives and actual A17/M2 shell
traces pass. Evidence: wifi-ui-test.txt. Rollback: remove0126, restore three
existing files to base+0125 and delete its helper/manifest; later dependent
release/profile changes must be removed/rebased first.

## Patch0127 — Scoped initial M4 Face/Bixby/Settings selection

Reason: six optional S22 patches fail on actual BZI6, and the search/key arrays
changed. Files: `rom/scripts/utils/common_utils.sh`,
`rom/scripts/internal/apply_modules.sh`, `rom/scripts/make_rom.sh`,
`rom/scripts/internal/verify_exynos9810_boot_contract.sh`, release-default
verifier, M1 base profile, `rom/unica/mods/settings/customize.sh`,
`rom/unica/mods/settings/a376b_search.py`, new
`rom/unica/mods/settings/a376b_android17_boot_limits.py`, and the existing
Exynos9810 stack/finalization customizers; patch/series/lock.

A narrowly scoped predicate is used only at individually reviewed branches.
Skip five unsupported optional behavior patches plus Bixby remap and remove
the matching six XML controls; retain other options. Keep key703 WINK. Wire
existing Face omission into M4 make/boot/release selection while keeping
FaceManager's feature and other HALs. Do not broadly extend M2 Knox/other skips.
Pin BZI6 search factory/base/mobile, retain211 donor registrations and add4
UN1CA providers. Pin the Intelligence constructor, retain36 keys including
Buds and append UN1CA in a new local instead of overwriting v36.

Verification: six original dry-runs all fail with saved exact diagnostics;
actual dispatcher skips only those six in selected scope. BZI6 source/negative/
DEX/search gates pass; actual AZH4 control search output matches byte for byte.
Actual keylayout finalizer and pre-build Face branch fixtures pass; shared
Face cleanup is idempotent and preserves fingerprint. Compiled XML/keylayout
M4 checks are wired but full Settings/work output is pending. Evidence:
settings-limits-test.txt, search-m2-regression.txt, boot-selection-test.txt and
optional-original-preflight.txt. Rollback: remove0127 and restore its eleven
files to base+0126. No device action or new kernel build occurred.

## Patch0128 — Separate Android17 donor configuration and selection cache key

Reason: the BZI6 input must have its own registry/config and cache identity;
never use M2-only flags/locks or overwrite AZH4. Files: new
`rom/unica/configs/essi_a376b_android17.sh`, `rom/scripts/make_rom.sh`;
patch/series/lock. Metadata comes from the recorded BZI6 audit, including
SDK37/shipping36/board34, source features and dynamic SIM count=none. Unknown
Bluetooth metadata stays explicit with a new-consumer guard. Wrong nonempty
source suffix rejects. Donor hash now includes each M4 runtime/UI/fixed choice.

Verification: actual gen_config runs in isolated output folders M2→M4→M2,
matching source cache markers and identical M2 values on return; original
rom/out/config.sh unchanged. All three M4 UI/fixed flags reset through the
actual base profiles. Actual donor-hash expression changes for each of four
M4 runtime/UI selections. Wrong suffix/new unknown-metadata consumer reject.
Evidence: source-selection-test.txt. This does not create a ready kernel/runtime
profile or claim complete work/cache switch testing. Rollback: remove0128,
restore make_rom to base+0127 and delete only the new donor config; do not remove
firmware caches or retained artifacts.

Checkpoint0128: all118 patches replay from pinned2b25ae6 to101 byte-identical
text files;114 earlier hashes unchanged. Evidence:
logs/20261026-m4-profile-preflight/replay-through-0128.txt. Shell syntax,
Python/JSON and whitespace checks pass. Full M4 build/image/runtime gates remain.
See docs/m4-source-selection.md for scope and remaining work.

## Patch0129 — Independent BZI6 native32 lock, staging and real-image gate

Reason: Android17 runtime APEX differs from AZH4 and must not reuse its lock or
cache; source-only checks cannot establish retention of bytes/labels in ext4.
Affected files: `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/`
`a376b_native32.py`, new `a376b_android17_native32.lock.json` and `customize.sh`;
`rom/scripts/internal/verify_exynos9810_release_defaults.py`,
`rom/scripts/internal/verify_exynos9810_images.sh`, and
`rom/target/star2lte/profiles/m1-g965f-no-ksu.sh`; patch/series/source lock.

The existing builder preserves every A37 payload file/symlink and adds only
17 native32 runtime paths;48 pinned platform32 libraries and the radio wrapper
keep their existing hashes. BZI6 gets its own input/output hash and cache.
M4 requires exact reviewed donor/F/SDK/target/ext4/no-KSU scope, rejects mixed
M2 flags, and uses app64/zygote64. M1/M2 base selection clears the M4 runtime
flag. Existing restore, release and ext4 entrypoints invoke the adapter.
The image gate checks50 locked files, modes/owners/labels and app64/linker state.

Verification: exact reproducible BZI6 APEX b6b559bf…5583cdaf, AVB, containerV3,
4KiB alignment and45 payload metadata paths pass. Actual M4 apply from empty
cache, idempotence,10 scope negatives and wrong-APEX rejection pass. Actual
AZH4 staging is byte-identical to the old helper. Real restore branches test
correct48 metadata rows for M2/M4. Actual image entrypoint passes a private
ext4 fixture; changed mode/SELinux label reject. The first image fixture lacked
lost+found metadata and was corrected before the complete passing run.
119 patch replay /102 matching text files,118 earlier hashes unchanged; shell,
Python AST/JSON and whitespace pass. No complete M4 ROM/profile or runtime test.
Full evidence: docs/m4-native32-runtime.md and logs/20261026-m4-runtime.

Rollback: remove0129 and its series/source-lock entry, restore the five existing
text files to base+0128 and delete only the new native32 lock. Do not remove
firmware caches or retained ROMs. M2 native32/runtime locks and selected kernel
are unchanged; runtime exceptions/camera/full boot/vendor/image checks remain.


## Patch0130 — isolate BZI6 donor-native Android17 runtime and image gates

Reason: keep Android17 A37 camera/audio/graphics/Tethering ABI paired, avoid
Android16/S22-only native exceptions, restore the exact donor vendor_kg account,
and persist aconfig metadata after vendor baseline restore. The same checkpoint
adapts the mandatory ADB/Auto Blocker defaults to BZI6's actual AdbService
lifecycle without bypassing stored user choices or changing donor UI. The A37
BZI6 source has no vendor aconfig directory, so four empty v2 vendor tables are
generated under a separate strict M4 lock and checked in packed ext4 images.

Affected files: new
`rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/`
`a376b_android17_runtime_compat.py` and `.lock.json`; existing boot-restore
`customize.sh`, `a376b_metadata_bind.py`, finalization `customize.sh`,
`rom/scripts/internal/verify_exynos9810_release_defaults.py`,
`verify_exynos9810_images.sh`, new `verify_m4_runtime_image.py`, and existing
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/adb_defaults.py`;
source series/hash lock and M4 documentation.

Verification: locked BZI6 marker/hash and aconfig-v2 tests; runtime staging and
verification idempotence; wrong camera replacement, UID/GID collision and
unknown vendor table negatives; M2 generated init/file-context byte comparison;
real128MiB ext4 SYSTEM+VENDOR fixture with wrong mode/SELinux rejection; Python
AST and shell syntax; actual BZI6 Apktool source decode, patch and rebuild plus compiled ADB
verification, retained Rampart/SecSettings/SUW/observer key actions, and byte-identical
M2 ADB output; replay of all120 patches from pinned commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1` yields105 matching files and keeps
all119 earlier patch hashes unchanged.

Limits: no M4 runtime profile, complete ROM, flash or boot. This does not prove
aconfig, Tethering/BPF or camera/audio/graphics HAL runtime. Kernel, Hybrid
Vendor, radio and hardware blobs remain selected as before. Rollback removes
0130 and restores the source tree by replaying through0129; retained packages
and firmware caches are not touched. Full report:
`docs/m4-native32-runtime.md`.



## Patch0131 — add scoped G965F M4 Android17 profile

Reason: the BZI6 native32/runtime/image gates were ready for profile-level
selection, but M2 flags, donor suffix and the reviewed M4 UI/boot choices needed
a separate F/no-KSU profile. The target config checks the donor suffix before
loading the donor registry, so the M4 profile pins `A376BXXU5BZI6` up front.

Affected files: new
`rom/target/star2lte/profiles/m4-g965f-a376b-android17-no-ksu.sh`;
`patches/fork/0131-add-scoped-g965f-m4-android17-profile.patch`, series and
source hash lock; M4 plan/source/dispatch/repo-map/status documentation and logs.
The profile sources the existing pinned M1 F/no-KSU DS-ACK prebuilt, sets M4
source/runtime/metadata flags, selects only reviewed M4 fixed-resolution, donor
Wi-Fi UI, Face omission and Settings boot-limit options, and keeps M2 flags false.

Verification: profile shell syntax and pinned kernel hash; actual isolated fork
`gen_config_file.sh` M2→M4→M2; AZH4→BZI6→AZH4 cache markers; exact M4 SDK37/
shipping36/board34/target-board33/ext4 config; M2 output equality after switch
back; M4 flags reset; original `rom/out/config.sh` hash unchanged. Exact initial
preflight error and corrected result are recorded under
`logs/20261026-m4-profile-0131/`. Rollback removes0131 from `series` and
`sources.lock.json`, deletes only the M4 profile/patch, then replays through0130.
Limits: no full work-tree/compiled-cache switch, clean ROM build, flash, or Android17
runtime test yet.


## Patch0132 — retain the BZI6 Android17 NFC APEX for M4

Reason: the first clean M4 build reached the Exynos9810 NFC adapter and stopped
because its Android16-only guard expected
`system/priv-app/NfcNci/NfcNci.apk`. Actual BZI6 SDK37 instead contains
`com.android.nfcservices.capex`, with the version17 `com.android.nfc`
`NfcNciApex.apk` inside its APEX payload. Keep that donor-native NFC package and
pair it with the existing S9+ S3NRN82 HIDL stack. This is a static interface
match only; runtime NFC support still needs service/tag logs.

Affected files: existing
`rom/unica/patches/exynos9810_device_stack/customize.sh` adds a strict M4/BZI6/
SDK37 APEX branch while keeping the Android16 path for M1/M2; new
`rom/unica/patches/exynos9810_device_stack/nfc/a376b_android17_nfc.lock.json`
pins exact BZI6 capex, embedded app/JNI, and `SecNfc.apk` hashes; existing
`rom/scripts/internal/verify_m4_runtime_image.py` requires the locked capex and
Samsung NFC app in the final SYSTEM ext4 image and checks bytes,
mode/owner/SELinux metadata; the NFC README, series/source lock and M4 status
docs record the scope and evidence.

Verification: `aapt2` reports package `com.android.nfc`, version17,
compile/target SDK37 and minSDK36; capex, embedded APK and JNI match their lock
hashes. `SecNfc.apk` is package `com.samsung.android.nfc` v17 and statically
references both Samsung HIDL and AIDL `ISehNfc`; the target S3NRN82 service
advertises Samsung HIDL 2.0 only, so runtime path/UI support remains unknown.
The final-image gate accepts exact donor inputs and rejects a mutated work
`SecNfc.apk`. All122 ordered patches replay on pinned fork commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1`; the four 0132 source files match
the replay byte-for-byte. Shell/Python/JSON/whitespace checks pass. Clean
attempt01 failed before image creation at the old Android16 path; clean
attempt02 and packed-image verification remain pending. Evidence:
`logs/20261026-m4-nfc-0132/nfc-source-audit.json`,
`logs/20261026-m4-nfc-0132/replay-local-0132-success.txt`, and
`logs/20261026-m4-profile-0131/m4-build-attempt01.txt`.

Rollback: remove0132 from series/source lock, restore the affected source paths
to replayed0131, and delete only the new NFC lock. Do not remove BZI6 firmware,
M1/M2 artifacts, target NFC blobs, kernel or vendor inputs.


## Patch0133 — keep the BZI6 Android17 camera app for M4

Reason: attempt02 cleared the earlier NFC guard, then stopped at
`logs/20261026-m4-nfc-0132/m4-build-attempt02.txt` with
`(unica/patches/exynos9810_device_stack/customize.sh:4562:_EXYNOS9810_PATCH_ONEUI8_CAMERA_APP) PackageUtil.smali not found in SamsungCamera.apk`.
The M4 donor APK is the hash-locked BZI6 Android17 package; its decoded
`PackageUtil.smali` path is absent. The existing finalization branch already
retains the pinned donor camera app/server/runtime set, so the old One UI8
Smali adapter must not decode or rewrite the M4 APK.

Affected files: existing
`rom/unica/patches/exynos9810_device_stack/customize.sh` adds an early return
only for the exact A37 Android17/M4/star2lte/G965F/SDK37 runtime profile and
rejects an out-of-scope Android17 selection; M1/M2 paths keep the prior One UI8
patch behavior. `patches/fork/series` and `sources.lock.json` add the new
ordered patch and SHA. Documentation and
`logs/20261026-m4-nfc-0132/camera-source-audit.json` record the source evidence.

Verification: BZI6 `SamsungCamera.apk` SHA256
`21a830e6d2f4d73456f6837a73edf8170ddc84222ff710b71fb1318ae7fc61c1` matches
the existing M4 runtime lock; the full donor marker/hash/aconfig source verifier
passes. A direct dispatch fixture confirms the exact M4 profile returns before
`DECODE_APK`, and a wrong M4 milestone is rejected. `bash -n` passes. Applying
0133 after the successful 122-patch replay passed dry-run and byte comparison;
the complete 123-patch state is recorded in
`logs/20261026-m4-nfc-0132/replay-local-0133-success.txt`. A fresh clean build
is next; camera hardware behavior remains unverified.

Rollback: remove0133 from series/source lock and restore only the added guard in
`customize.sh`. Do not replace the BZI6 donor camera app/server or change the
Hybrid Vendor, kernel, radio, modem or protected partitions.


## Patch0134 — retain the BZI6 Android17 Keystore SP-blob flow for M4

Reason: clean attempt03 reached the existing synthetic-password adapter and
failed at `logs/20261026-m4-nfc-0132/m4-build-attempt03.txt:248`:
`Failed to patch method "createSpBlob(Ljava/lang/String;[B[BJ)[B"`. The actual
BZI6 `SyntheticPasswordManager.smali` has instead
`createSyntheticPasswordBlob(...)` and `unwrapSyntheticPasswordBlob(...)`; its
blob format stores a generated AES key through Android Keystore and wraps/
unwraps the payload with that key. Replacing those donor methods with the old
Android16 software-only bodies would change the blob format and protection
semantics. Keep the donor methods intact for M4; do not claim their Keystore
security or PIN behavior until runtime tested.

Affected files: existing
`rom/unica/patches/exynos9810_device_stack/customize.sh` now requires the exact
M4/BZI6/G965F/SDK37 boot-test profile, checks both donor method signatures and
the Keystore/AES wrapping anchors, rejects the legacy method layout for this
Android17 branch, and returns before the Android16 method replacements. M1/M2
continue through the original two replacements. The ordered patch series and
`sources.lock.json` add patch0134 and its SHA256. `docs/PLAN.md`, M4 status/
dispatch/source-selection/NFC reports, README and `logs/20261026-m4-spblob-0134/`
record the evidence and remaining runtime gate.

Verification: actual decoded BZI6 source has both modern methods, Keystore
`setEntry/getKey`, and `SyntheticPasswordCrypto` AES-key encrypt/decrypt calls;
the two old method definitions are absent. Shell dispatch fixtures pass for
exact M4 (retains donor path without calling legacy replacements), reject a
wrong M4 milestone, and confirm M2 still calls both old replacements. All124
patch entries match `patches/fork/series` order and the SHA lock; 0134 applies
to the already replayed 0133 tree and the customize source matches byte-for-byte.
Attempt04 must confirm the real build enters this branch. M2's KeyStore2 log at
`logs/20261024-m2-integrated-build/logcat-first-adb.txt:6269` says no secure
Keymaster implementation was found, so PIN creation, lock/unlock, SID binding
and the actual security level remain explicit device runtime tests.

Rollback: remove0134 from series and `sources.lock.json`, then restore only this
guard to the replayed0133 `customize.sh`. Preserve all donor inputs and the
legacy M1/M2 code; do not modify kernel, Hybrid Vendor, radio, modem or protected
partitions.


## Patch0135 — adapt BZI6 Android17 Wi-Fi Direct callback preservation for M4

Reason: clean attempt04 passed the NFC, camera, synthetic-password and software
Keymaster steps, then failed at
`logs/20261026-m4-spblob-0134/m4-build-attempt04.txt:106` with
`RuntimeError: expected one Samsung P2P callback registration, found 0`. The
input BZI6 `semwifi-service.jar` SHA256 is
`978ba8cb40f93d76c75552deb67c1492475ce53b462a9ceb31c0fea2f198ab69`; its DEX
contains zero copies of the byte pattern used by the old adapter. The decoded
BZI6 `SemSupplicantP2pIfaceHalHidlImpl.setupIface` still has the Samsung
standard callback registration and then separately registers the Samsung Seh
callback. The old behavior is needed because the standard callback replaces the
framework callback while Samsung's `onDeviceFound()` implementation is empty.

Affected files: `rom/unica/patches/exynos9810_device_stack/customize.sh` adds a
strict `essi_a376b_android17` branch requiring the reviewed M4 G965F/SDK37
profile and runtime-compat flag, then decodes the donor JAR and invokes the
Smali adapter. Existing M1/M2 behavior remains in place. The patcher
`rom/platform/exynos9810/tools/patch_semwifi_p2p_callback.py` uses its existing
exact owner/method/call-site anchors and changes only the standard callback
selection, leaving Seh registration intact. `patches/fork/series` and
`sources.lock.json` add the ordered patch and SHA256. `docs/PLAN.md`,
`docs/m4-plan.md`, `docs/m4-dispatch-ledger.md`, `docs/bugs.md`, `STATUS.md`,
`README.md` and the test log record the decision/evidence.

Verification: the exact M4 fixture passes and confirms standard callback
replacement plus Seh registration retention. A wrong M4 scope is rejected
before mutation. The M2 profile fixture still follows its pre-existing
Android16 Smali route. A mutated callback anchor fails closed without modifying
input. `bash -n` and Python AST checks pass; all125 patch names, ordering and
hashes match the lock. Reversing and reapplying0135 to the current source
reproduces both fork files byte-for-byte. Full output is in
`logs/20261026-m4-p2p-0135/p2p-dispatch-test.txt`. A clean attempt05 build is
still required to validate dispatch in the real fork build.

Rollback: remove0135 from `patches/fork/series` and `sources.lock.json`, then
reverse only this patch from the two fork source files. Keep donor inputs and
all prior patches; do not change kernel, Hybrid Vendor, radio, modem or protected
partitions.


## Patch0136 — allow BZI6 arm64-only stagefright camera path for M4

Reason: attempt05 passed the real P2P branch, then failed at
`logs/20261026-m4-p2p-0135/m4-build-attempt05.txt:926`:
`rom/unica/patches/camera/customize.sh: line 397: abort: command not found`.
BZI6's source tree has no `system/lib/libstagefright.so`; it has only
`system/lib64/libstagefright.so` (SHA256
`f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517`), consistent
with the selected arm64-v8a application ABI. That 64-bit file contains exactly
one old `ro.product.model` key and remains the target of the existing camera
compatibility patch. The fork defines `ABORT`, not lowercase `abort`.

Affected files: only `rom/unica/patches/camera/customize.sh` in the fork source.
For exact M4 G965F/BZI6/SDK37 runtime scope, the camera loop may skip the absent
32-bit file only if the 64-bit file exists. The 64-bit file is then processed by
the existing `HEX_PATCH`. Any missing 64-bit file or out-of-scope Android17
selection uses `ABORT` and returns failure. The existing M2 SDK36 exception is
unchanged. `patches/fork/series` and `sources.lock.json` add the patch and SHA;
M4 reports, status, README, bug log and the test log record the result.

Verification: a fixture copied the actual BZI6 lib64 binary; the M4 path
replaced its single old key and produced one new key. Wrong-profile and missing
64-bit cases failed closed. The M2 legacy missing-ABI fixture passed. `bash -n`,
all126 patch names/order/SHA256 locks, and reverse/reapply byte comparison pass.
Full test output: `logs/20261026-m4-camera-0136/stagefright-dispatch-test.txt`.
Attempt06 passed the actual module dispatch and then exposed the separate
DVFS class mismatch recorded in Patch0137; the packed-image contents still need
verification in attempt09 after the camera/HEVC composition fix.

Rollback: remove0136 from `patches/fork/series` and `sources.lock.json`, then
reverse only this diff from `rom/unica/patches/camera/customize.sh`. Keep the
BZI6 source and all prior patches; do not change the kernel, Hybrid Vendor,
radio, modem or protected partitions.


## Patch0137 — map the BZI6 SDHMS DVFS feature class for M4

Reason: clean attempt06 passed the P2P and stagefright branches, then stopped at
`logs/20261026-m4-camera-0136/m4-build-attempt06.txt:940` with
`(unica/patches/dvfs/customize.sh:74:SMALI_PATCH) Missing BZI6 class descriptor: U1/w.smali`.
The BZI6 source APK SHA256 is
`53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`. A fresh
no-debug-info decode identifies `smali/n5/i0.smali` (`Ln5/i0;`) at SHA256
`39ff200b94cf1e141c3d02aed62709850648d3b914d12f0931c80f02a14afdd6`. Its
initializer overwrites the requested `ssrm_default` with donor-only
`siop_a37x_s5e8845`, which is incompatible with the selected S9+ policy.

Affected file: only `rom/unica/patches/dvfs/customize.sh` in the fork source.
Under exact M4 G965F/BZI6/SDK37/runtime scope, the adapter verifies the source
and decoded-class hashes, source policy and target policy, then uses the
existing `SMALI_PATCH` helper on `smali/n5/i0.smali` and fails if the old forced
literal remains. The AZH4 M2 `smali/o5/w.smali` dispatch is unchanged.
`patches/fork/series` and `sources.lock.json` add the ordered patch and SHA;
`docs/PLAN.md`, `docs/m4-plan.md`, `docs/m4-dispatch-ledger.md`, `docs/bugs.md`,
`STATUS.md`, `README.md` and the test log record the rationale, files and checks.

Verification: actual BZI6 decode fixture changes the one initializer literal.
Wrong M4 scope, modified class hash and modified source APK hash reject before
class mutation; the AZH4 M2 fixture still patches `o5/w`. The fork
`SMALI_PATCH` helper, `bash -n`, all127 ordered patch hashes and
reverse/reapply byte comparison pass. Full output is in
`logs/20261026-m4-dvfs-0137/sdhms-feature-test.txt`. Attempt07 reached the
adapter but exposed an unset source-cache variable; Patch0138 fixes that lookup.
Attempt08 verified this adapter and then stopped in the later media module; see
Patch0139 and verify the final packed output in attempt09.

Rollback: remove0137 from `patches/fork/series` and `sources.lock.json`, then
reverse only this patch from `rom/unica/patches/dvfs/customize.sh`. Preserve
all earlier patches and donor inputs; do not change kernel, Hybrid Vendor,
radio, modem or protected partitions.


## Patch0138 — resolve the BZI6 SDHMS APK through the fork cache helper

Reason: attempt07 reached the patch0137 DVFS guard but stopped at
`logs/20261026-m4-dvfs-0137/m4-build-attempt07.txt:940`:
`BZI6 SamsungDeviceHealthManagerService.apk is missing`. Patch0137 had read
`$SOURCE_FIRMWARE_PATH`, but that variable is local to build script scope and is
unset when the module customization runs. The fork already defines
`GET_FIRMWARE_CACHE_KEY` for this exact purpose.

Affected file: only `rom/unica/patches/dvfs/customize.sh`. Patch0138 derives the
selected donor's cache directory from `SOURCE_FIRMWARE` using the fork helper,
then applies the existing pinned APK-hash check. With the reviewed M4 profile,
the helper returns `SM-A376B_XXV_A376BXXU5BZI6`; the APK at that path hashes to
`53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`.
`patches/fork/series` and `sources.lock.json` add the ordered patch and SHA.
The rationale, file and validation are recorded here and in the M4 status,
README, `docs/PLAN.md`, `docs/m4-plan.md`, `docs/m4-dispatch-ledger.md`,
`docs/bugs.md` and the test log.

Verification: call the actual fork helper under exact M4 profile values, assert
the returned cache path exists and its APK hash matches the pin; `bash -n`, all
128 ordered patch hashes and reverse/reapply byte comparison pass. Evidence:
`logs/20261026-m4-dvfs-0138/cache-key-test.txt`. Attempt08 verified the cache
lookup and SDHMS patch in the real build; final image/package checks remain.

Rollback: remove0138 from `patches/fork/series` and `sources.lock.json`, then
reverse only this diff from `rom/unica/patches/dvfs/customize.sh`. Preserve the
previous cache/hash guard and all earlier patches; do not change kernel,
Hybrid Vendor, radio, modem or protected partitions.


## Patch0139 — compose BZI6 camera metadata and HEVC binary edits

Reason: attempt08 passed the DVFS/SDHMS gate, then failed at
`logs/20261026-m4-dvfs-0138/m4-build-attempt08.txt:1005` with
`RuntimeError: Unreviewed BZI6 libstagefright preimage`. Patch0136's camera
customizer had already changed one `ro.product.model\0` key before the later
legacy HEVC adapter checked the original whole-file donor hash. Source
`libstagefright.so` is pinned at
`f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517`; the
camera-patched intermediate observed in attempt08 is
`005a887d8432a140804e5c6c783bbef9e303219e388884807c9cf785900f7372`.

Affected file: only `rom/unica/patches/legacy/a376b_android17_media.py`.
Patch0139 requires that exact intermediate and exactly one replacement property,
proves that reversing only the property key reproduces the donor hash, then
checks the reviewed HEVC context and replaces the branch with NOP. The combined
output hash is pinned at
`5f59107464897883db56189605bf4290da62ee4ac88f723afb8008911aaf2210`; the
compiled verifier uses the combined hash. `patches/fork/series` and
`sources.lock.json` record the new patch and SHA.

Verification: actual attempt08 intermediate fixture plus the four donor APK
encoder fixtures pass and produce the pinned combined native output. The
untouched donor is rejected, ensuring the media gate cannot silently run before
the camera patch. Python compile, all129 ordered patch hashes and
reverse/reapply byte comparison pass. Log:
`logs/20261026-m4-media-0139/stagefright-composition-test.txt`. Clean attempt09
must verify the real build and packaged image.

Rollback: remove0139 from `patches/fork/series` and `sources.lock.json`, then
reverse only this diff from `rom/unica/patches/legacy/a376b_android17_media.py`.
Keep camera metadata and HEVC edits separately pinned; do not change kernel,
Hybrid Vendor, radio, modem or protected partitions.


## Patch0140 — map BZI6 UsbHostRestrictor message handler

Reason: clean M4 attempt09 stopped at
`logs/20261026-m4-media-0139/m4-build-attempt09.txt:1002` with
`Missing BZI6 class descriptor: com/android/server/usb/UsbHostRestrictor$8.smali`.
The actual BZI6 decoded `services.jar` contains no `$8` class. Its only class
with `handleMessage(Landroid/os/Message;)V` and both `SUNNY_WORK_MODE` and
`RAINY_RESTRICT_MODE` is `UsbHostRestrictor$11.smali`, in
`smali_classes2`; its SHA256 is
`2a833bebbcb297d96747e31f901d6a2d25dbbf81ce12d44f27e9f2d84b7153c1`.

Affected file: only `rom/unica/patches/legacy/customize.sh`. The original `$8`
path remains the default. The `$11` mapping is selected only for the exact
M4/star2lte/SM-G965F/SDK37/A376BXXU5BZI6 profile; any other Android17 scope
fails closed. `patches/fork/series` and `sources.lock.json` add the ordered patch
and SHA256 `b881ddc7bff5a6d97c35c83d28c63dc6e744cc39aaaabcb0648e068f8eede570`.

Verification: actual attempt09 decoded-tree review finds exactly one candidate,
confirms both constants occur once inside the target handler and confirms `$8`
is absent. Patch dry-run/apply reproduces the modified source byte-for-byte;
`bash -n` and all130 ordered patch hashes pass. In attempt10, the fork logged
`Using reviewed BZI6 UsbHostRestrictor handler descriptor: ...$11.smali` and
patched both mode constants in the actual `services.jar` at lines1003–1006.
Evidence: `logs/20261026-m4-usb-host-0140/usb-host-descriptor-review.txt` and
`m4-build-attempt10.txt`. The build then stopped on the following RescueParty
class mismatch; Patch0141 records that separate issue.


Rollback: remove0140 from `patches/fork/series` and
`sources.lock.json`, then reverse only this diff from
`rom/unica/patches/legacy/customize.sh`. Do not change kernel, Hybrid Vendor,
radio, modem or protected partitions.


## Patch0141 — retain the hash-pinned BZI6 Samsung SecRescueParty

Reason: attempt10 passed Patch0140, then stopped at
`logs/20261026-m4-usb-host-0140/m4-build-attempt10.txt:1027`:
`Missing BZI6 class descriptor: com/android/server/RescueParty.smali`. BZI6 has
no AOSP `RescueParty` class; it supplies `SecRescueParty.smali`, SHA256
`7c27e2d00fe9ad358a880004a4a7272b57f592e1204cc9798d09776259e3a716`, and
`SystemServer` registers its observer. That Samsung implementation includes
separate recovery, warm-reboot and app-data erase paths, so the old
`isDisabled()` replacement cannot be inferred as equivalent.

Affected file: only `rom/unica/patches/miscs/customize.sh`. For the exact
M4/star2lte/SM-G965F/SDK37/A376BXXU5BZI6 tuple, the adapter now verifies the
AOSP class is absent, checks the pinned Samsung class and its SystemServer
registration, then keeps the donor code intact. Other profiles still use the
original AOSP `isDisabled()` patch. `patches/fork/series` and
`sources.lock.json` add patch0141, SHA256
`887aa36ca2b2cfda64f86f027fe42bd6f7de602cf2b7b4a3091271e0318e176a`.

Verification: attempt10 decoded-tree review confirms the class hash, expected
entry points, SystemServer reference and `RecoverySystem.rebootPromptAndWipeAppData`
call; patch dry-run/apply byte comparison and `bash -n` pass. Evidence:
`logs/20261026-m4-rescue-0141/rescue-party-dispatch-review.txt`. Clean attempt11
must confirm the strict branch in a full build. Runtime behavior of Samsung
rescue actions on the port remains unverified and must be observed before
relying on recovery behavior.

Rollback: remove0141 from `patches/fork/series` and
`sources.lock.json`, then reverse only this diff from
`rom/unica/patches/miscs/customize.sh`. Preserve donor source, kernel, Hybrid
Vendor, radio, modem and protected partitions.


## Patch0142 — adapt BZI6 vendor-mismatch warning

Reason: Attempt11 lines1060 and1138 show the legacy patch expects
ActivityManagerService lambda6 and an outdated handler body. BZI6 uses
hash-pinned lambda7 and lambda4 classes, with class ID3 dedicated to the
vendor-mismatch dialog.

Affected files: rom/scripts/internal/apply_modules.sh adds an exact
M4/star2lte/SM-G965F/SDK37/A376BXXU5BZI6 dispatch; new
rom/unica/patches/miscs/a376b_android17/0001-disable-bzi6-vendor-mismatch-warning.patch
changes only the BZI6 caller and handler. It removes warning-dialog dispatch
while retaining the warning log, UID alert class ID2 and emergency handler
cases0–2. Patch0142 SHA256:
2711d3dd2f857c5dee57ac55eec27f844dabe0644c5427e0c64881aeb96f6047.

Verification: source hashes, strict branch review, patch apply/content fixture,
Apktool services.jar rebuild, bash -n, and all132 ordered patches replay from
the pinned fork base with affected files matching the working tree. Evidence:
logs/20261026-m4-vendor-warning-0142/. Attempt12 confirmed Patch0142 and exposed the NFC sidecar mismatch, documented in patch0143.
Rollback removes0142 from patches/fork/series and sources.lock.json, then
reverses only its two-file fork diff; no donor firmware or target partitions are
changed.


## Patch0143 — retain BZI6 NFC sidecars for G965F

Reason: Attempt12 log line1039 shows the locked G965F PHN source has no
libnfc_prop_extn.so; libnfc_vendor_extn.so is absent too. The source G965F core
JNI/NCI and Samsung NFC HIDL HAL are present and their DT_NEEDED lists do not
name either sidecar. The work tree already contains the hash-pinned BZI6 donor
sidecars used by the Android17 NFC source selection.

Affected file: only rom/unica/patches/nfc/customize.sh. The exact M4/G965F/
PHN + BZI6/SDK37 branch verifies the target JNI and both donor sidecar hashes,
then leaves those donor files staged only when both G965F sidecars are absent.
Its initial target-SDK29 guard did not match the generated M4 target SDK31; attempt15
exposed this and patch0144 corrects the guard. Partial sidecars or changed inputs
fail closed; other profiles retain the original fork behavior. Patch SHA256:
93dd48c8f59d2ed0e2841f5912f5bb56f6c26b5a4109bdd24a8ac28f26ed0b83.

Verification: source presence/hash and ELF dependency audit; positive exact
profile, wrong-model and mutated-JNI tests; bash syntax; and all133 ordered
patches replay from the pinned fork base with final file matching the work tree.
Logs: logs/20261026-m4-vendor-warning-0142/nfc-target-blob-review.txt,
nfc-sidecar-dispatch-test.txt and series-replay-0143.txt. The patch0143 test had
a stale target-SDK value and did not represent the generated build config.
Rollback removes0143 from patches/fork/series and sources.lock.json, then reverts
only its NFC customizer edit.


## Patch0144 — match generated G965F target SDK in NFC guard

Reason: attempt15 log line1038 still tried to copy the absent G965F
`libnfc_prop_extn.so`. Its generated config sets
`TARGET_PLATFORM_SDK_VERSION="31"` (config line22), while patch0143 required29
and fell through.

Affected file: only `rom/unica/patches/nfc/customize.sh`; the exact G965F PHN /
BZI6 branch now checks target SDK31. It keeps all model, target/source firmware,
donor SDK and blob-hash guards.

Verification: real generated-config value checked; SDK31 positive; wrong-model,
SDK29 and modified-JNI negatives fail closed; `bash -n`; 134 ordered patches
replay from the pinned fork commit and 111 resulting files match byte-for-byte.
Patch SHA256: `092ce3155bede5598cdbc0d2f8324925350864efd4d4dfa29ee0bda13ac11f47`. Evidence:
`logs/20261026-m4-vendor-warning-0142/nfc-sidecar-dispatch-test-0144.txt` and
`series-replay-0144.txt`. Attempt16 confirmed the donor-retention branch at line1038, then reached a separate display gate. Rollback removes0144
from `patches/fork/series` and `sources.lock.json` and reverts the one guard line.


## Patch0145 — compose BZI6 CoreRune display hash after product identity

Reason: attempt16 failed at `m4-build-attempt16.txt:1080` with
`RuntimeError: Unreviewed BZI6 product method: CoreRune.smali <clinit>()V`.
The earlier identity patch logged exact model/device property replacements at
lines1024–1027. The method in the failed build has SHA256
`f383d6643c1d420005df733e8b418cb075077bebfd020c73f14f093249ab22f7`; reversing those two changes yields the original pinned source hash
`56d6e8ff4f638415b65c1b6afedd07bf5df1f163d98997d3d1dc889338ca6931`, confirming the only expected ordering difference.

Affected file: only
`rom/unica/patches/product_feature/a376b_android17_display.json`. The display
adapter now pins the exact intermediate method after identity handling, then
performs the existing fixed-resolution edit. It does not loosen class or other
method checks.

Verification: real attempt16 CoreRune composition; display positive and all15
class/field mutation negatives; ten DEX039 fixture assemblies; compiled
product/display and stale-input negatives; M2 HFR/brightness trace unchanged;
`json.tool`; 135 ordered patch replay from pinned fork base, 111 resulting files
match and previous134 locks unchanged. Patch SHA256: `ea73978222dc24df8027cbe8cfc58d701a9641dda472bf661824478f09b24879`. Logs:
`logs/20261026-m4-vendor-warning-0142/product-display-0145/display-fixed-test.txt`,
`core-rune-composition-review-0145.txt` and `series-replay-0145.txt`. Attempt17
full build pending. Rollback removes0145 from series and sources.lock.json and
restores the prior CoreRune method hash in the display manifest.


## Patch0146 — retain the exact BZI6 Strongbox DeviceID no-op

Reason: attempt17 line1074 reports failure replacing the return value of
`isSupportStrongboxDeviceID()Z`. The exact BZI6 method already returns false,
so the generic patch produces no change. Attempt16 and17 whole-class hashes
match at `2d4eda789aa30c266fa9a3c66a2abec140d18004157ae1d9fd72080928578e5e`.

Affected files: `rom/unica/patches/product_feature/customize.sh` and
`rom/unica/patches/product_feature/a376b_android17_strongbox.sh`. For the exact
M4/G965F/BZI6 configuration only, verify SDK/model/cache scope and the full
class SHA256 before retaining the donor's already-false method. Other source
profiles keep the existing patch behavior; an unreviewed BZI6 class stops.

Verification: actual attempts16/17 decoded class; exact-hash positive fixture;
changed-class, wrong-model and missing-class negatives; `bash -n`; 136-patch
replay matched112 files and preserved the prior135 locks. Attempt18 confirms the
no-op log at line1072. Patch SHA256:
`83d9bfe17ec14d5e8b6184d7d209ef0305ede864f6e422d5fc0e39b6ba1b8927`.
Rollback removes0146 from the fork series and source lock.


## Patch0147 — stage BZI6's relocated Midas moire model

Reason: attempt18 line1087 fails to copy `vendor/etc/midas/moire_detection/`
because BZI6 stores the exact referenced model in `vendor/etc/VslMesDetector/`.
The locked Midas config references `moire_detection/moire_detection.tflite`.

Affected files: `rom/unica/patches/saiv/customize.sh` and
`rom/unica/patches/saiv/a376b_android17_moire.sh`. The helper is limited to
M4/G965F/BZI6 and pins the full Midas config and relocated model hashes before
staging at the referenced path with vendor metadata. Existing AZH4 M2 routing
is unchanged.

Verification: pinned config/model path and hashes; successful staging/metadata
fixture; changed-config, changed-model and wrong-model negatives; `bash -n`;
137-patch replay matched113 files and preserved previous136 lock hashes.
Attempt19 confirms actual staging and folder metadata at log lines1084–1086;
final image inclusion remains to verify. Patch SHA256:
`e92a711ba193a9a9aa945c3a11ffa06b878d02b58afca035a8c349314e75e68c`.
Rollback removes0147 from the fork series and source lock.


## Patch0148 — stage the donor-named BZI6 PhotoEditor model pairs

Reason: attempt19 line1091 fails because the legacy PhotoEditor branch requests
`hs_segmenter.info`, while BZI6 supplies `12-23` HumanSeg and `12-39`
BGPclassifier `.info` files with matching versioned `.tflite` models.

Affected files: `rom/unica/patches/saiv/customize.sh` and
`rom/unica/patches/saiv/a376b_android17_hs_segmenter.sh`. The exact M4/G965F/BZI6
branch verifies all four file hashes, requires each `.info` model path to point
to its paired file, and stages the donor directory with vendor metadata. The
AZH4 M2 and generic legacy routes remain unchanged.

Verification: positive directory staging and exact file hashes; changed-model
and wrong-model negatives; shell syntax; 138-patch replay matched114 files and
preserved previous137 locks. Attempt20 will verify actual workdir/image entries.
Patch SHA256: `810f22ea278fe2fc24b48c6597be187467dfd0f7f318e66d40d707d1b0679b10`.
Rollback removes0148 from the fork series and source lock.


## Patch0149 — adapt the BZI6 ImageCropper paths

Reason: attempt20 line1097 asks for legacy
`system/system/saiv/smartcropping_2.0/db/smartcrop_saliency_deploy.prototxt`,
which BZI6 does not ship. BZI6 enables `IMAGE_CROPPER_VERSION=V1` and stores
`sce_detector_cnn.tflite` under `system/saiv/imageCropper`; the S9+ vendor
baseline supplies its separate old-path SCE model.

Affected files: `rom/unica/patches/saiv/customize.sh` and
`rom/unica/patches/saiv/a376b_android17_image_cropper.sh`. The exact M4/G965F/BZI6
branch pins System/Vendor floating features, donor model and paired library,
checks the exact G965F legacy model hashes before removing them, stages the donor
System model at its source path, and verifies that the pinned star2lte vendor
SCE model remains unchanged. Other donor routes, including AZH4 M2, remain on
the existing fork branch.

Verification: positive staging/retention fixture; wrong-model, modified donor
model and modified vendor baseline negatives; `bash -n`; 139 ordered patches
replay from pinned commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`, 115
resulting files match the work tree and all previous138 lock hashes remain
unchanged. Patch SHA256:
`7c7a8e940412b7c4ddc8a76ba2bc141ea767a07fd9ce6be51c285a7ebd722307`.
Attempt20 confirms the exact missing path at line1097; attempt21 must verify the
actual final image. Evidence: `logs/20261026-m4-vendor-warning-0142/`.
Rollback removes0149 from the fork series and source lock.


## Patch0150 — resolve BZI6 Battery Health controller in classes3

Reason: attempt22 failed at `m4-build-attempt22.txt:1164` because the BZI6
`BatteryRegulatoryPreferenceController` moved to `smali_classes3`. The
first-use-date controller inherits its availability method and has no local
override, so patching the child method as well is invalid.

Affected file: `rom/unica/mods/batteryhealth/customize.sh`.

Verification: decoded pinned BZI6 `SecSettings.apk` SHA256
`49dcc59150efb0d38ecc93ec2ffe675eeb6f4b98d39ae6d0f3f970d44b3c4693`; exact
base-class resolution, inherited child method and reject-duplicate/missing
method fixtures pass. Attempt23 passed the real build gate at lines1161–1163.
Rollback removes0150 from the fork series and source lock. Patch SHA256:
`71a57e2cecdf64c4f8a4068d9b35b5e682517d86773e7976af847c7ddb8a162a`.


## Patch0151 — retain BZI6 donor PackageBlockListPolicy

Reason: attempt23 showed that the old optional Package Block List mod replaces
donor LDU policy and lifecycle hooks, but its hunks do not match BZI6. The
M2 decision was to keep donor policy when this optional Settings feature cannot
be safely adapted.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: skip is scoped to M4 / G965F / star2lte / SDK37 / board API33 /
BZI6 cache and local donor ID; wrong target and wrong milestone fail closed,
and unrelated modules keep normal validation. Replayed patch0150–0151 on the
pinned fork baseline; sources.lock hashes validate. Attempt24 is the clean
full-build check. Rollback removes0151 from series and source lock. Patch SHA256:
`314a3973afeb2de3ddab0e8a33c229b00db16cc62adf0bf893fc19a956357a4a`.


## Patch0152 — retain BZI6 donor FOTA suggestion behavior

Reason: attempt24 line1179 shows the optional legacy UN1CA Updates suggestion
patch expects `Settings$FOTASuggestionActivity` in `smali_classes2`; BZI6
relocates that activity to `smali/`. The shortcut is optional, so the M4 test
keeps donor Settings behavior instead of applying an unreviewed Android17 edit.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board33/BZI6 dispatch skips only
this shortcut; wrong model and milestone fail closed; the existing AZH4 route
and unrelated patch route pass. Shell syntax and patch replay pass, and all142
source-lock patch hashes validate. Clean attempt25 is the full-build check.
Rollback removes0152 from series and sources.lock. Patch SHA256:
`9f507af954b4c190931dddb736ccad9962ff616b459e0ab9fa9833f528d4a630`.


## Patch0153 — retain BZI6 donor Remote management tile

Reason: attempt25 line1181 reports missing
`TopLevelRemoteSupportPreferenceController.smali` in BZI6 SecSettings. The
optional CSC customization hides this Settings tile; M4 keeps donor visibility
until the relocated/removed control is reviewed.

Affected file: `rom/unica/mods/csc/customize.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board33/BZI6 branch skips only this
customization; wrong model/milestone fail closed; existing AZH4 behavior still
executes all three legacy CSC patches. Shell syntax, isolated source replay and
all143 source-lock hashes pass. Clean attempt26 is the full-build check. Rollback
removes0153 from series and sources.lock. Patch SHA256:
`929f1fe75aa0cb105cc3a3b14c96d1dff5280196a15cad3004fc318389ce844b`.


## Patch0154 — retain BZI6 donor network-speed Settings behavior

Reason: attempt26 line1190 reports missing
`NotificationsItem.smali` in the old SecSettings patch. The optional CSC feature
patch is not adapted to BZI6; preserve its donor network-speed behavior.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lite/SDK37/board33/BZI6 branch skips only the
legacy SecSettings patch; wrong model/milestone fail closed; AZH4 and unrelated
patch routes remain unchanged. Shell syntax, five dispatcher fixtures, replay,
and all144 source-lock hashes pass. Clean attempt27 is the full-build check.
Rollback removes0154 from series and sources.lock. Patch SHA256:
`a94385791333cf4b179b30996875bad86e5578bc4d21bbc6bb5ec7308adcf271`.


## Patch0155 — retain BZI6 donor Secure Folder key behavior

Reason: attempt27 partially applies the optional CSC Secure Folder quick-switch
patch, then fails at `FunctionKeyUtils.smali:589` because the old BZI6 method
body differs. Keep donor key behavior and avoid a partial Settings change.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board33/BZI6 branch skips only the
`0003-Enable-quick-switch-to-Secure-Folder-feature.patch`; wrong model/milestone
fail closed, AZH4 routes stay unchanged, unrelated paths still apply. Shell
syntax, five dispatch fixtures, patch replay and all145 source-lock hashes pass.
Clean attempt28 is the full-build check. Rollback removes0155 from series and
sources.lock. Patch SHA256:
`6ecf4ccbe17e36de0e5dbe0a5d08a2467d47e3891a794f620433dfdd643b7a70`.


## Patch0156 — retain BZI6 donor SystemUI network-speed behavior

Reason: attempt28 lines1187–1203 show the paired SystemUI status-bar network-speed patch fails at `BasicRune.smali:644` because BZI6 donor code differs. Together with patch0154, retain donor network-speed behavior in both Settings and SystemUI for this exact M4 target.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board API33/BZI6 route skips only the paired SystemUI patch; wrong model and milestone fail closed; M2 AZH4 and unrelated patch routes stay active. Shell syntax, five dispatcher assertions, all146 ordered patches replay from pinned base, dispatcher byte-identical, and all146 lock hashes pass. Clean attempt29 is the full-build check. Rollback removes0156 from series and sources.lock. Patch SHA256: `3c6034e20e59ba039562a83a68eef1bce459bfbf00ea9022ac1bd29b96c15b7c`.


## Patch0157 — retain BZI6 donor Smart View tile behavior

Reason: attempt29 lines1188–1214 show the optional Smart View visibility patch fails at `QpRune.smali:308` because BZI6 donor initialization differs. Keep donor tile behavior for this exact M4 target while the feature is unported.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board API33/BZI6 route skips only the Smart View visibility patch; wrong model/milestone fail closed; AZH4 and unrelated routes remain active. Five dispatcher assertions pass; all147 ordered patches replay from the pinned base, dispatcher matches byte-for-byte, all147 source-lock hashes pass. Clean attempt30 is the full-build check. Rollback removes0157 from series and sources.lock. Patch SHA256: `80caaebedafdb5686d69c25bf4bc092efe503219ee6a2ad75024cd2056ec22e7`.


## Patch0158 — retain BZI6 donor Dual Messenger behavior

Reason: attempt30 lines1188–1398 show the optional all-apps Dual Messenger patch fails against BZI6 DAAgent in `DualAppProvider.smali:512`, `WhiteListApps.smali:4`, `DualAppIntentReceiver.smali:33`, and `DAUtility.smali:1947`. Retain the donor allowlist instead of applying partial changes.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board API33/BZI6 route skips only the optional all-apps patch; wrong model/milestone fail closed; AZH4 M2 and unrelated routes remain active. Five dispatcher assertions pass; all148 ordered patches replay from pinned base, dispatcher matches byte-for-byte, and all148 source-lock hashes pass. Clean attempt31 is the full-build check. Rollback removes0158 from series and sources.lock. Patch SHA256: `67b84c000eb9bd553ca8975538f1f65c6410588bc27c971502f87d8f2da5a32f`.


## Patch0159 — retain BZI6 donor Knox stack

Reason: attempt31 line1264 reports `Missing BZI6 class descriptor: com/android/server/knox/dar/ddar/ta/TAProxy.smali` after DeKnox removed donor Knox components. Retain the complete donor Knox stack instead of applying a partial DAR/Knox replacement.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/board API33/BZI6 route skips only DeKnox; wrong model/milestone fail closed; AZH4 route and unrelated modules remain active. Five module-dispatch tests pass; all149 ordered patches replay from pinned base, dispatcher matches byte-for-byte and all149 source-lock hashes pass. Attempt32 later passed this gate. Rollback removes0159 from series and sources.lock. Patch SHA256: `c11c57d7f086917a290d0cdfd256b89cfa16bcab8e56111f2fcdd2993d41fa8a`.


## Patch0160 — match BZI6 device image property access

Reason: attempt32 failed at `m4-build-attempt32.txt:1195` with `PATTERN_NOT_FOUND`. BZI6 `DeviceImageManager$1.smali` uses `const-string` and places `SystemProperties.get` directly after it; the prior device image matcher required `const-string/jumbo` and blank-line spacing. Accept both equivalent Smali forms while continuing to pin the real G965F code `SM-G965FZKDEUX`.

Affected file: `rom/unica/mods/device_image/customize.sh`.

Verification: the exact BZI6 SecSettings APK (SHA256 `49dcc59150efb0d38ecc93ec2ffe675eeb6f4b98d39ae6d0f3f970d44b3c4693`) decodes with the fork Apktool; actual BZI6 matcher, legacy jumbo/spacing fixture, and changed-accessor negative pass. Shell syntax passes. All150 ordered patches replay from pinned fork commit `2b25ae6`, and final customizer is byte-identical to the active source. Attempt33 confirms the matcher fix and advances to the next module before a separate optional-module failure. Rollback removes0160 from `patches/fork/series` and `sources.lock.json`, then restores the previous customizer. Patch SHA256: `0cba69691c4992f3d774c56f5d1d252f71ff68dbf3f7a6d699babbc1cbb5625a`. Evidence: `logs/20261026-m4-vendor-warning-0142/test-device-image-0160.txt`, `series-replay-0160.txt`.


## Patch0161 — retain BZI6 donor Hide My Applist behavior

Reason: attempt33 lines1232 and1245 report failed legacy Hide My Applist hunks in BZI6 `ComputerEngine.smali:9954` and `ActivityStarter.smali:5341`, with build failure at line1248. Avoid a partial framework integration and preserve donor behavior for this optional mod.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/target SDK31/board API33/BZI6 route retains donor behavior; wrong model and milestone fail closed; AZH4 M2 behavior and unrelated module processing remain unchanged. Five dispatch assertions and `bash -n` pass. All151 ordered patches replay from pinned commit `2b25ae6`; dispatcher matches byte-for-byte. Attempt34 confirms this donor-retention route and advances to the next optional module before a separate Keybox layout failure. Rollback removes0161 from `patches/fork/series` and `sources.lock.json`, then restores the prior dispatcher. Patch SHA256: `8a700afc2086567d35cbf4bebb4e93e3c6789f08cb8d9091a9f40f77c6accaf5`. Evidence: `logs/20261026-m4-vendor-warning-0142/test-hma-0161.txt`, `series-replay-0161.txt`.


## Patch0162 — retain BZI6 donor key-attestation behavior

Reason: attempt34 lines1204 and1206 show the optional Keybox imitation module expects missing framework `IKeystoreSecurityLevel$Stub$Proxy.smali` and `IKeystoreService$Stub$Proxy.smali`; line1226 records build failure. Avoid a partial key-attestation integration and keep donor behavior.

Affected file: `rom/scripts/internal/apply_modules.sh`.

Verification: exact M4/G965F/star2lte/SDK37/target SDK31/board API33/BZI6 route retains donor behavior; wrong model/milestone fail closed; AZH4 M2 route and unrelated module processing remain unchanged. Five dispatch assertions, `bash -n`, all152 ordered patch hashes and full replay pass; dispatcher matches byte-for-byte. Attempt35 was interrupted before module execution; clean attempt36 is the full-build check. Rollback removes0162 from `patches/fork/series` and `sources.lock.json`, then restores the previous dispatcher. Patch SHA256: `22d1381ec115ded70e5094e3da678fc828bd2f1c97d9364a244de8f1ec4436a9`. Evidence: `logs/20261026-m4-vendor-warning-0142/test-keybox-0162.txt`, `series-replay-0162.txt`.


## Patch0163 — Retain BZI6 donor Knox and attestation behavior

Reason: attempt36 lines1250,1262,1296,1313,1329,1341 show incompatible legacy KnoxPatch ICD/attestation hunks in DevicePolicyManagerService, DarManagerService, UserManagerService, AttestParameterSpec, AttestationUtils and SemSsdidManagerService. Earlier steps already remove WSM and insert framework hooks; retain the complete donor stack rather than a partial optional integration.

Affected: scripts/internal/apply_modules.sh.

Verify: exact M4/G965F/star2lte/SDK37/target SDK31/board API33/BZI6 route skips only KnoxPatch; wrong model/milestone fail closed; AZH4 M2 and unrelated modules unchanged; bash syntax, hash lock/replay and clean build.

Rollback: remove0163 from patches/fork/series and sources.lock.json, then restore the prior dispatcher.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-knoxpatch-0163.txt` PASS. SHA256: `cb480c8cc1c45736103e1128b94b38717fb5b2cac8ec8a750dc160955eb3a339`. Full clean build and image gates remain pending.


## Patch0164 — Adapt BZI6 paired live wallpaper default

Reason: tail-attempt36/s9_infinity_Wallpaper.txt:27 reports `unknown paired factory-live default branch`. Actual BZI6 setFactoryDefaultWallpaper keeps the null component in p4 and omits the old branch-local p0 assignment. Pin the complete donor method, then use saveDefaultLiveWallpaperData on only that paired-live branch as the existing Android16 fix does.

Affected: unica/mods/s9_infinity_Wallpaper/source/patch_factory_live_default.py.

Verify: actual BZI6 class positive/idempotence, changed-method negative, legacy Android16 fixture and DEX assembly; hash-lock/replay; clean compiled artifact checks remain required.

Rollback: remove0164 from series/source lock and restore the previous helper. No payload, vendor, radio or kernel change.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-wallpaper-0164.txt` PASS. SHA256: `31c2f9d06dc5c572b47172d3d4af09ddc25daefb1190b93eed4b01739dbeaefd`. Full clean build and image gates remain pending.


## Patch0165 — Review absent BZI6 legacy OTA Settings method

Reason: tail-attempt36/settings.txt:6 reports missing isOTAUpgradeAllowed(Context) in SoftwareUpdateUtils. The unique BZI6 class has no such method. Hash-pin this reviewed class and retain donor navigation; do not invent an Android17 method or bypass the separate mandatory ADB/Auto Blocker checks.

Affected: unica/mods/settings/customize.sh.

Verify: exact decoded BZI6 class hash/absent method, changed-preimage and wrong-scope rejection, legacy branch retained; actual module continuation, shell syntax, source-lock/replay and clean compiled output.

Rollback: remove0165 from series/source lock and restore the previous customizer.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-ota-0165.txt` PASS. SHA256: `7d0a6e1955da36897e9747502a11bab26035d8facb51bb825200a24e0107cde6`. Full clean build and image gates remain pending.


## Patch0166 — Retain reviewed BZI6 optional donor behaviors

Reason: actual tail-attempt36 diagnostics fail in paradigm EAD display hunks (:34/46/59/71/82), pif old AndroidKeyStoreSpi DEX path (:8), setupwizard missing S2/f (:4), and tweaks donor SystemUI layouts (:20/38/50). settings-after-0165.txt:171 also fails the optional MultiSound five-app expansion. Preserve donor behavior coherently for these optional features in the initial M4 boot build; mandatory ADB/Auto Blocker and supported Settings integration remain active.

Affected: scripts/internal/apply_modules.sh.

Verify: exact M4/G965F/BZI6 module/patch dispatch, wrong model/milestone rejection, M2 and unrelated route regression, source-lock/replay and clean build. Four optional modules plus one specific MultiSound patch only.

Rollback: remove0166 from series/source lock and restore the prior dispatcher; port each optional feature separately.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-optional-0166.txt` PASS. SHA256: `3ec831c24f2e6aad833a62270a88d58de8b55e4395ab8b63a066856045f04329`. Full clean build and image gates remain pending.


## Patch0167 — Port BZI6 FloatingFeature primary getter hooks

Reason: settings-static-after-0166.txt:21 fails the legacy default-value getter body. Actual BZI6 getBoolean(String,Z), getInt(String,I), getString(String,String) delegate to their primary getters. Hook only the three hash-pinned primary getters and preserve overload delegation; retain the upstream FloatingFeatureHooks class unchanged.

Affected: scripts/internal/apply_modules.sh and unica/mods/settings/a376b_android17/0001-Introduce-BZI6-FloatingFeatureHooks.patch.

Verify: actual BZI6 class preimage, primary hook calls, unchanged overloads, changed-preimage/wrong-scope negatives, helper class and DEX assembly; source-lock/replay and clean compiled output.

Rollback: remove0167 from series/source lock and restore previous dispatcher; no kernel/vendor/radio changes.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-floatingfeature-0167.txt` PASS. SHA256: `b6ba7ad20028b83377f3753bee84aad64fb9688ade6545ca69c0e630350da79f`. Full clean build and image gates remain pending.


## Patch0168 — Keep M4 optional module controls and defaults truthful

Reason: settings-rest-after-0167.txt:32 fails optional ASKSManagerService hooks. Preserve donor ASKS and hide its control. M4 HMA/keybox/PIF modules are omitted, but their seven controls and the inherited PIF=true default remained; remove only those controls, seed false in final property files after vendor restore, and make compiled release checks reject stale hooks/controls. Mandatory ADB/Auto Blocker verification is unchanged.

Affected: scripts/internal/apply_modules.sh; scripts/internal/verify_exynos9810_release_defaults.py; unica/mods/settings/a376b_android17_boot_limits.py; exynos9810_finalization/customize.sh.

Verify: source XML positive/negative fixtures, actual property copies and scope checks; M2 selection unchanged; compiled output guards, hash-lock/replay and clean build.

Rollback: remove0168 from series/source lock and restore previous files.

Actual fixture/dispatch log: `logs/20261026-m4-vendor-warning-0142/test-controls-0168.txt` PASS. SHA256: `8f2c4ff6b46dc49a67e1eea3fc2b34fc0042b6a582e71ffdfd726f54bbbfd690`. Full clean build and image gates remain pending.


## Patch0169 — Resolve source cache inside vendor restore module

Reason: boot-restore-preflight-0168.txt:126 reports BZI6 runtime donor marker mismatch. make_rom computes SOURCE_FIRMWARE_PATH without exporting it, while apply_modules runs as a child process. The restore module therefore passes FW_DIR itself instead of the selected donor root. finalization-preflight-0169.txt:12 proves the same dependency in finalization. Resolve the key locally in both modules with GET_FIRMWARE_CACHE_KEY, including the locked BZI6 suffix.

Affected: unica/patches/exynos9810_device_stack/{exynos9810_boot_restore,exynos9810_finalization}/customize.sh (six runtime donor callers).

Verify: M1/M2/M4 cache selectors, actual BZI6 source_root/hash verification and wrong-cache negative, shell syntax, hash-lock/replay and final-stage/clean build continuation.

Rollback: remove0169 from series/source lock and restore previous module. No build-system replacement or firmware/kernel/radio mutation.

SHA256: `b75f084e8360451c8e9ab33b734eccca1cee267379f53bd1cf6225f4361fda65`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0170 — retain locked bzi6 camera in final ocr reapply

Reason: finalization-preflight-0169b.txt:13 reports missing MakerCallbackManager.smali after the locked Android17 camera has been restored. The Android16 OCR patch must not mutate this different donor camera.

Affected: exynos9810_finalization/customize.sh legacy OCR reapply function

Verify: Actual M4 finalization, wrong-model scope failure, runtime lock verification, shell syntax and full replay; clean image audit required.

Rollback: Remove0170 and restore previous function; no kernel/vendor/radio change.

SHA256: `ae672d851a8cc3fd122691a8e8532a69db1a6fa0a5c13514682843d05e85a829`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0171 — port bzi6 play integrity fix and live settings

Reason: User requests prioritizing Play Integrity Fix. tail-attempt36/pif.txt:8 proves keystore moved DEX; actual services dry-run fails old label context. Adapt reviewed methods while retaining upstream PIF logic and donor fallback permission checks.

Affected: PIF customizer/new BZI6 helper, dispatcher, M4 Settings/defaults and compiled release gates

Verify: Actual BZI6 method preimages; upstream helper equality, DEX assembly, wrong donor/method negatives, compiled hook/control/property gates and clean full build. Device Integrity verdict needs runtime/network testing.

Rollback: Remove0171 to restore donor PIF omission and hidden controls; M1/M2 route and ADB rules unchanged.

SHA256: `9ee7810d5cf4108858f101b139693ca5f55ba20b4b6a016a44b15ebaf74c5216`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0172 — remove stale scloud preinstall config after debloat

Reason: finalization-preflight-0170.txt reports Samsung Cloud support file remains: preinstalled-packages-com.samsung.android.scloud.xml. It only enables installation of the removed optional Cloud APK for FULL/PROFILE users. Delete alongside existing preload triggers, retain SCPM/shared-UID contract.

Affected: exynos9810_finalization/customize.sh final debloat deletion list

Verify: Actual XML inspected; final Cloud absence/shared-UID gates, metadata deletion helper and clean build.

Rollback: Remove0172 to restore previous deletion list.

SHA256: `185b787f2536d2743b68b2b63b271fe45f6ad30b06c1efbc825677ba1eeb254b`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0173 — review bzi6 secure folder configuration desktop check

Reason: finalization-preflight-0172.txt fails legacy LockTypePreferenceData lookup. BZI6 renamed the package and removed isDexOnPcMode entirely: its wrapper uses Configuration fields, with no legacy optional desktopmode-service lookup. Hash-pin and retain that donor implementation.

Affected: exynos9810_finalization/customize.sh Secure Folder desktop compatibility function

Verify: Actual LockTypePreferenceData/DesktopModeWrapperImpl inspected and hash-pinned; exact M4 scope, final-stage continuation and clean build. Secure Folder UI still requires runtime test.

Rollback: Remove0173 to restore previous compatibility route; M1/M2 unchanged.

SHA256: `546b0192e5e74abae6dce01bea56770432fa0327d98b36a06a62f7035dc59417`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0174 — retain bzi6 native brief components

Reason: final-tail-preflight-0173.txt: Now Brief receiver not found in disabled or enabled state. BZI6 removed legacy AiSuggestionAppWidgetReceiver; actual manifest provides new briefsettings activities/providers. Preserve reviewed donor components instead of fabricating an obsolete widget receiver.

Affected: exynos9810_finalization/customize.sh Now Brief optional widget enable function

Verify: Actual decoded manifest hash and full M4 scope; new BriefSettingsProvider/BriefMainSettingsActivity inspected; continuation and clean APK build. New Brief UI requires runtime verification.

Rollback: Remove0174 and restore previous legacy widget route; M1/M2 unchanged.

SHA256: `ac663ef3dc36c1629ff0800b169712701a61e3a831c6bb5434f296781014752d`. Diagnostic logs: `logs/20261026-m4-vendor-warning-0142`; clean full build/image gates still pending.


## Patch0175 — port bzi6 heads up fallback observer

Reason: final-tail-preflight-0174.txt fails removed NotificationInterruptStateProviderImpl$1. Actual BZI6 PeekDisabledSuppressor observer still defaults unset heads_up_notifications_enabled to0. Change only fallback argument to1 using dead v4; keep explicit zero and releaseAllImmediately logic.

Affected: notifications/patch_brief_heads_up.py and verify_brief_rebuilt_artifacts.py

Verify: Hash-pinned real onChange method, unchanged explicit opt-out, idempotence/negative fixture, DEX039 assembly and compiled observer/EdgeLighting gate; legacy tests unchanged.

Rollback: Remove0175 and restore legacy patch/checker. No ADB or hardware/kernel changes.

SHA256: `45d8b70c7db2f42f44f3122abbfdc32d55a406ae6d763b97f5855f13975918c5`. Full169 source replay passed; actual finalization/compiled fixtures in `logs/20261026-m4-vendor-warning-0142`. Clean attempt37/image gates pending.


## Patch0176 — pin native bzi6 apps before s22 only workarounds

Reason: Sequential finalization diagnostics0175 fail removed S22 PhotoPresenter/ObjectCaptureImpl/VizManager/Rubin37 classes and PhotoEditor_AIFull/Moments packages. BZI6 source has native PhotoEditor_Full, Rubin38 and new Smart Suggestions Brief components, without Moments. Preserve full native camera/wallpaper/classifier/database apps pending runtime evidence; do not weaken donor database encryption.

Affected: finalization/customize.sh and verify_bzi6_final_apps.py; six APK input hashes locked

Verify: Actual donor APK hashes, full M4 scope, selectors and required permission/library checks; negative payload/scope fixtures, sequential finalization and clean build. These optional behaviors need M4 runtime tests.

Rollback: Remove0176 and restore previous routes; M1/M2 unchanged.

SHA256: `cded018d40df81923560eeb15df8aacd6d2af034a9d23c1cfe5b93e367aa8b37`. Full169 source replay passed; actual finalization/compiled fixtures in `logs/20261026-m4-vendor-warning-0142`. Clean attempt37/image gates pending.


## Patch0177 — retain native bzi6 mdf and avoid sigpipe false failure

Reason: final-functions-0175 MDF restore claims missing JNI, but actual S22 and BZI6 ELF files contain/export it. strings|grep -q under pipefail can exit141 after an early match. Consume full stream. For M4 restore hash-pinned native BZI6 lib64 MDF with matching framework instead of injecting the S22 two-ABI pair; BZI6 apps64 has no 32-bit MDF input.

Affected: finalization/customize.sh MDF restore and final symbol checks

Verify: Actual source ELF JNI symbol/export and SHA; native64 restore/metadata; explicit pipefail reproduction; final-stage and clean build. M1/M2 restore route retained.

Rollback: Remove0177 and restore previous MDF restore/checks; no kernel/vendor/radio changes.

SHA256: `4fe75f276b0f7bd0c6503bf8f92f09a88bfc8d6e8096b956d11f88478730a842`. Full169 source replay passed; actual finalization/compiled fixtures in `logs/20261026-m4-vendor-warning-0142`. Clean attempt37/image gates pending.


## Patch0178 — preserve bzi6 proc compaction policy input

Reason: final-tail-preflight-0177b.txt fails A17 system_ext proc-compaction genfscon preimage changed. m4-build-attempt36.txt:1114 proves the earlier generic SELinux module deleted this donor label/mapping, conflicting with the reviewed M4 final adapter. Preserve exactly this type on scoped BZI6; final adapter removes the obsolete vendor genfscon and supplies VNDK33 aliases.

Affected: unica/patches/selinux/customize.sh one entry exception

Verify: Exact M4 scope; preserve actual donor type/mapping/genfscon, remove old vendor label using existing helper; full split-policy compilation and clean build mandatory. M1/M2 cleanup unchanged.

Rollback: Remove0178 and restore previous generic policy loop.

SHA256: `26f52fa58a9c95da7eb3f74ae9a8ab342bd2b6ea5c564a1396d4e95309918d46`. Full169 source replay passed; actual finalization/compiled fixtures in `logs/20261026-m4-vendor-warning-0142`. Clean attempt37/image gates pending.


## Patch0179 — omit absent bzi6 otapreopt slot compatibility rule

Reason: selinux-full-preflight-0178.txt fails plat_sepolicy.cil:86108. That appended S22 allow references otapreopt_slot, absent from actual BZI6 types just as on AZH4. Extend the existing exact one-rule removal to scope-checked BZI6, with declaration/count negatives retained.

Affected: finalization/customize.sh embedded policy composition

Verify: Actual absent donor declaration and failing allow line; final adapter plus full split-policy compilation, source replay and clean build. No permissive-domain addition.

Rollback: Remove0179 to restore previous A37 branch; S22 unchanged.

SHA256: `49cd8d1463f6f080f5eab0704ad953480bea55bdaa9c8eb790e7e713f79d7f7f`. Full169 source replay passed; actual finalization/compiled fixtures in `logs/20261026-m4-vendor-warning-0142`. Clean attempt37/image gates pending.


## Patch0180 — preserve locked bzi6 native apks through global rebuild

Reason: Clean attempt37 logs Building/Signing SamsungCamera, Smart Suggestions and Secure Folder even though finalization explicitly retains donor apps. The global rebuild traverses inspection-only decode caches, changing locked APK bytes and Samsung signatures. Guard seven exact native APK paths with hash/scope verification and verify the final raw APKs after all rebuilds.

Affected: scripts/apktool.sh BUILD guard; verify_bzi6_final_apps.py SecureFolder/all selection; compiled release defaults gate

Verify: Actual attempt37 output/input hash comparison, production BUILD skip/payload and wrong-scope negatives; all other APKs including SystemUI/Settings/framework/services use existing rebuild. Re-run full compiled/policy/image gates via fork.

Rollback: Remove0180 and restore previous APK rebuild route; M1/M2 unchanged.

SHA256: `ecdbd97b314780bfebd2b1504ca006515c5e3cf566f41c7acffa18b8e57ad04f`. 171-patch/125-file replay passed; native-APK production guard fixtures passed. Actual staged packaging/image checks pending.


## Patch0181 — verify native bzi6 setupwizard selection

Reason: m4-build-attempt37.txt boot contract fails missing UN1CA SetupWizard navigation-step patch, although user chose retaining donor Setup Wizard UI and0166 skips that whole module for exact M4. Verify the raw BZI6 SetupWizard APK hash/signature preservation instead; all Settings/PIF/ADB and other contract checks remain mandatory.

Affected: scripts/internal/verify_exynos9810_boot_contract.sh SetupWizard check

Verify: Actual clean attempt37 native APK and donor SHA equality; full boot contract and wrong-payload negative; M1/M2 old marker checks unchanged.

Rollback: Remove0181 to restore previous SetupWizard gate.

SHA256: `8eb10ff13118e98e4fa422b57dad8b90190ef7ae4725d4d0d4aa7d45524d608d`. 171-patch/125-file replay passed; native-APK production guard fixtures passed. Actual staged packaging/image checks pending.


## Patch0182 — clear restored bzi6 unsupported camera claims

Reason: m4-package-attempt37-0181.txt:24 rejects vendor ACTION_CLASSIFIER=1,0. Vendor baseline restore also reinstates ARDOODLE=PET_DETECTION,IMG_PICKING, although both implementations are absent in BZI6 and earlier camera selection clears them. Reconcile the two exact reviewed vendor values at finalization end; retain strict final gate and donor GPPM.

Affected: camera/a376b_android17_features.py apply-work and exynos9810_finalization/customize.sh final call

Verify: Actual restored values, implementation absence, XML parse, both-file preflight before writes, idempotence/unreviewed-value negative, final compiled/image gates.

Rollback: Remove0182 to restore previous order; no binary/kernel/radio changes.

SHA256: `1a6addd007aa4a7d25dff3cd57e7c915443bdafddc7e5da0495f0d7917fed977`. Evidence: `logs/20261026-m4-vendor-warning-0142/`; actual staged packaging gates remain in progress.


## Patch0183 — distinguish pinned camera input from omx apk output

Reason: m4-package-attempt37-0182.txt rejects stock camera EncoderFilter: raw camera retention conflicts with already-reviewed OMX application patch. Camera must rebuild only its known legacy encoder change. Keep strict raw input/runtime binary hashes; add explicit post-build camera mode with full encoder-method equality, unchanged embedded native libraries, Jar signature/build-certificate verification, and real ext4 byte/metadata audit. Six unrelated donor APKs retain Samsung signatures.

Affected: apktool BUILD guard; native-app all selection; M4 runtime input/output verification; OMX camera output verifier; release and ext4 image gates

Verify: Source donor EncoderFilter SHA0ae136... and exact patched SHA5dad71... established. Rebuild Camera through fork, verify method/signature/native bytes and original-camera negative, then rerun full compiled/policy/image gates and source replay.

Rollback: Remove0183; raw input checks remain baseline defaults. No kernel/vendor/radio change.

SHA256: `14649f46ab6ff2e6056875d3e13e2592d9e8d82f42051c7d8941e3893f526872`. Evidence: `logs/20261026-m4-vendor-warning-0142/`; actual staged packaging gates remain in progress.


## Patch0184 — rebuild reviewed bzi6 framework jars after source patches

Reason: m4-package-attempt37-0183.txt rejects stale SSRM PreMonitor.getBrightness bytecode. Actual clean log skipped ssrm/esecomm/semwifi/secinputdev/gamemanager JARs using old Exynos9810 list, although M4 source adapters changed their decoded classes. Extend existing M2 rebuild override to exact reviewed M4 scope; retain unrelated native APK guards.

Affected: scripts/apktool.sh legacy JAR skip-list dispatch

Verify: Actual SSRM stale compiled failure and clean skip logs; scoped BUILD dispatch and rebuilding existing decoded JARs, full compiled product/WLAN/ABI gates and source replay.

Rollback: Remove0184; M1/M2 behavior unchanged.

SHA256: `3f2cb542ed579fa751e0869d3c97f6888612c0c37d065776beb5ff2964dc60f5`. Evidence: `logs/20261026-m4-vendor-warning-0142/`; actual staged packaging gates remain in progress.


## Patch0185 — resolve bzi6 compiled system ext in essi system

Reason: m4-package-attempt37-0184.txt product gate raises FileNotFoundError for work_dir/system_ext/.../SystemUI.apk. Actual fork ESSI output embeds that APK at work_dir/system/system/system_ext, unlike decoded Apktool layout. Resolve the reviewed false system_ext-partition layout explicitly and keep all compiled assertions.

Affected: product_feature/a376b_android17_product.py compiled artifact resolver

Verify: Actual SystemUI/SSRM/work paths inspected; full compiled product gate and wrong layout failure, source replay and image audit.

Rollback: Remove0185 and restore old resolver. No ROM payload change.

SHA256: `59c7788d8d7a4188f40d4f62372e6e7f4c65cc9f6f61d6f40babd8be62ac803b`. Full176 source replay passed; actual compiled product/eSE/Wi-Fi/Settings/PIF/ADB/runtime and policy gates now pass in `m4-package-attempt37-0186.txt`. Actual images/package still pending.


## Patch0186 — share essi archive resolution with wifi ui verifier

Reason: m4-package-attempt37-0185.txt confirms the compiled Wi-Fi UI verifier duplicates the same wrong direct system_ext path exposed by the earlier product gate. Use the reviewed product resolver from0185 in both verifiers so all13 Wi-Fi class checks inspect the actual ESSI APK, retaining strict layout rejection.

Affected: product_feature/a376b_android17_product.py shared work_archive resolver; wifi/a376b_android17_ui.py consumer

Verify: Actual ESSI SystemUI path, shared SYSTEM/system_ext mapping and wrong-layout negative; full compiled product/Wi-Fi gates and source replay.

Rollback: Remove0186; no ROM payload or build-system replacement.

SHA256: `3a8890fc490acb8f48c7ff114c9ffef90a93315b0b27292fa85b5dfaedde135e`. Full176 source replay passed; actual compiled product/eSE/Wi-Fi/Settings/PIF/ADB/runtime and policy gates now pass in `m4-package-attempt37-0186.txt`. Actual images/package still pending.


## Patch0187 — verify final native32 sphal labels in ext4

Reason: m4-package-attempt37-0186.txt image gate rejects mapper@2.0 SELinux label. Actual metadata and existing final graphics restoration explicitly require same_process_hal_file for mapper2.0/2.1 and libgralloctypes in both ABIs. The isolated native32 image checker incorrectly expected system_lib_file for all copied libraries, ignoring later intentional graphics composition. Preserve established graphics labels and require the exact three-path exception in the real image checker.

Affected: a376b_native32.py M4 verify_image metadata expectations only

Verify: Actual finalization label assignment and file_context-system entries inspected; strict bytes/mode/UID/GID/symlink checks unchanged; exact SPHAL label required for three files, generic system_lib_file for all others; rerun real ext4 audits.

Rollback: Remove0187; no library byte, kernel/vendor/radio or policy change.

SHA256: `a94169ab771bfbfd40067fd84a19901a8d0ef2194b3ae7ae548ef010d62c692d`. Actual real-image retry in `m4-package-attempt37-0187.txt`; all byte/owner/mode checks remain mandatory.


## Patch0188 — deterministic M4 remotedisplay library modes

Reason: actual ext4 audit after0187 rejected `system/lib/android.hardware.common-V2-ndk.so` with `M4 native32 image mode mismatch`. Embedded Smart View/DeX restoration copied host mode0664 into fs_config for both common-V2 ABIs.

Affected: `exynos9810_finalization/customize.sh`, M4-only system lib/lib64/framework metadata branch. No library bytes, radio ABI, kernel or SELinux policy changed.

Verify: M4 overlay metadata now uses0644;43 staged overlay library hashes are unchanged, including both common-V2 ABIs. Shell syntax and178-patch replay from pinned fork passed,125 final text files match and138 older locks are unchanged. Existing compiled/policy and actual image audits rerun through `--package-staged --build-rom-zip`. Evidence: `logs/20261026-m4-vendor-warning-0142/staged-metadata-0188.json`, `replay-result-0188.json`, `m4-package-attempt37-0188.txt`.

Rollback: reverse0188 and rebuild M4; retained M1/M2 packages stay unchanged.

SHA256: `e33a06b39634cf60ab0edc4c9cb984a8e5423ae61049564f4890b6d3a68c1a74`.


Actual0188 result: `PASS M4 real ext4 native32 bytes/mode/UID/GID/SELinux, app64 and linker; 50 locked files`; SYSTEM/VENDOR runtime image audits and SYSTEM/VENDOR/ODM ext4 checks all passed. See `actual-image-audit-0188.json`. Target-files/full ZIP creation is continuing; this result does not establish device boot.


Final0188 host result: full signed M4 package `b8c70dfded510beed8e14207e0fecbe33e98d3061408c6f10fdc3b1925135f3e`, target-files and8 ZIP32 stages verified. See `docs/m4-attempt37-artifact-review.md`. Operational wrapper DEBUG initialization was corrected separately;178 fork patches remain unchanged. No M4 device boot is claimed.


## Patch0189 — M4 fstab mount ownership

Reason: actual M4 init parses both hw and secondary rc, mounts early/late
twice, and records EBUSY/fs_mgr_mount_all error. Affected: existing
a376b_metadata_bind.py M4 transform and Android17 runtime verifier/self-test.
Only the secondary two mount commands become comments; all other hardware
commands and the canonical metadata bind remain. M2 behavior is unchanged.

Verification: real installed rc fixtures/idempotence/extra-mount and missing
marker/canonical rejection, M2 byte preservation, full runtime self-test;
179 patches replay to125 identical files,178 older locks unchanged. Device
logcat test and BOOT/VENDOR rollback pending; see docs/m4-init-runtime-fix.md.
Rollback: reverse0189 for source; restore full VENDOR snapshot and original
M4 BOOT for the diagnostic device. No successful M4 boot is claimed.

Patch0189 SHA256: `0ce0b7ab4422fa37d542d30cb0039021021c83afcd43d3970c1f26a61a6926c1`.


## Patch0190 — M4 imported hardware ueventd configuration

Reason: actual M4 ueventd cannot read /vendor/etc/ueventd.rc; mcDriverDaemon
then logs Permission denied opening admin device and Can't open secure world.
The Hybrid Vendor's root ueventd.rc contains the existing S9+ node rules.
Affected: M4 runtime helper stages/verifies a hash-identical copy at the path
M4 imports, boot_restore registers root:root0644/vendor_configs_file metadata,
and the real-ext4 verifier checks its bytes and metadata.

Verification: full runtime self-test and destination-drift rejection PASS;180
source patches replay to125 matching files,179 previous hashes unchanged.
Actual node ownership/readiness and Android boot remain under device test;
see docs/m4-init-runtime-fix.md. Rollback: reverse0190/rebuild or restore saved
VENDOR and original M4 BOOT. No Trustonic/radio binary or kernel change.

SHA256: `ee5a654067039279925edf5cd1efc4dae2a623e740a4a8223db0c1b6bf265bbb`.


## Patch0191 — explicit M4 runtime diagnostic profile and logger image gate

Reason: retain the existing evidence-backed WIPEONFORK kernel and CACHE logger
for the next M4 test instead of accidentally reverting to the original kernel
with its earlier runtime failure. Affected: new sourceable M4 diagnostic
profile and M4 real-ext4 logger gate. Original production profiles preserved.
Verify: pinned existing archive/no KernelSU, diagnostic-to-production/M1 flag
reset and shell syntax PASS;181 patches replay to126 matching files,180 older
locks unchanged. New complete image validation remains pending next build.
Rollback: select the original M4 profile or reverse0191; no kernel rebuild.

SHA256: `c89a8bb79a7c2cc3df3ae2c07ca373b5d46053f1d2d5e59fe0678676f57564c6`.


## 27/10/2026 — patch0192, M4 additive graphics V6

Actual paired-network log exposes Hybrid Vendor libgralloctypes V6 dependency
after bpfloader exit0. Add locked S22 64-bit V6 alongside BZI6 V7 through the
existing M4 runtime helper/lock, restore metadata loop and real-ext4 verifier.
Helper negatives, actual ext4 byte/owner/mode/label fixture and182-patch/126-file
replay PASS;181 prior hashes unchanged. M2, network lock and kernel remain
unchanged. Reason/files/tests/rollback: docs/m4-graphics-v6-runtime.md.


## 27/10/2026 — patch0193, retained mapper3/2 fallback

Actual V6 trial exposes BZI6 libui API>35 guard skipping retained mapper3/2
code. Extend existing M4 helper/lock/restore/image gates with a single locked
four-byte branch adaptation; preserve genuine mapper/allocator load checks,
all other bytes and OS properties. Source/output/context/ELF/branch-target
gates, idempotence, bad destination/wrong M2/N rejection, actual ext4 metadata
fixture and183-patch/126-file replay PASS;182 earlier hashes unchanged.
Reason/files/runtime limits/rollback: docs/m4-graphics-v6-runtime.md.


## 27/10/2026 — patch0194, complete mapper call-site coverage

Actual0193 trial still fails in inlined GraphicBuffer constructor. Full BZI6
executable BL audit finds three API35 guards: out-of-line constructor, inline
constructor and preloadHal. Extend only existing M4 helper/lock to cover all
three with per-site segment/context/branch-target and whole-output hashes;
12 instruction bytes change, real mapper/allocator load checks stay. Helper,
actual ext4 metadata fixture and184-patch/126-file replay PASS;183 previous
hashes unchanged. Runtime result is separate. Reason/files/rollback and failed
0193 evidence: docs/m4-graphics-v6-runtime.md.


## 27/10/2026 — patch0195, real A17 native connectivity composition

New diagnostic-only helper/lock in the existing vendor-restore module compose
S22 Tethering with only the actual BZI6 connectivity-native DSO replaced. No
API stub, Java/framework downgrade, normal profile integration or kernel
change. All remaining payload bytes/links and75 metadata records preserved;
AVB/v3/4KiB and three identical independent builds PASS.185-patch/128-file
replay PASS;184 previous hashes unchanged. Static native consumers audited;
actual runtime and networking remain separate gates. Files/reason/limits/
rollback: docs/m4-native-connectivity-runtime.md.

- **0196 — diagnostic Java L4S compatibility:** actual M4 SystemServer fatal
  `NoSuchMethodError getL4sDeveloperOption(Context)I` proves the S22 Java
  provider lacks an A17 API despite195 native load success. Extend existing
  diagnostic APEX builder/lock and add scoped Java adapter; preserve all old
  methods, donate four real BZI6 getter/setter bodies and five constants.
  Verify1684 class round-trip, all7 actual services references, repeated fixed
  output/AVB/v3/metadata, native-only195 regression and186-patch replay.
  Rollback exact BZI6 APEX/libui plus full BOOT/VENDOR and V6 removal; normal
  profile/kernel/radio unchanged. See m4-native-connectivity-runtime.md.

- **0197 — stats/transport diagnostic closure:** real StatsPullAtomService
  NoSuchMethodError proves newer query API is missing. Existing network APEX
  builder/lock plus stats adapter donate actual query methods and shared
  identity/template/Parcel/v7-v8 unit, preserving old entry points and adding
  two exact utility methods.2161 classes/385 consumer refs and36 new refs,
  metadata/AVB/v3/repeated hash and187-patch replay pass. Runtime fixture and
  exact two-directory netstats snapshot/restore cover the new persistence
  boundary. No normal-profile, service/Binder, vendor/radio or new kernel
  replacement. Reason/files/tests/rollback: m4-network-stats-runtime.md.


## 0198 — optional GPU-map failure path (28 October 2026)

Reason: fresh197 SystemServer native SIGABRT at libmeminfo0x199e4 is the
negative-fd abort before existing GPU reader false/error handling. Files:
new a376b_android17_meminfo.py/.lock.json in exynos9810_boot_restore. A single
TBNZ target changes to the existing epilogue; valid-map verifier/results and
callers remain identical. No normal profile integration. Strict hashes/ELF/
branch/caller/context checks and negative cases pass;188 patches/132 files
replay PASS with187 older hashes unchanged. Runtime/rollback evidence and
limitations: [meminfo diagnostic](m4-meminfo-runtime.md).

Actual0198 runtime: Debug JNI/MemInfoReader exit0 PASS, raw root/su BPF pin
probe ENOENT, boot_completed1/bootanim stopped with Enforcing. Existing
SystemServer survives to337.22s; SystemUI/SDHMS/NFC still fail independently.
189 recovery files captured; complete diagnostic rollback is recorded in
the meminfo runtime report. No complete ROM package or full M4 acceptance
is implied by the fixture or boot property.

| 0199 | Composition0195–0198 chưa được chọn bởi profile và vendor restore cuối có thể xóa output; đưa đúng output đã đo vào profile chẩn đoán sau restore | `target/star2lte/profiles/m4-g965f-a376b-android17-runtime-init-debug.sh`, `.../a376b_android17_runtime_compat.py`, `.../a376b_android17_postboot_diagnostic.py`, finalization, `verify_m4_runtime_image.py` | Fixture BZI6: APEX `f17fa222…62cc6`, libmeminfo `b19a1c37…70fe9`; patch replay byte-for-byte; Python/shell syntax. Compiled APK/ext4 gate còn chờ build | Candidate diagnostic M4/G965F only; normal profile unchanged. Rollback: bỏ profile flag và reverse0199; chưa build/flash.
| 0200 | BZI6 SDHMS throws `No enum constant … OverheatComplexType.DEX`; user authorized omission of the DEX-specific special branch | `unica/patches/dvfs/customize.sh`, `a376b_android17_dvfs_policy.py/.lock.json` | APK source pin, input/output hashes and sizes, exact one-line transform, XML parse/no-DEX, distinct SSRM hash, replay fixture, shell/Python syntax PASS. Compiled APK/ext4 and thermal runtime pending | Only DEX/SKIN/470 node omitted; other S9 SIOP limits preserved. Not thermal PASS. Rollback original M4 recipe/profile; no device write.

| 0201 | Callback FaceWidget tạo LifecycleRegistry trên HandlerThread và crash vì bắt buộc main thread | `a376b_android17_facewidget_thread.py/.lock.json`, finalization, `verify_m4_runtime_image.py` | Pin 3 class Smali BZI6; generation + raw-instance stale guards; Apktool build rồi decode APK và kiểm cấu trúc PASS; patch replay byte-for-byte. Device latency/reconnect/UI tests pending | Chỉ M4/G965F/BZI6; init phase chạy main-looper một lần. Không bỏ lifecycle assertion/AOD. Rollback lấy lại SystemUI donor nguyên gốc; chưa build ROM/flash.
| 0202 | PHN `libnfc_nci_jni.so` đòi provider nhưng Java BZI6 có NativeNfcManager contract khác (54 native method so với72) nên không thêm PHN provider một cách mù quáng | `unica/patches/nfc/customize.sh`, `a376b_android17_nfc_native.py/.lock.json`, finalization, `verify_m4_runtime_image.py` | Nested BZI6 APEX JNI hash extraction PASS; exact PHN JNI hash pinned; helper loại bỏ system JNI shadow sau restore; image gate xác nhận embedded JNI và từ chối system-level copy. Profile Python/Bash syntax PASS | Chỉ explicit M4/G965F diagnostic profile; giữ nguyên S9 SLSI service/RF/HAL và Samsung JNI. Runtime linker namespace/JNI/HAL/tag chưa kiểm. Rollback reverse0202; chưa build ROM/flash.
| 0203 | Postboot helper xác minh thành công nhưng thất bại khi JSON serialize khóa `Path` | `a376b_android17_postboot_diagnostic.py` | Giữ native `Path` cho file checks, đổi khóa về string ở JSON output; syntax và verify-output PASS; build tiếp tục | Serialization only; không đổi payload.
| 0204 | Compiled release verifier đòi hash Tethering donor sau khi diagnostic profile đã chép output đo/khóa; log: `Final M4 runtime input mismatch: system/system/apex/com.google.android.tethering_compressed.apex` | `scripts/internal/verify_exynos9810_release_defaults.py` | Diagnostic-only profile check; cho phép bỏ đúng APEX trong generic check rồi xác minh cả hai output bằng postboot helper; compiled release gate PASS. Final image gate/build pending | Profile bình thường vẫn yêu cầu hash donor; không nới các input khác. Rollback reverse0204.
| 0205 | Image gate báo `M4 compiled SystemUI or bundled Apktool is missing`; profile gộp system_ext vào system/system_ext | `scripts/internal/verify_m4_runtime_image.py` | Chọn path theo `TARGET_OS_BUILD_SYSTEM_EXT_PARTITION`, map metadata SystemUI cho layout gộp; Python/bash syntax, patch reverse-check và hash series/lock 195/195 PASS. Rebuild và verifier ảnh thật pending | Chỉ đổi kiểm tra đường dẫn/metadata, không đổi payload; rollback reverse0205.
| 0206 | package0206 sau các image gate PASS dừng ở `star2lte: final Samsung Photo/QR repair is not applied`; M4 giữ camera donor BZI6 nên không dùng Photo/QR Smali repair One UI8 | `scripts/internal/verify_exynos9810_camera_matrix.sh` | Lưu target codename trước vòng lặp; đúng M4/star2lte/G965F/BZI6/runtime tuple chạy verifier BZI6 compiled OMX; cấu hình khác giữ gate Photo/QR cũ. Bash syntax, reverse/reapply, source-lock/series name+SHA audit, and M4 camera matrix including 4 DEX encoder/native hash checks PASS; full package rerun pending | Gate-only, không đổi camera payload. Rollback reverse0206 và bỏ khỏi `patches/fork/series`/`sources.lock.json`.

| M4-NETD / Patch0207 | Attempt0207 log repeats `CANNOT LINK EXECUTABLE "/system/bin/netd": library "libnetd_resolv.so" not found`; A37 netd needs this DSO, but the S22-base diagnostic APEX omitted it. | `a376b_android17_network_native.py` and its lock only; selected by explicit M4 G965F postboot diagnostic helper. | Adds hash-pinned BZI6 `libnetd_resolv.so` and protobuf `provideNativeLibs` field 7; verifies five changed payload files, all 76 metadata paths, ext4 roundtrip, AVB/v3/4KiB, and repeat APEX SHA `03a0fbcdd952df34795a6b3648583ae7feb6b49ab785a3cd074db85f7fb2a2ba`. Runtime network bring-up pending. | Patch0207; reverse it and rebuild prior diagnostic package. Does not change regular M4, kernel, modem, EFS, or target vendor/RIL. |

| M4-BUILD / Patch0208 | Attempt0208 reached completed postboot output composition, then `KeyError: PosixPath('system/system/apex/com.google.android.tethering_compressed.apex')` because `verify()` returns stringified report keys but `apply()` indexed with `Path`. | `a376b_android17_postboot_diagnostic.py` final report indexing only. | Python compile, scoped verify/apply JSON with exact APEX/libmeminfo hashes; resume current staged work_dir, then run compiled release, image and package gates. | Patch0208; reverse only the report lookup. Payload content is unchanged. |
| M4-NETD / Patch0209 | Attempt0208 apexd rejects the composed Tethering APEX: logcat_live lines 6203-6205 say `Manifest inside filesystem does not match manifest outside it`; patch0207 changed only the payload manifest. | `a376b_android17_network_native.py` and `.lock.json` | Outer and payload manifests now byte-match and both export `libnetd_resolv.so`; 76 payload metadata records, ext4 roundtrip, AVB, container v3 and 4KiB alignment pass. Two builds produce `0698d84c…65f6853` (67,911,680 bytes). Device activation/netd runtime still pending. | Patch0209; reverse and restore previous output hash to recreate the measured failure. Diagnostic M4/G965F only; no kernel, radio, or normal profile change. |

- 0210: retain BZI6 libnetd_resolv with its exact same-APEX private libcrypto/libssl providers. Reason: attempt0209 logcat:54555 fails netd loading libcrypto in tethering namespace, followed by netd1shot reboot. Files: existing network APEX builder+lock; no kernel/vendor/RIL change. Verification: recursive namespace import/DT_NEEDED gate,23-DSO strong symbol/version closure,78-path metadata/ext4/AVB/v3/alignment, two identical selected outputs bd17e835…, and fuzz0 byte-identical replay. TWRP runtime check is blocked by its unpatched MADV_WIPEONFORK recovery kernel; one-file device test uses the existing diagnostic BOOT. Details/rollback: docs/m4-postboot-runtime-repair.md.

- 0211: move the BZI6 resolver into the matching SYSTEM namespace, remove its private transplant from selected S22 APEX, reclassify one system linker provider declaration, and add stage/image metadata gates for both SYSTEM paths. Reason: actual0210 netd loader rejects fmt v12 in S22 APEX libbase; initial static symbol-union PASS was invalidated and namespace-aware negative fixture now catches it. Verify: two identical signed APEX outputs650f75fa…, fourteen pinned SYSTEM providers+statsd, positive/mutated-provider fixtures,20-DSO static closure and fuzz0 replay. Device and full-build runtime acceptance remain pending. Rollback backs up/restores APEX+linker config and removes added resolver; no kernel/vendor/RIL write.

- 0212: restore exact legacy `/dev/ion` -> `ion_device` pathname mapping for M4 Hybrid Vendor gralloc. Reason: live SetupWizard/SystemUI ion_open Permission denied with generic device label; changing only the live node label renders Welcome while Enforcing. Files: existing M4 runtime helper/lock and actual ext4 image checker. Verify: exact donor pin, existing runtime self-test, single-rule idempotence and missing/wrong/duplicate/symlink rejection, device-context fixture, targeted real ext4 metadata fixture,3-file fuzz0 replay, protected partition hash/readback after one-file recovery stage. Persistent reboot underway; full package pending. Rollback restores original plat_file_contexts.

- 0212 runtime: persistent reboot PASS automatic ion_device label, Enforcing, main SystemServer/netd120s and physical Welcome confirmed by user; zero previous ion_open/1440x2960 allocation failures in captured boot log. Known AOD SystemUI restart remains; full package pending.
- 0213: record already-tested release verifier local-import removal so exported series matches live source. Files: release defaults verifier only; no payload/device changes. Verify: Python syntax and203-patch/140-file byte-identical replay. Source preparation now recognizes historical a/rom/ headers;0209 diff hunk lengths corrected without payload changes.

- 0214: label only `/devices/platform/panel_drv@001/backlight/panel/brightness` as the existing `sysfs_ski_display_writable` type. Reason: SDHMS reports `fstat failed: EACCES` while node has generic `sysfs`; the vendor tree already grants `system_app_33_0` `getattr`/read on the selected type. Files: M4 vendor restore hook, new exact-scope helper/lock, and vendor ext4 image verifier. Verify: temporary label test, helper idempotence and duplicate/conflict/missing-permission negatives, real embedded-CIL fixture, Python/shell syntax, then exact CIL+bytes+metadata in the final vendor image. Source, vendor CIL/ext4 image and flash/reboot/runtime gates PASS for brightness only; after fresh reboot brightness has the expected context and no EACCES. Fresh log then exposed adjacent max_brightness EACCES; see0215. No new allow, thermal value, kernel, vendor blob or radio change; rollback by reversing0214 and rebuilding. Evidence/limitations: `docs/m4-thermal-backlight-0214.md`.
- 0215: extend the M4 SDHMS backlight label helper to the exact `/devices/platform/panel_drv@001/backlight/panel/max_brightness` node. Reason: package0214 reboot proves brightness label fixed, but log `10-28 19:23:37.606 8737 8859 E SDHMS:f1: ... max_brightness: fstat failed: EACCES`; temporary exact runtime label stopped new errors in a20s sample. Files: existing M4 thermal helper/lock and series/source lock. Verify: self-test for positive/idempotent/duplicate/conflict/missing permission, JSON/Python/shell/diff checks; actual vendor ext4 gate and fresh Enforcing runtime PASS in candidate `star2lte-1793191531-88WPtN` (package SHA256 `6d0c43e77c3bae6f392250cb5c4625fb6fc3af0041637b44bfd7e96ff61ae58d`), ZIP32 8-stage manifest PASS; both nodes show the intended type and no backlight fstat EACCES. Settings60/180 changes sysfs brightness and setting is restored. HAL cooling/threshold remain unexposed; actuator/panel output not fully validated. No new allow or DAC/thermal/kernel/vendor/RIL changes. Rollback reverse0215 and rebuild; retain0214. Evidence: `docs/m4-thermal-backlight-0215.md`.


## Patch0216 — M4 BZI6 PAC3 backlight scale (28 October 2026)

Reason: fresh M4-0215 reads show Settings brightness60/180/230 reaching the panel
as raw59/180/230, while the pinned DS-ACK PAC3 panel uses scale100 and its
first distinct brightness-table entries are at raw700. `PowerManagerUtil`
selects `AUTO_BRIGHTNESS_TYPE3` and leaves the donor-specific factor at1; the
locked legacy lights HAL writes the extended value unchanged.

Affected files: `rom/unica/patches/legacy/lights/services.jar/0003-BZI6-legacy-SehLights-backlight.patch`
and its exact-hash guard in `rom/unica/patches/legacy/lights/patch_bzi6_sehlights.py`.
For the existing M4/A376B-BZI6/SDK37/G965F branch, the main backlight extension
now always multiplies normalized brightness by255 and then100. Other light
types, SurfaceControl, global auto-brightness settings, mute/cache handling,
HAL, vendor, radio and kernel remain unchanged.

Host verification: reconstructed the exact BZI6 adapter preimage from the
compiled0215 output (SHA256 `a673e484…b3b90c88`), regenerated the nested unified
diff, and verified it applies at fuzz0 to produce the expected adapter
(SHA256 `5887361c…c00418d5`). The patch/helper hash pins and project patch are
recorded in `sources.lock.json`. Isolated package M4-0217 passed full fork
replay/build, final ext4/SELinux/runtime/ADB gates, signed ZIP verification and
all eight ZIP32 stage checks. It used the known-boot M4-0215 build config and
same embedded diagnostic kernel; the brightness patch is the only source-payload
delta. Fresh G965F boot reached Setup Wizard and `sys.boot_completed=1`; awake
Settings20/60/120 produced panel raw1907/5923/11947; zero/off reached raw0,
screen off/wake toggled raw0↔10943, and auto mode produced setting112/raw11114.
Original manual110 and USB stay-awake0 were restored. This verifies monotonic
PAC3 raw scaling on G965F. Follow-up29/10 adds5Dozing→Awake cycles without
new crash-buffer lines and the user's confirmation of clear/normal physical
brightness change. Full AOD functionality, ambient response across lux,
HBM/bounds and thermal limiter remain pending; KG/AOD/network/BT faults are
tracked independently in `docs/m4-validation-after-0217.md`.
Runtime evidence and exact artifact hashes: `logs/20261029-m4-brightness-isolation-0217/result.md`.

Rollback: remove0216 from the series/source lock and restore the prior nested
patch and hash pin. Keep package0215 as the rollback image until a replacement
boots and the modest-brightness/off/wake checks pass. Detailed evidence: [M4 brightness scale0216](m4-brightness-scale-0216.md).


Source replay correction for patch0214 (28 October 2026): clean replay exposed
`Hunk #3 FAILED at 177` in `verify_m4_runtime_image.py`; the old hunk expected
the `ueventd.rc` and `init.h` paths on separate source lines. Corrected only the
patch context/replacement to match the pinned preimage, updated the0214 SHA in
`sources.lock.json`, and replayed all206 patches from the pinned fork commit.
The 97 paths touched by the exported series are byte-identical to the working
fork after replay. This record fix does not change the built image behavior.

## Patch0217 — nonblocking M4 KG30 service lookup (29 October 2026)

Reason: M4-0217 first-boot ANR evidence at
`logs/20261029-m4-validation-0217/firstboot-watchdog-anr.txt:277–295` puts
SystemServer in the KnoxGuard30 TA-state FutureTask while its worker waits for
the absent G965F KG30 provider through `ServiceManager.waitForService()`.
Affected: the M4 finalization hook, a new exact BZI6 class/hash-pinned helper and
lock, and `verify_m4_runtime_image.py`'s compiled-JAR/ext4 byte/metadata gate.
Only `KnoxGuard30VendorController.getService()` changes to the already-present
`ServiceManager.checkService()`. The existing `0xff02` fail-closed response,
state5 mapping, binder checks, safeCall lock cleanup and three-attempt/100ms
retry remain unchanged. No vendor, RIL, kernel or system state is replaced.
Verification: pinned source and support-class hashes; wrong donor/profile
negatives; idempotent source apply; Apktool assemble/decode; compiled
checkService/no-wait, fail-close and retry contract; patch0217 fixture replay
byte-identical; syntax and JSON/source-lock hashes PASS. Build0218 package and
image gates pass. Three G965F cold boots reached boot_completed under Enforcing;
SystemServer stayed stable across120/60/60-second windows with zero watchdog,
system_server ANR or fatal lines in captured logs. Missing provider still
returns the existing fail-closed response. See [M4 KG30 patch report](m4-kg30-
nonblocking-0217.md). Rollback: remove0217 from the ordered series/source lock
and rebuild the JAR from the pinned BZI6 donor; retain M4-0218 until later
candidate gates pass.

## Patch0218 — exact S9+ RTC labels for M4 SDHMS (29 October 2026)

Reason: M4-0218 Enforcing logs show RTC node `fstat failed: EACCES` at
`logs/20261029-m4-kg30-nonblocking-0218/run/cache-unica/unica/logcat_live.txt`
lines180708,180710,180711. Runtime nodes resolve under the exact S9+ S2MPS18
RTC path and use generic `sysfs`; existing `sysfs_rtc` has the required
system_app directory/file permissions. Live `chcon` is rejected by sysfs, and
the exit trap plus readback preserve the original contexts.

Affected: extend `a376b_android17_thermal.py` and its lock with only three exact
RTC `genfscon` rules, alongside the two existing backlight rules. No new allow,
thermal threshold, brightness change, kernel, blob or radio modification.
Verification: Python syntax/JSON; helper idempotence and per-node duplicate,
conflict and missing-permission negatives; actual vendor CIL fixture adds
exactly three genfscon lines and no allow; full split SELinux policy compiles
with `secilc`; full ordered replay from the pinned fork commit passes for208
patches and144 touched paths byte-identical; patch reverse/reapply yields
byte-identical helper/lock. Build0219 completed through the existing fork with
exit0, compiled policy/ext4/metadata/ADB checks, signed package verification and
all eight signed ZIP32 stages. Readback of the final target-files VENDOR image
found each exact RTC rule once and no added allow. Fresh G965F install and two
cold boots under Enforcing confirmed all three runtime labels, advancing
`since_epoch`, zero RTC `fstat EACCES`, stable SystemServer and no second-boot
ANR/watchdog/fatal. Install and runtime evidence is in
`logs/20261029-m4-rtc-label-0219/`; see [M4 RTC report](m4-rtc-label-0218.md).

The first-boot capture separately contains
`Fatal signal 11 (SIGSEGV)` in the GNSS service at
`attempt0219/logcat-first-boot.txt:75181`; the second-boot 90-second capture has
no fatal signal and the service is running. GPS functionality remains a
separate open test. M4 overall is not accepted. Rollback: remove0218 from the
series/source lock and rebuild a separately versioned package run; do not
overwrite the retained M4-0219 artifact.

## Patch0219 — optional Samsung Internet preload (29 October 2026)

Reason: the clean build in `logs/20261029-m4-g965f-bt-native-0220/build.txt`
stopped at lines1228–1229 when Galaxy Store returned no URI for the optional
`com.sec.android.app.sbrowser` preload (`No download URI found` followed by
`URL is not set`). An unavailable optional download should not block unrelated
ROM compilation, while a returned URL that fails to download must remain a
hard error.

Affected file: `rom/unica/mods/preload/customize.sh`. It now warns and skips
only when the store returns no URI, and guards preload metadata enumeration if
the preload directory is absent. It still fails closed when a URL is present
but `DOWNLOAD_FILE` fails.

Verification: `bash -n`, both URL-unavailable and URL-present harness paths,
and `git diff --check` passed. Patch SHA256:
`defe64419062d21ce00e71a3b998003158ec3e4ec72611ec8376fdf3b3c354e2`.
The clean attempt0222 build and full package then passed; its compiled image
records the Samsung Internet preload metadata at
`logs/20261029-m4-g965f-bt-native-0222/build.txt:1222–1225`. This is a host
build fix only; it does not imply M4 runtime acceptance. Rollback by removing
0219 from `patches/fork/series`/`sources.lock.json` and restoring the prior
customizer.

## Patch0220 — skip unsupported BZI6 DBFW FDF1 send on M4 G965F (29 October 2026)

Reason: fresh M4 attempt0222 repeatedly logs
`Timed out waiting for Unknown OpCode(0xfdf1)` at
`logs/20261029-m4-g965f-flash-0222/flash-01/runtime/logcat-afterboot.txt:96022`,
followed by `ServiceConnection.onServiceDisconnected(...)` at line60466. Live
follow-up on the still-running image reached `Bluetooth crashed 116 times`;
the last captured timeout is in
`logs/20261029-m4-g965f-fdf1-followup-0223/prebuild-device/bluetooth-events.txt`.
The attempt0222 build logged that Android16 FDF1 patterns did not match BZI6 at
`build.txt:886–888`. Disassembly pins BZI6's optional enqueue in
`BtifAvStateMachine::StateOpened::OnEnter()` at VA `0x6979e8`.

Affected files: BZI6/G965F-only exact-hash sender helper and lock,
`rom/unica/patches/bluetooth/customize.sh`, the M4 profile,
`rom/scripts/internal/verify_m4_runtime_image.py`, and the Bluetooth README.
Patch0220 also exports the already-used M4 native VaultKeeper backend helper
and lock so a clean replay reproduces attempt0222's pre-existing JNI behavior;
the only new JNI payload delta is the single FDF1 enqueue BL replaced with
`NOP`. No fabricated controller response, kernel, vendor, radio or RIL change.

Host verification: fixture confirms the donor APEX/JNI SHA256, GNU Build ID,
unique context/preimage/output hashes, one-call replacement, idempotence,
wrong milestone/target and changed-preimage rejection. `bash -n`, Python
compile and `git diff --check` pass. Patches0219–0220 apply at fuzz0 after the
previous208 patches; the replayed fork matches the active source tree outside
initialized submodule and embedded-LFS payload directories. Patch SHA256:
`ef11bda169e52ecf408e33a97463dbae0bc5e7557b52796a762c02e81e834198`.
Clean build, image/package audit and G965F runtime trial remain pending.

Rollback: remove0220 from `patches/fork/series` and its key from
`sources.lock.json`, then rebuild the retained M4-0222 release or another
exclusive run. Keep patch0219 because the optional preload URL issue is an
independent host build fix.


## M4 runtime0221–0224 —30/10/2026

| Patch | Lý do / file ảnh hưởng | Kiểm chứng / kết quả / rollback |
| --- | --- | --- |
|0221|framework-res khai báo OTT ConnectivityCallListenerService nhưng compatibility APEX thiếu class; helper/lock finalization, manifest declaration và image verifier|Backendflags3/bit4off; chỉ enabled=false; nonsignature payload khác duy nhất manifest, cert giữ; reboot/default component0 không resolve; system_server count1. Baseline APK lưu riêng để rollback|
|0222|HFP-only NOP thử nghiệm thất bại; thêm helper/lock capability setter và compose locks/image gate|Hai lệnh store-zero/nhánh unsupported đúng ngữ nghĩa, donor/negative/idempotence PASS. Runtime loại bỏ HFP messages nhưng vẫn7crash: chưa đủ một mình; cần0223. Bản thử HFP NOP không nằm trong series|
|0223|DBFW+ còn main flag và12 null-callback sends; helper/lock/dispatch,3 compose locks, image gate|Khóa13instruction, BuildID/context/opcode/callback/table/branch; giữ nonnull callback sau guard, không giả completion. Startup120s,3normal toggles, extended uptime491s0crash/timeout; reboot của0224 vẫn0crash. Phụ kiện/HFP/audio chưa thử. Baseline JNI/hash từng lượt để rollback|
|0224|Legacy signed Infinity AOD thiếu hook disconnectPluginOnBackground; VersionCheckingProxy adapter/lock/dispatch và compiled/image AOD gate|Compiled reflection4cases + baseline negativecontrol, onDestroy giữ;64.508class DEX chỉ1semantic change, cert giữ. Reboot +7mẫu/121,77s và5Asleep→Awake giữ core PID; extended423s cùng server/UI/BT. Screenshot Welcome; physical confirmation/AOD extended open. Baseline SystemUI giữ để rollback|
|0225|M4 live `ro.product.model/name` trả donor A37 dù generic target props đã được đặt; system/system_ext build.prop vẫn giữ `SM-A376B/essi`|Chỉ đổi partition-qualified device/model/name cho system và system_ext trong đúng BZI6/G965F M4; giữ nguyên donor Android17 fingerprints. Exact donor preimage, wrong-scope/preimage negatives, idempotence và compiled-work check PASS. Runtime/image verification pending. Rollback bằng cách bỏ helper call; không cần đổi vendor/kernel/radio|

215patch replay thành160path byte-identical từ fork đã khóa (bỏ 7 `.orig` chỉ
do replay tạo). Syntax,
Python compile, scoped composed JNI/compiled APK verifier và fork diff-check
PASS. Image/ZIP đầy đủ mới chưa build; giữ full attempt0223 nguyên. Chi tiết
bằng chứng cụ thể và cách rollback:
[m4-post-0223-runtime-repair.md](m4-post-0223-runtime-repair.md).
Không ghi kernel/vendor/radio/EFS;10protected hashes đối chiếu trong từng
cửa sổ ghi recovery. Mọi số đo trên chỉ áp dụng G965F/M4/BZI6.

0225 ordered-replay result: all215 patches applied from pinned fork HEAD
`2b25ae6fc3098650be249bd1898e21e563cd5df1`; all160 active/added source files
match the working checkout byte-for-byte after excluding seven local `.orig`
backup files created only in the clean replay tree. Evidence:
`logs/20261030-m4-remaining/source-replay-0225.json` and
`source-replay-0225-tree-compare.json`.
# Patch 0226 — verify the installed Infinity AOD payload

Reason: M4 finalization checks the legacy AOD package before the later
`s9_infinity_Wallpaper` mod replaces the donor package. The pre-mod work tree
contains the BZI6 AOD (SHA256 `c223328adb3be501c0b78964f766fecec475ce2013d18d849c5413466dff8183`),
while the intended, hash-locked S9 Infinity payload is
`b374094fe87c6ab9f231096ddad4cd1bb0945be74c3247c9a441af8a3262b9f3`.
Checking the already-pinned payload file tests the exact APK that the later mod
installs and preserves the Samsung signature.

Files affected: `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`
and `patches/fork/0226-fix-m4-aod-payload-preimage-check.patch`.
Verification: ordered patch applies after 0225; the payload SHA matches the
existing `s9_infinity_Wallpaper/SHA256SUMS`; run the legacy FaceWidget helper
fixture, then verify the final SYSTEM image contains that exact AOD APK hash.


30/10 build0229 completed the corrected M4 package. Patch0225's six partition
identity properties and Patch0226's pinned AOD payload passed the final image
gate. The image verifier also includes the exact `framework-res.apk` metadata
entry required by the ext4 image. SYSTEM/VENDOR/ODM image checks passed; all
eight signed sideload stages passed CRC, signature, ZIP32 and order checks.
Package review is in `docs/m4-attempt0229-artifact-review.md`; runtime testing
completed first boot on G965F, but interactive/hardware acceptance remains open.
The signed Bixby receiver crash remains accepted per user direction.

## Patch0227 — clear stale kernel extraction before install (31 October 2026)

Reason: attempt0229 stage08 tmpfs retry encountered a stale `LICENSE` entry under `/tmp/exynos9810/kernel`; non-interactive unzip reported `unzip: can't read standard input: Not a typewriter`, then `E9810: Failed to extract kernel installer.` The successful retry removed that temporary directory first, then installed the same pinned kernel package and completed formatting.

Affected file: `rom/target/star2lte/installer/install-end.edify`.

Verification: isolated source-hunk replay against the post-0226 installer,
cleanup-before-mkdir/unzip ordering assertion, and a stale-directory fixture
all PASS. The successful clean tmpfs retry is the device-side behavioral
evidence. Generated-updater/package gates remain required on the next clean
build. Attempt0229 predates this patch. Rollback by reverting Patch0227; it
only changes a TWRP temporary path and does not alter system/vendor/kernel/radio
payloads.

- M4-r5 local staging: SamsungPropsHooks ART writer and scoped A37 identity, plus donor main switch layout for HideDeveloperStatusFragment. Reason, affected files, checks and rollback: [r5 local repair](m4-r5-local-repair.md). Isolated smali assembly and preimage rejection PASS; build/runtime pending.

### 0228 — M4 Integrity adapters and preformatted Lite installer source handoff

Lý do: chuỗi công khai dừng tại 0227, trong khi fork đã được phát triển thêm
cho M4 identity/Integrity và Lite một ZIP với USERDATA format trước. Patch này
chỉ ghi lại chênh lệch mã đã có, không chứa keybox, firmware hay ROM image.
Phạm vi: 28 file trong `rom/scripts/`, `rom/target/star2lte/profiles/`,
`rom/unica/mods/` và `rom/unica/patches/exynos9810_device_stack/`; tên từng file
nằm trong header của `patches/fork/0228-sync-m4-integrity-and-lite-installer.patch`.
Kiểm chứng: áp lần lượt 217 patch trước lên fork gốc 2b25ae6, dry-run rồi áp
0228; đối chiếu 171 file đã thay đổi trong fork, tất cả khớp byte. Kiểm build và
flash Lite một ZIP đã ghi trong `docs/m4-lite-preformatted-installer-20261103.md`;
không suy ra verdict Play Integrity hoặc nghiệm thu toàn bộ phần cứng.
