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


25/10/2026 storage relocation: all120retained release files /136,547,395,090
bytes and10complete runs were copied to the existing second ext4 NVMe, full
tree checksum and all stored SHA256 manifests verified, then the redundant
source copy removed. `rom/out/releases` is a symlink to the verified location.
Independent M1 archive remains on the primary disk. Original release bytes and
logical paths remain intact. Run `bash tools/mount-release-storage.sh` after a
PC reboot; no filesystem was formatted. Full evidence/rollback:
docs/storage-cleanup-20261025.md.

## User retention update — 26/10/2026

The user changed the retention rule: keep only the newest complete ROM build
for each of M1, M2, and M4, and remove older versions. This supersedes the
earlier instruction to keep every package candidate/archive. Incomplete build
scratch trees are disposable after their logs and source evidence are retained.

M1 retained at
`/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases/M1/star2lte-1792729136-64VYgG/`;
flashable SHA256 `8803ddf3d22670275b1bc07c08b385dd334e28a4703cf35a52cf74abd4655d6e`,
target-files SHA256 `8691457682d5819ac1eeee3c99625f357867ee1e6621be86fe5279b174f2aa73`.
M2 retained at
`/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases/M2/star2lte-1792900635-m9nN7O/`;
flashable SHA256 `f6e4a1069cca283ef32751abace1aedc23222d9ea5eb002d9a78e19475ee5e15`,
target-files SHA256 `fe2a891ca884883290d41efa10338d4e2249469df23f45d5f78e40585386b708`.
Both retained runs have `.complete`; their full manifests were verified with
`sha256sum -c SHA256SUMS`.

Before cleanup, M4 had no complete release. The older 20/10/2026 M1 archive
(`ba4ed009...`) and its 11 output aliases were removed. Fifteen failed M4 work
trees from attempts02–15/profile0131, the failed attempt22 tree, 42 partial
ZIPs, and the empty attempt22 release staging directory were removed. Per-run
build logs and patch/replay evidence remain under `logs/`. No firmware inputs,
source trees, or retained final package files were deleted. See
`docs/storage-cleanup-20261026.md` and
`logs/20261026-storage-cleanup/cleanup-inventory.json`.

New M4 build/output staging is placed on the second NVMe under
`star2lte-port/builds`, `star2lte-port/tmp`, and `star2lte-port/releases/M4`.


## M4 package attempt37 —27/10/2026 (historical)

M4 attempt37/source0188 is complete at `/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases/M4/star2lte-1793025724-rBdwLO`. Full signed ZIP SHA256 `b8c70dfded510beed8e14207e0fecbe33e98d3061408c6f10fdc3b1925135f3e`; target-files SHA256 `16246888838d7ec554ff327ee0c2af594ebf053f076017e57de0854e6e589c1b`. Full/target-files SHA256 checks passed again;8/8 legacy ZIP32 stages passed CRC/signature/range/order checks. `.complete`, `SHA256SUMS`, `SHA256SUMS.legacy`, legacy manifest and `artifact-review.json` are retained. This establishes package integrity, not M4 boot or a live Play Integrity verdict. Detailed review: `docs/m4-attempt37-artifact-review.md`.

Three incomplete M4 runs and four disposable replay fixtures were deleted after verification. Only the latest completed M1/M2/M4 run remains; their manifests were preserved. Old target-files was a hard link to the retained final file, so its logical size is not a reclaimed-byte claim. Cleanup proof: `logs/20261026-m4-vendor-warning-0142/cleanup-final-m4.json`. Available: primary190.53 GiB; secondary161.36 GiB.

## M4 retention update — 29/10/2026

M4-attempt0222 is the newest complete host package, retained at
`rom/out/releases/M4/star2lte-1793274163-CcPxEn/`. Its signed full ZIP SHA256 is
`c3aff071dec34579a69e7e66f88e134a9714e89d850e2a09582667ae367ee23d`; target-files
SHA256 is `c741a3d1118169e707d02874d1cc96fc9925db9e1b0101618344e9919c9dbf06`.
Both package sums and all eight legacy ZIP32 stage sums passed readback. The
legacy manifest SHA256 is
`e200a708f17b90746d5c4dbb5da1a9a5e983c4cb68fc70e835bd9b88bdd3dd2f`; its
`artifact-review.json`, build config, `.complete`, and checksum files are in the
run. This is a G965F diagnostic candidate only: it has not been flashed or
runtime tested, so M4 remains unaccepted. Review:
`docs/m4-attempt0222-artifact-review.md`.

Under the user's 26/10 latest-only rule, the previous complete M4-0219 release
directory `star2lte-1793258408-cX3l8d` was removed only after attempt0222 passed
full/target ZIP, stage and manifest verification. Its 19-file inventory with
hashes is in `logs/20261029-m4-retention-cleanup-0222/cleanup-inventory.json`;
the removal result is in `cleanup-result.json`. It contained 16,138,594,917
logical bytes. Runtime documents, build/flash logs, RTC/thermal captures and
patch evidence remain outside the deleted release directory. Exactly one M4
release directory remains. Rollback now requires a new versioned build; never
overwrite attempt0222 before a later package and its manifest pass.

## M4 retention update — 31/10/2026, r3

R3 is the only retained M4 release at
`rom/out/releases/M4/star2lte-1793426231-65eoO4/`. Its full flashable SHA256 is
`45d34b63ee275b0123fd7fca6c9e868a7a4d12c6bd8069f9c80dcb7356d33daf`; target-files
SHA256 is `664dcf2599a17b6f70bc52649f3e1beafbffd86b54452bc39b56478a845ef04a`.
Eight ZIP32 stages, their manifest and both checksum lists passed readback. R3
booted on G965F and reached Setup Wizard; this is runtime evidence, not M4
acceptance or a Play Integrity server verdict.

After the new run was complete and verified, three older completed M4 release
folders and five incomplete package scratch folders were removed under the
26/10 latest-only rule. Their existing full/target and legacy checksum files
were verified first for the completed runs. Pre-delete names, file sizes,
checksum text and result are recorded in
`logs/20261031-m4-identity-integrity-0232/release-prune-inventory.json` and
`release-prune-result.json`. About 59.2 GB of logical files were removed; NVMe
free space rose from 107 GiB to 162 GiB (55 GiB actually reclaimed; old runs
contained hard-linked/duplicate data). Build/flash logs and rollback partition
images under `logs/20261030-m4-g965f-test-0229/` remain. No firmware inputs or
source trees were deleted. Exactly one M4 release directory remains.

## M4 Integrity candidate build — 01/11/2026

A later, uniquely staged host build exists at
`rom/out/releases/M4/star2lte-1793520715-JIolKW/`. Both files passed
`sha256sum -c SHA256SUMS`; the `.complete` marker is present. Full flashable
SHA256 is `63d613364228be9df5df48d04b041fc303c6f0f3f265fa874ae92ba7c3412c77`;
target-files SHA256 is `c6cb87a98d9f5942f390154d3fb0beb932a357fdd15d2d8c89561acc201a993d`.
The build log and source/compiled verification are recorded in
`logs/20261101-m4-keybox-port/build.txt` and
`docs/m4-integrity-implementation-status-20261101.md`.

The candidate was subsequently split into eight verified ZIP32 stages and
fresh-installed on the G965F. All stage installs completed, but the device
bootlooped and was returned to TWRP; there is no Google server verdict. The
previously booted r5 remains the rollback package. Keep both while diagnosing
the failure; do not describe the new package as a passing Integrity release.
Runtime record: `docs/m4-integrity-implementation-status-20261101.md` and
`logs/20261101-m4-keybox-candidate-flash/`.
