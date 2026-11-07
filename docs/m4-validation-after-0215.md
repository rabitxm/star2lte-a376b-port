# M4-0215 — kiểm thử thực tế ngày 28/10/2026

Kế hoạch sửa hiện hành: [m4-post-0215-repair-plan.md](m4-post-0215-repair-plan.md).
Đây là lượt kiểm trên **package0215 đã fresh-flash**, không phải nền0212 cũ.
Phạm vi chỉ G965F đã chọn trong AGENTS.md; không suy nghiệm thu sang G965N.
Không flash/reboot/wipe hoặc sửa payload ROM/kernel/vendor/radio trong lượt này.

## Nền và bằng chứng đã khóa

- Candidate `star2lte-1793191531-88WPtN`, package SHA256
  `6d0c43e77c3bae6f392250cb5c4625fb6fc3af0041637b44bfd7e96ff61ae58d`.
- G965FXXUHFVG6, star2lte, SDK37, One UI90000, boot_completed1,
  SELinux Enforcing. Product/model Android vẫn là donor, không dùng model đó
  thay nhận dạng phần cứng/bootloader.
- BOOT sau flash SHA `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`.
- `/proc/config.gz` của kernel đang chạy xác nhận
  `CONFIG_PANEL_BACKLIGHT_PAC_3_0=y` và `CONFIG_EXYNOS_DECON_LCD_S6E3HA8=y`.
- Device và staged services.jar cùng SHA
  `fb1a80887c80a815e8903ba6b209a2a7dffed2180dee3bf297cabf316439459e`.
- Device/staged/embedded lights HAL cùng SHA
  `b296e403ebd2a2bcbfc719d553604d3baacbfa59a828d632b1989702ca2f14df`.
- Raw logs, tombstone, ảnh và probe ở
  `logs/20261028-m4-validation-after-0215/`, chỉ lưu cục bộ ngoài Git.
  `collection-status.json` giữ stdout/stderr/exit, vì dumpsys có thể exit0
  nhưng stderr báo FAILED_TRANSACTION. Raw log có thông tin SIM/mạng,
  không được đẩy nguyên lên public repo.

## Kết quả kiểm

| Hạng mục | Đo được trên0215 | Giới hạn |
| --- | --- | --- |
| Android/ADB | boot_completed1, Enforcing, ro.adb.secure0, adb_enabled1; Rampart main/strict/auto-enabled đều0; provisioned/setup1 | Không lặp test công tắc/người dùng và PC mới trong lượt này |
| Core stability | 18 mẫu/180s, không lỗi ADB, cùng boot_id; SystemServer2214, netd1820, SurfaceFlinger1890, SystemUI10581, NFC6503, SDHMS8669 giữ PID/starttime | SystemUI đã crash một lần đầu boot trước cửa sổ đo |
| Hiển thị | Screenshot lockscreen vẽ đúng;5 vòng Dozing→Awake, PID lõi giữ nguyên, crash buffer không thêm dòng | Không thay thế đo quang học, multitouch hay auto-brightness |
| Backlight permission | Cả brightness/max_brightness đúng sysfs_ski_display_writable; logcat snapshot có0 backlight fstat EACCES,0 ion_open error | Đã khép lỗi nhãn0214/0215; còn lỗi scale riêng |
| Brightness scale | Settings60/180 trước đó ra sysfs59/180, actual_brightness3;5 lượt wake ra raw230/actual3, max44800 | Source/kernel/HAL audit bên dưới giải thích lỗi1×/100×; chưa sửa trong lượt này |
| Thermal | HAL2.0 connected, AP/BAT/PA/SKIN/USB có giá trị; snapshot AP46.3°C, SKIN38°C/status1 | Cooling/static thresholds vẫn trống; không nghiệm thu limiter |
| Passive raw thermal | BIG39–74°C, LITTLE39–64, G3D38–53, ISP38–54; không cố ý tạo tải trong180s | Đây là các sensor khác HAL AP/SKIN; không gọi là cùng scaling |
| Bluetooth | Snapshot17crash; cuối lượt38crash, BLE_TURNING_ON;180s có6 PID khác nhau | Chưa pairing/A2DP/OPP/HFP; không kết luận controller chết |
| Wi-Fi/mạng | Android có WIFI+VALIDATED; ping IP/DNS rc2 Message too long | Trạng thái VALIDATED chưa đủ nghiệm thu app network |
| TCP raw | Current0215 direct syscall connect1.1.1.1:80 → EINPROGRESS/poll/SO_ERROR0, exit0, không gửi payload | Chưa kiểm DNS/TLS/UID/fwmark |
| SCM_RIGHTS control |6 tình huống socketpair52byte+FD đều đúng semantics, probe v2exit0 | Không phải phép đo receive thật của fwmarkd |
| NFC/SDHMS dump | Cùng FAILED_TRANSACTION; /dev/null cũng fail; Binder ghi FD translation EPERM | Chưa biết RF/NFC functional state; không thay NFC blob từ lỗi dump |
| Camera/audio/sensors | Camera enumerate3 normal devices; audio/sensor services có dump/đăng ký | Chưa chụp/quay/mic/loa/call/GNSS thực. Không tạo workload dài khi thermal/BT còn mở |

Mốc cuối đọc: uptime1915.76s, cùng core PID, Enforcing/boot_completed1.
Settings brightness230, manual mode0, stay-awake0 và Binder debug_mask0.

## Bằng chứng lỗi và số dòng

Số dòng dưới đây trỏ vào `logcat.txt`/`crash.txt` trong thư mục raw log mới,
không dùng số occurrence làm số crash độc lập.

- RTC: `logcat.txt:36907–36909`, ba node date/time/since_epoch:
  `fstat failed: EACCES (Permission denied)`.
- Thermal HAL: `:43013`, `no cooling device for cooling type 0`;
  `:43015`, `no threshold data for temperature type 0`. Mỗi mẫu lặp26lần
  trong snapshot. Kernel cooling_device có thật nhưng chưa chứng minh HAL/SDHMS
  mapping đúng.
- Bluetooth: `:108198` HCI reset complete; `:108399` chờ
  `vendor.samsung.hardware.security.vaultkeeper.ISehVaultKeeper/default`;
  `:112273` backend không start, trảNULL; lặp tới `:133508`;
  `:133535` `irk read failed`; `:133596` GATT completed;
  `:133604` `BLE_TURNING_ON : BLE_START_TIMEOUT`. Khoảng chờ backend~29s.
- AOD: `crash.txt:18`:
  `AbstractMethodError ... PluginKeyguardStatusView.disconnectPluginOnBackground()`.
- Bixby: `crash.txt:306`, `IllegalStateException: No matching ComponentResult found!`.
- KG: `logcat.txt:59927`, `ANR in com.samsung.android.kgclient`;
  `TLC_ERROR` có69occurrence trong snapshot; không phải69ANR.
- Google Assistant: `tombstone_00:10` là
  `com.google.android.googlequicksearchbox:interactor`, không phải app tên hbox;
  `:15` `GC tried to mark invalid reference 0x737973`, static field d/offset236;
  stack ở libart ConcurrentCopying/LogHeapCorruption. Một crash đầu boot rồi
  restart; chưa chứng minh ART/kernel là bên ghi hỏng heap. Tombstone text+pb và metadata package/codePath/version hiện tại đã giữ; Play Store đã schedule update sau crash, nên chưa được coi APK hiện tại chính là APK lúc crash.

## Phát hiện mới: brightness scale có nguyên nhân cụ thể

Kernel PAC3 dùng BRT_SCALE100 (`panel_bl.h:26–28`). Bảng S6E3HA8 star2 bắt đầu
BRT0/BRT7/BRT14, tức0/700/1400; luminance bắt đầu2/3/4. `panel_get_brightness`
trả lookup luminance, không phải raw input hoặc đo quang học. Vì vậy raw59,
180,230 đều nằm dưới700 và trả luminance3 đúng như đã đọc.

Trong services.jar đang cài, PowerManagerUtil dùng AUTO_BRIGHTNESS_TYPE3,
USE_PERSONAL_AUTO_BRIGHTNESS_V3=false. Legacy BacklightAdapter lại chỉ chọn
factor100 khi cờ này true, nên path target hiện gửi1×. Cấu hình type3 phục vụ
thuật toán/cảm biến thật; không đổi toàn cục sang4 để né lỗi scale.

Native AIDL lights HAL đã audit theo symbol .gnu_debugdata: extension w3
được chuyển tiếp tới setLightBacklight rồi writeIntToFileDesc nguyên vẹn khi
không âm. Nó không nhân100 thêm. Có đủ bằng chứng cho trial chỉ sửa scale ở
BacklightAdapter, không sửa kernel/HAL. Chi tiết file/gate ở plan mới.

## Các phép thử chẩn đoán và giới hạn

- `socketpair-fd-runtime-v2.txt`: stream/dgram52byte+FD với buffer24 và cả
  recv flags0x40004028 đều recv52/rights1, không TRUNC/CTRUNC. PASSCRED+buffer24
  cố ý sinh CTRUNC; buffer128 nhận cả credentials+rights bình thường.
  V1 classifier sai vì coi bit CMSG_CLOEXEC được kernel giữ lại là lỗi;
  source/log v1 còn nguyên, verdict v1 không dùng. Raw field v1 vẫn đúng.
- Binder failed log có nfc/sdhms call size28:8,
  `ret 29201/-1 l=3616`; source line3616 là binder_translate_fd thất bại.
  USER_ERROR debug_mask0→1 trong một lượt dump rồi khôi phục0; không có
  thông báo target-does-not-allow-fds. Điều này hỗ trợ nhánh security transfer,
  chưa đủ kết luận cạnh permission cụ thể.
- Receiver NFC/system_app vẫn Enforcing dù caller là permissive-domain su.
  dumpsys tạo pipe nội bộ dưới caller; policy cho appdomain dùng shell/adbd FD,
  không có generic appdomain→su FD use. Đây là đầu mối mạnh cho lỗi công cụ.
- setuid2000 cũ chưa đổi SELinux domain. runcon trực tiếp vào id/toybox bị
  Permission denied. Trial entrypoint shell_exec đúng với /system/bin/sh lại
  exit139 Segmentation fault; đã giữ log riêng và không lặp thêm. Đây là lỗi
  tự kích hoạt của phép thử domain, không cộng vào baseline app crash ở trên.
- Probe dùng socket/FD riêng, không attach PID1/daemon, không đọc khóa. Hai
  binary tạm đã xóa khỏi /data/local/tmp sau thử. Không thay chính sách để dump
  trông như PASS; không dựng success giả cho NFC/KG/thermal.
