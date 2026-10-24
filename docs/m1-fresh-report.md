# M1 G965F — flash fresh 21/10/2026

## Kết quả

Đã sideload đủ8/8 phần của đúng gói ADB17, Format Data thật, boot fresh và
reboot kiểm tra thành công. Máy để nguyên thiết lập ban đầu, chưa thêm tài khoản,
không khôi phục data cũ. G965N không bị nhắm tới. Vẫn là M1 S22 Android16/SDK36/
OneUI80000 + Hybrid Vendor VNDK33 + DS-ACK May nguyên bản không KernelSU;
không phải nghiệm thu A376B Android17.

## Vì sao lần trước còn data

Bằng chứng mới ở `inputs/m1-runtime-logs/flash-03/fresh-format/`:
`installer-format-attempt.txt:2` ghi
`/dev/block/platform/11120000.ufs/by-name/USERDATA is mounted; will not make a filesystem here!`.
Recovery vẫn ghi `Updater process ended with RC=0` vì format data là optional.
/proc/mounts có nhiều /sdcard alias cùng USERDATA; script cũ chỉ tháo /data.
Ngay cả lệnh TWRP Format Data lần đầu cũng báo `Unable to wipe Data.` và UUID
không đổi. Không reboot sau thất bại đó.

Đã tháo tất cả alias bằng umount thường, kiểm USERDATA không còn mount, gọi lại
TWRP Format Data. Kết quả `Done.`; UUID đổi từ e3b5a33b-e849-4bc9-a2cd-337133b55b75
sang43622329-2c28-45d1-86ca-09f5d35ce89e. Settings global/secure/system XML,
packages.xml, adb_keys và hai dấu kiểm tra /data + internal storage đều mất
trước boot. Chỉ ext4 TYPE hoặc RC0 không được coi là bằng chứng data mới.

## Mặc định trước mọi thay đổi Settings

| Mục | Giá trị thực |
| --- | --- |
| device_provisioned / user_setup_complete | 0 / 0 |
| adb_enabled / development_settings_enabled | 1 / 1 |
| rampart_main_switch_enabled | 0 |
| rampart_strict_protection_switch_enabled | 0 |
| rampart_blocked_adb_cmd / rampart_blocked_usb_data_transfer | 0 / 0 |
| rampart_suw_main_on | 0 |
| ro.adb.secure / ro.debuggable | 0 / 1 |
| sys.usb.config / persist.sys.usb.config | sec_charging,adb |
| init.svc.adbd | running |

Fresh boot được watcher thấy boot_completed1 ở165 giây; reboot kiểm tra ở46
 giây. Đây là thời điểm lấy mẫu, không phải đo thời gian hoạt ảnh chính xác.
Hai lần keyless test đều gửi USB CNXN trực tiếp, không mở key file/không gửi
RSA AUTH reply; thiết bị trả CNXN và cho OPEN shell `USB_NO_KEY_OK SM-G965F`.
Kiểm này không dựa vào khóa PC đã được điện thoại tin trước đó.

## Công tắc và giới hạn kiểm chứng

- Auto Blocker UI mặc định Off, descriptions hiện đủ. Bật thật bằng UI làm
  master1, blocked_adb1, adb_enabled0, adbd stopped, USB sec_charging và host
  mất thiết bị ADB. Tắt lại bằng UI + hộp thoại gốc trả master0/adb1 và ADB
  kết nối lại. Temporary helper không dùng fallback trong test này.
- Tắt adb_enabled qua Settings provider khi master0 làm adbd stopped, USB none
  và host mất ADB. Giá trị vẫn0 trong cửa sổ quan sát, không bị boot/service
  ép lại. Helper thử tạm khôi phục1 bằng callback Settings bình thường.
  Đây là kiểm callback, chưa phải kiểm công tắc USB debugging bằng GUI.
- Developer options chưa mở được khi máy ở Welcome. Không đổi provisioned/setup
  để vượt bước thiết lập; công tắc USB debugging GUI còn chờ nghiệm thu sau setup.
- Đã xóa helper/UI XML tạm khỏi điện thoại, stay_on_while_plugged_in trở về0.
  Lệnh cleanup trước reboot bị host timeout20s; đã xác minh/dọn lại thành công
  sau reboot bằng final-cleanup.json. Trạng thái cuối vẫn provisioned0/setup0,
  adb1/master0/SUW0; không có vòng lặp hoặc init script thử được cài vào ROM.

## Phân vùng và log còn mở

Preboot audit PASS: GPT CRC hai bản, prefix1–17, kích thước phân vùng, BOOT,
kernel/DTB/ramdisk khớp; EFS/CPEFS/RADIO/RECOVERY/BOTA không đổi so với baseline
ngay trước flash-03. Không khôi phục hay đánh dấu lại EFS baseline cũ flash-01;
audit lịch sử FAIL đó vẫn giữ nguyên.

Snapshot fresh boot không có mẫu FATAL EXCEPTION/Fatal signal/ANR/Kernel panic/
BUG/Call trace/VINTF incompatible. USB sớm vẫn có dòng `UDC ... No such device`;
148 khớp trong cả logcat+dmesg có trùng giữa hai snapshot. Không kết luận đã sửa
dứt USB hoặc toàn bộ phần cứng/SELinux. Không sửa/build kernel.

## Artifact, patch và evidence

Gói được cài có SHA256
`ba4ed009bd984ab647bd4316700081d8f6db54b592b0f005fac2f2fcc5d3aebb`, gồm17patch.
Lượt này giữ nguyên payload đó để thử các sửa ADB đã có trên data mới.
Patch0018 mới sửa source installer: tháo mọi alias, không lazy-unmount, lỗi busy/
mke2fs phải chặn cài data, verifier từ chối optional cũ. Shell syntax, behavioral
fixtures, archive guard accept/reject và replay18patch/29file khớp byte PASS.
Patch0018 chưa nằm trong ZIP đang chạy; sẽ có hiệu lực khi package tiếp theo.

Evidence cục bộ ở `logs/20261021-m1-g965f/flash-03/`:
fresh-data-audit.json, preboot-audit.json, legacy-status.json, boot-status.json,
boot-status-reboot.json, fresh-usb-no-key.json, reboot-usb-no-key.json,
runtime-summary.json, final-cleanup.json, format-guard-fixtures.json,
format-archive-guard-fixtures.json, patch18-replay.json.
Raw logs/ảnh/UI hierarchy mode600 trong inputs/m1-runtime-logs/flash-03, bỏ Git.
