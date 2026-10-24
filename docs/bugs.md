# Blocker và kết quả chưa kiểm chứng

M1F đã boot thực và user xác nhận; xem m1-boot-report.md. Bảng cũ dưới đây
giữ bằng chứng lịch sử môi trường/source; kết quả runtime mới ở cuối file.
Không suy từ cảnh báo thành yêu cầu sửa kernel.

| ID | Bằng chứng cụ thể | Tác động và bước giải quyết |
| --- | --- | --- |
| ENV-001 | `git-lfs/3.7.1 (GitHub; linux amd64; go 1.25.0)` sau khi người dùng cài | Tool đã có; phải hoàn tất pull và xác minh payload; không dùng pointer |
| ENV-002 | `sudo: interactive authentication is required`; command -v java/cmake/clang/lz4 chưa tìm thấy | Cần công cụ build/decompress; không yêu cầu gửi password vào chat |
| INPUT-001 | File ZIP DS-ACK trong fork bắt đầu `version https://git-lfs.github.com/spec/v1` | Chỉ là pointer lúc clone; BadZipFile khi kiểm tra là do chưa tải LFS, chưa kết luận ZIP nguồn hỏng |
| INPUT-002 | `TARGET_FIRMWARE="SM-G965F/PHN/..."` trong target/star2lte/config.sh | Chưa có package/hashes G965N để chọn blob/cấu hình N |
| LAYOUT-001 | N sda18=4454400 KiB, F sda18=4505600 KiB; fork system_size_mib=9500 | ZIP mặc định sẽ rẽ vào auto-repartition; chỉ chuẩn bị thiết kế, chưa chạy |
| RECOVERY-001 | Không có erofs trong /proc/filesystems cả hai máy | Ưu tiên ext4 debug; không flash EROFS trên recovery hiện tại chưa kiểm chứng |
| DONOR-001 | `_EXYNOS9810_RESTORE_BOOTING_SYSTEM_CORE` thay framework/lib theo baseline embedded | Đổi SOURCE_FIRMWARE không đủ tạo A37 port; cần kiểm provenance output, không chỉ version string |
| KERNEL-001 | prebuilt/README nói gói mặc định build từ Eend15 và thay G965F payload ngày 14/10 | Không đồng nhất với duhansysl nguyên bản. Gói duhansysl đã tải riêng/hash PASS; đã tích hợp/audit ZIP M1, chưa boot |
| GATE-001 | Chưa có log boot M1/M2 | Không chuyển mốc ROM; kiểm kê A17 độc lập được phép |
| INPUT-003 | `curl: (22) The requested URL returned error: 403` với curl mặc định; samloader cùng URL/header Kies2.0_FUS thành công | ĐÃ SỬA hook dùng cùng header; cả bốn metadata đọc được, S22 tải và kiểm MD5 xong |
| INPUT-004 | `download-fw.txt:31`: `Exception: DownloadBinaryInform returned 408, firmware could not be found?` với định danh mẫu; request Samsung serial thật được người dùng cho phép trả Status=200 | ĐÃ GIẢI QUYẾT tải: pipeline gốc exit0, MD5 năm file và marker PASS; không công bố định danh |
| ENV-003 | `Package 'liblz4' not found` từ pkg-config; CMake android-tools có pkg_check_modules(liblz4 REQUIRED ...) | Gói lz4 executable đã có, thiếu liblz4-dev cho công cụ host; không liên quan kernel |
| HOST-001 | `build-dependencies-before-path-fix.txt:16`: `/bin/sh: 1: Syntax error: "(" unexpected` | Đã sửa bằng VERBATIM trong CMake mkbootimg_symlink; android-tools đã build thành công |
| HOST-002 | NoNetworkConfigTest XML: `file must be a directory: ... Galaxy S9  (star2lte)/...` trong khi workspace là S9+ | ĐÃ SỬA bằng URI/JarURLConnection; apktool-lib 185 tests, 0 fail/0 skip |
| AUTH-001 | extract_fw.sh:111–157 gọi sudo -v, sudo fuse.erofs/mount/cp/stat/getfattr; sudo -n trước đó báo interactive authentication required | ĐÃ GIẢI QUYẾT: người dùng chạy trích target bằng sudo; marker/metadata và CRC522 archive PASS. Dòng echo thừa xuất hiện sau target không làm mất kết quả đã kiểm |
| HOST-004 | `extract-s22.txt:20`: `(./scripts/extract_fw.sh:106:EVAL) Command returned a non-zero exit code`; chạy binary tracked --help exit 139/Segmentation fault | ĐÃ SỬA: build nguồn đi kèm qua external/make.sh, extract dùng out/tools/bin. S22 extract/metadata năm partition/CRC 524 archive PASS; nguồn Go không đổi |
| HOST-005 | `m1-build-attempt1.txt:28`: `cp: cannot stat '.../exynos9810_rootfs_audit_filter_table': No such file or directory` | ĐÃ SỬA quote bên trong chuỗi debugfs -R và kiểm file output; năm rootfs copy/hash/metadata fixture PASS. Đang build lại M1 từ work_dir sạch |
| HOST-006 | `m1-build-attempt2.txt:58`: `(scripts/apktool.sh:200) Work dir needs to be set up before using this script` | ĐÃ SỬA GET_PROP giữ danh sách file bằng mảng, cat quoted paths; tag thực S901BXXSOGZH3 đọc được; tám case PASS. Lượt3 dùng work_dir sạch |
| HOST-007 | `m1-build-attempt3.txt:907`: `xxd: .../libbluetooth_jni.so: No such file or directory`; `legacy-unpack-error.txt`: `UnicodeDecodeError: 'utf-8' codec can't decode byte 0x83 in position 534: invalid start byte` | ĐÃ SỬA Bluetooth debugfs quote/output guard, EVAL lỗi dưới set-e, đọc raw payload header legacy có bounds check khi modern parser thất bại; chỉ kiểm feature, boot/DS-ACK không đổi |
| HOST-008 | `m1-build-attempt4.txt:2695`: `llvm-readelf is required to inspect Android packed ELF relocations` | ĐÃ SỬA discovery qua compiler; /usr/lib/llvm-21/bin/llvm-readelf đã có, native ABI check PASS, không cài thêm gói |
| PROFILE-001 | Source module dsack_star2lte_kernel chọn payload/boot-ext4.img hash b8daf... không xét profile không KernelSU | ĐÃ SỬA nhánh false dùng template kernel-less hash6a6fe... của platform installer; true không đổi. Đây là audit source, chưa phải lỗi boot. Full ZIP audit PASS; boot NOT_TESTED |
| HOST-010 | `m1-build-attempt5.txt:2844`: `Refusing to package work_dir: ROM patches/mods were not applied for the current source state` | Audit chỉ ra verify_factory_live_default.cpython-314.pyc sinh sau module; bỏ đúng cache tái tạo được hai hash gốc. Patch0010 loại __pycache__ khỏi source hash, giữ tất cả real inputs; package-staged đã hoàn tất exit0, verifier và ZIP audit PASS |
| HOST-011 | `m1-build-attempt6.txt`: `build_full_ota_zip.sh:301:EVAL` và `syntax error near unexpected token (` ở 7z path | Patch0011 quote đường dẫn archive trong hai lệnh; actual 7z fixture CRC/member PASS. Image ext4/ZIP đã dựng; package exit0 và audit PASS |
| INSTALL-001 | Source `install_utils.sh` PRINT_ASSERTIONS explicit model branch cho phép OR `ro.product.device == star2lte` | G965N cũng có codename này. Patch0012 chỉ cho star2lte model-list khớp, fixtures F/N/unknown PASS; final ZIP dòng1 model-only trước prepare_layout dòng21, audit PASS |


ENV-001 đã giải quyết: Git LFS 3.7.1; 4464/4464 path payload đã xác minh SHA256.
ENV-002/ENV-003 đã có gói cần thiết sau người dùng cài; pkg-config liblz4=1.10.0.
INPUT-001 đã giải quyết bằng pull LFS, không phải sửa binary.

Với lỗi runtime, ghi log cụ thể `avc: denied`, `CANNOT LINK EXECUTABLE`,
`dlopen failed`, `Fatal signal`, `vintf ... incompatible`, `init: ... failed`.
Android 17 chấp nhận HAL/VNDK nào: **cần xác minh bằng log**.

RECOVERY-TOOLS: flash-01/missing-system-bin-tools.txt:1 `ls: /system/bin/mkdir:
No such file or directory`, tương tựunzip/chmod. Fix0013; chưa flash bản lỗi.
GPT-ABI: flash-01/sgdisk-interpreter-check.txt có `CANNOT LINK EXECUTABLE
"/tmp/m1-preflight/sgdisk": cannot locate symbol` basic_stringstream. Fix0015
pin statictool đã chạy/verifyGPT thật và RAM script fixture PASS. Đây là
incompatibility utility/recovery, không phải bằng chứng cần sửa kernel.

RECOVERY-SIZE: after-sideload-01/recovery.txt:719 `sideload-host file size
1184633520 block size65536`, :723 `Zip file is corrupt!`; fullZIP5479600816B
lệch2^32. HostADB source client/commandline.cpp:904 gửi PRId64, lỗi ở recovery
legacy. GPT entry bytes vẫn nguyên sau reject. Patch0016 optional legacy
packaging trong fork; output8 ZIP32/CMS <2GiB, live RAM transfer/order test PASS.
Không sửa kernel hoặc recovery để né giới hạn này.


## Runtime M1 F — 20/10/2026

- EFS-001: preboot-audit.json `"protected_partitions_changed": ["EFS"]`, tổng FAIL.
  CPEFS/RADIO/RECOVERY/BOTA giữ hash. efs-mount-kernel-evidence.txt:1976/3889
  recovery mount ordered-data/journal; chưa chứng minh chỉ metadata. User đã
  yêu cầu tiếp tục, exception ràng buộc hash audit; không restore hay giả PASS.
- USB-001: logcat snapshot boot-completed-01:15012 `init: Command 'write
  /config/usb_gadget/g1/UDC ${sys.usb.controller}' ... failed: ... No such device`;
  :18460 `symlink() failed: File exists`. ADB ngắt/kết nối thật; chưa sửa kernel/USB.
- POLICY-001: logcat:14637 `avc: denied { set } ... scontext=u:r:su:s0 ...
  permissive=1`; global getenforce Enforcing, debugADBroot/secure0. Cần nghiệm thu
  release policy, không chỉ dựa getenforce; không setenforce/patch policy trong lượt này.
- WIFI-001: logcat:4644 `Service 'exec 2 (/vendor/bin/modprobe -a -d
  /vendor/lib/modules dhd.ko)' ... exited with status 1`. Wi-Fi HAL đăng ký sau boot;
  chưa kiểm Wi-Fi thực, không tự kết luận driver hỏng hoặc cần build kernel.
- RADIO-001: logcat:26881 `Service 'ril-daemon' (pid 1805) exited with status 0`;
  :26887 `restart cpboot-daemon5g ... failed: service cpboot-daemon5g not found`.
  Sau boot daemon running và IRadio1.6/Samsung bridge2.0 hai slot đăng ký. SIM
  ABSENT,ABSENT; chưa nghiệm thu gọi/data/IMS, chưa thay radio stack khóa ABI/hash.
- BOOT-001: M1F boot gate PASS theo user và boot_completed1, ảnh Welcome;
  hai snapshot không có fatal exception/signal/ANR/VINTF incompatible. Không
  đồng nghĩa toàn bộ tính năng PASS; N/A37 chưa thử.

Nguồn số dòng: inputs/m1-runtime-logs/flash-01/boot-completed-01/logcat.txt,
raw mode600 bỏ Git. Trích đoạn đã rà soát ở docs/m1-boot-report.md.


ADB-DEFAULT-001: rule mới yêu cầu mọi build default adb_on/rampart_off/no_RSA.
Patch0017 source/compiled checks PASS; candidate chưa flash. Việc bản M1 cũ
ro.adb.secure0/adb_enabled1 sau boot không chứng minh default setup hay USB ổn định.
USB-001 còn mở runtime; default sec_charging,adb mới cần xác minh log.
POLICY-001: debug/no-auth ADB nay là lựa chọn có chủ đích của dự án theo user;
permissive su domain vẫn là vấn đề policy riêng, patch này không thay SELinux.
Xem docs/adb-defaults.md về lỗi verifier đã sửa và test còn chưa chạy.


## Fresh data — 21/10/2026

- INSTALL-FRESH-001: `flash-03/fresh-format/installer-format-attempt.txt:2`:
  `USERDATA is mounted; will not make a filesystem here!`; recovery-after.txt
  vẫn ghi `Updater process ended with RC=0`. TWRP format lần đầu báo
  `Unable to wipe Data.`; UUID không đổi. Có nhiều bind /sdcard cùng source.
  Lượt flash-03 đã xử lý bằng tháo tất cả alias, TWRP Format Data lại, kiểm
  UUID mới và mất Settings/probe/adb_keys cũ trước boot. Patch0018 sửa guard
  installer/verifier cho lần package sau, đã qua fixtures; chưa flash patch0018.
- ADB-DEFAULT-001: flash-03 boot data mới có device_provisioned0/setup0,
  adb_enabled1/development1, Rampart master/strict/blocked_adb/blocked_usb_data0,
  SUW checkbox seed0. USB keyless CNXN/shell PASS; UI Auto Blocker Off và
  bật/tắt có tác dụng thật, adbd stopped/running đúng. USB debugging callback
  được thử qua Settings provider; công tắc GUI chưa thử vì đang Welcome.
- USB-001 vẫn mở: initial-log-analysis.json ghi148 lần khớp UDC No such device
  trong cả logcat+dmesg (có dòng trùng giữa hai snapshot). Không coi default
  sec_charging,adb là đã sửa dứt điểm USB. Không sửa/build kernel.

## Packaging / đầu vào M2 — 21/10/2026

- RETENTION-001: source cũ make_rom.sh có `rm -f "$OUT_DIR/$ZIP_FILE_NAME"`
  và `find "$OUT_DIR" -maxdepth 1 -type f -name "UN1CA_*_$TARGET_CODENAME*.zip" -delete`.
  Đây là nguy cơ mất output do đọc source, chưa ghi nhận M1 archive bị mất.
  Patch0019 loại bỏ delete, cấp unique run và exclusive publish; fixtures/replay
  PASS. M1 đã có bản copy SHA/size-verified, chỉ đọc; không chỉ dựa chmod.
- M2-INPUT-001: fus-android16-empty-identity.json ghi `"status": "408"`,
  `"fields": {}` cho hai request lịch sử; không đọc/gửi identity thiết bị.
  SamFW UI hiển thị `Calling Heimdall...`, nút route khác disabled và chưa tạo
  file tải; screenshot samfw-waiting.png. User đã nhận tự tải Full Files
  A376BXXS5AZH4/XXV. Blocker hiện tại: chưa có ZIP Android16 đã xác minh trên PC,
  không kết luận firmware không tồn tại hoặc tất cả nguồn đều hỏng.
- M2-VERSION-001: nhãn kế hoạch OneUI8, trang SamFW ghi OneUI8.5; cần đọc props/
  framework/APEX thực trước config. Không sửa property chỉ để khớp tên mốc.

## Đầu vào M2 đã giải quyết, donor mới cần guard riêng

- M2-INPUT-001: RESOLVED bằng ZIP user tải; MD5/CRC/Odin6 PASS. Lượt trích đầu
  extract-a376b.txt:20 ghi `Root permissions are required to unpack OS partitions`;
  user chạy wrapper sudo exit0, .extracted/metadata/CRC607 đã kiểm PASS.
- M2-VERSION-001: RESOLVED bằng system-build.prop:22 `ro.system.build.version.sdk=36`,
  :23 `ro.system.build.version.sdk_full=36.1`, :171 `ro.build.version.oneui=80500`.
  Gói là OneUI8.5, không phải8.0; M2 vẫn track A37Android16, M4Android17 riêng.
- M2-ADB-001: source preflight `Unknown Samsung AdbService lifecycle; review donor
  before building`; review p1/.locals6 và patch0021 + compiled class fixtures PASS.
  Chưa coi source fixtures là full donor build/boot.
- HOST-PROBE-DEX041: kiểm class fixture dùng --api36 dừng `dexVersion must be
  within [0, 999]`; --api35 standalone writer tạo DEX041 mà reader báo `Unexpected
  container offset in header`. Fixture instruction test dùngAPI32/DEX039 riêng,
  không sửa minSdk/ROM properties hoặc coi full services rebuild đã PASS.
  Full A376B services input có DEX041/minSdk35; phải kiểm khi build thực.
- READONLY-PROBE: dump.erofs --cat cho path SystemUI không tồn tại trả exit0 và
  file0byte. Decode ghi NotAZipFileException. Đã discover đúng system_ext path,
  kiểm nonempty + ZipFile rồi decode resource PASS. Không phải lỗi firmware.
- DONOR-CACHE-001: cache cũ không có selected donor/version. Patch0022 khóa
  identity trong hash và test donor-switch reject reuse, S22 config không đổi.

## Build M2 lần1

- M2-CAMERA-001: attempt1.txt:65 `Failed to patch method "prepare(IIII)V"`
  trong MultiMicController.smali; source A376B dùng prepare(IIIIZ)V. Patch0023
  guard null không thay body/ABI/feature class; actual S22/A376B compiled class
  fixtures PASS, replay23/34 PASS. Đang build lại attempt2, chưa có log camera.
- HOST-PROBE-DEX041: full services rebuild bằng Apktool thực đã PASS sau probe
  standalone; services-full-rebuild.json xác nhận output đọc được và ADB p1.
  minSdk35 không sửa, Apktool tự xuất039. Không coi probe standalone là lỗi
  chặn full ROM và không cần patch tool/kernel từ kết quả này.

## Build M2 lần2

- M2-CAMERA-002: attempt2.txt:905 `Missing SPF patches for condition
  (SOURCE_CAMERA_CONFIG_ACTION_CLASSIFIER: [], TARGET_CAMERA_CONFIG_ACTION_CLASSIFIER:
  [1,0]). Aborting`. Target claim cũ không có implementation donor. Patch0024
  xóa riêng optional action-classifier claim cho AZH4 đã kiểm; không giả source
  support hoặc thay blob. Năm fixtures/replay24/35 PASS. Optional feature này
  chưa hỗ trợ, normal camera chưa nghiệm thu; attempt3 đang chạy.
- BT-A376B: attempt2.txt có `No Android 16 Bluetooth CLOCK_BOOTTIME_ALARM selector
  found`, `No Android 16 Bluetooth primary CLOCK_BOOTTIME timer found`,
  `No Android 16 Bluetooth DBFW+ capability setter found`. Một số native pattern
  M1 khác A37; chỉ SCHED helper exact bytes khớp đã được patch. Không coi cảnh
  báo là bằng chứng Bluetooth hỏng hoặc cần build kernel; cần log runtime.
- CAMERA-LENS-A376B: `Skipping unsafe back bokeh lens select guard: SamsungCamera.apk
  does not contain the expected One UI 8 lens selector pattern`. Guard cũ không
  được áp dụng, Portrait/lens chức năng còn mở, không giả đã kiểm camera.

Policy source review A376B: AutoBlockerPreferenceController đọc secure master;
Rampart G1/c đọc system rampart_suw_main_on, registry/ADB policy dùng các khóa
đã seed. Source hashes/paths ở policy-bindings-review.json. Đây là source binding
review, không thay compiled full-ROM checks và fresh runtime test.

## Build M2 lần4

- M2-CAMERA-003: attempt4 dừng với HEX_PATCH báo không tìm thấy /system/system/lib/libstagefright.so tại unica/patches/camera/customize.sh:383. A376B AZH4 chỉ mang ABI áp dụng; patch camera gọi cả lib và lib64 vô điều kiện. Patch0027 chỉ xử lý file hiện hữu và chỉ cho phép ABI thiếu với đúng A376B XXV LOCAL SDK36; donor khác fail closed. stagefright-abi-fixtures.json có bốn trường hợp và shell syntax PASS; replay 27 patch/35 file PASS. Attempt5 đang chạy, chưa có gói M2.

## Build M2 lần5

- M2-DVFS-001: attempt5.txt:941 reported Smali not found at SamsungDeviceHealthManagerService.apk/smali/U1/w.smali. Apktool output lists smali/o5/w.smali; inspection confirms <clinit>()V reads SEC_FLOATING_FEATURE_SYSTEM_CONFIG_SIOP_POLICY_FILENAME and carries the A37 fallback string. Patch0028 selects o5/w only for exact A376B XXV LOCAL SDK36 and keeps the upstream mapping elsewhere. dvfs-obfuscation-review.json, shell syntax, and replay 28 patches/36 files PASS. Attempt6 starting.

## Build M2 lần6 — quyết định bridge Face HAL

- M2-FACE-001: attempt6 áp dụng legacy Fallback-to-Face-HIDL-2.0 và `git apply`
  báo context lệch trên A37 AZH4. Các dòng đại diện: `attempt6.txt:976`
  AidlResponseHandler, `:988` FaceAuthenticationClient, `:994` thiếu
  SemFaceServiceExImpl ExternalSyntheticLambda5, `:1009` HidlToAidlCallbackConverter,
  `:1032` TestHal. Phần này chuyển Samsung AIDL/Face HIDL3 donor sang HIDL2
  vendor target, nên không được bỏ qua như patch cosmetic. Người dùng đang chọn
  giữ Face Unlock bằng cách port bridge theo A37 hay tạm loại Face Unlock khỏi
  M2. Chưa build tiếp/package trước quyết định. Evidence
  face-a37-compat-review.json và source patch trong fork.

## M2 lần8 — legacy SehLights backport AZH4

- M2-LIGHTS-001: `attempt8.txt:1104-1105` báo patch không áp dụng tại
  `LightsService$LightImpl.smali:384`; `attempt8.txt:1132-1133` báo patch không
  áp dụng tại `PowerManagerUtil.smali:1020`. Cả hai thuộc
  `unica/patches/legacy/lights/services.jar/0001-Backport-legacy-SehLights-HAL-code.patch`.
  AZH4 services.jar có control-flow labels/register allocation khác patch
  upstream. Không bỏ qua backport vì target dùng legacy Exynos9810 SehLights;
  cần port có kiểm chứng trước khi package.

M2-LIGHTS-001 update: patch0030 adds a donor-specific AZH4 path rather than
skipping legacy lights. `sehlights-a376b-preflight.txt` records successful
strict adaptation and Apktool assembly on actual donor services.jar. Attempt8
remains the recorded failure; attempt9 is the clean full-ROM confirmation. Until
attempt9/package audit and device testing, runtime lights behavior is unverified.


## M2 lần10 — MotionPhoto AZH4 encoder method

- M2-MOTIONPHOTO-001: `attempt10.txt:1000` dừng vì không tìm thấy method
  `configVideoEncoderParameters(Landroid/media/MediaFormat;Lcom/samsung/android/motionphoto/utils/v2/video/VideoTranscodingTask;)V` trong `MotionPhoto.apk/.../VideoTranscoder.smali`. AZH4 có method codec formatter trong `sum/core/filter/EncoderFilter.smali`, nhưng không có entrypoint transcoder đời cũ. Patch0031 bỏ riêng bước sửa method này chỉ với config A376B Android16; giữ nguyên class donor, tiếp tục áp dụng các patch codec formatter đã có. Donor khác vẫn strict như upstream. Kiểm chứng tiếp bằng build sạch và audit compiled media path; MotionPhoto runtime cần log/functional test.

## M2 lần11 — stagefright OMX compatibility chưa có port

- M2-MEDIA-001: `attempt11.txt:1003-1005` ghi `xxd: .../system/system/lib/libstagefright.so: No such file or directory` và `ABORT) No known patch available for the supplied libstagefright.so`. AZH4 system chỉ có lib64; file 64-bit AZH4 SHA256 `1567ef98eb1ee090c14289c6a1b8cf87e9078a8c5a67495178874e3f998db2e5` không chứa hai signature mà fork hỗ trợ. G965F vendor khai báo OMX codec video. Không lấy libstagefright Android10 làm thay thế cho donor Android16 và không đoán byte patch mới. Chi tiết file, size, hash và đối chiếu S22 ở `logs/20261021-m2-build/media-stagefright-a376b-review.json`. Cần chọn port ngữ nghĩa theo disassembly, bỏ riêng nhánh legacy OMX với giới hạn chức năng rõ ràng, hoặc giữ M2 blocked. Chưa package/flash; camera/video hardware behavior chưa xác minh.


M2-MEDIA-001 update: user selected disassembly port. Patch0032 resolves AZH4 `.rela.dyn` slot `0x2639a0` as `_ZN7android25MEDIA_MIMETYPE_VIDEO_HEVCE`; disassembly at `lib64/libstagefright.so:0xe5654` is `cbz w0, 0xe6004` inside `ACodec::setupVideoEncoder`, reached after color format 25/10 checks. This is the HEVC-only early-exit analog to the S22 branch patched by the fork. It NOPs only that instruction for exact A376B16 profile; unexpected 32-bit lib aborts. Shell syntax/signature uniqueness/reverse patch check PASS. Next clean build12 and inspect output disassembly; camera/OMX runtime remains unverified.


## M2 attempt12

- M2-MEDIA-002: `attempt12.txt` reports at legacy/customize.sh:719 `Failed to replace value "const v3, 0x7f420888"` in GlobalPostProcMgr EncoderFilter.configCodec. AZH4 disassembly is Smali: method is present and the default constant exists as `const v4, 0x7f420888` at `rom/out/target/star2lte/apktool/system/priv-app/GlobalPostProcMgr/GlobalPostProcMgr.apk/smali/com/samsung/android/sum/core/filter/EncoderFilter.smali:1908`; subsequent code carries v4 to Message.get. Patch0033 uses v4 only for A376B16. Validate rebuilt APK with clean build13.


## M2 attempt13

- M2-MEDIA-003: `attempt13.txt:1007` reports `File not found: /system/system/priv-app/vexfwk_service/vexfwk_service.apk`. Source inventory of extracted A376B AZH4 has no such APK; the previous EncoderFilter patches passed. Patch0034 adds an exact-profile optional-file guard and warning; non-A376B donor behavior is unchanged. Verify clean build14.


## M2 attempt14

- M2-FRAMEWORK-001: vendor mismatch warning patch failed at `attempt14.txt:1035-1061`, targeting two synthetic lambda files absent/different in AZH4. Donor fingerprint string is in `ActivityManagerService$$ExternalSyntheticLambda8`; its UI work routes through lambda IDs 1/2, unlike fork patch expectation. Patch0035 skips the UI-warning patch for exact AZH4 profile; no framework behavior is altered. Build15 will verify the remaining pipeline.


## M2 attempt15

- M2-INPUT-001: `attempt15.txt:1038-1044` shows optional Bixby patch fails around PhoneWindowManager.smali:11681, expecting `move-object/from16 v2, p2` and `move/from16 v3, p3`; the AZH4 method is processing KeyGestureEvent. Patch0036 skips this nonessential remap only for A376B Android16; it does not modify key dispatch.


## M2 attempt16

- M2-PRODUCT-001: `attempt16.txt:1043` reports `Failed to replace value "36" of method "<init>()V"` in `HdmVendorController.smali`; donor class contains no constructor. Patch0037 guards that absent method only for AZH4; other shipping API substitutions remain. Clean build17 to verify.


## M2 attempt17

- M2-PRODUCT-002: `attempt17.txt:1045` reports missing `smali/com/android/server/knox/dar/ddar/ta/TAProxy.smali`; source class inventory confirms AZH4 has no TAProxy. Patch0038 skips only that impossible edit on A376B Android16, preserving the other API-level patches. Verify build18.


## M2 attempt18

- M2-DISPLAY-001: `attempt18.txt:1087-1097` reports git apply mismatch in `CoreRune.smali` for FW_DYNAMIC_RESOLUTION_CONTROL. AZH4 current codeline assigns false at `<clinit>`; target profile config says enabled and floating feature says `WQHD,FHD,HD`; two VRR resolution policy flags are already false. Patch0039 applies a narrow CoreRune codeline substitution only for A376B16. Confirm final compiled fields in build19; display switching still requires hardware/runtime test.


## M2 attempt19

- M2-DISPLAY-002: `attempt19.txt:1087-1097` repeats the resolution hunk mismatch. The A376B resolution patch is directly invoked from `unica/patches/product_feature/customize.sh`, not the generic smali patch walker. Patch0039 now adapts that exact call site; build20 must show the donor-specific SMALI_PATCH log and validate final CoreRune values.


## M2 attempt20

- M2-DISPLAY-002: attempt20 confirms CoreRune dynamic flag patch passed, then `attempt20.txt:1090` reports absent `GameManagerService$DisplayListener.smali`. AZH4 equivalent is `GameDisplayListener.smali` constructor, where `WQHD,FHD,HD` is compared to empty and assigned to MULTI_RESOLUTION_SUPPORTED. Patch0040 replaces the empty comparison operand only on A376B16. Verify compiled jar and later runtime resolution switching.


## M2 attempt21

- M2-DISPLAY-003: `attempt21.txt:1086` reports the AZH4 GameDisplayListener patch could not replace `const-string v1, ""`; helper expects full-line replacement for empty literal, and the previous call did not request line mode. Patch0040 now includes a newline in replacement to trigger line mode. Validate build22 output.


## Continuation static audit after attempt22

- M2-REPLAY-001: replay reported `patch: **** malformed patch at line 28: diff --git ...0030...`; after correcting the root audit patch hunk count, it also exposed invalid `index 0000000..0000000` headers. Both record errors are repaired, source recipe unchanged, lock hash refreshed. `patch40-replay.json`: PASS 40 patches, 40 byte-identical files.
- M2-DISPLAY-PREFLIGHT-001: `resolution-settings-preflight.txt` reports `error: .../smali_classes2/com/android/settings/DisplaySettings$3.smali: No such file or directory` and other failures (63 error lines across the generic 57-file Settings patch). The actual AZH4 Settings classes moved among dex folders, and lambda/control flow differs. This is an independent read-only preflight, not a fresh whole-ROM build failure. Pending scope decision: full resolution port or temporary fixed resolution. No fallback applied.
- M2-DISPLAY-003 static update: fork SMALI_PATCH line mode on actual decoded AZH4 GameDisplayListener and Apktool rebuild now pass, evidence `resolution-game-patch.txt`, `resolution-game-rebuild.txt`, `continuation-static-audit.json`. Attempt22 was interrupted before reaching this step; no ROM pass is claimed.
- M2-MEDIA-001 static update: `stagefright-disassembly/after.txt` shows `e5654: d503201f nop`. The four-byte instruction is the only changed range in the isolated donor copy. Original relocation identifies HEVC, and the taken path logs `Do not enable feature for HDR10Plus recording` before CHECK(false). Camera/video runtime remains open.


M2-DISPLAY-PREFLIGHT-001 decision: user chose temporary fixed resolution for M2. Patch0041 disables dynamic-resolution recipe selection only in that profile, clears stale feature claims and updates the final feature audit to require disabled state. This is a scoped limitation for M2; full WQHD/FHD/HD switching remains deferred. Static profile fixtures and replay41 PASS. No runtime claim.


Product-feature continuation findings — 21/10/2026:

- M2-HDR-001: `attempt23.txt:1053` reports `Missing SPF patches for condition (SOURCE_COMMON_SUPPORT_HDR_EFFECT: [false], TARGET_COMMON_SUPPORT_HDR_EFFECT: [true]). Aborting`. Scoped0042 restores retained controller availability; diagnostic preflight passes, runtime pending.
- M2-FP-001: `product-tail-after-hdr.txt:10` reports `SOURCE_FINGERPRINT_CONFIG_SENSOR: [google_touch_display_optical,settings=3], TARGET_FINGERPRINT_CONFIG_SENSOR: [google_touch_rear,settings=3,navi=1]`. Patch0043 adapts reviewed constants; pinned compatibility BiometricSetting differs from raw donor, verified before edits. Static preflight passes; enrollment/unlock pending.
- M2-HFR-001: `product-tail-lcd.txt:22` reports `RefreshRateConfig.smali: patch does not apply`; later VrrManager owns the moved VRR initializer. Patch0044 passes HFR diagnostic steps, runtime60Hz pending.
- M2-MDNIE-001: `product-tail-hfr-supported.txt:7-8` reports source HWfalse -> targettrue and color weakness3->3 as unsupported. Patch0045 restores actual bodies/flags rather than only the feature label. Static preflight PASS; hardware paths need logs.
- M2-STRONGBOX-001: `product-tail-ril.txt:2` reports `Failed to replace return value ... isSupportStrongboxDeviceID()Z ... const/4 v0, 0x0`; method already exactly matches generated false return. Patch0046 logs verified exact no-op.
- M2-WIFI-BACKOFF-001: `product-tail-security.txt:19` reports `SemWifiCoexManager.smali: patch does not apply`. Patch0047 empties verified placeholder and preserves existing parser guard; preflight passes.
- M2-WIFI-UI-001 OPEN: `product-tail-wifi.txt:8` reports `WifiSlice.smali: No such file or directory`. Relocation preflight still fails with16 Settings /6 SystemUI error lines and missing WifiEntriesFilter synthetic lambda. User preference pending; no skip applied.
- M2-FACE-FINAL-001: source review found finalization restoring Face HAL despite selected omission and release verifier unconditionally requiring bridge. Patch0048 adds late omission + negative checks, profile isolation. Fixtures and actual incomplete staged-tree preflight pass; full build/release gates remain.


M2-WIFI-UI-001 selected limitation: user chose retaining donor scan UI for M2. Patch0049 skips only Settings/SystemUI 80211AX icon/label diffs under the explicit scoped profile flag. Potential Wi-Fi6 labels remain; full UI port deferred. Other Wi-Fi branches stay strict; attempt24 clean build running.


- M2-SMALI-SCOPE-001: attempt24 local regression, getMainInstance method selector leaked into SecInput replaceall. Fixed0050; actual helper fixture and continuation PASS. No clean full build pass claimed yet.
- M2-WIFI-BACKEND-001: product-tail-after-input.txt:94-95 `SemWifiInjector.smali:1641 ... patch does not apply`. Scoped0051 static preflight passes actual APE/personalization adaptation; no native/runtime claim.
- M2-SOFTAP-001: product-tail-after-ape.txt and product-tail-softap.txt old-donor diffs fail SemSoftApConfiguration/SemWifiApChipInfo and SemWifiApServiceImpl. Power/sharing diffs also fail independent preflight. Scoped0052 fixes semantic paths with driver queries and Binder cleanup; recipe-tail/independent assembly PASS, runtime pending.
- M2-SAIV-LAYOUT-001: remaining-modules-preflight.txt:5 lacks legacy moire path; saiv-preflight.txt:12 lacks generic hs_segmenter.info. Scoped0053 uses hash-identical relocated A376B model and actual paired donor files. SAIV2 preflight PASS. Initial temporary-parent fallback produced `Can't open /vendor/etc/midas/moire_detection ...` during SELinux guessing; explicit metadata was added and checked instead. No package created from the diagnostic tree.
- M2-MODULE-LAYOUT-001: attempt25.txt:1230 failed on missing SecSettings `Settings$FOTASuggestionActivity.smali`; customize-only skip was insufficient because the fork applies the module's duplicate Smali patch after customize.sh. Patch0057/0058 skip both routes only on A376B Android16. attempt27 confirms skip and continues. Optional Updates suggestion shortcut unavailable on M2.
- M2-MODULE-LAYOUT-002: attempt27.txt:1237–1241 reports three absent legacy SecSettings classes in the CSC real-time network-speed patch. Patch0059 keeps donor implementation only on AZH4 M2; clean attempt28 ongoing. Setting is not adapted and runtime UI remains donor behavior.
- M2-MODULE-LAYOUT-003: mods2-preflight.txt:75,136 shows old blocklist patch requires sIsRduDevice/sLduBlocklist and old lifecycle hook, while AZH4 uses changed PackageBlockListPolicy. Patch0056 skips optional legacy blocklist mod for M2; donor policy retained. It does not affect the separately verified ADB/Auto Blocker rules.
- M2-MODULE-LAYOUT-004: attempt28.txt:1238 reports the CSC PowerOffLockPreferenceController class missing from AZH4 SecSettings. Patch0060 skips only the optional legacy hook for A376B16 M2 and retains donor behavior; attempt29 checks the remaining CSC module path.
- M2-MODULE-LAYOUT-005: attempt29.txt:1239,1241 reports legacy `Usefulfeature$1.smali` and `FunctionKeyUtils.smali` missing in AZH4. Patch0061 skips only the quick-switch Secure Folder hook on A376B16 M2; donor behavior is retained.
- M2-MODULE-LAYOUT-006: attempt30.txt:1261 reports CSC Smart View tile visibility patch does not apply to AZH4 QpRune initialization. Patch0062 skips this optional tile setting for M2; donor Smart View UI remains.
- M2-MODULE-LAYOUT-007: attempt31.txt:1236–1454 reports four DAAgent patch hunks fail against AZH4. Patch0063 preserves donor Dual Messenger allowlist and skips only the optional all-apps customization on M2.
- M2-MODULE-LAYOUT-008: attempt32 fails with `Smali not found` for Knox TAProxy at deknox/customize.sh:103; AZH4 services.jar lacks this class. Patch0064 preserves other Nuke Knox changes and skips only this edit for M2.
- M2-MODULE-LAYOUT-009: attempt33.txt:1437–1480 shows Nuke Knox patch failures in LockPatternUtils, VerifyCredentialResponse, DarRune and CoreRune after TAProxy was also absent. Patch0065 skips the complete optional mod for A376B16 M2 to avoid partially removing Knox; donor Knox remains enabled.
- M2-MODULE-LAYOUT-010: attempt34.txt:1281–1295 shows Hide My Applist hooks mismatch AZH4 ComputerEngine/ActivityStarter. Patch0066 skips the complete optional module for M2 so no partial privacy hook is applied; donor behavior remains.
- M2-MODULE-LAYOUT-011: attempt35.txt:1256–1260 reports KeyboxImitationHooks patch targets absent AZH4 keystore2/BouncyCastle classes. Patch0067 skips the complete optional spoof module and preserves donor attestation behavior.
- M2-MODULE-LAYOUT-012: attempt36.txt:1263 identifies `knoxpatch/services.jar/0001-Bypass-ICD-verification.patch`; multiple attestation call sites fail (DevicePolicyManagerService, DarManagerService, UserManagerService and keystore helpers). Patch0068 preserves the donor implementation by omitting the complete optional KnoxPatch mod on M2. Runtime Knox features remain unverified.
- M2-MODULE-LAYOUT-013: attempt37.txt:1280 identifies `paradigm/ead/services.jar/0001-Add-Adaptive-color-tone-feature.patch`; five display-service classes mismatch AZH4. Patch0069 omits this optional feature. Attempt39.txt:1333 then reports `File not found: /system/system/priv-app/EnvironmentAdaptiveDisplay/EnvironmentAdaptiveDisplay.apk` because the generic Smali walker still saw its patches; patch0070 covers that separate route. Replay70 passes; full clean build verification pending.
- M2-SETTINGS-SEARCH-001: `attempt41.txt:1489` says `Failed to replace value ... SearchIndexableResourcesBase;-><init>()V ... SearchFeatureProviderImpl$$ExternalSyntheticLambda0.smali`. AZH4 inlines allocation/HashSet initialization and 211 provider registrations; Base/Mobile constructors are absent. Patch0071 appends a static four-provider registration, preserving the donor factory. Hash/rejection fixtures and DEX assembly PASS; Settings search runtime pending.
- M2-MULTISOUND-001: `settings-tail71.txt:41` says `SelectMultipleAppsFragment.smali:1427 ... patch does not apply`. AZH4 adds footer-child enablement. Patch0072 bypasses only the two five-app tests, preserving donor list saving and UI behavior; DEX assembly and actual dispatch PASS, six-app test pending.
- M2-SETUP-001: `postsettings72.txt:2` says `Smali not found ... smali/S2/f.smali`. Patch0073 uses reviewed I2/f.d and Activity.e methods with exact hashes. Custom disclaimer then fails resource/Activity hunks (`postsettings73.txt:53,96`); patch0074 preserves the donor page. Navigation DEX assembly PASS; setup runtime pending.
- M2-OPTIONAL-SETTINGS-001: Five optional feature patches mismatch AZH4 (`settings-patches-preflight72.json`; first actual error `settings-smali72.txt:18–19`, `ApplicationPackageManager.smali:209 ... patch does not apply`). Patch0075 temporarily retains donor behavior and removes the corresponding Extra Settings switches; supported hooks PASS in settings-smali75. These are not the mandatory USB debugging/Auto Blocker controls.
- M2-OPTIONAL-SYSTEMUI-001: QP text marquee patch fails multiple AZH4 XML layouts (`postsetup74.txt`); sensor-confirmation patch targets an absent class (`tweaks-preflight74.json`). Patch0076 retains donor text scrolling and sensor confirmation. postsetup76 dispatcher PASS; haptics was already applied before the QP failure and is not skipped.
- M2-ABI-001 (OPEN): `finalstages76.txt:90` reports `Missing system graphics interface: system/system/lib/android.hardware.graphics.mapper@2.0.so`. Audit confirms A37 system/app ABI is arm64-only and runtime APEX has linker64 only, but hybrid vendor still sets zygote64_32 and nine ELF32 vendor executables request /system/bin/linker (audio, camera, OMX among them). Do not bypass the graphics check or discard vendor32 blobs. Native32 runtime/namespace adaptation is required regardless of app ABI choice; app32 scope awaits user. See docs/m2-abi-review.md and ABI audit JSONs. No M2 runtime failure claimed.
- M2-ABI-001 update0077: user confirmed arm64-only apps. Native32 candidate keeps all original A37 runtime payload files and adds pinned S22 native32;48 native platform libraries staged. AVB/v3/alignment/reproducibility and rejection tests PASS, original A37 uid/gid/mode/SELinux PASS. finalstages77 native32/ABI/graphics gates PASS. This resolves the static restore blocker; generated linker namespaces/hardware runtime still need device logs. No M2 boot claim.

22/10/2026 M2 build blocker: attempt42.txt:2393 PhotoEditor_AIFull missing. AZH4 package is PhotoEditor_Full and stage matches donor SHA91a458916c289f9218e1791c0d934cb40de1f538a72ab6f402c97d77008d412e. Patch0081 fixes exact donor path, keeps integrity/library/permission gate; isolated helper PASS. Clean build and Photo Editor runtime remain pending.

M2 host gate mismatch: boot-contract-preflight43.txt rejects missing face-default-sec.rc intentionally removed by M2 Face omission.0082 reconciles verifier with exact selected profile; keeps original requirements for other donors and existing omission audit. S22 SDP absence gate is replaced only on M2 by retained AZH4 SDP class inventory; runtime Knox compatibility still requires logs. Final compiled audit pending.

M2 compiled feature mismatch: attempt43 ends after29min10sec at missing UN1CA Bixby remapper.0036 skips incompatible key-event hook, while Settings controls and final keylayout USER mapping remain. User decision pending: hide omitted feature/retain original keylayout or port remapper. Full boot gate blocks package. Release-default verifier Bixby positive-control XML alone is insufficient; keep boot contract mandatory. Compiled ADB and native32 independent checks PASS.


## M2-BOOT-001 — repeated boot before ADB, cause unresolved (22/10/2026)

Fresh M2 G965F test repeatedly restarts. Bootloader trace `logs/20261022-m2-g965f-flash-01/last-kmsg-after-bootloop.txt:22341` ends normal-start handoff with `Starting kernel...`; no Android init/panic signature captured. Actual installed BOOT is byte-identical to successful M1 flash03; nine early system/vendor files match staging (audit02 JSON evidence). Data has no Android runtime logs. This does not establish a kernel/HAL/SELinux cause. Need early handoff/first-stage/second-stage console capture. Do not use existing PID1 wrapper patch blindly: current logger source is standalone and explicitly forbids replacing `/init`. Kernel remains pinned; no diagnostic image flashed.


Flash02 postboot capture complete: G965F verified again in TWRP (`SM-G965F`, `G965FXXUHFVG6`, serialM2_G965F_SERIAL). Raw read-only logs in `logs/20261022-m2-g965f-flash-02/postboot-recovery/`. 2MiB last-kmsg trace records repeated normal boots 07:31–07:34 UTC then recovery; line22392 is `Starting kernel...`. Pstore only has pmsg-ramoops-0 (16,372 bytes), with adbd FunctionFS/USB endpoint shutdown messages, including `remote usb: read terminated (message): Cannot send after transport endpoint shutdown`; this does not establish a system-boot cause or which boot produced every record. No console-ramoops/panic/Android init failure captured. USERDATA still has the new UUID74d8755a-7aff-4d40-93da-20b57bda9b97 and only lost+found/media after recovery creates its media directory. Fresh install did not resolve the bootloop; runtime gate remains FAIL, root cause unconfirmed. Phone left in TWRP; no additional write/reboot during this capture.


Early diagnostic test01 completed22/10/2026: user confirmed bootloop and returned G965F to TWRP. Device identity and diagnostic BOOT SHA were verified; `/proc/last_kmsg`, `/proc/sec_log`, pstore and recovery logs captured under `logs/20261022-m2-early-diag/test-01/postboot/`. No console-ramoops or M2 fatal/panic signature recovered. Logs contain recovery kernel `Linux version 4.9.118-g36e6ddff22b9`; fstab errors (`/etc/recovery.fstab`) and hung-task lines belong to a recovery trace and are not evidence of M2 failure. Diagnostic effectiveness unproven, root cause unresolved. Original BOOT restored and entire partition readback SHA verified `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; no post-rollback reboot. Phone left TWRP. No kernel change, no data format, no N operation. Result JSON in test01. Next investigation must improve capture before/through early kernel/init rather than repeat unchanged fresh installs.


Probe02 completed: verified G965F returned to TWRP; probe BOOT hash matched. Captured pstore (empty), last_kmsg/sec_log/recovery/reset reason under `logs/20261022-m2-early-diag/pid1-probe-02/postboot/`. No M2-PID1 marker or M2 fatal recovered; absence does not distinguish pre-init failure from unavailable pmsg/sysfs/dev nodes or recorder failure. Restored baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d with entire-partition readback PASS, no reboot. Phone TWRP.

Patch0088 is an unflashed follow-up prototype: isolated monitor mount namespace for only tmpfs/sysfs so original init mount table stays untouched. RAM preflight FAILED `NAMESPACE stage=4 errno=016` (sysfs mount EBUSY), exit20; parent mount table SHA unchanged. Do not flash this prototype; no diagnostic image03 created/installed. Evidence `logs/20261022-m2-early-diag/pid1-probe-03/namespace-selftest.txt`. Root cause still unresolved; preserve pinned kernel and baseline BOOT.

Probe04/05 follow-up (22/10/2026): probe04 warm/cold sysfs and pmsg persistence preflight passed, yet after M2 bootloop recovery pstore had no PID1 marker. Probe05's dedicated CACHE log survived a TWRP reboot but after the M2 bootloop still contained only `M2-CACHE-RAM-SELFTEST-20261022-PROBE05`; no runtime marker. G965F BOOT was restored to SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Last-kmsg probe05 line25231 says `Starting kernel...`; no Android panic/init line is preserved. The missing marker is inconclusive because the child monitor may fail before CACHE is available or under system SELinux. Kernel/DTB were unchanged; no evidence yet justifies a kernel edit. See `logs/20261022-m2-early-diag/pid1-probe-04/` and `pid1-probe-05/`.

23/10/2026 — probe05 không tạo CACHE marker ngay cả khi M1 boot thành công. Android runtime báo `NAMESPACE stage=4 errno=002`: `/sys/class/pmsg/pmsg0/dev` không tồn tại trên kernel DS-ACK system. Thêm nữa, self-test CACHE báo PASS nhưng marker không thấy ở `/cache` khi ext4 mount trả `EBUSY`; probe đã ghi vào tmpfs riêng. Patch0091 sửa cả hai và kiểm ext4 trước khi ghi. M1 với probe06 boot hoàn tất và file `/cache/m2-early-boot-probe06.log` chứa `M2-PID1-PROBE-v4: persistent cache monitor ready`, `init: boot_stat write failed`. M2 fresh với cùng probe06 chưa có ADB trong 240 giây; phải đọc CACHE sau khi trở về TWRP trước khi chẩn đoán lỗi M2. Không đủ log để sửa kernel.

23/10/2026 — probe06 on M2: `/cache/m2-early-boot-probe06.log:2` has `M2-PID1-PROBE-v4: persistent cache monitor ready`, repeated 23 times through the bootloop; each captured segment ends at selected kmsg time ~0.82–0.84 s, e.g. line29 `dwc3 10c00000.dwc3: dwc3_core_init: max speed:5, hibernation:0, nr_scratch:0`. No true `;init:` line or fatal/AVC was recorded. This establishes that PID1 wrapper and CACHE work under M2, narrowing failure to after wrapper start/before or during original init transition, but exact cause is still unknown. M1 with the same BOOT logs init to 3.14 s and completes boot. G965F BOOT baseline restored with full hash readback; M2 runtime gate FAIL. Evidence `logs/20261023-m2-probe06/`.

23/10/2026 — probe07 hai máy: M1 trên G965F serial `M1_G965F_SERIAL` boot hoàn tất, CACHE có `M2-PID1-PROBE-v5: monitor ready` và hai `M2-PROBE-HEARTBEAT`; M2 trên G965F serial `M2_G965F_SERIAL` không có ADB trong 240 s, CACHE có 19 marker nhưng không heartbeat. Kmsg không lọc M2 ghi được đến median ~0.326 s; không có dòng fatal chứng minh kernel, HAL hoặc init cụ thể. M1 recorder cũng dừng ghi sau ~1.397 s dù boot tiếp, nên thiếu log sau đó chưa chứng minh thời điểm reset. Hai BOOT gốc đã khôi phục, full SHA readback PASS. Evidence `logs/20261023-m1-second-probe07/`, `logs/20261023-m2-probe07/`. M2 runtime gate FAIL; root cause còn mở.

23/10/2026 — probe08: M1 control boot hoàn tất và ghi 17 heartbeat đến `ms=1761 records=4130`; M2 sau 240s không ADB, CACHE ghi 19 PID1 markers nhưng mỗi segment có tối đa một heartbeat (`ms=114 records=1536` là một ví dụ), không có `;init:` hoặc fatal rõ. Ghi theo lô loại bỏ phần lớn tác động fsync từng dòng của probe07; vẫn chưa thể phân biệt init làm child dừng với reset máy. Hai BOOT baseline được khôi phục/readback. Evidence `logs/20261023-probe08/`. M2 root cause còn mở, không đủ căn cứ sửa kernel.

### 23/10/2026 — probe09 xác nhận exec PID1 và rollback

Patch0094 thêm pipe `O_CLOEXEC` giữa wrapper PID1 và recorder để ghi lỗi `execve("/init.real")` nếu exec trả về; EOF cho biết exec thành công hoặc parent kết thúc. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0094-report-init-exec-through-cache-child.patch`. Self-test errno `ENOENT` qua pipe PASS; M1 control trên serial `M1_G965F_SERIAL` boot completed. M2 serial `M2_G965F_SERIAL` vẫn không có ADB sau 240 giây. CACHE `/cache/m2-early-boot-probe09.log` ghi nhiều lần `M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1`, không có `M2-INIT-EXEC-FAILED`; do đó `/init.real` được exec ở các lượt ghi được, nhưng log này chưa chứng minh init đã hoàn tất chuyển trạng thái. `pmsg-ramoops-0` ghi adbd khởi tạo FunctionFS/USB, sau đó transport bị shutdown; không có panic/fatal rõ. CACHE 396373 byte, last_kmsg 2097152 byte, pmsg 1817 byte; hash và bản gốc trong `logs/20261023-probe09/`. Hai máy đã được đưa về TWRP; BOOT cả hai restore về baseline SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`, full-partition readback PASS. M2 gate vẫn FAIL; nguyên nhân sau `execve` còn mở. Không sửa kernel/vendor/radio, không ghi EFS/CPEFS/modem/bootloader.

23/10/2026 — postboot USERDATA audit: G965F M2 serial `M2_G965F_SERIAL` đã ở TWRP; `/data` chỉ có `lost+found` và `media/TWRP`, không có tombstones, ANR, DropBox, bootstat, logd hay Android tree. `/cache/recovery` có log TWRP; pstore `pmsg-ramoops-0` được lấy ở phiên recovery nên adbd/USB records không chứngում Android M2 đã chạy adbd. Không có Android hậu khởi động artifacts. Chẩn đoán tiếp theo dùng các hook logger/init-debug có sẵn, bật qua build flags cho gói riêng M1 calibration rồi M2. Evidence `logs/20261023-m2-postboot-userdata/`.

## M1-INIT-DEBUG-001 — early logger snapshots partially empty (23/10/2026)

Fresh M1 build0095 booted to `sys.boot_completed=1`; cache logger recorded
post-fs/post-fs-data markers, cmdline, partitions and 15/60-second logcat, but
`init_debug_start.txt` had only the labels `cmdline:`, `mounts:`, `partitions:`;
`props_start.txt` and `dmesg_start.txt` were empty, as were timed dmesg snapshots.
Running the same installed script later in Android produced a 22,419-byte summary,
58,798-byte properties, and 386,188-byte dmesg. This narrows the difference to
startup timing/service execution context; cause is not yet isolated. Evidence is
under `logs/20261023-m1-init-debug/flash-01/runtime/`. No source patch was made
from this observation. M2 has persistent logcat/early init markers; dmesg remains
an evidence gap if the OS never reaches ADB.

## M1-EFS-HASH-001 — unexpected EFS raw hash delta after installer (23/10/2026)

M1 serial `M1_G965F_SERIAL` preflight and post-install raw hashes differ for EFS;
CPEFS, RADIO, RECOVERY and BOTA0/1/2 remain identical. The eight updater scripts
write SYSTEM/VENDOR/ODM/BOOT and format CACHE/OMR/PRELOAD/USERDATA; the embedded
DS-ACK installer targets BOOT only. EFS was not mounted during the recovery audit.
No EFS read dump, write or restore was attempted. User was notified and told us to
continue on this dev device. Source of the hash change is unconfirmed; do not infer
EFS corruption. Compare again only through an authorized read-only audit if needed.
Evidence: `logs/20261023-m1-init-debug/flash-01/postinstall/preboot-check.json`.

## 23/10/2026 — M2 attempt47

- M2-RUNTIME-ATTEMPT47 remains FAIL: full fresh install (8/8 RC=0), USERDATA UUID changed, but Android ADB did not complete within 240 seconds and G965F was returned to TWRP by hard keys. Exact captured failure line is unavailable: last_kmsg ends at bootloader `Starting kernel...` (2 MiB trace, final occurrence at `logs/20261023-m2-attempt47/flash-01/postloop-last-kmsg.bin` extracted line 38986); no kernel panic or fatal line is captured. Pstore has only 951 bytes with `adbd` FunctionFS initialization and `ArmToArm64Assembler` scanline records; boot attribution and failure point remain uncertain. TWRP `/tmp/recovery.log` is current recovery startup, not Android M2. Do not infer a specific kernel, HAL, init, or VNDK cause.
- Install audit passed: package SHA256 `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`, eight signed ZIP32 stages all CRC/signature/Zip32 PASS, each installer RC=0; final marker `next=done`. Fresh USERDATA UUID changed from `615a10c2-626c-4cd7-9bb6-65fe8d5a126b` to `b2841bc0-1371-4d89-84a7-aa71e91f9f3a`. EFS/CPEFS/RADIO/RECOVERY/BOTA unchanged between prewrite and immediate preboot hashes. EFS later read after reboot/recovery differs (`27051fae...c597b8` before vs `471ca981...b41e88` after); installer did not write EFS. No EFS restoration/write performed. Post-loop BOOT hash readback could not complete due ADB transport drops; BOOT matched baseline before reboot. Evidence: `logs/20261023-m2-attempt47/flash-01/`.
- Next diagnosis: use existing init/vendor and system boot logger build flags, first calibrate on the working M1 control with the same recorder, then reproduce M2 with no kernel/vendor/RIL changes absent a specific log.

## M2-BOOTLOG-004 — attempt48 logger files absent after bootloop (23/10/2026)

Attempt48 fresh install passed all 8/8 stage RC/order checks, changed USERDATA UUID, kept exact-size protected raw hashes unchanged, and retained BOOT baseline. After the user returned the bootlooping G965F to TWRP, `/cache/unica` and `/data/bootloop-logs` were absent. Captured `last_kmsg.bin` has 8 `Starting kernel...` bootloader entries (last extracted line 21369), but no Android fatal/panic or logger marker. Lines 14416 (`init: Received sys.powerctl='reboot,adb' from pid: 3529 (/sbin/adbd)`) and 14624 (`reboot: Restarting system with command 'adb'`) show this log includes the TWRP session used for the reboot and cannot be assigned to M2 Android. `pmsg-ramoops-0` contains repeated adbd/FunctionFS recovery records and no `M2-`/`UN1CA` marker. Cause remains unknown; no evidence to alter kernel, Hybrid Vendor, or radio. Next attempt will enable the fork's existing vendor init phase trace so `/dev/kmsg` markers can be matched against existing PSTORE_CONSOLE support. Evidence: `logs/20261023-m2-attempt48-init-debug/postloop/`.


## M2-BOOTLOG-005 — attempt49 trace payload installed but no attributable Android log (23/10/2026)

Attempt49 builds the fork's existing vendor init trace plus the attempt48 init/system loggers. Build, image filesystem, signed ZIP and all eight legacy stage checks passed; fresh install completed 8/8 with RC=0, fresh USERDATA, BOOT baseline and protected partition hashes unchanged during install. The 240-second watcher recorded `2026-10-23T17:04:35+07:00 state=absent` and `2026-10-23T17:08:36+07:00 watch_end`. Postboot TWRP capture is complete. A read-only ro,noload mount of installed VENDOR /dev/block/sda19 confirmed the trace rc with all eight /dev/kmsg actions; exact copy and SHA256 4da50683a0fd7aa3d64951c4961482e70e1de75deb0c44d2d6a57fd8fc627b32 are in postboot/installed-vendor-trace.rc. This confirms installation only, not that Android init parsed or executed the rc. /proc/last_kmsg is 2 MiB with 68 bootloader `Starting kernel...` entries (last at line 38096) and no following kernel console or Android phase/fatal log. Pstore has only a 951-byte recovery-associated adbd/graphics pmsg; console-ramoops and CACHE logger files are absent. Captured sec_log line 4537 says `init: Command 'mount functionfs adb /dev/usb-ffs/adb uid=2000,gid=2000' action=fs (/init.rc:55) took 0ms and failed: mount() failed: Device or resource busy`; it is from TWRP recovery init, not Android M2. Root cause remains unknown; no kernel or radio change is justified. Attempt48 predates patch0095, so its missing init logger is consistent with the vendor restore deleting it. Attempt49 does not: its build log at lines 1712–1713 includes metadata for both files, and read-only mounts of installed VENDOR confirmed both files exist. Thus patch0095 fixed final-image omission but did not explain attempt49 missing CACHE output. Installed /system/bin/init is an Android 36 ELF64 AArch64 PIE with the bootstrap linker; static dependency file checks pass for the listed non-bionic libraries and bootstrap libc/libm/libdl, but runtime linker/SELinux success remains unproven. Probe09 line 3 reports M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1; this confirms only that exec did not return with an error through that pipe, not that second-stage init started. No retained M2 log contains Attempted to kill init; do not call this a confirmed kernel panic. Rebuilding identical attempt49 flags as attempt50 would repeat the same test. EFS was stable through attempt49 install (`76b914…`) but two consecutive read-only device hashes after boot/TWRP return are `a0892e10e469aa350be69fcf597a0f6b04e2a915d35d81e552e00e1f172c9040`; cause/timing unknown and no EFS write or restore was performed. Evidence: `logs/20261023-m2-attempt49-vendor-trace/`.


## M2-LINKER-001 — A37 Android 16 linker aborts on DS-ACK kernel (23/10/2026)

Direct execution of A37 linker64 from attempt49 on M1 G965F running the pinned DS-ACK 4.9.337 kernel aborts before loading init: `arc4random data MADV_WIPEONFORK failed: Invalid argument`, exit 134. M1 native linker on the same kernel lists native init and exits 0. The same failure was reproduced against the actual installed M2 SYSTEM mounted `ro,noload` in TWRP; `init`, `sh`, and `secilc` each print `libc: arc4random data MADV_WIPEONFORK failed: Invalid argument` and abort with exit 134 under the TWRP 4.9.118 recovery kernel. The M1 run remains the evidence for the pinned DS-ACK kernel behavior. This establishes that the A37 linker requires MADV_WIPEONFORK behavior the current 4.9.337 kernel rejects with EINVAL; it does not indicate SIGILL or prove later SELinux behavior.

A same-device control copied M1 Android16 bootstrap linker/libc/libm/libdl/libdl_android to TWRP tmpfs and bind-mounted them over only the matching paths in a chroot view of the read-only installed M2 SYSTEM. Under the M1 loader, A37 `init` and `sh` dependency listing exited 0; A37 `sh -c 'echo M2_LOADER_SH_OK'` ran and exited 0. `secilc --help` reached usage output but returned 1, so no successful policy compile is claimed. The test did not execute `init selinux_setup` or `second_stage`. It supports a M2-scoped bootstrap overlay as a new diagnostic candidate, but does not prove it will boot. Probe10 remains gated and was not run. No kernel/vendor/radio/EFS/partition writes or reboot occurred; temporary mounts were removed. Evidence: `logs/20261023-m2-attempt49-vendor-trace/postboot/m1-linker-comparison.txt`, `logs/20261023-m2-attempt49-vendor-trace/postboot/linker-runtime-check.txt`, and `logs/20261023-m2-linker-runtime-step1/runtime-test.txt`.

## 24/10/2026 — probe10 wait status bug invalidates apparent compile stall

The actual A37 secilc/CIL compiles on recovery and DS-ACK with exit0 and the
same policy hash; this does not test loading it into the kernel. Probe10 used
`status & 0x7f` as a signal, returning127 at a normal ptrace stop. A real
SIGCHLD test reproduces `OLD-DECODE signal=127 PTRACE_CONT errno=5`; old code
ignored this resume error. Thus probe10 can freeze its tracee after the compiler
child exits. The corrected test resumes with WSTOPSIG17 and passes. Source and
limits: `diagnostics/`; evidence: `support/evidence/probe11-selftests.txt`.
Corrected probe11 and passive probe12 subsequently ran; see the dated result below. Underlying M2 bootloop remains unresolved.


## 24/10/2026 — corrected probe11 and passive probe12 reach policy loading

Probe11 captures compiler SIGCHLD `child_status=0`, `init: Loading SELinux
policy`, then kernel `SELinux: Completing initialization.` and `SELinux:
Setting up existing superblocks.` across eight boot sequences. No resume error
was captured after the WSTOPSIG correction. Passive probe12 uses no PID1 ptrace
and still bootloops: 14 first-stage starts, 11 load-policy markers and 10
existing-superblock markers in 5,151,108 bytes. Representative exact lines and
raw-log hashes are in `support/evidence/probe11-init-policy.txt` and
`probe12-init-policy.txt` plus their summary JSON files.

Neither run captured a second-stage marker, explicit fatal/panic, or successful
return from load_policy. The five `SELinux: __security_genfs_sid: genfs || cmp`
messages are emitted on an ENOENT path in the pinned kernel source; the text
alone does not establish a panic. Both BOOT tests were restored to baseline
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.
Protected hashes were unchanged during each rollback. EFS raw hash changed
across each boot/recovery cycle; no EFS write/restore command was performed,
and the cause remains unverified. Other measured protected partitions match.

Recorder limitations remain: proc paths become ENOENT after SwitchRoot;
CACHE write/fsync results and negative kmsg reads are not persisted. The kernel
revalidates file permissions when the SELinux policy sequence changes, even
for already-open descriptors. This supports testing recorder visibility before
inferring a hang at the last persisted line. See `diagnostics/probe13-plan.md`.
No kernel/vendor/radio change or full ROM rebuild was made for probe11/12.


24/10/2026 — offline recorder access check: user-space libsepol against the
actual compiled M2 policy returns `requested permission append denied by
te-rule` for kernel -> unlabeled:file, and likewise denies write. Recovery
reports both saved probe11/12 CACHE files with an unlabeled xattr. Runtime
monitor SID/enforcement/inode labels remain unconfirmed; this is a recorder
visibility hypothesis, not a captured AVC or proven bootloop cause. M1 DS-ACK
has no pmsg node, unlike recovery. No live policy load, edit, enforcement
change, device write or reboot was performed. See
`diagnostics/offline-recorder-policy.md` and selected query evidence.


24/10/2026 — probe13 M1 calibration PASS. The passive recorder now preallocates
and mmap-writes a dedicated regular CACHE file, with checksum frames and checked
msync/fsync. Actual namespace/detach/FD-close/error tests pass on recovery and
DS-ACK. M1 boots completed1/enforcing and observes recorder u:r:kernel:s0;
`KMSG_READ_ERROR=-13` and `VFS_WRITE_RC=-13` confirm runtime access loss for
ordinary I/O. A one-time correction of the recorder-private kmsg inode label
succeeds, kmsg reading resumes, and `init: init second stage started!` is saved.
Mapped output survives recovery with msync0/fsync0, heartbeat68410ms and zero
dropped frames. Thus the old recorder's visibility boundary is real on M1,
not proof of a fatal at policy loading. M1 BOOT rolled back to baseline with
full hash verification and returned to Android. M2 probe13 test is now in
progress after fresh serial/model/hash checks; no M2 result is claimed yet.
No kernel/vendor/radio/policy/normal-ROM change or data format. Local evidence:
logs/20261024-probe13/; selected evidence: support/evidence/probe13-m1-*.


24/10/2026 — probe13 completed; preceding in-progress entries are closed.
M2 persisted30 checksum-valid boot slots, all reaching second-stage init.
23 retain `Service boringssl_self_test64_vendor has 'reboot_on_failure' option
and failed, shutting down system.` and22 retain `Got shutdown_command
'reboot,boringssl-self-check-failed'`. This is an explicit reboot trigger;
the old policy boundary was a recorder failure. Normal runtime APEX linker
from installed M2 independently reproduces MADV_WIPEONFORK EINVAL/exit134 on
both recovery and pinned DS-ACK. Bootstrap-only patch0096 leaves that runtime
untouched. A37 init's linkerconfig path is in APEX, so missing system-bin
linkerconfig alone is not an established cause of its exec ENOENT.
Both BOOT rollbacks verify baseline cc618abc...776ed01d; M1 boots baseline,
M2 remains in TWRP, N untouched. Protected hashes match during rollback; EFS
changes across boot/recovery remain unverified with no EFS write/restore.
No kernel/vendor/radio/policy/self-test change or ROM rebuild. Full report,
limits and next implementation decision: docs/probe13-report.md. Selected
counts, exact lines, source hashes and loader outputs are in support/evidence/.

## 24/10/2026 — M2 BoringSSL loader kernel interface, backport under test

Probe13 established the actual restart path. The exact installed A37 runtime
APEX linker emitted `arc4random data MADV_WIPEONFORK failed: Invalid argument`
and exited134; vendor init then logged `boringssl_self_test64_vendor ...
received SIGABRT` and `reboot_on_failure`. The pinned4.9.337 kernel's madvise
returned EINVAL. This is direct evidence for a narrowly scoped kernel
compatibility backport; it is not evidence of a broken crypto algorithm.

The isolated DS-ACK V1.12 source build adds the upstream WIPEONFORK semantics
and its prerequisite/follow-up. M1 calibration booted enforcing, a raw AArch64
test passed 32 parent/child/grandchild rounds, KEEPONFORK and invalid mapping
checks, and the exact A37 linker `--list` exited0. The self-test's crypto main
has not been validated yet. M1 has been returned to original BOOT hash
`cc618abc...776ed01d`; M2 candidate is now being tested with the same probe13
ramdisk. See `docs/m2-runtime-kernel-fix.md` and its retained image/build audit.
Open items: confirm M2 reaches Android, observe vendor self-test completion,
inspect linkerconfig execution, and restore M2 BOOT baseline after capture.

## 24/10/2026 — M2 next boot gate after WIPEONFORK fix: Android 25Q4 BPF

Feature BOOT `b01912f...` was tested and then rolled back. Probe13 generation33
has 6,446 valid frames, heartbeat17,773ms, zero dropped frames, and captures
second-stage init. `linkerconfig generated /linkerconfig with mounted APEX
modules info`; vendor and system BoringSSL self-test services both exit status0.
The next failure is exact: `/cache/unica/logcat_live.txt:24286` reports
`NetBpfLoad: Android 25Q4 requires kernel 5.10.` Then line24287 records
`Service 'bpfloader' ... exited with status 7`, and lines24291–24292 record
`reboot_on_failure` and `reboot,bpfloader-failed`. Thus M2 still cannot boot,
but the earlier linkerconfig and BoringSSL blockers were cleared by this
controlled kernel test. APEX version is361153320 and reports API36/3610. The
loader logs a synthetic minimum 5.4.302 while the actual kernel release remains
4.9.337; spoofing a string is not an acceptable fix because actual BPF
features are used. M2 BOOT returned to `cc618abc...776ed01d`; protected hash
snapshot and guarded no-change audit are retained. Next: inventory exact
25Q4 BPF object requirements against DS-ACK4.9 before considering a separate
kernel backport or a compatible APEX. No SELinux relaxation or reboot-on-failure
bypass. Details: `docs/m2-runtime-kernel-fix.md`.

Candidate narrowing: the retained S22 M1 Tethering APEX is version360743340
(2025Q2/API36.0); on the same DS-ACK environment it logs `detected 5 of 5:
2025Q2 api:36.0.0` and `bpfloader` exits0. A37's APEX is361153320 (25Q4/API36.1)
and enforces kernel5.10. Their BPF object layouts differ. Thus a single-module
APEX downgrade is a plausible reversible experiment, but cross-version Android
API and hotspot/IPv4/IPv6/DHCP/DNS/data-saver behavior remain unverified. Do not
adopt based only on bpfloader exit0. APEX hashes/layout: runtime kernel report.


## 24/10/2026 — RANGING crash traced to metadata storage; diagnostic Welcome boot

Prior log `logs/20261024-m2-runtime-fix/m2-v6-test/logcat-first-adb.txt:17008` reports `Failed to load aconfig package android.permission.flags`; line6032 fails copying package.map into read-only /metadata. At63013, AppOps throws `Missing permission definition for permission "android.permission.RANGING" associated with app op 151`. Installed framework-res matches final staged hash and declares the permission behind ranging_permission_enabled; generated AppOps code returns true. Volatile tmpfs before donor aconfigd fixes flag table availability and permission registration. M2 reaches boot_completed1/Welcome in77.7s, Enforcing, stable SystemServer through uptime156.61s. Controlled test and full rollback complete.

Remaining exact failures: new `runtime/logcat-boot.txt:6058` says `Failed to initialize platform storage in post-fs: fail to get storage files for vendor`; line10703 says `CANNOT LINK EXECUTABLE "/system/bin/cameraserver": cannot locate symbol "_ZN7android11AudioSystem22streamTypeToAttributesE19audio_stream_type_t"`. The pinned legacy native server imports the symbol; A37 libaudioclient does not export it. No camera/audio fix applied. tmpfs is volatile and is not an accepted persistent metadata solution. Full details and rollback hashes: docs/m2-metadata-runtime.md. All SYSTEM edits restored; baseline BOOT verified; M2 in TWRP.

## 24 October 2026 — camera ABI and persistent metadata fixes

The former camera/audio symbol failure, exact80500 Camera exception and missing
native32 GLES1 wrapper are fixed in scoped source patches0099/0101 and actual
rear/front preview/JPEG smoke tests pass. The first errors were
`CANNOT LINK EXECUTABLE "/system/bin/cameraserver": cannot locate symbol "_ZN7android11AudioSystem22streamTypeToAttributesE19audio_stream_type_t"`,
`java.lang.IllegalArgumentException: get : argument is invalid - 80500`, and
`dlopen failed: library "libGLESv1_CM.so" not found`. Exact source/line references
and candidate hashes are in docs/m2-camera-metadata-fixes.md.

Patch0100 retains the existing CACHE metadata bind after final vendor restore
and labels both paths. Marker hash/inode persists through reboot. Empty v2
storage truthfully describes flagless vendor; platform aconfigd exits0 instead
of `fail to get storage files for vendor`. This does not add A37 hardware flags.

Remaining: complete combination is not yet a packaged ROM; production kernel
selection stays pinned. Optional camera vendor tags warn; video/audio/telephoto,
advanced modes, storage wipe/OTA/staged flags, fresh ADB defaults/switches,
connectivity and N remain unaccepted. All live test files/BOOT rolled back;
M2 stays in TWRP. No EFS/CPEFS/radio write issued.
