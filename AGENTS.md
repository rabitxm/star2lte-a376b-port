# AGENTS.md — luật làm việc cho Codex

## Quyết định hiện hành — 20/10/2026

Phần này thay thế các mục xung đột bên dưới theo chỉ thị trực tiếp của người dùng:
- BẮT BUỘC dùng Hybrid Vendor VNDK33 của fork, kết hợp blob phần cứng/RIL gốc
  G965N; F phải có nguồn blob phù hợp biến thể, kiểm chứng riêng.
- Đã được người dùng chấp thuận giữ cầu nối radio của fork khi cần; ghép
  blob/cấu hình N đúng phần cứng, không thay rời rạc bộ radio đã khóa ABI/hash.
- Dùng gói DS-ACK có sẵn của duhansysl. Chỉ sửa/build kernel khi log chứng minh
  cần; không tự làm K1/K2/K3 hoặc đổi chuỗi phiên bản kernel.
- Đã chốt gói nguyên bản 08/05/2026 Enforcing-OneUI7 không KernelSU cho M1;
  profile `rom/target/star2lte/profiles/m1-g965f-no-ksu.sh`, hash trong lockfile.
- Kế hoạch thực thi: `docs/PLAN.md`; các kế hoạch cũ ở gốc là lịch sử.
- Người dùng xác nhận đã backup cả hai máy; thử G965F trước, giữ G965N đối chiếu.
- Quy tắc ADB cho M1 và MỌI bản sau (M2/M4/M5), cả F và N, mọi donor:
  Auto Blocker mặc định TẮT ở thiết lập/Settings; USB debugging mặc định BẬT.
  ADB USB không yêu cầu RSA key/ủy quyền máy tính: PC mới cũng phải nhận ADB.
  Giữ công tắc, mô tả và phản hồi Settings hoạt động đúng; người dùng chủ động
  tắt USB debugging hoặc bật Auto Blocker thì phải có tác dụng thật. Chỉ seed
  mặc định khi tạo data/settings mới; không có vòng lặp/boot script ép lại lựa chọn.
  Build phải kiểm mặc định và compiled code; donor Android mới khác cấu trúc
  phải dừng để rà soát, không âm thầm bỏ qua rule. Kernel vẫn giữ gói đã chốt.
- Ngày 21/10/2026: giữ ROM hoàn chỉnh M1/M2/M4/M5, không ghi đè/xóa khi build
  lại. Mỗi lượt package có thư mục riêng, hash/manifest; M1 đã archive độc lập.
  Xem `docs/release-retention.md`. Được chuẩn bị/build M2 trên PC khi có donor
  A37 Android16 đã xác minh. A376BXXS5AZH4 đã nhập/trích, payload thực
  SDK36.1/OneUI80500; profile M2 song song, cache phải khóa donor/version.
- Quyền hiện hành 22/10/2026: người dùng nói “ok cài đi update agents.md cho mày
  toàn quyền flash”. Codex được tự thực hiện các lượt flash, fresh-format,
  repartition theo installer đã audit, reboot, thu log và rollback cần thiết
  trong dự án port này, kể cả artifact/build chẩn đoán mới, không cần xin lại
  quyền chỉ vì đổi hash package. Trước mỗi thao tác ghi phải chọn và đối chiếu
  serial/model, kiểm hash/phạm vi phân vùng, lưu log và phương án rollback.
  Kế hoạch thử G965F trước, giữ G965N đối chiếu vẫn áp dụng; quyền flash không
  tự đổi thứ tự nghiệm thu hoặc cho phép đoán blob/kernel. Không ghi EFS/CPEFS
  hay tự đổi modem/bootloader. Quyền này thay các giới hạn flash cũ bên dưới.
- 22/10/2026: người dùng trực tiếp yêu cầu Codex flash fresh M2 và thu log trên
  G965F serial `M2_G965F_SERIAL`, đã xác minh trong TWRP là SM-G965F/
  G965FXXUHFVG6; người dùng nói rõ USERDATA không cần giữ. Ngoại lệ chỉ áp dụng
  package M2 SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`
  trên G965F; không mở rộng sang G965N hoặc package/mốc khác.

Các mục kernel tự build và vendor stock bất biến trong tài liệu cũ không còn
hiệu lực. Mọi quy tắc còn lại bên dưới tiếp tục áp dụng nếu không xung đột.

## Mục tiêu
Port One UI 9 / Android 17 từ Galaxy A37 5G (SM-A376B, Exynos 1480) lên Galaxy S9+
SM-G965N (chính) và SM-G965F (phụ) — codename `star2lte`, Exynos 9810.
Phần cứng giữ nguyên của S9+ (vendor, modem, bootloader). Kernel **tự build**:
Samsung 4.9.118 merge lên ACK android-4.9-q (4.9.337), kiểu DS-ACK, nhưng không dùng
binary của người khác.

Kế hoạch đầy đủ: `docs/PLAN.md`. Đọc file đó trước khi làm bất cứ gì.

## Hai track, một biến mỗi lần
- **Track K (kernel)** trong `kernel/`: K1 (rebuild stock) → K2 (merge ACK) → K3 (config Android mới + ramdisk fork).
- **Track R (ROM)** trong `rom/`: M1 (đối chứng S22) → M2 (A37 One UI 8) → M3 (ghép kernel K3) → M4 (A37 One UI 9) → M5 (hoàn thiện).
- Mỗi mốc chỉ đổi MỘT biến (donor, hoặc kernel, hoặc phiên bản Android). Không bao giờ đổi hai cùng lúc.
- Không sang mốc sau nếu mốc trước chưa được người dùng xác nhận boot bằng log trong `logs/`.

## Quy tắc chung
- Không viết build system mới. Mở rộng fork `Eend15/Unofficial-Un1ca-9810` (nhánh Android-16).
- Không đoán đường dẫn hay tên biến trong fork. Đọc repo trước, ghi kết quả vào `docs/repo-map.md`.
- Không xóa cấu hình donor S22 của fork; thêm cấu hình SM-A376B song song.
- Mọi patch/commit phải có ghi chú: lý do, file bị ảnh hưởng, cách kiểm chứng.
- Khi báo lỗi, luôn trích dòng log cụ thể (`avc: denied`, `CANNOT LINK EXECUTABLE`,
  `dlopen failed`, `Fatal signal`, `vintf ... incompatible`, `init: ... failed`).
- Khi không chắc Android 17 có chấp nhận một HAL/VNDK nào đó, ghi "cần xác minh bằng log",
  không suy luận thành kết luận.

## Kernel
- Toolchain: clang AOSP. Tắt RKP/UH/KDP/TIMA/DEFEX/PROCA/FIVE ngay từ K1.
- Khi merge conflict: ưu tiên code Samsung ở `drivers/`, `arch/arm64/boot/dts`, `sound/soc/samsung`;
  ưu tiên ACK ở `kernel/`, `mm/`, `fs/`, `net/`, `security/`, `drivers/android`.
- Luôn build 2 bản: permissive (debug) và enforcing (release), cho cả N và F (dtb khác).
- Bật IKCONFIG + PSTORE để debug được khi bootloop.
- Không copy binary kernel của người khác vào repo. Chỉ tham khảo config và ghi credit.
- Kernel là GPLv2: mọi boot.img phát hành phải đi kèm link source.

## Không được làm
- Không commit firmware Samsung, `out/`, `work/`, `*.img`, `*.tar.md5`, `*.zip`, keybox, EFS dump.
- Flash máy dev theo quyền hiện hành ở trên và ghi log theo `docs/log-protocol.md`; không áp dụng lệnh cấm flash lịch sử.
- Không phát hành image chứa firmware Samsung; chỉ chia sẻ script và patch.

## Tài liệu phải tạo và cập nhật
`docs/repo-map.md`, `docs/sources.md`, `docs/hw-diff.md`, `docs/patch-matrix.md`, `docs/bugs.md`.
Cập nhật `README.md` mỗi khi hoàn thành một mốc.

- 24/10/2026: probe13 supplied direct runtime proof that A37 runtime APEX
  aborts on the pinned DS-ACK kernel because MADV_WIPEONFORK returns EINVAL,
  and the vendor self-test's reboot_on_failure restarts M2. This meets the
  existing evidence-first exception for a diagnostic kernel build. A three
  patch upstream-semantics backport was isolated, calibrated on M1 with actual
  nested-fork/KEEPONFORK checks and exact linker loading, then returned M1 to
  baseline. It is being tested only as a diagnostic BOOT on authorized G965F
  M2; this does not replace the selected production kernel package or authorize
  G965N. Keep full build/image provenance in docs/m2-runtime-kernel-fix.md.
