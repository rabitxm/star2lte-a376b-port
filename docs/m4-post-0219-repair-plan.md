# M4 sau0219 — phương án sửa lỗi chi tiết

Ngày29/10/2026. Dựa trên [kiểm thử0219](m4-validation-after-0219.md),
[fresh flash/runtime0222](m4-validation-after-0222.md) và live follow-up;
đây là kế hoạch triển khai tiếp theo. M4 chưa nghiệm thu; G965N chưa được test
theo kết quả F. BZI6 sender patch0220 đã được tạo trong cây fork và qua fixture;
đang hoàn tất replay/hash lock trước clean build, chưa có phone validation.

## 1. Nền cố định và thứ tự

Giữ M4-0219/G965F/BZI6/SDK37, Hybrid Vendor VNDK33/blob F và radio bridge,
profile `rom/target/star2lte/profiles/m4-g965f-a376b-android17-runtime-init-debug.sh`,
gói kernel hiện hành `4cea63f84d5add494bfcc2f9fff105848806a930a24aca975dc261bf6720e20a`.
Không đổi modem/bootloader/EFS/CPEFS. Giữ ADB/Auto Blocker rule và tác dụng
thật của lựa chọn người dùng, Enforcing, brightness0216 và RTC0218 đã PASS.
Không có log mới đòi sửa kernel. Không viết build system khác.

| Thứ tự | Unit | Điều kiện chọn patch/gate |
| --- | --- | --- |
|1|Bluetooth BZI6 sender repair, sau đó backend+format contract|Live manager ghi116 crash; binary disassembly khóa optional enqueue tại0x6979e8; patch0220 bỏ duy nhất BL bằng NOP và fixture hash/scope/idempotence PASS; build/flash/runtime còn pending. VaultKeeper format tách riêng|
|2|Mạng fwmark/libbase receive|Raw TCP PASS, normal connect FAIL; đo đúng receive owner trước patch|
|3|FaceWidget/AOD contract|Có exact missing-method failure và compiled mismatch|
|4|Bixby registry|Khóa component/key thiếu, giữ original signature|
|5|GNSS callback lifetime|Tách lỗi firstboot null dereference khỏi GPS chưa test|
|6|Provider loader/NFC RF/physical matrix|Chỉ sửa nếu tái hiện lỗi tương ứng|
|Song song PC|Thermal overlay/caller provenance|Đã sửa audit; không thay threshold từ tương quan|
|Cuối|Package tích hợp|Mỗi unit PASS rồi replay/compiled/image/fresh/runtime gates|

Mỗi trial trên máy chỉ đổi một unit, xác minh serial/model/preimage/hash/
UID/GID/mode/SELinux, lưu rollback trước ghi. Quyền flash hiện hành đã có;
không cần xin lại chỉ vì đổi hash. Quyền đó không thay điều kiện kỹ thuật để
chọn patch an toàn. Giữ log failure trước rollback và chỉ xóa package cũ theo
release-retention sau khi bản mới hoàn chỉnh/manifest/hash đã kiểm.

## 2. Bluetooth: backend selector kèm định dạng đọc/ghi

**Bước B0 mới theo run0222 — phân biệt HCI command path trước storage patch.**
Run0222 ghi `Timed out waiting for Unknown OpCode(0xfdf1)` ở
`logs/20261029-m4-g965f-flash-0222/flash-01/runtime/logcat-afterboot.txt:96022`;
trước đó cùng logger ghi nhiều event prefix `0e0401f1fd` ở dòng96017–96020.
Manager ghi21 lần crash trong stability snapshot, rồi
`ServiceConnection.onServiceDisconnected(...)` tại dòng60466. Cùng lượt có
`ISehVaultKeeper/default didn't start` và `vaultkeeper read error(-1)` ở
dòng218693–218694. Chưa biết lệnh `0xfdf1` do stack donor, vendor HAL/transport
hay controller firmware phát; cũng chưa chứng minh VaultKeeper là nguyên nhân
trực tiếp của HCI timeout. Không diễn giải prefix/length log thành malformed
packet nếu chưa kiểm full event và thứ tự TX/RX.

1. Dùng HCI snoop/runtime trace có sẵn sau khi Setup Wizard hoàn tất để ghi
   metadata lệnh opcode, chiều TX/RX, timestamp, độ dài và completion/status;
   tránh lưu dữ liệu ghép đôi/key. Đọc controller firmware version và manifest/
   hash thực tế của Bluetooth HAL/vendor files.
2. Từ đúng BZI6 runtime native stack, pin callsite gửi `0xfdf1` và expected
   command-complete semantics; đối chiếu vendor transport/controller support.
   So sánh với known-good M1 trên cùng biến thể phần cứng nếu log cùng giai đoạn
   có sẵn. Không coi cùng opcode bytes trong event dump là đủ xác nhận ACK.
3. Static review đã định vị lời gọi sender trong
   `BtifAvStateMachine::StateOpened::OnEnter()` (VA `0x6979e8`), sau nhánh
   capability BZI6. Build0222 xác nhận các signature Android16 không match
   (`build.txt:886–888`), và live G965F tái hiện timeout. Patch0220 khóa donor
   APEX/JNI, Build ID, context và output hash rồi thay duy nhất BL enqueue bằng
   NOP; không giả response controller hay sửa transport. Fixture negative
   scope/preimage, idempotence và untouched bytes PASS. Kế tiếp là replay đủ210
   patch, build mới/audit image, rồi thử Bluetooth runtime trên G965F.
   Không coi timeout biến mất là đủ để tuyên bố pair/audio hoạt động.
4. Tiếp tục nhánh VaultKeeper riêng sau lượt test này: nếu native backend vẫn
   chạm service AIDL, lấy stack/format evidence và kiểm reader/writer fixture;
   không reset bond, tạo identity mới hoặc làm lẫn thay đổi storage với sender.

## 2A. Wireless debugging mặc định

Fresh0222 đọc được `adb_wifi_enabled=1` và `persist.adb.tls_server.enable=1`;
`service.adb.tcp.port` rỗng, `ss -ltn` chỉ thấy `.sec.imsservice` trên cổng6200,
`ss -ltn6` không có listener và `adb mdns services` rỗng. USB ADB vẫn hoạt động
với `ro.adb.secure=0`, `adbd=running`, USB config `sec_charging,adb`, không có
`/data/misc/adb/adb_keys`. Do đó USB keyless PASS trên host nối, còn cờ TLS/
wireless có vẻ bật nhưng chưa thấy endpoint quảng bá/listen.

Đọc luồng settings/property trong fork, kể cả
`rom/unica/patches/exynos9810_device_stack/customize.sh:3240–3243`, nơi đang
đặt `persist.adb.tls_server.enable=1`. Xác nhận semantics công tắc UI và adbd
trên boot hoàn tất; candidate kế phải không mở TCP ADB mặc định mà vẫn để USB
debugging keyless và công tắc USB debugging hoạt động. Chưa đổi live device.

**Bằng chứng mới.** Native stack16:57:20 khóa main:
`AdapterService.startScanController→enableNative→stack_enable→btif_dm_load_ble_local_keys`
`→btif_storage_get_ble_local_key+468→VaultKeeperManager::read+60→AServiceManager_getService`.
AIDL service check `not found`; log cùng PID chờ29278ms rồi BLE_START_TIMEOUT.
JNI runtime hash `f8a662ca15d9526ed8271af8ecdb97bb09e9b850386d0f77abe17eb77c030757`.
Audit đầy đủ ở `logs/20261029-m4-validation-0219/run-1655/bluetooth-backend-audit.txt`.

**Owner/file.** Existing `rom/unica/patches/bluetooth/customize.sh:31–42`
chỉ match hai signature cũ và warn khi không match. Finalization
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh:1333–1345`
reapply sau restore. Bổ sung adapter+lock BZI6 trong chính module Bluetooth,
kiểm compiled final JNI và packed image qua gates hiện có, kể cả
`rom/scripts/internal/verify_m4_runtime_image.py` nếu unit nằm trong phạm vi gate.
Không chỉnh generated work tree làm source chính.

**B1 — static và định dạng, chưa ghi máy.**

- Function VA0x7aab6c; selector load/branch tại0x7aac04/0x7aac08,
  unique bytes `8816523948050037` có1match; hai old pattern0match.
  Branch đi0x7aacb0. Legacy unconditional branch chỉ là candidate, chưa được
  dùng làm fix hoàn chỉnh.
- VK read tại0x7aad40 chờ provider. Nhánh encrypted đọc32byte rồi AES128-CBC;
  fallback0x7ab864 đọc config16byte; writer add_ble_local_key có hai format
  đối xứng. Hai global flags còn điều khiển BR/EDR và BLE bonding readers/writers.
- Shim GetBin0x5bb424–0x5bb434 copy min(capacity,stored_size) và vẫn success.
  Vì vậy native16 fallback với stored32 có thể trả ciphertext-prefix như key.
  Không NOP read/ép globalflag/tăng timeout/thêm broad allow hoặc xóa key store.
- Chỉ thu metadata entry presence/length/format cần thiết, không xuất key.
  Length32 chưa tự chứng minh ciphertext nếu chưa khóa provenance. Nếu format
  không tương thích hoặc không thể decrypt bằng provider hiện có, giữ nguyên
  bytes và ghi blocker migration; không tự tạo identity mới để báo ON.

**B2 — fixture và thiết kế patch.** Dùng key tổng hợp, không dùng key máy:
absent/valid16/encrypted32/malformed/truncated, VK present/unavailable, local
và bonded read/write, restart/persistence. Cả reader/writer phải chọn cùng
format và giữ identity; unavailable không được thành success giả hoặc silent
truncate. Nếu chọn backend config native, scope đầy đủ sharedflag callers và
migration/failure semantics. Adapter fail build khi donor/hash/preimage/target
sai, idempotent, output hash riêng; kiểm instructions/callgraph sau patch.
Không sửa libbinder/libvkmanager dùng chung để bỏ wait toàn hệ thống.

**B3 — trial và rollback.** JNI/metadata riêng trên đúng G965F, retained config,
không wipe. ON trước deadline, không còn wait29s,10toggle,15phút passive không
tăng restart. Theo dõi identity/format metadata qua service restart/cold boot;
peer thật để scan/pair/reconnect/A2DP/OPP, HFP gate riêng. Media/load dài đợi
thermal acceptance phù hợp. Rollback exact JNI+metadata. Nếu schema storage
thực sự thay thì phải có transactional snapshot/rollback cả unit trước thử;
không để binary cũ đọc format mới. Chưa có fixture/migration PASS thì chưa flash.

## 3. Mạng: xác minh receive boundary trước chọn owner

**Bằng chứng.** Ping IP/tên miền rc2 `connect: Message too long` tái hiện0219;
raw syscall TCP mới uid0→1.1.1.1:80 `so_error=0`,exit0. Trace0212 có fwmark
AF_UNIX send52byte+1FD, recv int32-90 trước IP connect; raw SCM_RIGHTS controls0215
PASS. Các phép đo đó không chứng minh receive wrapper BZI6 đang đúng. Chrome
FirstRun trong lượt này chưa thực hiện bài web.

**N1 — process diagnostic độc lập.** Dùng toolchain/helper kiểm hiện có của
fork, nạp đúng BZI6 `libbase.so` runtime hash
`6c328ad4107810fa7a5e3f58e06c3e9142af1aabe159c6c51ec08b83296aacc2`.
Gọi ReceiveFileDescriptorVector với52byte/1FD, raw và wrapper cùng socket type/
options/capacity; negative expected-truncation controls. Pin maps/namespace/
CRT/TLS/BuildID để không gọi libbase S22. PASSCRED chỉ là control, không giả
định fwmark bật nó. Không đụng server production khi chưa biết wrapper result.

**N2 — receive thật nếu wrapper PASS.** Thu iov_len, control capacity/returned
length, msg_flags, FD count, return/errno ở chính callsite netd/fwmark;
không lấy payload/credential. Runtime netd9ac97406..., libnetd_clientf79ce6d9...
đã pin trong final-state-native-provenance. Chọn sửa sender/serializer,
netd receiver hoặc provider theo kết quả. Không đổi MTU/kernel, thay toàn
libbase, hoặc nhét fix vào Tethering APEX chỉ vì có builder sẵn.

**File dự kiến.** Owner chưa khóa nên chưa chốt binary/source patch. Existing
`exynos9810_boot_restore/a376b_android17_network_native.py/.lock.json` là
composition đã có; chỉ mở rộng nếu lỗi thực sự thuộc unit đó. Mọi file patch
mới phải gắn vào fork/gate hiện có và có preimage/output lock.

**Gate/rollback.** Ping IP/name, dynamic TCP, DNS không chỉ cache, HTTP và HTTPS
qua app bình thường; UID/network selection/block-unblock, DHCP/reconnect và
interface/UID stats. VPN/hotspot/mobiledata riêng. Giữ resolver0211 và netstats
ABI đã sửa; rollback whole provider/config+metadata đúng unit. Chỉ snapshot
netstats khi schema reader/writer thực sự thay, không reset dữ liệu để che lỗi.

## 4. FaceWidget/AOD: sửa contract thật

**Bằng chứng.** Current boot0219 crash buffer16:21:24:
`AbstractMethodError ... disconnectPluginOnBackground()`. Live180s và5vòng
sleep/wake không thêm crash, nhưng không xóa failure startup đã đo. Runtime
SystemUI APK04963cd4... khớp compiled audit. Interface62abstract; implementation
S9 Infinity thiếu12, hai callback không được VersionCheck bảo vệ là
`disconnectPluginOnBackground()` và `setDarkAmounts(FF)`. BZI6 donor cũng thiếu
setDarkAmounts, nên copy nguyên donor chưa đủ.

**File.** Existing `rom/unica/mods/s9_infinity_Wallpaper/source/rebuild.py`,
`customize.sh`, `SHA256SUMS`; thêm adapter facewidget trong source module này
(không coi đường dẫn mới là đã tồn tại). Giữ finalization
`a376b_android17_facewidget_thread.py/.lock.json` của0201.

**Sửa dự kiến.** Trace donor disconnect gọi callback.a() và cleanup thực tế;
port đúng lifecycle/main-looper/generation, không no-op/catch-only. Đọc caller
FaceWidgetContainerWrapper và receiver dark amount để khóa nghĩa hai float;
không gộp tùy tiện sang API một float. Phân biệt62interface methods theo
version annotation/caller; chỉ thêm implementation có semantics đã chứng minh.

**Gate/rollback.** Version-aware compiled contract, DEX roundtrip/hash và
wrong-source negative; connectA→disconnect→connectB, stale generation/reload,
20lock/wake, AOD toggle, widget/notification/media,5native+5Infinity wallpaper,
passive15phút không SystemUI restart. Restore nguyên AOD/SystemUI/wallpaper
composition+metadata nếu fail, không rebase riêng một class rồi bỏ lifecycle.

## 5. Bixby, provider và GNSS

**Bixby.** Current boot16:21:40 `LockedBootCompletedReceiver`→d1.c.g→
`No matching ComponentResult found!`, version411504000. Finalization
customize.sh3427–3440 giữ original Samsung signature. Decode đúng APK/cert/
receiver để tìm requested component/key thiếu, so registry/feature/sysconfig
composition với donor; `system/etc/sysconfig/bixbyagent.xml` là candidate phải
chứng minh vai trò. Ưu tiên sửa composition nếu đó là owner; không sửa APK làm
mất cert hoặc trả registry giả. Giữ WINK/keylayout gốc, không bật remap đã bỏ.
Gate hai cold boots/onboarding/key activation, không receiver fatal; rollback
whole signed APK/config/metadata unit.

**SystemUIIndexProvider.** Class/superclass từng có trong compiled DEX, lỗi0217
chưa tái hiện trong0219. Sau fixAOD, test Finder/Settings search/provider reload,
kiểm nested load cause/uses-library/splits/dexopt trong process thật. Chỉ sửa
loader/dependency khi tái hiện; không xóa manifest provider để biến log thành PASS.

**GNSS.** Firstboot0219 SIGSEGV/null+4 có stack libutils atomic→RefBase::incStrong
→BpHwAGnssRilCallback::requestSetIdCb→AGnssRil::requestSetId→gps.default.so.
Đây là null/lifetime path; không có bằng chứng ISA/kernel fault. Pin service/
libraries/BuildIDs so M1/M4, trace callback installation/cleanup và request
sớm. Dòng Samsung extension mGpsLocationxIface0x0 chưa chứng minh cùng object.
Nếu thấy callback-before-registration race thì sửa order/lifecycle đúng owner,
không drop request hoặc stub success. Current dump GPSOFF/0fix không phải một
bài outdoor acquisition. Gate fresh/cold boot, GNSS start/stop, outdoor TTFF/
satellite fixes; rollback full GNSS dependency unit. Chưa đổi blob/modem/kernel.

## 6. NFC, thermal và nghiệm thu phần cứng

**NFC.** Runtime framework-nfc.jar hashacd3f0f1... khớp stub đã decode:
getState transaction6, không args/FD, parcel NoException+3 xác nhận ON.
Dumpsys FAILED_TRANSACTION là measurement path riêng, không phải RF fail.
Test tag thật đọc nhiều lượt, off/on, screen state và reconnect trước patch;
HCE/UICC/eSE/payment riêng. Nếu có lỗi thật mới sửa existing
`rom/unica/patches/exynos9810_device_stack/nfc/` helper/lock tương ứng.

**Thermal.** Đã sửa kết luận sai từ base framework-res. Effective SDHMS RRO
max867 và table306/408/510 hỗ trợ chính các cap quan sát;13class/194compiled
methods đã đối chiếu. k5/s1 và j5/a1 vẫn là ứng viên, chưa có direct caller trace.
Bước tối thiểu tiếp theo: timestamped row/column/3sample/gates và API callsite,
chỉ metadata; nếu cần instrumentation thì một diagnostic unit riêng có hash/
rollback, không đổi threshold. `BrightnessControlForTG` và event
`HighBrightnessForTG` là hai thứ cần trace riêng. Column>=6 fallback867 là
hành vi static; cần hiểu tương tác base SIOP/HBM trước gọi là bug. Không có
nhiệt kế ngoài, nên không gây nhiệt hay chạy workload để ép ngưỡng. Calibrated
trigger/hysteresis/cooldown và DeX-specific thermal vẫn chưa nghiệm thu.

**Play Integrity/PIF.** Giữ ưu tiên PIF đã chọn và adaptation hiện hữu. Sau
khi normal network/Google services hoạt động, đo và ghi verdict trực tiếp với
APK/version/môi trường tương ứng; compiled gate không thay runtime verdict.
Lượt này chưa đo, không coi là PASS hoặc phát sinh lỗi PIF mới.

**Physical matrix.** Camera preview/photo/video, mic/loa/earpiece/call, SIM/RIL,
fingerprint/sensors/USB audio, Bluetooth peer, NFC tag, GNSS outdoor cần hành vi
thực. Giữ xác nhận trước của người dùng rằng phần cứng cơ bản bình thường;
các bài cụ thể chưa chạy được ghi riêng cùng bằng chứng service/inventory.
Không gọi số hoặc nhắn ai trong kiểm thử nếu không được chỉ định.

## 7. Build, package và tiêu chí đóng mốc

Mỗi patch có lý do/files/preimage/output/verification/rollback, scope donor/
model đúng, không silent skip. Clean source replay, negative/idempotence,
compiled ABI/API/ADB rule, SELinux/metadata trên ext4 thật, signed fullZIP và
8stage ZIP32 gates đều phải qua. Không ghi đè package đang giữ; retention chỉ
đổi sau candidate hoàn chỉnh/hash/manifest. Fresh trial G965F sau các unit,
startup không watchdog/Java/native fatal không xử lý; core ổn, BT+normalnetwork
và physical matrix theo gate. G965N có lượt blob/RIL/hardware riêng sau F.

Lý do tài liệu: thay plan bằng native BT owner/format risk, fresh network
control, NFC typed query và thermal correction mới. File ảnh hưởng: tài liệu
này, validation0219 và các status/audit docs; không có ROM source patch/build/
flash mới trong lượt lập plan. Kiểm chứng bằng evidence run-1655, disassembly
và compiled-method report; trạng thái proposed/untested được ghi riêng.
