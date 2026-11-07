# M4 Lite ZIP32 — uptime 14 ngày

Yêu cầu: tạo bản riêng từ M4 Lite Preformatted Internal ZIP32 cho G965F, offset 1.209.600 giây khi khởi động, theo patch PortKit trước. Giữ release nguồn và dữ liệu máy riêng.

Nguồn ZIP được chọn: `star2lte-preformatted-1793692940-kKZ7vE/M4-Lite-Preformatted-Internal-ZIP32-sign.zip`, SHA-256 theo manifest `fa600a7395eaddf5e5f2cd782cbd28b755ba83f09f7de3fa5e72c55947ae614c`. Kernel trong gói là DS-ACK V1.12 G965F WIPEONFORK diagnostic, no KernelSU. Source và .config chẩn đoán nằm trong `~/.cache/star2lte-dsack-wipeonfork-20261024/`.

PortKit trước thêm offset tại `kernel/time/timekeeping.c`, cho monotonic, boottime và monotonic-raw; cần kiểm lại ảnh hưởng timer, realtime, suspend, keystore và ứng dụng sau boot. Không chứng minh giá trị uptime là thời gian hoạt động thật.

Trạng thái: đã build và kiểm package tĩnh; chưa flash và chưa nghiệm thu runtime.

## Kernel build

Đã sao chép source WIPEONFORK sang `~/.cache/m4-lite-u14d-20261103/source`, giữ source gốc. Patch thêm `CONFIG_PORTKIT_BOOT_UPTIME_OFFSET_SECONDS=1209600` và áp dụng cùng thay đổi timekeeping đã dùng ở PortKit 14 ngày. Giữ cấu hình gốc G965F, no KernelSU và không bật always-permissive. Dùng AOSP clang r547379 sẵn có.

Build `Image` exit 0, SHA-256 `fde5f62f09c4cf0be6f291c77415f6e74658c15686c93bdbbb6719a6701eeb8e`. So với .config gốc chỉ thêm cấu hình offset; 10.189 symbol export có CRC giống kernel trước. Kernel cũ giải mã từ ZIP nguồn khớp SHA-256 `92b14be170225954476869d1fd7700f9c2c92d98eb6447d0181c50a11e80fcdd`. Build có cảnh báo section mismatch và thông báo find đường dẫn DTB; DTB gói gốc được giữ, cần kiểm runtime trên máy.

Script đóng gói: `tools/build_m4_lite_u14d_20261103.py`; kết quả và log build nằm tại `~/.cache/m4-lite-u14d-20261103/`. Chưa flash, chưa xác minh API Android/timer/suspend/keystore trên bản mới.

## ROM hoàn chỉnh

- File: `rom/out/releases/M4/star2lte-uptime14d-20261103/M4-Lite-Preformatted-G965F-Uptime14d-ZIP32-sign.zip`.
- Kích thước: 4.051.039.182 byte; SHA-256 `0c905bc9ec7959e0f146302556f186eaa1cee7708251dd9ecdd76aadb699e5c3`.
- ZIP32, kích thước dưới ngưỡng 3,8 GiB, CRC và chữ ký OTA testkey: PASS qua verifier gốc `verify_m4_lite_zip32.py`.
- Bộ entry và hash tất cả payload ngoài `exynos9810/kernel.zip` và `exynos9810/kernel_name` khớp ZIP nguồn. BSDIFF G965F trong kernel.zip tái tạo đúng SHA-256 Image mới; DTB và phần còn lại của kernel.zip giữ theo nguồn.
- Package chỉ dành cho G965F, profile preformatted: cần format USERDATA trước khi chép ZIP; updater kiểm layout, không tự repartition hay format USERDATA ở cuối.
- Chưa flash hoặc boot bản này. Cần đo `/proc/uptime`, `CLOCK_MONOTONIC`, `CLOCK_BOOTTIME`, `CLOCK_MONOTONIC_RAW`, `SystemClock.uptimeMillis()` và `SystemClock.elapsedRealtime()` sau fresh boot/reboot; kiểm timer, suspend/wake, TEE/keystore, mạng và ứng dụng. Giá trị offset không đại diện uptime thực.
- Rollback: flash lại bản Lite nguồn `star2lte-preformatted-1793692940-kKZ7vE` sau khi kiểm target, hash và dữ liệu; không ghi EFS/NV/FRP hay clone state thiết bị.

Log đóng gói: `~/.cache/m4-lite-u14d-20261103/package.log`; kernel và cấu hình tại `~/.cache/m4-lite-u14d-20261103/out/`.
