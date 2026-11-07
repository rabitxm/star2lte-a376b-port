# M4 sau0217 — phương án sửa chi tiết

**Đã được thay trạng thái/thứ tự bằng [plan sau0219](m4-post-0219-repair-plan.md) và [kiểm thử0219](m4-validation-after-0219.md).** Nội dung dưới giữ lịch sử; thermal inference đã sửa theo effective overlay.

Ngày29/10/2026. Kế hoạch dựa trên [kiểm thử0217](m4-validation-after-0217.md),
thay thứ tự/trạng thái của plan sau0215. Patch0217 KG30 giữ fail-closed khi
provider còn thiếu; SystemServer ổn định trong các cold boot Enforcing đã
ghi. Đơn vị RTC patch0218 đã PASS trên package M4-0219: build/image gate,
fresh install, hai cold boot, exact labels, advancing RTC và không còn RTC
`fstat EACCES`. Chi tiết ở [RTC report](m4-rtc-label-0218.md). Đơn vị hiện tại
là thermal sensor/policy/actuator provenance; M4 chưa nghiệm thu tổng thể.

## 1. Nền cố định và điều kiện chung

G965F BZI6/SDK37, Hybrid Vendor VNDK33/blob F/radio bridge hiện hành;
M4-0219 full ZIP SHA256
`13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`.
Giữ đúng profile
`rom/target/star2lte/profiles/m4-g965f-a376b-android17-runtime-init-debug.sh`
và gói kernel hiện hữu `4cea63f84d5add494bfcc2f9fff105848806a930a24aca975dc261bf6720e20a`.
Không chuyển sang profile no-ksu mặc định làm mất backport/logger như lượt0216.
Không có bằng chứng mới yêu cầu build kernel, đổi modem hoặc ghép blob khác.

Patch0216 đã qua raw scale,5vòng doze/wake và người dùng xác nhận màn hình thật
đổi sáng bình thường. Giữ patch này; không cần thay bằng độ sáng cố định.

| Thứ tự | Unit | Điều kiện xong trước khi đi tiếp |
| --- | --- | --- |
|1 — P0| KG30 blocking lookup/retry gây watchdog | **M4-0218 PASS:** ba cold boot Enforcing, SystemServer ổn định, lookup thiếu provider trả fail-closed nhanh; provider vẫn absent |
|2 — P0 cho workload dài| RTC exact labels — patch0218 **PASS**; thermal provenance/API actuator subgate **PASS** | Sensor/policy mapping đã đối chiếu; cần xác minh trigger/cooling/cooldown tự động qua ngưỡng thật |
|3 — P1| Bluetooth VaultKeeper selector | ON trước timeout;10toggle,15phút không restart rồi test peer thật |
|4 — P1| fwmark/libbase receive boundary | Xác định nhánh trả-90 và sửa đúng owner; IP/DNS/HTTP/HTTPS qua đường app thực PASS |
|5 — P1| FaceWidget/AOD contract; provider loader là unit riêng | Không thiếu callback thực sự dùng; lifecycle/search/restart ổn, không xóa chức năng che lỗi |
|6 — P2| Công cụ Binder rồi NFC RF | Query trạng thái tin cậy; thẻ thật/off-on/reconnect qua gate riêng |
|7 — P2| Bixby registry; GSA có điều kiện | Không receiver crash, khóa version; chỉ sửa GSA nếu tái hiện với provenance đủ |
|8| Package tích hợp/nghiệm thu | Source replay/compiled/image/ADB gates và fresh test matrix đều PASS |

Có thể phân tích source song song; mỗi trial trên máy chỉ thay một unit.
Trước ghi: chọn serial/model/bootloader, xác minh preimage/hash/metadata,
backup unit và rollback. Không ghi EFS/CPEFS/RADIO/bootloader/recovery. Giữ
Enforcing, giữ ADB no-auth/defaults và tác dụng thật của công tắc người dùng.

## 2. Unit đầu tiên: gỡ chặn boot KG30 bằng lỗi có giới hạn

**Lý do.** ANR gốc `firstboot-watchdog-anr.txt:277–295` khóa main trong
FutureTask.get→KnoxGuardNative→KnoxGuard30Service constructor; worker185
giữ global lock trong waitForService, các retry worker sau chờ lock. Source
timeout25.000ms ×3retry giải thích vì sao main vượt watchdog67s. KG30 provider
không hiện diện trong VENDOR/ODM VINTF/init đã kiểm và init không tìm được
`aidl/vendor.samsung.hardware.kg30.ISehKg30/default`.

**Owner/source đã xác minh.** Decoded donor services.jar có:

- `com/samsung/android/knoxguard30/service/KnoxGuard30VendorController.smali`,
  getService dòng91, waitForService102, safeCall236, null→fail-close282.
- `.../KnoxGuardNative.smali`, timeout25.000ms tại36, retry ở665.
- `.../KnoxGuard30Service`/`utils/Utils`, constructor gọi lấy trạng thái TA
  đồng bộ trên SystemServer main.

Không chỉnh cây generated `rom/out/target/.../apktool` làm source chính.
Thêm helper/lock BZI6 có scope vào module finalization hiện có; điểm hook thật
là `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`
đã DECODE services.jar quanh7536 trước ADB defaults và các compiled gate.
Helper mới là file dự kiến, chưa tồn tại; pin method/class preimage và output
trong lock. Cập nhật verifier release/image, series/source lock và hồ sơ patch.

**Cách sửa dự kiến.** Xác minh API lookup không chặn của framework hiện tại,
kiểm service/capability/declaration rồi trả ngay nhánh unavailable nếu provider
không có. Giữ `generateFailCloseResponse(0xff02)` và lỗi TA thực; không giả
trạng thái đã mở khóa/thành công. Service-present/death/reconnect vẫn hoạt động
đúng. Giới hạn retry/worker lifecycle theo ngữ nghĩa đã kiểm, không chỉ giảm
Future timeout: `cancel` không chắc dừng native wait đang giữ lock.

**Kiểm PC.** Fixture absent/present/death/reconnect/null/failure; không chặn main
quá deadline, không tồn worker/lock sau trả lỗi, không đổi success response.
Wrong donor/model/hash và class khác phải fail build; replay idempotent,
assemble/decode lại compiled DEX, kiểm opcode/call graph và các class ngoài scope.

**Trial/gate.** Trial services.jar riêng, lưu bytes/mode/UID/GID/context và xử lý
đúng cache/oat của unit. Thu entry/exit timing, worker/lock, KG structured error.
Ít nhất hai cold boot, trong đó một lượt fresh-data khi đóng package: cùng một
SystemServer từ start đến boot_completed, không WATCHDOG KILLING, không retry
worker tích lũy. Nếu KG client vẫn ANR, khóa client call và failure handling
thành unit kế tiếp. Rollback toàn services.jar0217 cùng metadata/cache phù hợp.

**SSRM/EngineeringMode tách riêng.** Stack watchdog muộn nằm trong
`system/framework/ssrm.jar`: PerformanceLogging.checkDumpSinkAndDebugLevel
gọi EngineeringMode getStatus38; initPerfettoLogging dưới lock tại phase500.
Nó không thay thế ANR gốc KG30. Chỉ trace/sửa nếu sau fixKG vẫn có độ trễ;
không ghép hai sửa chưa đo vào cùng trial, không cắt thermal/DVFS service.

## 3. RTC và thermal: ba unit nhỏ, không tạo threshold giả

### 3.1 RTC exact label

**Bằng chứng:** M4-0218
`logs/20261029-m4-kg30-nonblocking-0218/run/cache-unica/unica/logcat_live.txt:180708,180710,180711`
ghi SDHMS fstat EACCES; `readlink -f` trỏ đúng
`/devices/platform/141c0000.speedy/i2c-17/17-0000/s2mps18-rtc/rtc/rtc0/`.
date/time/since_epoch mode0444 nhưng generic sysfs. Policy đã có
system_app_33_0→sysfs_rtc_33_0 read/open/getattr. Live `chcon` trả
`Permission denied`; trap/readback giữ nguyên nhãn generic.

**File dự kiến:** existing
`rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/a376b_android17_thermal.py`,
`.lock.json`, verifier vendor ext4 hiện có, series/source lock. Vì sysfs từ
runtime không cho relabel bằng `chcon`, patch0218 thêm đúng ba genfscon vào
existing final vendor restore helper với type `sysfs_rtc`; không label toàn
tree và không thêm allow. Helper negative/idempotence fixtures, fixture CIL
chỉ có ba dòng thêm, full split-policy `secilc` compilation và packed
target-files VENDOR audit đều PASS. Build0219 package SHA
`13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`
fresh-installed trên G965F. Hai cold boots Enforcing đều xác nhận ba exact
labels, RTC đọc được/tiến thời gian và SDHMS RTC EACCES count0. EFS/CPEFS/RADIO/
RECOVERY/BOTA hashes không đổi. Bằng chứng:
`logs/20261029-m4-rtc-label-0219/attempt0219/`. Unit RTC đã đóng; không đổi
giờ RTC. Nếu regression cần rollback, build một run riêng bỏ đúng ba rule;
không ghi đè package đang giữ.

### 3.2 Sensor/policy/actuator provenance

HAL có nhiệt/status nhưng cooling/threshold lists trống. Generic DDC clamp có
status2, data null, cap1.0/appliedfalse (`display.txt:463–470`); Samsung
BrightnessLimitController chưa có active runner trong snapshot. Đây là
**chưa nghiệm thu limiter**, không phải kết luận mọi bảo vệ nhiệt bị tắt.

Read-only M4-0219 capture xác nhận HAL Thermal2.0 connected, có nhiệt độ nhưng
cooling/threshold lists empty. Kernel có sáu zones (BIG/LITTLE/G3D/ISP/AC/
battery) và bốn cdev CPU/GPU/ISP; zones0–3 enabled với bindings/trips. BZI6
SDHMS đọc `ISehThermal::sehGetTemperatures`, nhân Celsius với10 và policy XML
dùng `SKIN`. Ban đầu SKIN35.7–36.2°C thấp hơn entry `DISPLAY/Brightness`380
(38.0°C), value `-1`; `thermalControlFlag=0` khi đọc ở16:23 nên riêng nhánh
`BrightnessControlForTG` tắt tại thời điểm đó. Không suy điều này thành tắt
base SIOP hoặc kernel cooling. `sys.siop.level`/`curLevel` qua
OverheatSensorChecker riêng; không dùng để nhận diện branch.

Sau reconnect, passive log ở16:21–16:28 cho thấy SDHMS PID3960 gọi
`setMasterBrightnessLimit` với upper306/408/510/867. Gọi API xảy ra trong chu
kỳ SIOP, nhưng thay đổi upper không khớp đơn điệu với các dòng SKIN kế cận và
log không có call stack. Do đó chưa xác định API caller là `j5/a1`/policy
component hay một SDHMS brightness path khác. Cũng quan sát được
`thermal-cpufreq-1` state8/17 lúc BIG79°C, rồi state0 lúc BIG46°C; sysfs nối
device này với BIG trip2=83°C, nhưng không bắt được điểm bắt đầu/hysteresis.
Đây là bằng chứng thụ động cho API/cdev sống, không phải threshold calibration.
Đính chính sau kiểm0219: suy luận trên base framework-res để loại `k5/s1`
là sai. Effective SDHMS product RRO trả max867/table306/408/510; callback có
thể tạo chính các cap đã thấy. Giữ cả k5/s1 và j5/a1 là candidates, không có
Java stack quy caller. Caller k5/p1 truyền true chỉ suppress API, không phải
reset; bảng thực không toàn-1 nên registration gate không bị loại.13class/
194compiled methods đã kiểm với built APK khớp runtime. Các flag
BrightnessControlForTG và HighBrightnessForTG phải trace riêng; chưa có bằng
chứng để sửa bảng/policy. Xem thermal audit đã đính chính và plan0219.
Chi tiết: [thermal audit](m4-thermal-limiter-audit-0219.md) và
`../logs/20261029-m4-thermal-0219/thermal-natural-passive-observation.txt`.
Không suy `BIG` thành Samsung `AP/SKIN`; không lấy frequency maximum làm
thermal cap.

Lập bảng từng sensor thật→đơn vị/công thức→policy chọn thực→threshold/hysteresis
→scenario/component→API/sysfs actuator→readback. Đọc source/dissassembly thermal
service đã pin, SDHMS a1/m (Celsius×10), j5/a1 (master brightness setter),
selected siop_default/ssrm_default và các cooling_device/cpufreq/GPU lock thật.
Không đồng nhất Linux BIG với Samsung AP/SKIN; không gọi maxfreq thấp là thermal
cap khi chưa loại QoS/power policy.

`Brightness value=-1` đi qua code thành request bỏ cap, không chứng minh toàn
thermal bị bỏ. Bảng `BrightnessControlForTG` chỉ được sử dụng nếu scenario thật
active. File chỉ sửa sau khi khóa lỗi mapping:
`rom/unica/patches/dvfs/a376b_android17_dvfs_policy.py`, lock/customize hoặc đúng
HAL adapter; không chép bảng Linux/đoán nhiệt sang generic display XML.

### 3.3 Master limiter API và điều khiển tự động đã quan sát; calibration còn mở

API contract, Binder argument order, `CONTROL_DISPLAY_BRIGHTNESS` permission
và grant cho SDHMS đã kiểm. Thử reversible trên màn hình awake với cap32:
runner đọc `upper=32`, panel setpoint/actual đổi6224/23→3200/7. Reset
`(-1,-1,0)` đưa runner về no-cap (`target/current=867`) và panel về6224/23;
manual setting63/mode0 không đổi. Harness từng kỳ vọng255 nên phát ba reset
no-cap giống nhau; readback cuối cùng xác nhận không còn cap. Vì trạng thái gốc
không có runner, hiện còn idle no-cap runner trong memory đến khi Power service
restart, nhưng không còn giới hạn hiệu lực hay setting bền vững. Đây là
**actuator subgate PASS**, không phải thermal PASS. Xem
`../logs/20261029-m4-thermal-0219/actuator-probe-review.txt` và
`actuator-restored-readback.txt`.

Không chỉnh `thermalControlFlag` hoặc policy. Không có thiết bị đo nhiệt ngoài,
nên không ép nhiệt hay chạy workload để tạo ngưỡng. Bước sửa tiếp theo là static
trace/runtime provenance để nhận diện caller và selected component cho các cap
306/408/510/867, rồi so với policy đã khóa. Chỉ sửa nếu mapping sai được chứng
minh bằng preimage/callsite/output. Khi có cảm biến ngoài và bounded workload,
log song song cảm biến độc lập, SKIN HAL, selected scenario, PowerManager cap,
cdev states, thời điểm qua ngưỡng và hồi phục. Hiện giữ thermal subgate là
**chưa nghiệm thu**, không tự đổi threshold hay kernel. DEX-specific branch
tiếp tục được ghi unsupported.

## 4. Bluetooth: chọn backend đúng BZI6

**Bằng chứng0217:** HCI reset hoàn tất; chờ VaultKeeper làm scan controller mất
29215ms, GATT vừa xong thì BLE_START_TIMEOUT (`logcat.txt:300030–300099`).
Manager19→28crash,6PID trong180s. Không cần lặp toggle lỗi thêm để chứng minh.

**File:** `rom/unica/patches/bluetooth/customize.sh:31–42` chỉ match hai signature
cũ; BZI6 không match chỉ LOGW (build0217:1032,1746). Existing finalization
customize.sh:1333–1345 reapply sau restore. Thêm helper/lock BZI6 ngay module
bluetooth hiện có và gate compiled/image cho **SYSTEM** libbluetooth_jni.so.
Pin JNI `f8a662ca15d9526ed8271af8ecdb97bb09e9b850386d0f77abe17eb77c030757`
và vkmanager `0fdf61d254ff10c50f9f5e9027e3d2f5e0ac1fb938637c086540f85ffb1596e4`.

Một native stack trong cửa sổ wait để khóa caller→vkmanager; disassemble
selector/fallback để chứng minh storage backend hợp lệ giữ identity/key
semantics. Không đọc/ghi IRK/link keys/EFS, không thêm find-default_android_service
hoặc kéo timeout. Nếu không có fallback hợp lệ, ghi blocker cụ thể trước sửa.

Chỉ khi chứng minh xong mới patch selector đúng branch/preimage/callsite/output,
wrong hash/donor fail build, idempotence. Trial chỉ JNI+metadata; ON trước
deadline,10toggle và15phút không restart. Sau đó scan/pair/reconnect/A2DP/OPP
với peer thật; HFP riêng. Lỗi chuyển tới callback/opcode khác thì thu log mới.
Rollback exact JNI/metadata; không wipe Bluetooth storage để che lỗi.

## 5. Mạng: tìm owner trả EMSGSIZE

Current0217 ping IP/DNS đều lỗi, rawTCP trực tiếp PASS `so_error=0`/exit0.
Trace0212 đã thấy fwmark UNIX send52byte→reply int32-90 trước IP connect;
raw SCM_RIGHTS controls0215 PASS. Vì thế không sửa MTU/kernel hay thay toàn
libbase theo suy đoán.

Bước tạo bằng chứng tiếp theo: diagnostic process riêng dùng chính libbase
BZI6 `ReceiveFileDescriptorVector`,52byte+1FD, so raw/wrapper cùng socket type,
option, ancillary capacity. Pin libbase/netd/libnetd_client/maps/namespace/CRT/TLS;
không vô tình gọi libbase S22. PASSCRED chỉ control, không khẳng định fwmark dùng.

Nếu wrapper PASS, đo một receive thật của server: iov_len, control capacity/
returned len, msg_flags, FD count, return/errno; không lấy payload/credential.
Chọn file sửa sau kết quả: serializer/client nếu sender sai, provider/callsite
nếu receive contract sai. Existing
`exynos9810_boot_restore/a376b_android17_network_native.py`/lock và composition
helper chỉ sửa nếu owner thật thuộc unit đó; không nhét fix vào Tethering APEX
chỉ vì đã có builder.

Gate: ping IP/DNS/nc hếtEMSGSIZE, DNS và HTTP/HTTPS qua app thật, DHCP/reconnect,
UID/interface stats, blocked/unblocked/network selection. VPN/hotspot/mobile
data tách riêng. Rollback nguyên provider/config unit; netstats backup chỉ cần
khi schema reader/writer thực sự đổi. Resolver0211/ION0212 giữ nguyên.

## 6. AOD/FaceWidget và SystemUI provider: tách hai lỗi

**AOD:** current0217 tái hiện disconnectPluginOnBackground AbstractMethodError.
Interface62method, implementation thiếu12abstract; hai method không VersionCheck
là disconnectPluginOnBackground và setDarkAmounts(FF). BZI6 donor cũng thiếu
setDarkAmounts, vì vậy rebase donor nguyên khối chưa đủ.

**File:** `rom/unica/mods/s9_infinity_Wallpaper/source/rebuild.py` hiện overlay
app/aodservice/plugin nhưng chưa overlay app/facewidget; thêm source adapter
FaceWidget trong chính module, cập nhật source/aod khi cần, SHA256SUMS,
customize.sh và compiled contract verifier. Giữ helper0201
`exynos9810_finalization/a376b_android17_facewidget_thread.py`/lock bảo vệ
main-looper/generation/instance.

Trace callback cleanup của donor disconnect (`mKeyguardStatusViewPluginCallback.a()`),
pin version và semantics từng method; không thêm return-void hoặc catch để báo
success giả. Compiled/version-aware gate phải phân biệt abstract bắt buộc,
guarded method và caller thật. DEX roundtrip; connectA→disconnect→connectB,
stale generation, classloader reload;20lock/wake, AOD toggle,
notification/media/widget,5native+5UN1CA Infinity và15phút không restart.
Rollback AOD/SystemUI/wallpaper composition nguyên unit có hash.

**Provider:** sau AOD crash có SystemUIIndexProvider ClassNotFoundException.
Class thực nằm classes2.dex, superclass SamsungSearchProvider ở classes5.dex;
Manifest có provider. APK installed theo pm path phải khớp compiled hash
`04963cd4f3e4785c7e70eba5fb7c41f893210a3c864da5332b1e278a105d1855`.
Đã thu installed package/splits/uses-library/dexopt; hiện chỉ base,
status run-from-apk. Sau sửa AOD, tái hiện provider restart/Finder/Settings
search rồi kiểm ClassLoader/load cause trong process thật. Chỉ patch dependency/
loader đã xác định; không thêm placeholder hoặc xóa manifest provider.

## 7. Binder measurement rồi NFC functional

NFC startup/RF discovery đã có bằng chứng; dump-pipe FAILED_TRANSACTION không
phải RF failure. FD transfer từ caller su là đầu mối của công cụ; UID2000 không
tự đổi SELinux domain. Không sửa adbd/no-auth/Enforcing để làm dump trông PASS.

Decode installed `INfcAdapter.Stub` lấy đúng getState transaction/Parcel shape;
query không truyền FD hoặc diagnostic typed client có quyền/domain phù hợp.
Không đoán transaction number. So trạng thái với NFA callbacks. Nếu ON và
service stable, dùng tag thật nhiều lượt, NFC toggle, screen off/on và reboot;
HCE/UICC/eSE/payment gate riêng. Không thay JNI/HAL/blob khi chưa có lỗi RF.
File NFC nếu cần sửa phải thuộc module
`rom/unica/patches/exynos9810_device_stack/nfc/`/helper/lock hiện có.

## 8. Bixby và lỗi GSA lịch sử

Bixby current411504000 crash ComponentResult trong LockedBootCompletedReceiver.
Pin APK/cert/version/manifest/registry; decode receiver+d1.c.g để tìm entry thiếu.
Kiểm `system/system/priv-app/Bixby/Bixby.apk`, sysconfig/bixbyagent.xml,
permission/preferred-app composition; source sửa ở owner restore/config đã xác
định qua module finalization, không sửa generated work tree. Không bật lại
Bixby remap mà người dùng đã chọn bỏ. Hai cold boot/onboarding/key/WINK không
receiver crash; rollback whole APK/config/metadata unit.

GSA GC crash0215 chưa tái hiện0217. Khóa APK/splits/cert/version và ART/boot
image lúc tái hiện trước sửa. Không thay ART/kernel hoặc tắt heap verification
chỉ vì stack phát hiện corruption; không chuyển lỗi lịch sử thành lỗi mới.

## 9. Đóng package và nghiệm thu

Mỗi unit đã qua trial phải đưa vào fork hiện có, source patch/lock đầy đủ lý
do/file/verification/rollback. Clean replay, negative wrong-source fixtures,
compiled ADB defaults/no-auth/stored-choice, real ext4 metadata/SELinux,
native ABI/hash closure và signed ZIP/ZIP32 stage gates bắt buộc PASS.

Package mới dùng run riêng, không ghi đè0217 hoặc rollback đang dùng; chỉ thay
bản giữ lại theo release-retention sau hoàn chỉnh/hash/manifest. Fresh flash
trên đúng G965F, test một lần cold/fresh boot không watchdog,15phút core stable,
BT/network functional, UI/AOD/search, RTC/thermal và ADB controls. Thu log trước
rollback nếu fail. G965N là lượt kiểm chứng blob/RIL/phần cứng riêng sau F,
không suy từ tên donor hoặc một boot thành công.

Camera/video/audio/mic/call/GNSS/fingerprint/USB/NFC tag/peer-Bluetooth test
matrix chỉ đánh PASS bằng hành vi thực, không dựa vào service registration.
Workload dài đợi thermal gate; test cần peer/tag/call được ghi rõ là cần thiết
bị đối tác. Không phát hành image Samsung lên public repo; chỉ tài liệu đã
lọc, source và patch theo quy tắc dự án.
