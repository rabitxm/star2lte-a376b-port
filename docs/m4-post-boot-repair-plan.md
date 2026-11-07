# Kế hoạch sửa M4 sau source0198 — 28/10/2026

**Kế hoạch này được cập nhật trạng thái/thứ tự bởi [plan sau0212](m4-post-0212-validation-plan.md).** Giữ nội dung dưới làm lịch sử phân tích; trạng thái “chưa flash”/nền0198 không còn là checkpoint hiện hành.


Trạng thái cập nhật 28/10/2026: **đang triển khai**. Patch0199 đã nối composition mạng/meminfo đã đo vào profile chẩn đoán; patch0200 lọc đúng node DEX đặc biệt theo chấp thuận của người dùng; patch0201 chuyển khởi tạo FaceWidget qua main-looper handler có generation/instance guard. Ba patch qua fixture/replay; APK SystemUI fixture assemble/decode PASS. Chưa build ROM package hoặc ghi điện thoại; runtime thermal/SystemUI/NFC, regression và nghiệm thu còn pending.
Đây là phần tiếp theo của docs/PLAN.md, giữ nguyên các quyết định AGENTS.md.

## 1. Đích cần đạt và nền đã chứng minh

Đích gần nhất là M4 G965F vào được Setup Wizard/Home, SystemUI và các dịch vụ
phần cứng sống ổn định, sau đó nghiệm thu fresh-data/reboot. Chưa chuyển sang
G965N hoặc đánh dấu hoàn tất port chỉ từ boot_completed.

Lượt0198 đã đo SDK37/OneUI90000, boot_completed1, bootanim stopped, Enforcing;
SystemServer PID1830 còn xuất hiện ở uptime337.22s. Fixture network197 và
Debug JNI/MemInfoReader198 đều PASS. GPU memory API trả-1 khi map không có,
không cung cấp số liệu giả. Các cửa sổ pidof có thêm tên tiến trình trùng được
giữ nguyên, không suy ra server restart chỉ từ pidof.

Nhưng SystemUI, SDHMS và NFC vẫn crash. Đã thu189 file và rollback đầy đủ;
trạng thái xác minh cuối là G965F ở TWRP. ROM hoàn chỉnh đang giữ vẫn là
attempt37/source0188, **chưa chứa toàn bộ composition chẩn đoán0195–0198**.

Nền cho các trial tiếp theo phải giữ đúng những thành phần đã đo:

| Thành phần | Nền khóa cho trial thành phần |
| --- | --- |
| Donor / máy | BZI6 Android17, star2lte G965F đã đối chiếu serial/bootloader |
| Vendor | Hybrid Vendor VNDK33; F đúng nguồn F; mount/ueventd0189–0190 |
| Kernel/BOOT | Giữ nguyên BOOT chẩn đoán WIPEONFORK/probe13 đã đo, SHA b01912f6…; không build kernel mới |
| Graphics | V6 bổ sung cùng V7; libui0194 SHA ef711d73… |
| Tethering | Composition0197 SHA f17fa222…; native BZI6 thật + L4S + stats/transport |
| Resolver | S22 resolver đã đo, SHA f1699315… |
| Meminfo |0198 SHA b19a1c37…; nhánh fd âm trở về error path sẵn có |
| Chẩn đoán | Logger CACHE hiện có; phân biệt lần boot/generation mới với slot cũ |

Các SHA đầy đủ và metadata nằm trong docs/m4-meminfo-runtime.md và private
logs/20261028-m4-meminfo-test. Không lấy tên file/BuildID/chuỗi phiên bản kernel
thay cho kiểm hash. Không đổi radio bridge, modem, bootloader hoặc ghi EFS/CPEFS.

## 2. Thứ tự thực hiện

Phân tích SystemUI, thermal và NFC trên PC có thể song song. Thử trên máy
tuần tự, mỗi trial chỉ thay một nhóm thành phần:

| Bước | Công việc / đầu ra | Điều kiện qua bước |
| --- | --- | --- |
| 0 | Khóa manifest nền0198, APK/library thật và rollback | Không nhầm cache/work tree với image đang thử |
| 1 | Nối composition đã đo vào profile chẩn đoán của fork | Output/metadata đúng, gate source/compiled/image không bị nới |
| 2 | Sửa policy SDHMS cho parser BZI6 | Parser hợp lệ, sensor/actuator thật, idle/light-load sống |
| 3 | Sửa SystemUI ↔ FaceWidget/AOD | Đúng ordering/thread/teardown, giữ Infinity, UI ổn15phút |
| 4 | Khép JNI/provider NFC | Đủ dependency/symbol/namespace/JNI; HAL/tag hoạt động |
| 5 | Kiểm lại Bixby, network, graphics và lỗi mới | Không che lỗi bằng service sống/boot flag |
| 6 | Tách probe/logger khỏi ramdisk bằng một trial riêng | Boot không phụ thuộc wrapper/marker chẩn đoán |
| 7 | Build sạch, image audit, package +8ZIP32 | Đúng composition, kernel provenance, ADB/PIF/SELinux đầy đủ |
| 8 | Fresh-flash và nghiệm thu G965F | Fresh defaults, reboot, UI/hardware/network đạt; N còn gate riêng |

Thermal được thử trên máy trước phiên UI dài vì SDHMS hiện đang restart.
Nếu bước thermal chưa qua sensor/control tối thiểu, chưa chạy workload dài;
phân tích và kiểm DEX/SystemUI trên PC vẫn tiếp tục được.

## 3. Bước0 — khóa đầu vào, loại bẫy cache cũ

1. Trước thao tác ghi, xác minh lại serial/model/bootloader, block path/size,
   BOOT/VENDOR và rollback; không coi trạng thái TWRP trong báo cáo cũ là live.
2. Lấy APK/libs từ image attempt37 đã flash hoặc bản đọc-only có hash, rồi
   ghi riêng donor APK, compiled installed APK và output dự kiến. Manifest
   gồm SHA/bytes/cert/DEX list/BuildID nếu có, UID/GID/mode/SELinux.
3. Decode isolated theo hash, donor, phiên bản apktool, profile. Không dùng
   thư mục decode chung làm nguồn sự thật. Hai bẫy đã tìm thấy:
   - Cache SDHMS hiện có source qb/113411477, còn crash BZI6 là qb/115099834.
     Enum DEX trong cache đó không chứng minh BZI6 hỗ trợ DEX.
   - JNI NFC work_dir hiện có hash cd02468b… khác JNI PHN ed74c2fc… mà recipe
     đóng và log đang lỗi. Phải pin lại file thật trước audit ABI.
4. Snapshot chính xác dữ liệu có thay đổi schema. Networking0197 dùng writer
   v8: giữ hai snapshot `/data/system/netstats` và
   `/data/misc/apexdata/com.android.tethering/netstats`, kể cả metadata, khi
   quay về reader cũ. Cache/data của APK khác chỉ xử lý sau khi đọc code tìm
   đúng đường dẫn và backup; không wipe toàn data để né lỗi.

Đầu ra: manifest nền bất biến, bảng file/source/hash, source marker của từng
APK và danh sách chính xác các file/data được rollback trong từng trial.

## 4. Bước1 — đưa nền đã đo vào fork mà không làm mất thay đổi cuối build

Hiện profile `m4-g965f-a376b-android17-runtime-init-debug.sh` chọn kernel/logger,
nhưng runtime lock thông thường vẫn giữ Tethering BZI6 gốc. Không thể chỉ build
lại profile hiện tại rồi mặc định rằng0195–0198 đã được chọn.

Mở rộng các điểm có sẵn:

- `rom/target/star2lte/profiles/m4-g965f-a376b-android17-runtime-init-debug.sh`:
  thêm lựa chọn composition chẩn đoán rõ ràng, giới hạn M4/BZI6/G965F.
- Trong `exynos9810_boot_restore/`: nối `a376b_android17_network_native.py`
  với `--stats-compat`, resolver đã khóa và helper meminfo0198 vào apply/verify.
  Dùng tool ext4/AVB/signapk/native32 hiện có; không tạo build system khác.
- `a376b_android17_runtime_compat.py` và lock: tách hash donor đầu vào khỏi
  hash output đã biến đổi; profile khác tiếp tục giữ hợp đồng cũ.
- `customize.sh` và `rom/scripts/internal/verify_m4_runtime_image.py`: kiểm
  sau restore/finalization và trong ext4 thật, gồm APEX pair, libmeminfo,
  libui/V6/V7, mount/ueventd/logger và metadata. Không để restore xóa sửa đổi.
- APEX dùng signing identity chẩn đoán hiện có; kiểm AVB/container signature,
  payload alignment, classpath/manifest và metadata. Không gọi đó là chữ ký
  Samsung gốc; key không vào Git. Ghi rõ container thực tế thay vì suy từ
  suffix `_compressed.apex`.

Gate: hash output đúng các artifact đã đo; round-trip DEX/API và dependency
checks hiện có PASS; sai donor/model/hash/thiếu một output phải FAIL. Các
patch cũ không sửa lại lịch sử; patch mới có lý do/file/kiểm/rollback và replay.
Nền lần đầu được dựng từ fork phải tái lập được hành vi0198 trước khi dùng
nó để quy lỗi cho APK mới. Không cần package toàn ROM cho mỗi trial thành phần.

Giữ original production profile/kernel selection hiện hành; gói mới giai
đoạn này là candidate chẩn đoán với provenance rõ. Không âm thầm gọi kernel
WIPEONFORK chẩn đoán là kernel production đã nghiệm thu.

## 5. Bước2 — SDHMS: sửa schema có đầy đủ ý nghĩa thermal

Bằng chứng: `sdhms-first-crash.txt:2`:
`No enum constant com.sec.android.sdhms.thermal.overheatcontrol.overheatcomplex.OverheatComplexType.DEX`.
Fork `dvfs/assets/siop_default.xml:60` chứa `<DEX formula="SKIN" temp="470" />`.
Recipe `dvfs/customize.sh:52–63` thay các asset donor bằng cùng XML cho cả
siop_default và ssrm_default, mặc dù fork có một ssrm_default.xml khác.
Chưa kết luận copy trùng là bug trước khi đọc precedence của loader.

**Audit bắt buộc trước patch:**

1. Decode BZI6 SDHMS thực (donor SHA53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9)
   và APK compiled. Đọc enum `<clinit>`, SiopExpression.M, loader, decrypt,
   policy selector và merge/override. Asset donor đang là binary/opaque,
   không đoán đó là XML hoặc đoán thuật toán giải mã.
2. Lập bảng mọi enum/formula/temperature/scenario/limiter mà parser nhận;
   đối chiếu toàn XML, không chỉ mục DEX. Kiểm cả scenario Dex riêng, SKIN,
   CPU/GPU, charging, brightness, multiwindow và hysteresis.
3. Đối chiếu S9/M1 hoạt động: tìm thermal zones bằng type/name thực tế,
   đơn vị/scaling, sensor có/không, formula SKIN, tần số hỗ trợ và actuator
   CPU/GPU/charging/brightness. Không bê sensor path/tần số A37 sang S9.

**Hướng ưu tiên:** chuyển policy S9 sang schema BZI6, giữ parser/thuật toán
donor và các giới hạn phần cứng target. Asset/adapter mới nằm trong module
DVFS hiện có, scope riêng M4. Giữ XML M1/M2 không đổi nếu không cần.

Nếu control DEX có replacement thực, map kèm formula/threshold/điều kiện.
Nếu phải giữ DeX nhưng donor bỏ implementation, đánh giá port đủ enum,
factory/switch, event và limiter từ nguồn đã chạy. Thêm một chuỗi DEX vào
enum, xóa dòng XML hoặc catch-ignore valueOf không phải bản sửa đạt gate.
Nếu cần giảm tính năng DeX để đi tiếp, ghi rõ phần mất và chốt với người dùng;
không tự coi thermal đã đạt.

**Kiểm:** chạy parser/semantic fixture theo implementation thật; reject
unknown enum/formula, sensor không resolve, thiếu attribute, sai cardinality,
units/threshold/hysteresis. Assemble/redecode APK và kiểm asset/selector trong
APK cuối, certificate/permissions/ZIP theo fork. Runtime bắt đầu idle và
thao tác nhẹ: SDHMS không restart, policy đã chọn đúng, sensor có sample mới
với timestamp/liveness, limiter được đăng ký và action/readback actuator thật,
không bị permission/AVC từ chối. Giá trị nhiệt hợp lý nhưng controller bị
silently skip vẫn FAIL. Trước camera/video/charging, chốt điều kiện dừng theo
policy/giới hạn S9 đã xác minh và theo dõi sensor độc lập SDHMS; dừng khi sensor
stale/mất hoặc actuator không đáp ứng, không đặt một ngưỡng°C tùy ý.
Sau đó mới idle→camera/video vừa phải→cooldown,
charging/unplug; ghi throttle và hồi phục thực. Chưa chạm một ngưỡng thì ghi
ngưỡng đó chưa nghiệm thu, không suy từ “không crash” thành thermal hoàn chỉnh.

File dự kiến: `rom/unica/patches/dvfs/customize.sh`, asset/adapter M4 trong cùng
module; Smali chỉ thêm nếu schema không đủ và đã có preimage đúng BZI6.

## 6. Bước3 — SystemUI / FaceWidget / AOD

Bằng chứng log: `systemui-first-crash.txt:15` có
`Method setCurrentState must be called on the main thread`.
`current-main-private.txt:17942–17943` ghi plugin4006 / SystemUI4022, rồi chạy
`[BG] Init Plugin Wrapper started`. Lệch version là đầu mối audit hợp đồng,
chưa tự chứng minh cần thay toàn APK.

Caller BZI6 đã đọc: PluginFaceWidgetManager tạo mUiHandler(main) và
HandlerThread PluginFaceWidget-BG riêng. `$6` gọi setPluginFaceWidgetCallback
trên BG, dùng ngay getter position/notification/clock, rồi mới post attach main.
Nó còn có disconnect/cancel và kiểm connected instance. **Chỉ post riêng
callback async rồi cho BG chạy tiếp có thể làm getter chạy trước DI/lifecycle.**

**Audit:** hash APK SystemUI/AOD compiled, cert/DEX/interface/plugin version;
decode AOD bundled và AOD BZI6 donor trong cache đã xác minh. Theo callback→DI→
lifecycle ctor/state changes, getters, attach, disconnect, classloader discard.
So interface PluginKeyguardStatusView/annotations đầy đủ, không chỉ getVersion.

**Hướng A ưu tiên nếu chênh lệch hẹp:** handoff có quản lý trạng thái:

1. Main thực hiện lifecycle/callback và các đối tượng bắt buộc main.
2. Chỉ sau khi hoàn tất mới chạy phần continuation BG thực sự an toàn.
3. Recheck plugin instance + generation/token trước publish và attach.
4. Disconnect/reconnect/classloader discard hủy continuation cũ; teardown và
   listener/View cleanup ở đúng luồng. Không để A cũ attach sau B mới.

Có thể chuyển cả pha kết nối sang main nếu audit chứng minh nhẹ, không I/O
hoặc blocking. Không đổi toàn mBgHandler thành main một cách máy móc; không
main↔BG wait hai chiều; không bỏ LifecycleRegistry assertion hoặc nuốt lỗi.

**Hướng B khi contract4006 quá khác4022:** rebase AOD lên BZI6, port có chọn
lọc Infinity bind/unbind/doze/scrim hooks. Không copy nguyên PluginAODImpl cũ
với các type obfuscated sang donor. Giữ SpriteWallpaper, wallpaper-res,
native engines, policy và sensor pair đúng target. Không bỏ Infinity để lấy
chỉ số “SystemUI không crash”.

Các điểm mở rộng đã thấy: module `s9_infinity_Wallpaper/source/rebuild.py`,
source/aod, payload APK/SHA256SUMS, customize.sh nếu cần selector donor;
`source/verify_built_wallpapers.py` và gate sau rebuild trong make_rom.sh.
Nếu thêm source FaceWidget, sửa rebuild để đặt đúng package/DEX; không nhét
vào package aodservice/plugin vốn chỉ copy các hook hiện có. Nếu sửa caller,
patch qua recipe SystemUI hiện có, không sửa riêng cache decoded.

**Gate PC/compiled:** multidex round-trip, interface/method/field/annotation,
signature/alignment/final image hash và không stale oat. Chứng minh ordering
callback hoàn tất→getter→connected recheck→main attach. Test race có thật:
disconnect trước main task; reconnect A→B; classloader discard; main inline;
BG continuation bị hủy; exception không bị che. Giữ gate hash riêng M1/M2/M4.

**Gate máy:** có marker/trace xác nhận Looper.myLooper()==Looper.getMainLooper()
tại lifecycle/callback/attach/teardown và ordering trên classloader đang chạy;
bỏ marker ồn trước package. SystemUI + SystemServer không restart ngoài ý
muốn/ANR15phút; tách cửa sổ này khỏi test chủ động restart/reconnect. Setup Wizard
và Home render, có focus/touch; lock/unlock20lần, AOD bật/tắt, notification,
media clock và plugin reconnect. Infinity đủ5 native +5 UN1CA, preview/apply,
lock/home, stars/fade/clock đúng thiết kế; wallpaper khác không bị fullscreen
hay giữ listener/scrim cũ. Không chỉ đo process sống trong khi widget biến mất.

## 7. Bước4 — NFC: hoàn chỉnh cả Java/JNI/provider/namespace/HAL

Lỗi `fast-008-crash.txt:5` và `fast-018-crash.txt:5`:
`dlopen failed: library "libnfc-nci.so" not found: needed by /system/lib64/libnfc_nci_jni.so in namespace clns-shared-9`.
Recipe nfc/customize.sh:64–86 stage JNI PHN nhưng thiếu provider cùng nguồn.

1. Chốt JNI thật trong SYSTEM và NFC APEX, provider/sidecar, linkerconfig và
   active APEX exports. Lập DAG DT_NEEDED, resolve undefined strong symbols
   cùng symbol versions và namespace. Có tên file chưa chứng minh ABI đúng.
2. Provider PHN có thật, SHA5568864964b841219af84532ae6ae6fd4e135de74cf0eb72bd4a04fe955fec82.
   Ngoài libnfc-nci còn phải kiểm libmetricslogger→libstatssocket, chrome,
   statslog, HIDL transport/hwbinder và ICU visibility. Không chép core
   Bionic/libbase/libutils/libc++ cũ đè SYSTEM hoặc mở toàn namespace.
3. So bảng native methods/callback/JNI_OnLoad của Java17 NativeNfcManager
   với JNI PHN và JNI BZI6. Nếu stock JNI khớp thì dùng closure nhỏ có khóa.
   Nếu không khớp, đánh giá route JNI BZI6→SLSI/HIDL thật; static references
   HIDL/AIDL không đủ chứng minh route đó hoạt động.
4. Giữ bộ SLSI target trong exynos9810_device_stack/nfc: service1.2,
   manifest/init, nfc_nci_sec.so, HIDL1.0/1.1/1.2/Samsung2.0 và firmware/RF
   star2lte đã khóa. Không thay sang donor NXP hoặc thay whole vendor.
5. Mở rộng `nfc/customize.sh`, `a376b_android17_nfc.lock.json` và
   `verify_m4_runtime_image.py`: hiện gate APEX/SecNfc chưa đủ để kiểm closure
   native. Thêm hash/source/ABI/metadata cuối image và reject thiếu dependency,
   lệch export/JNI, partial sidecar, sai donor/model.

Thử load và JNI registration trong đúng process/classloader NFC/APEX. Shell
dlopen chỉ là chẩn đoán phụ. Sau đó service đăng ký đúng HAL, không crash/AVC,
bật/tắt nhiều lần, đọc tag thật, sleep/wake và reboot. HCE/eSE/UICC chỉ ghi
PASS khi đã kiểm chức năng đó. Không có tag thì kết quả tag I/O còn chờ,
không dùng việc service sống thay cho test RF.

## 8. Bước5–6 — regression và thoát phụ thuộc chẩn đoán

- Kiểm lại Bixby receiver `No matching ComponentResult found!` trên nền UI ổn;
  xác định component bị thiếu/disabled do feature hay do setup/data cũ.
  Không suy lỗi remap đã chủ động tắt thành phải bật lại toàn bộ Bixby.
- Chạy lại fixtures197/198. Kiểm networking thật: Wi-Fi/connect/reconnect,
  DHCP/DNS, LAN transfer có giới hạn, stats theo UID/interface, transport và
  traffic sau reboot. Native getNetworkBlockedReason cần kết quả lỗi/blocked
  đúng semantics trên kernel thực; không chỉ load được symbol. Không giả
  success hoặc giảm API level để vượt loader.
- Graphics cần SurfaceFlinger/mapper ổn, không fatal native, touch, sleep/wake,
  camera preview/video; GPU meminfo-1 chưa phải chứng minh GPU rendering lỗi,
  cũng chưa phải PASS GPU memory accounting.
- Dùng PID + cmdline/UID/parent/starttime/boot_id, không riêng pidof. Log cũ,
  mmap slot cũ và nhiều crash trong một buffer không được tính là lượt mới.
- Sau khi giữ nguyên composition chức năng ổn, dùng một trial ramdisk riêng
  bỏ PID1/probe13 và marker chẩn đoán, giữ kernel/DTB/cmdline. Không yêu cầu
  BOOT hash vẫn b019… khi ramdisk đổi; so từng component và hash BOOT mới.
  Profile kernel ZIP hiện có và BOOT probe không mặc định byte-identical.
- Giảm logger theo profile sau khi gate ổn; không để logger/default script
  trở thành điều kiện cần cho boot. Cold boot và reboot phải vẫn đạt.

## 9. Bước7–8 — gói mới và fresh nghiệm thu

Build sạch bằng fork hiện có sau khi từng nhóm sửa đã có evidence. Dùng cache
đầu vào khóa donor/version/hash, vô hiệu đúng cache APK bị thay và kiểm không
restore mất sửa đổi. Chạy compiled gates ADB/Auto Blocker, PIF, Settings,
wallpaper, ABI/native32, runtime, SELinux; kiểm file và metadata trong ext4
thật, không chỉ work_dir. Nếu production kernel selection chưa được chốt theo
provenance thì gói vẫn phải ghi rõ candidate/diagnostic, không gắn nhãn release.

Package run riêng với config/source/firmware/tool locks, full ZIP/target-files,
8 ZIP32 có manifest/CRC/chữ ký/order/ranges. `.complete` chứng minh gói xong,
không chứng minh boot. Giữ M4 attempt37 tới khi gói mới hoàn chỉnh và kiểm
hash/manifest; sau đó mới thay bản giữ theo retention một bản cuối mỗi mốc.
M1/M2 và firmware input giữ nguyên. Không đưa firmware/key/raw log vào Git.

Fresh-flash chỉ sau audit package/installer/phân vùng và identity đúng F.
Format USERDATA theo quyền hiện hành, kiểm bằng filesystem UUID/dấu dữ liệu
mới, rồi thu defaults trước mọi thay đổi setting. Giữ kernel/BOOT provenance
như candidate đã chốt, không đổi hai biến trong lượt nghiệm thu.

Gate tối thiểu:

- First boot có log tiến triển, boot_completed1, Setup Wizard/Home thực,
  SystemServer/SystemUI/SDHMS/NFC không crash loop; Enforcing.
- ADB enabled mặc định, Auto Blocker tắt; máy tính không có key cũ nhận ADB
  không hỏi RSA. Cô lập host key/server khi thử để không phá kết nối máy đối
  chứng. Tắt USB debugging/bật Auto Blocker phải có tác dụng thật; bật lại qua
  Settings trên máy khi ADB đã tắt, không thêm script tự ép lựa chọn.
- Warm reboot ít nhất3lần, cold boot, sleep/wake/charge; lựa chọn Settings và
  dữ liệu người dùng giữ đúng, mặc định không seed lại sau reboot.
- Ma trận display/touch/AOD/audio/mic/camera/video/Wi-Fi/BT/GPS/sensor/vân tay/
  NFC/charging/thermal; RIL/SIM/IMS theo F và môi trường SIM thực. Cuộc gọi/SMS
  kiểm với đầu mối người dùng chọn, không tự liên hệ người khác.
- Network stats và pin/nhiệt có số liệu thực; verdict Play Integrity chỉ ghi
  theo kết quả trực tiếp, không hứa Strong/Device từ gate compiled hay dùng
  keybox không rõ nguồn. Các tính năng đã chủ động defer vẫn giữ trạng thái đó.

G965N có gate riêng sau khi F đạt và người dùng xác nhận: blob/config N đúng
phần cứng, giữ radio bridge ABI khóa khi cần, kernel/DTB/provenance đúng N.
Không mang image F sang N hoặc suy từ boot F thành PASS N.

## 10. Quy tắc mỗi trial, điều kiện dừng và hồ sơ hoàn tất

Mỗi trial có manifest nền→delta, reason/files/verify/rollback, hash input/output,
phạm vi ghi, recovery identity, protected-partition comparisons, boot_id và
log marker/generation mới. Không fresh-format để thử một lỗi callback/parser
hay thư viện đơn lẻ. Snapshot dữ liệu/cache có schema thay đổi trước khi thử.

Nếu có fatal/ANR mới, sensor/control không hợp lệ hoặc chưa boot: lấy crash,
system/main/kernel/CACHE/ANR/tombstones trước; đưa đúng F về TWRP và rollback
delta hoặc toàn nền theo manifest. ADB mất thì chỉ cần người dùng hỗ trợ phím
cứng/cáp; không tự suy đó là regression Auto Blocker. Có tiến triển dexopt mới
thì mới gia hạn thời gian chờ, không lặp reboot mù.

Mỗi phép ghi và khôi phục kiểm UID/GID/mode/SELinux/hash. So raw hash các phân
vùng ngoài phạm vi trong cùng phép ghi; biến đổi EFS/ODM qua Android/recovery
ghi riêng, không tự restore EFS/modem. SYSTEM file rollback không gọi là
khôi phục raw ext4 byte-identical. M1/control và G965N không bị ghi trong các
trial F này.

Nếu giải pháp buộc giảm Infinity/AOD/DeX/NFC hoặc mở rộng kernel/radio ngoài
bằng chứng hiện có, trình phương án cụ thể và phần mất trước khi chọn; không
âm thầm đánh dấu PASS. Các sửa routine đúng scope tiếp tục theo quyền hiện hành.

Sau mỗi nhóm: cập nhật docs/bugs.md, patch-matrix.md, sources.md, repo-map.md và
báo cáo runtime; source lock/series replay phải khớp. Kết thúc mốc mới cập nhật
README/STATUS theo nghiệm thu thật. Hạng mục còn thiếu dụng cụ/tag/SIM/ngưỡng
test ghi rõ pending, không thay bằng kết quả suy luận.


## Tiến độ triển khai 28/10/2026

- **Bước0:** G965F serial `M2_G965F_SERIAL` được xác minh live ở TWRP; BOOT hash `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d` khớp baseline đã ghi. Một lần đọc hàng loạt partition timeout nên phạm vi hash chỉ được coi là từng phần; chưa có ghi máy.
- **Bước1 / patch0199:** profile `m4-g965f-a376b-android17-runtime-init-debug.sh` chọn composition hậu-restore rõ ràng. Fixture từ BZI6 xác minh Tethering APEX SHA256 `f17fa222e1f127d349e61b592c705c127f682ad4d22dda164c03397857862cc6` và `libmeminfo.so` SHA256 `b19a1c37a8ebed72676739a12ab46be31dd3c74d4ba1c605f655a62661d70fe9`; verifier ext4 kiểm byte và metadata. Patch replay fixture byte-for-byte PASS. Profile thường không đổi.
- **Bước2 source / patch0200:** người dùng chốt bỏ riêng `<DEX formula="SKIN" temp="470" />` vì parser BZI6 không có enum DEX. SIOP input/output hash là `8e30ff47…23503` → `94a74d1c…47be`; output là input sau xóa đúng một dòng. SSRM giữ asset riêng đã khóa `6784ecc1…cb96`. Fixture XML/hash PASS. Đây **chưa** phải thermal runtime acceptance: cần xác minh sensor samples, policy load, limiter/actuator readback và cooldown trên S9. DeX-special overheat branch không được nghiệm thu.
- **Bước3 source / patch0201:** log `systemui-first-crash.txt:15` chỉ rõ callback trên `PluginFaceWidget-BG` tạo `FaceWidgetPluginLifeCycle`, gây `Method setCurrentState must be called on the main thread`. Route init sang handler UI đã xác minh dùng `Looper.getMainLooper()`; generation tăng ở connect/disconnect, UI kiểm cả generation và plugin instance rồi mới gọi init đồng bộ trên main. APK fixture build/decode và semantic guard PASS. Toàn bộ khối init chạy main một lần, nên startup latency/reconnect và UI ổn định vẫn là runtime gate. Không tắt lifecycle assertion.
- **Còn pending:** gate APK/ext4 của ROM candidate, sensor/limiter runtime, đo thời gian và thử 15 phút/20 lock-unlock cho SystemUI–FaceWidget, NFC dependency/JNI/HAL, regression, probe-removal trial, clean package và acceptance. Attempt37/source0188 tiếp tục được giữ nguyên tới khi candidate hoàn chỉnh và audit đủ.

Bằng chứng và file/lý do/kiểm/rollback: [M4 post-boot repair record](m4-postboot-runtime-repair.md).

### 28/10/2026 attempt0208 status update

Attempt0207 cache confirms repeated `netd` loader failure: `CANNOT LINK EXECUTABLE "/system/bin/netd": library "libnetd_resolv.so" not found`; post-fs and post-fs-data had both run. Patch0207 now adds the hash-pinned BZI6 `libnetd_resolv.so` plus APEX manifest field7 `provideNativeLibs`; independent APEX builds match SHA `03a0fbcdd952df34795a6b3648583ae7feb6b49ab785a3cd074db85f7fb2a2ba`. Attempt0208 clean build reached postboot composition after about12 minutes and failed only in its final JSON print: `KeyError: PosixPath('system/system/apex/com.google.android.tethering_compressed.apex')` at `a376b_android17_postboot_diagnostic.py:149`. Patch0208 fixes Path-vs-string report indexing. Resume the verified staged work_dir, run scoped helper verify/apply, then compiled release/ext4/target-files/8-stage audits. Attempt0207 and retained attempt37 remain unchanged. No flash occurred in attempt0208 yet.
