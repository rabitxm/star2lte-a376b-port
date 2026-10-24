# Quy tắc ADB / Auto Blocker cho mọi bản sau

User yêu cầu ngày20/10/2026 và yêu cầu lưu thành rule lâu dài. Áp dụng mọi donor,
M1/M2/M4/M5, SM-G965F và SM-G965N; nằm trong AGENTS.md và docs/PLAN.md.

## Hành vi đã khóa

- Settings mới: Auto Blocker OFF; USB debugging ON, Developer options ON để UI
  phản ánh trạng thái. ADB USB không hỏi RSA key/ủy quyền PC.
- Canonical và các bản property import đang tồn tại thống nhất ro.debuggable1,
  ro.adb.secure0, persist.sys.usb.config=sec_charging,adb. Đây là chính sách
  debug user yêu cầu, áp dụng cả build release của dự án; không mặc định mở TCP ADB.
- Công tắc và phần giải thích gốc giữ hoạt động. Khi chủ động tắt USB debugging
  hoặc bật Auto Blocker, hành vi phải đổi thật; không giấu công tắc, giả summary,
  ép luôn-on qua service/loop hoặc ghi đè Settings mỗi boot.
- Khi đã có lựa chọn lưu, phải giữ lựa chọn đó. Khi đổi donor có cấu trúc code
  khác, helper/verifier phải dừng để rà soát thay vì im lặng bỏ rule.

## Lý do và file ảnh hưởng

Patch0017 mở rộng module finalization và verifier hiện có trong fork:

1. `rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`:
   cuối các overlay/mods decode SettingsProvider/services qua hàm fork rồi gọi helper.
2. Cùng thư mục, `adb_defaults.py`: chèn seed vào ba hàm loadGlobal/System/SecureSettings
   của SettingsProvider. SQL INSERT OR IGNORE chỉ điền key thiếu khi tạo settings,
   giữ nguyên dữ liệu và lựa chọn đã có. Global adb_enabled1/development_settings_enabled1;
   System rampart_suw_main_on0; Secure main/strict/blocked_adb/blocked_usb_data0.
   AdbService Lifecycle đọc adb_enabled đã lưu trước khi khởi tạo trạng thái;
   USB property chỉ là fallback. Chuẩn hóa các property import có xung đột.
3. `rom/scripts/internal/verify_exynos9810_release_defaults.py`: kiểm actual compiled
   DEX và properties cho cả cached/package-staged build; không chỉ tìm string ở source.

Không sửa Rampart/SecSettings/SystemUI, UI descriptions, receiver, observer hay
luồng bật/tắt/Knox của SettingsProvider. Không build/sửa kernel, không viết build
system mới; dùng đúng script apktool và assembler bridge hiện có của fork.

## Nguồn đã đọc và lựa chọn USB

- `unica/patches/adb/customize.sh` chỉ tắt RSA auth khi DEBUG; chưa phải rule release.
- device_stack có props debug/USB sớm nhưng final overlay có thể phục hồi props vendor.
  Module mới chạy cuối và verifier kiểm các import thực.
- Rampart manifest giữ metadata `system,rampart_suw_main_on,1,0` ở hai alias:
  1 và0 là giá trị khi tick/bỏ tick. Không đổi thành0,0 vì sẽ làm hỏng công tắc.
- SUW `B2/f.e` đọc key system cho giá trị checkbox; default fallback là giá trị
  bật B=1. Seed system0 khiến ô mặc định bỏ chọn mà vẫn ghi1 nếu user chọn bật.
  Rampart `Z0/a.a` đọc System key này khi nhận setup event; native Settings
  và Rampart vẫn nhận lựa chọn thật. Inspection decode chỉ ở inputs/adb-defaults,
  đã đưa khỏi queue APK rebuild để giữ nguyên chữ ký/payload Rampart.
- Log M1 boot-completed-01/logcat.txt:15012: `init: Command 'write
  /config/usb_gadget/g1/UDC ${sys.usb.controller}' ... failed: ... No such device`
  với mtp,adb; :18460 `symlink() failed: File exists`.
- Cùng log :39219 `UsbDeviceManager: Set Persist USB Config to sec_charging,adb
  for USB Security`; :126663 broadcast có adb=true và configured=true.
  Chọn tổ hợp charging+adb đã cấu hình thành công làm default sớm. Đây là
  giả thuyết cải thiện boot USB cần xác minh trên candidate, chưa kết luận root
  cause hay báo đã sửa dứt điểm USB. Giao diện chọn truyền file vẫn nguyên.
- Live baseline cũ trước patch đã ro.adb.secure0, adb_enabled1, Auto Blocker0
  sau khi máy boot; chưa chứng minh default clean-data ở mọi bước setup.

## Kiểm chứng đã chạy trên PC

- Source idempotence và từ chối donor lạ PASS. SQLite thử data mới nhận giá trị
  đúng, đổi lựa chọn rồi seed lại giữ lựa chọn PASS cho cả ba bảng.
- Rebuild SettingsProvider APK ký platform qua scripts/apktool.sh; services.jar
  qua chính script đó, rồi giữ bridge classes3 bằng assembler fork có sẵn.
  classes2.dex và bridge classes3.dex giữ đúng byte; framework15715 class PASS.
- Actual compiled verifier disassemble hai class, kiểm SQL/seed calls, thứ tự đọc
  setting trong lifecycle và no-auth/USB properties PASS.
- Negative fixtures: vendor ro.adb.secure1, SettingsProvider cũ thiếu seed,
  services cũ thiếu lifecycle đều bị verifier chặn PASS.
- Verifier release của fork PASS: source-matched face bridge, profile không KSU,
  Infinity/Bixby/PIF và compiled ADB rule. Replay17 patch/28 file byte-identical.
- Lần verifier đầu báo `RuntimeError: Missing compiled default: adb_enabled`;
  kiểm disassembly cho thấy baksmali escape single quote thành backslash-quote.
  Đã sửa parser normalize escape, giữ nguyên SQL bytecode, rerun PASS.
- Khi chạy riêng rebuild services trước assembler, release check chặn
  `KeyError: "There is no item named 'classes3.dex' in the archive"` đúng dự kiến
  thứ tự pipeline. Đã dùng assembler fork giữ lại bridge source-verified, rerun PASS;
  không bỏ verifier hoặc tự chép framework donor khác.

Evidence ở logs/20261020-adb-defaults: source-fixtures.json,
negative-compiled-fixtures.json, compile-*.txt, compiled-verifier-02.txt,
release-defaults-03.txt, patch-replay-check.json, compiled-manifest.json.
APK/JAR/decoded proprietary code chỉ nằm inputs/ và rom/out/ bỏ Git.

## Runtime fresh đã kiểm 21/10/2026

M1 G965F đã flash và Format Data có UUID/absence proof, boot + reboot PASS.
Bản ghi trước sửa Settings có provisioned0/setup0, adb/development1,
Rampart master/strict/blocked0, SUW0. Raw USB không key cả hai lần trả CNXN
không AUTH, OPEN shell PASS. Auto Blocker GUI Off và bật/tắt làm adbd dừng/chạy
thật; Settings callback USB debugging cũng có tác dụng. USB debugging GUI chưa
nghiệm thu vì máy giữ Welcome, không sửa provisioned/setup để mở màn đó.

Lượt flash-02 có data cũ nên không dùng làm bằng chứng fresh. Đã tìm lỗi
mke2fs bị /sdcard alias giữ mount trong khi installer cho data optional.
Flash-03 dùng ZIP0017 giữ byte + explicit TWRP Format Data sau tháo aliases.
Patch0018 sửa guard installer/verifier cho package sau; chưa flash code0018.
Các kiểm runtime trên N/donor mới và USB sớm còn mở. Không suy F sang N/A37.
Chi tiết/evidence: [m1-fresh-report.md](m1-fresh-report.md).

Rollback: reverse patch0017, restore/rebuild outputs từ baseline được giữ ngoài
Git, chạy lại verifiers. Không sửa .completed/.modules_applied để bỏ cache guard.
