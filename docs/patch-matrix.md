# Ma trận patch và thay đổi

Chưa áp patch ROM nào lên A376B. `patch-inventory.tsv` liệt kê 154 file .patch
với hash và NOT_TESTED; `module-inventory.tsv` liệt kê 22 module patch cấp đầu.
Các nested final stages cũng cần audit; không coi số module cấp đầu là toàn bộ
những thay đổi động trong customize.sh.

| ID | Lý do | File ảnh hưởng | Kiểm chứng | Rollback/trạng thái |
| --- | --- | --- | --- | --- |
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
