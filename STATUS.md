# Trạng thái thực thi — cập nhật30/10/2026


**Cập nhật kiểm thử Bluetooth30/10:** người dùng xác nhận màn hình thật lên
Chào mừng và chủ động reboot. Ghép đôi với điện thoại `meizuo`, nhận2ảnh qua
OBEX/OPP PASS;34mẫu/337,03s giữ boot/PID core,0crash/0FDF1timeout; hai file
kết thúc status200, phiên hoàn tất200. A2DP/AVRCP/HFP và reconnect tai nghe/loa
chưa thử. Xem [báo cáo truyền file](docs/m4-bluetooth-phone-opp-validation.md).

**Tiếp tục sửa30/10:** live `getprop` vẫn chọn model/name donor vì
`ro.product.system.*` và `ro.product.system_ext.*` còn là A37/essi. Patch0225
đặt các trường identity partition-qualified thành star2lte/G965F, giữ donor
fingerprint Android17; fixture đúng/âm tính/idempotence PASS, build/image/runtime
còn pending. Crash buffer hiện tại chỉ còn lỗi receiver Bixby; xem báo cáo sửa.


## Cập nhật30/10 — sửa runtime sau attempt0223

G965F đã hiển thị trang **Chào mừng** qua screenshot ADB. Ba nhóm sửa đã áp riêng
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

Các sửa đang ở source và trên máy dưới dạng thay riêng framework-res, Bluetooth
JNI, SystemUI, giữ USERDATA. Chưa có full ROM mới tích hợp0221–0224, chưa có
image/ZIP gate mới cho composition này. ROM hoàn chỉnh mới nhất vẫn là M4
attempt0223, full ZIP SHA256
`a35d05fc9707930a1f747a10008ede86b62ec88273e35bca5319126ef865e4fa`.
215 patch replay thành160 path byte-identical sau0225; không sửa kernel/Hybrid
Vendor/radio. Hash10 phân vùng bảo vệ khớp trong từng cửa sổ ghi recovery;
không ghi/restore EFS. M1/G965N giữ nguyên.

Báo cáo, bằng chứng, giới hạn và bước tiếp:
[m4-post-0223-runtime-repair.md](docs/m4-post-0223-runtime-repair.md).
Ưu tiên tiếp: port hợp đồng phím Bixby đúng phần cứng và giữ chữ ký Samsung;
kiểm phụ kiện Bluetooth/hoàn tất wizard bằng thao tác thật; typed NFC/RF và
app network; sau đó build/audit full image/ZIP trong run mới. Không ép global
Side Key feature hoặc đổi blob/kernel để che lỗi cấu hình.

Các checkpoint bên dưới là lịch sử.

**M4-attempt0222 đã fresh-flash và boot được Android17 trên G965F; M4 chưa nghiệm thu.**
Full ZIP SHA256 `c3aff071dec34579a69e7e66f88e134a9714e89d850e2a09582667ae367ee23d`,
target-files `c741a3d1118169e707d02874d1cc96fc9925db9e1b0101618344e9919c9dbf06`;
8 ZIP32 stage, CRC/signature/ZIP32 và hash readback đều PASS. Runtime:
`sys.boot_completed=1`, Enforcing; `system_server`/SystemUI giữ PID khoảng180s.
Bluetooth FAIL: log boot ghi `Timed out waiting for Unknown OpCode(0xfdf1)` và
`ServiceConnection.onServiceDisconnected`; live follow-up tăng tới94 lần crash.
Disassembly khóa sender BZI6, source patch0220 qua fixture nhưng chưa build/
flash. Mạng app-level chưa được thử. Báo cáo:
`docs/m4-validation-after-0222.md`; artifact review:
`docs/m4-attempt0222-artifact-review.md`.

**M4-0219 chưa nghiệm thu tổng thể; số liệu đó là lịch sử, không thay kết quả0222.**
Lượt kiểm0219 có
18mẫu/180s giữ core PID/starttime và boot ID,5sleep/wake
không crash mới. Bluetooth FAIL7PID/6timeout: native stack khóa BLE local-key
load→VaultKeeper read→service wait~29s; metadata/format16–32byte phải được kiểm
trước backend adaptation. Network ping IP/name vẫn EMSGSIZE, rawTCP control
mới PASS; browser HTTP/HTTPS chưa có verdict do Chrome FirstRun.

NFC no-FD transaction6 đã verify runtime jar/stub, trả ON. AOD/Bixby Java fatal
trong startup buffer cùng boot vẫn mở, không phát sinh mới trong live window.
Provider failure0217 chưa tái hiện; GNSS firstboot null callback crash chưa
khép và chưa có outdoor fix test. Thermal effective product RRO là max867/
table306/408/510; đã rút lại suy luận caller từ base framework-res.13class/
194compiled methods PASS; actual caller/gates/calibrated thresholds còn mở.
Không có nhiệt kế ngoài, không ép nhiệt hoặc sửa policy/kernel.

Package mới nhất đang giữ là M4-attempt0222. Flash fresh và runtime logs lưu ở
`logs/20261029-m4-g965f-flash-0222/flash-01/`; EFS/CPEFS/RADIO/RECOVERY/BOTA
hash trước/sau khớp, không cần rollback. Kết quả Android/Enforcing, Bluetooth,
ADB, Auto Blocker và giới hạn mạng thuộc báo cáo0222; các kiểm tra Settings
task15/Dozing, brightness63/manual0/stay-awake0 vẫn chỉ thuộc lượt0219.
Báo cáo: [validation0219](docs/m4-validation-after-0219.md);
[plan hiện hành](docs/m4-post-0219-repair-plan.md), [validation0222](docs/m4-validation-after-0222.md), [thermal audit](docs/m4-thermal-limiter-audit-0219.md),
[RTC](docs/m4-rtc-label-0218.md), [bugs](docs/bugs.md).

Các checkpoint bên dưới là lịch sử.


Patch0214 đã flash fresh trên G965F: `/sys/devices/platform/panel_drv@001/backlight/panel/brightness` có context `sysfs_ski_display_writable` sau reboot Enforcing, không còn SDHMS brightness EACCES. Fresh boot lộ thêm `max_brightness` EACCES; patch0215 thêm exact genfscon node này. Patch0215 đã qua source tests và build: candidate `star2lte-1793191531-88WPtN`, package SHA256 `6d0c43e77c3bae6f392250cb5c4625fb6fc3af0041637b44bfd7e96ff61ae58d`; vendor CIL/image gate PASS, 8-stage ZIP32 manifest PASS. Đã fresh-flash candidate0215 8/8 stage, USERDATA format mới, boot_completed=1/Enforcing; hash EFS/CPEFS/RADIO/RECOVERY/BOTA khớp trước/sau. Cả brightness và max_brightness có label đúng, không còn backlight EACCES; thay Setting60/180 làm sysfs brightness đổi. Thermal HAL vẫn không expose cooling/threshold type0; limiter/actuator chưa nghiệm thu. Package0214 riêng được giữ nguyên; kernel/vendor blob/radio và protected partitions không đổi.

Cập nhật kiểm thử28/10 sau0212: giữ nguyên ROM; snapshot chi tiết1178.75s và mốc cuối2052.12s vẫn cùng PID core. SystemServer/netd/SurfaceFlinger ổn; SystemUI sau lần crash đầu sống hơn17phút,5 lượt sleep/wake không crash mới; người dùng xác nhận Home thật bình thường. **Chưa nghiệm thu M4**: Bluetooth ít nhất30restart, Bionic network nhận EMSGSIZE từ control socket (rawTCP đối chứng PASS), thermal backlight permissions đã qua fresh-flash/runtime gate0215; HAL cooling/threshold và actuator vẫn chờ audit, AOD thiếu API; NFC dump/Binder chưa kết luận RF, Bixby/KG còn lỗi. Kế hoạch hiện hành: [plan sửa sau0212](docs/m4-post-0212-validation-plan.md); bằng chứng: [validation](docs/m4-validation-after-0212.md), [thermal0214](docs/m4-thermal-backlight-0214.md).


Latest28/10 —0212: physical Welcome confirmed, persistent boot_completed/Enforcing and automatic ion_device context pass. Main SystemServer/netd remain stable120s; previous ION/gralloc allocation errors absent. Device stays in Android with0211 resolver +0212 context on full attempt0209. AOD background-disconnect AbstractMethodError still restarts SystemUI once; Bixby/general hardware tests and a new complete package remain pending. Source replay203 patches/140 byte-identical files. Details: [runtime repair](docs/m4-postboot-runtime-repair.md). Older entries below are historical.


Cập nhật28/10 — source0198: M4 đã quan sát được `sys.boot_completed=1`, `bootanim=stopped`, SDK37/OneUI90000 và SELinux Enforcing trên G965F. Fixture Debug JNI/MemInfoReader PASS exit0; GPU map không có trả-1, không còn làm SystemServer abort. SystemServer PID1830 còn ở uptime337.22s. **Chưa nghiệm thu giao diện/phần cứng**: SystemUI/FaceWidget sai luồng main, SDHMS lỗi enumDEX và NFC thiếu libnfc-nci.so. Đã thu189 file và hoàn tất rollback SYSTEM/BOOT/VENDOR/netstats, đối chiếu sau ghi PASS; máy đang ở TWRP.188 patch/132 file replay PASS,187 hash cũ giữ nguyên; chưa có ROM hoàn chỉnh mới tích hợp các thử nghiệm195–198. Chi tiết: [meminfo runtime](docs/m4-meminfo-runtime.md), [lỗi sau boot](docs/m4-post-boot-blockers.md).

Cập nhật runtime 28/10/2026 — source0197: đã port nhóm query stats và bộ transport/identity/template/Parcel/lưu v7-v8 từ BZI6, giữ entry point S22 cũ và Binder service.2161 lớp round-trip,385 tham chiếu services và36 phụ thuộc mới đã rà; APEX dựng lặp cùng hash,187 patch/130 file replay PASS. Fixture trên G965F đã PASS exit0; SystemServer sau đó native SIGABRT trong libmeminfo (chi tiết0198 ở trên). Đã thu136 file và hoàn tất rollback SYSTEM/BOOT/VENDOR cùng hai thư mục netstats. boot_completed chưa đạt. Chi tiết: [network stats runtime](docs/m4-network-stats-runtime.md).

**M4 đã build và đóng gói hoàn chỉnh —27/10/2026, attempt37/source0188.** Full ZIP đã ký (5,192,379,395 bytes) và target-files đã qua đối chiếu SHA256;8 gói ZIP32 đã qua CRC/chữ ký/giới hạn ZIP32 và có hash trong manifest; run `star2lte-1793025724-rBdwLO` có `.complete`. Toàn bộ kiểm compiled, PIF, ADB/Auto Blocker, SELinux và image ext4 thực tế đã PASS. Chuỗi178 patch replay thành125 file giống hệt từ fork đã khóa. **M4 đã fresh-flash G965F, khởi tạo được SystemServer nhưng chưa boot hoàn tất; đang sửa tương thích API mạng. Verdict Play Integrity trực tiếp chưa kiểm.**

Đã giữ đúng một bản ROM hoàn chỉnh cuối cho mỗi mốc M1/M2/M4 trên NVMe phụ;3 lượt package M4 dở và4 replay fixture đã xóa sau kiểm chứng. SSD chính còn khoảng190.5 GiB và NVMe phụ161.4 GiB trống. Người dùng ưu tiên PIF; Paradigm, UI Setup Wizard và Misc Tweaks giữ hành vi donor. Xem [artifact review](docs/m4-attempt37-artifact-review.md), [lưu trữ](docs/storage-cleanup-20261026.md). Các checkpoint dưới đây là lịch sử.

## M4 build — checkpoint0149, 26/10/2026

Attempt17 passed the NFC and CoreRune display paths after patches0144–0145, then
stopped at `m4-build-attempt17.txt:1074`: the generic Strongbox DeviceID patch
tried to rewrite `isSupportStrongboxDeviceID()Z`, although BZI6 already returns
false. Attempt16 and17 decode the class to the same SHA256
`2d4eda789aa30c266fa9a3c66a2abec140d18004157ae1d9fd72080928578e5e`.
Patch0146 adds a hash-pinned no-op only for M4/G965F/BZI6; exact-class positive,
changed-class, wrong-model and missing-class fixtures pass.

Attempt18 reached the next SAIV module after logging that Strongbox no-op at line
1072, then stopped at line1087 because the BZI6 donor has no legacy
`vendor/etc/midas/moire_detection/moire_detection.tflite` path. Its hash-pinned
`midas_config.json` references that relative path at line1531, while the exact
model is under `vendor/etc/VslMesDetector/moire_detection.tflite` with SHA256
`88e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1`.
Patch0147 stages those reviewed bytes at the referenced path with vendor
metadata, scoped to M4/G965F/BZI6. Attempt19 confirms the stage at log lines
1084–1086, including `uid:0 gid:2000 mode:755` and the vendor config SELinux
label.

Attempt19 then stopped at line1091: BZI6 PhotoEditor supplies two versioned
HumanSeg/BGPclassifier `.info` + `.tflite` pairs, while the generic branch asks
for `hs_segmenter.info`/`.tflite`. Patch0148 verifies the exact four-file set,
all four hashes and each JSON model path, then stages the whole donor directory.
Pair staging and changed-model/wrong-model negatives pass.

Attempt20 passed the pinned Strongbox no-op, Midas model staging and paired
PhotoEditor model staging, then stopped at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt20.txt:1097`:
`File not found: .../system/system/saiv/smartcropping_2.0/db/smartcrop_saliency_deploy.prototxt`.
BZI6 has no legacy `smartcropping_2.0` tree; its `IMAGE_CROPPER_VERSION=V1`
model is `system/saiv/imageCropper/sce_detector_cnn.tflite` (SHA256
`5ee6feae9f8307b7f0c363ec6f41e764ef97b5d2afeb83e72714659268b887f5`). The
target-specific vendor SCE model remains the pinned Exynos9810 baseline file
(SHA256 `f39455590d713701df4d051612d1b0baf427d66704e51eb404fd6fabb4498f1c`).
Patch0149 stages the donor system layout, removes only exact-hash G965F legacy
smartcropping data, and keeps that vendor baseline model.

Patch0149 positive staging/retention fixture and wrong-model, changed-donor and
changed-vendor negatives pass. `bash -n` passes; ordered replay matches 115
source files after139 patches and all138 earlier patch hashes remain unchanged.
Patch0149 SHA256 is
`7c7a8e940412b7c4ddc8a76ba2bc141ea767a07fd9ce6be51c285a7ebd722307`. A clean
attempt21 will use a dedicated M.2 temporary-image directory to keep root space
for the work tree and package output. The kernel parser warning at line956 was
nonfatal; no kernel source or selected DS-ACK package changed. No complete M4
image, flash or Android17 runtime proof exists; no phone was touched. Evidence:
`logs/20261026-m4-vendor-warning-0142/`.

## M4 build — checkpoint0144 (historical), 26/10/2026

Attempt13 and attempt14 did not finish and have no `build_exit` result; logs and
per-attempt configs are retained. Attempt15 reached NFC but the dispatcher still
requested the missing G965F PHN sidecar at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt15.txt:1038`. The attempt
config has `TARGET_PLATFORM_SDK_VERSION="31"` at line22, while patch0143 checked
29. Patch0144 changes only the exact G965F/BZI6 guard to SDK31. SDK31 positive,
wrong-model, wrong-SDK and changed-JNI fixture checks pass; shell syntax and all134
ordered patch replay pass with 111 source files byte-identical and prior133 patch
locks unchanged. Patch SHA256: `092ce3155bede5598cdbc0d2f8324925350864efd4d4dfa29ee0bda13ac11f47`. Attempt16 later confirmed the route at line1038; checkpoint0145 records the next blocker.
No M4 image or Android17 runtime is proven; no phone was touched. Evidence: logs/
20261026-m4-vendor-warning-0142/.

## M4 build — checkpoint0142, 26/10/2026

Attempt11 passed the BZI6 USB Host and Samsung RescueParty adaptations, then
stopped in the legacy vendor mismatch warning patch. Its caller expected
ActivityManagerService lambda6; BZI6 uses lambda7, and the handler hunk failed
against the changed lambda4 body. Exact errors are recorded at
logs/20261026-m4-rescue-0141/m4-build-attempt11.txt:1060,1138.

Patch0142 hash-pins the actual BZI6 caller/handler and suppresses only vendor
mismatch dialog dispatch (class ID3), preserving the diagnostic log, UID warning
(ID2), and handler cases0–2. It changes rom/scripts/internal/apply_modules.sh
and adds rom/unica/patches/miscs/a376b_android17/0001-disable-bzi6-vendor-mismatch-warning.patch.
The exact-source Smali fixture, Apktool services.jar rebuild, bash -n, and full
132-patch replay from the fork base pass. Patch SHA256:
2711d3dd2f857c5dee57ac55eec27f844dabe0644c5427e0c64881aeb96f6047.
Attempt12 used a fresh output path. This was a PC build only; no phone,
kernel, vendor, radio, or protected partition was touched.

## M4 build — checkpoint0143 (historical; guard corrected by 0144), 26/10/2026

Attempt12 exercised Patch0142 in the actual fork build. Log line1033 confirms
“Applied reviewed BZI6 vendor-mismatch warning suppression; preserved UID and
emergency dialogs.” It then stopped at line1039:
“(unica/patches/nfc/customize.sh:66:ADD_TO_WORK_DIR) File not found:
out/m4-run-20261026-attempt12/fw/SM-G965F_PHN/system/system/lib64/libnfc_prop_extn.so”.
The matching vendor extension is also absent from the locked G965F PHN source.
Both target core NFC libraries and target Samsung HIDL HAL are present; their
ELF dynamic dependencies do not require either sidecar.

Patch0143 added an exact profile route in rom/unica/patches/nfc/customize.sh. Its
initial guard expected target SDK29, later found stale because generated M4 config
uses SDK31. Attempt15 reproduced the fall-through; patch0144 corrects it. The
profile still verifies G965F JNI and BZI6 sidecar hashes, retains donor sidecars
only when both target sidecars are absent, and rejects partial or changed inputs.
Patch SHA256:
93dd48c8f59d2ed0e2841f5912f5bb56f6c26b5a4109bdd24a8ac28f26ed0b83.
Evidence: logs/20261026-m4-vendor-warning-0142/. This historical checkpoint was
superseded after attempt15; see checkpoint0144 above. No device was touched.




## M4 build — checkpoint0141, 26/10/2026

Attempt04 stopped because the Android16 P2P DEX signature occurs0 times in
BZI6. Patch0135 routed only exact M4 G965F/SDK37 through the reviewed decoded
Smali callback anchor. Attempt05 confirmed the real build takes that branch and
logged `Patched decoded Samsung Wi-Fi Direct callback selection; Seh registration retained`.
It then stopped at `logs/20261026-m4-p2p-0135/m4-build-attempt05.txt:926`:
`rom/unica/patches/camera/customize.sh: line 397: abort: command not found`.
BZI6 contains only `/system/lib64/libstagefright.so` (SHA256
`f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517`); its
old `ro.product.model` key occurs once. Patch0136 allows the absent 32-bit file
only for the exact M4 arm64-only profile when the 64-bit file exists, patches
the 64-bit binary and uses the fork's `ABORT` helper for failures. Attempt06
passed the P2P and stagefright branches, then stopped at
`logs/20261026-m4-camera-0136/m4-build-attempt06.txt:940`:
`(unica/patches/dvfs/customize.sh:74:SMALI_PATCH) Missing BZI6 class descriptor: U1/w.smali`.
Patch0137 maps the M4 SDHMS rule to its hash-pinned BZI6 class
`smali/n5/i0.smali` (APK SHA256
`53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`, class
SHA256 `39ff200b94cf1e141c3d02aed62709850648d3b914d12f0931c80f02a14afdd6`)
and keeps the AZH4 M2 route intact. Exact M4 positive, wrong-scope, mutated
APK/class and M2 fixtures pass; all127 ordered patch hashes matched. Attempt07
then reached the DVFS adapter and stopped at
`logs/20261026-m4-dvfs-0137/m4-build-attempt07.txt:940`:
`BZI6 SamsungDeviceHealthManagerService.apk is missing`. The guard used an unset
module-local cache path. Patch0138 now calls the fork's
`GET_FIRMWARE_CACHE_KEY "$SOURCE_FIRMWARE"`; it resolves the actual cache as
`SM-A376B_XXV_A376BXXU5BZI6`, whose APK hash matches the pin. Shell syntax,
actual cache lookup/hash, patch replay and all128 ordered patch hashes pass.
Attempt08 passed the cache lookup and patch0137 SDHMS class gate, then stopped
at `logs/20261026-m4-dvfs-0138/m4-build-attempt08.txt:1005` with
`RuntimeError: Unreviewed BZI6 libstagefright preimage`. The camera module had
already changed the model-property string, while the later media adapter still
expected the untouched donor hash. Patch0139 pins the post-camera intermediate
hash `005a887d...f7372`, verifies it reverses to donor hash
`f8ad6ae2...f517`, then applies the HEVC patch and checks combined output hash
`5f591074...2210`. Actual M4 fixture and original-donor rejection pass; all129
ordered patch hashes and reverse/reapply pass. Clean attempt09 is next in a new
output directory. No complete M4 package or Android17 runtime proof exists.
Details: `docs/patch-matrix.md` and
`logs/20261026-m4-media-0139/stagefright-composition-test.txt`.

## M4 scoped profile — checkpoint0131, 26/10/2026

Thêm profile G965F/no-KSU riêng, kế thừa gói duhansysl DS-ACK đã khóa, chọn
BZI6 suffix/runtime/metadata, fixed-resolution, donor Wi-Fi UI và boot limits đã
rà soát; mọi cờ M2 vẫn false. `gen_config_file.sh` thật chạy M2→M4→M2 trong
OUT cô lập, marker AZH4→BZI6→AZH4 và M2 config sau switch-back giống nhau;
`rom/out/config.sh` không đổi. Lần đầu guard báo chính xác
`M4 fixed-resolution preflight requires reviewed BZI6/star2lte/F.` vì suffix bị
M1 reset; profile đã khóa suffix trước khi target config chạy. Log:
`logs/20261026-m4-profile-0131/`.

Profile hiện chỉ chọn nguồn/flags; chưa có clean ROM build, flash hay Android17
runtime. Còn module/Knox dispatch, work/compiled-cache switch và boot/vendor/image
contracts. Patch0131 hash được khóa trong `sources.lock.json`.

## M4 donor-native runtime closure — checkpoint0130, 26/10/2026

Patch0129 vẫn giữ đường native32 riêng BZI6: APEX hash b6b559bf…5583cdaf,
runtime64 donor giữ nguyên,17 path runtime32 và48 thư viện platform32 đã khóa.
Patch0130 nối thêm donor-native A17 camera/audio/graphics V7/Tethering, vendor_kg
và aconfig-v2 qua restore/finalization/release/ext4 gate hiện có. ADB helper
được port riêng cho BZI6 `.locals 6`, source hash pin, DEX039 compiled fixture
và Rampart/SecSettings/SUW key-action checks PASS; M2 ADB output trùng byte.
Runtime helper, collision/overwrite negatives và real ext4 SYSTEM/VENDOR
fixture PASS; M2 metadata output trùng byte với helper trước0130. Replay120 patch từ commit2b25ae6 được kiểm lại thành công,105 file text trùng
byte và119 hash cũ không đổi. Log: `logs/20261026-m4-runtime-0130/replay-through-0130.txt`; ADB retest:
`logs/20261026-m4-runtime-0130/adb-final-retest.txt`.

Đây vẫn chỉ là source/image-fixture evidence: chưa có profile hoàn chỉnh, ROM,
flash hay boot Android17. Aconfigd, Tethering/BPF, camera/audio/graphics HAL,
full boot/vendor/image contracts và ADB/Auto Blocker fresh defaults/toggles cần
log runtime. Kernel, Hybrid Vendor, radio, modem, EFS/CPEFS và G965N không đổi.
Báo cáo: [docs/m4-native32-runtime.md](docs/m4-native32-runtime.md).

### Checkpoint0128 trước đó

Đã thêm donor config `essi_a376b_android17.sh` riêng: SDK37/shipping36/board34,
cache BZI6 và SIM tray runtime. Chạy gen_config thật trong OUT cô lập theo
M2→M4→M2 PASS; cache chọn AZH4→BZI6→AZH4, M2 khi quay lại trùng giá trị và
rom/out/config.sh cũ không đổi. Chưa tạo profile kernel/runtime M4 hoàn chỉnh.

0125 giữ eSE target-none như M2 bằng bốn class BZI6 đã pin, xử lý literal log
mới và kiểm DEX; không chọn COS mới.0126 giữ UI Wi-Fi donor qua cờ M4 riêng,
13 class source/DEX được kiểm; target phần cứng vẫn Wi-Fi5.0127 nối lựa chọn
Face/Bixby/5 tùy chọn UN1CA tạm bỏ: ẩn đúng control, giữ WINK và FaceManager
feature; không mở rộng các skip M2 khác. Search giữ211 provider donor +4 UN1CA;
Intelligence giữ36 khóa kể cả Buds, thêm đúng1 khóa UN1CA. Actual AZH4 control
search output không đổi.0128 thêm donor registry và khóa hash theo lựa chọn M4.

Replay118patch PASS,101file trùng byte;114hash trước giữ nguyên. Chỉ kiểm source,
config và DEX fixture trên PC; chưa build/flash M4. Bước tiếp theo là lock/stage
native32/runtime/APEX riêng M4, rà module/mod còn lại và hợp đồng boot/vendor/
image, rồi tạo profile đủ kiểm trước clean build. Rule ADB/Auto Blocker compiled
và fresh-data runtime vẫn bắt buộc. Chi tiết: docs/m4-source-selection.md.

### Checkpoint0124 trước đó

Đã thêm0121–0124 cho source BZI6: shipping API phần cứng36→26 (framework vẫn
SDK37), hai controller HDR, HFR default60/mode0, brightness3 và chế độ độ phân
giải cố định riêng M4 (native1440×2960/density560). Không đổi kernel/blob/radio.
Rune BZI6 dò cảm biến thật; giữ TYPE_LIGHT=5, không thay nhầm bằng profile3.
SMALI_PATCH định vị class theo đúng descriptor qua các DEX, scope BZI6/F/M4;
không đổi các nhánh M2. Profile M1/M2 xóa cờ M4 còn sót khi được chọn lại.

Fixture ghép helper lights→shipping/HDR→rear FP→display PASS, pin15 class/
19 method display và đóng DEX039 bằng tool hiện có của fork. Gate release mới
đọc DEX/XML thật; chặn donor HFR2 và XML mode2 còn sót. Replay114patch PASS,
95file trùng byte;110hash trước đó không đổi. Chỉ kiểm source/fixture trên PC,
chưa tạo profile, build image hay flash M4. Chi tiết: docs/m4-product-display.md.

Tiếp theo: chốt profile M4 cùng các lựa chọn Face/Wi-Fi/UN1CA UI/eSE, ghép
runtime/native32/APEX và hợp đồng boot/vendor/release, kiểm M2→M4→M2 trước
clean build. ADB/Auto Blocker compiled và fresh-data runtime vẫn bắt buộc.

### Checkpoint0120 trước đó

Đã thêm0119 cho BZI6 rear fingerprint: kiểm method/hash framework, services,
SystemUI, BiometricSetting đã khóa và Settings sau thay SPF; giữ true register
v3 của BZI6 cho các cờ SystemUI khác. Fixture Android16/17 và DEX assembly PASS.

0120 thích ứng bốn encoder BZI6 (MotionPhoto/GPPM/Camera/vexfwk) và đúng một
lệnh HEVC trong libstagefright. vexfwk_service có thật ở BZI6, dùng v2 thay vì
v3 của patch cũ; không bỏ qua APK. LLVM relocation/disassembly chứng minh nhánh
0xebdbc dẫn tới HDR10Plus assertion. Native chỉ đổi4byte, hash đầu ra khóa.
Gate release mới đọc DEX thực và kiểm native; fixture PASS, chặn được native
chưa vá và DEX donor vex còn default cũ. Chưa chứng nhận codec/runtime.

Replay110patch từ commit gốc PASS,93file trùng byte;108hash trước đó không đổi.
Lượt này chỉ sửa/kiểm source trên PC, chưa tạo profile/build/flash M4. Tiếp theo:
các nhánh product/display-feature, rồi profile/runtime/release wiring và kiểm
chuyển donor/cache. Xem docs/m4-dispatch-ledger.md và docs/patch-matrix.md.

### Checkpoint0118 trước đó

Đã thêm0116 cho WLAN target policy: bỏ đúng ba chuỗi bảng RF A37, giữ mã
HAL/coexistence, kiểm booster000 và dùng truy vấn chip cho SoftAP11ax. Fixture
chạy ghép với0114–0115 và đóng DEX PASS. Đây là source policy, chưa nghiệm thu RF.

Rà gate tìm thấy patch M2 signature và backlight không khớp BZI6.0117 sửa đúng
context try/catch, áp fuzz0 và giữ các kiểm chứng chỉ/platform identity hiện có.
0118 thích ứng lights trên cấu trúc mới: LogicalLight, setLightUnchecked và
mute/cache; chuyển thật tham số độ sáng mở rộng vào Parcel SehLights. Các
fixture source/negative/DEX đều PASS. Chưa kiểm các hành vi này trên M4 runtime.

Replay108patch từ commit gốc PASS,92file văn bản trùng byte;105hash trước đó
không đổi. Không đổi kernel/vendor/radio hoặc tác động điện thoại. Chưa tạo
profile/build/flash M4. Danh sách gate đã rà và còn mở:
[docs/m4-dispatch-ledger.md](docs/m4-dispatch-ledger.md). Tiếp theo: rear
fingerprint, OMX/media và các nhánh product-feature còn lại trước profile M4.

### Checkpoint0115 trước đó

Đã hoàn tất bước port source mDNIe, APE/personalization và ba nhánh SoftAP cho
BZI6 bằng patch0113–0115. Fixture donor Android16 và Android17 đều PASS, đóng
được DEX theo đúng writer API29/Dex039 hiện có của fork. Kiểm ghép theo thứ tự
module xác nhận chỉ hai lệnh APE/power-save đổi đích trong bảng dispatch;
permission check, HAL settings hợp lệ và Binder cleanup của country fallback
được giữ. Các trường hợp sai SDK, preimage hoặc thứ tự bị từ chối trước ghi.

Patch0112 sửa hash SIM của0110: fork dùng `--no-debug-info`, khác fixture debug
ban đầu. Đây là lỗi guard nguồn được phát hiện và sửa trước build. Bộ source
lock hiện có105patch; không thay hash của101patch đã khóa trước đó. Replay
toàn bộ105patch từ commit gốc PASS, đủ87file văn bản trùng byte với source.
Chi tiết
lý do, file ảnh hưởng, kiểm chứng và rollback ở docs/patch-matrix.md.

Chưa tạo profile/build/image/flash M4. M2 attempt56 vẫn là baseline. Bước tiếp:
rà RF/backoff cùng các dispatch M2-only còn lại, rồi tạo profile A17 riêng và
chạy kiểm đổi M2→M4→M2 trước clean build/compiled/image gates.

### Bằng chứng preflight trước đó

BZI6 extraction/cache isolation và patch0109/0110 đã có bằng chứng. Patch0111
thêm guard chỉ cho classifier/ARDoodle thực sự vắng trong BZI6; pin GPPM có
thật và kiểm final-work XML. Đã sửa hai kết luận audit quá mức: GPPM có APK và
file quyền; chuỗi JCOP5.2U nằm trong lookup nhiều COS của stock G965F, không
chứng minh COS được chọn. Replay đủ101patch từ commit gốc, source/negative
fixtures, syntax, whitespace và source-lock hashes PASS.

G965F M2 kết nối lại, boot_completed1 trên DS-ACK4.9.337. Các boot-SKU và
COS-default properties rỗng. BZI6 secilc compile split CIL đã thích ứng VNDK33
trong rootfs riêng exit0; cùng kích thước policy với host nhưng hash khác.
Giữ giới hạn: cờ-N bỏ neverallow, chưa load policy/chạy init/boot Android17.
Đã lưu output và dọn đúng rootfs tạm. Chưa có profile/build/flash M4. Tiếp theo:
hoàn tất donor feature/dispatch audits, profile riêng và compiled/image gates.
Tài liệu hiện hành: docs/m4-plan.md và docs/m4-donor-dispatch-audit.md.

## Cập nhật 21/10/2026 — flash fresh G965F đã PASS

8/8 phần ADB17 + explicit TWRP Format Data: UUID mới, Settings/adb_keys/probe
cũ đã mất trước boot. Fresh boot165s, reboot46s theo watcher; mặc định trước
mọi test writes: provisioned0/setup0, adb/development1, Rampart master/strict/
blocked0/SUW0. Keyless USB CNXN/shell PASS cả hai boot. Auto Blocker GUI bật/tắt
có tác dụng thật; USB debugging callback PASS, GUI còn chờ sau initial setup.
Máy vẫn Welcome, đã dọn helper và restore stayawake0; N không nhắm tới.

Root cause data cũ: mke2fs báo USERDATA mounted nhưng optional format che lỗi.
Patch0018 source + archive guard + behavioral/replay checks PASS, chưa đóng
vào gói đang chạy. Báo cáo hiện hành: docs/m1-fresh-report.md. Lỗi USB sớm còn
mở; kernel/Hybrid Vendor/RIL không đổi, không suy sang A37/N.


**M1 G965F đã cài và boot thành công; người dùng đã xác nhận.** ADB đúng máy F
báo Android16/SDK36/OneUI80000, kernel DS-ACK V1.12 May nguyên bản, boot_completed1,
bootanim stopped và ảnh Welcome. Xem [báo cáo runtime](docs/m1-boot-report.md).
Đây là đối chứng S22, chưa phải A37 OneUI9/Android17. G965N không flash.

## Kết quả thực

- Fork hiện tại Android-16 commit2b25ae6fc3098650be249bd1898e21e563cd5df1, 19 patch/30 file
  replay byte-identical; 41 submodule và4464 LFS payload đã kiểm SHA/size.
  Bộ công cụ host build PASS; apktool185 tests không fail/error/skip.
- S22EUX S901BXXSOGZH3 và G965F/PHN G965FXXUHFVG4 tải/trích/MD5/metadata/CRC PASS.
  Dùng Hybrid Vendor VNDK33, blob/cấu hình F riêng và cầu nối radio của fork.
- Compile attempt5/package attempt8 PASS; 503 archive/2937ELF được kiểm tĩnh.
  Outputext4, SELinux compile, model/order, CRC/CMS signature và kernel hash PASS.
- Kernel dùng gói duhansysl V1.12 ngày08/05/2026 Enforcing-OneUI7 không KernelSU,
  SHA9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072.
  Không build/sửa kernel hoặc đổi version. BOOT thực sau cài khớp repack RAM55MiB;
  kernel/DTB/ramdisk đúng payload khóa.
- FullZIP5479600816B SHA6a0096cee0750d0d47dcaa0605bac7b65d362c0c7c40950814683d427fb745eb
  bị TWRP9 đọc size modulo2^32, từ chối trước updater; GPT được kiểm chưa đổi
  ở lượt đó. Mode --legacy-sideload mở rộng chính script đóng gói của fork,
  chia8 ZIP32 <2GiB; giữ range/data order, Brotli round-trip, CRC/CMS PASS.
  Live native updater RAM test PASS sai thứ tự/no-write và partialranges.
- Theo ủy quyền trực tiếp, đã cài8/8 phần lên F, marker từng phần thành công;
  SYSTEM/VENDOR/ODM/CACHE thực9500/1300/700/600MiB; USERDATA format ext4.
  GPT primary/backup CRC và table khớp; prefix entry1–17 không đổi.
- User xác nhận “đã boot thành công”; watcher thấy boot_completed ở165s.
  Uptime176.21s và342.48s đều boot_completed1; runtimeVNDK33 và APEXv33 hiện diện.
  Radio1.6/bridge2.0 hai slot và HAL camera/audio/Wi-Fi/Bluetooth/sensors đăng ký.
  Đây là kiểm đăng ký, chưa phải test chức năng.

## Tồn đọng phải giữ nguyên

- Audit trước boot **FAIL vì EFS đổi raw hash**; CPEFS/RADIO/RECOVERY/BOTA0/1/2
  không đổi. Chưa chứng minh EFS chỉ thay metadata. Sau khi được báo và hỏi
  backup, user trả “tiếp tục đi, máy dev thì backup làm gì”: đã tiếp tục boot
  theo ngoại lệ ghi efs-boot-exception.json; không restore, không đổi audit PASS.
- getenforce=Enforcing nhưng su có avc permissive1; ADBroot/debuggable1/secure0.
  Chưa nghiệm thu policy/security release. Không tự setenforce hoặc đổi policy.
- USB/ADB ngắt/kết nối; log init USB UDC báo No such device và symlink File exists.
  Đã thu logcat/dmesg ban đầu, snapshot HAL và logcat sau boot; dmesg bổ sung thất bại
  khi transport mất. Không kết luận máy reboot từ ADB mất.
- Hai SIM ABSENT; baseband đọc được nhưng gọi/data/IMS và toàn bộ chức năng phần
  cứng còn chưa test. Một RIL exit0 trong boot, sau đó daemon/HAL chạy trở lại.
- Hai snapshot logcat không có mẫu fatal exception/signal/ANR hoặc VINTF incompatible;
  dmesg ban đầu không có mẫu kernel panic/BUG/Call trace. Phạm vi chỉ log đã thu.

## Mốc tiếp theo — xem cập nhật M2 bên dưới

Gate boot M1F đã được user xác nhận. Chưa flash milestone tiếp theo hoặc N.
Cần hoàn thiện kiểm chức năng và xử lý/đánh giá tồn đọng M1 trước lượt thử tiếp;
M2 cần donorA37 Android16, M4 mới dùng A37Android17, giữ một biến mỗi mốc.
Đầu vào A37Android17 đã kiểm tĩnh release17/SDK37/OneUI90000/framework37,
chưa build/boot; A37Android16 và nguồn blob/CSC N còn mở.

Evidence chính: docs/m1-boot-report.md, docs/m1-artifact-review.md,
logs/20261020-m1-g965f/flash-01/{legacy-status,preboot-audit,efs-boot-exception,
boot-status,boot-analysis,postboot-analysis}.json. Raw log và binary chỉ cục bộ,
bỏ Git; không commit/phân phối firmware, không viết build system mới.


## Rule ADB cho các build sau

User đã chốt rule lâu dài trong AGENTS.md/PLAN: default Auto Blocker OFF,
USB debugging ON, USB ADB không hỏi RSA key, giữ công tắc/description thật.
Patch0017 common finalization + release verifier đã rebuild SettingsProvider/services
và qua compiled/negative/release/class-preservation checks. Default USB mới
sec_charging,adb theo trạng thái đã cấu hình thành công ở M1; hiệu quả USB cần
log candidate. Chi tiết docs/adb-defaults.md, logs/20261020-adb-defaults.
Candidate17 đã flash fresh trên F ngày21/10/2026 và qua default/keyless tests;
chi tiết m1-fresh-report.md. Source có thêm patch0018 cho lần package sau.
Không thay kernel/RIL/EFS hoặc ghi N trong lượt này.

## Hiện tại — M1 đã lưu, M2 chờ file đầu vào

User yêu cầu chuyển sang build M2 trên PC và giữ mọi ROM M1/M2/M4/M5 không ghi
đè. M1 ADB17 đã archive đủ10ZIP/16.888.429.178byte, SHA/size nguồn-đích PASS:
inputs/rom-releases/M1/20261021-g965f-adb17-ba4ed009bd98/.
Patch0019 retention có7fixtures PASS, replay19patch/30file byte-identical PASS.
ROM archive/running chứa17patch; checkout19, không đồng nhất hai trạng thái.

M2 chưa build: chỉ có A37Android17 trong dự án. User đang tải Full Files
SM-A376B/XXV/A376BXXS5AZH4 Android16 từ SamFW. Trang ghi OneUI8.5, cần xác minh
payload. Chưa tạo config/profile A376B16, chưa có M2/M4/M5 hoàn chỉnh; thư mục
trống dự phòng không phải ROM. Tiếp tục kiểm hash/CRC/SDK/OneUI khi file đến,
thêm donor song song S22 và build qua fork; kernel/vendor/radio giữ contract.
Xem docs/m2-build.md và docs/release-retention.md. Không thực hiện ghi điện
thoại trong lượt chuẩn bị này; quyền M1 không tự mở rộng sang M2/N.

## Cập nhật — M2 build sạch đã bắt đầu

Gói user tải ở Downloads đã MD5/SHA/CRC PASS, cả6Odin MD5 PASS, trích sudo exit0,
metadata sáu partition + CRC607 archive PASS. Payload AZH4 là Android16 SDK36.1,
OneUI80500. Patch20 LOCAL input,21 reviewed ADB lifecycle và22 donor selector/
cache/config đã kiểm; replay22patch/33file PASS, S22 config không đổi.
Build sạch M2 F đang chạy (logs/20261021-m2-build/attempt1.txt), kernel/Hybrid
Vendor/radio contract giữ nguyên. M1 archive đủ10ZIP chỉ đọc. Chưa có ROM M2
hoàn chỉnh hoặc log boot M2; không tự mở rộng quyền flash từ M1.

## Build M2 attempt2

Attempt1 FAIL camera prepare ABI (attempt1.txt:65); patch0023 giữ ABI A376B và
chỉ thêm null-manager guard, actual S22/A376B compiled fixture PASS. Replay23/
34file PASS, attempt2 clean đang chạy. Full A376B services.jar + ADB rebuild
riêng PASS, không sửa minSdk/SDK. Chưa có full M2 ZIP hoặc ghi thiết bị.

## Build M2 attempt3

Attempt2 FAIL inherited optional action-classifier claim, không phải kernel.
Patch0024 chỉ xóa claim không có implementation ở AZH4; five fixtures PASS,
replay24patch/35file PASS. Attempt3 clean đang chạy; chưa có full ZIP hoặc boot M2.
Warnings Bluetooth patterns/unsafe lens guard được giữ trong bugs.md để kiểm
runtime. Source Rampart/SUW key bindings đã review; không coi là test thiết bị.

## M2 AZH4 SehLights patch and build9

Attempt8's exact SehLights patch failures are documented in `docs/bugs.md`.
Patch0030 adds a guarded A376B16 AZH4 adaptation using the donor's VintfHalCache;
actual donor services.jar rebuilt with Apktool in preflight, evidence and SHA256
in `logs/20261021-m2-build/sehlights-a376b-preflight.txt`. Patch record/hash is
in `patches/fork/0030...patch` and `sources.lock.json`. Clean attempt9 is running.
No M2 ZIP, device flash, or hardware-light runtime test is claimed yet.


M2 attempt9 was interrupted mid-overlay and produced no package; attempt10 clean
passed the A376B SehLights adapter and Face omission, then failed at
`attempt10.txt:1000` because AZH4 MotionPhoto lacks the legacy
`configVideoEncoderParameters(...)` method. Patch0031 now guards only that edit
for the exact A376B16 profile; strict upstream behavior remains for other donors.
Recheck method/patch replay, then clean build11. No M2 ZIP/flash/runtime claim.

M2 attempt11 passed the MotionPhoto method guard and applied the donor
EncoderFilter format edit, then stopped at `attempt11.txt:1003-1005`: AZH4 has
no 32-bit `system/lib/libstagefright.so`, and its 64-bit file matches neither
known legacy signature. The G965F target vendor exposes OMX video codecs, so no
blind native patch or Android10 system-library substitution is made. Evidence:
`logs/20261021-m2-build/media-stagefright-a376b-review.json`. Awaiting direction:
port by disassembly, omit only the legacy OMX path with explicit limitations, or
keep M2 blocked. No ZIP, flash, or runtime claim.


## Current continuation — 21/10/2026, after attempt22 interruption

Attempt22 was interrupted while decoding SamsungCamera; no live build and no M2 ZIP. Stagefright disassembly port is statically verified: HEVC HDR10Plus assertion branch at 0xe5654 is changed to NOP, isolated output unchanged elsewhere. Patch0040 GameDisplayListener line-mode edit now passes actual helper execution plus Apktool rebuild. Audit patch0030 had malformed diff metadata; repaired record and lock SHA now replay PASS for all 40 patches / 40 source files. Shell and fork patch-layout checks pass.

Next blocker is preflighted: Settings dynamic-resolution patch spans 57 files and fails against AZH4 with 63 error lines. Awaiting user choice: port full WQHD/FHD/HD or temporary fixed-resolution M2. No fallback has been applied. Whole-ROM compiled ADB/defaults, release checks, package/provenance audit and device validation remain pending. M1 retained packages remain untouched. No phone operations performed during this continuation. Evidence: logs/20261021-m2-build/continuation-static-audit.json.


## M2 fixed resolution selected — 21/10/2026

User selected temporary fixed resolution for the M2 boot test. Patch0041 sets the existing dynamic-resolution target flag false only for the M2/A376B Android16 profile; no generic resolution/Settings/SystemUI switcher patches run in that profile. The fork's native star2lte baseline is 1440x2960 with ro.sf.lcd_density=560 and ro.sf.init.lcd_density=560; it remains selected without forced-size boot scripts. Clear the inherited WQHD/FHD/HD feature claim early and in both final system/vendor floating_feature.xml files; final audit rejects a nonempty switcher claim. The donor Settings screen-resolution controller remains unavailable; actual panel resolution/visibility requires runtime verification.

Scope: target/profile files and existing device-stack feature edits/audit only. M1 remains dynamic; wrong-donor/milestone fixed mode is rejected. Config fixtures pass M1=true, M2=false and invalid profile rejection. All 41 source patches replay to 40 byte-identical files; shell syntax passes. Mandatory ADB defaults/compiled code checks stay required. Next clean build23 with full ROM packaging requested through the existing fork; no device actions.


## Current M2 checkpoint — product-feature preflight0042–0048

Clean attempt23 exited1 at HDR branch (`attempt23.txt:1053`); no ZIP M2. Fixed-resolution selection remains1440x2960/density560, target60Hz, no switching UI. Scoped patches cover exact DEX relocation/HDR controllers, rear capacitive FP, AZH4 HFR thresholds, actual HW mDNIe methods, identical StrongBox false return, Wi-Fi backoff and consistent late Face omission. Replay48patches/44files and layout154 PASS; local actual donor preflights pass those edits. Whole build/compiled release/runtime gates remain open.

Current unpatched blocker is Wi-Fi80211AX UI. Even exact descriptor relocation fails (Settings8/9 resolved +16errors, SystemUI3/3 +6errors); one lambda absent. User preference question pending: port complete Wi-Fi UI now or temporary donor UI for M2 boot, potentially showing Wi-Fi6 scan labels. No bypass applied. Separate APK assembly/ADB audit is ongoing. Incomplete debug work tree must not be packaged. M1 archives unchanged; no phone actions this continuation.


Checkpoint0048 verification completed: all six actual-tree artifacts (framework.jar, services.jar, BiometricSetting, SettingsProvider, SecSettings, SystemUI) rebuild through the fork's Apktool pipeline with exit0. After applying the existing ADB defaults recipe to this diagnostic tree and rebuilding services/SettingsProvider, the verifier reports `PASS compiled ADB defaults, no-auth properties and stored-choice lifecycle`. Compiled rear-FP constants and actual restored mDNIe methods pass independent disassembly checks; standard Face variant audit passes. These are diagnostic-tree static results, not a clean whole-ROM release pass or runtime result. Evidence `product-feature-checkpoint48.json`, `compiled-hardware-preflight.json`, `adb-current-compiled-verify.txt`, `face-current-compiled-audit.txt`. All10 retained M1 manifest files still exist with original sizes; no archive writes. Current Wi-Fi UI question remains pending; no further clean build/ZIP/flash.


## M2 donor Wi-Fi UI selected — 21/10/2026

User explicitly selected temporary donor Wi-Fi UI for M2. Patch0049 adds `STAR2LTE_M2_KEEP_DONOR_WIFI_UI=true` in M2 and resets it false in M1; the existing product_feature recipe validates M2/A376B16/star2lte/Wi-Fi5 scope and skips only the two old 80211AX UI diffs for Settings/SystemUI. It leaves source metadata, target Wi-Fi flags, driver/HAL/blob, custom-backoff adaptation and all other Wi-Fi recipe branches intact. Donor scan labels/icons may still show Wi-Fi6; that is a known temporary UI limitation, not S9+ hardware support. No extra UI bypasses are authorized by this selection.

Actual branch fixtures PASS selected0patch calls / strict2calls / wrong-donor and wrong-milestone rejection; M2-to-M1 flag reset PASS. All49 audit patches replay to44 byte-identical source files. Clean attempt24 is running through `make_rom.sh --clean-work-dir --build-rom-zip`, log `logs/20261021-m2-build/attempt24.txt`. Full release gates, retained run packaging and M2 runtime remain pending. No phone actions; pinned kernel/Hybrid Vendor/radio unchanged.


## M2 continuation after Wi-Fi UI decision — patches0050–0053

Attempt24 clean stopped at `attempt24.txt` with `Method "getMainInstance()Lcom/samsung/android/hardware/display/RefreshRateConfig;" not found ... SemInputFeatures.smali`. This was a local0044 regression: its METHOD loop variable leaked into SMALI_PATCH replaceall's method check. Patch0050 renames/unsets the loop selector and checks methods only for operations with a method argument; field-only replaceall with an unrelated caller METHOD passes, missing real methods still fail.

Diagnostic continuation then logged the user-selected donor Wi-Fi UI and stopped at `product-tail-after-input.txt:94-95`, `SemWifiInjector.smali:1641: patch does not apply`. Patch0051 ports the existing target APE/personalization=false behavior on actual AZH4: removes only the SHA-checked APE construction region, verifies no external label references, keeps null guards, disables the exact L4s default, disables the Qbox APE observer, and splits only SUPPORT_APE ordinal35 from a shared SQE branch. The moved Settings develop/ApePreferenceController is disabled and its reset seed becomes0; old ape map entries are verified absent. No scan icon/label changes. Four classes change; unknown enum fails before writes. Native/Settings runtime needs logs.

Patch0052 ports SoftAP target differences after old AX/country/power/sharing diffs fail. The AX feature1 case uses AZH4's existing public driver query, with null fallbackfalse. Country fallback applies only for missing/NA HAL values and preserves permission enforcement, valid HAL results and Binder identity cleanup on success/exception. Power support reads the driver string instead of compiledtrue; worker initialization is disabled as in the original fork, and only SQE power command ordinal29 is split to unsupported. Sharing-lite is conditional on sharing and retains carrier/model exclusions. Strict method hashes and command mappings are checked; unused callback classes retained without synthetic-ID guessing. Country/power/sharing fixtures and no-partial-write rejection pass. Actual product-feature diagnostic remainder passes. Separate jar assembly succeeds; actual-work-tree jar rebuild is also being checked. Runtime remains open.

SAIV then stopped at `remaining-modules-preflight.txt:5` missing `vendor/etc/midas/moire_detection/moire_detection.tflite`; A376B stores the identical model under VslMesDetector, SHA256 `88e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1`, which matches the older model exactly. Patch0053 verifies/copies the A376B bytes to the path still referenced by donor Midas config, using explicit parent/file metadata. A later missing generic hs_segmenter.info is addressed by validating/copying AZH4's intact two named info/model pairs. SAIV diagnostic preflight now passes; PhotoEditor runtime still needs logs.

All53 patches replay to47 byte-identical source files. Diagnostic continuations use the existing fork module/helper entry points and do not mark .completed or package the partial tree. After remaining module preflights, a fresh full build is still required. Mandatory ADB defaults/compiled audit, pinned duhansysl kernel and Hybrid Vendor/radio remain required. No phone actions or completed M2 ZIP; M1 archive untouched.

## M2 continuation — 22/10/2026

Clean attempt41 ended exit1 at Settings search factory (`attempt41.txt:1489`). Patches0071–0076 handle reviewed Settings/search/MultiSound/setup adaptations and temporary optional-feature omissions; replay76/58files PASS, search/MultiSound/setup class DEX assembly PASS. Settings tail, supported Settings hooks, tweaks/SystemUI, wallpaper and Weather diagnostic continuations PASS. Final vendor restore/finalization diagnostic is running in `logs/20261021-m2-build/finalstages76.txt`. Staging remains incomplete; no M2 release ZIP and no phone action. Next: finish final-stage preflight, clean build, compiled mandatory ADB defaults/control checks, kernel/vendor/radio audits and retained package publication.

Latest M2 state — 22/10/2026: final-stage diagnostic ended exit1 at `finalstages76.txt:90` (missing system32 graphics interface). Audit finds a wider ABI mismatch: A37 has arm64-only app/framework/runtime, while hybrid vendor includes ELF32 audio/camera/OMX services and selects zygote64_32. Native32 runtime/namespace work is required; app32 versus app64 scope awaits user. See docs/m2-abi-review.md. No ABI check bypass or payload swap, no attempt42, no complete M2 ZIP; M1 archive not modified. Patches0071–0076 remain replay-verified diagnostic adaptations, including provisional optional Settings omissions.

Current M2 state — 22/10/2026 (supersedes the preceding pending-decision entry): user confirmed app64-only and hiding five unsupported optional controls. Patches0077–0080 prepare pinned native32 runtime while preserving all original A37 runtime files and the fork radio wrapper, remove unsupported eSIM MEP declaration, adapt SecureFolder namespace and scope S22-specific final workarounds. Replay80/61files PASS; diagnostic final-last80 exit0. Clean attempt42 is running detached through the original fork pipeline. Full rebuild, compiled mandatory ADB/Auto Blocker checks and release/package audits remain pending. No complete M2 ZIP or phone action. See docs/m2-abi-review.md and docs/m2-build.md.

Latest: clean42 failed at Photo Editor inventory using S22 path (attempt42.txt:2393). Patch0081 recognizes AZH4 PhotoEditor_Full; donor/stage hashes match and inventory helper PASS. Replay81/61files PASS; remaining finalization diagnostic exit0. Clean attempt43 now running detached via original pipeline. No completed M2 package or phone action. M1 archive10 ZIP hashes/sizes rechecked PASS.

Attempt43 compiled partial audits: rebuilt SettingsProvider fresh-data seed calls/SQL PASS (settingsprovider-compiled43.json): global adb_enabled/development_settings_enabled=1, Rampart defaults0, INSERT OR IGNORE. Actual rebuilt SecSettings resource XML absence audit PASS for all5 omitted optional controls, with Bixby positive-control XML present (optional-controls-compiled43.json). These do not replace the pending full services lifecycle/boot/release checks. Build continues APK/JAR rebuild; no completed M2 ZIP or phone action.

Attempt43 ended exit1 after29min10sec at the compiled boot contract: missing UN1CA Bixby remapper in system/system/framework/services.jar. All APK/JAR rebuilds completed; final wallpaper and actual rebuilt SystemUI Brief/Edge Lighting gates PASS. Earlier0036 intentionally skipped the incompatible A37 key-event remapper, but Settings controls and USER keylayout mapping remained; gate correctly blocks this inconsistent feature. User has been asked whether to omit/hide remap with source keylayout retention for M2 or port it fully; no choice assumed and no further Bixby patch applied.

Independent read-only release-default checks PASS in release-defaults-independent43.txt: pinned native32 payload/zygote, selected Face omission, no-KernelSU runtime, compiled ADB properties/seeds/stored-choice lifecycle, Infinity and PIF defaults. Its Bixby resource-presence check is only UI inventory and does not prove functional remapping; the full boot contract remains FAIL. No complete M2 ZIP and no phone action. Current compiled tree retained; .completed/release publication not bypassed.

Current: user confirmed omit/hide M2 Bixby remap, original keylayout.0083 source/compiled gates prepared, replay83/62files PASS. Exact reviewed delta applied to attempt43 decoded Settings and2 keylayouts; existing fork resume attempt44 running detached with rebuild+all gates+unique package. Old compiled Settings rejection PASS. No complete M2 ZIP, no phone action.

Attempt44 rebuild was deliberately stopped via its own process group after1min54sec, before gates/package, to use existing fork UN1CA_APKTOOL_JOBS=4 on72 logical CPUs/53GiB available RAM. Its trap misleadingly prints Build completed on SIGTERM; attempt44.interrupted is authoritative, no exit record or package completion exists. Attempt45 is the same original --resume-after-mods --build-rom-zip with4 existing Apktool jobs, detached; no build-system/payload change.

Attempt45 reached SELinux compile and failed on stale S22 CIL allow `otapreopt_slot -> vendor_apex_file` (`plat_sepolicy.cil:78269`); patch0084 removes only this undeclared A37-domain rule. Actual helper, Exynos9810 verifier, full staged `secilc` and84-patch replay PASS. Attempt46 resumed the existing fork packaging pipeline and exited0. All compiled/release checks PASS, including mandatory ADB/Auto Blocker lifecycle, Hybrid Vendor VNDK33/radio, no-KSU pinned DS-ACK, enforcing SELinux, ext4 images, ABI and keylayout/Settings gates. Release retained at `rom/out/releases/M2/star2lte-1792642788-JFvPsK/`; both ZIPs SHA256-check and pass `7z t`. Flashable SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`; target-files SHA256 `d4c3b69fe96f39888f15b3671633b16db6258f99f620263794a7805ebcbb5efe`. PC build/package complete; M2 flash/boot/runtime validation remains pending and no phone was touched. M1 archive remains untouched. Full details in `docs/m2-build.md` and `logs/20261021-m2-build/attempt46.txt`.


Latest M2 runtime status — 22/10/2026: one-time user-authorized fresh flash on G965F (`M2_G965F_SERIAL`) completed via eight existing legacy ZIP32 stages because TWRP rejects the >4GiB Zip64 package. Stage08 retry2 exited RC=0 and formatted fresh USERDATA; package hash remains `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`. The phone repeatedly restarted during system boot and is now in TWRP. Read-only logs are saved under `logs/20261022-m2-g965f-flash-01/`. `/proc/last_kmsg` only shows repeated bootloader starts ending in `Starting kernel...`; pstore has no console ramoops, and no Android failure signature was captured. A single controlled boot retry never exposed G965F through ADB in75s; host saw Samsung MTP `04e8:6860`, while only G965N recovery remained in `adb devices`. Root cause undetermined; M2 boot/runtime acceptance FAIL, not accepted. G965N and M1 archive untouched; no further flash/format/reboot. See `docs/m2-build.md`.


Continuation audit 22/10/2026: comparison of retained M1 versus M2 flashable ZIP entries confirms `boot.img`, `exynos9810/ramdisk.img`, `kernel.zip`, `install` support/preflight/layout/vendor_bootfix payloads are byte-identical where present; only `exynos9810/format_partitions.sh` differs among the compared installer entries. Exact SHA evidence: `logs/20261022-m2-g965f-flash-01/m1-m2-installer-comparison.json`. This does not prove the on-device BOOT was installed identically and does not establish a cause. Current PC ADB inventory is empty; waiting for G965F TWRP connection to read and unpack actual BOOT and compare against the known-good M1 boot evidence. No kernel change or device write made during this audit. The fork already contains optional early/late diagnostic logger paths; their suitability must be reviewed before any diagnostic-image proposal.


Recovery audit02 — 22/10/2026: G965F reconnected and verified as SM-G965F / G965FXXUHFVG6 / TWRP3.7.0_9-0. Read-only full BOOT capture (57,671,680 bytes) matches the M1 flash03 known-good BOOT **byte for byte**, SHA256 `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Kernel/DTB/ramdisk hashes match M1 independently. Samsung legacy header is not accepted by the AOSP unpack tool (`UnicodeDecodeError` decoding header); used the established M1 verifier offsets to inspect it instead. Nine installed early userspace files (init, bootstrap linker/libraries, vendor fstab pair) match the staged M2 tree; SYSTEM/VENDOR mounted `ro,noload` for hashes then unmounted. Existing recovery-mounted data only contains lost+found/media, with no Android system/misc/tombstone or bootloop logger directories. Evidence: `boot-audit-recovery-02/{boot-comparison.json,early-files-comparison.json,recovery.log,last-kmsg.bin,pstore/}`. No partition writes or reboot in this audit.

Ramdisk `/init` is the unchanged statically linked Android29 first-stage binary. This is inventory only: compatibility with the A37 second-stage init remains unproven and must be diagnosed by early-boot logs. Existing `patch_bootlogger.sh` must not be invoked as-is: it replaces PID1 `/init`, while current `bootlogger_init.c` explicitly implements a standalone logger that must never replace/chain Android init. That source/installer mismatch makes it unsuitable as an unreviewed diagnostic shortcut. No logger image has been installed.


Repeat fresh M2 flash02 (22/10/2026, direct user request): all eight hash-verified existing ZIP32 stages installed sequentially on G965F serialM2_G965F_SERIAL; each updater RC=0 with correct next-stage state. Stage08 ran from RAM without retries. USERDATA UUID changed from `d9175a13-acb8-4ea3-9c85-14333b55afe9` to `74d8755a-7aff-4d40-93da-20b57bda9b97`; preboot read-only mount shows only lost+found. Actual BOOT SHA remains `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Fresh install verified; system boot watcher now running. Evidence: `logs/20261022-m2-g965f-flash-02/`. Runtime result pending; no N actions or kernel changes.


Flash02 runtime follow-up: user confirmed the fresh test still bootloops. System ADB did not become available during the observation window, so Android logcat/dmesg could not be retrieved. User was asked to return G965F to TWRP for post-failure read-only capture. All eight installer stages RC=0, UUID-change and empty-data proof remain valid; installation success does not imply boot success.


Flash02 postboot capture complete: G965F verified again in TWRP (`SM-G965F`, `G965FXXUHFVG6`, serialM2_G965F_SERIAL). Raw read-only logs in `logs/20261022-m2-g965f-flash-02/postboot-recovery/`. 2MiB last-kmsg trace records repeated normal boots 07:31–07:34 UTC then recovery; line22392 is `Starting kernel...`. Pstore only has pmsg-ramoops-0 (16,372 bytes), with adbd FunctionFS/USB endpoint shutdown messages, including `remote usb: read terminated (message): Cannot send after transport endpoint shutdown`; this does not establish a system-boot cause or which boot produced every record. No console-ramoops/panic/Android init failure captured. USERDATA still has the new UUID74d8755a-7aff-4d40-93da-20b57bda9b97 and only lost+found/media after recovery creates its media directory. Fresh install did not resolve the bootloop; runtime gate remains FAIL, root cause unconfirmed. Phone left in TWRP; no additional write/reboot during this capture.


Patch0086 / early diagnostic prepared on PC: new optional offline helper `rom/scripts/internal/prepare_star2lte_boot_diagnostic.py`, no automatic build activation. Only legacy BOOT command-line field changed; pinned kernel/ramdisk/DTB/tail unchanged. Diagnostic SHA `a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9`, rollback SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Wrong-input/no-output, output-collision and syntax checks PASS. No image flashed. Review: `docs/m2-early-diagnostic-review.md`; actual pstore/fatal behavior needs device evidence. Await user authorization for this distinct BOOT diagnostic plus rollback, G965F only.


Early diagnostic test01 authorized and installed on verified G965F on22/10/2026. BOOT full readback matches a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9. Only command-line field changed; no kernel/DTB/ramdisk replacement or data format. Boot observation in progress; preserve captures then restore pinned baseline. Evidence `logs/20261022-m2-early-diag/test-01/`. AGENTS.md now reflects user's project flash authorization; F-first/N-reference plan stays.


Early diagnostic test01 completed22/10/2026: user confirmed bootloop and returned G965F to TWRP. Device identity and diagnostic BOOT SHA were verified; `/proc/last_kmsg`, `/proc/sec_log`, pstore and recovery logs captured under `logs/20261022-m2-early-diag/test-01/postboot/`. No console-ramoops or M2 fatal/panic signature recovered. Logs contain recovery kernel `Linux version 4.9.118-g36e6ddff22b9`; fstab errors (`/etc/recovery.fstab`) and hung-task lines belong to a recovery trace and are not evidence of M2 failure. Diagnostic effectiveness unproven, root cause unresolved. Original BOOT restored and entire partition readback SHA verified `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; no post-rollback reboot. Phone left TWRP. No kernel change, no data format, no N operation. Result JSON in test01. Next investigation must improve capture before/through early kernel/init rather than repeat unchanged fresh installs.


Patch0087 / explicit PID1 pmsg probe02: optional raw-syscall AArch64 monitor in `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`, prepare helper extended for cpio. Original init preserved as /init.real; probe keeps PID1 then execs original with argv/env, child copies selected kmsg errors to pmsg only, no FS mounts/partition access. TWRP fork/wait/exec/env self-test PASS, non-PID1 refusal13. Existing DS-ACK MagiskBoot repacks RAM files. Independent CPIO audit preserves all other data/metadata; kernel/DTB/cmdline unchanged. BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926. Diagnostic only: timing/SELinux can affect capture; compare results cautiously. Plan: one verified G965F boot under current project authorization, capture then rollback baseline cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d. Evidence `logs/20261022-m2-early-diag/pid1-probe-02/`.


Probe02 installed under current project flash authorization on verified G965F; readback full BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926 PASS. One system boot requested. No system ADB observed in the initial90-second window; waiting for user-assisted return to TWRP to capture pmsg/last_kmsg/sec_log and restore baseline. No new success/root-cause claim; probe effectiveness unknown until logs are collected. Probe remains installed pending recovery access. Patch0087 reverse/apply preflight PASS.


Probe02 completed: verified G965F returned to TWRP; probe BOOT hash matched. Captured pstore (empty), last_kmsg/sec_log/recovery/reset reason under `logs/20261022-m2-early-diag/pid1-probe-02/postboot/`. No M2-PID1 marker or M2 fatal recovered; absence does not distinguish pre-init failure from unavailable pmsg/sysfs/dev nodes or recorder failure. Restored baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d with entire-partition readback PASS, no reboot. Phone TWRP.

Patch0088 is an unflashed follow-up prototype: isolated monitor mount namespace for only tmpfs/sysfs so original init mount table stays untouched. RAM preflight FAILED `NAMESPACE stage=4 errno=016` (sysfs mount EBUSY), exit20; parent mount table SHA unchanged. Do not flash this prototype; no diagnostic image03 created/installed. Evidence `logs/20261022-m2-early-diag/pid1-probe-03/namespace-selftest.txt`. Root cause still unresolved; preserve pinned kernel and baseline BOOT.

22/10/2026 probe04 and probe05 completed on G965F. Probe04 pmsg and probe05 CACHE persistence passed in TWRP, but neither captured an M2 PID1 runtime marker after the bootloop. Probe05 CACHE file still held only its TWRP self-test line. Full BOOT readback after each rollback matches pinned baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; phone is in TWRP. Bootloader logs show `Starting kernel...` but no Android kernel/init failure line. Offline A37 init bootstrap dependency closure resolved all 35 needed names in 36 scanned ELF files; this excludes a simple missing bootstrap library, not other init/SELinux failures. M2 runtime gate remains FAIL; no G965N flash or kernel change. Evidence and limits in `docs/m2-build.md` and `docs/bugs.md`.

23/10/2026: M1 control fresh boot PASS and probe06 recorder calibration PASS (`/cache/m2-early-boot-probe06.log` has PID1 marker and init lines). M2 fresh install 8/8 PASS on G965F, USERDATA UUID new, protected partition hashes unchanged during install. M2 with same calibrated recorder bootlooped: 240 s no ADB; CACHE contains 23 PID1 markers but no true init/fatal line, last selected kmsg ~0.84 s. Exact reset cause still unknown. Diagnostic BOOT rolled back to pinned baseline SHA `cc618abc...776ed01d` full readback; G965F remains in TWRP with M2 system/data. G965N untouched. No kernel build/edit justified.

23/10/2026: Bước chẩn đoán probe07 hoàn tất trên hai G965F song song. Máy đối chứng mới M1_G965F_SERIAL cài M1 8/8, boot completed với CACHE marker/heartbeat. Máy M2 M2_G965F_SERIAL vẫn bootloop: 240 s không ADB, CACHE 19 PID1 markers nhưng không heartbeat. Cả hai BOOT đã restore baseline SHA `cc618abc...776ed01d` full readback; hiện đều ở TWRP. Chưa có log fatal xác định nguyên nhân M2; chưa sửa kernel/vendor/radio. G965N chưa tác động.

23/10/2026: Probe08 đã chạy đủ trên hai G965F. M1 boot thành công, 17 heartbeat đến ms1761; M2 bootloop 240s không ADB, CACHE 19 PID1 markers và tối đa một heartbeat mỗi segment. Cả hai BOOT đã restore baseline SHA `cc618abc...776ed01d` full readback, đều ở TWRP. Chưa có fatal line xác định root cause; M2 gate FAIL, G965N chưa tác động.

23/10/2026: Probe09 hoàn tất. M1 control serial `M1_G965F_SERIAL` boot completed; M2 serial `M2_G965F_SERIAL` không có ADB sau 240s. M2 CACHE ghi `M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1`, không có `M2-INIT-EXEC-FAILED`, nên exec `/init.real` đã thành công ở các segment được ghi. pmsg 1817 byte được thu trong phiên TWRP sau bootloop nên adbd/USB records không gán được cho M2; chưa có panic/fatal M2 xác định. Thu CACHE/last_kmsg/pmsg và SHA lưu ở `logs/20261023-probe09/`. Hai BOOT restore baseline `cc618abc...776ed01d`, full-partition readback PASS; hai máy ở TWRP. M2 root cause còn mở; không sửa kernel/vendor/radio và G965N chưa tác động.

23/10/2026: Read-only USERDATA audit M2 hoàn tất: `/data` chỉ có lost+found và media/TWRP; không có Android tombstone/ANR/DropBox/bootstat/logd. pmsg hiện tại là từ phiên TWRP, không chứng minh adbd M2 đã chạy. Inventory + recovery logs lưu tại `logs/20261023-m2-postboot-userdata/`. Tiếp theo dùng hook logger/init-debug có sẵn của fork, gói calibration M1 trước rồi M2; chưa đổi kernel.

23/10/2026: M1 logger calibration build0095 đã cài trên G965F phụ `M1_G965F_SERIAL`; tám stage RC=0, USERDATA ext4 được format, Android boot completed SDK36/OneUI80000. ADB không RSA được xác nhận trực tiếp (`ro.adb.secure=0`, key dir rỗng, host status `device`); USB debugging=1 và mọi seed Rampart=0. Logger đầu boot ghi được logcat và marker CACHE nhưng `getprop/dmesg/proc` trong file tổng hợp rỗng; chạy logger trễ trong CACHE tạo đủ dữ liệu, nên timing/service-context cần rà soát. EFS raw hash đổi; CPEFS/RADIO/RECOVERY/BOTA giữ nguyên, installer/kernel không có lệnh ghi EFS; không restore/ghi EFS. M1 report/evidence: `docs/m1-boot-report.md`, `logs/20261023-m1-init-debug/flash-01/`. M2 build attempt45 đã fail `plat_sepolicy.cil:78269`; attempt46 chỉ bị guard FaceunlockFix variant từ chối resume. Attempt47 đang clean-build theo fork để chụp đủ CIL và tái hiện policy gate. G965N chưa bị tác động.

23/10/2026: M2 attempt47 clean build kết thúc exit0, policy/ADB/ABI/image/kernel gates passed. Release mới được giữ tại `rom/out/releases/M2/star2lte-1792735195-f3GTW7/`; SHA flashable `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`, target-files `14ccb6675bcae1ffeddd0b41f4488e3b2bd0aaa4f8c3db00970b0c74c4d474da`. Clean CIL gate qua; log xác nhận Hybrid Vendor VNDK33, cầu nối N770F IRadio1.6 và DS-ACK V1.12 duhansysl no-KSU. So với M2 trước, BOOT image giống; SYSTEM/VENDOR/ODM hashes đổi. Chưa flash package mới, M2 runtime vẫn FAIL từ các lượt trước; G965N không bị tác động.

23/10/2026 — M2 attempt47 fresh flash completed on G965F `M2_G965F_SERIAL`, package `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`. All 8 ZIP32 stages RC=0; fresh USERDATA UUID confirmed; protected hashes passed preboot; BOOT baseline matched before reboot. Android ADB did not complete within 240 seconds, user returned device to TWRP. Captured last_kmsg/pstore/recovery; no specific Android or kernel fatal cause. M2 runtime FAIL / root cause open. See `docs/m2-build.md`, `docs/bugs.md`, and `logs/20261023-m2-attempt47/flash-01/`. Next: M1 calibration of existing init/vendor + system boot logger flags, then M2 diagnostic build; no kernel change without evidence.

23/10/2026: M2 attempt48 logger build fresh-flashed on G965F `M2_G965F_SERIAL`; all 8 stages RC=0, fresh USERDATA UUID, protected raw hashes unchanged, BOOT baseline. Device bootlooped and returned to TWRP. No `/cache/unica` or `/data/bootloop-logs`; captured pstore/last_kmsg are recovery-associated (last_kmsg line 14416 reboot,adb, line 14624 restart command adb), no attributable M2 fatal/phase marker. No kernel change. Next: attempt49 with existing `EXYNOS9810_ENABLE_VENDOR_BOOT_TRACE=true` plus current loggers; verify vendor trace payload, then test fresh on same G965F. See `docs/m2-build.md`, `docs/bugs.md`, `logs/20261023-m2-attempt48-init-debug/`.


23/10/2026: M2 attempt49 vendor-trace profile built and fresh-installed on G965F `M2_G965F_SERIAL`. Build `06f83d57...31cb34d`, 8/8 ZIP32 stages RC=0, USERDATA fresh UUID, BOOT baseline and protected hashes unchanged during install. Vendor init rc passed final-tree/SELinux/fs metadata checks. Reboot watch 240s: ADB target absent; returned to TWRP and postboot capture complete, with no attributable Android error in retained logs; read-only TWRP mount verified the installed vendor trace rc is present, but not executed. EFS hash differed from attempt48 before attempt49 writes, remained stable during attempt49; no EFS write/restore. G965N and M1 control untouched. See `docs/m2-build.md`, `docs/bugs.md`, `logs/20261023-m2-attempt49-vendor-trace/`.

23/10/2026: Attempt49 postboot capture in TWRP found no attributable Android fatal or phase marker. `/proc/last_kmsg` is 2 MiB and ends at the bootloader-to-kernel handoff (68 `Starting kernel...` entries, last at line 38096); pstore contains recovery-associated pmsg only (951 B), with console-ramoops/CACHE logger absent. Captured sec_log line 4537 is a TWRP FunctionFS mount error, not Android M2 evidence. BOOT remains baseline. EFS hash changed after failed boot (from `76b914…` to two repeated read-only device results `a0892e…`); timing unknown, no EFS write/restore. M2 cause unresolved; no kernel/radio change. Evidence: `logs/20261023-m2-attempt49-vendor-trace/postboot/`.

23/10/2026 follow-up: Confirmed attempt49 already included patch0095: build log lines 1712–1713 records metadata for both vendor logger files, and read-only TWRP mounts found init.debug.rc and init_debug_log.sh installed. This is consistent with attempt48 missing logger payload, but does not explain attempt49 lack of CACHE. Static read-only /system/bin/init audit found Android36 AArch64 PIE + bootstrap linker and all listed dependencies on SYSTEM; runtime loading/SELinux remains unverified. Probe09 line3 reports M2-INIT-EXEC-PIPE-EOF; no Attempted to kill init line exists, so panic is not established. Same-flags attempt50 would duplicate attempt49. Evidence in logs/20261023-m2-attempt49-vendor-trace/postboot/system-init-elf.txt.

23/10/2026 diagnostic result: Step1 linker test failed reproducibly on DS-ACK 4.9.337: A37 linker aborts with arc4random data MADV_WIPEONFORK failed: Invalid argument (exit 134) while M1 native linker lists its init (exit 0). This explains the missing second-stage init/post-fs CACHE markers before init or sepolicy can be evaluated. Probe10 not run because its precondition (linker passes) failed. No kernel/vendor/radio/EFS writes or flash/reboot. Evidence: logs/20261023-m2-attempt49-vendor-trace/postboot/.

23/10/2026 follow-up runtime diagnostic: On the installed M2 SYSTEM mounted ro,noload in TWRP, the A37 linker reproduced the same exact MADV_WIPEONFORK EINVAL abort for init, sh and secilc (exit134). On the same G965F CPU, a temporary M1 Android16 bootstrap linker+bionic overlay in TWRP tmpfs successfully listed A37 init/sh and ran a harmless A37 shell command (exit0). This is evidence for a M2-only bootstrap compatibility overlay candidate, not proof that init SELinux setup/second stage will boot. Probe10 remains gated; no init mode, policy compile, flash, reboot, partition write, kernel/vendor/radio/EFS change occurred. TWRP mounts were cleaned up; M2 remains in recovery and M1 control remains booted. Evidence: `logs/20261023-m2-linker-runtime-step1/runtime-test.txt`.

24 October 2026 — next diagnostic completed: actual M2/A37 secilc+CIL compiles
successfully in read-only recovery chroot and on booted M1 pinned DS-ACK,
producing identical policy SHA2564725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b.
No live policy load. Probe10 signal decoding was wrong (127 instead of WSTOPSIG),
causing reproducible PTRACE_CONT errno5; the apparent compile stall is unreliable.
Probe11 correction source passes signal-forward/attach selftests on both kernels,
but no probe11 BOOT has been built/flashed. M2 remains TWRP with baseline BOOT.
Current collaboration brief: docs/HELP-WANTED.md; shareable evidence: support/evidence/.


24/10/2026 — probe11 and passive probe12 completed. Probe11 fixes the wait-status
bug and captures compiler exit0 plus kernel SELinux initialization; passive
probe12 reproduces the loop without PID1 ptrace (14 starts, 11 load-policy
markers, 10 superblock-init markers). No explicit fatal/panic or second-stage
start is captured. Both BOOT rollbacks verify the pinned full-partition hash
cc618abc...776ed01d; M2 is in TWRP, M1 stays booted, N untouched. EFS changed
across boot/recovery, without an EFS write/restore; other measured protected
hashes match. Logger permission/visibility across policy activation is the
next instrumentation gate. Source and selected evidence are prepared for the
GitHub assistance snapshot; images, firmware and raw logs stay local.
Live API inspection now reports the repository as public; visibility is not
changed by this update.


24/10/2026 — offline recorder access check: user-space libsepol against the
actual compiled M2 policy returns `requested permission append denied by
te-rule` for kernel -> unlabeled:file, and likewise denies write. Recovery
reports both saved probe11/12 CACHE files with an unlabeled xattr. Runtime
monitor SID/enforcement/inode labels remain unconfirmed; this is a recorder
visibility hypothesis, not a captured AVC or proven bootloop cause. M1 DS-ACK
has no pmsg node, unlike recovery. No live policy load, edit, enforcement
change, device write or reboot was performed. See
`diagnostics/offline-recorder-policy.md` and selected query evidence.


24/10/2026 — probe13 M1 calibration PASS. The passive recorder now preallocates
and mmap-writes a dedicated regular CACHE file, with checksum frames and checked
msync/fsync. Actual namespace/detach/FD-close/error tests pass on recovery and
DS-ACK. M1 boots completed1/enforcing and observes recorder u:r:kernel:s0;
`KMSG_READ_ERROR=-13` and `VFS_WRITE_RC=-13` confirm runtime access loss for
ordinary I/O. A one-time correction of the recorder-private kmsg inode label
succeeds, kmsg reading resumes, and `init: init second stage started!` is saved.
Mapped output survives recovery with msync0/fsync0, heartbeat68410ms and zero
dropped frames. Thus the old recorder's visibility boundary is real on M1,
not proof of a fatal at policy loading. M1 BOOT rolled back to baseline with
full hash verification and returned to Android. M2 probe13 test is now in
progress after fresh serial/model/hash checks; no M2 result is claimed yet.
No kernel/vendor/radio/policy/normal-ROM change or data format. Local evidence:
logs/20261024-probe13/; selected evidence: support/evidence/probe13-m1-*.


24/10/2026 — probe13 completed; preceding in-progress entries are closed.
M2 persisted30 checksum-valid boot slots, all reaching second-stage init.
23 retain `Service boringssl_self_test64_vendor has 'reboot_on_failure' option
and failed, shutting down system.` and22 retain `Got shutdown_command
'reboot,boringssl-self-check-failed'`. This is an explicit reboot trigger;
the old policy boundary was a recorder failure. Normal runtime APEX linker
from installed M2 independently reproduces MADV_WIPEONFORK EINVAL/exit134 on
both recovery and pinned DS-ACK. Bootstrap-only patch0096 leaves that runtime
untouched. A37 init's linkerconfig path is in APEX, so missing system-bin
linkerconfig alone is not an established cause of its exec ENOENT.
Both BOOT rollbacks verify baseline cc618abc...776ed01d; M1 boots baseline,
M2 remains in TWRP, N untouched. Protected hashes match during rollback; EFS
changes across boot/recovery remain unverified with no EFS write/restore.
No kernel/vendor/radio/policy/self-test change or ROM rebuild. Full report,
limits and next implementation decision: docs/probe13-report.md. Selected
counts, exact lines, source hashes and loader outputs are in support/evidence/.

## 24/10/2026 — WIPEONFORK backport calibration and M2 test

The precise runtime failure captured in probe13 justified a diagnostic kernel
build. A clean isolated source at duhansysl V1.12 commit
`914eabe7b6799ab0eb159360a5f9731dd50d0f1d` received patches0000–0002.
The unpatched baseline and feature builds used identical real `.config`, AOSP
clang-r547379, DTB, ramdisk, command line and release string. Static replay,
10,189 exported CRC comparison, no-KSU symbol check, and BOOT component audit
passed. The source writes stock_defconfig into `/proc/config.gz`, so the actual
build config is tracked by its own hash instead of inferred from runtime.

M1 control ran the rebuilt baseline enforcing to `boot_completed=1`; the
standalone test correctly saw `MADV_WIPEONFORK_RC=-22`. It then booted the
backport enforcing and passed: `MADV_WIPEONFORK_RC=0`, 32 parent/child/
grandchild zero-fill rounds, parent preservation, KEEPONFORK, and shared/file
mapping rejection. The exact A37 runtime APEX linker `--list` changed from
exit134 to exit0. The crypto self-test main is still not validated. M1 BOOT
was rolled back and full hash returned to `cc618abc...776ed01d`; it booted
completed1 again.

The feature BOOT SHA256
`b01912f62ae111570d9de3e45d799da3da4788f27d86f119f78a06a9efb8abd7` was then
written only to authorized G965F M2 BOOT (`M2_G965F_SERIAL`), with model,
bootloader, partition size/current hash and protected partition hashes checked.
Only BOOT changed. M2 is under bounded boot observation; capture probe13,
verify actual runtime-selftest logs, then restore BOOT to
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d` and verify
protected hashes. No USERDATA format, system/vendor/ODM, radio, EFS/CPEFS or
G965N operation occurred. Full technical record:
`docs/m2-runtime-kernel-fix.md`.

24/10/2026 M2 feature BOOT result: probe13 generation33 records 6,446 frames,
heartbeat17,773ms and zero drops; second-stage init and post-fs-data are reached.
`linkerconfig generated /linkerconfig with mounted APEX modules info`. Vendor
and system BoringSSL self-test services exit0, clearing the observed MADV/linker
restart trigger on this boot. New blocker from logcat line24286:
`NetBpfLoad: Android 25Q4 requires kernel 5.10.` `bpfloader` exits7 and init
logs `reboot,bpfloader-failed`. M2 returned to TWRP and guarded BOOT rollback to
cc618abc...776ed01d PASS; protected writes unchanged. Next: audit 25Q4 BPF
objects/APIs against DS-ACK4.9 before a separate kernel change or APEX decision.
See docs/m2-runtime-kernel-fix.md and exact local logs.

Candidate investigation: actual A37 Tethering APEX is version361153320 / 25Q4
(API36.1); retained M1 S22 APEX is360743340 / 2025Q2 (API36.0). On this same
S9+ DS-ACK setup the older M1 module logs `detected 5 of 5` and bpfloader exits0;
the A37 module rejects the synthetic5.4.302 kernel for requiring5.10 before
loading programs. BPF payload layouts differ; `CONFIG_DEBUG_INFO_BTF` is off
in the controlled4.9 config. Older APEX is now a candidate only, pending A37
API and tethering feature tests. No APEX/system change or test has been flashed.


24/10/2026 — M2 metadata diagnostic completed: prior S22 Tethering APEX test passed bpfloader, and exact S22 system64 graphics V6 interface cleared surfaceflinger linker failure. Next AppOps fatal was missing RANGING. Captured AconfigFlags package-load errors trace to read-only /metadata; generated AppOps flag is true while manifest parsing cannot read aconfig storage. Only new variable: volatile 16MiB /metadata tmpfs before donor aconfigd. M2 boot_completed1 observed at77.7s, Enforcing, Welcome screenshot captured, SystemServer1707/SurfaceFlinger1412/zygote1311 stable through uptime156.61s. RANGING is registered dangerous and donor flag reads enabled/default/read-only. Current adb_enabled1, rampart_main_switch_enabled0, ro.adb.secure0; reused data, not a fresh-default acceptance. Camera still fails AudioSystem symbol resolution; vendor aconfig tables and persistence remain unresolved. After capture, ADB recovery reboot succeeded; original rc/APEX, V6 absence and original BOOT cc618abc...776ed01d all verified. No protected partition changed during BOOT write/rollback. M2 remains in TWRP; production kernel/M1/N unchanged. See docs/m2-metadata-runtime.md. No M4 transition.

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

24/10/2026 — attempt52 integrated M2 G965F diagnostic package built, image-audited,
fresh-installed and boot-tested. Flashable SHA256 61bc3696dfe40939611356285f63ee09f96d0110dc56af4c65d3346dcbebea12;
eight TWRP32 stages passed; USERDATA UUID changed; protected EFS/CPEFS/RADIO/
RECOVERY/BOTA raw hashes match. Boot did not complete. Logcat line74018:
`Missing permission definition for permission "android.permission.RANGING" associated with app op 151`;
init line4590 reports `/metadata/aconfig` mkdir failed read-only. attempt52 BOOT
was restored and read back as baseline cc618abc...776ed01d. New scoped patch0104
adds the already boot-tested volatile metadata tmpfs hook only to M2 diagnostic.
Fixture apply/hash/idempotence and patch apply/hash-lock checks pass. Clean
attempt53 runs through the existing fork. M1/G965N untouched. See
`docs/m2-integrated-runtime-build.md` and `logs/20261024-m2-integrated-build/`.

24/10/2026 — attempt53 was built and passed compiled, actual SYSTEM/VENDOR/ODM ext4, signed package and eight TWRP32 stage checks. The user authorized one fresh G965F attempt for exact ZIP SHA256 `f72be9be303e34fd5f625c5fd3d50e0590589e62b85386d8816f97f9758343ef`. All stages RC=0; USERDATA UUID changed; protected EFS/CPEFS/RADIO/RECOVERY/BOTA hashes stayed identical; BOOT is diagnostic SHA `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`. First fresh boot reached `sys.boot_completed=1` in 196.4s, Enforcing; `/metadata` tmpfs/aconfig initialized, dangerous RANGING was defined and RangingService registered. ADB defaults are 1/1 with `ro.adb.secure=0`; Rampart logs `disabledRampart=true`. The selected runtime-diagnostic profile does not enable the optional patch0095 logger, so its absence is expected. USB gadget UDC errors need follow-up even though ADB is reachable. This does not establish full hardware acceptance, G965N or Android17. The one-time write authorization is consumed; do not reuse for another package. Detailed evidence: `docs/m2-integrated-runtime-build.md`, `logs/20261024-m2-attempt53/`.


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


## 25 October 2026 — attempt56 kgclient account patch prepared

Live read-only G965F confirms UID/GID5959 resolves to `oem_5959`; donor rule
requires `vendor_kg`, and only A37 donor passwd/group contain the locked exact
rows. Patch0107 adds them to the existing G965F M2 compatibility profile,
collision-checks them and verifies their actual vendor ext4 metadata. Fixture
hash/row/idempotence/collision checks and syntax pass. Clean build/real image
verification/live boot are next. attempt55 remains available; no BPF or kernel
change is included. Evidence: docs/m2-integrated-runtime-build.md and local
logs/20261025-kgclient-investigation.


## 25 October 2026 — M2 attempt56 runtime result

Attempt56 built through the existing fork, passed ext4/package/stage verification,
and fresh-installed on G965F with a new USERDATA UUID. All eight signed ZIP32
stages returned RC=0; protected EFS/CPEFS/RADIO/RECOVERY/BOTA hashes matched
before and after. Two Enforcing boots reached `sys.boot_completed=1`. Patch0107
restores only the locked A37 `vendor_kg` passwd/group rows: `id 5959` resolves
correctly, kgclient uses `u:r:kg_app`, and the old NSS lookup miss/zygote abort
no longer appears.

Both boots also reproduce an independent KnoxGuard TA timeout and
LockedBootCompletedReceiver ANR (`TLC_ERROR`, `@TimeoutException: getTAInfo -
null`, blocked `IKnoxGuardManager.getTAInfo`). CACHE init markers/logcat pass,
while init-context dmesg/props remain empty. BPF EINVAL and Bixby errors remain
independent. The same WIPEONFORK diagnostic BOOT was used; no kernel or radio
changes were made. G965N/M1 remain untouched. Next gate: diagnose the TA backend
without disabling KnoxGuard; M4/Android17 is not accepted. Full evidence and hashes
are in `docs/m2-integrated-runtime-build.md` and local
`logs/20261025-m2-attempt56/`.


## 26 October 2026 — storage cleanup and M4 attempt24

Removed only failed/intermediate ROM build outputs after verifying the final
M1 and M2 releases and their SHA256 manifests on the secondary NVMe. The root
SSD now has about190GiB free. Final M1/M2 artifacts remain checksum-verified;
there was no completed M4 release to retain. Inventory and exact retained
hashes: `docs/storage-cleanup-20261026.md`.

Attempt22 stopped at the BZI6 Battery Health lookup. Patch0150 resolves the
base controller in classes3 and attempt23 passes that gate. Attempt23 then
failed the incompatible optional blocklist hunks at log lines1194/1206/1234/1294.
Patch0151 retains the BZI6 donor policy for the exact M4 G965F profile. Both
patches have reason, affected files, verification, rollback and source-lock
hashes in `docs/patch-matrix.md`. Clean attempt24 is running on the secondary
NVMe; no completed M4 artifact, flash or runtime result is claimed yet.


## 26 October 2026 — attempt24 FOTA gate and attempt25

Attempt24 passed the BZI6 Battery Health gate and retained donor
PackageBlockListPolicy, then stopped in optional UN1CA Updates patching. The
log says at line1179 that
`smali_classes2/com/android/settings/Settings$FOTASuggestionActivity.smali` is
missing; line1181 reports `Build failed in 0hrs 6min 42sec.` BZI6 places the
class in `smali/` (SHA256
`a3a7cd0d1b6e533d45423f7d11a0da287733ffc52f55566d674a3b4c8c18bec8`).

Patch0152 adds exact M4/G965F/BZI6 donor-retention dispatch for that optional
shortcut. Exact profile, wrong model/milestone, AZH4 regression and unrelated
patch dispatch tests pass; replay matches the source. Patches0150–0152 are
locked and recorded in `docs/patch-matrix.md`. Failed attempt24's 12GiB scratch
tree is removed after preserving logs and the relocated-class hash. Clean
attempt25 is running on the second NVMe; no M4 release/device test is claimed.


## 26 October 2026 — attempt25 CSC failure and attempt26

Attempt25 passed the BZI6 Battery Health fix, donor blocklist retention, and
FOTA suggestion retention. It then failed at log line1181 because CSC's
optional Remote management tile patch requires missing
`TopLevelRemoteSupportPreferenceController.smali`; the build exited at line1183.
The actual BZI6 SecSettings.apk hash is
`49dcc59150efb0d38ecc93ec2ffe675eeb6f4b98d39ae6d0f3f970d44b3c4693`.

Patch0153 retains donor tile visibility only for M4/G965F/BZI6. Exact scope,
wrong-target/milestone negatives, AZH4 regression and patch replay pass. Patches
0150–0153 are recorded with reason/files/verification/rollback in
`docs/patch-matrix.md`; all143 source-lock hashes validate. Failed attempt25's
scratch was removed after preserving logs. Clean attempt26 is running on the
secondary NVMe; no completed M4 artifact or device flash is claimed.


## 26 October 2026 — attempt26 network-speed CSC failure and attempt27

Attempt26 passed the BZI6 Battery Health, donor blocklist, FOTA shortcut, and
Remote management tile gates. The next optional CSC network-speed patch failed
at line1190 because
`SecSettings.apk/smali_classes4/com/samsung/android/settings/eternal/provider/items/NotificationsItem.smali` is absent. The build exited1 at line1192. Patch0154 retains donor behavior on exact M4/G965F/BZI6; its scope, negative, AZH4, and unrelated-route tests plus replay pass. All144 patch locks validate. The attempt26 scratch was removed after logs were saved; clean attempt27 runs on M.2.


## 26 October 2026 — attempt27 Secure Folder quick-switch failure and attempt28

Attempt27 passes the Battery Health, donor blocklist, FOTA, Remote management,
and network-speed gates. The optional Secure Folder quick-switch patch then
partially changes `Usefulfeature$1` but fails at
`FunctionKeyUtils.smali:589`; line1206 says `patch does not apply`, and line1208
reports `Build failed in 0hrs 7min 16sec.` Patch0155 keeps donor Secure Folder
key behavior only for exact M4/G965F/BZI6. Scope, negative, AZH4 regression and
unrelated-route tests pass; patch replay and all145 lock hashes validate. The
failed attempt27 scratch was removed after log preservation. Clean attempt28 runs
on M.2; no completed M4 artifact or device flash is claimed.


## 26 October 2026 — attempt28 SystemUI mismatch and attempt29

Attempt28 passes the Battery Health, blocklist, FOTA, Remote management, Settings
network-speed and Secure Folder donor-retention gates. It fails at
`m4-build-attempt28.txt:1192–1203`: the optional SystemUI speed patch cannot find
its expected context in BZI6 `BasicRune.smali:644`; line1205 records exit1.
Patch0156 retains donor SystemUI speed behavior, paired with patch0154's Settings
retention, only on exact M4/G965F/BZI6. Its exact-scope, wrong-model/milestone,
AZH4 and unrelated-route assertions pass. All146 ordered patches replay from the
pinned fork and the dispatcher matches byte-for-byte; all source locks validate.
Attempt28 scratch/temp trees were removed after preserving the log and confirming
there was no `.complete` or M4 release. Attempt29 is building cleanly on the second
NVMe; the retained M1/M2 final packages are unchanged.


## 26 October 2026 — attempt29 Smart View mismatch and attempt30

Attempt29 passed the network-speed donor gates from patch0154/0156 and stopped
at `m4-build-attempt29.txt:1193–1214`: the optional Smart View visibility patch
expects a `QpRune.smali:308` context absent in BZI6; line1216 records exit1.
Patch0157 keeps donor tile behavior only for exact M4/G965F/BZI6. Wrong model and
milestone, AZH4 regression and unrelated-route assertions pass. All147 ordered
patches replay from the pinned fork; the active dispatcher matches byte-for-byte,
and all source locks validate. Attempt29 scratch was removed after log retention
and checks for no `.complete`/empty M4 release. Attempt30 is the next clean build
on the second NVMe; M1/M2 final packages remain unchanged.


## 26 October 2026 — attempt30 DAAgent mismatch and attempt31

Attempt30 passed patch0157's Smart View donor gate, then failed applying the
optional all-apps Dual Messenger modification. The log at
`m4-build-attempt30.txt:1198–1398` shows BZI6 mismatches in
`DualAppProvider.smali:512`, `WhiteListApps.smali:4`,
`DualAppIntentReceiver.smali:33`, and `DAUtility.smali:1947`; line1400 reports
`Build failed in 0hrs 6min 53sec.` Patch0158 preserves the donor allowlist only
for exact M4/G965F/BZI6. Wrong model/milestone, AZH4 regression, unrelated-route
tests, all148 patch replay and byte-identical dispatcher checks pass. Attempt30
scratch was removed after confirming no `.complete` and an empty M4 release root.
Attempt31 is the next clean build; retained M1/M2 packages are unchanged.


## 26 October 2026 — attempt31 DeKnox mismatch and attempt32

Attempt31 passed the donor-retention gates through DAAgent, then stopped in the
optional DeKnox module. Log line1264 says `Missing BZI6 class descriptor:
com/android/server/knox/dar/ddar/ta/TAProxy.smali`; line1266 reports
`Build failed in 0hrs 8min 16sec.` Patch0159 keeps the complete donor Knox/DAR
stack on exact M4/G965F/BZI6. Wrong model/milestone, AZH4 regression and unrelated
module tests pass. All149 patches replay from pinned base, the dispatcher matches
byte-for-byte, and all source locks validate. Attempt31 scratch was removed after
checking no `.complete` and empty M4 release. Attempt32 is the next clean build;
M1/M2 final packages remain unchanged.


## 26 October 2026 — attempt32 Device Image mismatch and attempt33

Attempt32 passed the M4 donor-retention gates through Knox, then `m4-build-attempt32.txt:1195` reported `PATTERN_NOT_FOUND` in the existing About phone device-image customizer; line1197 records build failure. Patch0160 supports the exact BZI6 `const-string`/compact-call layout and the legacy jumbo layout while pinning `SM-G965FZKDEUX`. The actual BZI6 APK was decoded after its hash check; positive, compatibility and changed-accessor negative tests pass. All150 ordered patches replay from the pinned fork. Attempt32 had no `.complete` marker, the M4 release root was empty, and its 12.45GB scratch was removed after preserving logs. Attempt33 is the next clean build on the secondary NVMe. M1/M2 final packages remain checksum-verified and untouched; no phone operation occurred.


## 26 October 2026 — attempt33 Hide My Applist mismatch and attempt34

Attempt33 confirmed Device Image patch0160 at log line1194 (`Pinning About phone device image model code to SM-G965FZKDEUX`), then failed on optional Hide My Applist hooks. Lines1232/1245 report failed contexts at BZI6 `ComputerEngine.smali:9954` and `ActivityStarter.smali:5341`; line1248 records build failure. Patch0161 retains donor behavior only for exact M4/G965F/BZI6. Five scope/regression assertions, shell syntax, all151 ordered patch hashes and full replay pass; dispatcher matches byte-for-byte. Attempt33 had no `.complete`, M4 release root was empty, and its 12.51GB scratch was removed after preserving logs. Attempt34 is the next clean build on the second NVMe. M1/M2 final packages remain untouched; no phone operation occurred.


## 26 October 2026 — attempt34 Keybox mismatch and attempt35

Attempt34 passed Device Image (line1194) and the scoped Hide My Applist donor-retention branch, then failed in Keybox imitation: lines1204/1206 report absent framework `IKeystoreSecurityLevel$Stub$Proxy.smali` and `IKeystoreService$Stub$Proxy.smali`; line1226 records build failure. Patch0162 retains BZI6 donor key-attestation behavior only on exact M4/G965F. Five scope/regression tests, shell syntax, all152 patch locks and full replay pass; dispatcher matches byte-for-byte. Attempt34 had no `.complete`, M4 release root was empty, and its 12.51GB scratch was removed after preserving logs. Attempt35 is the next clean build on the second NVMe. M1/M2 final packages remain untouched; no phone operation occurred.


## Resume after interrupted attempt35 — 26/10/2026

Attempt35 has no `build_exit` and stops immediately after the clean-work-dir
message; no build failure is inferred. The build process was absent on resume,
and the secondary NVMe was not mounted. `tools/mount-release-storage.sh` mounted
UUID `a26834a0-9fe6-4668-8896-c87a532862d9` at the expected path successfully.
Final M1/M2 `.complete` markers exist and M4 release root remains empty. Attempt35
partial scratch (24KiB allocated files) was removed with logs retained. All152
patch locks and baseline root config hash were rechecked; no source patch changed.
Attempt36 is a clean run of the same M4 profile; its per-run wrapper requires the
actual secondary UUID mount before creating output paths. This changes only
orchestration/output paths, not the fork build system. Shell syntax passes.


## 26/10/2026 — attempt36 and tail preflight, source through0168

Attempt36 passed the Keybox donor-retention gate, then failed in KnoxPatch:
`m4-build-attempt36.txt:1250,1262,1296,1313,1329,1341` reports incompatible
DevicePolicyManager/DAR/attestation hunks; line1344 says
`Build failed in 0hrs 7min 8sec.` Patch0163 retains the complete donor Knox/
attestation stack. This failed tree is diagnostic-only: KnoxPatch had already
changed some framework state before failure, and it will never be packaged.

Using the existing fork dispatcher on later modules found independent source
mismatches in Paradigm EAD, PIF, Setup Wizard, Tweaks, Settings and the paired
wallpaper default. Per-module exits and exact logs are in
`logs/20261026-m4-vendor-warning-0142/tail-attempt36/`. The initial M4 selection
retains donor behavior for the four optional modules Paradigm/PIF/Setup Wizard/
Tweaks and the MultiSound app-selection limit (0166). A user preference question
was sent; the recommended donor-preserving option is the stated working default
while independent fixes continue. Mandatory ADB/Auto Blocker gates stay active.

0164 adapts the exact BZI6 paired-live wallpaper branch using full-method hash
`74bc3b52672248a3ac4745093b97e3ac3d921e0eec6235cfbce8120910969e12` and preserves
the old Android16 route; actual class/negative/idempotence/DEX tests pass.
0165 reviews/hash-pins the removed legacy OTA method instead of inventing a new
API; actual Settings continuation passes search integration and hidden controls.
0167 adapts FloatingFeature hooks to three primary getters while retaining BZI6
overload delegation and the upstream helper unchanged; real DEX assembly passes.
0168 preserves donor ASKS after the actual hunk failure and hides omitted
ASKS/HMA/Keybox/PIF controls; it seeds PIF/Keybox false after vendor restore and
adds compiled hook/control absence guards. XML, real-property-copy and scope
fixtures pass. All patch reasons, files and verification are in patch-matrix.md.

The earlier Keybox failure was a stale DEX path: both proxy descriptors exist in
BZI6 `classes4` rather than legacy `classes3`. Evidence and hashes are in
`keybox-bzi6-layout.json`; retaining donor behavior does not mean Keystore2 is
missing. There is still no completed M4 image/package or Android17 boot evidence.
M1/M2 final releases remain retained. Next: final-stage preflight, full ordered
source replay and clean attempt37. No phone was operated.


## 26/10 continuation — finalization and PIF priority

User selected porting Play Integrity Fix first; Paradigm, Setup Wizard UI and
Misc Tweaks retain donor behavior. This supersedes the PIF omission in0166/0168.
Patch0171 restores PIF application/keystore/task hooks for exact BZI6 methods,
retains upstream hook helper classes, restores its three Settings controls and
enables its default; unsupported Keybox/HMA/Knox integration remains omitted.
Actual BZI6 compiled DEX fixture and Settings/property gates passed
(`test-pif-0171.txt`, `test-pif-settings-0171.txt`). A live Integrity verdict is
still untested; no device writes occurred.

Patches0169–0174 fix child-module donor cache resolution, prevent applying the
Android16 Scene/OCR patch to the locked BZI6 camera, remove the stale optional
Cloud preinstall config, and retain reviewed native Secure Folder desktop checks
and new Brief components. Every patch records reason/files/verification/rollback
in `patches/fork/` and is hash-locked in `sources.lock.json`.
The disposable attempt36 tree is only for sequential finalization diagnostics;
it contains partial earlier Knox edits and must never be packaged. After the
remaining finalization checks, remove that tree and run a clean attempt37.
M1/M2 retained completed releases remain unchanged. No completed M4 release yet.


Cập nhật28/10 — source0200: patch0199 đã tích hợp fixture network197 + meminfo198 vào profile M4/G965F diagnostic sau bước restore cuối; fixture hash PASS. Patch0200 loại bỏ đúng nhánh DEX special theo người dùng chấp thuận; SIOP/SSRM hash/XML fixture PASS. Chưa build hoặc flash; APK cuối/ext4 chưa kiểm runtime. SDHMS thermal liveness/control, SystemUI main-thread lifecycle và NFC provider/namespace closure còn pending. Chi tiết: [repair record](docs/m4-postboot-runtime-repair.md).


Cập nhật28/10 — source0201: SystemUI source patch đã chuyển pha khởi tạo FaceWidget sang handler main-looper hiện có, kiểm connection generation và plugin instance trước dispatch. BZI6 APK fixture đã Apktool build + decode roundtrip và qua semantic gate. Mất/lỗi UI, thời gian khởi tạo, AOD/Infinity và reconnect runtime chưa kiểm. 0199/0200/0201 chưa build thành ROM; M4 đang ở source/candidate stage, attempt37 giữ nguyên. Xem [M4 repair record](docs/m4-postboot-runtime-repair.md).

Cập nhật28/10 — source0202: audit NFC decode xác nhận JNI PHN không khớp Java BZI6 (72 so với54 native methods; nhiều descriptor khác). Thay hướng sang JNI nằm cùng BZI6 NFC APEX và bỏ riêng system JNI PHN shadow trong profile chẩn đoán M4/G965F; giữ nguyên SLSI HAL/RF/Samsung JNI của S9. Hash extractor/gate static đã thêm. Runtime namespace, registration, HAL, tag và thermal/SystemUI còn chưa nghiệm thu. Không build/flash; attempt37 nguyên vẹn, máy ở TWRP. Xem [M4 repair record](docs/m4-postboot-runtime-repair.md).
