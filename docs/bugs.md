## Kế hoạch Play Integrity từ cộng đồng — 01/11/2026

M4 vẫn chưa có label server. Đã xác minh mod keybox tích hợp của fork bị bỏ
qua trong build r5 và UI bị ẩn; hướng ưu tiên mới là port mod này theo BZI6,
không chờ khôi phục KM3. Dry-run trên copy sau remap DEX còn5 hunk lỗi;
chưa port/build/runtime. Native crypto fixture và keybox thực là hai đầu vào
khác nhau; chưa xác nhận có keybox còn hiệu lực để thử server.
[Phương án, ca cộng đồng thành công/thất bại và điều kiện nghiệm thu](m4-play-integrity-community-plan-20261101.md).

## M4 Keymaster EFS guard và TEE — 01/11/2026

- Software KM4 init trong r5 có bốn lệnh metadata/directory tới EFS. Đã sửa
  payload fork và thêm final-tree/ext4 image gates; fixture PASS. **Chưa build,
  flash hay kiểm boot**; r5 cài trên máy chưa có sửa này. Các service khác vẫn
  có thể truy cập EFS.
- Integrity và KM3 TEE **vẫn mở**. Binary stock có nhánh ghi SAK vào EFS; test
  tmpfs RO chỉ chứng minh guard theo từng mount, chưa bảo vệ toàn bộ EFS/daemon.
  Chưa chạy KM3 để tuân thủ quy tắc phân vùng bảo vệ.

[Bằng chứng và chi tiết patch](m4-g965f-keymaster-tee-work-20261101.md).

## M4 r5 Keymaster/attestation audit — 01/11/2026

- **Mở:** live vendor chỉ có software KM4; `KeyInfo` của EC probe báo
  `false` cho secure hardware. Hai app keys trả attestation software, trung
  gian hết hạn và root không khớp Google Hardware Attestation Root. Không suy
  app chain là chain GMS hoặc một nguyên nhân duy nhất của verdict rỗng.
- **Mở:** GMS field readback chưa trực tiếp; compiled scope và PIF ON/OFF đã
  kiểm. `run-as` lỗi `Segmentation fault`/exit139, cần chẩn đoán riêng nếu dùng.
- Hai key thử và app probe đã dọn; PIF ON/boot complete. Không có patch runtime.

[Bằng chứng, giới hạn và bước tiếp](m4-r5-attestation-audit.md).

## M4 r5 sau Setup Wizard — 01/11/2026

- Hide-developer page: crash/lưu trạng thái giao diện **đã đóng theo phạm vi
  kiểm thử**; mở/toggle/back/reopen và app-selection persistence PASS. Chưa
  kiểm toàn bộ hành vi app đọc Developer options.
- Play Integrity: **còn mở**. Bốn request mới (checker + Play Store, ON/OFF)
  đều không có nhãn; Play Store báo chưa chứng nhận. Fingerprint thật trong
  Play Store thay đổi theo switch; PIF đã khôi phục ON.
- Không thấy lại lỗi Samsung/PIF final-field writer trong request captures.
  Chưa có readback đầy đủ bên trong GMS; chưa phân tích root/extension chain.
- RPC warning vẫn xuất hiện nhưng sau đó attestation record generation thành
  công; không báo warning riêng là fatal. Core PID giữ nguyên, không crash
  Settings/core mới trong buffer kiểm tra.

[Log, kết quả và kế hoạch tiếp](m4-r5-post-setup-validation.md).

## M4 r5 initial boot — 31/10/2026 (lịch sử)

- Settings hide-developer and SamsungPropsHooks: source/build/flash complete;
  on-device isolated writer String/int PASS. Settings page and GMS process
  runtime still pending Setup Wizard completion.
- Integrity: no r5 server verdict yet. Do not carry r4 result forward as r5.
- Early linker: `CANNOT LINK EXECUTABLE "/system/bin/smdexe": cannot locate
  symbol "_ZNSt3__18ios_base7failureD0Ev"` at boot; subsequent smdexe process
  and LTE in-service snapshot observed. Runtime impact requires verification.
- Startup PIF guard: `shouldBypassTaskPermission: unable to get gms uid`, caught
  PackageManager-null exception during early SystemServer initialization; boot
  completed. Later request behavior not yet verified.
- GPT snapshot: actual logical sector4096, both new primary/backup CRC and
  arrays PASS; older 512-byte-sector r4 snapshot is incomplete.

[Details](m4-r5-local-repair.md).

## R4 post-Setup follow-up — 31/10/2026

- Public model display: **closed for r4**, user confirms `SM-A376B`; live
  Setup Wizard complete.
- Play Integrity: **open**, fresh checker JSON has no device label; built-in
  Play Store check returns `Labels: []`, certification says uncertified. Finsky
  attestation record generation succeeds. GMS unstable is observed and logs
  separate SamsungPropsHooks final-field write errors; causality is unproven.
- Settings hide-developer page: **new reproducible crash in log**,
  `NoSuchFieldError: ... mMainSwitch ... SettingsActivity`,
  `HideDeveloperStatusFragment.onCreateView`, at19:51:22.630.
  No implementation change yet; do not attribute Integrity failure to it.

[Evidence and next checks](m4-r4-integrity-followup-plan.md).

## M4 r4 — 31/10/2026

Fresh-flashed and booted on G965F. The installed PIF writer passes isolated
String/int readback and the tested legacy attestation fallback is enabled.
About phone visual confirmation, GMS hook request, OFF/allowlist controls and
Google verdict remain pending after Setup Wizard. Known Bixby receiver crash and
`SamsungPropsHooks` final-field `IllegalAccessException` remain. See
[r4 test](m4-r4-identity-integrity-test.md).

## Historical pre-r3 findings — checkpoint0232, 31/10/2026

Người dùng xác nhận Shopee/TikTok và app đang dùng hiện tải/chạy bình thường.
Không có lỗi app-store compatibility đã tái hiện từ các ví dụ đó. NFC/Bixby
được giữ feature/library/package/API; không port thêm chức năng RF/voice và
không khai báo unsupported. Kết quả RF chưa xác định không đổi thành PASS.

Lỗi đang sửa theo kế hoạch mới: model/name vẫn G965F/S9+ do finalizer/0225,
trong khi yêu cầu là Galaxy A37 5G/SM-A376B; PIF ON nhưng runtime ném
`IllegalAccessException: Cannot set public static final ... Build.FINGERPRINT`;
profile seed thiếu field và updater version bằng nhau bỏ qua; RKP-only1 nhưng
không có IRPC HAL, native key generation trả `HARDWARE_TYPE_UNAVAILABLE`.
Checker JSON có app/license/Play Protect UNEVALUATED và không có nhãn device
integrity. Chưa áp sửa nguồn/flash. Xem
[kế hoạch và bằng chứng0232](m4-identity-integrity-plan-0232.md).

Các kết quả NFC và post-Setup bên dưới là lịch sử trước chỉ thị phạm vi mới.

## Historical post-Setup checks from attempt0229 — 31/10/2026

After Setup Wizard, G965F remains booted and SELinux Enforcing. Auto Blocker is off and ADB keyless is enabled; Wi-Fi validated; SIM is Viettel LTE in-service; camera capture remains untested; Bluetooth is accepted PASS by the user. NFC is unresolved: adapter state3 (ON), `nfc.initialized=true`, HAL running; the user reports no reaction to a known-good tag at the bottom of the unlocked phone on two attempts. The test location may miss the antenna area, so repeat over the marked rear antenna region before treating this as RF failure. Earlier logcat has `NFA_EeSetDefaultProtoRouting: Bad ee_handle` and a T3T routing failure. Corrected mapping shows `14370000.hsi2c` is `/hsi2c@14370000/mfc-charger@3b`, not NFC; NFC is `/hsi2c@108E0000/sec-nfc@27`. See `docs/m4-nfc-functional-test-0231.md`; no NFC source/kernel patch yet.

# A376B One UI → Galaxy S9+ (star2lte)

**Lịch sử sau Setup Wizard của attempt0229 — 31/10/2026:** G965F khi đó boot ổn, SELinux Enforcing. NFC state3 ON nhưng tag chỉ được thử ở đáy máy và không phản hồi; cần áp đúng vùng antenna sau lưng trước khi chốt lỗi RF. Lỗi `14370000.hsi2c` là của MFC charger, không phải NFC. [Chi tiết NFC 0231](docs/m4-nfc-functional-test-0231.md).

**Historical pre-r3 status — 31 October 2026:** M4 attempt0229 booted on G965F `M2_G965F_SERIAL` to Android17 / SDK37 / One UI90000; its Setup Wizard was complete. NFC tag test was inconclusive because the user placed a known-good tag at the phone bottom, outside any confirmed antenna alignment; service state was ON. Bluetooth was accepted by the user. See [artifact review](docs/m4-attempt0229-artifact-review.md) and [NFC test 0231](docs/m4-nfc-functional-test-0231.md). R3 is the latest flash and its fresh USERDATA still requires Setup Wizard completion.


**Cập nhật kiểm thử Bluetooth30/10:** người dùng xác nhận màn hình thật lên
Chào mừng và chủ động reboot. Ghép đôi với điện thoại `meizuo`, nhận2ảnh qua
OBEX/OPP PASS;34mẫu/337,03s giữ boot/PID core,0crash/0FDF1timeout; hai file
kết thúc status200, phiên hoàn tất200. A2DP/AVRCP/HFP và reconnect chưa đo riêng; người dùng chấp nhận Bluetooth là PASS.
Xem [báo cáo truyền file](docs/m4-bluetooth-phone-opp-validation.md).

**Tiếp tục sửa30/10:** live `getprop` cho thấy model/name vẫn lấy từ donor BZI6.
Patch0225 sửa đúng `ro.product.system.*` và `ro.product.system_ext.*` trong
profile M4/G965F, giữ nguyên fingerprint Android17. Fixture đã PASS; replay,
build, ảnh ext4 và boot xác nhận model vẫn đang chờ. Bixby còn lỗi receiver
`No matching ComponentResult found!`; chưa đổi feature phím vì binary phải giữ
chữ ký Samsung và cờ Side Key hiện phản ánh phần cứng S9. M4 vẫn chưa nghiệm thu.


**Cập nhật30/10 — M4 runtime sau attempt0223:** máy G965F đã hiện “Chào mừng”.
Đã sửa service OTT làm system_server restart, timeout Bluetooth DBFW+ và hook
nền thiếu của AOD làm SystemUI crash. Lượt cuối7mẫu/122s +5sleep/wake giữ core
PID; mốc uptime423s vẫn cùng server/UI/Bluetooth, Bluetooth0crash. Bixby Side
Key, phụ kiện Bluetooth, NFC RF, app network và nghiệm thu phần cứng còn mở.
**M4 chưa hoàn tất.** 215patch/160path source replay PASS; các sửa hiện là
one-file runtime trials, chưa đóng full ROM mới. Full package mới nhất là
attempt0223, SHA256 `a35d05fc9707930a1f747a10008ede86b62ec88273e35bca5319126ef865e4fa`.
Xem [báo cáo sửa runtime](docs/m4-post-0223-runtime-repair.md) và
[kế hoạch hiện hành](docs/PLAN.md). Kernel, Hybrid Vendor, radio và USERDATA
được giữ trong các trial; M1/G965N không đổi.

Các checkpoint29/10 trở về trước dưới đây là lịch sử.


**Cập nhật29/10 — M4-attempt0222 đã build và đóng gói trên PC, chưa flash và
M4 chưa nghiệm thu.** Full ZIP/target-files và8 stage đã qua hash/signature/
ZIP32 checks. Candidate này chưa chứng minh đã sửa lỗi Bluetooth, network hoặc
reboot. Runtime evidence mới nhất vẫn từ0219: core18mẫu/180s và5vòng sleep/wake
không restart mới; RTC0218 và độ sáng0216 PASS theo phạm vi đo. Bluetooth vẫn
lặp~29s: native stack chờ VaultKeeper khi đọc BLE local-key; normal network
connect vẫn EMSGSIZE dù rawTCP control PASS. NFC typed Binder state query trả ON;
RF/tag còn mở. AOD/Bixby startup Java faults vẫn có trong current-boot buffer.
Thermal audit xác định resource hiệu lực max867/table306/408/510, chưa có direct
caller/calibrated-threshold verdict.

Giữ M4-attempt0222 tại `rom/out/releases/M4/star2lte-1793274163-CcPxEn/`;
full ZIP SHA256 `c3aff071dec34579a69e7e66f88e134a9714e89d850e2a09582667ae367ee23d`.
Xem [artifact review0222](docs/m4-attempt0222-artifact-review.md),
[báo cáo runtime0219](docs/m4-validation-after-0219.md),
[phương án sửa chi tiết](docs/m4-post-0219-repair-plan.md), [trạng thái](STATUS.md),
[RTC](docs/m4-rtc-label-0218.md), [thermal audit](docs/m4-thermal-limiter-audit-0219.md).

Các checkpoint bên dưới là lịch sử; không coi chúng là nghiệm thu phần cứng.


Cập nhật28/10: patch0214 đã flash fresh trên G965F và xác nhận `brightness` mang context đúng dưới Enforcing; log fresh boot phát hiện `max_brightness` vẫn EACCES. Patch0215 thêm exact label node đó; source/image gates PASS và 8-stage ZIP32 candidate đã sẵn (`star2lte-1793191531-88WPtN`, SHA256 `6d0c43e77c3bae6f392250cb5c4625fb6fc3af0041637b44bfd7e96ff61ae58d`). Đã fresh-flash 8/8 stage trên G965F; reboot vào Android, sys.boot_completed=1/Enforcing. Hai node backlight đều có label chính xác và không còn fstat EACCES; kiểm Settings60/180 làm sysfs brightness đổi, rồi khôi phục230. Thermal HAL vẫn báo cooling/threshold type0 thiếu, nên limiter chưa nghiệm thu. **M4 chưa nghiệm thu**: Bluetooth restart, network control socket EMSGSIZE, AOD callback API, NFC RF/Binder và một số app còn mở. Kế hoạch/bằng chứng: [plan sau0212](docs/m4-post-0212-validation-plan.md), [thermal0214/0215](docs/m4-thermal-backlight-0214.md), [max-brightness0215](docs/m4-thermal-backlight-0215.md), [validation](docs/m4-validation-after-0212.md).


Cập nhật28/10 —0211/0212: M4 đã boot tới màn hình **Chào mừng**, người dùng xác nhận màn hình thật.0211 đặt resolver BZI6 vào SYSTEM có đúng ABI;0212 khôi phục nhãn chính xác `/dev/ion` cho gralloc của Hybrid Vendor. Sau reboot, nhãn tự đúng, SELinux Enforcing, main SystemServer/netd ổn định thêm120s; không wipe hay đổi kernel/vendor/radio. **Chưa nghiệm thu M4 hoàn chỉnh**: AOD thiếu `disconnectPluginOnBackground()` làm SystemUI restart một lần; Bixby và kiểm phần cứng/mạng còn mở. ROM hoàn chỉnh đang giữ là attempt0209;0211/0212 mới là sửa SYSTEM chẩn đoán trên máy.203 patch/140 file replay byte-identical; chưa có package tích hợp mới. Chi tiết: [runtime repair](docs/m4-postboot-runtime-repair.md).

Các checkpoint tiếp theo là lịch sử; trạng thái mới nhất ở đoạn trên.


Cập nhật28/10 — source0198: M4 đã quan sát được `sys.boot_completed=1`, `bootanim=stopped`, SDK37/OneUI90000 và SELinux Enforcing trên G965F. Fixture Debug JNI/MemInfoReader PASS exit0; GPU map không có trả-1, không còn làm SystemServer abort. SystemServer PID1830 còn ở uptime337.22s. **Chưa nghiệm thu giao diện/phần cứng**: SystemUI/FaceWidget sai luồng main, SDHMS lỗi enumDEX và NFC thiếu libnfc-nci.so. Đã thu189 file và hoàn tất rollback SYSTEM/BOOT/VENDOR/netstats, đối chiếu sau ghi PASS; máy đang ở TWRP.188 patch/132 file replay PASS,187 hash cũ giữ nguyên; chưa có ROM hoàn chỉnh mới tích hợp các thử nghiệm195–198. Chi tiết: [meminfo runtime](docs/m4-meminfo-runtime.md), [lỗi sau boot](docs/m4-post-boot-blockers.md).

Cập nhật runtime 28/10/2026 — source0197: đã port nhóm query stats và bộ transport/identity/template/Parcel/lưu v7-v8 từ BZI6, giữ entry point S22 cũ và Binder service.2161 lớp round-trip,385 tham chiếu services và36 phụ thuộc mới đã rà; APEX dựng lặp cùng hash,187 patch/130 file replay PASS. Fixture trên G965F đã PASS exit0; SystemServer sau đó native SIGABRT trong libmeminfo (chi tiết0198 ở trên). Đã thu136 file và hoàn tất rollback SYSTEM/BOOT/VENDOR cùng hai thư mục netstats. boot_completed chưa đạt. Chi tiết: [network stats runtime](docs/m4-network-stats-runtime.md).

**M4 đã build và đóng gói hoàn chỉnh —27/10/2026, attempt37/source0188.** Full ZIP đã ký (5,192,379,395 bytes) và target-files đã qua đối chiếu SHA256;8 gói ZIP32 đã qua CRC/chữ ký/giới hạn ZIP32 và có hash trong manifest; run `star2lte-1793025724-rBdwLO` có `.complete`. Toàn bộ kiểm compiled, PIF, ADB/Auto Blocker, SELinux và image ext4 thực tế đã PASS. Chuỗi178 patch replay thành125 file giống hệt từ fork đã khóa. **M4 đã fresh-flash G965F, khởi tạo được SystemServer nhưng chưa boot hoàn tất; đang sửa tương thích API mạng. Verdict Play Integrity trực tiếp chưa kiểm.**

Ở trạng thái lưu trữ sau dọn dẹp26/10, mỗi mốc M1/M2/M4 có một ROM cuối trên NVMe phụ; các lượt M4 lỗi và fixture tạm đã xóa sau khi giữ log. Dữ liệu dung lượng và danh sách khi đó chỉ là snapshot lịch sử. M4 package0215 được giữ làm rollback trong khi package0216 chờ kiểm thử runtime; xem [artifact review cũ](docs/m4-attempt37-artifact-review.md), [lưu trữ](docs/storage-cleanup-20261026.md), và [báo cáo0216](docs/m4-brightness-scale-0216.md). Các checkpoint dưới đây là lịch sử.

Porting project for Galaxy S9+ Exynos 9810: SM-G965N is the primary target,
SM-G965F is tested first. Final goal: Galaxy A37 SM-A376B One UI 9 / Android 17.
This repository shares source patches, diagnostics and evidence for assistance.
It does not contain a working Android 17 ROM or firmware images.

**Current M4 build — 28 October 2026:** Run `star2lte-1793199175-ncgBlt`
contains patch0216 for the BZI6 legacy backlight/PAC3 scale path. The signed
flashable ZIP and target-files checksums pass; all eight classic ZIP32 stages
pass CRC/signature/range checks. The installed-system brightness scale, off and
wake paths, thermal limiter, and other open M4 runtime issues still require
device testing. This build step did not flash or reboot the phone. See
[build and runtime checklist](docs/m4-brightness-scale-0216.md) and
[current status](STATUS.md).

**Historical source checkpoint — source0179 / attempt37 (26 October 2026):**
169 hash-locked patches replay to125 byte-identical source files from the pinned
fork. User prioritized Play Integrity Fix: BZI6 keystore/task/application hooks,
compiled DEX and Settings controls/defaults passed fixtures. Paradigm, Setup
Wizard UI and Misc Tweaks retain donor behavior. Finalization tail and full split
SELinux policy compilation passed on disposable attempt36; native BZI6 camera,
MDF, Secure Folder, Brief, photo editor and wallpaper app selection are reviewed.
Mandatory ADB/no-RSA/Auto Blocker controls/defaults remain enforced. Attempt36
and its diagnostic scratch were deleted (15,348,322,304 bytes reclaimed); M1/M2
final completed releases were unchanged at that checkpoint. At that point there
was no completed M4 package, Android17 boot or live Integrity verdict.

**M2 runtime status — 25 October 2026:** Attempt56 was built through the existing fork and fresh-installed on G965F in eight signed ZIP32 stages. USERDATA was freshly formatted, protected partition hashes matched before/after, and the selected diagnostic BOOT hash matched. Two Enforcing boots reached `sys.boot_completed=1` (197s fresh /84s reboot). The patch restores only the donor-pinned `vendor_kg` passwd/group rows: `id 5959` now resolves to `vendor_kg`, the process enters `u:r:kg_app`, and the old `No match for app with uid 5959` / zygote `SIGABRT` is absent in both captures.

The init logger produced `/cache/unica` post-fs, post-fs-data and boot-complete markers with nonempty start/15s/60s logcat on both boots; init-context dmesg/props snapshots remain empty. A separate KnoxGuard failure remains: `TLC_ERROR` and `@TimeoutException: getTAInfo - null` lead to one `LockedBootCompletedReceiver` ANR on each boot (attempt56 boot1 logcat lines253262/253958; boot2 lines195922/195946; ANR trace main thread blocks in `IKnoxGuardManager.getTAInfo`). BPF still returns `EINVAL`; Bixby and broader hardware/stability checks remain open. This is a diagnostic Android16 M2 result, not full hardware acceptance or Android17 readiness. The same WIPEONFORK diagnostic kernel as attempt55 was used; no kernel was rebuilt. M1/G965N and production kernel selection are unchanged.

Completed ROMs were checksum-verified and moved to the spare ext4 NVMe; duplicate/intermediate files reclaimed38.51GiB. Run `bash tools/mount-release-storage.sh` after a PC reboot before accessing releases. See [storage cleanup](docs/storage-cleanup-20261025.md), [integrated runtime build report](docs/m2-integrated-runtime-build.md), [kernel provenance](docs/m2-runtime-kernel-fix.md) and [current assistance brief](docs/HELP-WANTED.md).


**Earlier M4 source preflight history — 26 October 2026:** Initial source work preceded the current source0179 checkpoint, including the scoped G965F/no-KSU profile, BZI6 Android17
NFC APEX gate and strict donor-camera dispatch. The existing fork stages a
hash-locked donor-native Android17 camera/audio/graphics V7/Tethering set,
`vendor_kg` account and aconfig-v2 metadata, then checks SYSTEM/VENDOR ext4
output. Helper and wrong-mode/label negatives pass. Actual BZI6
SettingsProvider/services.jar were decoded, patched and rebuilt with fork
Apktool; compiled ADB, no-RSA, stored-choice, donor UI/observer checks and M2
regression pass. Attempts01–03 closed the NFC, donor-camera and Keystore
synthetic-password dispatch mismatches. Attempt04 then stopped because the old
Android16 P2P DEX signature is absent in BZI6;0135 adapts the reviewed Smali
callback while retaining Samsung Seh setup. Attempt05 passed that branch in the
real build, then stopped at `camera/customize.sh: line 397: abort: command not
found` because BZI6 supplies only 64-bit `libstagefright.so`. Patch0136 adds a
strict M4 arm64-only skip for the absent 32-bit file, requires and patches the
64-bit donor library, and uses the fork's `ABORT` helper for failures. Attempt06
passed P2P and stagefright, then stopped at DVFS because the generic
`smali/U1/w.smali` class is absent. Patch0137 pins the exact BZI6
`SamsungDeviceHealthManagerService.apk` and decoded class `smali/n5/i0.smali`,
then adapts the forced A37 SSRM policy literal to the target `ssrm_default`.
Exact M4, mutated-hash, wrong-scope and M2 AZH4 fixtures pass; all127 patch0137
hashes matched. Attempt07 reached the DVFS adapter but the new source-APK guard
used a module-local cache variable that was unset; patch0138 uses the fork's
`GET_FIRMWARE_CACHE_KEY` helper and resolves the pinned donor APK in its actual
cache. Attempt08 passed the cache and SDHMS mapping, then exposed a media order
conflict: camera patching changed the Stagefright model string before the
legacy HEVC adapter checked its original whole-file hash. Patch0139 verifies the
camera-patched intermediate by hash and reverses that property change back to
the donor hash before applying/checking the HEVC edit; the combined output hash
is pinned. Actual intermediate fixture, original-donor negative and ordered patch locks
pass. Attempt10 exercised Patch0140, then found the missing AOSP RescueParty
class; Patch0141 preserves the hash-pinned BZI6 Samsung replacement only in M4.
Attempt11 stopped at the vendor-mismatch warning patch because BZI6 moved its caller and changed the handler. Patch0142 hash-pins the BZI6 classes and suppresses only that warning dialog; UID and emergency dialogs remain. Its Smali fixture, Apktool services.jar rebuild, and full132 patch replay pass. Attempt12 applied this adapter, then stopped at NFC because G965F PHN lacks libnfc_prop_extn.so and libnfc_vendor_extn.so. Patch0143 selected hash-pinned BZI6 donor sidecars, but attempt15 showed its guard expected SDK29 while the generated target config is SDK31 (`m4-build-attempt15.txt:1038`). Patch0144 fixes that exact guard, confirmed by attempt16 log line1038. Attempt16 then stopped at line1080 because legacy identity replacements changed CoreRune before the strict display hash check. Patch0145 pins the exact intermediate after reversing only the two logged model/device substitutions; display and compiled DEX fixtures, wrong-preimage gates, and all135 patch replay pass. Attempts13/14 were interrupted without a build result; later progress is recorded below. There is no complete M4 ROM, flash or runtime proof. ADB/Auto
Blocker fresh defaults and live controls remain
mandatory gates. See the [M4 NFC report](docs/m4-nfc-integration.md), [M4
runtime report](docs/m4-native32-runtime.md), [source-selection report](docs/m4-source-selection.md)
and [dispatch ledger](docs/m4-dispatch-ledger.md).

## Source reproduction

The existing build system is
[Eend15/Unofficial-Un1ca-9810](https://github.com/Eend15/Unofficial-Un1ca-9810),
branch Android-16, pinned commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1`. The active text patches in
`patches/fork/series` extend that fork through 0218.

```bash
bash tools/prepare_fork.sh
# Read docs/build-environment.md and docs/m2-build.md before building.
# Initialize dependencies / acquire firmware separately, as the fork requires.
```

The helper refuses an existing destination and only clones/applies patches.
Builds remain in the fork's `rom/` checkout. Donor S22 configuration is retained;
A37 Android16/17 profiles are added alongside it. No firmware or device operations
are performed by the preparation helper. Source provenance is recorded in
sources.lock.json, firmware.lock.json and tools.lock.json.

## Fixed project decisions

- Hybrid Vendor VNDK33 from the fork plus correct F/N hardware and RIL blobs;
  use the fork's ABI/hash-locked radio bridge when needed.
- Original duhansysl DS-ACK V1.12 08/05/2026 Enforcing-OneUI7 without KernelSU;
  kernel changes need specific runtime evidence and a separate review.
- Preserve modem, bootloader, EFS and CPEFS. G965N remains unflashed.
- Default Auto Blocker off, USB debugging on, USB ADB without RSA authorization
  on development builds. Preserve functional Settings switches and descriptions;
  seed defaults only for new settings, with no boot loop overriding user choices.
- Keep the latest completed M1/M2/M4 release for each milestone, per the user's
  26 October retention update. Package into a new run and verify it before
  deleting the older release. Artifacts stay local and are never published in
  this source repository.

## Reading guide

- [M4 plan: A37 One UI 9 / Android17](docs/m4-plan.md)
- [Assistance brief / current evidence](docs/HELP-WANTED.md)
- [Working rules](AGENTS.md), [plan](docs/PLAN.md), [chronological status](STATUS.md)
- [Fork map](docs/repo-map.md), [source provenance](docs/sources.md)
- [Patch matrix](docs/patch-matrix.md), [bugs and logs](docs/bugs.md)
- [M2 build history](docs/m2-build.md), [hardware differences](docs/hw-diff.md)
- [M1 fresh boot report](docs/m1-fresh-report.md), [ADB rule](docs/adb-defaults.md)
- [Diagnostic sources and test limits](diagnostics/README.md)
- [Selected shareable evidence](support/evidence/)

All build/runtime claims are scoped to the documented donor, target and artifact.
Older chronological notes may describe superseded decisions; current AGENTS.md
and the dated assistance brief explain the active state. A successful G965F
boot does not establish G965N compatibility or Android17 support.

## Sharing

Proprietary Samsung firmware, generated output, ROM/BOOT images, EFS dumps,
credentials, signing keys and raw device logs are excluded. The exported GitHub
snapshot substitutes physical device identifiers with role aliases, replaces
local absolute paths with `${LOCAL_ABSOLUTE_PATH}`, and includes only allowlisted
text/source files. This path redaction was added after review found a local
Downloads path in prior snapshots; verification is the exporter's new-snapshot
scan plus secret/identifier checks. Files affected: `tools/export_github_handoff.py`
and this handoff note. Upstream code and kernel retain their respective licenses
and credits; this handoff does not relicense third-party code.

## Runtime kernel diagnostic — 24 October 2026

Probe13 reproduced the M2 vendor BoringSSL restart trigger as an A37 runtime
APEX linker abort: `arc4random data MADV_WIPEONFORK failed: Invalid argument`
(exit134), followed by init's `reboot_on_failure`. A separately built, narrowly
scoped DS-ACK V1.12 backport passed real fork/zero-fill/KEEPONFORK checks and
made the exact A37 linker `--list` exit0 on the M1 G965F control under enforcing.
M1 has been restored to original BOOT. The audited diagnostic BOOT cleared both vendor/system BoringSSL self-tests on
M2. Subsequent tests reached Android, retained metadata through reboot and captured
rear/front photos. All test writes have been rolled back; a complete production
M2 artifact and hardware acceptance remain pending.
This is not a production kernel/package change. Details and hashes:
[diagnostic report](docs/m2-runtime-kernel-fix.md).

The M2 test now clears the previous BoringSSL/linkerconfig failures: both
BoringSSL self-tests exit0 and linkerconfig generates its config. It then reaches
a separate A37 Tethering gate: `Android 25Q4 requires kernel 5.10`, causing
`bpfloader` exit7 and `reboot,bpfloader-failed` on DS-ACK4.9.337. The diagnostic
BOOT was restored to its original hash after capture. The older retained S22 Tethering APEX then cleared that loader gate, and
the later V6/metadata test reached Welcome. Hardware and module acceptance
remain pending; see the updated runtime and metadata reports.

## M2 diagnostic status — 24 October 2026

The integrated G965F Android 16 attempt53 diagnostic build passed compiled gates, actual ext4 SYSTEM/VENDOR/ODM audits, and signed package verification. After one-time authorization for this exact package, it was fresh-installed on G965F `M2_G965F_SERIAL`; all eight TWRP32 stages passed, USERDATA UUID changed, and protected partition hashes remained unchanged. Its first boot reached `sys.boot_completed=1` at 196.4s with Enforcing SELinux. The `/metadata` tmpfs initialized aconfig; RANGING is dangerous and RangingService registered. Fresh ADB defaults were enabled with `ro.adb.secure=0`, and Rampart logged `disabledRampart=true`. Attempt53's selected runtime-diagnostic profile did not enable the optional patch0095 CACHE logger. USB gadget init logs a missing UDC while host ADB is reachable. This is a diagnostic first boot, not full hardware or M4/G965N acceptance. See [the integrated runtime report](docs/m2-integrated-runtime-build.md).


The latest M4 build passed the BZI6 Battery Health classes3 gate with patch0150.
Attempt23 then failed only in the optional legacy Package Block List hunks
(`m4-build-attempt23.txt:1194,1206,1234,1294`). Patch0151 retains the BZI6
donor package policy on the exact M4 G965F profile. Clean attempt24 is running
on the secondary NVMe; no completed M4 package or device test is claimed yet.


**Historical checkpoint0168:** superseded by current source0179/PIF/attempt37 status above.


## M4 runtime repair — 28 October 2026

Patches0199–0206 integrate the measured runtime repairs into the explicit G965F diagnostic profile, including the user-approved omission of only the BZI6-unsupported DeX thermal special entry. Source/image/package checks passed in isolated run `star2lte-1793162155-h9hgDY`; all eight stages were freshly installed on G965F `M2_G965F_SERIAL`. Protected EFS/CPEFS/RADIO/RECOVERY/BOTA hashes remained unchanged and the partition layout matched the audited sizes. The first boot did **not** return ADB within 300 seconds (`boot_completed_not_observed_within=300s`), so attempt0207 is diagnostic and not accepted. USB now enumerates as Samsung RNDIS only; the ADB TCP endpoint refused connection, so Android boot-complete is still unconfirmed. Recovery log records `/tmp/exynos9810/vendor_bootfix.sh[255]: can't create temporary file : No such file or directory` even though the installer completed. The retained attempt37 release remains untouched. Need return the device to TWRP, collect CACHE/pstore/recovery logs, then diagnose before another build or flash. See [repair record](docs/m4-postboot-runtime-repair.md), [boot log](logs/20261028-m4-repair-build/boot-attempt0207.txt), and [plan](docs/m4-post-boot-repair-plan.md).


Patch0201 also addresses the measured BZI6 FaceWidget main-thread crash using the existing UI handler plus connection-generation and plugin-identity checks. Apktool fixture build/decode passes; runtime startup/UI/AOD stability is still pending.

Patch0202 addresses the measured NFC loader failure without mixing incompatible Java/JNI APIs: the BZI6 `NfcNciApex.apk` and G965F PHN `NfcNci.apk` have different `NativeNfcManager` native declarations (54 vs72). The M4/G965F diagnostic profile now selects the JNI embedded with BZI6 Java and omits only the PHN global JNI shadow, retaining the target SLSI service/RF/HAL stack. Static hash/source gates pass; runtime JNI registration and RF/tag behavior remain unverified. No new ROM build or flash has occurred; the retained attempt37 artifact is unchanged.


M4 attempt0207 reboot loop is now traced to the exact runtime error `CANNOT LINK EXECUTABLE "/system/bin/netd": library "libnetd_resolv.so" not found`. Patch0207 adds the pinned BZI6 resolver library and its APEX `provideNativeLibs` declaration to the M4-only diagnostic composition. APEX build/signature/metadata reproducibility checks pass; rebuilding and live boot/network verification remain pending. See [M4 runtime repair](docs/m4-postboot-runtime-repair.md).
# M4 r3 checkpoint — 31/10/2026

- **Public identity: verified live.** G965F reports `SM-A376B` / `a37xnaxx`,
  Settings device name `Galaxy A37 5G`; physical model remains `SM-G965F`,
  device codename `star2lte`. No app-level `Build.MODEL` test yet because Setup
  Wizard is still foreground.
- **PIF configuration: seeded and verified live; runtime hook not yet exercised.**
  PIF on, version `20261011`, security patch `2026-10-05`, first API `32`.
  Prior runtime error was `IllegalAccessException` writing
  `android.os.Build.FINGERPRINT`; the guarded Unsafe writer passed compiled
  gates, but a new checker request is pending Setup Wizard completion.
- **Keystore/RKP remains open.** `remote_provisioning.tee.rkp_only=1`; live stack
  exposes Keymaster HIDL4.0 and no IRPC/default Remote Provisioning HAL. Earlier
  error: `Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)`. Do not set a release fallback
  until a controlled key-generation/attestation test proves behavior.
- **NFC/Bixby compatibility retained.** Runtime package/feature/library checks
  pass; RF/voice are not being ported. Known accepted donor Bixby crash remains:
  `java.lang.IllegalStateException: No matching ComponentResult found!`.
- **Boot/flash: pass to Setup Wizard.** `sys.boot_completed=1`, SELinux Enforcing,
  SystemServer PID2216/SystemUI PID6238 at first sample. Only the accepted Bixby
  crash appears in crash buffer; user must finish Setup Wizard before Integrity
  checker and Play Store tests. Full record:
  [M4 implementation r3](m4-identity-integrity-implementation-0232.md).

