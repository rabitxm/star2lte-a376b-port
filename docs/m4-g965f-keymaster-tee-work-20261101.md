# M4 G965F — Keymaster EFS guard và chẩn đoán TEE, 01/11/2026

## Thay đổi đã thực hiện

Software Keymaster 4 hiện có `on post-fs` với bốn lệnh `mkdir`, `restorecon`,
`chown`, `chmod` trên `/mnt/vendor/efs/DAK`. Đây là nhánh init của payload
software, trong khi quy tắc dự án không cho Codex ghi EFS. Mã nguồn payload
software và thư viện software đã kiểm không có chuỗi `/efs`, `DAK` hay
`gakkeybox`; điều đó là cơ sở loại nhánh init này, chưa thay thế kiểm thử boot.

Đã sửa trong fork:

1. `rom/unica/patches/exynos9810_device_stack/keymaster4/vendor/etc/init/android.hardware.keymaster@4.0-service.rc`:
   bỏ riêng bốn lệnh EFS, giữ service/interface và lệnh tạo
   `/data/vendor/keymaster`.
2. `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/customize.sh`:
   final verifier báo lỗi nếu init software KM4 cuối còn tham chiếu `/efs` hoặc
   `/mnt/vendor/efs`.
3. `rom/scripts/internal/verify_m4_runtime_image.py`: M4 vendor image gate
   kiểm init này có service/interface/data-dir, không tham chiếu EFS; so sánh
   byte với work tree và kiểm mode/owner/SELinux trong ext4 image thật.

Payload và final verifier được dùng chung cho các build Exynos9810 dùng
software KM4; image gate bổ sung chỉ áp dụng M4. Các ROM đã đóng gói không
thay đổi.

Kiểm chứng đã thực hiện: `bash -n`, `py_compile`, `git diff --check`; fixture
gọi **final verifier thật** nhận RC sửa và từ chối cả RC cũ lẫn một RC thêm
`chmod /efs/test`. Fixture tạo hai ext4 tạm, đọc lại RC bằng `debugfs`, xác nhận
image có RC sửa qua kiểm tra và RC cũ bị từ chối; kiểm contract metadata của
image verifier. Log: `logs/20261101-m4-tee-repair/keymaster-guard-fixture.txt`
và `keymaster-image-fixture.txt`. Chưa build ROM, chưa flash, chưa boot thử patch;
RC hiện cài trên r5 vẫn là bản cũ. Không kết luận patch này giúp pass Integrity.

Giới hạn quan trọng: đây chỉ loại các lệnh EFS trong **RC software KM4**. Các
RC và daemon khác của ROM vẫn tham chiếu EFS; không tuyên bố toàn bộ boot hoặc
toàn bộ hệ thống không thể sửa EFS. Rollback: đảo đúng ba thay đổi trên trong
fork; r5 đang chạy và ZIP r5 chưa bị thay.

## Bằng chứng mới về KM3/TEE

- `/vendor/lib64/hw/keystore.mdfpp.so` và các dependency trực tiếp hiện có;
  `/system/bin/linker64 --list` liệt kê được chúng. Phép thử chạy theo namespace
  của linker từ shell, không chứng minh HAL vendor chạy được hoặc resolve đúng
  namespace của service.
- `libkeymaster_helper_vendor.so` stock F có path trustlet
  `/vendor/app/mcRegistry/ffffffff00000000000000000000003e.tlbin`. SHA-256 của
  file trustlet trên thiết bị khớp cache stock F và work tree. Disassembly của
  `OpenSession` cho thấy đọc file trustlet rồi gọi `mcOpenTrustlet`; chưa gọi
  hàm này trên máy.
- `KM_generate_sak` có các lệnh gọi `write_blob` tại offset `0xcefc` và
  `0xcf8c`; `KM_install_sak_cert` gọi `write_blob` tại `0xcce0`. Disassembly
  `write_blob` có `fopen`, `fwrite`, flush và `chmod`. Trong binary có format
  `/efs/DAK/%s_%s.%s%s` và `/mnt/vendor/efs/DAK/%s_%s.%s%s`. Đây là bằng chứng
  thư viện **có nhánh ghi EFS**; chưa chứng minh mở HAL thông thường chắc chắn
  đi vào nhánh tạo/cài SAK.
- `mobicore.rc` live export `MC_AUTH_TOKEN_PATH /efs`, có lệnh tạo `/efs/TEE`;
  daemon hiện chạy trong mount namespace khác PID1/ADB. Tài liệu Samsung/AOSP
  mô tả biến này là đường chứa auth token cho daemon phiên bản cũ; chưa suy
  mọi chi tiết phiên bản cũ đúng với binary Exynos9810. [Nguồn AOSP](https://android.googlesource.com/platform/hardware/samsung_slsi/exynos5/+/jb-mr1-dev/mobicore/README.android).
- Stock F có 30 file mcRegistry, work tree hiện tại có 33, 30 tên chung trong
  đó 26 hash giống. Bốn entry cùng tên khác hash; chỉ trustlet KM3 `...003e`
  nói trên đã xác minh khớp riêng. Không thay cả registry hoặc vendor.
- AOSP `km_compat.cpp` có nhánh ánh xạ software Keymaster sang slot TEE khi
  không thấy backend an toàn; binary `libkm_compat.so` của r5 có chuỗi cảnh báo
  tương ứng và hash trên máy khớp work tree. Đây giải thích *có thể* vì sao log
  request nói `TRUSTED_ENVIRONMENT` trong khi KeyInfo báo software; nhánh exact
  chưa được quan sát trong log boot r5. [Nguồn AOSP](https://android.googlesource.com/platform/system/security/+/d66aa296cfc2e23f4ebb4e4b8b6d533f415ff5ba/keystore2/src/km_compat/km_compat.cpp).

## Thử chặn ghi trên filesystem giả

Trong mount namespace tạm đã đặt private, dùng tmpfs dưới `/data/local/tmp`
và mount bind hai alias; không mount lại hay ghi EFS thật. Toybox `mount` không
thực hiện được lệnh bind-remount trên scratch, báo:

```text
mount: '/data/local/tmp/codex_tee_guard_scratch_20261101' not in /proc/mounts
```

Một helper ARM64 tạm chỉ gọi `mount(2)` đã qua scratch test: alias RO từ chối
ghi, đổi RW ghi được; khi chỉ khóa alias A thì alias B vẫn ghi được, và khi
khóa cả hai thì cả hai từ chối ghi. Test cleanup xóa scratch mount/thư mục.
Helper không được dùng trên EFS; source/binary/log ở thư mục log riêng. Test
này chứng minh tính chất per-mount, **không chứng minh đã bảo vệ được EFS thật**.

`/proc/1/ns/mnt` và `/proc/1009/ns/mnt` khác nhau; `/proc/*/mountinfo` của hai
process cho thấy nhiều mount cùng thiết bị EFS tại `/efs` và `/mnt/vendor/efs`.
Một lần quét mọi namespace chưa hoàn thành và bị dừng; không suy danh sách hiện
có là toàn bộ alias, không remount EFS thật, không chạy lại daemon/HAL. Gọi
HAL KM3 trước khi kiểm được cả đường ghi gián tiếp qua daemon sẽ có nguy cơ
xung đột trực tiếp với quy tắc EFS. Cần thiết kế guard có kiểm chứng đầy đủ và
rollback hoặc một phép thử độc lập khác trước lượt runtime. Không giả định việc
khôi phục TEE sẽ làm Play Integrity pass.

## Kiểm tra bổ sung: đường mở KM3 và hai namespace

Disassembly stock F cho thấy `device_open` tại `0x29a0` gọi
`nwd_swd_open_connection`; hàm này chuyển sang `nwd_open_connection` rồi
`nwd_tz_open`. Tại `0xd790`, `nwd_tz_open` đọc property
`vendor.sys.mobicoredaemon.enable`, sau đó gọi `mcOpenDevice` và `OpenSession`;
`OpenSession` đọc trustlet và gọi `mcOpenTrustlet` tại `0xdb4c`. Đây là đường
đi tĩnh trong binary stock, không phải kết quả gọi HAL thực. Property live hiện
`true`; điều đó không chứng minh mở trustlet thành công. Các bản disassembly
nằm trong `km3-device-open-disassembly.txt` và `km3-open-chain-disassembly.txt`.

`libMcClient.so` live khớp work tree; stock F là prefix byte-identical của
file live và file live chỉ có thêm 23 byte `duhansysl@DuhanROM-V4.3` ở cuối.
Daemon live khớp hash stock F. Hai so sánh này không chứng minh tương thích
runtime hoặc nội dung secure world. Kiểm tra file descriptor chỉ đọc cho thấy
daemon đang giữ `/dev/mobicore` và `/dev/mobicore-user`.

Đã chạy một fixture tmpfs mới trên thiết bị với **hai mount namespace và một
mock daemon**, tất cả bên dưới `/data/local/tmp`, không dùng EFS. Kết quả:

```text
PASS_client_view_blocks_write
PASS_daemon_other_namespace_can_write
```

Do đó việc chỉ cho namespace client thấy mount RO không thể bảo vệ EFS khỏi
daemon ở namespace khác. Fixture và kết quả trong
`mount-guard-daemon-scratch.sh` / `mount-guard-daemon-scratch.txt`; tmpfs, script
và helper tạm trên điện thoại đã dọn. Không mở HAL, chạy daemon thứ hai, đổi
daemon đang chạy, remount EFS hay flash.

Source DS-ACK đã dùng cho kernel chẩn đoán của dự án
`${LOCAL_ABSOLUTE_PATH}`
có `admin_open` chỉ cho một kết nối và trả `-EBUSY` nếu admin đã được mở;
`admin_release` đưa kênh sang NOT_CONNECTED và trả `ESHUTDOWN` cho request đang
đợi. Log boot trước ghi `Trustonic TEE: admin_open: daemon connection open, TGID
1012`, và daemon live hiện giữ descriptor `/dev/mobicore`. Đây là cơ sở không
khởi động song song daemon thử nghiệm hoặc dừng daemon thật khi chưa có kế hoạch
an toàn; chưa tự gọi mở device để xác minh trực tiếp. Source tương ứng bản build
chẩn đoán đã ghi provenance tại `m2-runtime-kernel-fix.md`; không khẳng định mọi
kernel DS-ACK khác có cùng hành vi.

Máy `M2_G965F_SERIAL` vẫn ở Android, `sys.boot_completed=1`, PIF `true`,
SELinux Enforcing; không ghi phân vùng, không đổi vendor/radio/kernel đã cài.
Log kỹ thuật dưới `logs/20261101-m4-tee-repair/`, lưu riêng mode0700/0600.
