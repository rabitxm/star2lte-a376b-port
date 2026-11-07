# Bản đồ fork đã đọc

Kiểm bổ sung 01/11/2026: `unica/mods/keybox/` có hooks/generator/db/operations
của UN1CA, nhưng `scripts/internal/apply_modules.sh` bỏ mod trên M4/BZI6.
`unica/mods/settings/a376b_android17_boot_limits.py` ẩn UI và compiled gate
cấm hook hiện tại. Trong `out/target/star2lte/apktool/system/framework/framework.jar`,
hai proxy Keystore2 ở DEX4, `KeyStore2` ở DEX3, `KeymasterUtils` ở DEX2.
Preflight copy và kế hoạch port: [Integrity community plan](m4-play-integrity-community-plan-20261101.md).

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
| `.../exynos9810_boot_restore/a376b_android17_selinux.py` | Patch policy trong WORK_DIR riêng M4: khai báo alias VNDK33 cần cho BZI6, đổi đúng telephony label và bỏ một genfscon cũ trùng; không load policy trực tiếp trên thiết bị |
| `.../product_feature/a376b_android17_sim.py` | Xác minh method hash BZI6 và giữ TelephonyFeatures.isOneTray() runtime; tránh hard-code tray count chưa có bằng chứng |
| `.../product_feature/mdnie/a376b_hw.py` | Khôi phục mDNIe sysfs từ diff đã khóa của fork; nhánh BZI6 kiểm method/hash riêng, inline logger cũ đã mất và giữ permission check |
| `.../product_feature/wifi/a376b_backend.py` | Nhánh BZI6 tắt APE/personalization theo cờ target; kiểm Qbox handler, ordinal37 và thanh ghi SQE thật trước khi sửa |
| `.../product_feature/wifi/a376b_softap.py` | Nhánh BZI6 cho country fallback, power và sharing-lite; power kiểm đầu vào sau APE, sharing-lite kiểm sau power, không đổi HAL/radio blob |
| `.../product_feature/wifi/a376b_android17_wlan.py` | Chỉ M4/BZI6/F: kiểm booster000, bỏ ba literal bảng RF donor qua empty guard sẵn có, chuyển SoftAP11ax sang query chip; không sửa vendor/calibration |
| `.../product_feature/a376b_android17_product.py` + `a376b_android17_display.json` | Ba phase shipping/HDR/display chỉ BZI6/F/M4; pin source, giữ sensor queries; gate release đọc DEX/XML cuối |
| `scripts/utils/smali_utils.sh` | A16 fallback cũ; BZI6/F/M4/SDK37 phân giải descriptor chính xác qua DEX và chặn thiếu/trùng class |
| `.../product_feature/fingerprint/a376b_rear.py` | Nhánh BZI6 pin method framework/service/SystemUI và client BiometricSetting, kiểm Settings sau SPF replacement; giữ register v3 cho các cờ khác |
| `unica/patches/legacy/a376b_android17_media.py` | Chỉ BZI6/F/M4: kiểm ELF/hash/PT_LOAD/CBZ, vá4byte native và4 default encoder đã pin; verify-compiled đọc DEX thật bằng baksmali và được gọi ở release gate |
| `unica/patches/signature/verify_a376b17_preimages.py` | Pin ba class PM BZI6 trước patch signature riêng; giữ helper platform identity và so chứng chỉ của fork |
| `unica/patches/legacy/lights/patch_bzi6_sehlights.py` | Pin bốn class trước ghi; giữ LogicalLight/mute, thêm overload và cache brightness, chuyển extra argument vào SehLights Parcel; dùng patch backlight BZI6 riêng |
| `.../exynos9810_finalization/customize.sh` | Khôi phục N770F radio rồi stock_radio_restore_v5 (rild/channel/imsd); giữ libsec-ril/libril_sem đi cặp, kiểm hash cuối pipeline |
| `target/star2lte/camera/camera-feature.xml` | Ma trận camera S9+; tái dùng, không thay bằng ma trận camera donor |
| `unica/patches/camera/a376b_android17_features.py` | Pin marker/XML/GPPM BZI6, kiểm classifier/ARDoodle thực sự vắng; release gate kiểm claim không hồi sinh và GPPM còn trong work tree |
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


24/10/2026 diagnostic map: standalone probe13 recorder is
`diagnostics/bootlogger_pid1_probe13.c`; the read-only checksum/slot decoder is
`tools/decode_probe13.py`; final report is `docs/probe13-report.md`. These are
outside the normal fork patch series/recipes. Installed SYSTEM runtime input:
`system/apex/com.android.runtime.apex`, payload `bin/linker64` and
`bin/linkerconfig`. A37 init references the APEX linkerconfig path. The vendor
self-test rc is `vendor/etc/init/boringssl_self_test.rc`, executable
`vendor/bin/boringssl_self_test64`, interpreter `/system/bin/linker64` pointing
at the runtime APEX. Do not infer paths from the old S22 layout.

24/10/2026 kernel-runtime diagnostic additions: `patches/kernel/series` is a
separate DS-ACK source patch list, not consumed by the Android ROM build.
`diagnostics/madvise_wipeonfork_selftest.c` tests real kernel fork semantics.
`docs/m2-runtime-kernel-fix.md` records provenance and gates. Paired build
outputs/images and device evidence are local under
`logs/20261024-m2-runtime-fix/` and are intentionally excluded from export.


24/10/2026 runtime metadata mapping: donor SYSTEM system/etc/init/aconfigd.rc runs early-platform-init in early-init and platform-init in post-fs. A37 framework-res RANGING declaration has featureFlag android.permission.flags.ranging_permission_enabled. Decoded framework.jar classes com/android/internal/pm/pkg/component/AconfigFlags and android/os/flagging/PlatformAconfigPackage consume /metadata/aconfig/maps and /metadata/aconfig/boot; original tables reside in system/etc/aconfig/{package.map,flag.map,flag.val,flag.info}. Existing plat_file_contexts labels /metadata/aconfig as aconfig_storage_metadata_file. No metadata by-name partition/fstab mount exists on the G965F test layout. Diagnostic diff: diagnostics/m2-metadata-tmpfs.patch, not a new build entrypoint or production change.

Native camera mapping: existing exynos9810_finalization/customize.sh `_EXYNOS9810_FINAL_INSTALL_NATIVE_CAMERA` pins native_camera/cameraserver hash06d1c14d... and copies it into system/bin/cameraserver. This server imports the old AudioSystem::streamTypeToAttributes symbol; system/lib64/libaudioclient.so remains A37 and lacks it. Current graphics mapper restore copies S22 libgralloctypes, whose common-V6 NDK dependency is absent from the normal M2 system64 image. Runtime corroboration and exact source hashes are in docs/m2-metadata-runtime.md.

## 24 October 2026 — M2 native camera and metadata finalization

- Existing native-camera finalization installs hash-pinned A37 cameraserver for
  M2/A376B; native_camera/patches/a376b_cameraserver.py ports the reviewed8bytes.
- Existing Camera decode/finalization calls a376b_camera_version.py for exact80500;
  original version property, resources, native libs and signer are retained.
- exynos9810_boot_restore/customize.sh reapplies metadata after final vendor
  restore; a376b_metadata_bind.py edits canonical hw init and20 CACHE aliases.
- a376b_empty_vendor_flags.py generates only zero-package/flag v2 vendor storage;
  metadata remains registered through the fork's existing helper.
- a376b_native32.lock.json adds the exact system32 GLES1 wrapper dependency of
  installed legacy gralloc. No app32/ART32 or hardware driver replacement.
- M2 profile sets EXYNOS9810_ENABLE_METADATA_BIND=true; M1 resets false.

Patch series now91,72touched paths replay identically. Runtime/basic rear/front
still proof and remaining build/lifecycle limits: m2-camera-metadata-fixes.md.

## Integrated M2 runtime diagnostic path — 24 October 2026

New explicit profile: rom/target/star2lte/profiles/m2-g965f-a376b-android16-runtime-diagnostic.sh.
The existing DS-ACK archive variable is selected here; preparation helper
scripts/internal/prepare_m2_runtime_kernel.py creates a separate hash-locked
archive from the pinned original and already built/calibrated Image. It changes
two entry contents including a G965F-only guard, with original DTBs retained.
No kernel compilation/device writes in the helper. M1 resets the opt-in.

Existing boot_restore invokes a376b_runtime_compat.py with its adjacent lock to
stage Tethering and V6; existing compiled release verifier calls final checks.
Existing verify_exynos9810_images.sh calls verify_m2_runtime_image.py for actual
ext4 dumps/metadata, including the compiled ADB SettingsProvider/services bytes.
make_rom.sh work-dir identity includes runtime flag/kernel hash/metadata flag.
All normal profile defaults and the existing packaging/retention system remain.
Detailed execution and limits: docs/m2-integrated-runtime-build.md.

24/10/2026 M2 runtime diagnostic extension: `unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_runtime_compat.py` now hash-checks and stages a volatile `/metadata` tmpfs hook in donor `system/etc/init/aconfigd.rc` for the opt-in G965F M2 profile only. The generated rc SHA is pinned in the adjacent runtime lock and checked by compiled and actual-ext4 gates. Patch0104 is after0103 in the fork series; production/M1 profiles remain off.

24/10/2026 M2 attempt53 output is retained at `rom/out/releases/M2/star2lte-1792833533-dLCdbp`; read-only preflight, eight TWRP32 stages and install helper are under `logs/20261024-m2-attempt53/`. Existing fork paths remain the build, profile, hybrid vendor restore, image verification and package split implementation. The package has not been flashed under the current exact-SHA authorization; see `docs/m2-integrated-runtime-build.md`.


25/10/2026: parallel M2 logger profile is
`rom/target/star2lte/profiles/m2-g965f-a376b-android16-runtime-init-debug.sh`.
It sources the existing runtime-diagnostic profile and enables only INIT_DEBUG.
Existing `scripts/make_rom.sh` cache identity records both logger flags;
`gen_config_file.sh` preserves them in build-config, and
`verify_m2_runtime_image.py` audits the two logger files in actual ext4 VENDOR.
Storage placement/rollback is documented in docs/storage-cleanup-20261025.md;
`tools/mount-release-storage.sh` mounts only the known second-drive UUID.


Checkpoint0124: STAR2LTE_M4_FIXED_RESOLUTION is a distinct guarded selection in
`target/star2lte/config.sh`. Its floating-feature removal/final audit live in
the existing device-stack and finalization customizers. The M1 base profile
resets it, so selecting M1/M2 cannot inherit it. Checkpoint0131 later adds the
scoped M4 profile.
Detailed source/runtime boundaries: docs/m4-product-display.md.

## Checkpoint0128 — BZI6 registry, eSE and initial UI wiring

- `unica/configs/essi_a376b_android17.sh`: separate SDK37/shipping36/board34
  registry, locked BZI6 cache suffix, dynamic SIM count and unknown-Bluetooth
  consumer guard. It is not a ready target kernel/runtime profile.
- `unica/patches/ese/a376b_android17_baseline.py`: pins four BZI6 classes,
  applies existing target-none substitutions and checks compiled fields/paths.
- `unica/patches/product_feature/wifi/a376b_android17_ui.py/json`: pins13 retained
  UI classes and checks canonical compiled bodies before/after the skipped S22
  UI edits. Hardware capability configuration remains target Wi-Fi5.
- `scripts/utils/common_utils.sh::IS_A376B_M4_BOOT_TEST`: strict BZI6/F/M4/SDK37
  predicate used only for reviewed optional selection branches.
- `unica/mods/settings/a376b_android17_boot_limits.py`: initial control pruning,
  BZI6 Intelligence key array and compiled search/remap gate. The existing
  a376b_search.py now has separate BZI6 input pins, preserving A16 output.
- `scripts/make_rom.sh`: scoped Face omission entry and donor-hash coverage for
  four M4 runtime/UI selections. Existing boot/release verifiers gain the
  corresponding selected Face/Bixby/Settings checks; other M2-only workarounds
  remain restricted.

Evidence/limits: docs/m4-source-selection.md and
logs/20261026-m4-profile-preflight. Full M4 native32/runtime staging and profile
are still pending; no phone operation occurred in this checkpoint.

## Checkpoint0129 — M4 native32 staging within existing fork

- `.../exynos9810_boot_restore/a376b_native32.py::lock_path`: existing M2 lock
  by default; exact BZI6/F/M4/ext4/no-KSU selects the separate
  `a376b_android17_native32.lock.json`. Mixed M2 flags reject.
- Existing restore customizer uses that lock and
  `OUT_DIR/compat/a376b-android17-native32`; signing identity remains the local
  ignored `keys/m2-native32-apex`. No standalone ROM builder is added.
- Existing release-default A17 branch calls native32 verify. Existing ext4
  image verifier calls `verify-image` on M4 SYSTEM. This checks only the
  native32/app64 slice, not all still-pending M4 runtime/vendor/boot contracts.
- M1 base profile resets EXYNOS9810_M4_RUNTIME_COMPAT; M2 inherits that reset.

Exact APEX, actual M2/M4 staging, real ext4 fixture and image entrypoint pass.
119 patches replay to102 matching text files,118 old hashes unchanged.
Full M4 build/image/flash remain pending. See docs/m4-native32-runtime.md.


## Checkpoint0130 — isolated BZI6 runtime staging and image gate

- `.../exynos9810_boot_restore/a376b_android17_runtime_compat.py` consumes its
  adjacent hash lock, validates M4 scope and donor marker, stages matching A37
  camera/audio/graphics V7/Tethering plus vendor_kg and vendor aconfig-v2.
- `a376b_metadata_bind.py --milestone M4` uses the existing CACHE-backed
  `/metadata` hook with M4-specific SELinux aliases; M2 remains its default.
- Existing restore/finalization scripts invoke the M4-only source path and
  keep the A37 camera server/app native. Release defaults and image verification
  call the new work-tree/real-ext4 gate; no new build system was added.
- Existing `adb_defaults.py` keeps one-time SettingsProvider seeds and no-RSA
  properties, with a strict BZI6 AdbService lifecycle read. The source hashes
  for BZI6 `services.jar`/SettingsProvider are separate from staged runtime
  files; the existing Rampart/SecSettings/SUW UI is retained and statically
  checked, not patched.
- The image gate checks locked bytes, ext4 mode/owner/SELinux, vendor account,
  init bind and aconfig files. Positive ext4 fixture and wrong-mode/label
  negatives pass; the pre-0130 M2 metadata output is byte-identical.

Full M4 profile/build and runtime are still absent. No kernel, vendor binary,
radio, modem or device partition was modified. See
`docs/m4-native32-runtime.md` and `logs/20261026-m4-runtime-0130/`.



## Checkpoint0131 — scoped M4 profile and source/cache switch

- `target/star2lte/profiles/m4-g965f-a376b-android17-no-ksu.sh` sources the
  pinned M1/F/no-KSU DS-ACK baseline, then selects BZI6, M4 runtime/native32,
  metadata bind and the reviewed fixed-resolution/donor-Wi-Fi/boot-limit choices.
  It explicitly keeps every M2-only compatibility flag false.
- `patches/fork/0131-add-scoped-g965f-m4-android17-profile.patch` adds the
  profile to the existing fork series; its hash is in `sources.lock.json`.
- `logs/20261026-m4-profile-0131/profile-config-switch.txt` records actual
  isolated M2→M4→M2 generator/cache-marker verification. `rom/out/config.sh`
  and the M2 config/output remain unchanged.
- This is a scoped source profile, not an accepted release. Generic module/Knox
  dispatch, compiled work-cache switching and final boot/vendor/image gates remain.
  Clean build attempt01 stopped at the NFC guard before image creation.


## Checkpoint0132 — BZI6 Android17 NFC APEX

- `unica/patches/exynos9810_device_stack/customize.sh` now has a strict M4/BZI6/
  SDK37 branch that retains `system/apex/com.android.nfcservices.capex`; M1/M2
  continue using the existing Android16 `NfcNci.apk` route.
- `nfc/a376b_android17_nfc.lock.json` pins the exact donor capex and records the
  embedded `com.android.nfc` v17 APK/JNI hashes. Static APEX JNI inspection sees
  HIDL NFC1.0/1.2 and AIDL symbols; device logs are still needed for runtime proof.
- `scripts/internal/verify_m4_runtime_image.py` verifies donor/work capex and
  `SecNfc.apk` hashes plus packed SYSTEM bytes, mode0644, root ownership and
  system_file SELinux labels.
- Source audit, positive exact-file fixture, and changed-SecNfc negative pass in
  `logs/20261026-m4-nfc-0132/nfc-source-audit.json`; attempt01's exact path error
  is retained in `logs/20261026-m4-profile-0131/m4-build-attempt01.txt`.
- All122 ordered patches replay on pinned fork commit `2b25ae6`; all four 0132
  source files match byte-for-byte. Replay log:
  `logs/20261026-m4-nfc-0132/replay-local-0132-success.txt`.

Complete build and packed-image verification are still pending. No phone
operation occurred. See `docs/m4-nfc-integration.md`.


## Checkpoint0133 — BZI6 donor camera dispatch

- Attempt02 passed the NFC branch, then stopped at
  `customize.sh:4562:_EXYNOS9810_PATCH_ONEUI8_CAMERA_APP` because the A37 Android17
  camera APK lacks `PackageUtil.smali`.
- Patch0133 scopes the legacy One UI8 camera Smali adapter away from only the
  exact M4/G965F/SDK37 selection. Existing M4 finalization keeps the exact
  hash-locked BZI6 SamsungCamera APK/cameraserver/runtime. M1/M2 dispatch stays
  on the prior behavior.
- The donor runtime lock/source verification and valid/invalid profile fixtures
  pass. The release image and camera runtime remain unverified. Details:
  `docs/m4-plan.md`, `docs/patch-matrix.md`, and
  `logs/20261026-m4-nfc-0132/camera-source-audit.json`.


26/10/2026 M4 BZI6 patch0142: the existing scripts/internal/apply_modules.sh
dispatcher routes the donor vendor-mismatch warning patch to a separate
hash-locked Android17 decoded-Smali adapter only for exact M4 G965F/BZI6 scope.
Rationale and validation are in docs/patch-matrix.md; fixture/replay/Apktool
assembly evidence is under logs/20261026-m4-vendor-warning-0142/.


26/10/2026 M4 BZI6 patch0143: the existing unica/patches/nfc/customize.sh
dispatch preserves the exact donor Android17 sidecar libraries only for the
hash-pinned G965F/PHN target when both stock sidecars are absent. Patch rationale
and checks are in docs/patch-matrix.md; source/ELF/dispatch/replay evidence is
under logs/20261026-m4-vendor-warning-0142/.


27/10/2026 — M4 runtime mount owner: `a376b_metadata_bind.py` now removes only
the secondary early/late mount_all commands for M4 after vendor baseline
restore. `a376b_android17_runtime_compat.py` validates sole canonical ownership,
and the existing `verify_m4_runtime_image.py` composes this check with actual
rc image/work byte equality. M2 metadata transform is unchanged. Both legacy
rc files are still present because their other hardware actions differ.


27/10/2026 — M4 ueventd and diagnostics: a376b_android17_runtime_compat.py
copies/verifies the pinned existing vendor/ueventd.rc at vendor/etc/ueventd.rc;
boot_restore/customize.sh registers the new file metadata and the real M4
ext4 gate checks it. The separate M4 runtime-init-debug profile reuses the
existing WIPEONFORK archive and CACHE logger; the image gate checks logger
artifacts only when selected. No new build system or kernel build is involved.


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

- source0196: vendor-restore `a376b_android17_network_l4s.py` is imported only
  for diagnostic `a376b_android17_network_native.py --l4s-compat`; donor JARs,
  output and existing signing identity are hash-pinned. No normal profile
  automatically enables it. Real services API audit/private builds and device
  test: logs/20261027-m4-network-java and logs/20261027-m4-l4s-test.

- source0197: vendor-restore a376b_android17_network_stats.py is an optional
  adapter called only by the existing network-native builder --stats-compat
  (implies196 L4S). It changes main/t boot Java JARs, not the stats Binder
  service. support/diagnostics/m4-network-stats contains source-only runtime
  fixture; private logs/20261028-m4-stats-test holds narrow stats-data rollback.


0198 adds explicit diagnostic-only a376b_android17_meminfo.py/.lock.json
inside exynos9810_boot_restore; it is not selected by any normal profile.
Read actual BZI6/S22 libmeminfo disassembly and installed file before the
bounded GPU BPF invalid-fd experiment. See docs/m4-meminfo-runtime.md.


## M4 runtime-repair additions — 28/10/2026

- `unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_android17_postboot_diagnostic.py`: strict-scope composition of already measured network APEX and libmeminfo outputs after final vendor restore. It uses the existing network/meminfo builders, not a new build system.
- `unica/patches/dvfs/a376b_android17_dvfs_policy.py` + lock: exact hash-pinned removal of the user-approved BZI6-unsupported DEX special node; no generic suppression.
- `scripts/internal/verify_m4_runtime_image.py`: diagnostic profile checks compiled SDHMS policy payload, runtime files and ext4 bytes/metadata. Full final image gate still needs a new build.
- `patches/fork/series` now ends at0199/0200; `sources.lock.json` pins both patch hashes. Replay fixture checked each patch against its saved preimage.

- `unica/patches/exynos9810_device_stack/exynos9810_finalization/a376b_android17_facewidget_thread.py`: exact BZI6 `PluginFaceWidgetManager` dispatch bridge; routes one-time init through the existing main-looper handler with connect-generation and plugin-identity checks. `verify-apk` performs Apktool compile/decode validation.

- `unica/patches/exynos9810_device_stack/nfc/a376b_android17_nfc_native.py`: M4/G965F-only verifier for the matching BZI6 NFC APEX JNI and strict removal of the PHN global JNI shadow after restores. The target SLSI service, firmware/RF and Samsung JNI remain from the Exynos9810 target stack. The compiled image gate checks the nested APEX JNI hash and rejects a global PHN JNI. Runtime classloader namespace/JNI/HAL/tag acceptance is still required.

28/10 patch0210 extends the existing `exynos9810_boot_restore/a376b_android17_network_native.py` composition helper with a hash-pinned BZI6 resolver TLS member set and recursive APEX DT_NEEDED/import gate. The existing selected diagnostic profile consumes its locked stats output; normal M4 profile remains separate. Device tests can replace only the APEX file with a byte-verified backup, retaining BOOT/vendor/radio. Full provenance: docs/m4-postboot-runtime-repair.md.

Patch0211 adds `--system-resolver` to the existing network composition helper. The selected postboot helper stages BZI6 resolver plus the one-entry system linker protobuf transform, verifies14 SYSTEM DSOs+statsd, and exposes both paths through image_paths() to the real ext4 verifier. Finalizer applies the reviewed system_lib_file/system_file metadata. Earlier APEX-resolver modes remain explicit historical diagnostic modes; selected M4 diagnostic uses the SYSTEM namespace mode.

M4 ION display boundary (0212): `a376b_android17_runtime_compat.py::ion_file_context` stages/verifies one exact legacy `/dev/ion` pathname in platform contexts; the runtime lock pins donor contexts and rule. Existing apply/verify owns it, and `verify_m4_runtime_image.py` includes byte/owner/mode/file_contexts_file metadata checks inside packed SYSTEM. No new build system, vendor node permission or policy allow rule.

Source preparation audit28/10: `tools/prepare_fork.sh` selects -p2 for historical a/rom/ patch headers, otherwise -p1. Full203-patch replay reproduces140 current source files; default GNU patch fuzz remains needed for14 historical patches.0212 separately replays fuzz0.0209 hunk-count correction and0213 release-verifier import-scope record do not alter tested APK/APEX/device bytes.

28/10 post0212 plan/code ownership: Bluetooth JNI/capability adaptations belong to `unica/patches/bluetooth/customize.sh`; AOD/Infinity rebuild and payload/gates to `unica/mods/s9_infinity_Wallpaper/source/rebuild.py` plus source/aod and existing SystemUI main-thread adapter; thermal pathname policy to final boot_restore mapping with DVFS parser/asset lock; networking to existing network_native/postboot helpers after identifying the actual fwmark parser error owner. Current FwmarkServer string provenance is BZI6 netd, not enough evidence to replace S22 libnetd_updatable blindly.

29/10 Bluetooth follow-up: `unica/patches/bluetooth/a376b_android17_fdf1_skip.py`
and its JSON lock own the one-call BZI6 sender repair. The existing
`unica/patches/bluetooth/customize.sh` applies it only when the explicit
M4/G965F/BZI6/SDK37/star2lte profile flag is true, including final reapply;
`scripts/internal/verify_m4_runtime_image.py` verifies the final JNI hash in the
packed ext4 image. The adjacent `a376b_android17_native_backend.py` and lock
reproduce the VaultKeeper backend already used by attempt0222. Patch series and
lock entries are0219/0220; fixture/replay passes, while build and device runtime
remain pending.
