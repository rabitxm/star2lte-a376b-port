# Nguồn và provenance

Ngày audit: 20/10/2026. Thông tin máy là dữ liệu đọc trực tiếp, không lấy kế hoạch
cũ làm bằng chứng đã kiểm chứng firmware/kernel.

- Fork: https://github.com/Eend15/Unofficial-Un1ca-9810, Android-16,
  commit 2b25ae6fc3098650be249bd1898e21e563cd5df1. UN1CA của salvogiangri là upstream.
- Kernel: https://github.com/duhansysl/exynos9810-kernel/releases/tag/V1.12.
  Release mô tả OneUI7 cho Android 15–16; Android 17 **cần xác minh bằng log**.
- Gói gốc đã tải: `inputs/kernels/DS-ACK-V1.12-08.05.2026-Enforcing-OneUI7.zip`.
  SHA-256 thực khớp digest công bố của asset:
  `9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072`.
  CRC PASS; đủ entry G965F/G965N kernel+dtb (các entry là binary delta BSDIFF40,
  cần dựng với baseline G960F, không copy delta như Image). DTB baseline không
  có marker erofs. Đã tái dựng delta gốc F/N để audit, không build kernel; chưa boot hoặc khẳng định enforcing thực.
- Source kernel release liên kết commit 914eabe7b6799ab0eb159360a5f9731dd50d0f1d.
  Chưa clone source kernel; không cần build khi chưa có log chứng minh nhu cầu.
- Fork có gói tháng 9/10 build từ Eend15/exynos9810-kernel; xem prebuilt/README.md.
  Phải phân biệt nguồn này với gói gốc duhansysl; các ZIP trong checkout ban đầu
  là LFS pointer. OID/size kỳ vọng được lưu trong sources.lock.json, không phải
  hash payload đã xác minh.

## Firmware đã có trên PC

`SAMFW.COM_SM-A376B_XXV_A376BXXU5BZI6_fac.zip`, 9016689387 byte.
SHA-256: `b152ca2c87c1b54aee6f606f436f52076633e894a7dea330ee1da87f5117b415`.
`unzip -t` PASS tất cả sáu entry, kết thúc bằng
`No errors detected in compressed data of ...`.
Tên AP ghi meta_OS17; tar chứa boot/init_boot/vendor_boot/dtbo/super/vbmeta,
fota.zip và super_used_size.txt. Đã kiểm kê nội dung system/framework/APEX
như dưới đây. CRC/hash không thay cho xác thực chữ ký Samsung.
Giữ BL/CP/USERDATA A37 ngoài payload cài lên S9+.

Cập nhật kiểm kê: đã đọc `meta-data/fota.zip` trong AP. `SYSTEM/build.prop`
quan sát được `ro.build.PDA=A376BXXU5BZI6`, `ro.build.version.release=17`,
`ro.build.version.sdk=37`, `ro.build.version.oneui=90000`, ABI system arm64-v8a
và abilist32 rỗng. Log tại logs/20261020-m0-donor-a376b/SYSTEM_build.prop.
FOTA vendor/boot props lại ghi release14/SDK34; đây là metadata theo partition,
chưa được dùng làm kết luận về framework thực hay tính toàn vẹn firmware.
Đã đối chiếu properties trực tiếp từ system_a.img: khớp FOTA. EROFS system,
product, odm đều qua fsck --extract; hash ba image tách bằng 7-Zip khớp từng
extent byte tương ứng từ super raw theo lpdump. lpdump --all đọc được ba slot.
7-Zip từng báo Headers Error; không dùng riêng kết quả 7-Zip để bỏ kiểm metadata.
framework.jar, services.jar, runtime APEX đọc và CRC PASS; framework.jar đã
decode bằng apktool. Build$VERSION_CODES có `CINNAMON_BUN:I = 0x25` (37).
Đây là kiểm chứng tĩnh nội dung, chưa phải bằng chứng boot/hardware Android17.

Đã thử GET version.xml của Samsung cho S901B/EUX, G965F/PHN, G965N/KOO và
A376B/XXV bằng đúng URL mẫu GET_LATEST_FIRMWARE của fork: cả bốn trả
`curl: (22) The requested URL returned error: 403`. Chưa có metadata online
mới ở lần thử đó. Sau khi đối chiếu samloader, đã sửa hàm của fork dùng
User-Agent Kies2.0_FUS: metadata cả bốn model/CSC đều đọc được. S22 EUX có
S901BXXSOGZH3, Android16; A376B XXV có A376BXXU5BZI6, Android17; N KOO có
G965NKSU5FVG2/G965NOKR5FVG2/G965NKOU5FVA1, Android10. F PHN mặc định fork
có FVG4, khác FVG6 trên máy thật. Không gán CP từ metadata cho baseband thực
trong recovery. S22 đã tải/giải mã và kiểm MD5 năm file Odin thành công; SHA256 từng file ở
logs/20261020-m0-buildhost/s22-downloaded-manifest.json. Target F tải thất bại:
`download-fw.txt:31`: `DownloadBinaryInform returned 408, firmware could not be found?`
Request mẫu mới vẫn Status=408, không có BINARY_NAME (g965f-fus-retry.json).
Người dùng đã cho phép dùng định danh G965F để tải từ Samsung. Đọc riêng
Samsung serial qua EFS ro,noload, rồi unmount; request mới Status=200 và có
BINARY_NAME (g965f-fus-device-serial.json), pipeline đã tải/giải mã/MD5 năm file Odin PASS, hash trong
g965f-downloaded-manifest.json. Không lưu giá trị
định danh vào tài liệu/Git/chat; không kết luận chỉ từ mã lỗi của request mẫu.

Đã có bản sao VENDOR live của F/N trong inputs/baselines và manifest hash tại
logs/20261020-m0-g965f, logs/20261020-m0-g965n. Properties lần lượt nhận build
G965FXXUHFVG6 và G965NKSU5FVG2, SDK29. Đây là ứng viên blob từ máy hiện tại,
chưa chứng minh byte-identical với package stock chuẩn. Một số blob có hash
khác nhau giữa F/N (libsec-ril.so, libexynoscamera3.so); không tráo lẫn tự động.

## Đầu vào còn cần

| Đầu vào | Trạng thái |
| --- | --- |
| Payload LFS của fork | 4464/4464 path đã xác minh hash/size; 3224 object duy nhất, 3528273919 byte |
| Submodules của fork | Đã init 41 đệ quy đúng gitlink; tool build áp patch dependency, snapshot trong sources.lock.json |
| S22 Android 16 cho M1 | S901BXXSOGZH3/EUX/o=16 tải/MD5 PASS; SHA256 từng Odin file trong manifest; extract PASS bằng nguồn F2FS đi kèm, metadata năm partition/CRC524 archive PASS |
| Stock G965F dùng extraction | Tải/giải mã/MD5 năm file Odin PASS; SHA256 manifest; trích/metadata và CRC522 archive PASS |
| Stock G965N dùng blob/cấu hình N | Chưa có package đã hash. Bootloader N là G965NKSU5FVG2; không suy đây là modem hoặc hash vendor |
| A37 Android 16 cho M2 | Chưa có package đã xác minh; version.xml có sáu build lịch sử nhưng không ghi Android từng entry, không suy từ suffix |
| Gói khôi phục và backup F/N | Người dùng xác nhận đã backup cả hai; chưa kiểm nội dung; không thu EFS/IMEI vào repo |

Firmware, vendor và kernel binary chỉ lưu cục bộ trong vùng bỏ Git. Không phát
hành image chứa firmware Samsung; chia sẻ patch/script và provenance thích hợp.

## Công cụ F2FS dựng từ nguồn của fork

Tracked binary f2fs_unpack khớp byte commit pin nhưng exit139/Segmentation fault
với --help. Dùng nguyên nguồn f2fs_unpack_src (module Sushrut1101/f2fs-extractor)
qua external/make.sh, output ngoài Git ở out/tools/bin; không sửa nguồn Go hay
kernel. Source/tool/hash/runtime cục bộ và audit S22 được khóa trong tools.lock.json
và logs/20261020-m0-buildhost/f2fs-tool-checks.json.

Đã khôi phục payload F/N từ chính base+BSDIFF40 delta trong ZIP May bằng
bspatch Ubuntu chạy trên PC, chỉ để audit. SHA/size/config ở
may-kernel-payload-audit.json; ZIP gốc không đổi. Cả hai có IKCONFIG và PSTORE;
page-budget gồm kernel/DTB/ramdisk khoảng40MB, dưới BOOT55MiB. Đây không phải
chạy MagiskBoot repack hoặc bằng chứng boot. Không build/patch tùy biến kernel.

Patch0011/0012 bổ sung đóng gói path và khóa model star2lte. Toàn bộ12 patch
replay từ commit gốc,20 file byte-identical; không có delta kernel tự tạo.
Lượt7 dừng chủ động trước package để chặn shared-codename bypass; lượt8
package-staged tái xác minh output attempt5, giữ provenance donor/kernel.


Patch0017 thêm chính sách ADB lâu dài theo chỉ thị user, không thêm binary nguồn
ngoài. Phân tích từ donor M1 đã khóa: SettingsProvider, AdbService Lifecycle,
SecSettings AdbPreferenceController, SUW B2/f và Rampart manifest/Z0/a.
Rampart giữ nguyên payload/chữ ký; inspection decode nằm vùng inputs bỏ Git.
SDK17 cần kiểm guard và log riêng, không coi code M1 là chứng minh tương thích.
Replay17patch/28file PASS; hash patch trong sources.lock.json; docs/adb-defaults.md.


21/10/2026: Patch0018 installer-format guard xuất từ đúng fork hiện có,
hash trong sources.lock.json; replay đủ18patch/29file khớp byte. Chỉ là patch
script cục bộ, không đổi nguồn kernel/firmware. Artifact flash-03 vẫn là ADB17
SHA256 ba4ed009bd984ab647bd4316700081d8f6db54b592b0f005fac2f2fcc5d3aebb;
format fresh thực hiện bằng TWRP sau sideload, có UUID/absence audit riêng.

## M2 A376B Android16 — nguồn đã chọn, chưa tải xong

Samsung update note https://doc.samsungmobile.com/SM-A376B/038051260423/eng.html
liệt kê A376BXXS5AZH4 Android16, phát hành08/09/2026, security patch05/08/2026.
SamFW https://samfw.com/firmware/SM-A376B/XXV/A376BXXS5AZH4 liệt kê Full Files
8,4GB, AP A376BXXS5AZH4 / CSC A376BOLM5AZH4, OneUI8.5, MD5 kỳ vọng
7ffba2650a25ae900424826b8dc997d6. Chưa coi MD5 trang là hash file đã tải, SDK/
OneUI thực cần đọc payload. User sẽ tải ZIP nguyên vào dự án rồi báo lại.
FUS history requests identity rỗng trả Status408; SamFW UI Calling Heimdall
chưa tạo download. Evidence logs/20261021-m2-inputs, docs/m2-build.md.
A37Android17 đang có giữ nguyên, không dùng thay donor M2.

Patch0019 không thêm nguồn binary, chỉ mở rộng packaging fork. M1 archive
ba4ed009bd98 đủ full/target-files/8legacy, manifest logs/20261021-release-retention/
m1-archive-manifest.json. Nguồn patch hiện19, ROM archive vẫn17; không gán
patch0018/0019 cho payload đã boot. Xem docs/release-retention.md.

## A376B16 đã nhận và kiểm — 21/10/2026

File nguồn /home/whitemmo/Downloads/SAMFW.COM_SM-A376B_XXV_A376BXXS5AZH4_fac.zip,
9.015.573.471byte. MD5 7ffba2650a25ae900424826b8dc997d6 khớp kỳ vọng;
SHA256 fe633153bfa05b042943321c410c0e5a761ac03d77a9f42ab9014f61610d4550;
CRC toànZIP PASS, MD5 trailer cả6Odin PASS. ZIP gốc không di chuyển/ghi đè.
Import vào rom/out/odin/SM-A376B_XXV; manifest hash từng tar.md5 ở
logs/20261021-m2-inputs/downloaded-manifest.json. Descriptor LOCAL không identity.

User trích sudo exit0; .extracted đúng A376BXXS5AZH4/A376BOLM5AZH4/A376BXXS5AZH4.
Sáu partition metadata và CRC607 APK/JAR/APEX/CAPEX PASS. Props thực system:
release16, SDK36, SDK_full36.1, OneUI80500, security_patch2026-08-05.
Vendor donor ghi SDK34/VNDK34, product.first_api_level36, board.first_api_level34;
không đồng nhất các trường này hoặc chép vendor A37 lên đích. Hybrid VNDK33
của fork và blob F vẫn là contract M2. Không dùng BL/CP/init_boot A37 trên S9+.
Evidence extracted-audit.json, system-build.prop và firmware.lock.json.
S22 cấu hình giữ nguyên; patch20–22 không thêm kernel/vendor binary mới.

Patch0030 is recorded in `patches/fork/series` and `sources.lock.json` with its
SHA256. It contains only local source changes for the A376B AZH4 SehLights
compatibility path; no donor firmware payload was copied into the patch.


## Continuation audit — 21/10/2026

Local audit patch0030 metadata was repaired (unified hunk count and new-file index headers); its SHA in sources.lock.json was refreshed. No firmware/recipe payload changed for this repair. All 40 local patches now replay from pinned fork commit to 40 byte-identical source files (`patch40-replay.json`).

Stagefright proof: original AZH4 SHA256 `1567ef98eb1ee090c14289c6a1b8cf87e9078a8c5a67495178874e3f998db2e5`; isolated four-byte native patch copy SHA256 `8e840fd1147b6b84310f19e01ade2cbb9d341541a80debfbbe1d5ebae9339463`. This isolated audit copy is not the final ROM library (other existing camera edits may also apply). Before/after disassembly is in logs/20261021-m2-build/stagefright-disassembly/. No kernel source/binary or Samsung input was changed.


Product-feature provenance update0043/0045 (21/10/2026): the actual staged BiometricSetting APK matches fork embedded star2lte compatibility payload SHA256 `bc64ca2e5083ae651a57e523eb68496ccf6f9dcaeabfed32c3adc1998acb877c`, distinct from raw AZH4 APK SHA256 `21408c1ccf899e35f7a6679442c4cff123ea0ae96e7ad06084886ef7dbcd7aa5`. Patch0043 adapts this actual overlay version. HW mDNIe method source is the existing fork `product_feature/mdnie/hw/services.jar/0001-Disable-HW-mDNIe.patch`, SHA256 `0d81648383615d03218463e4da7117ff269e329e12e59dbfb076749dfa653557`; complete old method bodies are extracted/validated by0045. No Samsung firmware binary, kernel payload, or other full framework is added to the audit patch series. All48 patches have locked hashes and replay to44 source files.


Patch0053 model provenance: A376B vendor/etc/VslMesDetector/moire_detection.tflite SHA25688e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1 equals S22 legacy Midas file; copy still comes from locked A376B firmware. Named hs_segmenter info/model pairs remain together from that donor, with model_path validated. Wi-Fi0051–0052 retain donor classes and target HAL/blobs; no other framework or native Wi-Fi payload imported. Audit series53/47 replays with locked SHAs.


24/10/2026 probe13 source/evidence provenance: native runtime linker was read
from the actual installed M2 SYSTEM's com.android.runtime.apex. APEX SHA256
`a4f643533716a6ef7823b6758cde2bdad70c1249ae16c84e1dca2ed91eb49b7f`,
linker64 SHA256 `b22c6d701616a74a51872906c12aa327402b5c2075dc9c33a0f7f8859c38c512`.
The exact vendor self-test ELF was copied only for a temporary loader-list
check on recovery and pinned DS-ACK; both reproduce MADV_WIPEONFORK EINVAL /
exit134. No proprietary payload is exported. Existing source lock, kernel,
normal fork series and retained ROM artifacts are unchanged. See
support/evidence/probe13-runtime-linker.json and docs/probe13-report.md.

## 24/10/2026 isolated DS-ACK diagnostic backport

Runtime justification and exact source/toolchain/config provenance are recorded
in `docs/m2-runtime-kernel-fix.md`. The original source input and normal
`rom/` kernel package are preserved. Diagnostic series `patches/kernel/series`
applies to duhansysl V1.12 source commit `914eabe7b6799ab0eb159360a5f9731dd50d0f1d`.
Three textual patches affect seven kernel source files. The two paired builds
use identical real `.config`, pinned AOSP clang-r547379, and unchanged
`4.9.337-DS-ACK-V1.12-G965F-08.05.2026` release string. Source embeds
stock_defconfig in `/proc/config.gz`; record real `.config` hash instead of
treating that embedded config as the release build config. Static and M1
functional gates pass; M2 device runtime remains pending. No kernel binary,
firmware or raw log is in source sharing.
