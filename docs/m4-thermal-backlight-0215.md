# M4 SDHMS max-brightness label — patch0215

G965F `M2_G965F_SERIAL` đã boot fresh package0214 với SELinux Enforcing.
Bootloader `G965FXXUHFVG6`, SDK37, One UI90000. Runtime context của node thật
`/sys/devices/platform/panel_drv@001/backlight/panel/brightness` đã đúng
`u:object_r:sysfs_ski_display_writable:s0`; log mới không còn lỗi `brightness`.

Fresh log phát hiện node kề bên vẫn generic `sysfs`:

```text
10-28 19:23:37.606 8737 8859 E SDHMS:f1: e = /sys/class/backlight/panel/max_brightness: fstat failed: EACCES (Permission denied)
```

`max_brightness` là root-owned, mode0444. Gán tạm existing display type lên
duy nhất node này trong20 giây; không có fstat error mới trong khoảng đó, sau
đó phục hồi đúng context `u:object_r:sysfs:s0`. Không ghi node brightness.
Điều này hỗ trợ chẩn đoán nhãn SELinux và không thay DAC hoặc giá trị nhiệt.

Patch0215 mở rộng helper/lock của0214 để thêm đúng rule:

```text
(genfscon sysfs "/devices/platform/panel_drv@001/backlight/panel/max_brightness" (u object_r sysfs_ski_display_writable ((s0) (s0))))
```

Ảnh hưởng: helper `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_android17_thermal.py`, lock, existing `customize.sh` hook, M4 image verifier, fork patch `0215`, series/source lock, cùng hồ sơ M4 runtime. Helper vẫn bắt buộc existing `system_app_33_0` permission, từ chối duplicate/conflict và giữ scope M4/A37 Android17. Không thêm allow mới, không đổi mode/owner, kernel, vendor blob, radio hay thermal threshold.

Kiểm tra đã chạy: self-test positive/idempotence/duplicate/conflict/missing-permission; `py_compile`; JSON parse; `bash -n`; `git diff --check` — PASS. Patch0215 và series/hash record được lưu ở `patches/fork/`. Candidate `rom/out/releases/M4/star2lte-1793191531-88WPtN` build PASS; package SHA256 `6d0c43e77c3bae6f392250cb5c4625fb6fc3af0041637b44bfd7e96ff61ae58d`, target-files SHA256 `8a1e1e694b2915bd10b98a99333b337e95a9f06863c5575ee309f2b582467132`. Vendor CIL exact-rule, ext4 bytes/metadata, full package verifier và 8 stage ZIP32 manifest đều PASS. Candidate0215 đã fresh-flash ngày28/10 trên G965F. TWRP cài đủ8/8 stage, mỗi stage trả `RC=0`; marker kết thúc `next=done`. `fresh-format.txt` ghi USERDATA UUID đổi từ `be7a3d7d-140c-49f5-b870-6c65c5552bba` sang `eea29746-9f8b-4a36-9752-89471ef4cd20`. Hash sau cài của EFS/CPEFS/RADIO/RECOVERY/BOTA0/1/2 khớp hoàn toàn hash trước cài; BOOT backup trước ghi lưu trong thư mục log để rollback khi cần. Android đạt `sys.boot_completed=1`, SDK37/OneUI90000, `G965FXXUHFVG6`, Enforcing, `ro.adb.secure=0`.

Runtime sau0215: brightness và max_brightness đều có `u:object_r:sysfs_ski_display_writable:s0`; không còn SDHMS `fstat failed: EACCES` cho hai node. Khi màn hình Awake, thay `screen_brightness` 230→60→180 làm sysfs brightness đổi 230→59→180; sau đó đã khôi phục setting230 và `stay_on_while_plugged_in` về0. `actual_brightness` vẫn đọc3 trong sample, nên chưa kết luận panel quang học/limiter hoạt động đủ. Còn SDHMS EACCES trên RTC `date/time/since_epoch` (logcat lines172387–172389), tách biệt backlight. Thermal HAL vẫn báo `no cooling device for cooling type 0` (lines214763,288642) và `no threshold data for temperature type 0` (214767,288643); `dumpsys thermalservice` có nhiệt độ AP/BAT/PA/SKIN/USB nhưng danh sách cooling devices và static thresholds trống. Linux có cooling_device0–3 (thermal-gpufreq-0, thermal-isp-0, thermal-cpufreq-0/1), với cur_state 0/0/0/2. `/sys/class/thermal/thermal_zone0..3` còn có trip points: BIG passive83°C, LITTLE active76/81/86/91/96/101°C, G3D passive88°C; ISP query trip_point_7 báo thiếu file. Dù vậy HAL chưa expose cooling/threshold qua HIDL. Không ánh xạ type0 sang một trip tùy ý: AP là công thức SDHMS riêng, cần audit HAL/vendor policy để xác nhận ý nghĩa và hysteresis trước khi sửa. Patch0215 rollback bằng cách reverse patch, gỡ khỏi series/source lock và build lại; giữ candidate0214 làm đối chứng. Bằng chứng đầy đủ ở `logs/m4-thermal-0215-install-run/`.
