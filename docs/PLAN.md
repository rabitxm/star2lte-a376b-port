# Kế hoạch thực thi hiện hành

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
