# Đối chứng boot ứng viên M4 — 01/11/2026

> Cập nhật sau các lượt đối chứng dưới đây: lỗi bootloop đã được khoanh vùng
> bằng log `bpfloader`; máy đã boot tới Setup Wizard với framework ứng viên,
> BOOT và bộ mạng r5. Đây là trạng thái ghép chẩn đoán, chưa phải ROM mới đã
> đóng gói hoặc bằng chứng Play Integrity/keybox hoạt động. Xem
> `docs/m4-bpf-profile-regression-20261101.md`.

Thiết bị: G965F `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`, TWRP
`3.7.0_9-0`. Mỗi lần ghi đều kiểm model/bootloader/serial và hash trước/sau;
không ghi EFS/CPEFS/RADIO/RECOVERY/BOTA. Raw log và bản sao framework/BOOT
nằm tại `logs/20261101-m4-keybox-control/`.

## Sai lệch đầu vào phát hiện được

Build ứng viên dùng sai profile kernel so với r5: log ứng viên
`logs/20261101-m4-keybox-port/build.txt:2551` chọn
`DS-ACK-V1.12-08.05.2026-Enforcing-OneUI7.zip`, trong khi log r5
`logs/20261031-m4-r5-repair/build-r5.txt:2553` chọn gói G965F WIPEONFORK
diagnostic đã tồn tại. Đây là sai sót chọn profile khi build ứng viên, không
phải kernel mới được build ở lượt đối chứng. Hồ sơ trước đó đã chứng minh lỗi
`arc4random data MADV_WIPEONFORK failed: Invalid argument` trên BOOT baseline
trong `docs/m2-runtime-kernel-fix.md`; không suy nó là lỗi hiện tại nếu thiếu
log runtime tương ứng.

BOOT ứng viên toàn phân vùng là `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`;
BOOT r5 lưu trước flash là `55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`.
Framework ứng viên là `3975030c7f39b2830fc400b0ef213c89977220b9c3f80f6577fb861df5617b38`;
framework r5 trích từ target-files khớp hash đã đo trên máy r5:
`096502ebe60b6d2b393608db858866702274d3858454d93575d9a389655df80b`.

## Ma trận thử

| Framework | BOOT | Kết quả | Bằng chứng |
| --- | --- | --- | --- |
| Ứng viên | Ứng viên `cc618…` | Bootloop sau fresh flash | `logs/20261101-m4-keybox-candidate-flash/bootloop-01/` |
| r5 | Ứng viên `cc618…` | Không thấy ADB Android; log ghi nhiều lần Normal → `Starting kernel...` rồi vào recovery | `control-last-kmsg.txt`, `control-pstore/` |
| Ứng viên | r5 `55f3…` | Người dùng xác nhận bootloop | `boot-r5-control-state.txt`, `boot-r5-control-pstore/` |
| r5 | r5 `55f3…` | Người dùng xác nhận bootloop | `r5r5-state.txt`, `r5r5-last-kmsg.txt` |

Ở ba lượt đối chứng, các phân vùng SYSTEM/VENDOR/ODM còn lại là ứng viên;
USERDATA đã trải qua nhiều lần boot thử nên không phải dữ liệu fresh độc lập
cho từng ô. Framework r5 khác ứng viên theo cả nội dung framework, không chỉ
một hook. Bốn kết quả này loại bỏ kết luận đơn giản rằng chỉ thay BOOT hoặc
chỉ thay framework sang r5 là đủ sửa bootloop. Chúng chưa xác định nguyên nhân
và chưa chứng minh Keybox gây lỗi hay vô can.

Trước mỗi boot đối chứng, framework và BOOT được đọc lại đúng hash dự kiến;
`e2fsck -fn` SYSTEM trả 0; hash bảy phân vùng bảo vệ không đổi trong lúc ghi.
Hash EFS thay đổi qua các chu kỳ boot (không có lệnh ghi/khôi phục EFS), nên
không coi hash đó là bằng chứng nguyên nhân hay tình trạng dữ liệu bên trong.
CPEFS/RADIO/RECOVERY/BOTA giữ hash trước/sau các lượt ghi.

## Giới hạn log và trạng thái cuối

Build-config r5 đặt `EXYNOS9810_ENABLE_INIT_DEBUG="true"`; ứng viên đặt `false`.
Đọc VENDOR ứng viên trong TWRP xác nhận thiếu cả
`/vendor/etc/init/init.debug.rc` và `/vendor/bin/init_debug_log.sh`. Do đó thiếu
`/cache/unica` là phù hợp cấu hình logger và không chứng minh boot chết trước
`post-fs`. Các pstore đối chứng chỉ có 951 byte, gồm thông điệp adbd/graphics
có thể thuộc recovery; không có fatal userspace đủ xác định nguyên nhân. Ví dụ
`r5r5-last-kmsg.txt:24078` ghi `Starting kernel...`, sau đó các bootloader cycle
khác tiếp tục; `:25566` ghi `set_oneshot_recovery: recovery mode set!`.

Tại thời điểm kết thúc ma trận trên, máy ở TWRP với **BOOT r5 và framework r5, nhưng SYSTEM/VENDOR/ODM còn lại
là ứng viên**. Đây không phải rollback hoàn chỉnh r5. Hai framework và cả hai
BOOT đã lưu/đối chiếu hash; rollback hoàn chỉnh cần toàn bộ package r5 đã giữ.
Không có verdict Google và không có kiểm tra keybox runtime.

Bước chẩn đoán tiếp theo: tạo biến thể đối chứng từ đúng profile r5
`m4-g965f-a376b-android17-runtime-init-debug.sh`, kiểm kernel diagnostic và
logger trong image cuối, đồng thời so sánh những thay đổi SYSTEM/VENDOR còn lại
với r5. Hoặc dùng logger đã kiểm chứng hoạt động sớm trên cùng kernel. Chỉ thay
một biến cho mỗi lượt và thu log trước khi sửa tiếp.
