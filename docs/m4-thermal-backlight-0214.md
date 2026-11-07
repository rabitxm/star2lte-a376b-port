# M4 thermal backlight labels — patches0214/0215

Ngày28/10/2026. Khảo sát và triển khai trên G965F `M2_G965F_SERIAL`; Android báo donor
`SM-A376B/XXV/A376BXXU5BZI6`, bootloader `G965FXXUHFVG6`, SELinux Enforcing.
Lượt chẩn đoán ban đầu chỉ đọc; các flash fresh0214/0215 và kiểm runtime được ghi riêng bên dưới và trong tài liệu0215.

## Bằng chứng

Build0214 đã được flash fresh. Sau reboot, exact genfscon cho `brightness` xuất
hiện đúng trên máy Enforcing. Lỗi đầu tiên đã hết, nhưng log fresh boot lộ thêm
node kế bên `max_brightness`, dẫn tới patch0215.

Log ban đầu:

```text
10-28 17:36:33.762 4537 4686 E SDHMS:f1: e = /sys/class/backlight/panel/brightness: fstat failed: EACCES (Permission denied)
```

PID4537 là `com.sec.android.sdhms` trong `u:r:system_app:s0`. Symlink
`/sys/class/backlight/panel/brightness` trỏ tới
`/sys/devices/platform/panel_drv@001/backlight/panel/brightness`. Node thật là
0644 `system:system`, đang mang `u:object_r:sysfs:s0`.

Embedded Hybrid Vendor policy có các rule genfscon cho `panel_drv_0` và
`panel_drv@0`, nhưng không cho `panel_drv@001`. Type
`sysfs_ski_display_writable` đã tồn tại, và rule đã có sẵn cho
`system_app_33_0` gồm `getattr`, `read` và quyền display liên quan. Vì vậy sửa
chỉ ánh xạ node thật vào type đang có; không tạo allow mới hay phân quyền
`system_app` trên generic `sysfs`.

Đã gán tạm type này cho duy nhất node `brightness` trong phép thử khoảng33s;
không có lỗi mới trong khoảng quan sát và nhãn được trả lại `sysfs`.

Sau khi flash package0214 và reboot fresh, `brightness` có context
`u:object_r:sysfs_ski_display_writable:s0`, trong khi `max_brightness` còn
`sysfs`. Log fresh boot ghi:

```text
10-28 19:23:37.606 8737 8859 E SDHMS:f1: e = /sys/class/backlight/panel/max_brightness: fstat failed: EACCES (Permission denied)
```

Tạm gán cùng existing display type cho đúng `max_brightness` trong20s; không
xuất hiện lỗi fstat mới trong cửa sổ đó, rồi context được khôi phục chính xác
thành `u:object_r:sysfs:s0`. Không ghi giá trị brightness.

## Patch và kiểm chứng source

Patch0214 thêm genfscon chính xác cho `brightness`:

```text
(genfscon sysfs "/devices/platform/panel_drv@001/backlight/panel/brightness" (u object_r sysfs_ski_display_writable ((s0) (s0))))
```

Patch0215 mở helper/lock/verifier để giữ thêm đúng node `max_brightness` với
cùng type đã có quyền `system_app_33_0`:

```text
(genfscon sysfs "/devices/platform/panel_drv@001/backlight/panel/max_brightness" (u object_r sysfs_ski_display_writable ((s0) (s0))))
```

File ảnh hưởng:

- `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/customize.sh` — gọi helper sau khi khôi phục vendor baseline và thêm supplementary policy, chỉ cho M4/A37 Android17.
- `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_android17_thermal.py` và `.lock.json` — kiểm hai genfscon exact; từ chối rule trùng, xung đột hoặc mất quyền policy đang có.
- `rom/scripts/internal/verify_m4_runtime_image.py` — kiểm CIL sau build và bytes/metadata của file bên trong vendor ext4 image.
- `patches/fork/0214-label-s9-backlight-for-m4-sdhms.patch`, `0215-label-s9-max-backlight-for-m4-sdhms.patch`, `patches/fork/series` và `sources.lock.json` — ghi lý do, file, kiểm chứng và rollback.

Patch0214 candidate `star2lte-1793185796-jPRfTA` đã build và flash fresh trên
G965F. Package Zip64 bị TWRP3.7 từ chối trước khi ghi (`Zip file is corrupt!`),
CRC/signature-verified ZIP32 8-stage helper của fork được dùng tiếp. Stage1–7
hoàn tất; stage8 lần đầu fail vì ZIP nằm trên USERDATA, lần thứ hai vì tồn tại
stale `/tmp/exynos9810/kernel` (log: `unzip: can't read standard input: Not a
typewriter`); xóa đúng temp staging rồi retry từ `/tmp`, stage8 hoàn tất với
`RC=0`, format data thành công.

Runtime sau0214: `sysfs` label đã tự đổi đúng cho brightness sau reboot và
SDHMS không còn ghi EACCES cho brightness. Runtime tạm thử max-brightness cho
thấy cùng mapping có tác dụng. Patch0215 đã qua helper self-test (positive,
idempotence, duplicate/conflict/missing-permission), py_compile, JSON validation,
`bash -n` và `git diff --check`; package/image/reboot của0215 và thermal HAL
cooling/threshold/actuator vẫn pending. Chưa thử đủ ngưỡng nhiệt để tuyên bố
limiter đạt.

Log audit cục bộ: `logs/20261028-m4-thermal-audit/`; chứa raw policy/runtime
log, chỉ để trong máy và không commit.


Source replay note (28/10/2026): clean fork replay found the `verify_m4_runtime_image.py`
hunk expected a split path line that is not present in the pinned preimage. The
hunk context was corrected with no intended logic/image change; the current
patch SHA is recorded in `sources.lock.json`. All206 exported patches now replay.
