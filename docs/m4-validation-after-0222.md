# M4-attempt0222 — fresh flash và runtime validation, 29/10/2026

**Fresh flash đã boot tới Android 17, nhưng M4 chưa nghiệm thu.** G965F đạt
`sys.boot_completed=1` và SELinux Enforcing; trong cửa sổ theo dõi khoảng180s,
`system_server` và SystemUI giữ nguyên PID. Bluetooth FAIL với vòng crash/restart
lặp lại quanh HCI `Unknown OpCode(0xfdf1)`. Mạng app-level, các nút trong Settings,
ghép Bluetooth với thiết bị thật và phần cứng chưa được kiểm trong lượt này.

## Gói và phạm vi flash

- Release: `rom/out/releases/M4/star2lte-1793274163-CcPxEn/`.
- Full ZIP SHA256: `c3aff071dec34579a69e7e66f88e134a9714e89d850e2a09582667ae367ee23d`.
- Target-files SHA256: `c741a3d1118169e707d02874d1cc96fc9925db9e1b0101618344e9919c9dbf06`.
- Legacy manifest SHA256: `e200a708f17b90746d5c4dbb5da1a9a5e983c4cb68fc70e835bd9b88bdd3dd2f`.
- Máy đích đã guard là G965F / `star2lte`, bootloader `G965FXXUHFVG6`;
  serial đầy đủ chỉ được giữ trong metadata/log riêng của lượt chạy.
- Tám ZIP32 stage tuần tự đều trả `RC=0`. Installer ghi SYSTEM/VENDOR/ODM/BOOT,
  format CACHE/OMR/HIDDEN/USERDATA; preflight xác nhận không cần repartition.
  USERDATA UUID đổi sau fresh format. EFS/CPEFS/RADIO/RECOVERY/BOTA0–2 giữ nguyên
  hash pre/post. Không có lệnh ghi vào các phân vùng bảo vệ này.
- Rollback images và GPT edge captures được lưu riêng tại
  `logs/20261029-m4-g965f-flash-0222/flash-01/prewrite/rollback/`.
  Không rollback vì Android đã boot và không có thay đổi ở các hash bảo vệ.
- Dùng gói DS-ACK diagnostic đã khóa trong artifact; không build kernel mới.
  G965N không bị tác động.

## Kết quả runtime

| Hạng mục | Kết quả | Giới hạn |
| --- | --- | --- |
| Android boot | PASS tới `sys.boot_completed=1`; Android17 / SDK37 / One UI90000; SELinux Enforcing | Chỉ xác nhận boot G965F; chưa nghiệm thu M4 |
| Màn hình | Ảnh sau khi đánh thức cho thấy trang Chào mừng của Setup Wizard | Wizard chưa hoàn tất; chưa mở Settings hoặc đổi thiết lập |
| Core | Trong mẫu khoảng180s, `system_server` PID2208 và `com.android.systemui` PID9995 giữ nguyên; boot flag và Enforcing giữ ổn định | Chỉ là cửa sổ đo ngắn; không khẳng định không có lỗi hiếm hoặc reboot muộn |
| Bluetooth | FAIL; manager ghi 21 lần crash trong snapshot cuối; trạng thái ON/BLE_TURNING_ON thay đổi khi service tự khởi động lại | Chưa scan, pair, reconnect, gọi HFP hay thử âm thanh Bluetooth |
| USB ADB | PASS trên PC đang nối: `ro.adb.secure=0`, `adbd=running`, USB config `sec_charging,adb`, `/data/misc/adb/adb_keys` không tồn tại | Chưa thử từ một PC mới; chưa kiểm thao tác công tắc sau Setup Wizard |
| Auto Blocker | Các giá trị đọc cuối: `rampart_suw_main_on=0`, `rampart_main_switch_enabled=0`, `rampart_strict_protection_switch_enabled=0`, `rampart_blocked_adb_cmd=0`, `rampart_blocked_usb_data_transfer=0` | Chưa thao tác công tắc UI; `rampart_auto_enabled_switch_enabled` đọc `null`, nên không tính nó là PASS |
| Wireless ADB | `adb_wifi_enabled=1` và `persist.adb.tls_server.enable=1`; `service.adb.tcp.port` rỗng, `ss -ltn6` không có listener, `adb mdns services` rỗng | Listener duy nhất trong `ss -ltn` là `.sec.imsservice` trên cổng6200; không phải adbd. Cờ/property wireless vẫn cần audit so với quy tắc không bật TCP ADB mặc định |
| Mạng | NOT_TESTED ở tầng app/socket | Snapshot có LTE active, Wi-Fi không active; không phát traffic qua dữ liệu di động |
| Reboot bất thường | Không quan sát thấy reboot thiết bị trong cửa sổ stability | Không đủ thời gian để bác bỏ lỗi reboot muộn; lỗi người dùng báo trước đó chưa được coi là đã sửa |

## Bằng chứng lỗi Bluetooth

Trong log runtime có nhiều vòng timeout cùng mẫu. Ví dụ
`logs/20261029-m4-g965f-flash-0222/flash-01/runtime/logcat-afterboot.txt:96022`:

```text
10-29 20:21:51.735 11294 11462 E bluetooth: system/gd/hci/hci_layer.cc:368 on_hci_timeout: [BTCORE] Timed out waiting for Unknown OpCode(0xfdf1) for 5000ms, which was armed at: 2026-10-29 13:21:46.734121
```

Trước timeout, cùng log ghi các event prefix như
`hciEventReceived: len 6, 1st 5 bytes '0e0401f1fd'` tại dòng96017–96020;
đây là một chi tiết cần đối chiếu theo thứ tự gửi/nhận đầy đủ, chưa đủ để kết
luận event sai định dạng hay chỉ ra bên phát lệnh. Sau đó stack đợi debug data
thêm7000ms rồi ngắt service; ví dụ dòng60466:

```text
10-29 20:21:39.363 2208 2208 E BluetoothSystemServer: BluetoothManagerService: ServiceConnection.onServiceDisconnected(com.android.bluetooth.btservice.AdapterService)
```

Crash count tăng từ8 lên21 trong các mẫu stability. Dấu VaultKeeper cũng xuất
hiện riêng: tại dòng218693–218694 log ghi
`Service vendor.samsung.hardware.security.vaultkeeper.ISehVaultKeeper/default didn't start. Returning NULL`,
`Failed to get service of ISehVaultKeeper`, và `vaultkeeper read error(-1)`.
Đây là bằng chứng VaultKeeper service không sẵn sàng trong lượt này, nhưng chưa
chứng minh nó gây timeout HCI `0xfdf1`; hai nhánh cần được phân biệt.

## Giới hạn và bước tiếp theo

Chưa có scan/pair, kiểm âm thanh, browser HTTP/HTTPS, DNS/socket bình thường,
PIF verdict, kiểm nhiệt, camera, NFC RF, GNSS ngoài trời, SIM/RIL hoặc thao tác
Auto Blocker/USB-debugging UI. Không dùng LTE cho thử mạng để tránh phát sinh
dữ liệu di động. Trạng thái G965N không suy ra từ kết quả G965F.

Ưu tiên tiếp theo là source patch0220 đã khóa đúng sender BZI6:
`BtifAvStateMachine::StateOpened::OnEnter()` gửi enqueue tại VA `0x6979e8`;
patch bỏ đúng lời gọi đó bằng một AArch64 NOP, không giả controller reply hoặc
đổi transport. Fixture donor hash/context/Build ID, output bytes, idempotence,
wrong-scope và wrong-preimage PASS. Bản vá chưa được replay đầy đủ, build, đóng
gói hay chạy trên máy tại thời điểm báo cáo gốc này.

**Live follow-up trước build0223:** thiết bị tiếp tục ở Android17
(`sys.boot_completed=1`, SDK37, bootloader `G965FXXUHFVG6`, SystemServer PID2208).
Tới uptime1906.72s, `dumpsys bluetooth_manager` ghi `Bluetooth crashed 116 times`.
Log lưu tại `logs/20261029-m4-g965f-fdf1-followup-0223/prebuild-device/
bluetooth-events.txt`; dòng cuối lặp `Timed out waiting for Unknown OpCode(0xfdf1)`
và `ServiceConnection.onServiceDisconnected(...)`. Đây là lỗi vẫn tái hiện trên
artifact đang chạy, không phải kết quả của patch0220. VaultKeeper vẫn là nhánh
riêng và chưa được chứng minh là trigger HCI. Source patch, clean build, audit
image/ZIP rồi thử lại trên G965F là bước hiện hành. Mạng app-level vẫn chưa thử
vì Wi-Fi không active; không dùng LTE.

## Lý do, file ảnh hưởng và kiểm chứng

Lý do: ghi nhận kết quả fresh flash, tách lỗi runtime mới khỏi các kiểm tra
host-side, và cập nhật bước chẩn đoán dựa trên log. File ảnh hưởng: báo cáo này,
`docs/PLAN.md`, `docs/m4-post-0219-repair-plan.md`, `docs/bugs.md` và `STATUS.md`;
không sửa ROM source hay artifact. Kiểm chứng: đối chiếu package/prewrite/install
receipts, hash phân vùng bảo vệ, boot properties, Enforcing, ADB/Auto Blocker
state, live PID samples và các dòng logcat được trích ở trên. Dữ liệu thô,
ảnh và serial đầy đủ chỉ nằm trong run logs riêng.
