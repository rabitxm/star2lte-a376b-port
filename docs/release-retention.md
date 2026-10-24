# Lưu ROM theo mốc, không ghi đè

Ngày 21/10/2026, người dùng yêu cầu giữ ROM đã đóng gói hoàn chỉnh ở M1/M2/M4/M5
và chuyển sang chuẩn bị build M2. Đây là thay đổi trên build system fork hiện có.

## M1 đã lưu

Thư mục cục bộ bỏ Git:
`inputs/rom-releases/M1/20261021-g965f-adb17-ba4ed009bd98/`.
Đã copy riêng, đối chiếu SHA256/size nguồn và đích cho 10 ZIP, tổng
16.888.429.178 byte: full flashable, target-files và tám phần legacy.
Không dùng hardlink; file chỉ đọc và thư mục archive không có quyền ghi.
Manifest công khai không chứa payload: `logs/20261021-release-retention/m1-archive-manifest.json`.

Full ROM SHA256:
`ba4ed009bd984ab647bd4316700081d8f6db54b592b0f005fac2f2fcc5d3aebb`.
Target-files SHA256:
`3623ae4e0ff0f41acb8fd40cfaf1adea295860e83ea3888d55187615cbaa5e04`.
Archive kèm config, source/firmware lock snapshot, legacy manifest và evidence
fresh boot. ROM này chứa patch0017; checkout khi lưu có patch0018. Không suy
patch0018/0019 đã có trong ROM M1 đang chạy. Bản M1 cũ trong
`inputs/m1-candidates/m1-16patch-booted/` cũng được giữ nguyên.

## Quy tắc những lần package sau — patch0019

- Profile phải đặt `ROM_MILESTONE=M1/M2/M4/M5`. Nếu chưa phân loại, output ở
  `unclassified`; không tự gán donor mới thành M1.
- `make_rom.sh` cấp thư mục mới bằng mktemp tại
  `rom/out/releases/<mốc>/<target>-<build_timestamp>-<unique_run>/`.
  Không xóa ZIP cũ bằng wildcard. Build lại cùng mốc vẫn có run riêng.
- Target-files/full flashable được tạo vào staging rồi publish exclusive;
  đường dẫn đã tồn tại, kể cả symlink, phải bị từ chối. Không ghi đè qua signapk.
- Run giữ build-config, milestone, source hash markers khi có, SHA256SUMS và
  `.complete` sau khi packaging thành công; ZIP/checksum chuyển chỉ đọc.
  Thiếu `.complete` thì chưa coi run hoàn tất. Marker này không chứng minh boot.
- Legacy splitter đặt các phần cạnh full ROM, kiểm va chạm trước khi làm việc,
  publish không thay thế và tạo manifest exclusive. Các phần đã xác minh chỉ đọc.
  Legacy manifest có hash riêng; các phần tạo sau không tự thêm vào SHA256SUMS cũ.
- Không đổi OUT_DIR dùng chung vì firmware/tools/cache của fork vẫn ở đó.
  Chỉ xóa working/temp theo pipeline; không tự dọn release/archive để lấy chỗ.

Kiểm chứng: `logs/20261021-release-retention/retention-fixtures.json` PASS bảy
case, gồm publisher cạnh tranh, symlink, hai run M2, actual 7z và legacy collision.
Replay 19 patch/30 file byte-identical PASS ở `patch19-replay.json`.
Đây là kiểm thử host với payload nhỏ; chưa có ROM M2 hoàn chỉnh.
Rollback: reverse0019 nhưng giữ archive M1; không chạy packaging cũ có wildcard
xóa lên nơi còn chứa gói cần giữ. Không đổi kernel/vendor hay ghi điện thoại.
