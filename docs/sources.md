# Kế hoạch thực thi hiện hành

## Nguồn nghiên cứu Play Integrity — 01/11/2026

Đã đọc nguồn gốc và đối chiếu fork; chưa tải/cài module hoặc keybox. Đánh giá
case, phiên bản, giới hạn và kế hoạch tại [community plan](m4-play-integrity-community-plan-20261101.md).

- [UN1CA Spoof settings #628](https://github.com/salvogiangri/UN1CA/discussions/628)
  và [FAQ #629](https://github.com/salvogiangri/UN1CA/discussions/629): cơ chế
  tích hợp, yêu cầu keybox và giới hạn PIF Android13+.
- [KeyAttestation #49](https://github.com/vvb2060/KeyAttestation/issues/49):
  case TEE lỗi/local probe và nhãn Google khác nhau, có vấn đề target/revocation.
- [TEESimulator #300](https://github.com/JingMatrix/TEESimulator/issues/300),
  [#286](https://github.com/JingMatrix/TEESimulator/issues/286),
  [#293](https://github.com/JingMatrix/TEESimulator/issues/293): báo cáo Android17,
  Samsung và intermediate hết hạn, không là xác nhận cùng stack G965F.
- [TrickyStoreOSS](https://github.com/beakthoven/TrickyStoreOSS),
  [TEESimulator](https://github.com/JingMatrix/TEESimulator),
  [PIFork](https://github.com/osm0sis/PlayIntegrityFork): generation mode,
  cấu hình/version và khác biệt verdict legacy.
- [Google verdict definitions](https://developer.android.com/google/play/integrity/verdicts):
  tiêu chí trả BASIC/DEVICE/STRONG và điều kiện licensing.
- XDA PIF4607985/TrickyStore4683446 không truy cập được từ công cụ khi tra cứu;
  không ghi nội dung chưa đọc thành kết quả đã kiểm.

M4 implementation started 25/10/2026; execution status and gates: [docs/m4-plan.md](m4-plan.md).
M4-0 is locked; M4-1 cache isolation patch0108 is prepared and verified. Full BZI6 fork extraction has completed with partition metadata. Donor dispatch/config audit and
Android17 preflights remain pending; isolated Android17 build attempts have run
through SAIV but no complete image or flash exists. M2 attempt56 remains the baseline; historical notes below do not override
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


## M4 BZI6 recheck — 25/10/2026

The original verified firmware ZIP is absent from the locked Downloads path and
was not found in Downloads or inputs/donor-a376b. Existing AP and BL Odin tar.md5
files still match the locked byte sizes and their internal MD5 trailers; the
three logical SYSTEM/PRODUCT/ODM images match their recorded hashes and LP
extents. The extracted selected tree is partial, so it cannot be used as a build
tree. The AP and BL are exposed through hardlinks in the separate ignored
rom/out/odin/SM-A376B_XXV_A376BXXU5BZI6/ cache with the exact BZI6 marker.
This does not duplicate their payload or touch M2's AZH4 cache.

25/10/2026 M4-2 SELinux preflight: BZI6 Android17 source aliases and property
contexts do not compile unchanged with the retained Hybrid Vendor VNDK33. The
first full staged `secilc` failure was the unresolved
`attiqi_app_33_0` expansion at system_ext mapping33 line359; correct staging then
exposed a single stale vendor `genfscon` collision and telephony property label
change. Patch0109 adds a source-pinned WORK_DIR adapter; exact pre/post hashes,
16 aliases, one telephony-rule conversion, one stale genfscon removal, adapter
self-test/idempotence and the fork SELinux verifier PASS. The complete fork
series replays from pinned base through0109 and the replayed helper matches the
active tree. The A17 linker loaded init dependencies and an A17 shell command
ran on the current DS-ACK M2 kernel; `secilc --version` was an invalid runtime
invocation (`secilc: unrecognized option '--version'`). No M4 profile/build,
live SELinux policy load, flash or device partition write occurred. Remaining
runtime/policy limits and exact logs are in
`docs/m4-donor-dispatch-audit.md` and `logs/20261025-m4-preflight/`.

25/10/2026 M4 feature recheck of the fork-extracted BZI6 cache: hashed
`semwifi-service.jar` confirms donor MBO and SoftAP defaults; hashed
`framework.jar` confirms RecordAlive 07010, ACH ringtone=false and virtual
vibration sound=false; system/vendor floating features confirm dual-speaker.
These donor feature values do not define S9+ WLAN/RF capability. The separate
target configuration must remain tied to the star2lte HAL/blob. BZI6 still
does not statically assign SIM-tray count; its RIL init files consume a runtime
vendor property. Samsung's official SM-A376B/DS support page confirms the DS
model designation, but does not establish the fork's tray-count feature value:
https://www.samsung.com/sg/support/model/SM-A376BLVSXXV/.
Exact decoded method/field evidence and hashes are in
`logs/20261025-m4-preflight/config-feature-audit.txt`.

Patch0110 preserves the BZI6 runtime SIM-tray detector rather than guessing a
static SIM count. Its helper checks the exact BZI6 `framework.jar` and
`telephony-common.jar` method hashes, dynamic modem/slot-property reads, and
waterproof-tray broadcast value. Verification is static; target behavior still
needs M4 radio logs. The broader A17 dispatch audit remains open. Static eSE
review now records the stock G965F COS mapping and donor/target implementation
difference in `docs/m4-donor-dispatch-audit.md`; target boot-SKU runtime
behavior is still unverified. No M4 profile or build exists yet.

Patch0111 adds BZI6-specific optional camera guards for the absent classifier
and ARDoodle implementation, preserving the actual donor GPPM APK. Full cache
recheck corrected the earlier false GPPM-absence assertion. Real-source and
negative final-work fixtures pass; the complete 101-patch series replays and
the three new/changed fork files match. Source-lock entries0108–0111 are now
in the existing `fork.local_patches` list consumed by the handoff exporter;
all earlier recorded hashes matched before consolidation.

Patches0112–0115 now extend the locked series to105 entries without changing
any of the previous101 patch hashes. SIM validation uses the fork's real
no-debug decode. BZI6 mDNIe and Wi-Fi APE/SoftAP adapters use pinned actual
method preimages, with a regression fixture for AZH4. DEX assembly follows the
existing local Apktool SmaliBuilder API29 cap (DEX039), not an inferred API37
writer. Evidence and exact source fingerprints are in
`logs/20261025-m4-preflight/wifi-backend-method-diff.txt`,
`wifi-softap-method-diff.txt`, `mdnie-a17-test.txt`,
`wifi-backend-a17-test.txt` and `wifi-softap-a17-test.txt`. These are source and
assembly tests; full M4 artifacts and runtime hardware remain untested.

Full textual-series replay through0115 from pinned commit2b25ae6 succeeds;
all87 resulting source files are byte-identical to the active fork. Replay
materializes only patch-touched tracked text files, not proprietary firmware
trees or absolute vendor symlinks. Log:
`logs/20261025-m4-preflight/replay-through-0115.txt`.

Checkpoint0118 extends the lock to108 source patches, preserving all105
previous hashes.0116 pins WLAN preimages and removes only the donor RF table;
0117 adapts the certificate-collection exception context with three class pins;
0118 pins four lights classes/backlight diff, retains split/mute APIs and
forwards the extra Parcel int. All92 resulting textual files match after
complete replay. Logs: `wlan-rf-a17-test.txt`, `signature-bzi6-test.txt`,
`lights-bzi6-test.txt`, `replay-through-0118.txt` in the same preflight folder.
No proprietary artifacts are exported; hardware/runtime acceptance is pending.

Checkpoint0120 locks110 source patches; all108 previous hashes remain unchanged.
0119 pins BZI6 rear-fingerprint methods and the existing star2lte BiometricSetting
compatibility client (APK SHA bc64ca2e5083ae651a57e523eb68496ccf6f9dcaeabfed32c3adc1998acb877c).
0120 pins BZI6 Stagefright and four encoder methods, with a compiled release
gate. vexfwk_service is present in BZI6 and is retained/adapted. Native before/
after hashes and ELF/relocation evidence are in
`logs/20261025-m4-preflight/media-stagefright-bzi6/audit.json`; no proprietary
binary is added to source patches. Full replay matches93 textual files.
Both donor fingerprint fixtures, media DEX fixtures and negative release-gate
checks pass. Actual M4 artifacts, runtime codecs and fingerprint remain untested.

Read-only G965F M2 runtime recheck found empty boot-SKU/COS-default properties,
an active `secure_element` service and advertised NFC/eSE card-emulation
features. This does not establish OMAPI eSE/SemService support. BZI6 `secilc`
then compiled the adapted split CIL on the running DS-ACK kernel with exit0.
The test used the existing fork's `-N` compiler flag, so neverallow validation
and live policy loading remain open. Host/device outputs have equal size but
different hashes; no semantic equivalence claim. Exact scratch inputs were
removed after preserving the result. Logs are under
`logs/20261025-m4-preflight/runtime-secilc-compile.*`.

Patch0108 isolates source cache paths using optional
SOURCE_FIRMWARE_CACHE_SUFFIX; default behavior remains the existing
model/CSC path. Selector tests confirm M1 S22 remains unchanged, M2 selects
SM-A376B_XXV, and the M4 suffix selects
SM-A376B_XXV_A376BXXU5BZI6. The full extraction has now completed through the fork with interactive sudo.
All six partition trees (SYSTEM/VENDOR/PRODUCT/ODM/VENDOR_DLKM/SYSTEM_DLKM)
and non-empty fs_config/file_context metadata are present; the exact marker is
A376BXXU5BZI6/A376BOLM5BZI6/A376BXXU5BZI6. The wrapper returned 1 only after
extracting because an appended `printf` token was parsed as a second firmware
selector; see `logs/20261025-m4-preflight/extract-bzi6-user.txt`. Audit record:
`logs/20261025-m4-preflight/input-audit.json`.


## 26/10/2026 — Product/display BZI6 checkpoint0124

Regenerated no-debug fixtures from the extracted BZI6 cache because previous
/tmp fixtures were gone. Nine original APK/JAR paths and SHA256 values are in
logs/20261026-m4-product-display/decode-manifest.json; no input firmware is
published. The star2lte compatibility BiometricSetting pin remains bc64ca2e…;
its full hash is recorded in docs/m4-product-display.md and the earlier lock.
New source patches0121–0124 are ordered in patches/fork/series and
fork.local_patches;114 patches replay to95 byte-identical text files and all110
previous hashes remain unchanged. The display JSON stores reviewed method/
class hashes and25 selected literal edits plus two fields. Fixtures use the
fork's unchanged API29/Dex039 writer; no toolchain/kernel rebuild occurred.

## 26/10/2026 — BZI6 donor registry and UI policy, checkpoint0128

The separate essi_a376b_android17 registry records only the BZI6 metadata traced
in m4-donor-dispatch-audit.md/config-feature-audit.txt, with source SDK37,
shipping36/board34 and source suffix A376BXXU5BZI6. M2/AZH4 remains independent.
Target eSE metadata stays none; no JCOP version is inferred from stock lookup
strings. The new eSE helper pins actual four-class SHA256 preimages. Wi-Fi UI
pins13 source/canonical compiled class hashes, including BZI6's replacement
Kotlin filter. Its old synthetic lambda is absent. Settings search pins three
actual BZI6 classes plus the existing UN1CA source helper; Intelligence pins
the actual36-key constructor including Buds. The original source vendor
keylayout SHA remains f5cacb4896c414de1c8497d00a7d8bc8287c80bbb9b55e3c09d6473587a15dd8.
All118 textual patches replay to101 matching files;114 earlier hashes remain
unchanged. Evidence: logs/20261026-m4-profile-preflight and
m4-source-selection.md. No firmware/image/raw log is added to the public source
series; private fixtures remain under ignored logs.

## 26/10/2026 — Independent BZI6 native32 lock, checkpoint0129

BZI6 runtime APEX input f308d89271dbf319dd79b833e41b850bf8de36e96efced954a7d85f7013036c9
plus the previously pinned S22 runtime/native32 inputs regenerates output
b6b559bf52dfb07d4c8f1d073b037f1d319b0366e3a732c3b56bbbb55583cdaf.
The existing fork builder, local signing identity, AVB/signapk tools and
fixed4KiB procedure are reused. All original payload bytes/symlinks and other
original container entries remain;17 native32 paths are added. AVB/V3 and45
payload UID/GID/mode/SELinux checks pass. The new BZI6 lock pins the exact APEX,
48 existing platform32 hashes and the retained radio wrapper. Old M2 locks
remain byte-identical. Private APEX/images/keys/logs are not published.

The BZI6 Tethering CAPEX SHA is
540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d.
Its kernel-message xrefs/conditional API flag initialization, original
netbpfload.rc37.0 and cameraserver/audio dynsym mismatches are recorded locally;
these are static evidence, not M4 boot errors or a new kernel authorization.
119 source patches replay to102 matching text files;118 earlier hashes unchanged.
See docs/m4-native32-runtime.md and logs/20261026-m4-runtime.


## 26/10/2026 — Patch0130 BZI6 Android17 runtime closure

Patch0130 pins the BZI6 cameraserver, SamsungCamera, libaudiobase,
libaudioclient, graphics-common V7, Tethering CAPEX, aconfigd.rc and system
aconfig-v2 files in `a376b_android17_runtime_compat.lock.json`. It also pins
vendor_kg passwd/group source rows and synthesizes empty vendor aconfig-v2
outputs because BZI6 contains no vendor source directory. Existing fork
restore/finalization/release/image entrypoints consume these inputs under exact
M4 scope. M2 retains byte-identical metadata output. Test logs and a real ext4
fixture result are in `logs/20261026-m4-runtime-0130/`. The final actual BZI6
Apktool decode/patch/rebuild/compiled ADB retest also passes; the ordered series
replays 120 patches byte-identically with prior hashes unchanged. Firmware, images,
APEX payloads, private keys and phone dumps remain excluded. This is not runtime
or release acceptance.



## 26/10/2026 — Scoped G965F M4 profile, checkpoint0131

Patch0131 adds `m4-g965f-a376b-android17-no-ksu.sh`, sourced from the pinned
M1/F/no-KSU DS-ACK baseline. It selects the BZI6 cache suffix early because the
existing target config validates it before donor-config loading; M4 runtime and
metadata binding are enabled, M2-only runtime flags remain false, and the
reviewed fixed-resolution, donor-Wi-Fi-UI, Face omission and Settings boot-limit
choices are selected. Actual isolated fork config generation passes M2→M4→M2,
resolves AZH4→BZI6→AZH4 cache markers, returns the M2 config unchanged and
leaves `rom/out/config.sh` unchanged. The first suffix failure and exact message
are recorded. No build or phone operation occurred. Full work/compiled-cache
switch, generic module/Knox dispatch and final boot/vendor/image gates remain.
Evidence: `logs/20261026-m4-profile-0131/`.


## 26/10/2026 — BZI6 Android17 NFC APEX, checkpoint0132

Clean M4 build attempt01 stopped at `m4-build-attempt01.txt:62` with
`Android 16 NfcNci.apk is missing from the system image`. Actual BZI6 places
`com.android.nfc` v17 inside `system/system/apex/com.android.nfcservices.capex`
as `NfcNciApex.apk`. The embedded package reports compile/target SDK37 and
minSDK36. The APEX JNI has static HIDL NFC1.0/1.2 and AIDL references; the
selected G965F target service exposes HIDL1.0/1.1/1.2. This is source evidence,
not proof of runtime NFC interoperability.

Patch0132 adds a strict M4/BZI6/SDK37 branch, locks the exact capex/APK/JNI and
Samsung `SecNfc.apk` hashes, and extends the existing ext4 image gate to check
both files' bytes and mode/owner/SELinux labels. Positive exact-file and
mutated-SecNfc negative checks pass. All122 patches replay on pinned fork HEAD
and the four 0132 files match byte-for-byte. No image, flash or device operation
occurred. Evidence:
`docs/m4-nfc-integration.md` and
`logs/20261026-m4-nfc-0132/nfc-source-audit.json`, plus
`logs/20261026-m4-nfc-0132/replay-local-0132-success.txt`.


## 26/10/2026 — BZI6 donor camera dispatch, checkpoint0133

Attempt02 cleared the old NFC guard and then reported
`PackageUtil.smali not found in SamsungCamera.apk` at the One UI8 camera hook.
The BZI6 Android17 camera APK is hash-locked by the M4 runtime manifest at
`21a830e6d2f4d73456f6837a73edf8170ddc84222ff710b71fb1318ae7fc61c1`; the
expected One UI8 PackageUtil class is absent. Patch0133 skips those legacy
Smali edits only for the exact M4/G965F/SDK37 profile, leaving M1/M2 behavior
unchanged and relying on the existing finalizer to retain the locked BZI6 app
and matching runtime. Donor source verification, exact-profile dispatch,
wrong-milestone rejection, shell syntax and sequential patch replay pass. The
next clean build and actual camera behavior remain open. Evidence:
`logs/20261026-m4-nfc-0132/camera-source-audit.json`,
`logs/20261026-m4-nfc-0132/camera-dispatch-test.txt`, and
`logs/20261026-m4-nfc-0132/replay-local-0133-success.txt`.


## 26/10/2026 — BZI6 vendor mismatch warning adapter, patch0142

Attempt11 proves the older warning patch does not match BZI6 caller/handler.
Their exact hashes are locked in the M4-only dispatcher. Patch0142 changes
rom/scripts/internal/apply_modules.sh and adds a scoped decoded-Smali patch. It
suppresses only the donor vendor mismatch dialog dispatch while preserving UID
and emergency dialogs. Patch SHA256 is recorded in sources.lock.json; all132
patches replay from the pinned fork commit and both affected final files match
the build tree. Exact-source semantic tests and Apktool assembly pass. Attempt12
is the next full build. Evidence: logs/20261026-m4-vendor-warning-0142/.


## 26/10/2026 — G965F NFC sidecar mapping, patch0143

Attempt12 identifies a source mismatch in the existing NFC customizer:
G965F PHN FVG4 provides the legacy JNI and S9 Samsung HIDL HAL but no
libnfc_prop_extn.so or libnfc_vendor_extn.so. The G965F core ELF dependency
audit and target/donor hashes are in
logs/20261026-m4-vendor-warning-0142/nfc-target-blob-review.txt. Patch0143
adds an exact-profile branch in rom/unica/patches/nfc/customize.sh and keeps
the already-staged BZI6 Android17 sidecars only for that exact missing-file
case. Positive and negative dispatch fixtures and 133-patch source replay pass.
Attempt15 showed the first branch guard expected target SDK29 but the generated config uses31. Patch0144 corrects the guard; hardware NFC runtime remains to be tested.


## 26/10/2026 — NFC guard corrected to generated target SDK, patch0144

Attempt15 generated `TARGET_PLATFORM_SDK_VERSION="31"` for the target. The
patch0143 exact-profile branch checked29, so the stock copy path remained active
and failed at `m4-build-attempt15.txt:1038` on the absent PHN sidecar. Patch0144
changes only `rom/unica/patches/nfc/customize.sh` to check31 and retains the
existing G965F/BZI6/blob hashes. SDK31 positive, wrong-model, wrong-SDK and
changed-JNI fixtures pass; all134 ordered patch locks replay and 111 files match
byte-for-byte. Patch SHA256 `092ce3155bede5598cdbc0d2f8324925350864efd4d4dfa29ee0bda13ac11f47`. Evidence is in
`logs/20261026-m4-vendor-warning-0142/`; attempt16 confirmed the route and reached a separate display blocker, recorded below.


## 26/10/2026 — CoreRune display composition after identity patch, patch0145

Attempt16 confirms patch0144's NFC route at build log line1038, then stops at
line1080 because the display adapter's pinned CoreRune `<clinit>` hash is taken
after legacy identity substitutions. Build lines1024–1027 show the exact model
and device property changes. Reversing those two strings from the intermediate
restores source hash `56d6e8ff4f638415b65c1b6afedd07bf5df1f163d98997d3d1dc889338ca6931`; actual intermediate is `f383d6643c1d420005df733e8b418cb075077bebfd020c73f14f093249ab22f7`. Patch0145
updates only `rom/unica/patches/product_feature/a376b_android17_display.json` to
pin the known intermediate. Exact display, 15 preimage negatives, ten compiled
DEX039 fixtures, compiled gate and M2 dispatch regression pass. All135 patch
locks replay; 111 files match and previous134 hashes remain. Patch SHA256
`ea73978222dc24df8027cbe8cfc58d701a9641dda472bf661824478f09b24879`. Evidence: `logs/20261026-m4-vendor-warning-0142/product-display-0145/`;
attempt17 is next.


## 26/10/2026 — BZI6 Strongbox no-op and relocated Midas model, patches0146–0147

Attempt17 decoded `DeviceIDProvisionManager$DeviceIDProvisionWorker.smali` to
the same whole-class SHA256 in attempts16 and17:
`2d4eda789aa30c266fa9a3c66a2abec140d18004157ae1d9fd72080928578e5e`. Its
`isSupportStrongboxDeviceID()Z` method already has `.locals 1`, `const/4 v0,
0x0`, `return v0`; the generic return-false operation therefore failed without
changing the donor. Patch0146 adds `a376b_android17_strongbox.sh`, called only
for Android17 M4, star2lte, G965F, SDK37/target SDK31/board API33 and BZI6. The
exact class hash is mandatory. Positive and changed-class/wrong-model/missing-
class negative fixtures pass. Attempt18 logged the no-op at line1072.

Attempt18 then failed at line1087 adding the expected legacy path
`vendor/etc/midas/moire_detection/moire_detection.tflite`. The BZI6 donor
configuration `vendor/etc/midas/midas_config.json` is SHA256
`91a09684275917a418f1a39525d613a9d50ee735b605c3acfeaad58eacac2abd` and
contains the relative reference `moire_detection/moire_detection.tflite`.
The matching donor bytes reside at
`vendor/etc/VslMesDetector/moire_detection.tflite`, SHA256
`88e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1`.
Patch0147 adds a profile-scoped stager that pins both inputs, copies the model
to the configured Midas path and sets vendor metadata. Staging/hash positive
and changed-config, changed-model, wrong-model negatives pass. Attempt19
confirms the real stage and metadata at lines1084–1086. Patch0148 preserves the
BZI6 PhotoEditor HumanSeg/BGPclassifier layout after attempt19 found the legacy
`hs_segmenter.info` and `.tflite` names absent at line1091. It pins the exact
four-file set (two `.info`, two `.tflite`), all hashes and each absolute
`model_path`, then stages the directory for the exact M4/G965F/BZI6 profile.
Positive pair validation/staging and changed-model/wrong-model negatives pass.

Patches0146–0149 have reason/file/check/rollback headers and are in
`patches/fork/series` and `sources.lock.json`; all139 patches replay from the
pinned fork commit, 115 files match the current tree, and the prior138 locks
are unchanged. Patch0146
SHA256 `83d9bfe17ec14d5e8b6184d7d209ef0305ede864f6e422d5fc0e39b6ba1b8927`;
patch0147 SHA256 `e92a711ba193a9a9aa945c3a11ffa06b878d02b58afca035a8c349314e75e68c`.
Patch0148 SHA256 `810f22ea278fe2fc24b48c6597be187467dfd0f7f318e66d40d707d1b0679b10`.

## Patch0149 — BZI6 ImageCropper and star2lte vendor SCE paths

Attempt20 fails at
`logs/20261026-m4-vendor-warning-0142/m4-build-attempt20.txt:1097` because the
BZI6 System image lacks the legacy
`system/saiv/smartcropping_2.0/db/smartcrop_saliency_deploy.prototxt`.
The BZI6 System floating-feature XML is SHA256
`76f9e27c92500562fe4163366c07b71e2ac41e510a57b643500e3a7b68aa1989`; its Vendor
counterpart is `59324c8572e9668db0d29cb09e7ab0247a6abff9a736d71fb447317d44933521`.
Both declare `SEC_FLOATING_FEATURE_VISION_CONFIG_IMAGE_CROPPER_VERSION=V1` and
neither declares the legacy smart-cropping feature. BZI6's exact
`system/saiv/imageCropper/sce_detector_cnn.tflite` model SHA256 is
`5ee6feae9f8307b7f0c363ec6f41e764ef97b5d2afeb83e72714659268b887f5`; paired
`libImageCropper.camera.samsung.so` SHA256 is
`e5ab1aa650369208dd8113292c05f18313881196530425a637923f1e1ca55914`.

The target G965F stock smartcropping files have SHA256
`6f0d4daf03b3330941afdee6305401f6db717bfd0c4209a93e9820211fec8e88` and
`3046afb0a51ead841297f42a38cd3c7733fd7304f39a2fabdeff6bac966fb678`; patch0149
removes this exact legacy pair because BZI6 has no matching feature/model tree.
The separate star2lte embedded vendor SCE model is SHA256
`f39455590d713701df4d051612d1b0baf427d66704e51eb404fd6fabb4498f1c` and stays in
the hardware-specific vendor baseline. Patch0149 stages only the BZI6 System
ImageCropper tree and does not move the donor model to vendor. Its fixture,
negative cases and full 139-patch replay pass; image inclusion and runtime use
are still unverified. See `docs/bugs.md` and the patch0149 section in
`docs/patch-matrix.md`.
Evidence is in `logs/20261026-m4-vendor-warning-0142/`.

## M4 diagnostic network API provenance — source0195/0196

Native connectivity provider comes byte-for-byte from hash-pinned BZI6
Tethering; Java L4S getter/setter and utility bodies/constants also come from
that same BZI6 APEX's framework-connectivity.jar. The surrounding S22 Java/BPF
payload stays selected only by the explicit diagnostic builder. Exact inputs,
old/native-only/L4S outputs and existing local signing identity are pinned in
`a376b_android17_network_native.lock.json`. Locally signed outputs are not
Samsung/Google-original signatures and no binary/key is published. Donor JAR
hashes: S22 e36da5426bcec43d537d756e0b0e36a16edde945915a8b342c9c6b5f110fe0a7;
BZI6 391f21176a39a112285efe9b492fdb8d50b3c1dc36b5ea4f07da7dabe91c3b33.
Actual installed M4 services.jar was pulled read-only from SYSTEM ro,noload
and independently matched c05208d9... before its seven SettingsManager method
references were audited. See m4-native-connectivity-runtime.md for limits.


28/10/2026 source0198 GPU meminfo diagnostic: exact BZI6 libmeminfo donor
SHA db2ec28e3f2609c4b5ae3442b4f60cc2be50a66ab76921aeb1f35d6347df6ec1
(186424 bytes; installed hash/metadata verified ro,noload). One branch target
changes; output b19a1c37a8ebed72676739a12ab46be31dd3c74d4ba1c605f655a62661d70fe9.
Original ELF BuildId is retained; identify the experiment by full SHA, not
BuildId alone. S22 library comparison only, no donation. Existing kernel,
network APEX and retained complete ROM provenance remain unchanged. Source
text only in0198; no normal profile integration. Evidence/docs:
[m4-meminfo-runtime.md](m4-meminfo-runtime.md).


## M4 source/runtime repair pins — 28/10/2026

- BZI6 source marker `A376BXXU5BZI6/A376BOLM5BZI6/A376BXXU5BZI6`; runtime source locks in `rom/unica/patches/exynos9810_device_stack/exynos9810_boot_restore/`. Diagnostic Tethering output SHA256 `f17fa222e1f127d349e61b592c705c127f682ad4d22dda164c03397857862cc6`; measured libmeminfo output `b19a1c37a8ebed72676739a12ab46be31dd3c74d4ba1c605f655a62661d70fe9`.
- BZI6 SDHMS APK SHA256 `53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`. S9 SIOP policy source/output SHA256 `8e30ff47bec85ea6526b839df5057ee3ea0769a017ef9a9e3573dea16bf23503` → `94a74d1c278207fbd59eb595db12ad337ccf1a8b8a39797451745e16923347be`; output removes one user-approved DEX special entry only. Distinct SSRM asset SHA256 `6784ecc18f0326c0599b9278fbd2865c56e3c4d58f2e6ca1f5250280ef78cb96`.

- BZI6 SystemUI source APK SHA256 `87f0c37e465257adf0c597faeb1e871f1b0ff62a2129ea49ef73ec4843543868`; three source/output/round-trip Smali hashes and M4-only scope are pinned in `a376b_android17_facewidget_thread.lock.json`. Exact crash path: `PluginFaceWidgetManager$6.handleMessage` → `setPluginFaceWidgetCallback` → `FaceWidgetPluginLifeCycle` → `LifecycleRegistry.setCurrentState`.

28/10 source reproducibility after0212:203 exported fork patches reproduce140 working text files from pinned HEAD (default GNU patch matching,14 historical fuzzy applications).0209's malformed diff counts are corrected;0213 records the existing release-verifier import-scope fix. Device firmware/image payload is unchanged by these record repairs. The exact legacy ION context was proved by live relabel and persistent reboot while Enforcing.

29/10 source replay extension: Patch0219 exports the optional Samsung Internet
preload URL guard already exercised by attempt0222. Patch0220 exports the
G965F/M4/BZI6 Bluetooth native-backend helper used by attempt0222 and adds the
exact BZI6 `0xfdf1` sender skip. Donor APEX/JNI, Build ID, VA/context bytes and
final JNI hash are pinned in `rom/unica/patches/bluetooth/` and
`sources.lock.json`. Ordered replay from pinned commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1` passes210 patches and matches nine
scoped active source paths byte-for-byte; evidence is
`logs/20261029-m4-g965f-fdf1-followup-0223/series-replay-0220.txt`. This is
source provenance, not a build or Bluetooth runtime pass.


30/10 runtime source provenance:0221–0224 source/locks are exported in
`patches/fork/series`, with patch/helper/lock/current composition hashes in
`sources.lock.json`. All214 patches reproduce158 touched paths byte-for-byte.
Historical0220 helper-lock hashes describe that earlier snapshot; the new
composition explicitly records the updated Bluetooth locks and image verifier.
The four synthetic optional-hook runtime fixtures are source-only under
`support/diagnostics/m4-facewidget-legacy/`. Raw APK/JNI/log/image data stays
private and outside Git.

The [AOSP HCI vendor-send implementation](https://android.googlesource.com/platform/packages/modules/Bluetooth/+/refs/heads/main/system/stack/hcic/hcicmds.cc)
was consulted for its void-return and optional-callback contract. Actual Samsung
BZI6 call sites, arguments, function pointers, BuildID and output hashes come
from the local pinned ELF and are recorded in the sender lock, not inferred
from AOSP offsets. See [runtime repair report](m4-post-0223-runtime-repair.md).
