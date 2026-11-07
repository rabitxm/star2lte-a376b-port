# M4 sau0212 — kiểm thử thực tế và kế hoạch sửa tiếp

**Lịch sử:** bản fresh0215 đã được kiểm và kế hoạch hiện hành chuyển sang [m4-post-0215-repair-plan.md](m4-post-0215-repair-plan.md). Kết quả mới: [m4-validation-after-0215.md](m4-validation-after-0215.md).

Ngày28/10/2026. Đây là kế hoạch hiện hành sau khi G965F đã hiện Chào mừng;
thay phần thứ tự/trạng thái cũ trong m4-post-boot-repair-plan.md, giữ tài liệu
cũ làm lịch sử kỹ thuật. Patch0214 đã được flash fresh và reboot kiểm chứng:
nhãn `brightness` hoạt động dưới Enforcing. Fresh log phát hiện thêm lỗi
`max_brightness`; patch0215 mở rộng exact mapping. Cooling/threshold/actuator
vẫn chưa đạt gate.

## 1. Nền và kết quả vừa đo

Máy thử là G965F được chọn trong AGENTS.md, bootloader G965FXXUHFVG6. Nền là
full attempt0209 cùng ba file SYSTEM của0211 và file context0212. Kernel vẫn
là BOOT WIPEONFORK chẩn đoán đã được hiệu chuẩn; không đổi sang kernel khác.
Hybrid Vendor VNDK33, blob F và cầu nối radio giữ nguyên. Không suy kết quả
này sang G965N. ROM hoàn chỉnh mới chứa0211/0212 chưa được đóng gói.

Trong lượt kiểm này không flash/reboot/wipe, không sửa policy hay cài đặt lưu
lâu dài. Các chương trình chẩn đoán tạm chỉ dùng socket/tiến trình con của
chính chúng, sau thử đã xóa khỏi điện thoại. Raw log và ảnh lưu ngoài Git tại
`logs/20261028-m4-validation-after-0212/`, mode600/700. Không chia sẻ raw log
chưa lọc vì dumpsys có thể chứa thông tin SIM, mạng và thiết bị.

| Hạng mục | Kết quả hiện tại | Giới hạn kết luận |
| --- | --- | --- |
| Android | SDK37, OneUI90000, boot_completed1, Enforcing | Chưa là nghiệm thu toàn ROM |
| SystemServer/netd/SurfaceFlinger | PID/starttime giữ nguyên; snapshot uptime1178.75s | Kernel/probe chẩn đoán vẫn còn |
| SystemUI | Đã crash1 lần đầu boot; PID4885 hiện sống1056.75s, hơn17phút | Không được gọi là boot sạch không crash |
| Màn hình/ION | Settings/lockscreen vẽ được;5 lượt sleep/wake không crash mới; không còn lỗi ion_open | Touch vật lý nhiều điểm/độ sáng tự động chưa nghiệm thu |
| Home | Người dùng xác nhận phím Home thật vào launcher bình thường | Lệnh Home ADB không chuyển ngay không chứng minh launcher hỏng |
| ADB/Auto Blocker | adb_enabled1, ro.adb.secure0, Rampart main0; setup đã hoàn tất | Lượt này chỉ đọc; test công tắc và PC mới phải lặp khi fresh package mới |
| Wi-Fi | Đã liên kết11ac và Android báo VALIDATED | Ping/nc qua Bionic đang FAIL; chưa nghiệm thu mạng app/DNS/HTTPS |
| TCP raw control | Kết nối IP bằng syscall trực tiếp PASS, SO_ERROR0 | Không kiểm lớp fwmark, DNS, TLS, policy mạng của Android |
| Bluetooth | BLE_START_TIMEOUT; dumpsys ghi30 lượt crash/restart tới snapshot | Chưa pairing/A2DP/OPP; chưa chứng minh hỏng controller |
| NFC | Process2972, HIDL SLSI1.0/1.1/1.2 +Samsung2.0 và JNI APEX có hoạt động | dumpsys/cmd FAILED_TRANSACTION; RF/tag chưa thử |
| Thermal | SDHMS sống, mẫu nhiệt thay đổi, backlight brightness/max không EACCES; HAL cần expose cooling/threshold và actuator | Backlight permission đã PASS sau0215; HAL cooling/threshold vẫn trống, actuator chưa nghiệm thu |
| Camera/audio/sensor | Camera enumerate3 public devices; các service/sensor đã đăng ký | Chưa chụp/quay/thu âm/phát âm/cuộc gọi để kết luận phần cứng |
| Apps | Bixby crash1 receiver; KG client ANR1 đầu boot có TLC_ERROR | Chưa phân loại đầy đủ component/TA; không phải lỗi modem đã chứng minh |
| Play Integrity | Giao diện PIF đang hiển thị và công tắc bật | Chưa có verdict trực tiếp; không suy từ công tắc |

Mốc cuối lượt kiểm: `end-of-validation.txt` có uptime2052.12s, cùng PID core và ION đúng, Enforcing.

Nguồn số liệu: `validation-summary.json`, `stability-samples.json`,
`sleep-wake/cycles.json`, `final-state.txt`, `final-crash.txt`,
`error-index.json` trong thư mục log trên. Các mốc nhiệt khác nhau: raw TMU BIG
có lúc80°C; HAL AP/SKIN là giá trị/biểu thức khác. Không coi chúng là cùng cảm
biến hoặc kết luận sai scaling trước khi đọc công thức SDHMS. Sau tải nhẹ giảm,
HAL AP xuống37.8°C và SKIN34.9°C ở snapshot cuối; đây là quan sát cooldown,
không phải bằng chứng mọi limiter đã điều khiển đúng.

## 2. Thứ tự và nguyên tắc thử

Thứ tự sửa: **quyền điều khiển thermal tối thiểu → vòng restart Bluetooth →
đường fwmark/mạng → hợp đồng AOD → NFC và app còn lỗi → nghiệm thu phần cứng →
package sạch và fresh-flash**. Có thể đọc source của nhóm sau trong lúc chờ
log; mỗi trial trên máy chỉ thay một nhóm có manifest và rollback riêng.

Không build lại nguyên ROM cho mỗi thí nghiệm nhỏ. Dùng điểm mở rộng của
fork hiện có, preimage/output hash, compiled/image gates và đọc ngược file
sau ghi. Khi nhóm sửa đã qua gate, mới tích hợp vào một build sạch chung.
Không sửa kernel hoặc radio từ giả thuyết; raw TCP PASS không chứng minh
mọi syscall/ancillary-message của kernel đều tương thích.

Mọi patch phải ghi lý do/file/kiểm/rollback; đánh số kế tiếp khi thực sự tạo.
Không đánh dấu các việc trong kế hoạch này là patch đã có.

## 3. Thermal: khép quyền và đường điều khiển trước test tải

**Bằng chứng:** `final-logcat.txt` trong log hiện tại có nhiều dòng:
`SDHMS:f1: ... /sys/class/backlight/panel/brightness: fstat failed: EACCES (Permission denied)`.
`thermal-metadata.txt` cho thấy đường thật
`/sys/devices/platform/panel_drv@001/backlight/panel/brightness` là0644,
system:system, nhưng mang nhãn chung `sysfs`; SDHMS ở `system_app`.
`ThermalManagerService` còn báo `no cooling device for cooling type 0` và
`no threshold data for temperature type 0`.

**Đã sửa bằng patch0214 và đang mở rộng bằng0215:** node thật ban đầu mang `sysfs` do đường
`panel_drv@001` không khớp genfscon cũ `panel_drv@0`; source policy đã có type
`sysfs_ski_display_writable` cùng quyền `system_app_33_0` cần cho `getattr` và
đọc. Một nhãn thử tạm trên đúng node nhận được context đó; trong khoảng quan sát
không xuất hiện lỗi `SDHMS:f1 ... fstat failed`, rồi nhãn runtime được trả về
`sysfs`. Patch0214 thêm genfscon cho brightness; flash fresh xác nhận context
runtime đúng và lỗi brightness không còn. Log sau boot ghi thêm:
`10-28 19:23:37.606 8737 8859 E SDHMS:f1: e = /sys/class/backlight/panel/max_brightness: fstat failed: EACCES (Permission denied)`.
Thử tạm gán existing display type cho đúng node max trong20s không có lỗi mới,
rồi khôi phục nhãn gốc. Patch0215 thêm node này vào exact genfscon list; không thêm allow và không gán nhãn cả thư mục. Candidate fresh-flash/reboot PASS: hai node mang type đúng dưới Enforcing, không còn backlight fstat EACCES. Khi Awake, Settings60/180 đổi sysfs brightness; setting được khôi phục230. `actual_brightness` sample đứng3 nên chưa kết luận độ sáng panel thực. SDHMS vẫn báo EACCES trên RTC date/time/since_epoch; đó là node khác.

**Việc làm:**

1. So mapping đường backlight thực với vendor file_contexts/genfscon và policy
   đã compile; phân biệt DAC, SELinux, nhãn tại symlink/đường thật và hành vi
   restorecon. Có dontaudit thì dùng truy vấn policy/mở thử đúng UID/domain,
   không tắt Enforcing để lấy kết quả đẹp.
2. Kiểm loại nhãn backlight mà M1/fork đã dùng cùng quyền hiện có. Nếu nhãn
   thiếu là nguyên nhân, thêm đúng đường/nhóm node cần thiết, không allow rộng
   `system_app sysfs:*`. Chỉ thêm allow tối thiểu khi đã có tuple và chứng minh
   loại nhãn đúng vẫn thiếu quyền. Không thay giá trị nhiệt hay ngưỡng để né lỗi.
3. Đọc đường SDHMS chọn policy, sensor/formula SKIN/AP/BAT và limiter; đối
   chiếu hai asset SIOP/SSRM bằng hash. Policy DEX-filter được khóa trong `a376b_android17_dvfs_policy.lock.json` (0200).
   Nhánh DEX đặc biệt đã được user cho bỏ; các nhánh chung phải giữ ý nghĩa.
4. Tách việc thermal HAL thiếu cooling/threshold khỏi SDHMS. Kernel có cooling_device thật không có nghĩa HIDL HAL đã expose/SDHMS đã dùng đúng. Runtime fresh0215 cho thấy kernel có cooling_device0–3 (`thermal-gpufreq-0`, `thermal-isp-0`, `thermal-cpufreq-0/1`, cur_state0/0/0/2) và trip points trên BIG/LITTLE/G3D/ISP, nhưng `dumpsys thermalservice` trả danh sách cooling devices/static thresholds trống; log cụ thể báo `no cooling device for cooling type 0` và `no threshold data for temperature type 0`. BIG có passive83°C, LITTLE active76/81/86/91/96/101°C, G3D passive88°C; đây chưa chứng minh Samsung AP threshold/hysteresis mapping. Đọc mapping trong HAL/config và policy BZI6 so với vendor G965F trước khi sửa; chưa đổi kernel hoặc tự ánh xạ tên. Lập bảng sensor→formula→threshold/hysteresis→actuator→readback.

**File patch0214/0215:** `exynos9810_boot_restore/customize.sh`,
`a376b_android17_thermal.py`/lock và `verify_m4_runtime_image.py`. `unica/patches/dvfs/`
chỉ sửa khi audit chứng minh policy cần đổi; hiện chưa có bằng chứng để đổi
threshold hay policy SIOP/SSRM.

**Qua gate:** mẫu nhiệt có timestamp/liveness; quyền brightness hết lỗi trong
đúng process; limiter/action và readback thật khi điều kiện xảy ra; cooldown
và hồi phục có log. Trước camera/video dài phải có giới hạn dừng từ policy
S9 đã xác minh và sensor độc lập. Chưa chạm một threshold thì ghi chưa thử.
Rollback đúng file context/policy đã đổi; giữ bản đọc lại và manifest trước thử.

## 4. Bluetooth: tìm pha mắc trước khi can thiệp HCI

**Bằng chứng:** `final-state.txt:71`: `Bluetooth crashed 30 times`.
`logcat.txt:249275` ghi `BLE_TURNING_ON : BLE_START_TIMEOUT`; các vòng lặp có
`irk read failed` và truy vấn AIDL VaultKeeper bị từ chối. Cùng log có
`HciLayer::reset is completed` và `OnFirmwareConfigured`, nên không mặc định
lỗi là UART/firmware/controller không lên.

**Việc làm:**

1. Pin APK/APEX Bluetooth và JNI **đang thực sự được map**; so JNI đặt ở SYSTEM
   với JNI trong APEX và output module Bluetooth. Ghi các pattern cũ bị skip
   khi donor thay layout; không dùng LOGW như gate runtime PASS.
2. Theo timeline từng lần enable: firmware/HCI reset→storage/identity/IRK→GATT
   registration→BLE ready callback→timeout→abnormal recovery. Xác định bước
   nào chưa trả callback, không tăng timeout trước khi biết nguyên nhân.
3. Truy vấn VaultKeeper là đầu mối, chưa là nguyên nhân đã chứng minh. So
   capability/selector A17 với backend HIDL/prop thực có trên target, key
   provider và secure-storage fallback hợp lệ. Không đọc/ghi IRK, link key,
   EFS hoặc tự tạo key giả để vượt init.
4. Nếu fail do API mới: port đúng selector/call flow trong JNI/Java BZI6 với
   preimage/disassembly. Nếu fail do lệnh vendor không được controller hỗ trợ:
   cần log opcode/response cụ thể trước khi sửa capability. Không áp dụng
   hàng loạt pattern Android16 cho binary17 hoặc thay kernel.

**File dự kiến:** `rom/unica/patches/bluetooth/customize.sh`; helper/lock M4
trong module đó nếu cần; existing finalization để giữ đúng output cuối và
image gate cho APK/JNI. Giữ nguyên bộ HAL/firmware/radio target trong trial đầu.

**Qua gate:** bật/tắt10lần; BLE/BT đạt ON và giữ15phút không tự restart; sau đó
scan/pair thiết bị thật, reconnect, A2DP và OPP hai chiều. HFP/mic cuộc gọi
là gate riêng. Phải có thiết bị đối tác thật cho từng profile. Rollback đúng
APK/JNI/APEX của unit và snapshot thư mục data liên quan nếu schema đổi;
không xóa toàn bộ dữ liệu Bluetooth để che lỗi.

## 5. Mạng: xử lý lỗi fwmark đã bắt được

**Bằng chứng trực tiếp:** ping IP và DNS exit2, nc exit1 đều `Message too long`.
`socket-raw-syscalls.txt`: nonblocking connect→EINPROGRESS→poll writable→
SO_ERROR0, exit0. `trace-network-v5.txt` theo dõi **chỉ tiến trình ping con mới**:
connect AF_UNIX thành công, sendmsg52byte, recv4byte có int32=-90; ping thoát
trước khi gọi connect AF_INET tới đích. Chỉ đọc bốn byte mã lỗi, không lấy key.

V1/v2 tracer có lỗi vị trí tham số PTRACE_SETOPTIONS và phân loại entry/exit;
đã giữ nguyên log và đánh dấu không dùng kết luận. V3/v4 sửa đúng raw syscall,
nhận TRACEEXEC4 và các syscall-stop có TRACESYSGOOD. V4 đọc mã trả về; V5 xác nhận thêm pathname /dev/socket/fwmarkd.

Installed `netd`, `libnetd_client.so`, ping đều khớp **BZI6**. Netd còn map
`libnetd_updatable.so` S22 trong composition đã khóa, nhưng audit strings chỉ
ra FwmarkServer nằm trong binary netd BZI6. Vì vậy **chưa chứng minh** chỉ cần
đổi libnetd_updatable hoặc đổ nguyên nhân cho “APEX cũ”.

**Việc làm:**

1. Trace/đọc code receive bên server: command ID, layout/sizeof, recv length,
   truncation flags, số FD/SCM_RIGHTS và mã trả lỗi. So sender52byte với đúng
   branch server trả EMSGSIZE. V5 đã xác nhận pathname /dev/socket/fwmarkd của kết nối này.
2. Tạo socketpair control trong tiến trình chẩn đoán: gửi52byte+kèm FD, kiểm
   nhận/readback trên kernel hiện có. So raw syscall với Bionic/namespace
   thật, và UID shell/app. Không kết luận kernel chỉ từ tên errno.
3. Theo call graph/default namespace/APEX callback để xác định **file sở hữu
   lỗi thật**. Ưu tiên adaptation giao thức/provider nhỏ, giữ SDK37, network
   selection/UID/permission/VPN semantics. Nếu thay provider BZI6, phải audit
   dependency/symbol/version/CFI và import visibility trước; không thay libbase
   dùng chung hoặc chép cả APEX để né một lỗi.
4. Giữ resolver SYSTEM0211, libui/V6, network Java stats/transport và BPF
   composition đã chứng minh; mỗi output mới có lock và APEX gates như trước.

**File dự kiến:** builder/lock `a376b_android17_network_native.*`,
`a376b_android17_postboot_diagnostic.py`, finalization và image verifier.
Chỉ thêm native adapter ở module hiện có khi đã xác định ABI; không tạo build
system mới, trả success giả hoặc vô hiệu network policy.

**Qua gate:** ping/nc bình thường hết EMSGSIZE; DNS và HTTP/HTTPS từ app có dữ
liệu thật; Wi-Fi DHCP/reconnect; thống kê UID/interface tăng theo traffic;
blocked/unblocked và chọn network trả đúng. Test VPN/hotspot/mobile data riêng,
có giới hạn dữ liệu và đúng SIM. VALIDATED hoặc raw TCP một mình chưa đủ.
Rollback giữ snapshot cả hai netstats directory theo protocol197 khi reader/
writer thay; restore APEX/config/DSO theo unit, không rollback ION0212 vô cớ.

## 6. SystemUI/AOD: sửa hợp đồng, giữ Infinity

**Bằng chứng:** `final-crash.txt:18` là AbstractMethodError của
`disconnectPluginOnBackground()`. Installed SystemUI/AOD khớp host APK đã
hash; SystemUI compiled đã decode riêng để kiểm API. Trong62 method interface,
retained implementation thiếu12 abstract:10 có VersionCheck,2 không có là
`disconnectPluginOnBackground()V` và `setDarkAmounts(FF)V`. Cả hai có caller
trong SystemUI; chưa có crash setDarkAmounts trong log, đây là rủi ro API đã
xác minh tĩnh. BZI6 AOD có disconnect, nhưng không có custom setDarkAmounts
của interface đã port, nên thay nguyên APK BZI6 chưa đủ.

**Việc làm:**

1. Audit thêm callback interface/annotation, version proxy, clock/notification,
   AODPlugin và Infinity hooks. Không gọi12 method thiếu là12 crash đã đo;
   VersionCheck phải được kiểm đúng khả năng bỏ qua từng method.
2. Ưu tiên adapter hẹp nếu semantics tương đương: BG disconnect phải dừng đúng
   observer/listener/continuation của plugin cũ; phần View/Lifecycle cleanup
   chạy main; generation/instance guard0201 giữ nguyên.
3. Với setDarkAmounts, truy đường hai giá trị doze/transition tới model/scrim/
   wallpaper của retained AOD; mapping phải làm việc thật. Không thêm no-op,
   catch AbstractMethodError, ép getVersion hoặc bỏ kiểm luồng để hết crash.
4. Nếu DI/lifecycle quá khác, rebase phần FaceWidget/AOD lên BZI6 rồi port lại
   hook Infinity có chọn lọc. Giữ wallpaper engine/catalog/policy và sensor
   pair target; không copy class obfuscated nguyên khối giữa hai APK.

**File dự kiến:** `s9_infinity_Wallpaper/source/rebuild.py`, source FaceWidget
mới trong module đó, `source/aod/`, payload SHA256SUMS, customize/gate wallpaper;
caller adapter `a376b_android17_facewidget_thread.py`/lock chỉ đổi khi cần.
Phải xử lý đúng package/DEX, chữ ký theo fork và stale oat; không sửa cache
apktool như bản sửa cuối cùng.

**Qua gate:** contract có annotation/guard đầy đủ; assemble/redecode; race
connectA→disconnect→connectB và classloader discard; callback hoàn tất trước
getter/attach; main-thread teardown đúng. Fresh boot không crash SystemUI,
15phút ổn định,20lượt lock/wake, bật/tắt AOD, notification/media/widget,
Infinity5 native+5 UN1CA, preview/apply, lock/home và wallpaper thường.
Lượt kiểm hiện tại chỉ qua5 wake không crash thêm, chưa đạt gate này vì boot
đầu đã crash và plugin có thể đang bị cô lập sau lỗi.
Rollback unit SystemUI/AOD/wallpaper đúng pair; giữ0211/0212 đã đạt.

## 7. NFC và các lỗi app: tách lỗi truy vấn khỏi lỗi chức năng

NFC `dumpsys`/`cmd` trả FAILED_TRANSACTION cả root lẫn UID2000 (contextsu);
SDHMS dump cũng lỗi tương tự. NFC process/HIDL/JNI vẫn sống, có screen-state/
routing callbacks; chưa có bằng chứng RF/tag đã hoạt động.

1. Tái hiện Binder dump/shell-command bằng caller đúng UID **và domain**, so
   permissions, FD transfer, Parcel/kernel reply, kích thước phản hồi và log
   transaction. UID2000 trong domainsu chưa tương đương shell domain đầy đủ.
2. Kiểm adapter state qua API app đúng hợp đồng/current compiled classloader;
   NFC on/off, RF enable và HAL callbacks. Giữ JNI Java37 trong APEX và target
   SLSI; không quay lại JNI PHN72method khi Java BZI6 có54method.
3. Sau query path rõ ràng, tag read nhiều lần, screen off/on, reboot; HCE/eSE/
   UICC riêng khi có tag/thiết bị kiểm thực. Tính năng thanh toán không được
   đánh dấu PASS từ service sống hoặc Settings bật.

File dự kiến chỉ khi có bằng chứng cần đổi:
`exynos9810_device_stack/nfc/a376b_android17_nfc_native.py`/lock,
`unica/patches/nfc/customize.sh`, image gate; không sửa HAL/RF blob trước
khi log yêu cầu. Nếu lỗi chỉ là công cụ dump thì sửa công cụ chẩn đoán,
không coi đó là lý do thay NFC payload.

Bixby: current boot có `No matching ComponentResult found!` ở
LockedBootCompletedReceiver. APK thật `/system/priv-app/Bixby/Bixby.apk` được
finalization giữ donor-signed. Đọc registry/feature/component/package state
trong receiver, so manifest cùng donor; ưu tiên sửa selection/input config
của module hiện có. Không bật lại remap đã hoãn hay thay APK rồi bỏ qua ảnh
hưởng chữ ký/Samsung Account. Test lại boot receiver và mở app thực.

KG client: current boot có1ANR LOCKED_BOOT_COMPLETED, các TLC_ERROR/getTAInfo.
Thu đúng stack và service/TA timeout hiện tại; phân biệt lỗi TA không hỗ trợ
với lỗi queue/receiver. Giữ account UID/vendor_kg và bảo vệ EFS/modem; không
trả TA success giả, thay keybox hoặc vô hiệu bảo vệ thiết bị. Nếu giới hạn TA
của port không khắc phục được trong phạm vi nguồn hiện có, ghi hạn chế thực.

Google/app `dlopen failed` không kèm crash có thể là đường optional/module
fallback. Chỉ nâng thành bug khi có failure chức năng hoặc thiếu closure bắt
buộc được chứng minh; không copy thư viện hàng loạt từ donor khác.

## 8. Nghiệm thu phần cứng và package mới

Sau thermal tối thiểu và các vòng restart đạt, kiểm có log riêng từng nhóm:

| Nhóm | Test thực | Bằng chứng cần giữ |
| --- | --- | --- |
| UI/touch/display | Home/Settings/QS, touch nhiều điểm thật,20lock/wake, brightness manual/auto | Frame/focus, input thật, sensor lux, brightness readback, process starttime |
| Camera/media | Rear/front preview, ảnh thật, video ngắn rồi playback | HAL events, output decode/size/duration, codec error, nhiệt/throttle |
| Audio | Loa/tai nghe/mic ghi-phát, route/call bình thường | File ghi thực, audio route, nghe vật lý; không gọi số khẩn cấp để thử |
| RIL/SIM | Nhận SIM, cuộc gọi/SMS/data trên SIM thử | Registration/route/data; không public số/IMEI; F/N nghiệm thu riêng |
| Wi-Fi/BT/NFC | Reconnect/traffic, pairing/profile, RF/tag thật | Gate riêng các nhóm trên, không chỉ service dumps |
| Sensor/GPS | Rotation/proximity/light, GNSS fix khi điều kiện cho phép | Events/timestamp, sai số/fix; enum sensor không thay test |
| Power/thermal | Idle/screen-off/wake, sạc/rút, tải nhẹ→camera→cooldown | Sensor độc lập, limiter/action/readback, không chỉ service sống |
| ADB/PIF | Defaults fresh, GUI toggle có tác dụng, PC mới/noRSA; Integrity verdict riêng | Compiled gate+runtime; công tắc PIF không là verdict |

Gỡ probe/logger bằng **một trial riêng**, giữ kernel/DTB/cmdline/provenance;
đối chiếu ramdisk khác biệt và BOOT hash mới. Chưa tự gọi diagnostic WIPEONFORK
là production package đã nghiệm thu; không đổi kernel trong các trial trên.

Khi các gate chức năng đạt: build sạch bằng fork, input cache khóa donor/version,
patch replay, compiled ADB/AutoBlocker/PIF, SELinux/fs_config và ext4 image thật;
APEX manifests/AVB/container alignment; gói full+8ZIP32, CRC/signature/hash/
manifest trong run riêng. Fresh-flash G965F, kiểm USERDATA UUID/defaults,
cold boot+reboot rồi lặp gate chính. Chỉ thay bản M4 đang giữ sau khi package
mới hoàn chỉnh và kiểm đủ; M1/M2 giữ đúng latest theo retention. G965N vẫn có
kế hoạch blob/radio/kernel/kiểm riêng, không chuyển mốc từ kết quả F.
