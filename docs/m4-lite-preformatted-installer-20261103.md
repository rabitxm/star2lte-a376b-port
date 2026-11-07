# M4 Lite — installer cho USERDATA đã format

03/11/2026: người dùng yêu cầu một ZIP Lite mới bỏ format USERDATA ở cuối.
Lý do: TWRP đọc ZIP trên `/sdcard`, là một phần của USERDATA, nên unmount
phân vùng đang chứa ZIP có thể thất bại. Log trước đây có `umount: can't
umount /sdcard: Device or resource busy` và `E9810: could not unmount data
aliases` trong `logs/20261022-m2-g965f-flash-01/twrp-stage08-format-failure.txt`.

## Thay đổi và phạm vi

- `rom/scripts/build_flashable_zip.sh` thêm mode `--preformatted-data`, chỉ
  nhận profile star2lte M4 Lite và input ROM hoàn chỉnh đã kiểm chứng.
- `rom/scripts/internal/build_preformatted_lite_zip.py` khóa SHA-256 source
  `5e84d7e4f9a1fa4990a4b981465655eccb7b3ab71dfbb901d8078791a98815d5`,
  giữ nguyên byte nội dung của mọi ZIP entry ngoại trừ ba file installer:
  `META-INF/com/google/android/updater-script`, `exynos9810/format_partitions.sh`
  và `exynos9810/preflight.sh`. Không build ảnh, kernel, radio hay vendor.
- Updater không unmount hoặc format `/data`; mode formatter mới chỉ format
  CACHE/OMR/HIDDEN. Gói yêu cầu USERDATA đã format trước, không tự kiểm chứng
  được data sạch. Layout được kiểm tra *trước ghi image*, không tự repartition;
  preflight yêu cầu đủ kích thước không có sai số 16 MiB. Trên layout chưa đủ,
  installer dừng và phải chuẩn bị layout riêng trước khi format/copy ZIP.
- `rom/scripts/internal/verify_exynos9810_flashable.sh` thêm gate riêng cho
  mode này, giữ nguyên gate bắt format USERDATA của gói thông thường. Gate ZIP32,
  chữ ký OTA và so sánh SHA-256 nội dung của mọi entry còn lại vẫn bắt buộc.

## Cách kiểm chứng

Kiểm cú pháp shell/Python; build bằng fork, chạy gate flashable/ZIP32, kiểm CRC,
chữ ký OTA, hash và so sánh nội dung từng entry giữa hai gói; kiểm guard layout
và nhánh formatter. Gói cũ giữ nguyên. TWRP thực tế chưa xác minh khả năng đọc
một ZIP32 3–4 GiB: host PASS không chứng minh recovery cài được một lần.

Quy trình dự kiến khi đã có layout đủ: TWRP **Format Data**, reboot lại TWRP,
mount data, chép ZIP vào bộ nhớ trong, kiểm hash rồi Install ZIP một lần.
ROM ZIP sẽ còn trên USERDATA sau cài. Không flash biến thể này để thử trong
lượt build khi người dùng chưa yêu cầu flash bản mới.

## Kết quả build

Gói hoàn chỉnh: `rom/out/releases/M4/star2lte-preformatted-1793692940-kKZ7vE/M4-Lite-Preformatted-Internal-ZIP32-sign.zip`.
Kích thước 4.060.299.475 byte; SHA-256
`fa600a7395eaddf5e5f2cd782cbd28b755ba83f09f7de3fa5e72c55947ae614c`.
`SHA256SUMS` đã đọc lại PASS. Build log
`logs/20261103-m4-preformatted-build-r2.txt` ghi `PASS: source payload entries
unchanged; only updater, formatter and layout preflight modified`, verifier
flashable PASS và `PASS M4 Lite ZIP32 size, entry versions/offsets, CRC and
OTA signature: 4060299475`. Gate bổ sung cho nhánh bảo toàn USERDATA đã chạy
PASS: `logs/20261103-m4-preformatted-enhanced-verification.txt`. Tại thời điểm
build, bản gốc và thiết bị đang chạy được giữ nguyên; lượt cài sau được ghi bên dưới.

## Thử cài một ZIP trên G965F

Sau chỉ thị flash, TWRP 3.7.0_9-0 đã cài một ZIP thử 4.061.319.222 byte có
updater vô hại, đọc thành công một entry ở offset 4.061.316.102; nội dung
entry đối chiếu SHA-256 khớp. Log có `M4 ZIP32 TAIL READ PREFLIGHT PASSED`.
Đã format USERDATA bằng hàm unmount/mke2fs của fork; UUID đổi và mount chỉ đọc
thấy `lost+found`. Reboot TWRP, chép ZIP ROM vào `/data/media/0` và xác nhận
SHA-256 trên máy khớp gói nguồn.

Một lệnh TWRP Install duy nhất trả RC=0 sau 422 giây; log ghi `USERDATA is
preserved` và `script succeeded`. Hash ZIP và UUID USERDATA sau cài khớp lúc
trước install; GPT primary/backup, CPEFS, RADIO, RECOVERY, BOTA và BOOT khớp
baseline, e2fsck SYSTEM/VENDOR/ODM PASS. Android lần boot đầu đạt
`sys.boot_completed=1`, SDK 37, public model SM-A376B, physical SM-G965F,
SELinux Enforcing; Setup Wizard chưa hoàn tất. Chưa kiểm chức năng tổng thể.

Hash raw EFS khác giữa baseline TWRP đầu và sau cài. Có một lần reboot TWRP
giữa hai phép đo mà không đo EFS xen giữa; dmesg TWRP sau reboot ghi
`EXT4-fs (sda3): mounted filesystem with ordered data mode`, sau đó có mount
`Opts: noload` của vendor bootfix. Không đủ bằng chứng xác định lúc/nguyên nhân
đổi hash, không kết luận dữ liệu EFS bị hỏng. Hash sau cài đọc lặp lại ổn định;
không khôi phục EFS. Bằng chứng riêng tư ở
`logs/20261103-m4-onezip-flash-M1_G965F_SERIAL/`.
