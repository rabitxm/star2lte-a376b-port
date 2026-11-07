# M4-0217 — kiểm thử G965F ngày 29/10/2026

Kế hoạch sửa hiện hành: [m4-post-0217-repair-plan.md](m4-post-0217-repair-plan.md).
M4 đã vào Android và dùng được giao diện, nhưng **chưa boot sạch, chưa nghiệm thu
Bluetooth/mạng/thermal**. Tài liệu này thay kết luận kiểm thử sơ bộ của lượt flash
0217; không chuyển kết quả F sang N.

## Nền thử và phạm vi

- G965F `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`, star2lte, SDK37 /
  One UI90000, SELinux Enforcing, `sys.boot_completed=1`.
- Full ZIP `cf2b93a6b21858d4c976fabd2a19ce75933945948acdd0647650bea702cc1323`;
  release `rom/out/releases/M4/star2lte-1793245763-uG9DKB/`.
- Giữ profile runtime-init-debug và gói G965F WIPEONFORK đã hiệu chuẩn,
  SHA256 `4cea63f84d5add494bfcc2f9fff105848806a930a24aca975dc261bf6720e20a`.
- Lượt này chỉ thu log, đo ổn định, năm vòng phím sleep/wake và chạy một probe
  TCP riêng không gửi payload. Không flash, reboot, wipe, sửa Settings/payload,
  đổi kernel/vendor/radio hay chạm EFS/CPEFS. Binary probe tạm đã xóa.
- Raw log chỉ lưu cục bộ trong `logs/20261029-m4-validation-0217/`, directory0700,
  file0600; có dữ liệu ứng dụng/mạng nên không đưa nguyên log lên public GitHub.

## Kết quả hiện tại

| Hạng mục | Đã đo | Phần chưa khép |
| --- | --- | --- |
| Core runtime | 18 mẫu/180s, cùng boot ID, ADB không lỗi; SystemServer6572, netd6381, SurfaceFlinger1894, SystemUI11632, SDHMS10090, NFC7645 giữ PID và starttime | Có một watchdog SystemServer và hai crash SystemUI trước cửa sổ này |
| Sleep/wake | 5 vòng Dozing→Awake, raw0→18301; `actual_brightness`2→286, core PID giữ nguyên, crash buffer không thêm dòng | Chưa kiểm đầy đủ AOD/widget/notification; trạng thái Dozing không chứng minh toàn bộ AOD plugin hoạt động |
| Brightness0216 | Lượt flash đã đo20/60/120→raw1907/5923/11947; người dùng xác nhận trực tiếp màn hình đổi sáng rõ, bình thường | Còn multi-lux auto, HBM/bounds và thermal limiter; không cần chuyển sang độ sáng cố định |
| ADB/Auto Blocker | `ro.adb.secure=0`, adb_enabled1; secure Rampart main/strict/auto đều0, suw_main_on0, blocked_adb_cmd0/blocked_usb_data_transfer0 | Không lặp test công tắc/PC mới vì người dùng đã xác nhận và không cần cắt ADB trong lượt đo này |
| Bluetooth | BLE_TURNING_ON; bộ đếm manager19→28crash; 6PID trong180s | ON/scan/pair/profile thật chưa đạt; không gọi là Bluetooth ổn |
| Wi-Fi/mạng | Link Wi-Fi connected/VALIDATED; ping IP và DNS cùng `connect: Message too long`; raw TCP syscall probe PASS exit0 | DNS/HTTP/HTTPS qua app/Bionic chưa đạt; VALIDATED không thay cho phép thử socket |
| NFC | Log boot có JNI ok, HIDL remote, NFA enable status0, STATE_ON và RF discovery started; PID giữ nguyên, nhận screen OFF/ON | `dumpsys nfc` FAILED_TRANSACTION; thẻ thật/HCE/UICC/eSE/payment chưa kiểm |
| Thermal | HAL2.0 ready, AP41.3°C/BAT38.3/PA39.5/SKIN38.2 status2/USB37.9; Linux cooling devices tồn tại | Generic clamp không có data, cap1.0/appliedfalse; Samsung master limiter chưa chứng minh; cooling/threshold HAL trống |
| RTC | Root đọc được nhưng SDHMS vẫn EACCES trên date/time/since_epoch | Cần sửa label chính xác ba node i2c17, không thêm allow rộng |
| Camera/audio/sensor | Camera enumerate/dump, audio/sensor service có dữ liệu; nhãn ION/backlight đúng | Chưa chụp/quay/mic/loa/call/GNSS thực; chưa tạo workload dài khi thermal limiter chưa khép |

Uptime cuối1327.38s, boot_completed1, Enforcing; Settings người dùng là
brightness183/manual0/stay-awake0 trước và sau lượt đo. Không trả người dùng
về brightness110 của bài thử cũ. `sleep-wake.json` có trường cũ
`initial_state_restored=false` do script gán giá trị `initial_asleep` cho tên
trường này; không dùng trường đó làm verdict. Các vòng và trạng thái thức/ngủ
được giữ riêng trong raw JSON, không đổi Settings.

Collector lịch sử hỏi ba key Rampart ở bảng global nên nhận `null`; **null đó
không phải bằng chứng Auto Blocker tắt**. Đã truy vấn đúng bảng secure, lưu ở
`actual-rampart-settings.txt`; các giá trị0 ở đó là bằng chứng runtime hiện tại.

## Lỗi boot lần đầu: watchdog có ANR gốc

Báo cáo flash sơ bộ chỉ quét vài pattern Java/native và bỏ sót watchdog. Log
boot gốc `logs/20261029-m4-brightness-isolation-0217/run/logcat-all.txt:70647`
ghi `WATCHDOG KILLING SYSTEM PROCESS: Blocked in handler on main thread (main)
for 67s`. SystemServer2210 chết, lần sau6572 mới vào Android.

Đã kéo ANR chính xác `firstboot-watchdog-anr.txt`. Dòng277–295 cho thấy main:

```
FutureTask.get
KnoxGuardNative.executeMethodWithTimeOut
KnoxGuardNative.performTAApiCallWithRetry
KnoxGuardNative.getTAStateResponse
Utils.getStateAndSetToKGSystemProperty
KnoxGuard30Service.<init>
SystemServer.startOtherServices
```

Worker185 ở dòng254–269 giữ global lock trong `ServiceManager.waitForService`;
hai worker retry ở dòng37–60 lại chờ lock. Source hiện dùng25.000ms/lần và ba
lần retry. Đây là bằng chứng chặn ở KG30, mạnh hơn stack EngineeringMode được
in khoảng7giây sau lúc watchdog phát hiện kẹt. Chỉ thấy một watchdog kill trong
log đã thu; cửa sổ ổn định mới không có SystemServer restart.

## Dòng lỗi cụ thể

Các số dòng sau dùng raw log mới `logs/20261029-m4-validation-0217/`:

- `crash.txt:22–38`: SystemUI PID7724,
  `AbstractMethodError ... PluginKeyguardStatusView.disconnectPluginOnBackground()`.
- `crash.txt:39–65`: SystemUI PID11434,
  `ClassNotFoundException: com.android.systemui.indexsearch.SystemUIIndexProvider`.
  Class và superclass có trong DEX compiled; APK installed khớp hash compiled
  `04963cd4f3e4785c7e70eba5fb7c41f893210a3c864da5332b1e278a105d1855`.
  Chỉ base split, dexopt run-from-apk. Cần kiểm loader, không kết luận APK
  thiếu class hoặc xóa provider để che lỗi.
- `crash.txt:66–84`: Bixby `LockedBootCompletedReceiver`,
  `IllegalStateException: No matching ComponentResult found!`.
- `logcat.txt:78970`: `ANR in com.samsung.android.kgclient`. Có69dòng
  `TLC_ERROR` trong snapshot, không phải69ANR.
- `logcat.txt:300030`: VaultKeeper `didn't start. Returning NULL`;
  `:300072`: scan controller mất29215ms; `:300097`:
  `BLE_TURNING_ON : BLE_START_TIMEOUT`. HCI reset đã hoàn thành ở`:295956`.
- `ping-ip.txt:3`, `ping-dns.txt:3`: `connect: Message too long`.
  `tcp-raw-current.txt` có `so_error=0`, `exit=0` trên cùng0217.
- `logcat.txt:58182–58184`: RTC date/time/since_epoch
  `fstat failed: EACCES (Permission denied)`.
- `logcat.txt:304837–304838`: `no cooling device for cooling type 0` và
  `no threshold data for temperature type 0`.
- `display.txt:463–470`: throttling status2, data null, brightness cap1.0,
  appliedfalse. Đây là bằng chứng generic clamp chưa áp; chưa kết luận mọi
  cơ chế bảo vệ nhiệt Samsung/Linux bị vô hiệu.
- `nfc.txt` và `sdhms.txt`: dumpsys exit0 nhưng stderr `FAILED_TRANSACTION`;
  không dùng exit0 làm PASS Binder/RF.

Crash buffer mới có ba Java fatal event: hai SystemUI, một Bixby. GSA native GC
crash0215 chưa tái hiện trong buffer0217 này; giữ như lỗi lịch sử chưa khép.

## Hồ sơ kiểm và giới hạn

`collection-status.json` ghi stdout/stderr/exit từng lệnh;
`stability-samples.json`/`stability-summary.json` giữ dữ liệu PID và nhiệt;
`sleep-wake.json`/hai crash buffers giữ bài sleep/wake;
`firstboot-watchdog-anr.txt` và `watchdog-dropbox.txt` giữ nguyên nhân watchdog.
`error-index.json` chỉ đếm occurrence, không dùng làm số lần crash độc lập.

Trong180s không tạo tải, BIG43–70°C, LITTLE43–65, G3D41–56, ISP42–57,
battery38.1–38.6. Các sensor này không được đồng nhất với AP/SKIN của Samsung
HAL. Policy4 max1924000 thấp hơn cpuinfo2704000 chưa chứng minh thermal cap vì
còn QoS/power policy khác. Không thay kernel/blob/radio dựa vào các số này.
