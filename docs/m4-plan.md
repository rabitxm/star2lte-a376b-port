# Phương án M4 — A37 One UI 9 / Android 17 trên star2lte

Kế hoạch sửa chi tiết sau0198: [m4-post-boot-repair-plan.md](m4-post-boot-repair-plan.md). Phân tích PC song song; trial từng thành phần, thermal qua gate tối thiểu trước phiên UI dài. Đây là kế hoạch mới, chưa triển khai; checkpoint runtime hiện có ở dưới.

Cập nhật28/10 — source0198: M4 đã quan sát được `sys.boot_completed=1`, `bootanim=stopped`, SDK37/OneUI90000 và SELinux Enforcing trên G965F. Fixture Debug JNI/MemInfoReader PASS exit0; GPU map không có trả-1, không còn làm SystemServer abort. SystemServer PID1830 còn ở uptime337.22s. **Chưa nghiệm thu giao diện/phần cứng**: SystemUI/FaceWidget sai luồng main, SDHMS lỗi enumDEX và NFC thiếu libnfc-nci.so. Đã thu189 file và hoàn tất rollback SYSTEM/BOOT/VENDOR/netstats, đối chiếu sau ghi PASS; máy đang ở TWRP.188 patch/132 file replay PASS,187 hash cũ giữ nguyên; chưa có ROM hoàn chỉnh mới tích hợp các thử nghiệm195–198. Chi tiết: [meminfo runtime](m4-meminfo-runtime.md), [lỗi sau boot](m4-post-boot-blockers.md).

Cập nhật runtime 28/10/2026 — source0197: đã port nhóm query stats và bộ transport/identity/template/Parcel/lưu v7-v8 từ BZI6, giữ entry point S22 cũ và Binder service.2161 lớp round-trip,385 tham chiếu services và36 phụ thuộc mới đã rà; APEX dựng lặp cùng hash,187 patch/130 file replay PASS. Fixture trên G965F đã PASS exit0; SystemServer sau đó native SIGABRT trong libmeminfo (chi tiết0198 ở trên). Đã thu136 file và hoàn tất rollback SYSTEM/BOOT/VENDOR cùng hai thư mục netstats. boot_completed chưa đạt. Chi tiết: [network stats runtime](m4-network-stats-runtime.md).

**M4 đã build và đóng gói hoàn chỉnh —27/10/2026, attempt37/source0188.** Full ZIP đã ký (5,192,379,395 bytes) và target-files đã qua đối chiếu SHA256;8 gói ZIP32 đã qua CRC/chữ ký/giới hạn ZIP32 và có hash trong manifest; run `star2lte-1793025724-rBdwLO` có `.complete`. Toàn bộ kiểm compiled, PIF, ADB/Auto Blocker, SELinux và image ext4 thực tế đã PASS. Chuỗi178 patch replay thành125 file giống hệt từ fork đã khóa. **M4 đã fresh-flash G965F, khởi tạo được SystemServer nhưng chưa boot hoàn tất; đang sửa tương thích API mạng. Verdict Play Integrity trực tiếp chưa kiểm.**

Đã giữ đúng một bản ROM hoàn chỉnh cuối cho mỗi mốc M1/M2/M4 trên NVMe phụ;3 lượt package M4 dở và4 replay fixture đã xóa sau kiểm chứng. SSD chính còn khoảng190.5 GiB và NVMe phụ161.4 GiB trống. Người dùng ưu tiên PIF; Paradigm, UI Setup Wizard và Misc Tweaks giữ hành vi donor. Xem [artifact review](m4-attempt37-artifact-review.md), [lưu trữ](storage-cleanup-20261026.md). Các checkpoint dưới đây là lịch sử.

Ngày lập: 25/10/2026. Trạng thái cập nhật 26/10/2026: profile G965F/no-KSU đã
được tạo ở checkpoint0131; attempt01 dừng ở guard NFC Android16, attempt02 qua
NFC rồi dừng ở hook Camera One UI8 do BZI6 không có `PackageUtil.smali`, và
attempt03 qua NFC/camera rồi dừng tại adapter `createSpBlob` cũ. Patch0134 giữ
luồng blob Keystore Android17 của donor đúng profile M4. Attempt04 qua các gate
đó nhưng dừng vì mẫu byte DEX Wi-Fi Direct cũ không có trong BZI6; patch0135
chuyển riêng M4 sang adapter Smali có anchor chặt. Attempt05 xác nhận nhánh P2P
chạy thật, rồi dừng ở camera arm64-only; patch0136 sửa dispatch đúng profile.
Attempt06 qua P2P và stagefright nhưng dừng ở DVFS do BZI6 không có
`smali/U1/w.smali`. Patch0137 khóa hash APK/class SDHMS BZI6 và dùng
`smali/n5/i0.smali` cho riêng M4, vẫn giữ đường AZH4 của M2. Attempt07 xác nhận
nhánh adapter nhưng phát hiện source APK guard dùng biến cache không có trong
module scope. Patch0138 gọi helper `GET_FIRMWARE_CACHE_KEY` của fork; attempt08
đã qua cache lookup và SDHMS adapter. Lượt đó dừng tiếp ở media vì camera patch
đổi model property trong `libstagefright` trước khi legacy HEVC gate kiểm hash
donor nguyên bản. Patch0139 khóa preimage sau camera patch và hash binary cuối
sau HEVC patch; fixture positive/negative, 129 patch lock/replay đều qua.
Attempt09 là lượt build sạch tiếp theo.
Chưa có image, flash hoặc runtime M4. Bổ sung cho `docs/PLAN.md`, tuân theo quyết định hiện hành
trong `AGENTS.md`; không mở rộng thử nghiệm sang G965N.

## Trạng thái build — checkpoint0149, 26/10/2026

Attempt17 passed NFC and display, then failed at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt17.txt:1074` because the
hash-pinned BZI6 DeviceIDProvisionWorker already returns false for Strongbox
DeviceID. Patch0146 adds an exact M4/G965F/BZI6 no-op guard; its changed-class,
wrong-model and missing-class negatives pass. Attempt18 confirmed the guard at
line1072 and reached SAIV, then failed at line1087 because `midas_config.json`
references `moire_detection/moire_detection.tflite` while the actual BZI6 model
is at `VslMesDetector/moire_detection.tflite`. Patch0147 pins both the donor
config and relocated model, stages it at the configured path and sets vendor
metadata. Attempt19 confirmed this stage at lines1084–1086, then failed at
line1091 because BZI6 PhotoEditor ships two versioned HumanSeg/BGPclassifier
info/model pairs rather than the legacy `hs_segmenter.info` and `.tflite` names.
Patch0148 pins the exact four-file set, hashes and info model paths and stages
the intact donor directory. Its positive and changed-model/wrong-model
negatives pass. Attempt20 then stopped at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt20.txt:1097` with:
`File not found: .../system/system/saiv/smartcropping_2.0/db/smartcrop_saliency_deploy.prototxt`.
BZI6 lacks `smartcropping_2.0`; its `IMAGE_CROPPER_VERSION=V1` model is under
`system/saiv/imageCropper/`. The target's star2lte SCE model is separately
provided by the embedded Exynos9810 vendor baseline, not the BZI6 vendor path.
Patch0149 hash-pins both inputs and the old G965F model files, stages the BZI6
system folder, removes only the reviewed stale smartcropping folder, and retains
the vendor baseline model unchanged. Fixture positive and wrong-model,
changed-donor-model, and changed-vendor-baseline negatives pass. `bash -n` and
ordered replay pass: 139 patches, 115 matching source files, prior138 locks
unchanged. Patch SHA256:
`7c7a8e940412b7c4ddc8a76ba2bc141ea767a07fd9ce6be51c285a7ebd722307`.
Attempt21 is the next clean build, with its temporary images on the secondary
M.2. No M4 package or Android17 runtime is proven; no phone was changed.

## 1. Mục tiêu và đối chứng

M4 đưa framework/services/ART/runtime và giao diện One UI 9 của A37 Android17
lên G965F trước. BẮT BUỘC Hybrid Vendor VNDK33 của fork; blob phần cứng/RIL
đúng biến thể F. Bản N sau đó có profile, blob/RIL gốc N và nghiệm thu riêng,
giữ cầu nối radio ABI/hash-locked của fork khi cần. Không dùng vendor stock
Android10 nguyên bản thay Hybrid Vendor.

M2 attempt56 là đối chứng Android16 đã có hai lượt boot Enforcing và log.
Người dùng đã xác nhận ADB/Auto Blocker cùng phần cứng cơ bản hoạt động ổn định.
Xác nhận đó không thay thế ma trận chức năng hoặc làm các lỗi còn mở thành PASS.
Không cần hoàn thiện mọi tính năng M5 mới chuẩn bị M4 trên PC.

| Thành phần | Baseline/đề xuất cho lượt M4F đầu |
| --- | --- |
| Fork | `rom/`, Android-16, commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`; baseline M2 có97patch đến0107, source M4 hiện có119patch đến0129 |
| Đối chứng ROM | M2 attempt56, flashable SHA256 `f6e4a1069cca283ef32751abace1aedc23222d9ea5eb002d9a78e19475ee5e15` |
| Donor M4 | A376BXXU5BZI6, nội dung release17 / SDK37 / OneUI90000 đã kiểm tĩnh |
| Phần cứng | Giữ họ Hybrid Vendor/ODM/blob/radio F và layout đã kiểm; chỉ điều chỉnh tương thích khi có bằng chứng |
| Kernel thử M4F | Đề xuất dùng đúng kernel WIPEONFORK diagnostic đang dùng ở attempt56 để chỉ đổi Android; không build thêm kernel theo kế hoạch ban đầu |
| Kernel ZIP diagnostic | SHA256 `4cea63f84d5add494bfcc2f9fff105848806a930a24aca975dc261bf6720e20a` |
| Kernel Image diagnostic | SHA256 `92b14be170225954476869d1fd7700f9c2c92d98eb6447d0181c50a11e80fcdd` |
| BOOT M2 đã cài | Full-partition SHA256 `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`; kiểm lại trước ghi |
| Thiết bị | Máy F đang thử M2 sẽ thử M4; giữ G965F M1 đối chứng và G965N nguyên trạng |

Gói sản xuất đã chọn vẫn là duhansysl V1.12 08/05/2026 no-KSU. Kernel diagnostic
có provenance trong `docs/m2-runtime-kernel-fix.md`, chưa được chọn làm kernel
sản xuất. Phạm vi backport hiện ghi M2/G965F; khi triển khai M4 phải ghi rõ việc
tái sử dụng chính binary đó vào hồ sơ M4F, giữ nhãn experimental, không suy sang
N. Mọi thay đổi kernel mới phải dựa vào log M4 và có lượt so sánh riêng.
Nếu ramdisk cần thích ứng A17, kiểm diff riêng, giữ kernel/DTB/cmdline và phân
biệt hash toàn BOOT với hash kernel Image.

## 2. Đầu vào đã có

Firmware tại chỗ: `SAMFW.COM_SM-A376B_XXV_A376BXXU5BZI6_fac.zip`,
9,016,689,387 byte, SHA256
`b152ca2c87c1b54aee6f606f436f52076633e894a7dea330ee1da87f5117b415`.
`firmware.lock.json` ghi CRC PASS, properties đọc từ SYSTEM khớp FOTA,
framework có `CINNAMON_BUN=37`. Đây chưa phải xác thực chữ ký Samsung hoặc boot.
Nguồn: `docs/sources.md`, `logs/20261020-m0-donor-a376b/`.

Đã kiểm lại sự hiện diện của `inputs/donor-a376b/odin/`, `logical/`, `selected/`
và `decoded-framework/`. AP và logical SYSTEM/PRODUCT/ODM còn giữ. Hai super
intermediate đã dọn có chủ đích; tái tạo từ AP nếu bước trích đầy đủ cần chúng.
Không cần tải lại firmware trước khi kiểm các nguồn hiện có.

Trước dùng: đối chiếu ZIP/AP/hash, trích đủ file và UID/GID/SELinux bằng công cụ
của fork; lập bảng APEX, ELF/dependencies, ABI, init/fstab, VINTF/FCM, policy
mapping, aconfig và account/seapp của BZI6. Shipping/board API đọc từ đúng nguồn,
không đặt tất cả thành37. Metadata vendor/boot SDK34 không thay SDK37 framework
và không cho phép flash vendor/boot/modem/bootloader của A37 lên S9+.

## Tiến độ triển khai — 25/10/2026

M4-0 baseline đã khóa theo các manifest/log ở trên; M2 attempt56 và kernel/BOOT
hash giữ nguyên. M4-1 cache isolation và extraction đã đạt; donor/profile audit đang làm: patch0108 thêm source-cache suffix tùy chọn theo
model/CSC, thay các caller download/extract/workdir/config/module/build-info và
customizer liên quan. Selector fixture hiện PASS: M1 giữ SM-S901B_EUX, M2 giữ
SM-A376B_XXV, M4 resolve sang SM-A376B_XXV_A376BXXU5BZI6; suffix traversal bị
từ chối. Patch được replay thử trên base + 97 patch cũ; config fixture giữ suffix được
chọn và reset M2 về rỗng, cú pháp shell/Python đã kiểm tra. Lỗi gen_config đầu
tiên (`SOURCE_FIRMWARE_CACHE_SUFFIX is not set!`) được sửa trước extraction. Thay đổi được ghi tại patches/fork/0108-isolate-a376b-source-firmware-cache.patch,
patches/fork/series, sources.lock.json và docs/patch-matrix.md.

Đã tạo cache Odin BZI6 cô lập bằng hardlink tới AP/BL đã kiểm hash; AZH4 cache
M2 không đổi. ZIP đầy đủ không còn tại đường dẫn đã khóa, dù AP/BL và các logical
SYSTEM/PRODUCT/ODM image vẫn còn. Phần selected/system hiện chỉ là partial; chưa
đủ để build. Fork extraction BZI6 đã hoàn tất trong `rom/out/fw/SM-A376B_XXV_A376BXXU5BZI6`:
SYSTEM/VENDOR/PRODUCT/ODM/VENDOR_DLKM/SYSTEM_DLKM và fs_config/file_context
đều có dữ liệu. Metadata dynamic-partition cũng được sinh. Lệnh ngoài trả exit1
sau extraction vì wrapper nhận nhầm token `printf` là firmware thứ hai; log cụ thể
ở `logs/20261025-m4-preflight/extract-bzi6-user.txt:35`:
`(./scripts/extract_fw.sh:377:PARSE_FIRMWARE_STRING) No device model value found in "printf"`.
Không cần chạy lại extraction. Android 17 profile/config, remaining donor-feature
audit, compiled/runtime audit và build vẫn chưa hoàn tất; chưa flash hay tác động
máy. Dispatch audit đã xác nhận BZI6 `TelephonyFeatures.RIL_FEATURES=onebinary`
và ghi hash delta radio/Wi-Fi AZH4↔BZI6. A37 RILD APEX bị loại khỏi vendor cuối
bởi đường thay toàn bộ Hybrid Vendor VNDK33 của Exynos9810; build cuối vẫn cần
kiểm âm APEX đó và kiểm dương cầu nối radio đã khóa. eSE COS, SIM-tray metadata,
chi tiết WLAN RF còn mở; audio inputs currently consumed by the fork are now
resolved from BZI6. ADB/Auto Blocker seed+lifecycle
patch qua source-layout preflight BZI6 nhưng compiled/runtime M4 gate chưa chạy.
Runtime APEX BZI6 ghép được với 17 đường dẫn native32 đã khóa từ S22; AVB và
container V3 signature kiểm qua, nhưng runtime Android17 chưa được chứng minh.

Patch0110 now keeps BZI6's `TelephonyFeatures.isOneTray()` runtime decision
instead of assigning an unverified static tray count. Its exact framework and
telephony smali hashes, dynamic modem/slot-property checks, and waterproof-tray
flag pass against the extracted BZI6 fixture. This closes only the source
metadata gap; target SIM mode still needs a live radio log. The M4 donor dispatch
audit is still open across M2-specific guards. eSE static review found stock
G965F `SemService` includes `JCOP5.2U` in a multi-COS lookup, which does not
establish the selected target COS. BZI6 uses `JCOP6.5U`/`NXP_SKU` and its
donor NXP vendor config is discarded by Hybrid Vendor restore. The verified
G965F M2 device reports empty boot SKU and COS-default properties, so the A17
SKU-based eSE detection requires adaptation. That was the pre-profile checkpoint;
see checkpoint0131/0132 below for the current profile and build state.

M4-2 runtime preflight trên G965F đang chạy M2 với kernel DS-ACK 4.9.337:
BZI6 `linker64 --list` nạp được `/system/bin/init` (exit0), A17 `sh` chạy lệnh
vô hại thành công (exit0); không thấy `FATAL: kernel too old` hoặc `SIGILL`.
Lượt `secilc --version` đầu tiên là lời gọi không hợp lệ (exit1). Sau khi ADB
kết nối lại, BZI6 `secilc` đã compile thành công bộ split CIL đã thích ứng
VNDK33 ngay trong rootfs riêng trên G965F M2/DS-ACK (exit0). Cờ giống verifier
của fork, gồm `-N`, nên chưa kiểm neverallow. Host và device đều tạo policy
1,663,784 byte với hash khác nhau; chưa xác nhận tương đương ngữ nghĩa. Không
nạp policy hay chạy init. Đã lưu input hashes/output policy và dọn đúng rootfs
chẩn đoán sau khi kiểm không có mount. Log: `runtime-secilc-compile.txt/json`
và `runtime-scratch-cleanup.txt` trong `logs/20261025-m4-preflight/`.

Patch0111 extends only the verified absent classifier/ARDoodle claim cleanup
to BZI6, pins the actual GPPM package as a positive input, and adds a final-work
release gate. Source/fixture/replay checks pass; actual M4 image checks remain
pending. GPPM is present in the complete extraction; the earlier absence claim
has been corrected. Target eSE settings remain the M2 baseline until a separate
eSE change is justified; empty boot SKU is recorded as a compatibility issue,
not a reason to invent target COS metadata.

Patch0112 corrects the BZI6 SIM method hash for the fork's real
`--no-debug-info` decode. Patches0113–0115 port the reviewed mDNIe, APE/
connection-personalization and three SoftAP branches to BZI6. A17 mDNIe no
longer depends on a removed synthetic logger; APE and power-save shell commands
use the actual BZI6 ordinals/registers. Country fallback retains permission,
valid HAL data and Binder identity cleanup. Both AZH4 and BZI6 fixtures pass,
including composed APE → country → power → sharing-lite order, rejected changed
preimages, and DEX assembly using the existing fork's API29/Dex039 writer.
These are source/assembly checks, not final release-image or hardware results.
Remaining RF/backoff and other M2-only dispatches still require review before
the M4 profile and clean build. Evidence: `mdnie-a17-test.txt`,
`wifi-backend-a17-test.txt`, `wifi-softap-a17-test.txt` under
`logs/20261025-m4-preflight/`; detailed changes in `docs/patch-matrix.md`.

Checkpoint0118: target WLAN source policy is adapted by0116, including the
empty donor table/booster guard and chip-based SoftAP11ax query. Runtime RF
validation remains open. Mandatory signature and backlight preflights exposed
M2 patch mismatches;0117 ports the BZI6 certificate-collection context without
changing certificate policy, and0118 adapts BZI6's split lights API/mute/cache
and extended SehLights Parcel. Source/negative/DEX fixtures and complete108
patch replay pass (92 files byte-identical). Rear fingerprint, OMX/media,
other product features and profile/release wiring still need review. See
`docs/m4-dispatch-ledger.md` for current gates; older preflight notes above
describe the evidence at their checkpoint, not final M4 acceptance.

Checkpoint0120 closes the source rear-fingerprint/OMX dispatch gaps with
0119–0120. Both fingerprint donor fixtures assemble; the A17 method pins and
Settings substitution check fail closed. BZI6 vexfwk_service exists and must
be adapted, unlike AZH4. Actual encoder methods use v4/v4/v4/v2, with Camera
in DEX4. Native disassembly/relocation proves the single HEVC branch at0xebdbc;
only4bytes change to NOP and the full output hash is pinned. The new release
gate reads all four actual compiled encoder methods and checks native output;
fixture positives and stale-native/donor-DEX negatives pass. This remains
source/DEX/native static evidence, not a full M4 artifact or hardware test.
Next: product/display features, profile and runtime/release contracts. Full110
patch replay passes with93 byte-identical files;108 previous hashes unchanged.

Policy compile preflight đã tìm thấy và xử lý bằng patch0109 ba sai khác xác
định giữa BZI6 và Hybrid Vendor VNDK33: 16 alias `typeattribute` còn thiếu từ
mapping33, rule `vendor_init telephony_prop` đổi theo context BZI6
`telephony_config_prop`, và một `genfscon` vendor cũ bị trùng với nhãn
`proc_compaction_proactiveness` của Android17. Rule KnoxZT được giữ nguyên;
compiler chỉ resolve được khi stage đầy đủ `system_ext`. Adapter chỉ sửa
WORK_DIR, gọi ở finalization sau các rule chung; self-test, idempotence và
`verify_exynos9810_selinux.py` trên staged BZI6 + vendor VNDK33 đều PASS. Đây
chỉ là compile host, chưa chứng minh policy load Enforcing trên điện thoại.
Evidence: `docs/m4-donor-dispatch-audit.md` và
`logs/20261025-m4-preflight/selinux-vndk33-adaptation.txt`.

## 3. Chặn trộn nguồn/cache Android16 và17 trước build

Hiện `rom/out/fw/SM-A376B_XXV/.extracted` ghi
`A376BXXS5AZH4/A376BOLM5AZH4/A376BXXS5AZH4` — donor M2. Trong fork,
`scripts/make_rom.sh` và `scripts/internal/create_work_dir.sh` tính source path
theo model/CSC; runtime customizer M2 còn đọc account từ
`FW_DIR/SM-A376B_XXV/vendor`. Đổi riêng profile không đủ tách hai phiên bản.

Mở rộng cách phân giải source cache của fork theo model/CSC/build đã khóa:
M2 nhận AZH4, M4 nhận BZI6; không trích đè input M2. Audit toàn bộ caller trong
download/extract/work-dir/modules và metadata đi kèm, không chỉ thêm suffix ở
một script. Giữ OUT_DIR/dependency tools/build entrypoint; không viết builder
mới hoặc chạy hai build chung mutable WORK_DIR đồng thời. Tên hook mới sẽ chốt
khi triển khai sau audit, không coi tên giả định là API đang tồn tại.

Kiểm chứng trước đi tiếp:

- M1 chọn S22, M2 chọn AZH4, M4 chọn BZI6; marker/build/hash sai phải bị chặn.
- Chuyển M2 -> M4 -> M2 không dùng lại work/compiled output sai donor.
- Cache key khóa donor selector, build thực, patchset, kernel, profile và logger;
  thay đầu vào phải làm mất cache tương ứng.
- Marker `.extracted`/`.completed`/`.modules_applied` do pipeline sinh sau khi
  thành công; không tự ghi để vượt gate. Release/archive cũ giữ nguyên.

## 4. Những nhóm phải rà lại trên donor A17

| Nhóm | Cách làm M4 | Bằng chứng bắt buộc |
| --- | --- | --- |
| Linker/Bionic/ART | Nạp ELF thật bằng linker A17, harmless shell và crypto self-test; rà page-size/ISA/syscall/relocation | Exit/signal/stderr và syscall lỗi hoặc PC/disassembly khi crash; không bỏ self-test |
| APEX/Tethering/BPF | Rà module activation, dependencies và giao diện với framework/daemon A17 | Loader/program/map/helper/tracepoint errors cụ thể; không giả kernel-version string |
| VNDK33/VINTF | Giữ Hybrid Vendor, so manifest/matrix và symbols/namespaces thực | checkvintf đúng tổ hợp system/vendor/odm và log HAL; tương thích A17 cần xác minh bằng log |
| SELinux | Policy/mapping A17 với vendor đã chọn; sửa theo lỗi có bằng chứng | secilc compile, contexts/neverallow và Enforcing runtime; không allow rộng để che lỗi |
| Metadata/aconfig | Tái xét CACHE-backed metadata, mount ordering và consumer A17 | aconfigd exit0, storage/labels đúng, flags được đọc thật và marker giữ qua reboot |
| Vendor aconfig | Rà format và flags A17 cần, không mặc định copy zero-entry tables M2 | Parser/consumer thực chấp nhận, không tắt toàn cục feature flags |
| Camera/audio/graphics | Rà symbols/disassembly A17, giữ native32 mà HAL cần, app/ART64 như donor | Service sống và test ảnh/video có âm thanh; không chép offset patch AZH4 vào BZI6 |
| Account/KnoxGuard | Đọc account/seapp BZI6, khóa rows đúng và theo dõi TA riêng | id/context đúng; Binder/TA trace; không coi UID fix là TA fix hoặc tắt KnoxGuard |
| ADB/Auto Blocker | Port SettingsProvider/AdbService/SUW/Rampart theo code mới, seed một lần | Compiled DEX/property gate và fresh/toggle/PC mới; không chỉ grep source |
| Samsung UI/features | Rà Settings/SystemUI, rear fingerprint, lights, camera/Wi-Fi flags | Compiled logic, controller visibility và HAL thực; không quảng cáo phần cứng donor không có |

S22 Tethering APEX và graphics V6 trong M2 là ngoại lệ có hash; chưa chứng minh
dùng được trên A17. Chỉ giữ từng ngoại lệ sau kiểm lại ABI/APEX activation và
tính năng tương ứng. Không hạ cả ART/runtime/framework.jar/services.jar/UI về
M2 để đạt boot rồi gọi là Android17. Lưu provenance thành phần thực trong image.

Runtime calibration ưu tiên isolated read-only root trên kernel M2 đang chạy,
đủ bootstrap libs/namespace config. Nếu chạy trong TWRP, chỉ kết luận trong
kernel recovery đó, không lấy làm chứng cứ DS-ACK. Không chạy init
`selinux_setup`/`second_stage` trong chroot hoặc load policy vào recovery để thử;
chỉ compile policy ra file tạm. `ro,noload` dùng cho ext4 phù hợp; EROFS đọc bằng
cách tương ứng filesystem thật. Page alignment/ISA phải kiểm thực, không tự suy
rằng binary A17 chắc chắn SIGILL hoặc chắc chắn chạy được trên Exynos9810.

## 5. Phạm vi tính năng đề xuất cho vòng boot đầu

Giữ app arm64-only, panel60Hz và độ phân giải native cố định. Đề xuất tạm bỏ
Face Unlock/remap Bixby và ẩn các tùy chọn UN1CA chưa port tương tự M2; ghi rõ
giới hạn trong profile M4/ma trận chức năng khi triển khai. Các lựa chọn M2
không tự trở thành guard hợp lệ cho M4. Wi-Fi UI donor cũng phải rà lại; giao
diện tạm không đồng nghĩa S9+ có Wi-Fi6. Giới hạn mới hoặc mất chức năng cốt
lõi phải trình bày trước khi đưa vào lượt thử.

ADB là bắt buộc từ build đầu: fresh Auto Blocker OFF, USB debugging ON, USB ADB
không RSA. Công tắc/mô tả giữ đúng; lựa chọn của người dùng có tác dụng thật và
giữ qua reboot. Không loop hoặc boot script ép lại setting.

## 6. Thứ tự thực hiện và cổng nghiệm thu

| Đợt | Công việc/đầu ra | Điều kiện qua cổng |
| --- | --- | --- |
| M4-0 | Chốt manifest M2 attempt56, kernel/BOOT/layout, known bugs và rollback | Artifact đủ hash; baseline boot/log và phân biệt lỗi M2/M4 |
| M4-1 | Audit BZI6 đầy đủ, tách source cache, donor config/profile song song | Đúng hash/metadata; switch-back M2 chọn AZH4, không nhiễm output |
| M4-2 | Runtime ELF/self-tests, policy/VINTF/dependencies, APEX/aconfig audit | Kết quả thật; blocker có dòng lỗi/exit/signal và patch nhỏ có kiểm chứng |
| M4-3 | Port module theo nhóm, compile sạch và package bằng fork | Compiled ADB, policy, native ABI, provenance A17 và actual-image metadata PASS |
| M4-4 | Fresh flash package đã audit trên F, boot/log và reboot | A17 thật, boot_completed, SystemServer ổn định, ADB/Auto Blocker đúng |
| M4-5 | Hai lượt reboot, theo dõi ít nhất15phút sau boot, hardware smoke | Lỗi phân loại; không tự PASS pin/nhiệt/deep-sleep/IMS đầy đủ |
| M4-N | Audit blob/RIL/config/DTB N, build/test riêng sau khi F đạt | Không rename ROM F hoặc đưa kernel diagnostic F sang N |
| M5 | Hoàn thiện tùy chọn, phần cứng và độ ổn định dài hạn | Bằng chứng riêng từng model |

Chuẩn bị M4 trên PC không phải chờ mọi lỗi M2/M5 đều xong. Cổng thử M4 cần
baseline M2 tái lập và kernel/rollback rõ. KnoxGuard/BPF/Bixby còn mở phải được
theo dõi; nếu gây SystemServer hoặc chức năng cốt lõi mất ổn định thì đánh giá/
giải quyết trước khi dùng làm đối chứng. Cổng F đầu là experimental boot,
không phải stable release hoặc nghiệm thu G965N.

## 7. Phạm vi file khi triển khai

Đã đọc các đường dẫn hiện có sau trong fork:

- `rom/scripts/internal/gen_config_file.sh`, `rom/scripts/make_rom.sh`,
  `rom/scripts/internal/create_work_dir.sh`: donor/cache/hash/milestone.
- `rom/scripts/download_fw.sh`, `rom/scripts/extract_fw.sh` và firmware utilities
  liên quan: source-root/marker, không trích đè AZH4.
- `rom/target/star2lte/config.sh`, `rom/unica/patches/product_feature/`,
  `rom/unica/mods/settings/`: profile/feature/UI dispatch theo donor mới.
- `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/` và
  `exynos9810_finalization/`: metadata/account/camera/ADB và logger sau restore.
- `rom/scripts/internal/verify_exynos9810_release_defaults.py`,
  `verify_exynos9810_images.sh`, `verify_m2_runtime_image.py`: thêm verifier hoặc
  dispatch M4 phù hợp, giữ M2 scope; không bật cờ M2 để vượt gate.

Đường dẫn mới **đề xuất**, chưa tồn tại:
`rom/unica/configs/essi_a376b_android17.sh` và
`rom/target/star2lte/profiles/m4-g965f-a376b-android17-no-ksu.sh`.
Lock donor/runtime exceptions M4 tách M2; không thay hash AZH4 bằng BZI6 trong
lock cũ. Giữ config S22/M1/M2. Mỗi patch có lý do/files/verification/rollback,
series/SHA/source lock và replay. Không ép patch/skip hunk; review Smali hoặc
disassembly mới, kiểm compiled output. Negative checks: donor/cache sai,
compiled ADB thiếu, account collision, logger mất hoặc image metadata sai phải fail.

## 8. Build, dung lượng và giữ release

Dùng `make_rom.sh --clean-work-dir`. Có thể tách compile `--no-target-files`
và `--package-staged --build-rom-zip` như attempt56 khi staged/source/config/hash
khớp và tất cả gate PASS. Full/target-files đi qua flow hiện có; splitter legacy
có sẵn tính số stage theo kích thước thật, không mặc định ép8.

Đo ext4 SYSTEM/VENDOR/ODM cuối so layout đã audit trước package. Kích thước
EROFS compressed donor không phải kích thước output ext4. Không tự tăng SYSTEM
hoặc repartition để chữa overflow; lập danh sách gỡ thành phần với ảnh hưởng rõ
hoặc phương án layout riêng trước khi ghi nếu thực sự vượt dung lượng.

Lúc lập kế hoạch: ổ build trống khoảng195GiB, ổ release63GiB. Một run attempt56
full/target-files/legacy khoảng17GB; M4 có thể lớn hơn, phải đo lại. Ngân sách
đề xuất giữ dư ít nhất30GiB ở build và20GiB ở release, không phải cam kết đủ chỗ.
Không xóa ROM hoàn chỉnh để lấy chỗ. Kiểm mount/UUID ổ release bằng
`tools/mount-release-storage.sh`; dùng `rom/out/releases/M4/<unique-run>/`,
SHA256/manifest/.complete hiện có. Không phát hành firmware Samsung lên GitHub.

## 9. Flash, logging và rollback

Trước ghi: đối chiếu serial/model qua bootloader/hardware, hash package/stages,
layout/phạm vi partition, kernel Image/archive và BOOT hiện tại; lưu protected
hashes và rollback record. Fresh USERDATA theo installer đã audit, UUID mới,
default Settings thu trước chỉnh tay. Không ghi EFS/CPEFS/modem/bootloader/
RECOVERY/BOTA. Identity/hash/layout không khớp phải dừng thao tác ghi.

Logger phải có trong actual VENDOR image: bytes, owner/mode/label/trigger.
M2 CACHE markers/logcat hoạt động, init-context props/dmesg còn rỗng nên thu
thêm root ADB logcat/dmesg từ PC khi online. Nếu chết trước post-fs, dùng early
recorder đã hiệu chuẩn (probe13), kiểm persistence/fsync và rollback. Không
kết luận kernel panic từ việc mất ADB hoặc thiếu console-ramoops.

Theo dõi240giây ban đầu; chỉ tăng tới600giây khi log cho thấy dexopt/zygote/
SystemServer tiến triển và không lặp fatal. Bootanimation riêng không đủ lý do
chờ vô hạn. Nếu mất ADB cần người dùng đưa máy về TWRP bằng phím cứng; đọc
CACHE/pstore trước wipe hoặc ghi tiếp. Lưu build/kernel hashes, Enforcing,
APEX/aconfig/mounts/process start counts, logcat/dmesg/ANR/tombstones và dòng lỗi.
Raw logs cục bộ, không đưa secret/định danh vào tài liệu public.

Rollback ROM phải trả SYSTEM/VENDOR/ODM và BOOT về cùng bộ attempt56 đã giữ,
hoặc cài lại đúng package đó theo installer audit và kiểm hash. Chỉ rollback
BOOT sau khi đổi framework A17 không tái tạo đối chứng M2. Không dùng BOOT cũ
`cc618abc...` trước WIPEONFORK làm baseline attempt56. Ghi tác động USERDATA;
không restore EFS/CP để chữa bootloop.

## 10. Tiêu chí M4F experimental đạt

1. Framework/DEX/APEX cùng runtime chứng minh A17/SDK37, OneUI90000 khớp donor;
   mọi thành phần S22/M2 được giữ có nguồn/hash/lý do/test riêng, không chỉ đổi prop.
2. Fresh boot và reboot đạt boot_completed1, Enforcing; SystemServer không lặp
   crash/restart trong cửa sổ quan sát; logger có marker và dữ liệu thực.
3. Fresh defaults đúng; USB debugging OFF/ON, Auto Blocker ON/OFF có tác dụng,
   lựa chọn giữ qua reboot; client USB ADB mới/không key nhận mà không RSA.
4. Smoke cuộc gọi thông thường/SMS/data khi có SIM, Wi-Fi/hotspot/Bluetooth,
   front/rear ảnh/video có âm thanh, loa/mic/tai nghe, fingerprint, brightness/
   touch/rotation, GPS và sạc. Thiếu SIM/thiết bị kiểm phải ghi NOT_TESTED.
   Pin/nhiệt/deep-sleep/IMS dài hạn vẫn cần ma trận M5.
5. Lỗi kế thừa và lỗi mới ghi riêng; vào màn hình chính chưa đủ PASS nếu còn
   lỗi boot/data/radio cốt lõi hoặc chức năng bị che bằng bypass.

## 11. Known bugs M2 để đối chiếu

- vendor_kg đã sửa0107; hai boot không còn lookup-miss/SIGABRT cũ.
- KnoxGuard: `TLC_ERROR`, `@TimeoutException: getTAInfo - null`;
  `boot2/logcat-midwatch.txt:195946`: `ANR in com.samsung.android.kgclient`;
  `boot2/kgclient-anr-trace2.txt:90-94`: kẹt `IKnoxGuardManager.getTAInfo`.
- BPF `EINVAL`: boot2 logcat-midwatch dòng7875/7883; Bixby
  `IllegalStateException: No matching ComponentResult found!`: dòng171618.
- Init-context props/dmesg rỗng; CACHE markers/logcat có dữ liệu.

Log trên ở `logs/20261025-m2-attempt56/`, tổng hợp trong
`docs/m2-integrated-runtime-build.md`. Không mặc định A17 tự sửa hay giữ lỗi đó.
Không tắt KnoxGuard/BPF/SELinux để đạt boot.

## 12. Cổng tiếp theo

M4-0 baseline, M4-1 BZI6 extraction/cache isolation và bước policy compile
preflight hiện đã có bằng chứng. BZI6 `semwifi-service.jar` cũng đã xác nhận
donor MBO=true, antenna count=2, và các cờ SoftAP gồm 11ax=true, 6GHz=false,
Dual AP=false, OWE=false, power-save=true, priority traffic=false,
concurrency=true, Wi-Fi sharing lite=false, 5GHz-country-based=false,
backoff=true/SAR-backoff=false. Đây là donor A37 evidence, không chuyển 11ax,
antenna count hoặc RF/backoff assumptions sang chip Wi-Fi 5 của S9+.

Chưa tạo M4 profile/config, chưa chạy build, chưa flash. BZI6 framework smali
đã xác nhận ACH ringtone=false, virtual vibration sound=false,
dual-speaker=true, RecordAlive 07010, waterproof-tray=true, cùng dynamic
`isOneTray()` logic. Audio inputs hiện đang dùng trong fork đã được xác nhận.
Static donor SIM-tray count không được firmware gán; cần giữ logic runtime A17
và xác minh bằng target radio log. Tiếp tục rà từng M2-only dispatch đối với
BZI6; giữ eSE target theo baseline M2 trong vòng boot đầu. Target WLAN source
policy đã có0116; RF/runtime validation còn mở. Hoàn tất các dispatch bắt buộc
còn lại trong m4-dispatch-ledger, rồi thêm profile
A17 riêng và kiểm
M2→M4→M2 không trộn cache/work output. Sau đó
chạy clean build bằng fork và hoàn tất compiled ADB/defaults, VINTF, APEX,
image-size, SELinux và source-blob/radio gates. M4F vẫn experimental; chưa suy
ra readiness của G965N và chưa dùng kernel diagnostic như kernel sản xuất.

Ghi chú thay đổi tài liệu: lý do là yêu cầu lập phương án M4; file ảnh hưởng:
`docs/m4-plan.md`, liên kết ở `docs/PLAN.md` và `README.md`. Kiểm chứng bằng
firmware/source locks, config/cache/guard thật trong fork, artifact attempt56,
dung lượng hiện tại và đường dẫn đã đọc. Ghi chú này mô tả lượt lập kế hoạch
ban đầu. Preflight/patch tiếp theo được ghi ở phần tiến độ và patch-matrix;
runtime test chỉ ghi rootfs/file tạm đã dọn, không đổi phân vùng hay kernel.


## Checkpoint0124 — 26/10/2026

0121–0124 add guarded BZI6 shipping/HDR/display adapters, exact-descriptor DEX
resolution and the separate STAR2LTE_M4_FIXED_RESOLUTION selection. Preserve
runtime sensor queries and TYPE_LIGHT5; display configuration becomes3, HFR
mode0/default60, native1440x2960/density560 without a switcher. The actual helper
composition, changed-preimage negatives, DEX039 assembly and compiled DEX/XML
gate fixtures pass. M1/M2 dispatch traces remain unchanged and selecting their
profiles clears the new M4 flag.114 patches replay to95 byte-identical files;
all110 previous hashes remain locked. See docs/m4-product-display.md.

This finishes the reviewed shipping/HDR/HFR/fixed source step. A full M4
profile is still absent; optional Face/Wi-Fi/UN1CA UI and eSE dispatch, runtime/
native32/APEX composition, final contracts and complete source-profile
switching must be reviewed before clean build. No new kernel build or device
operation was needed. ADB/Auto Blocker defaults/compiled/lifecycle and fresh
runtime gates remain mandatory.


## Checkpoint0128 — donor registry and initial feature wiring

0125–0128 add the BZI6 donor config and reviewed target-none eSE, retained
Wi-Fi UI and initial Face/Bixby/optional Settings selections. Search preserves
211 donor providers +4 UN1CA and36 donor keys +1 UN1CA. The actual A16 control
output stays byte-identical. Source/DEX/selection fixtures pass. Actual isolated
config generation M2→M4→M2 selects correct cache markers and returns identical
M2 values; original rom/out config is unchanged.118 patches replay to101 matching
text files and114 earlier hashes remain locked. See docs/m4-source-selection.md.

The donor config now exists; the proposed M4 runtime/kernel profile still does
not. Native32/runtime locks and staging, remaining module/mod preflight, final
boot/vendor/image contracts and full work/cache switching remain before clean
build. No kernel build, phone operation or retained ROM overwrite occurred.


Checkpoint0129,26/10/2026: native32 lock/cache BZI6 riêng đã nối qua restore,
release và ext4 verifier hiện có; exact APEX/metadata/ext4 fixture và actual M2
regression PASS.119patch/102file replay;118hash cũ giữ nguyên. Chưa có profile
runtime/kernel hoàn chỉnh hoặc build/flash M4. Tiếp theo: Tethering/graphics/
aconfig/account, cameraserver A17, remaining mod/boot/vendor/image contracts.
Chi tiết docs/m4-native32-runtime.md; không suy từ chuỗi kernel5.15 thành yêu
cầu build kernel, không dùng patch cameraserver AZH4 cho BZI6.


## Checkpoint0130 — donor-native Android17 runtime closure

Patch0130 adds the exact BZI6 runtime lock/staging and packed-image checks to the
existing fork entrypoints. It keeps BZI6 cameraserver, SamsungCamera, audio
providers, graphics-common V7 and Tethering together; adds the pinned
vendor_kg account and empty vendor aconfig-v2 tables; and uses an M4-specific
persistent metadata label block. The Android16/S22 camera server, graphics V6
and Tethering selections are not reused on M4. M2 metadata output is byte-
identical to the pre-0130 helper.

The helper's positive/negative tests, actual ext4 SYSTEM/VENDOR fixtures,
M2 byte regression, Python AST and shell syntax checks pass. BZI6's actual
SettingsProvider/AdbService source decodes, patches and rebuilds through the fork
Apktool path; the compiled verifier passes the one-time seeds, no-RSA properties
and stored `adb_enabled` lifecycle. Donor Rampart/SecSettings/SUW keys, ON/OFF actions and
AdbSettingsObserver callback remain present. M2 AZH4 outputs compare byte for
byte. This closes source/compiled-fixture gates only. No M4 runtime profile/
build/flash exists; Android17 aconfig/Tethering/kernel interaction,
camera/audio/graphics hardware, full boot/vendor/image contracts and live ADB/
Auto Blocker fresh-data/toggle gates remain open. G965N remains untested. Evidence: `docs/m4-native32-runtime.md` and
`logs/20261026-m4-runtime-0130/`, including `adb-final-retest.txt` and the final
120-patch replay.



## Checkpoint0131 — scoped G965F M4 profile

The existing `m1-g965f-no-ksu.sh` profile remains the kernel/hardware baseline.
The new `m4-g965f-a376b-android17-no-ksu.sh` selects the BZI6 donor, suffix,
M4 runtime/native32 and metadata gates, plus the reviewed initial fixed-resolution,
donor Wi-Fi UI, Face omission and hidden unsupported Settings controls. It keeps
M2 compatibility flags false.

Real isolated `gen_config_file.sh` selection passes M2→M4→M2; cache markers
resolve AZH4→BZI6→AZH4, resulting M2 configs match, M4 source values remain
SDK37/shipping36/board34 with target board33/ext4, and the existing
`rom/out/config.sh` is unchanged. First run caught the exact profile suffix guard
`M4 fixed-resolution preflight requires reviewed BZI6/star2lte/F.`; the profile
now locks the BZI6 suffix before target config validation.

This creates a scoped source profile, not full build readiness. Still required:
generic module and Knox dispatch review, full work/compiled-cache M2↔M4 switch,
final boot/vendor/image contracts and the clean fork build. No ROM was built or
flashed. See `logs/20261026-m4-profile-0131/`.


## Checkpoint0132 — Android17 NFC APEX path

The first clean M4 build, logged in `logs/20261026-m4-profile-0131/`, reached
`_EXYNOS9810_APPLY_NFC_STACK` and stopped at the exact line
`Android 16 NfcNci.apk is missing from the system image`. BZI6 does not use that
Android16 path: its verified `com.android.nfcservices.capex` contains package
`com.android.nfc` version17 (`NfcNciApex.apk`) and the matching Android17 JNI.
Patch0132 keeps this donor APEX only for the exact M4/BZI6/SDK37 scope, leaves
the Android16 behavior unchanged, and pins the capex hash in the final SYSTEM
ext4 verifier. Static ELF inspection finds HIDL NFC1.0/1.2 and AIDL references;
the S9+ service exposes HIDL1.0/1.1/1.2. Runtime service startup and tag behavior
remain unverified.

`aapt2` package/version/sdk checks, embedded APK/JNI/capex and SecNfc hashes,
exact-file positive and mutated-SecNfc negative gate, Python/shell/JSON checks
pass. All123 patches replay on pinned commit `2b25ae6`; the 0133 camera dispatch
fixture accepts only the exact M4 profile and returns before decoding the donor
APK. The M4 profile and `rom/out/config.sh` remain unchanged. Run another clean
build and inspect the final packed SYSTEM image; no device operation or kernel,
vendor, radio, modem or protected-partition write occurred in this checkpoint.


## Checkpoint0133 — BZI6 donor camera app dispatch

Attempt02 passed the NFC guard and stopped at the exact line
`(unica/patches/exynos9810_device_stack/customize.sh:4562:_EXYNOS9810_PATCH_ONEUI8_CAMERA_APP) PackageUtil.smali not found in SamsungCamera.apk`.
The decoded BZI6 Android17 SamsungCamera APK lacks that old One UI8 class and
matches the existing runtime lock SHA
`21a830e6d2f4d73456f6837a73edf8170ddc84222ff710b71fb1318ae7fc61c1`. Patch0133
adds a strict M4/BZI6/G965F early return before the legacy adapter; existing M4
finalization retains the locked donor app/server/runtime set. The exact-profile
branch fixture passes, a wrong M4 milestone is rejected, and donor source
runtime hashes/aconfig provenance pass. No image was produced. A new clean
build remains the next gate; actual camera behavior still needs device testing.
Details: `docs/patch-matrix.md` and
`logs/20261026-m4-nfc-0132/camera-source-audit.json`.
Patch rationale/files/tests/rollback: `docs/patch-matrix.md`; source evidence:
`docs/m4-nfc-integration.md`.


## Checkpoint0136 — BZI6 arm64-only stagefright camera guard

Attempt05 passed the real patch0135 P2P branch, then stopped at the camera
module. Exact error at
`logs/20261026-m4-p2p-0135/m4-build-attempt05.txt:926`:
`rom/unica/patches/camera/customize.sh: line 397: abort: command not found`.
BZI6 source cache contains no `system/lib/libstagefright.so`; its
`system/lib64/libstagefright.so` exists with SHA256
`f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517` and one
old model-property key. This matches the selected arm64-v8a donor app ABI. The
existing camera property patch still applies to the 64-bit donor.

Patch0136 changes only
`rom/unica/patches/camera/customize.sh`: exact M4 G965F/BZI6/SDK37 scope may skip
missing 32-bit stagefright only when the 64-bit file exists; it then patches the
64-bit file normally. A missing 64-bit file or wrong profile fails with the
fork's actual uppercase `ABORT` helper, followed by return1. The M2 SDK36
exception remains unchanged. `patches/fork/series` and `sources.lock.json` pin
the new diff and SHA; this report, README, status, bugs and patch matrix record
reason/files/checks/rollback.

Verification: actual BZI6 64-bit library fixture changes exactly one old key to
one new key. Wrong M4 scope and missing 64-bit file fail closed; the M2 legacy
missing-ABI fixture still passes. `bash -n`, all126 ordered patch hashes, and
reverse/reapply byte comparison pass. Results are in
`logs/20261026-m4-camera-0136/stagefright-dispatch-test.txt`.

Attempt06 passed this branch and the P2P branch before the next DVFS blocker;
see checkpoint0137. No phone, kernel, Hybrid Vendor, radio, modem or protected
partition was changed.

## Checkpoint0137 — BZI6 SDHMS DVFS class mapping

Attempt06 passed the P2P and stagefright camera branches, then failed at the
DVFS module with the exact line
`logs/20261026-m4-camera-0136/m4-build-attempt06.txt:940`:
`(unica/patches/dvfs/customize.sh:74:SMALI_PATCH) Missing BZI6 class descriptor: U1/w.smali`.
The BZI6 source APK
`SamsungDeviceHealthManagerService.apk` is pinned at SHA256
`53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`. A fresh
no-debug-info decode produced `smali/n5/i0.smali`, descriptor
`.class public abstract Ln5/i0;`, SHA256
`39ff200b94cf1e141c3d02aed62709850648d3b914d12f0931c80f02a14afdd6`. Its
`<clinit>()V` reads the floating SSRM policy feature and, for `ssrm_default`,
forces `siop_a37x_s5e8845`; that donor override conflicts with the target
Exynos9810 profile, whose selected value is `ssrm_default`.

Patch0137 changes only `rom/unica/patches/dvfs/customize.sh`: the exact M4
G965F/BZI6/SDK37/runtime profile checks the source/target values and both input
hashes, then routes the existing `SMALI_PATCH` through `smali/n5/i0.smali` and
requires the forced donor literal to be absent afterward. The AZH4 M2
`smali/o5/w.smali` route remains intact. The patch is recorded in
`patches/fork/series` and `sources.lock.json`, SHA256
`6e115e11e1487c80a6fe662f05a2c0850ef50202e9867084e16c73483d8ce574`.

Verification: exact M4 actual-decode fixture patches the one initializer
literal; wrong scope, mutated class hash and mutated source APK hash fail before
class mutation; the M2 AZH4 fixture still patches its original class. The fork
`SMALI_PATCH` helper, shell syntax, all127 ordered patch hashes and
reverse/reapply byte comparison pass. Full evidence:
`logs/20261026-m4-dvfs-0137/sdhms-feature-test.txt`.

Attempt07 entered the DVFS adapter but stopped because its source APK lookup
used `SOURCE_FIRMWARE_PATH`, which is not set in that module's execution scope.
The exact error is
`logs/20261026-m4-dvfs-0137/m4-build-attempt07.txt:940`:
`BZI6 SamsungDeviceHealthManagerService.apk is missing`. Patch0138 below replaces
that lookup with the fork helper; the new guard is covered by cache-key/hash
verification. No phone, kernel, Hybrid Vendor, radio, modem or protected
partition was changed.

## Checkpoint0138 — resolve BZI6 source APK through fork cache helper

Attempt07 passed P2P and stagefright dispatch and reached the new patch0137 DVFS
guard, then stopped at
`logs/20261026-m4-dvfs-0137/m4-build-attempt07.txt:940` with
`BZI6 SamsungDeviceHealthManagerService.apk is missing`. The guard referenced
`SOURCE_FIRMWARE_PATH`, which the fork does not export into the module's
execution scope. The fork defines `GET_FIRMWARE_CACHE_KEY` in
`scripts/utils/common_utils.sh`; using it with the selected source selector and
suffix returns the cache directory that the build itself uses.

Patch0138 changes only `rom/unica/patches/dvfs/customize.sh`: it calculates the
cache key through that helper, then checks the source APK at the resulting
existing donor path. The exact resolved cache is
`SM-A376B_XXV_A376BXXU5BZI6`; the APK hash is still
`53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`.
Patch hash is `52eeb01f3aba1fe0e9d8074012e63260a3ce34df3026cf69b81c9704a46aa2aa`.

Verification: source the fork helper with the actual M4 profile, assert the
resolved path exists and matches the pinned APK hash; `bash -n` passes; all128
ordered patch names/hashes match `sources.lock.json`; reverse/reapply of0138
recreates the source byte-for-byte. Full log:
`logs/20261026-m4-dvfs-0138/cache-key-test.txt`.

Attempt08 verified that the source APK lookup and SDHMS Smali patch now work in
the real module run, then stopped at the later legacy OMX adapter with
`RuntimeError: Unreviewed BZI6 libstagefright preimage` at
`logs/20261026-m4-dvfs-0138/m4-build-attempt08.txt:1005`. The donor-native hash
was `f8ad6ae2...f517`; after the earlier camera metadata edit, the work-tree
hash was `005a887d...f7372`, while the legacy media adapter still expected the
untouched donor. Patch0139 below composes these reviewed edits. No device,
kernel, Hybrid Vendor, radio, modem or protected partition was changed.

## Checkpoint0139 — compose BZI6 camera metadata and OMX binary edits

Attempt08 passed patch0137/0138: the source APK cache resolved, the pinned
`smali/n5/i0.smali` class was verified, and its initializer literal was changed
to `ssrm_default`. The build then reached the legacy OMX adapter and failed at
`logs/20261026-m4-dvfs-0138/m4-build-attempt08.txt:1005`:
`RuntimeError: Unreviewed BZI6 libstagefright preimage`.

This was an order conflict between two existing M4 edits. Patch0136's camera
customizer replaced the model-property key in the sole BZI6 64-bit
`libstagefright.so` before `a376b_android17_media.py` ran. The donor starts at
SHA256 `f8ad6ae22b71d6ba4c7f5ba58e4d1223dfbfaf16e95435a195a9cd0dd886f517`; the
camera-patched work file is `005a887d8432a140804e5c6c783bbef9e303219e388884807c9cf785900f7372`.
Reverting exactly one `ro.boot.em.model\0` string to `ro.product.model\0`
reproduces the original donor hash. The HEVC branch and context remain at the
reviewed offset and match the known branch target.

Patch0139 changes only `rom/unica/patches/legacy/a376b_android17_media.py`.
It requires the exact post-camera hash and property count, proves that reversing
that metadata edit recovers the donor hash, then applies the four-byte HEVC
NOP. The combined output is pinned at SHA256
`5f59107464897883db56189605bf4290da62ee4ac88f723afb8008911aaf2210`; compiled
verification uses this final combined hash. Patch SHA256 is
`f4f087d9a2fa05a04f6d46d78a240097370476f781e271d96d2415d0c1d3b0b2`.

Verification: actual attempt08 camera-patched library fixture passes the
composed adapter and yields the pinned combined hash. The untouched donor is
rejected, preserving strict call order. Four APK encoder fixtures still pass.
Python compile, all129 patch hashes and reverse/reapply pass. Log:
`logs/20261026-m4-media-0139/stagefright-composition-test.txt`.

Next: clean attempt09 in `rom/out/m4-run-20261026-attempt09`; no device or
kernel/vendor/radio changes.

## Checkpoint0135 — BZI6 Android17 Wi-Fi Direct callback dispatch

Clean attempt04 passed the NFC APEX, camera retention, synthetic-password donor
flow and software-Keymaster checks, then failed at
`logs/20261026-m4-spblob-0134/m4-build-attempt04.txt:106`:
`RuntimeError: expected one Samsung P2P callback registration, found 0`.
The input `semwifi-service.jar` SHA256 is
`978ba8cb40f93d76c75552deb67c1492475ce53b462a9ceb31c0fea2f198ab69`; its
`classes.dex` contains zero copies of the old Android16 byte signature. The
decoded BZI6 `setupIface` still contains the Samsung standard callback
registration followed by a separate Seh registration. Patch0135 routes only
the exact M4 G965F/SDK37 runtime profile through the reviewed Smali anchor; it
preserves Seh setup and fails closed if the owner, method or call site differs.

Affected files: fork
`unica/patches/exynos9810_device_stack/customize.sh` and
`platform/exynos9810/tools/patch_semwifi_p2p_callback.py`; the local patch series
and SHA lock, plus the M4 status/patch matrix/dispatch ledger and this report.
Tests: exact M4 fixture passes and replaces only the standard callback; the Seh
registration remains. Wrong M4 scope is rejected without changing the file; the
M2 dispatch fixture still uses its existing Android16 route; a mutated callback
anchor is rejected without mutation. `bash -n`, Python AST, the125-entry ordered
SHA lock, and patch reverse/reapply byte comparison pass. Full output is in
`logs/20261026-m4-p2p-0135/p2p-dispatch-test.txt`.

Next: run clean attempt05 in isolated `rom/out/m4-run-20261026-attempt05`, then
audit packed SYSTEM/VENDOR/ODM and release gates. Attempt04's generated decode
tree was used as a temporary fixture; it is not a release artifact. No device,
kernel, Hybrid Vendor, radio, modem or protected partition was changed.

## Checkpoint0134 — BZI6 Android17 synthetic-password dispatch

Attempt03 passed the NFC APEX and camera-retention branches, then failed at
`logs/20261026-m4-nfc-0132/m4-build-attempt03.txt:248` because the existing
Android16 adapter expects `createSpBlob(...)`, absent from BZI6. The donor uses
`createSyntheticPasswordBlob(...)` and `unwrapSyntheticPasswordBlob(...)` with
Keystore-managed AES keys. Patch0134 retains that donor code only for exact M4/
G965F/BZI6/SDK37 and keeps the old adapter for M1/M2. Shell dispatch fixtures,
wrong-scope rejection, M2 regression, patch-series order and all124 patch hashes
pass; patch0134 applies on the replayed0133 source. Reasons/files/checks/rollback
are recorded in `docs/patch-matrix.md`; fixture log:
`logs/20261026-m4-spblob-0134/spblob-dispatch-test.txt`.

This is a build-dispatch fix, not credential acceptance. M2's Keystore2 evidence
at `logs/20261024-m2-integrated-build/logcat-first-adb.txt:6269` says no secure
Keymaster implementation was found; M4 PIN creation, reboot unlock, SID-bound
key behavior and reset/recovery remain open runtime tests. Attempt04 passed this
branch before failing at Wi-Fi Direct; see checkpoint0135. Do not flash or
advance to G965N from source fixtures alone.


## Checkpoint0140 — BZI6 UsbHostRestrictor handler mapping

Clean attempt09 advanced past the Stagefright composition check and the pinned
SDHMS DVFS patch. It stopped at
`logs/20261026-m4-media-0139/m4-build-attempt09.txt:1002`:
`Missing BZI6 class descriptor: com/android/server/usb/UsbHostRestrictor$8.smali`.
In the actual decoded BZI6 `services.jar`, `$8` is absent. Exactly one class has
the expected `handleMessage(Landroid/os/Message;)V` method and both work-mode
constants: `smali_classes2/com/android/server/usb/UsbHostRestrictor$11.smali`
(SHA256 `2a833bebbcb297d96747e31f901d6a2d25dbbf81ce12d44f27e9f2d84b7153c1`).

Patch0140 changes only `rom/unica/patches/legacy/customize.sh`. The old `$8`
selection remains for existing donor builds; `$11` is allowed only under the
exact M4/G965F/BZI6/SDK37 tuple, and mismatched Android17 selections stop.
`bash -n`, candidate uniqueness/constants, strict-scope source review, patch
dry-run/apply byte comparison, and all130 ordered patch hashes pass. Detailed
evidence is in `logs/20261026-m4-usb-host-0140/usb-host-descriptor-review.txt`.
Attempt10 exercised the mapping in the full build: lines1003–1006 show the
reviewed `$11` class and both replacements. The same attempt then stopped at
`m4-build-attempt10.txt:1027` because BZI6 has no AOSP `RescueParty.smali`.
Patch0141 now checks the hash-locked Samsung `SecRescueParty` implementation and
retains donor behavior only for exact M4/G965F/BZI6/SDK37; an equivalent disable
was not inferred because the donor has distinct recovery/data-erasure actions.
Its class/hash/registration fixture, patch replay and shell syntax pass. Clean
attempt11 is next in a new output directory. Attempts09–10 remain retained; no
device or kernel/vendor/radio partition was touched.


## Checkpoint0141 — preserve BZI6 Samsung RescueParty implementation

Attempt10 confirmed Patch0140 in the actual fork build: the log selects
`UsbHostRestrictor$11.smali` and changes `SUNNY_WORK_MODE` and
`RAINY_RESTRICT_MODE`. It then stopped at
`logs/20261026-m4-usb-host-0140/m4-build-attempt10.txt:1027` with
`Missing BZI6 class descriptor: com/android/server/RescueParty.smali`.

The decoded BZI6 framework has `SecRescueParty.smali`, not the AOSP class. Its
SHA256 is `7c27e2d00fe9ad358a880004a4a7272b57f592e1204cc9798d09776259e3a716`;
`SystemServer` registers its health observer, and its donor methods include
recovery and data-erasure actions. Patch0141 changes only
`rom/unica/patches/miscs/customize.sh`: on the exact M4 G965F/BZI6/SDK37
profile it checks the missing AOSP class, pins the Samsung class and registration,
and retains the donor implementation. Existing donor paths keep the old AOSP
`isDisabled()` adapter. Review fixture, `bash -n`, and patch replay pass; log:
`logs/20261026-m4-rescue-0141/rescue-party-dispatch-review.txt`. Runtime rescue
behavior remains open. All135 ordered patch hashes are locked and replayed. Attempt11 failed at the vendor mismatch warning, attempt12 passed Patch0142 then stopped at NFC; Patch0143 was added, and attempt15 exposed its stale target-SDK guard, corrected by Patch0144 and confirmed at attempt16 log line1038. Attempt16 then exposed a CoreRune hash-order mismatch corrected by Patch0145.
No complete M4 artifact or Android17 boot exists.


## Checkpoint0142 — adapt BZI6 vendor mismatch warning

Attempt11 passed the attempt10 USB Host and RescueParty gates, then failed while
applying the legacy Disable vendor mismatch warning patch. The exact log reports
the expected lambda6 caller patch failure at line1060 and the changed lambda4
handler hunk failure at line1138:
logs/20261026-m4-rescue-0141/m4-build-attempt11.txt. BZI6 caller hash is
5d7b6b1e016d1e9f806623e67892e290a1b3bc9178226901e377ca68d71e8009; handler hash
is 056c250b05e78ab1fbb1c0a99e96f14b45b9eaaaeac8e37314f9e1ac9aab4b2a.

Patch0142 changes only the existing fork dispatcher and adds a BZI6-specific
services.jar Smali patch, guarded to M4/star2lte/SM-G965F/SDK37/BZI6. It removes
class-ID3 warning-dialog scheduling and makes only the handler default branch
a no-op. The donor log line remains; UID warning class ID2 and emergency
handler cases0–2 remain.

Verification: exact source hashes, patch dry-run/apply, semantic checks for the
warning/UID/emergency cases, Apktool services.jar rebuild, shell syntax, 132
patch hashes, and sequential replay from pinned base all pass. Evidence is in
logs/20261026-m4-vendor-warning-0142/. Attempt12 confirmed Patch0142 and then stopped at the G965F NFC sidecar copy. Patch0143 adapts that input but its first full-build dispatch guard was wrong; attempt15 confirms this at log line1038. Patch0144 corrects it. No M4 package or Android17 runtime proof exists yet.


## Checkpoint0143 — G965F NFC sidecar mapping

Attempt12 passed the Patch0142 vendor-mismatch dispatcher: the build log at line
1033 reports the reviewed BZI6 warning suppression while preserving UID and
emergency dialogs. It next stopped at
logs/20261026-m4-vendor-warning-0142/m4-build-attempt12.txt:1039 because the
G965F PHN cache lacks system/system/lib64/libnfc_prop_extn.so; its paired
libnfc_vendor_extn.so is also absent.

The locked PHN target has libnfc_nci_jni.so SHA256
ed74c2fc3c76a835d0fe88043366b3e8c86a920c503cf247355cc84f94efa042,
libnfc-nci.so SHA256
5568864964b841219af84532ae6ae6fd4e135de74cf0eb72bd4a04fe955fec82 and target
vendor/lib64/nfc_nci_sec.so SHA256
65d8f6cd0e0408f3a1cfd1db5d78331b7477ccad4766b5e7058924dd440a4ba8. None has
a direct ELF NEEDED entry for either sidecar. The build work tree already has
the exact BZI6 donor sidecars, with hashes recorded in
logs/20261026-m4-vendor-warning-0142/nfc-target-blob-review.txt.

Patch0143 changes only rom/unica/patches/nfc/customize.sh. Its initial exact
M4/star2lte/SM-G965F/PHN plus BZI6/SDK37 route also required target SDK29. It
hash-checks the target JNI and donor sidecars and retains donor files only if
both G965F sidecars are absent. Attempt15 showed generated target SDK31, so the
29 guard fell through; Patch0144 corrects this. A partial set, wrong profile or
changed JNI still fails closed. The initial positive fixture used stale SDK29;
the corrected SDK31 fixture and full ordered replay are recorded under
logs/20261026-m4-vendor-warning-0142/. Patch SHA256:
93dd48c8f59d2ed0e2841f5912f5bb56f6c26b5a4109bdd24a8ac28f26ed0b83. NFC runtime
behavior remains unverified.


## Checkpoint0144 — correct NFC profile guard to generated target SDK

Attempt13 and attempt14 ended without `build_exit` records after their execution
sessions were interrupted; neither produced a ROM package or a reliable build
result. Their per-attempt logs/configs are retained. Attempt15 ran to the NFC
customizer and stopped at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt15.txt:1038`:
`(unica/patches/nfc/customize.sh:88:ADD_TO_WORK_DIR) File not found: out/m4-run-20261026-attempt15/fw/SM-G965F_PHN/system/system/lib64/libnfc_prop_extn.so`.
The generated attempt15 config records `TARGET_PLATFORM_SDK_VERSION="31"` at
`rom/out/m4-run-20261026-attempt15/config.sh:22`, while Patch0143 checked for29, so
its exact-profile branch did not run and stock-sidecar copying fell through.

Patch0144 changes only `rom/unica/patches/nfc/customize.sh`, updating that guard
to generated target SDK31 while retaining the G965F/BZI6 exact model, firmware,
donor-SDK and JNI/sidecar hash checks. Patch SHA256: `092ce3155bede5598cdbc0d2f8324925350864efd4d4dfa29ee0bda13ac11f47`.
Verification: positive fixture on SDK31, fail-closed wrong-model and SDK29
fixtures, changed-JNI rejection, `bash -n`, JSON validation and all134 ordered
patches replayed from pinned fork commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`;
111 resulting source files match the fork work tree byte-for-byte and previous
133 lock hashes are unchanged. Evidence is under
`logs/20261026-m4-vendor-warning-0142/`. Attempt16 confirmed the NFC route at
line1038 and then stopped in display; attempt17 is the next clean build. NFC
hardware/runtime and complete M4 image remain unverified; no device was touched.


## Checkpoint0145 — compose fixed-resolution check after identity patch

Attempt16 passed the corrected NFC path; log line1038 says the PHN target has no
NFC extension sidecars and the hash-pinned BZI6 donor extensions are retained.
It then stopped at `m4-build-attempt16.txt:1080`:
`RuntimeError: Unreviewed BZI6 product method: CoreRune.smali <clinit>()V`.
The exact preceding identity edits are logged at lines1024–1027: the legacy
customizer changes only `ro.product.device` and `ro.product.model` to vendor
properties in this method. Reversing only those two logged substitutions from
the attempt16 intermediate yields the previously locked donor method hash
`56d6e8ff4f638415b65c1b6afedd07bf5df1f163d98997d3d1dc889338ca6931`; the exact post-identity hash is `f383d6643c1d420005df733e8b418cb075077bebfd020c73f14f093249ab22f7`.

Patch0145 changes only
`rom/unica/patches/product_feature/a376b_android17_display.json` to pin that
intermediate BZI6 method before applying the already-reviewed fixed-resolution
constant. Patch SHA256: `ea73978222dc24df8027cbe8cfc58d701a9641dda472bf661824478f09b24879`. The composition fixture uses the actual
attempt16 CoreRune plus existing donor classes; exact M4 display pass and all15
class/field preimage negatives pass. Ten compiled DEX039 fixtures assemble; the
compiled product/display and stale-constant negative gates pass, and the M2
HFR/brightness dispatch remains byte-identical. All135 ordered patches replay
from pinned fork commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`; 111 files
match and prior134 patch locks are unchanged. Evidence is under
`logs/20261026-m4-vendor-warning-0142/product-display-0145/`,
`core-rune-composition-review-0145.txt` and `series-replay-0145.txt`.
Attempt17 is the next clean build on the secondary M.2 output path; no M4 image,
flash or Android17 runtime proof exists.


## M4 checkpoint — attempt28 failure (26/10/2026)

The primary SSD cleanup is complete; M1 and M2 final packages remain on the
second NVMe, and no M4 package has been completed. Attempt22 stopped in Battery
Health Stats at `m4-build-attempt22.txt:1164` because the BZI6 controller had
moved from the legacy classes4 path to classes3.

Patch0150 adapts `rom/unica/mods/batteryhealth/customize.sh`. The pinned BZI6
`SecSettings.apk` (SHA256
`49dcc59150efb0d38ecc93ec2ffe675eeb6f4b98d39ae6d0f3f970d44b3c4693`) places
`BatteryRegulatoryPreferenceController.getAvailabilityStatus()I` in classes3.
Its `SecBatteryFirstUseDatePreferenceController` subclass inherits that method
and has no override. Patch0150 uses the existing exact DEX resolver for the
base class and checks that the subclass still has the expected superclass and
does not override the method. Attempt23 passed this gate at lines1161–1163.

Attempt23 then stopped in the optional `PM Block List` mod. Its old patch tries
to replace donor LDU policy and lifecycle hooks; BZI6's `services.jar` does not
match those hunks. The exact `git apply` failures are at attempt23 lines1194,
1206, 1234 and1294, followed by `Build failed in 0hrs 6min 54sec.` at line1297.
Patch0151 adds an exact M4/G965F/BZI6 dispatch in
`scripts/internal/apply_modules.sh` to skip only that optional mod and retain
the donor PackageBlockListPolicy/PmLifecycleImpl behavior. This matches the
prior M2 donor-retention decision. The wrong-model and wrong-milestone tests
fail closed; unrelated module paths still use the existing validation.

Patch0150–0156 are in the fork series. All146 source-lock patch hashes
validate; the full ordered series replays from the pinned fork baseline and the
replayed dispatcher matches the active source byte-for-byte. Test logs and replay evidence are under
`logs/20261026-m4-vendor-warning-0142/`. Attempt23's failed worktree was removed;
its build log remains. Attempt24 passed Battery Health and donor-blocklist retention, then failed at
line1179 because the legacy UN1CA suggestion patch expected a missing
`smali_classes2` activity (line1181: `Build failed in 0hrs 6min 42sec.`). The
class is relocated to `smali/`; patch0152 retains donor FOTA suggestion behavior
on the exact M4/G965F/BZI6 profile. Its exact-profile and fail-closed tests and
source replay pass. Attempt25 then passed these gates but stopped at line1181 because the optional
CSC mod expects missing `TopLevelRemoteSupportPreferenceController.smali`; it
exited at line1183. Patch0153 retains donor visibility for the exact M4/G965F/BZI6
profile. Scope/regression tests and patch replay pass. Attempt24/25 failed scratch
trees were removed after preserving their logs. Attempt26 then failed at line1190 because the optional network-speed CSC patch
expects missing `NotificationsItem.smali`; the build exited at line1192. Patch0154
retains donor behavior for exact M4/G965F/BZI6. Its scope/regression tests and
source replay pass. Attempt24–26 scratch trees were removed after preserving logs.
Attempt27 then failed at lines1194–1206 while patching FunctionKeyUtils.smali:589;
line1208 reports `Build failed in 0hrs 7min 16sec.` Patch0155 retains donor
Secure Folder key behavior for the exact M4/G965F/BZI6 profile. Its profile and
regression tests plus replay pass. Attempts24–27 scratch trees were removed after
preserving logs. Attempt28 then failed in the paired SystemUI network-speed
patch. At lines1192–1203, `BasicRune.smali:644` does not match BZI6; line1205
reports `Build failed in 0hrs 6min 47sec.` Patch0156 keeps donor SystemUI
network-speed behavior for exact M4/G965F/BZI6, paired with patch0154's donor
Settings behavior. Its scoped fixture and route assertions pass; full146-patch
replay and dispatcher byte comparison pass. Attempt28 scratch was deleted after
no-complete-marker and empty-release checks.

## M4 checkpoint — attempt29 failure (26/10/2026)

Attempt29 passed the network-speed donor retention gates, then failed in the
optional Smart View visibility patch. At `m4-build-attempt29.txt:1193–1214`,
`QpRune.smali:308` does not match BZI6; line1216 reports
`Build failed in 0hrs 6min 55sec.` Patch0157 retains donor Smart View behavior
for exact M4/G965F/BZI6. Its scope tests, AZH4 regression, unrelated route, full
ordered patch replay and dispatcher byte comparison pass. Attempt29 scratch was
removed after preserving logs and checking no `.complete` and an empty M4 release
root.

## M4 checkpoint — attempt30 failure (26/10/2026)

Attempt30 passed the Smart View donor gate and prior CSC retention checks, then
failed in the optional all-apps Dual Messenger patch. The exact DAAgent hunk
failures are at `m4-build-attempt30.txt:1198–1398` across `DualAppProvider`,
`WhiteListApps`, `DualAppIntentReceiver` and `DAUtility`; line1400 reports
`Build failed in 0hrs 6min 53sec.` Patch0158 retains the donor allowlist for
exact M4/G965F/BZI6. Scoped tests, AZH4 regression, unrelated-route test, full
ordered replay and byte-identical dispatcher pass. Attempt30 scratch was removed
after log retention and no-complete/empty-release checks.

## M4 checkpoint — attempt31 failure (26/10/2026)

Attempt31 passed the donor-retention gates for SystemUI, Dual Messenger, and the
preceding modules, then stopped in DeKnox. At `m4-build-attempt31.txt:1264`, the
module reports `Missing BZI6 class descriptor:
com/android/server/knox/dar/ddar/ta/TAProxy.smali`; line1266 reports
`Build failed in 0hrs 8min 16sec.` Because the old module had already deleted
parts of donor Knox, patch0159 skips the complete optional DeKnox module for
exact M4/G965F/BZI6 and retains the donor Knox stack. Scope tests, AZH4 regression,
unrelated module, full replay and dispatcher byte comparison pass. Attempt31
scratch was removed after preserving logs and checking no `.complete` and empty
M4 release root.

## M4 checkpoint — attempt34 failure and patch0162 (26/10/2026)

Attempt34 passed the Device Image step (`Pinning About phone device image model
code to SM-G965FZKDEUX` at line1194) and skipped the incompatible optional Hide
My Applist module using patch0161. It then failed in optional Keybox imitation.
At `m4-build-attempt34.txt:1204` and `:1206`, `git apply` reports absent
`IKeystoreSecurityLevel$Stub$Proxy.smali` and
`IKeystoreService$Stub$Proxy.smali` under framework `smali_classes3`; line1226
reports `Build failed in 0hrs 6min 56sec.` Patch0162 retains donor
key-attestation behavior only for exact M4/G965F/star2lte, SDK37/target SDK31/
board API33/BZI6. It affects `scripts/internal/apply_modules.sh`. Five tests
cover the exact route, wrong model, wrong milestone, unchanged AZH4 M2 behavior,
and an unrelated module; `bash -n` passes. All152 ordered patches replay from
pinned fork commit `2b25ae6`, and the replayed dispatcher is byte-identical to
the active source. Patch SHA256:
`22d1381ec115ded70e5094e3da678fc828bd2f1c97d9364a244de8f1ec4436a9`.
Attempt34 had no `.complete` and an empty M4 release root; its 12.51GB scratch
was removed with logs preserved. Attempt35 is the clean build on the secondary
NVMe. M1/M2 final artifacts remain untouched. No phone or kernel action occurred;
no complete M4 package or Android17 runtime proof exists.


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


## Real ext4 composition check —0187

Attempt37 passed all compiled/boot/policy gates and generated three ext4 images.
The real native32 image gate rejected mapper2.0 because its checker expected
`system_lib_file` for every library, while the existing final graphics function
explicitly sets `same_process_hal_file` for mapper2.0/2.1 and libgralloctypes
(both ABIs). Actual file_context-system matched that intentional final mapping.
0187 adjusts only the three exact expectations, preserving all strict byte,
mode, UID/GID and symlink checks; no library/SELinux policy is changed.
`m4-package-attempt37-0187.txt` is the current full image/package retry.
