# Review M1 G965F — đã cài và boot xác nhận

Ngày20/10/2026, người dùng xác nhận USERDATA đã backup ra PC hoặc không cần giữ,
chấp nhận repartition/xóa data, rồi yêu cầu Codex tự flash/đọc log. Ngoại lệ
chỉ cho M1 G965F serialM2_G965F_SERIAL; G965N giữ đối chiếu.
Ủy quyền gốc: logs/20261020-m1-g965f/flash-01/authorization.json.

## Bộ cài thực dùng

M1 là đối chứng S22 OneUI8/Android16 SDK36 + Hybrid Vendor VNDK33 của fork và
blob/cấu hình F đã kiểm. Không phải A37 OneUI9/Android17; chưa nghiệm thu N.
Fork commit2b25ae6fc3098650be249bd1898e21e563cd5df1 +16 patch /27 file replay.
S22S901BXXSOGZH3/EUX, targetG965FXXUHFVG4/PHN. Modem/bootloader máy F giữ nguyên.

Nguồn full ZIP SHA256 `6a0096cee0750d0d47dcaa0605bac7b65d362c0c7c40950814683d427fb745eb` đã qua CRC, CMS, model/order/kernel
hash audit. Full ZIP5479600816B không dùng sideload trực tiếp trên TWRP9 này:
log báo size1184633520B (lệch2^32), rồi Zip file is corrupt trước updater.
Đã đối chiếu GPT nguyên vẹn sau lượt bị từ chối; host exit0 không phải thành công.
Bản cũ hơn hashced0ee... chưa flash và được lưu inputs/m1-candidates/pre-recovery-fix.

Mode --legacy-sideload mở rộng chính scripts/build_flashable_zip.sh của fork,
đọc full ZIP đã verify; không build image/kernel hoặc viết build system mới.
Manifest: logs/20261020-m1-g965f/flash-01/legacy-manifest.json. Bộ8 phần:

| Phần | Vai trò | Byte | SHA256 |
| --- | --- | ---: | --- |
| 1/8 | first-system | 311,193,254 | `b56340ff8e76f87a078d8d7c04ef1539afcd0f61e378ce144da22933fffa1036` |
| 2/8 | system | 1,153,382,747 | `eb1df63ea9e022d7e09231c4bdfaaa72c25b1085adcc84865588fec6f9412007` |
| 3/8 | system | 784,348,080 | `54971c7946c16e5ebf834d617c288b57dfa6d3369f691c73b2eb25a63cfa0e83` |
| 4/8 | system | 789,753,251 | `113db3fa85370518699c99a2ea223ba51a273a56848df7434674ca839c52c3b1` |
| 5/8 | system | 1,005,675,466 | `9a57e658b2a1ef8f42dd79cbae39eb57285bd71b547033c4ebe18b9ffdbc97db` |
| 6/8 | system | 771,466,434 | `f6381cea1890d369b746f5a024b299cda4947cbd53dd60a475043202b6ffb1d5` |
| 7/8 | system | 151,199,417 | `4604b660f0323d690d5e3a5bf4127f61aba78c0534e472443eb88f02aa1632fb` |
| 8/8 | final | 581,134,468 | `0b7c94797b018134e3809bc54ab78daee5b290a308c73a3632e8df8609bb3eee` |

Mỗi phần là ZIP32 <2GiB, CRC và chữ ký whole-file OTA CMS với testkey fork PASS.
Header transfer gốc được giữ, range/data order không đổi; mỗi chunk Brotli
round-trip đúng SHA raw. Payload được copy đối chiếu size/CRC từ gói full.
Test live actual updater của fork trên file RAM: wrong-order abort/no-write;
partial fragmented ranges ghép đúng32768B. Không viết phân vùng trong test đó.

## Thứ tự và tác động trên máy

Model assert F/FDS/FD trước mọi thay đổi. Phần1 giữ install-start gốc: chuẩn bị
layout, auto-repartition nếu thiếu, format system/vendor/odm/omr/preload rồi ghi
chunk system đầu. Phần2–7 tiếp tục các range system; chưa ghi BOOT hoặc format
USERDATA. Phần8 giữ tail gốc: vendor, bootfix SIM từ EFS ro,noload, odm, template
BOOT rồi nguyên gói kernel, cuối cùng format cache/omr/preload/USERDATA.
ID bộ và next-part trong /tmp chặn sai thứ tự; không reboot giữa các phần.
Nếu recovery phải reboot giữa chừng, bắt đầu lại từ phần1; không ép bỏ guard.

Lượt cài thật đã hoàn tất8/8 phần; marker thành công từng phần nằm trong
flash-01/legacy-status.json. Layout actual9500/1300/700/600MiB, BOOT khớp repack
RAM đầy partition, USERDATA ext4. Đã reboot F và thu boot_completed1/ảnh Welcome;
người dùng xác nhận “đã boot thành công”. Xem m1-boot-report.md về runtime và giới hạn.

## Kernel và công cụ recovery

Nguyên DS-ACK duhansysl V1.12 08/05/2026 Enforcing-OneUI7 không KernelSU,
SHA2569a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072.
[Nguồn release](https://github.com/duhansysl/exynos9810-kernel/releases/tag/V1.12),
[source commit](https://github.com/duhansysl/exynos9810-kernel/tree/914eabe7b6799ab0eb159360a5f9731dd50d0f1d).
Không sửa/build kernel hoặc đổi version. Template BOOT kernel-size0 không flash
riêng. Native tool gốc đã repack trong RAM: file template40MB; bản mô phỏng
BOOT đủ partition55MiB giữ padding và vừa đúng BOOT. Chưa phải bằng chứng boot.

TWRP3.7.0_9-0 thiếu /system/bin helper; patch0013 dùng /sbin đã kiểm và guard.
Bundled sgdiskAPI35 lỗi CANNOT LINK EXECUTABLE trên TWRP9; patch0015 pin static
ARM64 GPT fdisk1.0.5 SHA84d08da6871b5e4f2c4f7499f1896f11549cb036c8d3ed204c1f2f96d4e89785.
[Provenance prebuilt](https://github.com/evdenis/disk/tree/903e56c2d61e276c9d500bbda7c634842993613c).
Không cài module Magisk. Repository không cung cấp recipe tái dựng binary này.
Live readonly GPT4K print/verify và script repartition chuyển duy nhất DISK sang
file RAM PASS, entry1–17 không đổi. EFS đọc topology actualF2SIM/dsds bằngro,noload.

## Kiểm sau cài và gate boot

Sau đủ8 phần, verify_preboot.py kiểm cả GPT/header/table CRC, prefix1–17,
kích thước node, BOOT/kernel/DTB/ramdisk, USERDATAext4 đều PASS. Hash CPEFS/RADIO/
RECOVERY/BOTA0/1/2 giữ nguyên; **EFS đổi raw SHA nên audit tổng vẫn FAIL**.
Chưa xác minh nội dung EFS tương đương. Người dùng được báo và trả lời
“tiếp tục đi, máy dev thì backup làm gì”; ngoại lệ ràng buộc hash audit ghi
flash-01/efs-boot-exception.json. Đã boot theo chỉ thị, không restore EFS hoặc
biến audit thành PASS. GPT/BOOT/RECOVERY trước cài lưu cục bộ, chưa thử restore.

Raw recovery/logcat/dmesg mode600 ngoài Git tại inputs/m1-runtime-logs/flash-01.
boot-status.json = BOOT_COMPLETED_LOGS_CAPTURED; user đã xác nhận gate boot M1F.
Runtime Android16/SDK36/OneUI80000, kernelMay nguyên bản và VNDK33. Chức năng,
USB/ADB và SELinux debug còn mở; xem m1-boot-report.md. Chưa nghiệm thu N/A37;
Android17 chấp nhận HAL/VNDK nào cần xác minh bằng log. Binary chỉ cục bộ.
