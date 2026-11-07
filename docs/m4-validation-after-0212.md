# M4 kiểm thử sau0212 — 28/10/2026

Đã hoàn thành lượt kiểm và lập [kế hoạch sửa chi tiết](m4-post-0212-validation-plan.md).
Nền vẫn là attempt0209 + resolver0211 + context0212; không flash/reboot/wipe,
không thay kernel, vendor/RIL, modem, bootloader hay policy trong lượt này.
Phạm vi G965F đã chọn; không suy nghiệm thu sang N.

## Kết quả có bằng chứng

Ở snapshot uptime1178.75s: SDK37/OneUI90000, boot_completed1, Enforcing.
SystemServer1842, netd1451 và SurfaceFlinger1531 giữ nguyên PID/starttime.
SystemUI4885 sống1056.75s sau lần tự restart đầu boot. Monitor riêng180s có18
mẫu cho các tiến trình này, NFC2972 và SDHMS4537 không đổi; Bluetooth thay PID.
Không gọi trạng thái này là boot sạch vì SystemUI đã crash trước cửa sổ đo.

5 lượt sleep/wake đều trở về Awake, brightness0→88 và không có crash mới
trong crash buffer. Ảnh đã kiểm vẽ lockscreen và Settings; người dùng xác nhận
phím Home thật vào launcher bình thường. Input Home qua ADB không chuyển ngay
được giữ là phép thử không kết luận, không tạo bug launcher từ điều đó.
ION giữ nhãn đúng và final log không có `open /dev/ion failed`.

Settings hiện tại: setup/provisioned1, USB debugging1, ro.adb.secure0 và
Rampart main0. Không sửa công tắc trong lượt này; defaults từ fresh package
và PC mới/noRSA vẫn cần nghiệm thu lại khi đóng ROM mới. PIF UI hiện có nhưng
không có verdict Integrity trực tiếp.

## Lỗi đo được và giới hạn

| Nhóm | Dòng lỗi/bằng chứng | Kết luận |
| --- | --- | --- |
| Bluetooth | `final-state.txt:71`: `Bluetooth crashed 30 times`; `final-logcat.txt:92248`: `BLE_TURNING_ON : BLE_START_TIMEOUT`; có `irk read failed` | Ít nhất30lượt tới snapshot; HCI reset/firmware đã lên nên chưa quy lỗi cho controller |
| Mạng | `ping-ip.txt`/`ping-dns.txt`/`tcp-http.txt`: `connect: Message too long`; `trace-network-v5.txt`: sendmsg52, recv4, int32=-90 | Lỗi ở control boundary trước connectIP; cần tách parser/ancillary-FD/wrapper, chưa chứng minh file cần sửa |
| Thermal | `final-logcat.txt:40701`: `/sys/class/backlight/panel/brightness: fstat failed: EACCES (Permission denied)` | Node thật mang nhãn sysfs, SDHMS system_app; quyền/actuator chưa đạt |
| Thermal HAL | `thermal-state.txt`: cooling/threshold không có; raw sysfs có cooling và trạng thái8/17 ở lần đọc | Không suy kernel cooling hoạt động thành HAL/SDHMS điều khiển đầy đủ |
| AOD/SystemUI | `final-crash.txt:18`: `AbstractMethodError ... disconnectPluginOnBackground()` | Một crash đầu boot; hợp đồng API hiện tại chưa tương thích đầy đủ |
| NFC | `nfc.txt`: `FAILED_TRANSACTION`; `nfc-dump-shell-uid.txt` cũng lỗi với UID2000 | JNI/HIDL/process có hoạt động; lỗi dump còn xảy ra SDHMS nên chưa kết luận NFC RF hỏng |
| Bixby | `final-crash.txt:34`: `IllegalStateException: No matching ComponentResult found!` trong LockedBootCompletedReceiver | Receiver crash một lần; component/registry cần audit |
| KG | `logcat.txt:54154–54160`: `am_anr` / `ANR in com.samsung.android.kgclient`; final log có TLC_ERROR/getTAInfo | ANR đầu boot, cần phân loại TA/service/receiver; không phải bằng chứng modem lỗi |

Raw log ở `../logs/20261028-m4-validation-after-0212/`, ngoài Git, mode600/700.
`error-index.json` giữ số dòng đầu/cuối và số occurrence từng mẫu lỗi. Số
occurrence không phải số crash (một exception có thể in nhiều lần).

## Đối chứng mạng và audit API

Raw syscall TCP connect đến IP đích đã PASS: EINPROGRESS→poll writable→
SO_ERROR0. Thử này không dùng Bionic/netd hooks, không gửi HTTP payload.
Ngược lại ping/nc của Android FAIL trước IP connect. Tracer v4 chỉ attach
vào tiến trình ping con do nó tạo; bắt AF_UNIX connect tới /dev/socket/fwmarkd thành công, sendmsg52
và reply4byte=-90. Không trace PID1/daemon, không đọc key hoặc dữ liệu cá nhân.
Các probe đã được xóa khỏi /data/local/tmp sau thử; source/hash/provenance còn
trong log. V1/v2 tracer có lỗi đặt tham số ptrace/nhãn entry-exit, đã đánh dấu
không dùng; v3/v4 hợp lệ; v4 đọc mã phản hồi, v5 xác nhận thêm pathname fwmarkd.

Netd, libnetd_client và ping installed đều khớp donor BZI6. Netd map
libnetd_updatable S22 trong APEX, nhưng strings đặt FwmarkServer ở netd BZI6;
không suy đơn giản rằng đổi library đó sẽ sửa lỗi. Kế hoạch yêu cầu xác định
branch recv/parser/FD thực sự trả EMSGSIZE trước khi sửa. VALIDATED chỉ là
một phép kiểm Android đã qua; không thay thế các app/network tests đang fail.

Installed SystemUI/AOD hash khớp APK host; đã decode riêng SystemUI compiled
đúng bytes để xác nhận interface. Trong62 method, retained class thiếu12
abstract;10 có VersionCheck,2 không có là disconnectPluginOnBackground()V
và setDarkAmounts(FF)V. Cả hai có caller; chỉ disconnect đã crash runtime.
BZI6 AOD cũng không có setDarkAmounts custom của interface đã port, nên cần
sửa cả hợp đồng AOD/Infinity, không mặc định thay APK donor là đủ.

## Chưa thử và thứ tự tiếp theo

Camera enumerate3 public devices, audio/sensor/HAL có đăng ký. Chưa kiểm
ảnh/video thực, mic/loa vật lý, pairing/profile BT, RF/tag, cuộc gọi/SMS/data,
GNSS fix, workload thermal/actuator và Integrity verdict. Không nâng chúng
thành PASS chỉ từ process/service tồn tại. Chưa chạy tải camera/video dài vì
quyền brightness/control còn lỗi. Nhiệt đã giảm trong cooldown nhưng chưa
chứng minh mọi limiter.

Thứ tự sửa/gate/file/rollback nằm trong kế hoạch liên kết đầu tài liệu:
thermal tối thiểu→Bluetooth→fwmark→AOD→NFC/app→phần cứng→build/fresh nghiệm thu.
ROM hoàn chỉnh đang giữ không bị ghi đè; chưa có package mới trong lượt này.

## Mốc cuối lượt kiểm

`end-of-validation.txt` ghi uptime2052.12s (hơn34phút), boot_completed1, Enforcing và cùng PID SystemServer1842/netd1451/SystemUI4885; ION vẫn ion_device. Probe tạm đã được xóa khỏi điện thoại. Không có flash/reboot hoặc bản sửa runtime mới trong lượt này. Kết quả này bổ sung mốc snapshot1178.75s ở trên, không xóa các lỗi đã đo đầu boot hoặc các gate phần cứng chưa thử.
