# M4 sau0215 — phương án sửa lỗi chi tiết

Ngày28/10/2026. Thay thứ tự/trạng thái của plan sau0212; giữ tài liệu cũ làm
lịch sử. Bằng chứng runtime mới: [m4-validation-after-0215.md](m4-validation-after-0215.md).
Phạm vi G965F; Hybrid Vendor VNDK33, blob F và radio bridge đã khóa. Kernel
DS-ACK/BOOT hiện hành vẫn giữ, không có bằng chứng cần build kernel mới trong
lượt kiểm này. Không đánh số patch kế tiếp trước khi có thay đổi thật.

## 1. Thứ tự thực hiện và quy tắc chung

| Thứ tự | Việc | Điều kiện hoàn thành |
| --- | --- | --- |
|1| Sửa scale brightness trong BacklightAdapter | Raw đúng thangPAC3, lookup luminance thay đổi, off/wake và limiter không hồi quy |
|2| RTC exact mapping và audit thermal actuator | RTC đọc đúng dưới Enforcing; formula/policy/actuator/readback có provenance; đủ gate tối thiểu trước tải dài |
|3| Bluetooth VaultKeeper selector/storage path | BLE/BT ON trước timeout,10cycle,15phút không restart rồi test profile thật |
|4| libbase/fwmark receive boundary | Xác định nhánh trả-90, sửa đúng file, IP/DNS/HTTPS app và network semantics qua gate |
|5| Hợp đồng FaceWidget/AOD/Infinity | Không thiếu abstract API, lifecycle đúng main/BG; lock/wake/AOD/wallpaper qua gate |
|6| Công cụ Binder rồi NFC functional | Xác định caller/FD security; query tin cậy rồi tag/RF/HCE theo gate riêng |
|7| Bixby/KG/GSA | Phân loại input/TA/heap corruption và tái hiện theo version/hash; không giả success |
|8| Nghiệm thu phần cứng và package sạch | Trial đã khóa vào fork,compiled/image/source replay,fresh install và test matrix hoàn chỉnh |

Có thể phân tích source nhiều nhóm cùng lúc; trên điện thoại mỗi trial chỉ
đổi một unit. Dùng điểm mở rộng fork sẵn có; chưa cần full package cho mỗi
thí nghiệm. Trước ghi kiểm serial/bootloader,preimage/hash,metadata và rollback.
Không ghi EFS/CPEFS/modem/bootloader/recovery. Không bỏ Enforcing,không tạo
allow rộng,không ghi success giả hay bỏ timeout để che đường chờ sai.

## 2. Brightness: sửa đúng thang PAC3 trước

**Nguyên nhân đã khoanh:** kernel hiện chạy PAC3 scale100/S6E3HA8. Device
services.jar khớp staged; PowerManagerUtil AUTO_BRIGHTNESS_TYPE3 làm
USE_PERSONAL_AUTO_BRIGHTNESS_V3=false. BacklightAdapter dùng cờ đó để chọn1×,
nhưng native lights HAL chuyển extension nguyên vẹn. Settings60/180 thành
raw59/180; bảng kernel dưới700 đều lookup luminance3. `actual_brightness` không
phải máy đo ánh sáng, nhưng pattern hiện tại khớp lỗi scale.

**File dự kiến:**

- `rom/unica/patches/legacy/lights/services.jar/0003-BZI6-legacy-SehLights-backlight.patch`.
- `rom/unica/patches/legacy/lights/patch_bzi6_sehlights.py` (SHA patch guard tại72)
  hoặc một transformation exact-hash riêng sau adapter hiện tại.
- Existing compiled/image gates M4,`patches/fork/series`,source lock và hồ sơ
  patch. `rom/platform/exynos9810/installer/customize.sh:134` cần sửa comment
  scaling nếu comment còn mô tả selector cũ.

**Cách sửa:** thêm scale100 riêng cho main backlight extension trên đúng
M4/BZI6/G965F,PAC3 và HAL đã pin. Giữ AUTO_BRIGHTNESS_TYPE3,cảm biến,
USE_PERSONAL... toàn cục,Parcel order,overload/cache,mute/unmute,color0/off,
SurfaceControl branch và các light khác. Không sửa kernel,không nhân thang
trong cả framework lẫn HAL. Source audit native:
setLightState@90f0→setLightStateInternal@7070→type0@70c8→setLightBacklight@5aa0
→writeIntToFileDesc@4610; extension không âm được ghi nguyên integer.

**Kiểm PC:** pin preimage services/class,HAL SHA,kernel config/BOOT provenance;
reject wrong donor/model/hash. Build services.jar,decode output; kiểm đúng
factor100 trong path đang chạy,không scale hai lần; giữ nguyên các method
ngoài scope,extension Parcel,cache và unmute semantics. Image gate phải đọc
ngược bytes/UID/GID/mode/context từ SYSTEM ext4. M1/M2 fixtures không đổi.

**Trial:** chỉ services.jar theo protocol thay runtime hiện có,backup exact
bytes/metadata và cache/oat cần thiết. Awake/manual mode,test mức thấp20→60→120;
kỳ vọng raw khoảng2000→6000→12000,luminance lookup khác nhau theo bảng panel.
Không bắt đầu230 vì sửa scale tăng sáng lớn so với lum3 cũ. So screen-off,
AOD,wake,smooth ramp,thermal brightness limit,HBM bounds; đọc lỗi display/lights.
Khôi phục brightness/mode/stay-awake sau thử. Cần xác nhận panel thật thay đổi
sáng; screenshot chỉ xác minh render.

**Rollback:** trả đúng services.jar+metadata/hash và xử lý đúng oat/cache của
unit; reboot và đọc ngược hash baseline. Nếu raw100× mà luminance không đúng,
dừng,snapshot lights/kernel log,không đẩy mức sáng cao hoặc đổi bảng kernel.

## 3. RTC và thermal actuator: hai thay đổi độc lập

**RTC bằng chứng:** real path
`/sys/devices/platform/141c0000.speedy/i2c-17/17-0000/s2mps18-rtc/rtc/rtc0/`
chứa date/time/since_epoch,0444 root,generic sysfs. Existing CIL có i2c-10 và
18,thiếu17; quyền system_app_33_0→sysfs_rtc_33_0 read/open/getattr đã tồn tại.
SDHMS d5/i.E(J) đọc RTC cho nhánh log `[RTC]`; chưa chứng minh lỗi RTC phá sensor.

**RTC file/sửa:** mở rộng exact-node helper/lock hiện có
`exynos9810_boot_restore/a376b_android17_thermal.py`/lock và vendor image verifier,
hoặc helper riêng nếu giữ scope rõ hơn. Trial tạm label đúng3file sysfs_rtc,
kích hoạt đúng diagnostic read,ghi advancing time/EACCES rồi trả original label.
Chỉ khi trial có tác dụng mới thêm3genfscon; không thêm allow/write và không
label cả speedy tree. Negative fixtures duplicate/conflict/lost permission;
fresh Enforcing gate và rollback reverse riêng unit RTC.

**HAL/SDHMS bằng chứng:** Samsung thermal binary SHA
`bf7a71cdea61ed56c26df431856737401462c80f861c36d802e6b67e9d8de7bd`
trả đúng strings lỗi cooling/threshold; nhiệt độ/status vẫn lên. SDHMS Binder
getCoolingDevices() tự trả ArrayList rỗng,khác HIDL và khác Linux cooling.
SDHMS dùng Samsung sehGetTemperatures (Celsius→×10),Brightness limiter gọi
PowerManager.setMasterBrightnessLimit; limiter khác scan cooling_device names.
Không biến Linux BIG83°C thành Samsung AP/SKIN threshold bằng suy luận.

**File/audit:** `rom/unica/patches/dvfs/customize.sh:140–166`,
`a376b_android17_dvfs_policy.py/.lock.json`,locked siop_default.xml/ssrm,
SDHMS compiled `a1/m`, `j5/o3`, `j5/a1`,Samsung thermal service disassembly.
Lập bảng sensor thật→unit/formula→threshold/hysteresis→limiter name→actuator
path/API→readback. Đối chiếu built-in name map với thermal-gpufreq-0,
thermal-isp-0,thermal-cpufreq-0/1; parse ý nghĩa -1 từ code,không gọi là disabled
hoặc valid từ XML đơn lẻ. So M1 control nếu kết nối được,tránh đổi baseline M1.

**Gate:** nhiều mẫu có timestamp,liveness; tác vụ hạn chế thật khi policy
condition xảy ra và hồi phục khi cooldown; không dùng service-alive làm PASS.
Tách API framework không hỗ trợ khỏi giới hạn an toàn thực tế. Nếu vendor limiter
đúng nhưng generic threshold API không có dữ liệu,hồ sơ phải ghi limitation;
không tạo bảng giả để xóa log. Trước camera/video dài cần policy stop condition
và independent sensor đã xác minh. Hiện không có bằng chứng để đổi kernel.

## 4. Bluetooth: bỏ đường chọn backend không có bằng adaptation đúng

**Evidence:** HCI reset hoàn tất,JNI SYSTEM được nạp,nhưng vkmanager AIDL chờ
VaultKeeper/default nhiều lần~29s rồi irk read failed;GATT vừa xong thì
BLE_START_TIMEOUT. Snapshot17crash,cuối38;6PID trong180s. `LOGW` pattern cũ
không match trong build không phải adaptation M4 thành công.

**File dự kiến:** `rom/unica/patches/bluetooth/customize.sh:31–179`,helper/lock
BZI6 mới trong module đó,existing finalization customize.sh:1333–1345 và
image/compiled gate cho SYSTEM `libbluetooth_jni.so`.

1. Pin JNI SHA `f8a662ca15d9526ed8271af8ecdb97bb09e9b850386d0f77abe17eb77c030757`,
   vkmanager SHA `0fdf61d254ff10c50f9f5e9027e3d2f5e0ac1fb938637c086540f85ffb1596e4`
   và APK/APEX/maps. Một native stack trong cửa sổ wait để khóa call path;
   không cần lặp hàng chục enable failure nữa.
2. Disassemble support selector và fallback BZI6;chứng minh backend storage
   fallback hợp lệ,giữ identity/key semantics. Không đọc/ghi IRK/link key,EFS;
   không add find-default_android_service hoặc tăng timeout.
3. Adapt selector có scope/preimage/callsite/control-flow/output hash/idempotence;
   wrong donor/hash phải fail build. Giữ HCI/HAL/firmware/radio/kernel.
4. Trial chỉ JNI+metadata;ON trước timeout,10toggle,15phút không restart;
   rồi scan/pair/reconnect/A2DP/OPP với thiết bị thật,HFP gate riêng.
5. Nếu điểm lỗi chuyển tới GATT/offload,thu callback/opcode/status mới trước
   sửa tiếp. Rollback exact JNI;chỉ snapshot storage nếu schema thật sự đổi,
   không wipe Bluetooth data để che lỗi.

## 5. Mạng: định vị branch trả EMSGSIZE trước chọn file patch

Fresh ping IP/DNS fail Message too long;current raw TCP connect PASS. Trace
cũ xác nhận fwmarkd gửi52/nhận int32-90 trước IP connect. netd BZI6 nhận52,maxFD1
qua libbase;length mismatch trả-74,khác lỗi đã đo. libbase nhận control24,
kiểm TRUNC/CTRUNC và excessFD để đặt errno90. Raw socketpair mới chứng minh
recv flags0x40004028 và basic SCM_RIGHTS không tự lỗi trên kernel hiện hành.

**Thử kế tiếp:** process diagnostic riêng gọi đúng
`libbase::ReceiveFileDescriptorVector` BZI6 với cùng52byte+FD;pin libbase/maps/
namespace/CRT/TLS. So raw và wrapper cùng type/options;PASSCRED là control,
không tự khẳng định fwmark dùng nó. netd.rc chỉ khai socket stream không passcred.
Nếu wrapper cũng PASS,thu metadata một receive thật trên server: iov_len,
ancillary capacity/returned len,msg_flags,FD count,helper return/errno. Không
thu payload/credential/key;không sửa daemon để trả success giả.

**File chọn sau bằng chứng:** serializer/client nếu sender sai;SYSTEM
netd/libnetd_client hoặc callsite/provider nếu buffer/contract sai. Existing
`a376b_android17_network_native.py/.lock.json` và postboot helper chỉ dùng nếu
file sở hữu lỗi thực thuộc composition đó. Không thay toàn libbase hoặc nhét
sửa vào Tethering APEX do có builder sẵn. Giữ resolver0211,ION0212,BPF/Java
network đã qua gate.

**Gate:** IP/DNS/nc hết EMSGSIZE,appHTTP/HTTPS thật,DHCP/reconnect,UID/interface
stats,blocked/unblocked/network selection. VPN/hotspot/mobile data gate riêng.
Rollback whole affected provider/config unit;netstats snapshot chỉ khi schema
reader/writer đổi. Chưa có lý do sửa kernel hoặc MTU.

## 6. AOD/FaceWidget: sửa cả hợp đồng plugin

Fresh crash abstract disconnectPluginOnBackground;compiled interface có62method,
retained implementation thiếu12abstract,hai method không VersionCheck là
`disconnectPluginOnBackground()V` và `setDarkAmounts(FF)V`. Method thứ hai có
caller catchall nên không crash chưa chứng minh hoạt động.

**File:** `rom/unica/mods/s9_infinity_Wallpaper/source/rebuild.py` hiện chỉ overlay
aodservice/plugin,không overlay FaceWidget implementation. Thêm source adapter
FaceWidget rõ ràng,update source/aod nếu cần,SHA256SUMS/customize.sh và compiled
contract verifier. Giữ guard0201/main-thread/generation/instance helper hiện có.

Audit lifecycle/DI/callback thật. Disconnect phải tháo observer/listener/job ở
BG,View/Lifecycle cleanup ở main. Trace hai dark amounts tới doze/scrim/wallpaper
model,không no-op hoặc catch lỗi mới. Nếu lifecycle retained quá khác,rebase
FaceWidget BZI6 rồi port Infinity hooks có chọn lọc.

**Gate:** APK/DEX roundtrip,abstract contract đầy đủ;connectA→disconnect→connectB,
classloader reload;boot sạch,15phút không restart,20lock/wake,AOD toggle,
notification/media/widgets,5native+5UN1CA Infinity và wallpaper thường.
Rollback SystemUI/AOD/FaceWidget/wallpaper như một unit đã audit.

## 7. Binder diagnostics rồi NFC

NFC/SDHMS và các dump audio/netd có cùng FAILED_TRANSACTION. Binder log FD
translation EPERM;security hook kiểm receiver đối với FD-opener sid,không dựa
chỉ UID caller. su permissive không làm receiver permissive. dumpsys tạo pipe
nội bộ dưới su;policy appdomain→shell/adbd FD có sẵn nhưng không generic→su.
Đây là đầu mối chẩn đoán,không phải lý do nới policy toàn cục.

Phép thử true shell domain chưa hoàn thành: setuid2000 vẫn su;runcon toybox
entrypoint bị denied;runcon shell_exec đúng lại segfault. Giữ log này riêng.
Bước kế tiếp kiểm native failure của helper và đường tạo dump pipe dưới đúng
caller/domain;hoặc client Binder/query API độc lập có context hợp lệ,không đoán
transaction code. Nếu dùng adb unroot tạm,phải chứng minh đường root rollback
và noRSA/USB-debugging behavior trước,không làm mất ADB giữa chừng.

Sau khi query tin cậy,kiểm adapter STATE_ON,off/on,HAL callback/RF/screen routing;
tag thật đọc lặp,screen-off/reboot. HCE/UICC/eSE/payment gate riêng. Giữ BZI6
Java37/JNI54 pair và target SLSI HAL;chỉ sửa NFC helper/lock nếu log chỉ ra
closure/JNI/HAL thật. Rollback đúng NFC unit;không đổi blob từ dump failure.

## 8. App và native crash

- **Bixby:** pin cert/manifest/package/input registry BZI6,decode
  LockedBootCompletedReceiver→d1.c.g để tìm ComponentResult thiếu. Ưu tiên
  selection/config,giữ donor signature/account/store compatibility. Hai cold
  boot không receiver crash,onboarding bình thường,key/WINK kiểm riêng.
- **KG:** lấy ANR/Binder/vendor-service stacks,phân biệt TA unsupported/TLC_ERROR
  với queue/retry timeout. UID5959 đã đúng không chứng minh TA đúng. Không sửa
  keybox/modem/EFS,không giảTA success;không khắc phục được thì ghi limitation
  và behavior timeout có giới hạn đúng semantics.
- **Google Assistant:** tombstone00 chỉ một heap-invalid-reference crash đầu boot.
  Khóa APK/version/splits/cert tại thời điểm tái hiện vì Play Store đã schedule
  update sau crash. Xác minh libart/boot image provenance và class/static fieldd,
  JNI/Unsafe/JIT maps. Tái hiện cold voice-interactor;chỉ nâng audit ART chung
  khi nhiều app hoặc fixture độc lập cùng lỗi. Không tắt GC verification hay
  đổi nguyên ART/kernel từ stack GC vốn có thể là nơi phát hiện lỗi thứ cấp.

## 9. Tích hợp và nghiệm thu

Sau từng unit: reason,file,preimage/output hash,compiled/image gate,device readback,
log đúng attempt và rollback. Chỉ package sạch khi các unit đạt;fork build cũ,
không dựng build system mới. Mỗi run riêng,không ghi đè artifact đang giữ;
retention theo chỉ thị giữ bản cuối của từng M1/M2/M4 sau khi bản thay thế hoàn
chỉnh/manifest/hash được xác minh.

Fresh acceptance cuối:boot Enforcing và core stability30phút;không crash/ANR
chưa phân loại;full brightness/auto/AOD;BT profiles;Wi-Fi appHTTPS/VPN/hotspot;
camera3IDs ảnh/video trước/sau,codec playback/record,mic/loa/call;sensor/GNSS;
NFC tag;SMS/mobile data/IMS theo SIM/network thật. PC mới ADB không RSA,default
USB debuggingON/Auto BlockerOFF và công tắc thật vẫn có tác dụng,không boot-loop
script ép lại setting. PIF phải có verdict test thật,không suy từ toggle.
G965N là lượt riêng sau khi F đạt và nguồn blob/configN được audit.

## 10. Điểm source để bắt đầu triển khai

| Bằng chứng | File/điểm đã đọc |
| --- | --- |
| PAC3 scale100 | `inputs/kernel-source-duhansysl-v1.12/drivers/video/fbdev/exynos/panel/panel_bl.h:26–28`; config chạy thực trong log `running-config.gz` |
| actual_brightness là lookup | Cùng tree `panel_bl.c:202,356,757`; `s6e3ha8/s6e3ha8_star2_a3_s0_panel_dimming.h:336–361` |
| Feature type3 | `rom/unica/patches/product_feature/a376b_android17_display.json:93–106`; services decoded `PowerManagerUtil.smali:332–352` |
| Sai selector scale | `rom/unica/patches/legacy/lights/services.jar/0003-BZI6-legacy-SehLights-backlight.patch:78–112` |
| Hook lights/guard SHA | `rom/unica/patches/legacy/customize.sh:398–405`; `lights/patch_bzi6_sehlights.py:72,87–135` |
| RTC map thiếu i2c17 | Embedded vendor CIL:328,336 (có i2c10/18); staged CIL:6604–6606 quyềnread; system mapping33.0:2383 |
| SDHMS limiter | `logs/20261028-m4-postboot-repair/sdhms-bzi6-decoded/smali/j5/a1.smali:245–279`; `j5/o3.smali:391–401,1643–1707`; `a1/m.smali:859,912,958–968` |
| BT warning-only cũ | `rom/unica/patches/bluetooth/customize.sh:31–179`; finalization hook:1333–1345 |
| fwmark parser/provider | netdBZI6 VA5ea3c/5ea40/5ea44, mismatch5ea74; libbaseBZI6 VA110f8,11270–112a8,1131c,11768; hashes trong audit/report |
| FaceWidget source thiếu | `rom/unica/mods/s9_infinity_Wallpaper/source/rebuild.py:11–21`; compiled interface audit cũ `aod-interface-audit.json` |
| FD security | Source build path trong `logs/20261024-m2-runtime-fix/build-paths.json`; kernel `drivers/android/binder.c:2736–2791,3616`; `security/selinux/hooks.c:2354–2392` |
| GSA corruption | `logs/20261028-m4-validation-after-0215/tombstone_00:15–18,239–258`; raw ngoàiGit |

Các đường `rom/out/` và `logs/` là bằng chứng cục bộ, không commit payload/ảnh/raw
log. Khi đưa source lên repo, dùng script/patch và hash/provenance đã lọc.
