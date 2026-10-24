# Giao thức log

Mỗi lượt thử cần `logs/<ngày>-<mốc>-<g965n|g965f>/` với model, commit fork,
patchset, hash bộ cài, hash DS-ACK và trạng thái SELinux thực. Ngoại lệ M1 F ngày20/10: người dùng đã cho phép Codex flash, reboot và thu log
sau khi xác nhận USERDATA/repartition. Ngoại lệ chỉ áp dụng G965F, không N.

Ngoại lệ bổ sung 22/10/2026: người dùng yêu cầu Codex flash fresh M2, format
USERDATA như package định nghĩa và thu log trên G965F. Phạm vi đúng serial
`M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`, package SHA256
`e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc` בלבד;
không áp dụng G965N hoặc build khác. Hồ sơ lượt thử:
`logs/20261022-m2-g965f-flash-01/`.

Đối chiếu model bằng bootloader và serial; `ro.product.model` trong TWRP đang
là tên chung `Galaxy S9+`. Luôn dùng `adb -s <serial>` cho mọi lệnh.

| Máy | Serial đã đối chiếu 20/10/2026 | Bootloader trong recovery |
| --- | --- | --- |
| G965N | G965N_SERIAL | G965NKSU5FVG2 |
| G965F | M2_G965F_SERIAL | G965FXXUHFVG6 |

Đối chiếu lại khi kết nối thay đổi. Không coi `uname` của recovery là kernel ROM.
Không lấy IMEI, keybox, dữ liệu EFS hoặc credential vào log chia sẻ.

Sau lượt boot do người dùng thực hiện, thu `logcat -b all -d`, `dmesg`, các prop
cần thiết, `getprop ro.bootloader`, `uname -a`, `getenforce`, pstore nếu có.
Log có thể chứa dữ liệu cá nhân: lưu cục bộ và rà trước khi chia sẻ/commit.
Nếu chỉ lên recovery, đọc pstore và recovery log; không tự mount `/data` để lấy
tombstone. Trường hợp thiếu quyền/file thì lưu nguyên lỗi, không tạo log rỗng
và không đổi quyền/policy chỉ để lệnh thu log thành công.

Khi báo lỗi, trích dòng thật kèm file/dòng: `avc: denied`, `CANNOT LINK EXECUTABLE`,
`dlopen failed`, `Fatal signal`, `vintf ... incompatible`, `init: ... failed`.
Chưa có log boot ROM port thì trạng thái là NOT_TESTED.

Lượt được cho phép: `logs/20261020-m1-g965f/flash-01/authorization.json` và
`logs/20261022-m2-g965f-flash-01/authorization-and-target.txt`.
Raw recovery/logcat/dmesg có thể chứa dữ liệu cá nhân; lưu mode600 ngoài Git
hoặc trong thư mục log riêng mode700. Chỉ trích lỗi liên quan sau khi rà.


Fresh-data test chỉ trong lượt được user cho phép xóa data: ghi UUID cũ,
kiểm mkfs không bị alias mount, xác minh UUID mới và Settings/probes cũ mất
trước boot. RC0 hoặc blkid ext4 riêng lẻ không đủ. Thu defaults trước mọi
Settings test write; phân biệt GUI toggle với Settings provider callback.


M2 fresh-flash follow-up (22/10/2026): the authorized G965F test entered a repeated normal boot loop and was returned to TWRP. Read-only captures are under `logs/20261022-m2-g965f-flash-01/`: `twrp-recovery-bootloop-current.log`, `twrp-loggz-bootloop-current.gz`, `pmsg-ramoops-bootloop.bin`, and `last-kmsg-after-bootloop.txt`. The last-kmsg bootloader trace ends at `Starting kernel...` after repeated normal-start records; pmsg has no kernel console panic. Thus the cause remains unknown. Preserve this as a failed/not-accepted M2 runtime test. Do not flash, format, or reboot again as part of log collection without a separate explicit instruction.


22/10/2026 — user explicitly requested “mày thử flash fresh lại rồi thu log xem”. This authorizes a repeat fresh install/test of the same retained M2 package SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc` on verified G965F serial `M2_G965F_SERIAL` / G965FXXUHFVG6 only, including package-defined fresh USERDATA format, system reboot and log collection. Eight previously verified ZIP32 stages are used, final stage from RAM. Record: `logs/20261022-m2-g965f-flash-02/`. G965N and other packages are outside scope. Repeat installation is in progress; this entry does not claim success.


22/10/2026 — current direct user authorization supersedes hash-scoped flash restrictions: “ok cài đi update agents.md cho mày toàn quyền flash”. Codex may execute project dev-device flash/fresh-format/repartition per audited installer/reboot/log/rollback, including new diagnostic artifacts, without asking again merely for a new hash. Serial/model/hash/partition-scope verification and logs remain required. Existing F-first/N-reference milestone plan and protected EFS/CPEFS/modem/bootloader rules remain. Current action: install approved G965F diagnostic BOOT SHA a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9, boot once, capture and restore baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d.
