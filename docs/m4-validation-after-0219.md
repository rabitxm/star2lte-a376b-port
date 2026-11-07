# M4-0219 — kiểm thử G965F và báo cáo lỗi, 29/10/2026

**M4 chạy Android và core ổn trong cửa sổ đo, nhưng chưa nghiệm thu.** Bluetooth
vẫn lặp khởi động; đường connect thông thường vẫn lỗi EMSGSIZE. Đã có native
stack khóa đúng điểm chờ Bluetooth. NFC getState đã kiểm được bằng Binder
không truyền FD. Audit thermal trước có kết luận sai vì bỏ qua overlay; đã
đính chính bằng resource hiệu lực và kiểm compiled code.

Kế hoạch sửa hiện hành: [plan sau0219](m4-post-0219-repair-plan.md). Báo cáo
này thay trạng thái kiểm thử hiện tại của báo cáo0217; giữ các log cũ làm lịch sử.
Không chuyển kết quả G965F sang G965N.

## Nền và phạm vi

- Máy đã guard theo USB serial/model phần cứng: `SM-G965F`, bootloader
  `G965FXXUHFVG6`, `star2lte`, SDK37/One UI90000, Enforcing, boot_completed1.
  `ro.product.model=SM-A376B` là thuộc tính donor; không dùng nó nhận diện máy.
- Bản cài theo hồ sơ M4-0219: full ZIP SHA256
  `13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`.
  Lượt này không cài lại ROM; đã đối chiếu hash APK/native đang chạy với build.
- Runtime kernel vẫn `4.9.337-DS-ACK-V1.12-G965F-08.05.2026`, profile/runtime
  và Hybrid Vendor/blob F/radio bridge hiện hành. Không sửa/build kernel.
- Snapshot bắt đầu16:54:41; log live16:56:16–16:59:26. Có18 mẫu PID/starttime
  trải khoảng180s, sau đó5vòng sleep/wake, một TCP control và thử mở Chrome.
  Không clear logcat, không reboot/flash/wipe, không ghi EFS/CPEFS/modem/bootloader.
- Log riêng ở `logs/20261029-m4-validation-0219/run-1655/`, directory0700,
  files0600; raw log, ảnh, Wi-Fi/location dump chỉ lưu cục bộ, không đăng public.
  Executable TCP tạm có hash guard, chỉ handshake, không payload, đã xóa và
  kiểm tra đường dẫn không còn. Không đọc nội dung Bluetooth key.

## Ma trận kết quả

| Hạng mục | Kết quả đo lần này | Giới hạn/verdict |
| --- | --- | --- |
| Core |18mẫu, boot ID không đổi, ADB lỗi0; SystemServer1848, netd1456, SurfaceFlinger1553, SystemUI4212, SDHMS3960, NFC2513 giữ PID/starttime đến final snapshot|PASS cửa sổ đo; startup Java faults vẫn còn|
| Sleep/wake/display |5vòng Dozing→Awake, raw panel0→6224, actual2→23; PID core không đổi, crash buffer byte-identical|PASS đường vừa thử; không chứng minh đủ AOD/widget/HBM|
| RTC |3node vẫn `sysfs_rtc`, đọc được, since_epoch tăng; không có RTC EACCES mới trong log live|RTC gate tiếp tục PASS|
| ADB/Auto Blocker |ro.adb.secure0, adb_enabled1; secure main/strict/auto0, blockedADB/USB0; ADB ổn|Không lặp cắt ADB/toggle/PC mới; chức năng đã được người dùng xác nhận trước|
| Bluetooth |7PID trong18mẫu;6BLE_START_TIMEOUT trong log live; manager60→69 giữa baseline16:54 và final16:59|FAIL, chưa ON/scan/pair|
| Wi-Fi/connect |Wi-Fi link COMPLETED/VALIDATED; ping IP và tên miền đều rc2 `connect: Message too long`; raw TCP control rc0/so_error0 cùng máy|FAIL connect thông thường; link VALIDATED không thay phép thử socket|
| Browser |VIEW example.com resolve vào Chrome nhưng mở FirstRunActivity phía sau keyguard; không chấp thuận onboarding|HTTP/HTTPS qua app NOT_TESTED, không gọi đây là network PASS hay browser network failure|
| NFC |Runtime framework jar khớp stub đã decode; transaction6 trả NoException+state3 ON|PASS state query; tag RF/HCE/UICC/eSE/payment chưa thử|
| Thermal |HAL có nhiệt; effective overlay trả max867 và bảng306/408/510; compiled13class/194method đối chiếu PASS|Đã có provenance tốt hơn; actual caller/gates/ngưỡng/hồi phục chưa nghiệm thu|
| AOD/FaceWidget |Crash buffer cùng boot có lỗi disconnectPluginOnBackground lúc16:21:24|Lỗi0219 còn tồn tại; không crash mới trong live/sleep-wake|
| Bixby |Crash buffer cùng boot lúc16:21:40: `No matching ComponentResult found!`|Lỗi startup0219 còn tồn tại; không crash mới trong cửa sổ|
| Provider/GNSS |Không có provider exception/GNSS fatal mới trong live; GPS request OFF,0GNSS fixes trong dump|Provider là lỗi lịch sử chưa khép; GNSS chưa kiểm ngoài trời, không suy network location thành GPS fix|
| Play Integrity/PIF |Không đo verdict trực tiếp trong lượt này; giữ source/gate PIF đã port|NOT_TESTED runtime; kiểm sau normal network và Google services|
| Camera/audio/mic/fingerprint/RIL |Có service/inventory; không chụp/quay, thu âm, gọi điện hoặc test vân tay mới|NOT_TESTED hành vi trong lượt này; giữ xác nhận phần cứng cơ bản trước của người dùng là bằng chứng riêng|

Các key Rampart đọc đúng bảng secure. Collector nhận `null` ở key
`rampart_suw_main_on`; không dùng null đó làm bằng chứng mặc định OFF.
Không đổi brightness63/manual0/stay-awake0. Sau thử Chrome đã bring lại đúng
Settings SubSettings task15 có sẵn; final màn hình Dozing, Settings giữ nguyên.

## Bluetooth: call path gây chờ đã bắt được

`bt-native-backtrace.txt:12–23` ghi16:57:20, process29817:

```
AdapterService.startScanController → enableNative → stack_enable
→ btif_dm_load_ble_local_keys
→ btif_storage_get_ble_local_key +468 (JNI pc0x7aad40)
→ VaultKeeperManager::read +60 (pc0x5504)
→ AServiceManager_getService → CppBackendShim::getService → usleep
```

Guard process/maps và PID sau dump xác nhận đúng Bluetooth. Runtime JNI hash
`f8a662ca15d9526ed8271af8ecdb97bb09e9b850386d0f77abe17eb77c030757`,
vkmanager `0fdf61d254ff10c50f9f5e9027e3d2f5e0ac1fb938637c086540f85ffb1596e4`.
Service check AIDL `ISehVaultKeeper/default` trả `not found`. Log live dòng7:
`avc: denied { find } ... ISehVaultKeeper/default ... tclass=service_manager permissive=0`.
Cấp generic allow chưa tạo được provider đang thiếu.

Cùng PID29817, `logcat-live.txt:3350` lúc16:57:32.782:
`startScanController(): Completed in 29278ms`; dòng3386:
`BLE_TURNING_ON : BLE_START_TIMEOUT`. Hai vòng trước native dump đã có lỗi
29183/29174ms; không quy chuỗi restart cho hành động lấy stack.

Disassembly phát hiện branch BZI6 mà patch cũ chưa nhận; **chưa áp bypass**.
Fallback config16byte có thật, nhưng backend VK dùng encrypted32byte. Shim
GetBin copy min(capacity,stored_size) rồi vẫn báo success; ép đường16 có thể
đọc prefix ciphertext32 thành key. Các flag còn dùng cho local/BLE/BR-EDR
bonded readers/writers. Chi tiết, addresses và fixtures:
`bluetooth-backend-audit.txt`. Đơn vị sửa phải giữ format/identity semantics.

## Mạng: raw control mới PASS, owner EMSGSIZE chưa khóa

`ping-ip.txt` và `ping-dns.txt` đều ghi `connect: Message too long`, rc2.
`tcp-raw-current.txt` lần này ghi `connect_rc=-115`, poll writable,
`getsockopt_rc=0`, `so_error=0`, `exit=0`, uid0; không gửi payload. Đây là
control bypass libc/netd hook, không phải PASS đường app/Bionic/DNS/HTTPS.

Trace0212 trước đó thấy fwmark AF_UNIX send52byte/1FD rồi recv int32-90 trước
IP connect. Các raw SCM_RIGHTS control0215 đã PASS. Lượt0219 này không trace
receive của netd; không đổi MTU/kernel/libbase dựa trên errno đơn lẻ. Bước kế
là diagnostic gọi đúng BZI6 ReceiveFileDescriptorVector, rồi đo metadata
receive thật nếu wrapper PASS. Browser FirstRun không cung cấp verdict web.

## Thermal: đính chính kết luận trước, chưa sửa ngưỡng

Kết luận trước “resource toàn-1/max255 nên cap306/408/510/867 phải qua j5/a1”
là **sai** vì chỉ đọc base framework-res. `overlay-effective.json` lookup
trong context SDHMS/user0 trả max867 từ product RRO, cover255, PD formula rỗng:

```
-1,-1,-1,-1,510,408
-1,-1,-1,510,408,306
-1,-1,510,408,306,306
-1,510,408,306,306,306
```

`k5/r1` chọn row theo biến thiên3mẫu SKIN, column theo
`max(trunc((SKIN-350)/10),0)`; cell-1 hoặc column>=6 dùng fallback867.
Nó phù hợp các mức đã thấy, kể cả415→867 rồi404→408 và398→510. Log SIOP kế
cận không phải trace buffer/gates của callback nên **giữ cả k5/s1 và j5/a1
là ứng viên**, chưa quy caller. BrightnessControlForTG không tự đồng nhất với
HighBrightnessForTG. Với bảng hiệu lực này, registration gate có entries306;
không còn căn cứ nói callback không thể hoạt động.

Full APK installed khớp built3e8b54...,13class/194method built và donor đã được
baksmali kiểm độc lập, không có method khác. DEX hash tổng khác do rebuild;
không tuyên bố whole-DEX identity. Chi tiết: `thermal-compiled-check.txt` và
[thermal audit đã sửa](m4-thermal-limiter-audit-0219.md).

Nhánh table trả fallback từ SKIN>=410 là hành vi static đã thấy; chưa kết
luận là lỗ hổng bảo vệ vì còn base SIOP/HBM và kernel. Không có nhiệt kế ngoài,
không gây nhiệt/workload; chưa nghiệm thu threshold/hysteresis/cooling thực.

## Startup history và cửa sổ mới

`crash.txt:17` lúc16:21:24.495 ghi
`AbstractMethodError ... disconnectPluginOnBackground()`; dòng33 lúc16:21:40.402
ghi `IllegalStateException: No matching ComponentResult found!`. Đây là hai
startup events cùng boot đã xảy ra trước snapshot khoảng33phút. Trong190s
live capture và5vòng sleep/wake không có FATAL EXCEPTION/Fatal signal/ANR/
WATCHDOG mới; crash buffer cuối vẫn byte-identical với đầu lượt. Bluetooth
state-machine restart vẫn diễn ra, không cần có Java/native fatal để là FAIL.

GNSS firstboot0219 trước đó có SIGSEGV/null+4, stack AGnssRil callback→RefBase;
không tái hiện trong lượt này. SystemUIIndexProvider failure0217 chưa tái hiện.
Đã sửa cách diễn đạt “zero fatal” ở RTC report thành native Fatal signal trong
đúng cửa sổ, không che startup Java faults và không biến RTC PASS thành M4 PASS.

## File và kiểm chứng

Lý do: báo cáo đúng lỗi hiện tại, khóa call path Bluetooth, phân biệt live
window với buffered startup, sửa sai thermal attribution và lập repair unit.
File ảnh hưởng: báo cáo này, plan sau0219, PLAN/bugs/README/STATUS, chú thích
plan0217, RTC report, thermal audit và evidence logs. Không patch ROM source.

Kiểm chứng: collection-status có exit/stdout/stderr từng lệnh; semantic
FAILED_TRANSACTION không được coi PASS chỉ vì rc0; process stat có starttime;
raw TCP source/binary/runtime hash/cleanup; NFC stub+runtime jar hash;
resource effective lookup;194compiled methods; final foreground/wakefulness/
Settings và crash-buffer comparison. Manifest SHA256/mode lưu cùng run.
Không flash/reboot/wipe trong lượt này; package đang giữ không bị thay.
