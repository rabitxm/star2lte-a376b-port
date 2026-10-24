# Bản đồ fork đã đọc

Audit 20/10/2026, `rom/`, nhánh Android-16,
commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`. Không có AGENTS.md con trong fork.
Đã thêm hai bản vá host-path cho công cụ dependency; chưa chạy build ROM hoặc installer.

| Vị trí trong rom/ | Vai trò và kết quả đọc |
| --- | --- |
| `buildenv.sh` | Chọn target, tạo out/config.sh qua gen_config_file; hỗ trợ --fs-type ext4/erofs; alias unica gọi scripts/*.sh |
| `scripts/internal/gen_config_file.sh` | Source target → platform → target lần nữa → unica/configs/$TARGET_OS_SINGLE_SYSTEM_IMAGE.sh; không tùy ý đoán biến |
| `unica/configs/essi.sh` | Donor S22 SM-S901B/EUX, SDK 36. Giữ nguyên để M1 đối chứng |
| `target/star2lte/config.sh` | TARGET_FIRMWARE là SM-G965F/PHN; assert chấp nhận F/FD/N, TARGET_SIM_VARIANT=auto. Có override nguồn/model guard; chưa hoàn thiện port N |
| `platform/exynos9810/config.sh` | SSI essi, board API 33, target SDK mặc định 31, EROFS mặc định; override EXYNOS9810_TARGET_OS_FILE_SYSTEM_TYPE; không dynamic partitions |
| `scripts/download_fw.sh` | Tải SOURCE/TARGET và EXTRA_FIRMWARES; FUS qua samloader; kiểm MD5 Odin; lấy bản mới từ Samsung version.xml |
| `scripts/extract_fw.sh` | Trích boot/AVB và các partition; super qua simg2img/lpunpack; không phải thao tác flash thiết bị |
| `scripts/internal/create_work_dir.sh` | Ghép system/product/system_ext donor, vendor/odm/kernel target; metadata fs_config/file_context được mang theo |
| `scripts/make_rom.sh` | Tải/trích khi thiếu, áp platform/device/modules, rebuild APK/JAR, kiểm boot contract rồi tạo images/package |
| `scripts/internal/apply_modules.sh` | Thứ tự module theo build_order; boot_restore/finalization được chèn vào giai đoạn cuối |
| `unica/patches/exynos9810_device_stack/customize.sh` | Hybrid vendor/odm và tương thích Exynos9810. Mặc định dùng embedded/exynos9810_legacy_port; overlay ngoài phải opt-in |
| Cùng file, `_EXYNOS9810_RESTORE_BOOTING_VNDK_APEX` | Ghép com.android.vndk.v33.apex. Không suy khả năng Android 17 chỉ từ tên APEX/prop |
| Cùng file, `_EXYNOS9810_RESTORE_BOOTING_SYSTEM_CORE` | Có khả năng thay framework/lib/lib64/Settings bằng baseline embedded. Khi đổi donor phải audit để không vô tình giữ framework cũ |
| `.../exynos9810_boot_restore/customize.sh` | Xây lại vendor từ baseline + common/star2lte overlays; star2lte giữ baseline đã nhúng. Không phân biệt N chỉ bằng codename |
| `.../exynos9810_finalization/customize.sh` | Khôi phục N770F radio rồi stock_radio_restore_v5 (rild/channel/imsd); giữ libsec-ril/libril_sem đi cặp, kiểm hash cuối pipeline |
| `target/star2lte/camera/camera-feature.xml` | Ma trận camera S9+; tái dùng, không thay bằng ma trận camera donor |
| `target/star2lte/vintf/compatibility_matrix.device.xml` | Matrix thiết bị; phải so matrix/framework donor thật và log |
| `platform/exynos9810/installer/customize.sh` | Chọn kernel ZIP theo filesystem, xác minh hash/payload/DTB, ghép ramdisk và vendor_bootfix |
| `platform/exynos9810/installer/install_ds_ack.sh` | Phân biệt G965F/G965N từ bootloader/model, dựng kernel/DTB qua bsdiff; repack và ghi BOOT. CHƯA CHẠY |
| `target/star2lte/installer/install-start.edify` | Gọi prepare_layout, có thể repartition/format khi thiếu dung lượng. CHƯA CHẠY |
| `platform/exynos9810/installer/repartition/prepare_layout.sh` | preflight không đạt sẽ tự gọi repartition.sh; không được coi ZIP mặc định là không đổi layout |
| `scripts/internal/verify_exynos9810_flashable.sh` | Kiểm hash kernel/ramdisk và DTB filesystem; cho override EXYNOS9810_EXPECTED_KERNEL_SHA256; hiện yêu cầu enforcing |
| `scripts/internal/verify_exynos9810_boot_contract.sh` | Hiện hard-code star2lte về SM-G965F/star2ltexx. Profile N cần audit cả verifier, không chỉ đổi TARGET_FIRMWARE |
| `scripts/build_dependencies.sh` | Symlink tới ../external/make.sh; build công cụ từ submodule, có git reset --hard trong apktool. Đã chạy trên checkout sạch; không dùng nguyên CI để dọn ổ/đổi cấu hình global |

Các `.../exynos9810_*` trên đều nằm trong `unica/patches/exynos9810_device_stack/`.
Danh mục patch ban đầu: 154 file .patch và 22 module patch cấp đầu; xem các TSV.
Đây là kiểm kê source, chưa phải kết quả áp patch lên A376B.

## Kernel/provenance

Fork mặc định chọn gói tháng 9 của Eend15 (README nói payload G965F đã đổi ngày
14/10), không được mặc định coi đó là byte nguyên bản của duhansysl. Gói nguyên
bản duhansysl OneUI7 Enforcing đã tải riêng vào inputs/kernels để kiểm tra,
không build/sửa kernel. EXYNOS9810_KERNEL_ZIP và
EXYNOS9810_EXPECTED_KERNEL_SHA256 là hai hook thực đã đọc để chọn/pin gói khác.
Chưa chạy đóng gói; phải kiểm các verifier/runtime liên quan trước sử dụng.


## Phần mở rộng cho M1 đã chọn

- `target/star2lte/profiles/m1-g965f-no-ksu.sh`: profile sourceable, pin kernel
  gốc 08/05 bằng SHA256, ext4, chỉ model F/FD, không ghép runtime KernelSU.
- `platform/exynos9810/config.sh`: input EXYNOS9810_BUILD_KERNELSU_RUNTIME và
  output EXYNOS9810_ENABLE_KERNELSU_RUNTIME tách biệt để buildenv không xóa
  override khi nạp lại config. gen_config_file serialize output cho subprocess.
- `.../exynos9810_finalization/customize.sh`: nhánh false dọn runtime root qua
  helper metadata/cache hiện có; verifier cuối kiểm cả symlink/file còn sót.
- `scripts/extract_fw.sh:111–157`: cần sudo mount/fuse đọc-only và đọc UID/GID,
  security.selinux trước khi tạo metadata. Lệnh trích riêng S22 ở docs/m1-build.md;
  không dùng giải nén thiếu metadata để giả lập .extracted.

- Sau HOST-004: external/make.sh build scripts/internal/f2fs_unpack_src vào
  out/tools/bin/f2fs_unpack; extract ưu tiên bắt buộc tool này cho F2FS và giữ
  output CLI. Không chạy binary tracked bị segfault, không tạo build system khác.

- HOST-005 quote hai đối số của lệnh dump bên trong debugfs -R tại
  _EXYNOS9810_COPY_ROOTFS_FILE; kiểm file tạm đã tạo trước cp/metadata.

- HOST-006 GET_PROP trong scripts/utils/common_utils.sh giữ file list bằng mảng
  thay word splitting; giữ thứ tự/ưu tiên file property và explicit-path API.

- HOST-007: EVAL bắt status ngay trong if để không mất diagnostic dưới set-e;
  Bluetooth dump quote ở debugfs và bắt buộc output có dữ liệu. Legacy module
  có fallback đọc nguyên payload kernel theo header/page/bounds chỉ cho kiểm
  feature Exynos9810; không chỉnh image, gói DS-ACK, source/version kernel.

- HOST-008 build_utils tìm llvm-readelf cạnh compiler khi distro chỉ cài tên
  versioned, rồi kiểm dependency trước recipe.
- ROM-005 module unica/mods/dsack_star2lte_kernel có nhánh không KernelSU: dùng
  template kernel-less có sẵn, giống install-end.edify. Kernel thực được ghép
  từ EXYNOS9810_KERNEL_ZIP bởi install_ds_ack.sh; mặc định KernelSU vẫn giữ.

- HOST-010 GET_WORK_DIR_HASH prune __pycache__ trong nguồn chính, giống mục
  tiêu loại cache của Face hash. Mọi source/asset thực vẫn được hash. Dùng
  --package-staged có sẵn chỉ sau output compile/verifiers đã kiểm, không
  sửa trực tiếp .completed/.modules_applied để bỏ qua quy trình.

- HOST-011 build_full_ota_zip.sh: hai đường dẫn archive 7z phải quote bên trong
  chuỗi EVAL để chạy trong workspace có dấu cách/ngoặc. Payload không đổi.
- INSTALL-001 install_utils.sh PRINT_ASSERTIONS: star2lte với model-list chỉ
  chấp nhận model tương ứng; không fallback codename F/N dùng chung. Các
  target khác và nhánh không model-list giữ hành vi cũ. Assert sinh trước
  install-start.edify/prepare_layout.sh, cần kiểm lại trong ZIP cuối.

- scripts/build_flashable_zip.sh --legacy-sideload takes an already verified
  full flashable ZIP, invokes scripts/internal/split_legacy_sideload.py. It
  does not replace make_rom or build images. Only full/new-only/stash-free
  system transfers, ordered classicZIP stages <2GiB; guarded final tail.


- ADB rule lâu dài: finalization/customize.sh gọi adb_defaults.py SAU overlay/mods,
  TRƯỚC APK/JAR rebuild. SettingsProvider DatabaseHelper.loadGlobal/System/SecureSettings
  được chèn INSERT OR IGNORE, AdbService$Lifecycle.onBootPhase ưu tiên stored setting.
  Release-default verifier disassemble compiled classes và kiểm property imports;
  guard áp dụng cả cached/package-staged build. Đường dẫn class được discover trong
  smali*; signature/locals/SQL guard sẽ dừng khi donor thay layout. Không sửa UI/Rampart.
  Xem docs/adb-defaults.md và patch0017; rule không giới hạn profile M1 hoặc DEBUG.


## 21/10/2026 — đường format data đã kiểm thực tế

`platform/exynos9810/installer/repartition/format_partitions.sh`: post_install
trước đây để USERDATA optional và chỉ tháo /data; /sdcard alias còn mount làm
mke2fs từ chối nhưng updater RC0. Patch0018 thêm mounts_for_block/unmount_block,
format USERDATA required. `scripts/internal/verify_exynos9810_flashable.sh`
kiểm script thật trong ZIP. Không đổi build system, ảnh/kernel hay source blob.
Lượt flash-03 dùng ZIP0017 đã pin + explicit TWRP Format Data, không coi source
patch0018 mới là đã nằm trong artifact đó.

## Retention — patch0019, 21/10/2026

`scripts/make_rom.sh` đọc ROM_MILESTONE và cấp ARTIFACT_DIR bằng mktemp trong
OUT_DIR/releases; export ROM_ARTIFACT_OUTPUT_DIR để build_flashable_zip.sh
đặt full ZIP cạnh target-files. Không thay OUT_DIR chứa shared tools/firmware.
M1 profile export ROM_MILESTONE=M1; profile M2 chưa tạo vì chờ payload.
`scripts/utils/build_utils.sh` cung cấp PUBLISH_BUILD_ARTIFACT exclusive;
create_target_files_zip.sh và build_full_ota_zip.sh publish từ staging.
Legacy splitter preflight collision, exclusive link từng phần, manifest open x.
Các đường dẫn đã đọc/fixture/replay, không phải build system mới.
Chi tiết marker/hash/rollback: docs/release-retention.md.

Donor selector hiện tại: gen_config_file.sh chọn unica/configs theo
TARGET_OS_SINGLE_SYSTEM_IMAGE=essi; essi.sh vẫn S22. Chưa tạo selector A376B.
Device stack có definition _EXYNOS9810_RESTORE_BOOTING_SYSTEM_CORE nhưng tìm
hiện tại chưa thấy invocation; cần audit provenance output thực M2, không
mặc định kết luận hàm này đang ghi framework S22.

## A376B16 — patch0020–0022

Input LOCAL được PARSE_FIRMWARE_STRING hỗ trợ cho gói Odin import, không có
IMEI/SN. Download_fw từ chối missing import trước network và giữ local --force.
Selector input UN1CA_DONOR_CONFIG trỏ tên file cấu hình đã validate, không đổi
TARGET_OS_SINGLE_SYSTEM_IMAGE=essi. Output ROM_SOURCE_CONFIG và donor/version
tham gia GET_WORK_DIR_HASH; M1 reset essi, profile M2 chọn essi_a376b_android16.
Không đổi OUT_DIR hoặc viết builder khác. A37 SystemUI thực nằm trong
system/system/system_ext/priv-app/SystemUI, không phải system/priv-app/SystemUI.
MDF/feature-app restoration còn các nguồn SM-S901B_EUX trong finalization;
đây là điểm audit provenance trước package, không được coi mọi file cuối là A37.
Framework baseline restore chỉ có definition, chưa thấy invocation.
A37 AdbService lifecycle dùng p1; helper/verifier nhận đúng layout đã rà soát.

## A376B legacy lights compatibility — patch0030

`unica/patches/legacy/customize.sh` selects a donor-specific bridge only for
`ROM_SOURCE_CONFIG=essi_a376b_android16`. The bridge implementation reuses the
fork's existing legacy SehLights method body while retaining AZH4's
`LightsService$VintfHalCache`; see `unica/patches/legacy/lights/` and
`docs/m2-build.md`. Other donors still use the original upstream patch. Static
Apktool assembly on decoded AZH4 passed; no hardware-light runtime claim yet.


## A376B resolution and media review — continuation 21/10/2026

The product_feature module invokes resolution APPLY_PATCH calls directly from `unica/patches/product_feature/customize.sh`; they do not use the generic smali walker. Patch0039/0040 adapt CoreRune and the moved GameDisplayListener there. The next SecSettings patch spans 57 old-donor class files; AZH4 Settings Utils/DisplaySettings are in smali/ and Samsung display classes are in smali_classes5/, unlike the upstream patch's smali_classes2/4 paths. Do not remap dex paths alone: synthetic lambda shapes and display mode APIs need semantic review. Actual decoded source and preflight remain under logs/20261021-m2-build/resolution-review/.

Stagefright AZH4 `ACodec::setupVideoEncoder`: HEVC global at relocation `0x2639a0`, branch `0xe5654` leads to the HDR10Plus feature assertion. Patch0032 replaces that one branch instruction with NOP; source binary/provenance stay locked. Evidence contains disassembly and the isolated four-byte difference audit.

M2 fixed-resolution route (patch0041): profile exports STAR2LTE_M2_FIXED_RESOLUTION=true; target config validates M2/A376B16 and serializes TARGET_COMMON_SUPPORT_DYN_RESOLUTION_CONTROL=false. The generic product_feature resolution block is consequently not executed (patch0039/0040 remain available but inactive for this fixed M2 profile). Device stack clears the inherited feature claim; finalization empties it in both feature files and checks for nonempty claims. Native panel/base-density setup at device-stack customize.sh around the ro.sf.lcd_density case stays unchanged.


A376B product-feature review0042–0048: actual branch dispatch is in `unica/patches/product_feature/customize.sh`. Exact class descriptors moved across DEX folders are resolved by SMALI_PATCH only for A376B16, never by basename/synthetic guess. Samsung rear FP position3 differs from Android TYPE_REAR1; the existing HIDL mapper already translates it. The device-stack star2lte overlay replaces BiometricSetting with a compatibility APK before product_feature; inspect that output, not only raw donor. Its DisplayStateManager uses Context constructors and feature-driven sensor flags. New `fingerprint/a376b_rear.py` checks the reviewed bodies.

AZH4 RefreshRateConfig uses dumpProductFeature and separate literal append operations; VrrManager constructor owns game HFR mode. Hardware mDNIe restoration uses `product_feature/mdnie/a376b_hw.py`, extracting complete old methods from the already present HW-disable diff with locked hash; no external binary/framework replacement. Late Face restore/checks are in `exynos9810_device_stack/exynos9810_finalization/customize.sh`; `scripts/internal/verify_face_omission.py` now cleans/verifies the selected omission after vendor restores and in the release gate. Mandatory ADB compiled verifier remains independent and always invoked.


Patch0049 dispatch: product_feature/customize.sh validates STAR2LTE_M2_KEEP_DONOR_WIFI_UI against M2/A376B16/star2lte and target AXfalse/source6GHzfalse, then skips only wifi/80211ax Settings/SystemUI diffs. M1 profile resets this flag; M2 explicitly selects it. Target Wi-Fi/native flags are not falsified to avoid the branch.


AZH4 Wi-Fi backend mapping0051–0052: SemWifiInjector constructor contains APE V1/V2 worker branches and personalization/L4s scalar after Qbox observer registration. SemQboxController uses sem_wifi_ape_enabled_labs; Settings controller moved from wifi/intelligent to wifi/develop. SQE commands moved to SemSqeCommandHandler + its SqeCommand enum; APE ordinal35 and MHS power29 use different shared targets, so split only the reviewed entries. SoftAp chip support query is already public; country method now clears/restores Binder identity and requires an exception-safe fallback. New scoped helpers under product_feature/wifi adapt these actual bodies, not synthetic name guesses.

SAIV0053: raw A376B moire model is vendor/etc/VslMesDetector/moire_detection.tflite; donor midas_config.json still references midas/moire_detection/moire_detection.tflite. PhotoEditor hs_segmenter directory holds HumanSeg and BGPclassifier info/model pairs with absolute model_path fields, not generic hs_segmenter.info/tflite.

22/10/2026 M2 Settings compatibility: `unica/mods/settings/a376b_search.py` is called by the existing Settings module for exact A376B16 donor; it hashes the reviewed inlined factory/Base/Mobile/UN1CA helper and appends the four custom providers. `unica/mods/settings/a376b/0001-Allow-more-than-five-apps.patch` is selected by the existing Smali dispatcher for this donor only. No alternate build entrypoint is introduced.

22/10/2026 native32/finalization mapping: existing exynos9810_boot_restore invokes a376b_native32.py with a376b_native32.lock.json; exact S22 extra donor is declared in essi_a376b_android16.sh. Existing release-default verifier rechecks native32/zygote/app ABI after rebuild. Finalization Photo Editor gate now chooses AZH4 priv-app/PhotoEditor_Full/PhotoEditor_Full.apk for this donor (0081), while S22 keeps PhotoEditor_AIFull. Finalization SecureFolder guard follows AZH4 com/samsung/knox/securefolder/settings namespace (0079). No new build entrypoint. Local APEX keys are ignored under keys/m2-native32-apex, outside source payload.

Optional diagnostic utility: `rom/scripts/internal/prepare_star2lte_boot_diagnostic.py` creates an exclusive output directory from the exact pinned G965F BOOT capture. It changes only legacy header command line; never accesses ADB/devices or builds kernel/images through a new pipeline. Review in `docs/m2-early-diagnostic-review.md`.


Patch0087 / explicit PID1 pmsg probe02: optional raw-syscall AArch64 monitor in `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`, prepare helper extended for cpio. Original init preserved as /init.real; probe keeps PID1 then execs original with argv/env, child copies selected kmsg errors to pmsg only, no FS mounts/partition access. TWRP fork/wait/exec/env self-test PASS, non-PID1 refusal13. Existing DS-ACK MagiskBoot repacks RAM files. Independent CPIO audit preserves all other data/metadata; kernel/DTB/cmdline unchanged. BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926. Diagnostic only: timing/SELinux can affect capture; compare results cautiously. Plan: one verified G965F boot under current project authorization, capture then rollback baseline cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d. Evidence `logs/20261022-m2-early-diag/pid1-probe-02/`.
