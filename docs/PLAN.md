# Kế hoạch thực thi hiện hành

**Đối chứng M4 sau bootloop — 01/11/2026:** bốn tổ hợp framework/BOOT ứng viên
và r5 đều chưa boot khi giữ phần SYSTEM/VENDOR/ODM còn lại của ứng viên. Build
ứng viên đã chọn sai gói BOOT nguyên bản thay vì gói WIPEONFORK diagnostic của
r5, đồng thời tắt init logger; đổi BOOT hoặc framework riêng lẻ chưa sửa được
bootloop. Chưa xác định nguyên nhân. Máy đang ở TWRP với BOOT + framework r5
trên phần còn lại của ứng viên; đây không phải rollback r5 hoàn chỉnh. Chi tiết:
[đối chứng boot](m4-keybox-boot-control-20261101.md).

**Cập nhật Play Integrity M4 — 01/11/2026:** adapter BZI6 Keybox/PIF đã qua
source/compiled gates và tạo ứng viên build riêng. SHA256, file bị ảnh hưởng,
kiểm chứng và giới hạn được ghi trong
[trạng thái triển khai Integrity](m4-integrity-implementation-status-20261101.md).
Ứng viên sau đó đã chia stage và flash, nhưng bootloop; không có keybox được
cung cấp và không có verdict Google. Trạng thái máy và đối chứng mới nhất nằm
ở checkpoint phía trên. Phần Integrity chỉ được nghiệm thu qua trạng thái chứng
nhận/verdict thật; các mục cũ trong kế hoạch cộng đồng là đề xuất lịch sử.

**Kế hoạch Integrity từ upstream/cộng đồng, 01/11/2026:** ưu tiên port mod
`unica/mods/keybox` sẵn có cho BZI6, khôi phục import/status rồi kiểm crypto và
verdict server. M4 hiện chủ động bỏ mod này và ẩn UI; preflight trên copy chỉ
remap DEX còn5 hunk lỗi, không phải kết quả runtime. Tài liệu upstream và các
ca TEE lỗi/Android17 cho thấy KM3 không là điều kiện bắt buộc để thử hướng
giả lập; chưa có case cùng toàn bộ stack G965F của dự án. Keybox còn hiệu lực
là đầu vào riêng, chưa xác nhận có; không tải keybox leak. Chỉ nghiên cứu và
kiểm read-only, chưa đổi firmware máy. [Plan và nguồn](m4-play-integrity-community-plan-20261101.md).

**Tiếp tục audit KM3, 01/11/2026:** binary stock F mở HAL sẽ đi vào chuỗi mở
trustlet; không thể coi `linker --list` là phép thử TEE. Fixture tmpfs hai
namespace chứng minh client RO không ngăn mock daemon ở namespace khác ghi.
Source kernel chẩn đoán chỉ cho một kết nối admin và đóng daemon làm các request
đang đợi nhận `ESHUTDOWN`. Không chạy thêm daemon hay đổi daemon live; chưa có
guard đủ chứng minh bảo vệ EFS hoặc bản sửa Integrity. [Bằng chứng](m4-g965f-keymaster-tee-work-20261101.md).

**Mới nhất — sửa guard KM4 / nghiên cứu KM3, 01/11/2026:** fork đã bỏ bốn
lệnh init software KM4 đụng `/efs/DAK`, thêm final-tree và M4 ext4 image gate;
fixture dương/âm và kiểm ảnh ext4 mẫu PASS. Chưa build/flash/boot patch; r5 trên
máy còn RC cũ. Đây không sửa verdict Integrity. Đã xác định trustlet KM3 stock
F khớp máy và disassembly có nhánh `KM_generate_sak`/`KM_install_sak_cert` ghi
blob vào EFS. Scratch tmpfs đã chứng minh chặn ghi cần áp cho từng alias;
namespace PID1 và daemon TEE khác nhau, chưa có guard đủ chứng cứ cho EFS thật.
Không chạy HAL KM3 cho tới khi có phương pháp thử tương thích quy tắc EFS.
[Chi tiết và kiểm chứng](m4-g965f-keymaster-tee-work-20261101.md).

**Checkpoint audit Keymaster M4 r5, 01/11/2026:** hai EC key probe độc lập
trả software attestation; KeyInfo nói key không ở secure hardware. Trung gian
chain đã hết hạn và root không khớp Google Hardware Attestation Root công bố.
Live service/lib hash khớp software KM4 của fork; finalizer chủ động loại KM3
TEE. GMS chain và field readback trong tiến trình chưa được kiểm trực tiếp.
Không suy kết quả app probe thành nguyên nhân server duy nhất. Chưa có patch
runtime đủ bằng chứng; bước tiếp là audit dependency/blob/TEE KM3 theo variant,
chuẩn bị thử có rollback và kiểm KeyInfo/chain/boot, sau đó kiểm lại verdict.
Kiểm kê đầu: service/rc KM3 embedded khớp stock F; hai lib KM3 và impl 64-bit
khác hash vì được nối thêm đúng 23 byte nhãn DuhanROM sau toàn bộ file stock;
symbol động import/export và DT_NEEDED trùng. VaultKeeper stock có trong cache
và vắng ở đường embedded đã kiểm; stock F có HIDL 1.0, stage shim fork nhắc
2.0. Cần kiểm namespace/SELinux/chuỗi TEE trước khi thử ghép.
Chi tiết: [audit attestation](m4-r5-attestation-audit.md).

**Checkpoint sau Setup Wizard — M4 r5, 01/11/2026:** Setup Wizard đã hoàn tất;
model `SM-A376B`, boot complete/Enforcing. Trang Ẩn trạng thái nhà phát triển
qua mở/toggle/lưu lựa chọn/back/reopen. Checker và phép thử tích hợp Play Store
đều đã chạy mới với PIF ON/OFF: vẫn không có nhãn Integrity. Fingerprint
Play Store đổi đúng theo công tắc, record attestation được tạo thành công;
không thấy lại lỗi field writer trong capture này. PIF đã khôi phục ON.
Chưa xác định chính xác điều kiện server thất bại; bước tiếp là audit chain
thực và scope/readback GMS có kiểm soát trước khi chọn sửa/build khác. Không
đổi kernel/vendor/radio hay hứa nhãn Google. Bằng chứng, giới hạn và kế hoạch:
[r5 post-setup](m4-r5-post-setup-validation.md).

Các checkpoint chờ Setup Wizard và r4 bên dưới là lịch sử.

**Checkpoint lúc r5 vừa flash — 31/10/2026:** clean build/compiled/SELinux/
ext4/flashable gates PASS; 8/8 stage đã cài G965F, các hash phân vùng bảo vệ
khớp, e2fsck bảy filesystem PASS. Máy boot hoàn tất, model `SM-A376B`, ADB
keyless, Enforcing; probe writer Samsung String/int PASS. Setup Wizard đang chờ
hoàn tất để kiểm trang Settings và Play Integrity thực. Đã phát hiện sector GPT
thực 4096 byte; snapshot primary/backup mới qua CRC, snapshot r4 ghi 512 byte
không đầy đủ. Chi tiết, log và giới hạn: [r5 local repair](m4-r5-local-repair.md).

**Lịch sử r4 sau Setup Wizard — 31/10/2026:** người dùng xác nhận model trên
màn hình đã đúng `SM-A376B`, nhưng Integrity chưa đạt nhãn nào. Live
`user_setup_complete=1`; đã tự chạy checker/đọc JSON và kiểm trực tiếp Play
Store: `Labels: []`, thiết bị chưa chứng nhận. Finsky báo attestation record
generated successfully; GMS unstable còn lỗi SamsungPropsHooks. Mở riêng lỗi Settings `HideDeveloperStatusFragment`
đọc field `mMainSwitch` không tồn tại. Chưa có patch/flash mới; bước tiếp theo
là sửa helper SamsungPropsHooks đúng phạm vi/nhận diện và trang Settings,
sau đó kiểm ON/OFF và hai phép thử Integrity. Không cam kết nhãn server. Xem
[follow-up r4](m4-r4-integrity-followup-plan.md).

**Checkpoint lúc r4 vừa flash — r4 đã fresh-flash và boot, 31/10/2026:** G965F
`M2_G965F_SERIAL` đã cài 8/8 stage, hashes phân vùng bảo vệ trùng trước/sau,
e2fsck pass và `sys.boot_completed=1`. SecSettings/framework đã cài khớp bản
build; live model `SM-A376B`, `star2lte`, hardware `SM-G965F`, `rkp_only=0`,
ADB keyless và SELinux Enforcing. Probe tiến trình riêng gọi helper PIF đã cài
ghi/đọc thành công String và int. Setup Wizard của data mới chưa hoàn tất;
About phone trực quan, GMS request, verdict Google và OFF/allowlist vẫn cần
kiểm. Bixby và SamsungPropsHooks còn lỗi đã ghi. Chi tiết và artifact:
[r4 test](m4-r4-identity-integrity-test.md).

**Cập nhật r3 sau Setup Wizard — 31/10/2026:** người dùng báo Play Integrity
không đạt nhãn nào và About phone còn hiện `SM-G965F`. Log xác nhận hook PIF
thất bại, keystore không lấy được RPC; `ModelNameGetter` đọc boot model theo
patch mặc định của fork. Đã sửa source M4 cho model công khai và API Unsafe
đúng ART, qua fixture; build sạch r4 đang chạy. GMS runtime, Google verdict và
kiểm chứng keystore trong app context vẫn mở. Xem
[chẩn đoán sau Setup Wizard](m4-r3-post-setup-diagnosis.md).

**Lịch sử trước r4 — 31/10/2026, r3 đã flash:** user apps Shopee/TikTok vẫn
được người dùng xác nhận tải/chạy bình thường. NFC/Bixby RF/voice không port
thêm; giữ feature/library/package/API compatibility, không khai báo unsupported.
R3 đã clean-build, kiểm ảnh ext4 và flashable, đóng full ROM/stages, fresh-flash
G965F `M2_G965F_SERIAL`; boot tới Android SDK37/One UI9. Live identity là
`SM-A376B`/`Galaxy A37 5G`, `star2lte`, physical `SM-G965F`; ADB on/keyless và
Rampart/Auto Blocker settings off; NFC feature/library và Bixby packages còn.
Protected EFS/CPEFS/RADIO/RECOVERY/BOTA hashes không đổi, `e2fsck -fn` toàn bộ
image/data pass. Setup Wizard chưa hoàn tất; chưa có request Integrity runtime,
checker verdict, app Build.MODEL readback, hay key attestation test. RKP-only1
vẫn unresolved; crash Bixby `No matching ComponentResult found!` đã được người
dùng chấp nhận. Evidence, artifact hashes, installer scope, lỗi r1/r2 và bước
tiếp theo: [implementation r3](m4-identity-integrity-implementation-0232.md)
và [plan](m4-identity-integrity-plan-0232.md).

**Lịch sử kiểm tra live sau r3:** ADB khi đó thấy đúng serial `M2_G965F_SERIAL` ở
Android. `sys.boot_completed=1`; public model/name `SM-A376B`/`a37xnaxx` và
`Galaxy A37 5G`; physical model `SM-G965F`; PIF toggle `true`; Setup Wizard
chưa hoàn tất (`user_setup_complete=0`, foreground
`com.sec.android.app.SecSetupWizard/.UI.IntroLanguageSpinnerActivity`).
`system_server` PID2216 và SystemUI PID6238 đang sống. Crash buffer vẫn chỉ có
Bixby donor exception lúc `10-31 14:16:51.756`:
`IllegalStateException: No matching ComponentResult found!`; đây là lỗi đã được
chấp nhận. Chưa có PIF checker request hoặc verdict runtime.

**Bối cảnh trước r3 — checkpoint0232, 31/10/2026:** người dùng
xác nhận app như Shopee/TikTok vẫn tải và dùng bình thường. Dừng port thêm
NFC/Bixby, giữ feature/library/package/API tương thích và không báo unsupported.
Nhận diện công khai phải là Samsung Galaxy A37 5G / SM-A376B; target phần cứng
vẫn star2lte/G965F. PIF hiện bật nhưng log thực có `IllegalAccessException` khi
ghi Build.FINGERPRINT; profile thiếu field và updater cùng version bỏ qua sửa;
keystore RKP-only1 trên stack chỉ có Keymaster4.0 trả
`HARDWARE_TYPE_UNAVAILABLE`. Kế hoạch sửa có file/phép thử/rollback:
[identity và Integrity0232](m4-identity-integrity-plan-0232.md). Chưa sửa nguồn
ROM hoặc ghi điện thoại trong lượt kiểm tra này. Các đề xuất test/port NFC
bên dưới thuộc checkpoint trước, không còn là ưu tiên/chặn của phạm vi mới.

**Lịch sử sau Setup Wizard của attempt0229 — 31/10/2026:** G965F khi đó boot ổn, SELinux Enforcing, không ANR; Auto Blocker tắt/ADB keyless bật; Wi-Fi validated; SIM Viettel LTE in-service; camera chưa chụp; Bluetooth được người dùng chấp nhận PASS. NFC: state3 ON, HAL running; người dùng áp tag đã đọc được trên điện thoại khác ở đáy máy hai lần khi mở khóa mà không có phản ứng. Vị trí đáy chưa xác nhận trùng vùng antenna; cần thử theo vùng antenna phía lưng trước khi kết luận RF hỏng. Log route trước đó có `NFA_EeSetDefaultProtoRouting: Bad ee_handle`. Đính chính: HSI2C NO-ACK ở `14370000.hsi2c` là MFC charger, không phải NFC. GNSS chưa có fix; nhiệt độ là snapshot cảm biến. Xem [NFC test 0231](m4-nfc-functional-test-0231.md) và [post-setup test 0230](m4-post-setup-test-0230.md).

**Trạng thái31/10 — M4 attempt0229:** full 8/8-stage fresh install completed on G965F `M2_G965F_SERIAL`; `sys.boot_completed=1`, SDK37/One UI90000, identity SM-G965F/star2lte, SELinux Enforcing. SYSTEM/VENDOR/ODM/BOOT/CACHE/OMR/HIDDEN hashes and GPT copies were read back; SYSTEM/VENDOR/ODM `e2fsck` passed. ADB keyless/USB defaults are live (`ro.adb.secure=0`, adb enabled, `sec_charging,adb`). SystemServer/SystemUI/Bluetooth stayed stable during the captured samples. Setup Wizard đã hoàn tất; kiểm thử phần cứng cần thao tác người dùng vẫn còn. M4 is not accepted: NFC RF, app networking, SIM/calls, camera, GNSS, thermal and Auto Blocker functional toggle remain open; Bluetooth is accepted by the user. User approved keeping the signed Bixby donor unchanged and accepting its known receiver crash. Attempt0229 flash/runtime evidence and the unexplained EFS hash difference are documented in [artifact review](m4-attempt0229-artifact-review.md). Patch0227 records the successful stale-temp cleanup needed for future stage08 installs; attempt0229 predates that source fix.


**Cập nhật kiểm thử Bluetooth30/10:** người dùng xác nhận màn hình thật lên
Chào mừng và chủ động reboot. Ghép đôi với điện thoại `meizuo`, nhận2ảnh qua
OBEX/OPP PASS;34mẫu/337,03s giữ boot/PID core,0crash/0FDF1timeout; hai file
kết thúc status200, phiên hoàn tất200. A2DP/AVRCP/HFP và reconnect tai nghe/loa
chưa thử. Xem [báo cáo truyền file](m4-bluetooth-phone-opp-validation.md).

**Rà soát tiếp30/10:** live `getprop` xác nhận `ro.product.model=SM-A376B`,
`ro.product.name=a37xnaxx`; nguồn là các giá trị partition-qualified còn sót
trong `/system/build.prop` và `/system/system_ext/etc/build.prop`. Patch0225
đã thêm guard M4/BZI6/G965F để đặt device/model/name S9+ ở hai partition này,
giữ nguyên donor fingerprint Android17, và mở rộng compiled-image gate. Fixture
đúng preimage, scope âm tính, idempotence và verify-work PASS; chưa build ảnh,
chưa kiểm `getprop` sau reboot. Bixby receiver vẫn crash khi thiếu Side Key
menu; patch an toàn chưa chọn vì ký lại APK phá chữ ký Samsung còn ép feature
true khai báo sai phần cứng. ADB hiện boot_completed1, SystemServer start_count1,
Bluetooth ON/0 crash; capture mới giữ lỗi Bixby đơn lẻ.


## Cập nhật30/10 — sửa runtime sau attempt0223

G965F đã hiển thị trang **Chào mừng** qua screenshot ADB và được người dùng xác nhận trên màn hình thật. Ba nhóm sửa đã áp riêng
và kiểm chứng:0221 chặn service OTT thiếu class làm system_server restart;
0222+0223 xử lý DBFW+ trên Bluetooth;0224 nối hook nền tùy chọn của AOD cũ để
SystemUI không crash. Lượt cuối có boot_completed1/server start_count1,
7 mẫu/121,77 giây và5 vòng Asleep→Awake giữ PID core. Mốc uptime423 giây vẫn
cùng server/UI/Bluetooth PID, Bluetooth ON/0crash/không timeout FDF1. Bluetooth
còn qua3 vòng tắt/bật ở lượt thử trước và tự bật đúng sau reboot.

M4 **chưa nghiệm thu tổng thể**. Bixby còn crash do cấu hình Side Key; NFC
shell/dump vẫn lỗi transaction, chưa có kết quả RF; ghép đôi/phát nhạc/HFP với
phụ kiện, networking ứng dụng và phần lớn phần cứng chưa kiểm trong lượt này.
Model/name còn mang donor ở live properties; wireless ADB mặc định bật cần
được audit riêng. Chỉ hủy hộp thoại ủy quyền Wi-Fi, không cấp quyền mạng.

Các sửa đã áp riêng trên máy gồm framework-res, Bluetooth JNI, SystemUI, giữ
USERDATA. Chưa có full ROM mới tích hợp0221–0224, chưa có
image/ZIP gate mới cho composition này. ROM hoàn chỉnh mới nhất vẫn là M4
attempt0223, full ZIP SHA256
`a35d05fc9707930a1f747a10008ede86b62ec88273e35bca5319126ef865e4fa`.
215 patch replay thành160 path byte-identical trong cây nguồn hoạt động (bỏ 7
file `.orig` do patch tạo ở replay); không sửa kernel/Hybrid
Vendor/radio. Hash10 phân vùng bảo vệ khớp trong từng cửa sổ ghi recovery;
không ghi/restore EFS. M1/G965N giữ nguyên.

Báo cáo, bằng chứng, giới hạn và bước tiếp:
[m4-post-0223-runtime-repair.md](m4-post-0223-runtime-repair.md).
Ưu tiên tiếp: hoàn tất replay/build/image gate cho0225, rồi xác định phạm vi
Bixby mà vẫn giữ chữ ký Samsung; kiểm phụ kiện Bluetooth/hoàn tất wizard bằng
thao tác thật; typed NFC/RF và app network. Không ép global Side Key feature
hoặc đổi blob/kernel để che lỗi cấu hình.

Các checkpoint cũ bên dưới là lịch sử.

## Trạng thái mới nhất — M4-attempt0223, 30/10/2026

Attempt0223 đã được chia, kiểm hash/CRC/chữ ký, cài fresh đủ 8/8 stage lên G965F
`M2_G965F_SERIAL`, format USERDATA theo installer và boot tới Android17/SDK37,
One UI90000, SELinux Enforcing, `sys.boot_completed=1`. Không cần repartition.
ADB keyless và mặc định Auto Blocker tắt/USB debugging bật đã được xác minh trên
PC nối hiện tại. M4 **chưa nghiệm thu**: Bluetooth snapshot ghi 5 lần crash ở
mốc đầu rồi tiếp tục có `Disable CRASH`/`Enable RESTARTED` trong 90 giây; log có
`Timed out waiting for Unknown OpCode(0xfdf1)`; FaceWidget ném
`AbstractMethodError`, Bixby ném `IllegalStateException`; NFC dump trả
`FAILED_TRANSACTION`. Setup Wizard vẫn đang mở và chưa chạy phần lớn kiểm thử
phần cứng/mạng. Cần rà model identity: live properties có
`ro.product.model=SM-A376B` và `ro.product.name=a37xnaxx`, dù
`ro.product.device=star2lte` và bootloader là G965F. Báo cáo cùng hash, log và
giới hạn kiểm thử:
[M4 validation after attempt0223](m4-validation-after-0223.md).

Flash có sai khác EFS hash so với prewrite (`2973fbdc…` → `3a45e6a1…`),
nguyên nhân chưa rõ. Không có lệnh ghi/khôi phục EFS; CPEFS/RADIO/RECOVERY/BOTA
khớp. Giữ hai giá trị trong log riêng, không suy ra hỏng hóc từ hash đơn lẻ.
Kernel DS-ACK, Hybrid Vendor và radio bridge không đổi; G965N/M1 không bị tác
động.

Bước tiếp theo là rà model/name theo profile star2lte, rồi phân tích riêng các
lỗi FaceWidget/Bixby/Bluetooth/NFC bằng log và kiểm tra code/ABI trước khi sửa;
không gộp thay đổi kernel/vendor/radio.
Sau khi xử lý startup, tiếp tục kiểm tra Setup Wizard, ADB trên PC mới và công
tắc, rồi kiểm chứng từng khối phần cứng theo log. Không gọi M4 hoàn tất cho tới
khi các gate này có kết quả. Các ghi chú attempt0222 bên dưới chỉ còn là lịch
sử.

Ghi nhận: lý do là cập nhật trạng thái sau flash fresh và runtime check; file bị
ảnh hưởng là `docs/PLAN.md` và `docs/bugs.md` cùng báo cáo
`docs/m4-validation-after-0223.md`; kiểm chứng bằng hash/stage receipt, thuộc
tính boot, mẫu ổn định 90 giây và các dòng log được dẫn trong báo cáo. Không sửa
ROM source hoặc artifact.

Cập nhật29/10 sau **M4-attempt0222 và live follow-up**: G965F tiếp tục chạy
Android17/SDK37, `sys.boot_completed=1`, SystemServer PID2208. Bluetooth vẫn
FAIL: live `dumpsys bluetooth_manager` ghi116 crash; log mới lặp
`Timed out waiting for Unknown OpCode(0xfdf1)`. Disassembly khóa lời gọi enqueue
của BZI6 tại `StateOpened::OnEnter()`; source patch0220 bỏ đúng lời gọi tùy chọn
này, với donor/build/context/output hash khóa, đã qua fixture nhưng chưa được
build hay flash. Bước hiện hành là replay source, build M4 mới và qua audit ảnh/
ZIP trước lượt thử riêng G965F; không đổi kernel/vendor/RIL. VaultKeeper vẫn là
nhánh riêng, không kết luận là nguyên nhân HCI. Xem [runtime report0222](m4-validation-after-0222.md),
[plan Bluetooth](m4-post-0219-repair-plan.md) và run mới
`logs/20261029-m4-g965f-fdf1-followup-0223/`. Network app-level chưa thử vì
chưa có Wi-Fi active.

Cập nhật29/10 sau kiểm thử **M4-0219**: [báo cáo0219](m4-validation-after-0219.md)
và [plan sửa sau0219](m4-post-0219-repair-plan.md) là nền lịch sử cho các gate
còn mở; bằng chứng runtime mới nhất nằm trong báo cáo0222 ở trên. Các số đo
dưới đây chỉ thuộc lượt0219. Build/package0222 và runtime riêng được ghi ở đầu.
Giữ Hybrid Vendor VNDK33/blob F/radio bridge, profile runtime-init-debug và
kernel đã khóa; không có log mới đòi sửa kernel.

18mẫu/180s cùng boot/PID/starttime core,5vòng Dozing→Awake không crash mới,
Settings giữ63/manual0/stay-awake0; final đã trả đúng Settings task15 và Dozing.
Bluetooth FAIL:7PID/6timeout trong cửa sổ live; stack main khóa đường BLE local
key→VaultKeeperManager::read→AServiceManager_getService, chờ~29s. Disassembly
đã tìm candidate selector BZI6 nhưng backend32byte encrypted và16byte config
có nguy cơ silent truncation; unit tiếp theo phải kiểm cả reader/writer format,
không áp bypass một branch. Network normal connect vẫn Message too long;
rawTCP control mới PASS, app HTTP/HTTPS chưa thử được do Chrome FirstRun.

NFC typed getState transaction6 đã verify runtime jar/stub và trả ON; physical
tag/RF/payment còn mở. RTC0218 tiếp tục PASS trong package0219; KG30 absent
lookup vẫn fail-closed. AOD missing callback và Bixby ComponentResult fatal có
trong startup buffer cùng boot, không xuất hiện mới trong live/sleep-wake.
GNSS firstboot null callback crash và provider-loader0217 là các gate riêng.

Thermal audit trước **đã đính chính**: base framework-res không đại diện
resource SDHMS hiệu lực. Product RRO trả maximum867 và bảng306/408/510;
callback k5/s1 phù hợp các cap, nên không còn kết luận chỉ j5/a1.13class/
194compiled method đã đối chiếu PASS với APK built khớp runtime. Exact caller,
gates, calibrated threshold/hysteresis vẫn chưa xác minh; không có nhiệt kế
ngoài nên không ép nhiệt/workload hoặc sửa policy/ngưỡng/kernel.

Bản cài theo hồ sơ0219 full ZIP SHA256
`13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`.
Bằng chứng mới cục bộ: `logs/20261029-m4-validation-0219/run-1655/`.
Xem thêm [thermal audit](m4-thermal-limiter-audit-0219.md),
[RTC](m4-rtc-label-0218.md), [KG30](m4-kg30-nonblocking-0217.md).

Các checkpoint bên dưới là lịch sử.

Cập nhật 28/10 sau **fresh package0215**: đã kiểm 18 mẫu/180s và 5 vòng Dozing→Awake, core PID giữ nguyên, không có crash mới trong cửa sổ sleep/wake. Nhãn backlight/ION đúng nhưng phát hiện lỗi **brightness scale1× so với kernel PAC3 cần100×**, có đối chiếu framework→HAL→bảng panel. Bluetooth cuối lượt38restart với chuỗi chờ VaultKeeper AIDL thiếu; ping IP/DNS vẫn EMSGSIZE, còn raw TCP và6socketpair controls PASS. AOD thiếu callback, KG ANR, Bixby registry và1crash Google Assistant đầu boot vẫn mở. **M4 chưa nghiệm thu.** Kế hoạch hiện hành: [sửa sau0215](m4-post-0215-repair-plan.md); kết quả: [kiểm thử0215](m4-validation-after-0215.md). Lượt này không flash/reboot/wipe hay áp payload patch mới.

Các checkpoint bên dưới là lịch sử; trạng thái mới nhất theo hai tài liệu0215 ở trên.


Cập nhật kiểm thử28/10 sau0212: giữ nguyên ROM; snapshot chi tiết1178.75s và mốc cuối2052.12s vẫn cùng PID core. SystemServer/netd/SurfaceFlinger ổn; SystemUI sau lần crash đầu sống hơn17phút,5 lượt sleep/wake không crash mới; người dùng xác nhận Home thật bình thường. **Chưa nghiệm thu M4**: Bluetooth ít nhất30restart, Bionic network nhận EMSGSIZE từ control socket (rawTCP đối chứng PASS), SDHMS brightness EACCES, AOD thiếu API; NFC dump/Binder chưa kết luận RF, Bixby/KG còn lỗi. Kế hoạch hiện hành: [plan sửa sau0212](m4-post-0212-validation-plan.md); bằng chứng: [validation](m4-validation-after-0212.md). Lượt này chỉ kiểm/lập plan, chưa áp dụng sửa mới.


Cập nhật28/10 —0211/0212: M4 đã boot tới màn hình **Chào mừng**, người dùng xác nhận màn hình thật.0211 đặt resolver BZI6 vào SYSTEM có đúng ABI;0212 khôi phục nhãn chính xác `/dev/ion` cho gralloc của Hybrid Vendor. Sau reboot, nhãn tự đúng, SELinux Enforcing, main SystemServer/netd ổn định thêm120s; không wipe hay đổi kernel/vendor/radio. **Chưa nghiệm thu M4 hoàn chỉnh**: AOD thiếu `disconnectPluginOnBackground()` làm SystemUI restart một lần; Bixby và kiểm phần cứng/mạng còn mở. ROM hoàn chỉnh đang giữ là attempt0209;0211/0212 mới là sửa SYSTEM chẩn đoán trên máy.203 patch/140 file replay byte-identical; chưa có package tích hợp mới. Chi tiết: [runtime repair](m4-postboot-runtime-repair.md).

Các checkpoint tiếp theo là lịch sử; trạng thái mới nhất ở đoạn trên.


Kế hoạch sửa chi tiết sau0198: [m4-post-boot-repair-plan.md](m4-post-boot-repair-plan.md). Phân tích PC song song; trial từng thành phần, thermal qua gate tối thiểu trước phiên UI dài. Đây là kế hoạch mới, chưa triển khai; checkpoint runtime hiện có ở dưới.

Cập nhật28/10 — source0198: M4 đã quan sát được `sys.boot_completed=1`, `bootanim=stopped`, SDK37/OneUI90000 và SELinux Enforcing trên G965F. Fixture Debug JNI/MemInfoReader PASS exit0; GPU map không có trả-1, không còn làm SystemServer abort. SystemServer PID1830 còn ở uptime337.22s. **Chưa nghiệm thu giao diện/phần cứng**: SystemUI/FaceWidget sai luồng main, SDHMS lỗi enumDEX và NFC thiếu libnfc-nci.so. Đã thu189 file và hoàn tất rollback SYSTEM/BOOT/VENDOR/netstats, đối chiếu sau ghi PASS; máy đang ở TWRP.188 patch/132 file replay PASS,187 hash cũ giữ nguyên; chưa có ROM hoàn chỉnh mới tích hợp các thử nghiệm195–198. Chi tiết: [meminfo runtime](m4-meminfo-runtime.md), [lỗi sau boot](m4-post-boot-blockers.md).

Cập nhật runtime 28/10/2026 — source0197: đã port nhóm query stats và bộ transport/identity/template/Parcel/lưu v7-v8 từ BZI6, giữ entry point S22 cũ và Binder service.2161 lớp round-trip,385 tham chiếu services và36 phụ thuộc mới đã rà; APEX dựng lặp cùng hash,187 patch/130 file replay PASS. Fixture trên G965F đã PASS exit0; SystemServer sau đó native SIGABRT trong libmeminfo (chi tiết0198 ở trên). Đã thu136 file và hoàn tất rollback SYSTEM/BOOT/VENDOR cùng hai thư mục netstats. boot_completed chưa đạt. Chi tiết: [network stats runtime](m4-network-stats-runtime.md).

**M4 đã build và đóng gói hoàn chỉnh —27/10/2026, attempt37/source0188.** Full ZIP đã ký (5,192,379,395 bytes) và target-files đã qua đối chiếu SHA256;8 gói ZIP32 đã qua CRC/chữ ký/giới hạn ZIP32 và có hash trong manifest; run `star2lte-1793025724-rBdwLO` có `.complete`. Toàn bộ kiểm compiled, PIF, ADB/Auto Blocker, SELinux và image ext4 thực tế đã PASS. Chuỗi178 patch replay thành125 file giống hệt từ fork đã khóa. **M4 đã fresh-flash G965F, khởi tạo được SystemServer nhưng chưa boot hoàn tất; đang sửa tương thích API mạng. Verdict Play Integrity trực tiếp chưa kiểm.**

Đã giữ đúng một bản ROM hoàn chỉnh cuối cho mỗi mốc M1/M2/M4 trên NVMe phụ;3 lượt package M4 dở và4 replay fixture đã xóa sau kiểm chứng. SSD chính còn khoảng190.5 GiB và NVMe phụ161.4 GiB trống. Người dùng ưu tiên PIF; Paradigm, UI Setup Wizard và Misc Tweaks giữ hành vi donor. Xem [artifact review](m4-attempt37-artifact-review.md), [lưu trữ](storage-cleanup-20261026.md). Các checkpoint dưới đây là lịch sử.

Lưu trữ cập nhật 26/10/2026: theo chỉ thị mới nhất, chỉ giữ bản ROM hoàn chỉnh
mới nhất của mỗi mốc M1/M2/M4; các archive/build worktree cũ có thể xóa sau khi
giữ log. Chi tiết và hash artifact hiện giữ: [release-retention.md](release-retention.md)
và [storage-cleanup-20261026.md](storage-cleanup-20261026.md). M4 chưa có build
hoàn chỉnh; attempt36 dừng ở KnoxPatch; tail preflight đã sửa qua0168, chuẩn bị attempt37.

M4 implementation started 25/10/2026. Attempts04–19 and patches0135–0148 are recorded below. Attempt20 confirmed the Strongbox, Midas, and PhotoEditor adapters, then stopped because BZI6 has no legacy smartcropping model path. Patch0149 stages the hash-pinned BZI6 system ImageCropper model and preserves the star2lte vendor SCE baseline; a clean attempt21 is next. Attempts13/14 were interrupted without a build result. No complete M4 image, flash or Android17 runtime proof exists. Full status and open gates: [docs/m4-plan.md](m4-plan.md).

M4 implementation started 25/10/2026; execution status and gates: [docs/m4-plan.md](m4-plan.md).
M4-0 and BZI6 extraction/cache isolation are verified. Patches0109–0128 add
policy compatibility, runtime SIM detection, guarded optional camera claims,
and source-tested BZI6 mDNIe/APE/SoftAP adaptations. Patch0112 corrects the SIM
hash to the fork's actual `--no-debug-info` decode mode. Both A37 donor versions
pass the feature-adapter fixtures and DEX assembly; final images are pending.
Patches0116–0118 close source WLAN policy, signature context and split-lights
dispatch gaps. Current reviewed/open gates: [m4-dispatch-ledger.md](m4-dispatch-ledger.md).
Patches0119–0120 adapt rear fingerprint and the BZI6 OMX/media source path;
their DEX/native fixtures pass, including a compiled media release gate.
Patches0121–0124 adapt BZI6 shipping/HDR/HFR/brightness and an explicitly
scoped M4 fixed-resolution branch. Composed source/DEX fixtures and the new
compiled display gate pass;114 patches replay to95 byte-identical files.
Patches0125–0128 add the separate BZI6 donor registry and reviewed eSE/initial
Face/Bixby/Settings/Wi-Fi UI selections. Source/config/compiled fixture gates
pass;118 patches replay to101 byte-identical files,114 earlier hashes unchanged.
Patch0129 isolates the BZI6 native32 lock/cache and stages it through the existing
fork restore/release/ext4 verifiers. APEX and real ext4 fixtures pass; actual M2
staging remains byte-identical.119 patches replay to102 matching text files,
118 previous hashes unchanged. Complete runtime/profile, A17 camera, remaining
module/mod and boot/vendor/image contracts are still open. Evidence:
[m4-native32-runtime.md](m4-native32-runtime.md) and
[m4-source-selection.md](m4-source-selection.md).
Evidence: [m4-product-display.md](m4-product-display.md).
BZI6 secilc compiled the adapted split policy on the running DS-ACK M2 device;
neverallow/live policy load and remaining module/profile audits are pending.
No Android17 build/flash has run. M2 attempt56 remains the baseline; historical notes below do not override
AGENTS.md.

Cập nhật 20/10/2026 theo chỉ thị trực tiếp mới nhất của người dùng.
Các kế hoạch ở thư mục gốc là tài liệu lịch sử; các mục stock-strict,
tự build kernel bắt buộc, tái merge ACK và build system mới trong đó đã bị thay thế.

## Các quyết định đã khóa

- Đích chính SM-G965N; đích phụ SM-G965F, star2lte.
- Mở rộng `Eend15/Unofficial-Un1ca-9810`, nhánh `Android-16`, checkout `rom/`.
- Bắt buộc Hybrid Vendor VNDK33 của fork, ghép blob phần cứng/RIL gốc G965N
  cho N; F phải có nguồn blob đúng biến thể và kiểm chứng riêng.
- Người dùng đã cho phép giữ cầu nối radio của fork khi cần. Không bắt buộc
  thay cả họ radio tương thích bằng toàn bộ RIL Android 10 của N. Kiểm kê nguồn
  từng file; ghép blob/cấu hình N đúng phần cứng, kiểm tra ABI và log modem/IMS.
- Giữ modem/CP, bootloader và firmware bảo mật đúng từng máy. Không ghi EFS/CPEFS.
- Dùng gói DS-ACK có sẵn của duhansysl qua installer của fork. Chỉ sửa/build
  kernel khi log chỉ ra nhu cầu, ghi bằng chứng và giới hạn thay đổi.
- Người dùng đã chọn gói duhansysl V1.12 08/05/2026 Enforcing-OneUI7 không
  KernelSU; dùng profile M1 F riêng, không tự ghép manager/runtime KernelSU.
- Mục tiêu cuối là framework A376B One UI 9/Android 17 thật; xác minh SDK,
  framework/APEX từ firmware. Tên ZIP và ảnh Settings chưa đủ bằng chứng.
- Ngoại lệ trực tiếp 20/10/2026: sau khi xác nhận USERDATA backup/không cần giữ
  và chấp nhận repartition/xóa data, người dùng yêu cầu “mày tự flash rồi đọc
  log nhé”, rồi “tiếp tục”. Cho phép Codex thực hiện M1 trên G965F theo serial
  đã đối chiếu, gồm cài/repartition/format theo installer, reboot và thu log.
  G965N giữ đối chiếu; ngoại lệ này không cấp quyền flash N hoặc mốc sau.
- Quy tắc lâu dài do user chốt: mọi bản F/N, M1/M2/M4/M5 và mọi donor phải
  mặc định Auto Blocker tắt, USB debugging bật; ADB USB không hỏi RSA key,
  PC mới cũng nhận. Công tắc/mô tả giữ đúng hành vi, tắt/bật chủ động có tác dụng;
  seed một lần khi settings mới, không ép lại mỗi boot. Verifier compiled phải
  kiểm rule; donor thay cấu trúc thì dừng rà soát. Không mặc định mở TCP ADB.
- User yêu cầu “flash và test lại”: cho phép lượt M1F patch0017 (flash-02),
  fresh-data theo installer, reboot/thu log và test công tắc/ADB không RSA.
  Vẫn chỉ F; N và milestone donor khác không nằm trong lượt này.
- Ngày21/10/2026 user yêu cầu “flash fresh lại” vì lượt trước giữ data cũ.
  Cho phép flash-03 trên đúng G965F: cùng gói M1 patch0017, Format Data bằng
  TWRP gồm bộ nhớ trong, kiểm UUID mới và Settings/dấu kiểm tra cũ đã mất,
  rồi boot/thu mặc định trước sửa setting. N giữ đối chiếu.
- Ngày22/10/2026 user trực tiếp yêu cầu Codex flash fresh M2 và thu log trên
  G965F; xác nhận USERDATA không cần giữ. Cho phép duy nhất package M2 có SHA256
  `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc` trên
  serial `M2_G965F_SERIAL` đã xác minh là G965F. Không áp dụng cho N/mốc khác.
- Ngoài các lượt M1 F và M2 F được ghi đích danh trên, không tự
  flash/format/repartition/reboot.

## Các mốc

| Mốc | Nội dung | Điều kiện chuyển mốc |
| --- | --- | --- |
| M0 | Audit fork, pin nguồn, xác minh recovery/layout, nguồn blob F/N, backup và đường khôi phục | Đủ đầu vào, patch review được, installer đã audit |
| M1 | Đối chứng S22/Android 16 + Hybrid Vendor + DS-ACK có sẵn | Người dùng xác nhận boot với log đúng từng build/model |
| M2 | A37/Android 16, giữ kernel và lớp vendor đã kiểm chứng | Người dùng xác nhận boot bằng log |
| M4 | A37/Android 17, giữ kernel và lớp vendor, chỉ thay phiên bản Android | Người dùng xác nhận boot bằng log |
| M5 | Hoàn thiện phần cứng, SELinux và nghiệm thu riêng F/N | Ma trận chức năng có bằng chứng |

M3 cũ (thay bằng kernel tự build) không còn bắt buộc. Nếu log chứng minh cần sửa
kernel, tạo mốc K có tiêu chí riêng và chỉ thay một biến, rồi quay lại mốc ROM.
Không bỏ M1/M2 chỉ vì đã có firmware Android 17; được kiểm kê firmware độc lập.
Không suy từ boot F sang boot N. Người dùng đã chọn thử F trước, giữ N đối chiếu;
đã xác nhận backup cả hai máy lưu ngoài điện thoại (chưa audit nội dung backup).

## Kết quả M1 F thực tế

Đã cài8/8 phần legacy, repartition/format theo installer và boot thành công.
User xác nhận “đã boot thành công”; runtime evidence ở docs/m1-boot-report.md.
Gate boot M1F đạt; nghiệm thu chức năng, USB/ADB và policy release còn mở.
Trước boot, EFS đổi raw hash; user được báo và yêu cầu “tiếp tục đi, máy dev thì
backup làm gì”. Đã ghi ngoại lệ efs-boot-exception.json và tiếp tục boot;
preboot-audit.json vẫn FAIL vì chưa xác minh EFS. Không restore EFS, N không đổi.
Không suy từ M1F sang N/A37; chưa flash milestone tiếp theo.

## Việc đang được phép thực hiện trên PC

Đọc/clone fork, kiểm kê ZIP và hash đầu vào, đọc thông tin recovery theo serial,
ghi repo-map, source lock, phân tích vendor/RIL và installer, chuẩn bị patch nhỏ
có lý do/file/cách kiểm chứng/rollback. Không tạo build system khác.
Không chạy `buildenv.sh`, recipe, installer hay mã vừa tải trước khi đọc các bước
có thể ghi thiết bị hoặc tự tải/chọn nguồn. Không cài toolchain kernel mặc định.

## Phân biệt yêu cầu và bằng chứng

Hybrid Vendor VNDK33 là kiến trúc người dùng đã chọn. Android 17 chấp nhận từng
HAL/VNDK hay không: **cần xác minh bằng log** và kiểm tra tĩnh từ firmware thực.
Chưa có dump/file/log của máy G960N trong ảnh để xác định vendor của máy đó.

## Tài liệu thực thi

- `docs/repo-map.md`: vị trí và biến đã đọc từ fork.
- `docs/sources.md`: provenance, commit/hash, đầu vào còn thiếu.
- `docs/hw-diff.md`: khác biệt F/N, donor/target và kiểm chứng cần làm.
- `docs/patch-matrix.md`: mỗi patch có lý do, file, kiểm chứng, rollback.
- `docs/bugs.md`: blocker có dòng bằng chứng; không tạo boot bug giả.
- `docs/log-protocol.md`: thu log theo serial/model/build.
- `STATUS.md`: kết quả thực chạy và bước tiếp theo.


## Kết quả lượt fresh 21/10/2026

Flash-03 cùng artifact patch0017, Format Data explicit có UUID/absence proof,
preboot PASS, fresh boot/reboot/defaults/keyless PASS trên F. Auto Blocker UI
bật/tắt có tác dụng; USB debugging provider callback PASS, GUI còn chờ setup.
Source có patch0018 sửa guard format, chưa package/flash patch đó. Giữ Welcome,
không chuyển donor/milestone hoặc N. Báo cáo docs/m1-fresh-report.md.

## Chuyển sang chuẩn bị M2 — 21/10/2026

Người dùng yêu cầu giữ gói ROM hoàn chỉnh M1/M2/M4/M5 không ghi đè và build M2.
M1 ADB17 đã copy/archive độc lập đủ10 ZIP, SHA/size nguồn-đích PASS; source hiện
19 patch/30 file, patch0018/0019 chưa nằm trong ROM M1 đang chạy. Patch0019 mở
rộng packaging của fork: unique run, exclusive publish, không wildcard xóa ZIP.
Xem docs/release-retention.md và manifest logs/20261021-release-retention/.

M2 đang chờ Full Files A376BXXS5AZH4/XXV Android16; user đã nhận tải và báo lại.
SamFW ghi One UI8.5, phải xác minh giá trị thực trước khi tạo config, không gọi
đây là 8.0 khi chưa đọc payload. Chưa có build/profile A376B16 đã xác minh hoặc
ROM M2 hoàn chỉnh. Giữ S22 config, kernel, Hybrid Vendor và radio contract;
chỉ thêm donor song song theo docs/m2-build.md. M4 tiếp tục dùng A37Android17
sau gate M2, không lấy Android17 thay đầu vào M2. Phạm vi hiện tại PC build,
chưa được phép flash M2 hoặc N.

## M2 đã có đầu vào — 21/10/2026

User tải xong Downloads; full/Odin/extraction được xác minh, actual Android16
SDK36.1/OneUI80500. Đã tạo profile/config donor song song, cache khóa donor;
build sạch M2 F đang chạy qua fork. Xem docs/m2-build.md và log attempt1.
M1 archive giữ nguyên; chưa có ZIP M2 hoàn chỉnh hoặc thử M2 trên điện thoại.

## M2 build iteration — 21/10/2026

A376B Android16 AZH4 inputs are verified and extracted. Clean fork builds have reached camera modules; attempts1–4 exposed donor ABI/optional feature mismatches, each recorded in docs/bugs.md and patched with scoped guards. Local patch series now 27; replay verifies 35 files. Attempt5 is running. No M2 ZIP or M2 boot acceptance yet. Keep M1 artifact archive untouched and do not flash M2.


## M2 fixed resolution selected — 21/10/2026

User selected temporary fixed resolution for the M2 boot test. Patch0041 sets the existing dynamic-resolution target flag false only for the M2/A376B Android16 profile; no generic resolution/Settings/SystemUI switcher patches run in that profile. The fork's native star2lte baseline is 1440x2960 with ro.sf.lcd_density=560 and ro.sf.init.lcd_density=560; it remains selected without forced-size boot scripts. Clear the inherited WQHD/FHD/HD feature claim early and in both final system/vendor floating_feature.xml files; final audit rejects a nonempty switcher claim. The donor Settings screen-resolution controller remains unavailable; actual panel resolution/visibility requires runtime verification.

Scope: target/profile files and existing device-stack feature edits/audit only. M1 remains dynamic; wrong-donor/milestone fixed mode is rejected. Config fixtures pass M1=true, M2=false and invalid profile rejection. All 41 source patches replay to 40 byte-identical files; shell syntax passes. Mandatory ADB defaults/compiled code checks stay required. Next clean build23 with full ROM packaging requested through the existing fork; no device actions.


## M2 donor Wi-Fi UI selected — 21/10/2026

User explicitly selected temporary donor Wi-Fi UI for M2. Patch0049 adds `STAR2LTE_M2_KEEP_DONOR_WIFI_UI=true` in M2 and resets it false in M1; the existing product_feature recipe validates M2/A376B16/star2lte/Wi-Fi5 scope and skips only the two old 80211AX UI diffs for Settings/SystemUI. It leaves source metadata, target Wi-Fi flags, driver/HAL/blob, custom-backoff adaptation and all other Wi-Fi recipe branches intact. Donor scan labels/icons may still show Wi-Fi6; that is a known temporary UI limitation, not S9+ hardware support. No extra UI bypasses are authorized by this selection.

Actual branch fixtures PASS selected0patch calls / strict2calls / wrong-donor and wrong-milestone rejection; M2-to-M1 flag reset PASS. All49 audit patches replay to44 byte-identical source files. Clean attempt24 is running through `make_rom.sh --clean-work-dir --build-rom-zip`, log `logs/20261021-m2-build/attempt24.txt`. Full release gates, retained run packaging and M2 runtime remain pending. No phone actions; pinned kernel/Hybrid Vendor/radio unchanged.

## M2 scope confirmed — 22/10/2026

User selected A37 arm64-only application ABI for M2; retain native32 compatibility required by vendor audio/camera/OMX. Do not add app_process32/ART32 or advertise 32-bit app support. User also confirmed retaining donor behavior and hiding the five unsupported optional Settings controls (Google Photos spoof, secure windows bypass, app downgrade, target SDK bypass, capture detection bypass). Patch0075 scope is now confirmed.

22/10/2026: user confirmed M2 omits Bixby remapping, hides unsupported selectors and retains original keylayout. Restore703 WINK (G965F stock Generic and AZH4 Samsung vendor layout), preserve other donors; require compiled selector absence and original layout audit. No M2 flash authorization.


22/10/2026 — user explicitly requested “mày thử flash fresh lại rồi thu log xem”. This authorizes a repeat fresh install/test of the same retained M2 package SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc` on verified G965F serial `M2_G965F_SERIAL` / G965FXXUHFVG6 only, including package-defined fresh USERDATA format, system reboot and log collection. Eight previously verified ZIP32 stages are used, final stage from RAM. Record: `logs/20261022-m2-g965f-flash-02/`. G965N and other packages are outside scope. Repeat installation is in progress; this entry does not claim success.


22/10/2026 — current direct user authorization supersedes hash-scoped flash restrictions: “ok cài đi update agents.md cho mày toàn quyền flash”. Codex may execute project dev-device flash/fresh-format/repartition per audited installer/reboot/log/rollback, including new diagnostic artifacts, without asking again merely for a new hash. Serial/model/hash/partition-scope verification and logs remain required. Existing F-first/N-reference milestone plan and protected EFS/CPEFS/modem/bootloader rules remain. Current action: install approved G965F diagnostic BOOT SHA a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9, boot once, capture and restore baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d.

22/10/2026 — M2 early diagnostic test01, PID1 probe02, pmsg probe04, and CACHE probe05 each completed on G965F, with postboot capture and baseline BOOT restoration verified by full-partition SHA. Probe04 and probe05 persistence checks passed in TWRP, but no PID1 runtime marker survived their M2 bootloop tests. `last-kmsg` only confirms bootloader handoff (`Starting kernel...`); it has no M2 kernel/init fatal line. G965F is in TWRP with baseline BOOT `cc618abc...776ed01d`, G965N untouched. M2 boot gate remains failed; no kernel edits justified. Detailed evidence: `docs/m2-build.md`, `docs/bugs.md`, `logs/20261022-m2-early-diag/`.

24/10/2026 — M2 camera/persistent metadata source fixes completed and live-tested.
Patch0099 retains the A37 native cameraserver ABI and ports only the existing
8-byte max_buffers=0 fallback; exact80500 is recognized by the retained Camera
APK using its existing V8_0 feature set. Patch0100 retains the fork's existing
CACHE metadata bind after final vendor restore, mirrors20 metadata label aliases,
and describes the flagless Hybrid Vendor with a validated empty aconfig v2
container. Platform aconfigd now exits0; RANGING remains dangerous with the
original donor flag. Patch0101 adds the missing pinned system32 GLES1 wrapper
required by vendor gralloc. No audio-stack or vendor hardware/radio replacement.

Actual G965F tests: Enforcing/boot_completed1, marker inode/hash preserved across
reboot, stable cameraserver/SystemServer through uptime397.34s, rear/front preview
buffers and decoded JPEG4032x3024 /2640x1980. Camera optional vendor-tag warnings
remain; video/audio/telephoto and full hardware acceptance are open. All temporary
SYSTEM/VENDOR edits, generated files and BOOT were rolled back with hash checks;
CACHE metadata fixture backed up (215 hashes verified) and removed, restoring
its original absence. BOOT cc618abc...776ed01d; M2 stays in TWRP. No USERDATA format,
M1/G965N or protected-partition write issued. Full report:
`docs/m2-camera-metadata-fixes.md`. Source replay91patches/72paths PASS, hash locks
updated. No new complete ROM build/package or M4 transition. Diagnostic kernel,
Tethering APEX and graphics V6 controls still need explicit build integration.

24/10/2026 — user approved the proposed next integrated M2 diagnostic build/fresh test.
Added patch0102: independent G965F runtime profile with hash-pinned WIPEONFORK
archive, retained S22 Tethering/V6 inputs, source cache identity and final compiled
checks. Patch0103 extends existing actual-ext4 verification for new runtime and
mandatory ADB artifacts. Production profiles reset the opt-in; no kernel build,
version-string change or N operation. Recovery-only installer calibration writes
no BOOT and predicts55f3af8c...669fe77 with original ramdisk/DTB/cmdline.
Clean attempt51 is in progress through the existing fork. No package/fresh boot
success yet. Provenance, scope, gates and outcomes: docs/m2-integrated-runtime-build.md;
logs/20261024-m2-integrated-build. Existing complete releases are retained.


24/10/2026 — Attempt52 fresh G965F M2 runtime profile reaches Android second-stage/SystemServer but fails before boot completion: logcat line74018 reports missing `android.permission.RANGING`; init line4590/6042 shows aconfig storage creation failing on read-only `/metadata`. BOOT rolled back to baseline and protected partitions match. Patch0104 enables only the previously proven volatile 16MiB metadata tmpfs in the opt-in G965F M2 diagnostic. Attempt53 is rebuilding through the fork; no acceptance or M4/G965N transition until fresh runtime evidence.

24/10/2026 — Attempt53 built after patch0104 and, after one-time user authorization for this exact package, was fresh-installed on G965F `M2_G965F_SERIAL`. All eight stages RC=0, fresh USERDATA verified, protected partition hashes unchanged, BOOT equals the diagnostic image. The first fresh boot reached `sys.boot_completed=1` at 196.4s. `/metadata` tmpfs/aconfig initialized; `RANGING` is dangerous and RangingService registered; SystemServer stayed alive, Enforcing. Fresh ADB settings are enabled and `ro.adb.secure=0`; Rampart logs `disabledRampart=true`. The expected patch0095 CACHE logger files are absent from installed VENDOR and `/cache/unica` is missing, so diagnose that build/profile integration separately. This is a diagnostic G965F Android16 boot, not full acceptance; user-toggle lifecycle, hardware functions, persistent metadata, G965N and M4/Android17 remain unproven. Do not reuse the one-time package write authorization.


## Runtime gate update — 25/10/2026

Attempt54 is now fresh-installed on G965F and boots twice to completion under
Enforcing. A controlled marker confirms `/metadata` backed by `/dev/block/sda21`
survives one reboot. Fresh ADB/Rampart defaults pass. This is progress on M2
Android16 only; do not advance to M4/Android17 or G965N. `gpuMem.bpf` and
`gpuWork.bpf` still return `Invalid argument`; child zygote attempts for
`com.samsung.android.kgclient` abort because no matching SELinux seapp context
exists, and the Bixby agent has a separate `No matching ComponentResult found!`
exception. SystemServer remained up in the observed window. The attempt54 build
did not enable `EXYNOS9810_ENABLE_INIT_DEBUG`, so patch0095's CACHE logger was
not tested and `/cache/unica` is absent. Next steps: integrate the logger only
through a reviewed profile and verify final VENDOR contents; investigate the
BPF/kernel interface and missing `kgclient` seapp mapping from logs; then extend
hardware and stability tests on F before any N trial. Evidence and exact log
locations are in `docs/m2-integrated-runtime-build.md`.


## 25 October 2026 — attempt55 completed, CACHE logger partially verified

Fresh install passed8/8 signed ZIP32 stages; USERDATA changed and protected
partition hashes stayed unchanged. Corrected patch0106 profile/config and
actual final/installed VENDOR logger metadata checks passed. Fresh/reboot
completed in180.7s/79.7s under Enforcing. A temporary CACHE-backed /metadata
marker survived reboot and was removed. Fresh ADB/debugging are enabled/keyless,
Rampart off. First-boot SystemServer stayed one PID/start through uptime662.79s;
second-boot sampled PID/start stayed unchanged. This is diagnostic M2 evidence,
not full hardware or long-run stability acceptance, nor M4/G965N readiness.

/cache/unica now contains post-fs/post-fs-data/completed markers and nonempty
start/15s/60s logcat on both boots. props/dmesg snapshots remain empty under the
init service context, reproduced after late restart and read-only runcon;
root ADB captures work. That snapshot gap has no proven root cause yet.
BPF remains `gpuMem.bpf: Invalid argument (os error 22)` / gpuWork.bpf
(boot2/logcat-all.txt:7879,7887); kgclient has `No match for app with uid 5959`
with `username oem_5959` (:152556) and child `Fatal signal 6 (SIGABRT)` (:153275).
Bixby still throws `No matching ComponentResult found!` (:156141).
Raw paths are under logs/20261025-m2-attempt55. Do not claim these issues fixed.

The next scoped investigations are vendor_kg account-name integration (existing
seapp rule survives) and BPF program-type/tracepoint/helper/map ABI compatibility
(minimal TRACEPOINT/PERF_EVENT tests return EINVAL). No broad SELinux exception
or speculative production kernel change is justified. Complete artifact/hash,
files/reasons/checks and limits: docs/m2-integrated-runtime-build.md.


25/10/2026 — attempt56 completed the narrow `vendor_kg` account repair. Clean
existing-fork compile/package, actual ext4 VENDOR audit and all eight signed
ZIP32 stages passed. Fresh G965F install changed the USERDATA UUID and kept
EFS/CPEFS/RADIO/RECOVERY/BOTA raw hashes unchanged. Two Enforcing boots reached
`sys.boot_completed=1`; `id 5959` resolves to vendor_kg, the live package runs
under `u:r:kg_app`, and both logs contain zero `No match for app with uid 5959`
and zero zygote `Fatal signal 6 (SIGABRT)`. CACHE logger markers/logcat pass;
init-context dmesg/props remain empty. This confirms the account fix only.

Both boots also reproduce a distinct KnoxGuard TA problem: `TLC_ERROR`,
`@TimeoutException: getTAInfo - null`, then a background ANR in
`LockedBootCompletedReceiver` (attempt56 boot1 logcat lines253262/253958;
boot2 lines195922/195946; trace2 lines90-94 blocks at
`IKnoxGuardManager.getTAInfo`). `system_server` remains at start_count1 and the
device stays booted. The next M2 gate is a separate, evidence-led diagnosis of
that KnoxGuard TA/backend path; do not undo patch0107 or alter radio/kernel to
mask it. BPF `EINVAL`, Bixby and remaining hardware/stability tests remain
independent. Attempt55 remains retained; M4/G965N are not accepted. Full build,
install, hashes, limitations and patch provenance: docs/m2-integrated-runtime-build.md.
