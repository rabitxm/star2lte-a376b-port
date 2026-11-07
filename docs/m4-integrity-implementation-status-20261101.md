# M4 Play Integrity — trạng thái triển khai, 01/11/2026

**Đối chứng sau flash:** các tổ hợp framework/BOOT r5 và ứng viên đều chưa
boot trên phần SYSTEM/VENDOR/ODM còn lại của ứng viên. Phát hiện build ứng viên
đã chọn sai gói BOOT so với r5 và tắt init logger; nguyên nhân bootloop vẫn chưa
được log xác định. Xem [báo cáo đối chứng](m4-keybox-boot-control-20261101.md).

## Kết quả hiện tại

Fork đã được sửa và build một ứng viên M4 BZI6 có phần tương thích Keybox/PIF.
Các kiểm tra source và compiled trong build đều qua; đây không chứng minh key
attestation phần cứng, Play Protect certification hay verdict Google. Không có
keybox được cung cấp hoặc nhập. Ngày 01/11/2026 ứng viên đã được chia thành tám
ZIP32, fresh-flash lên G965F và **bootloop**; máy được đưa lại TWRP. Vì vậy phần
port chưa đạt runtime gate và không được coi là thành công.

Ứng viên nằm tại
`rom/out/releases/M4/star2lte-1793520715-JIolKW/`.

| Tệp | SHA256 |
| --- | --- |
| Flashable ZIP | `63d613364228be9df5df48d04b041fc303c6f0f3f265fa874ae92ba7c3412c77` |
| Target-files ZIP | `c6cb87a98d9f5942f390154d3fb0beb932a357fdd15d2d8c89561acc201a993d` |

`sha256sum -c SHA256SUMS` trả `OK` cho cả hai tệp. Tám stage cũng trả `OK` theo
`SHA256SUMS.legacy`; manifest ghi `status=PASS`, CRC/signature/ZIP32 đều PASS.
Build record:
`logs/20261101-m4-keybox-port/build.txt`.

## Kết quả flash và boot thật

Thiết bị được đối chiếu trong TWRP là `SM-G965F`, serial
`M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`. Cả tám stage cài tuần tự với
installer exit 0 và marker cuối `next=done`. Installer fresh-format USERDATA.
Trước boot, `e2fsck -fn` trả `rc=0` cho SYSTEM, VENDOR, ODM, USERDATA, CACHE,
OMR và HIDDEN; BOOT có SHA256
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.
EFS/CPEFS/RADIO/RECOVERY/BOTA không đổi trong lúc cài.

Máy sau đó bootloop và được người dùng đưa về TWRP. `/cache/unica` không được
tạo. `pmsg-ramoops-0` chỉ có 3.549 byte; bản trích chuỗi ghi `adbd` đóng transport
ở dòng 20–25 rồi lặp khởi tạo/đóng transport ở các dòng 27–92. Nó không chứa
`Fatal signal`, kernel panic, linker fatal hay lỗi Keybox/Keystore đủ để chốt
nguyên nhân. `last-kmsg.txt` chứa nhiều vòng boot, gồm `Starting kernel...` tại
dòng 21271 rồi một bootloader cycle mới từ dòng 21305; bản ghi sau cùng chuyển
vào recovery. Do thiếu dòng userspace gây reboot, hiện chỉ kết luận được ứng
viên không boot, không được suy rằng Keybox là nguyên nhân đã chứng minh.

Sau các vòng boot, EFS đổi từ
`e9df4094ee695e60da730408422fc08df7a9363d641a6be15f668e0bf0f7f8d9`
sang `1fc032634977ff6277ccfe034c652430e886b1b520476db3d33c3841d89aa1ee`;
CPEFS/RADIO/RECOVERY/BOTA và BOOT không đổi. Codex không ghi hay khôi phục EFS.
Hồ sơ cài và log cục bộ nằm ở
`logs/20261101-m4-keybox-candidate-flash/`.

## Lý do và phạm vi thay đổi

Mục tiêu của bản vá là tương thích source BZI6/SDK37 với giao diện Keystore2
của fork và giữ các mục điều khiển hiện có trong Settings. Tệp chính gồm:

- `rom/unica/mods/keybox/a376b_android17_keybox.py`
- `rom/scripts/internal/apply_modules.sh`
- `rom/unica/mods/pif/customize.sh`
- `rom/unica/mods/settings/a376b_android17_boot_limits.py`
- `rom/unica/mods/settings/SecSettings.apk/res/xml/unica_spoof_settings.xml`
- `rom/unica/mods/settings/SecSettings.apk/res/values/strings.xml`
- `rom/unica/mods/settings/SecSettings.apk/res/values-vi/strings.xml`
- `rom/scripts/internal/verify_exynos9810_release_defaults.py`

Kernel DS-ACK, vendor, modem, radio, EFS/CPEFS và bootloader không được thay đổi.
Build log xác nhận patch nguồn tại dòng 1198 và PIF tại dòng 1211; Settings tại
dòng 1362; compiled Keybox/PIF gates tại dòng 2453–2455 và 2475–2477; kernel
đã chọn tại dòng 2552; flashable verifier tại dòng 2557. `git diff --check`,
`bash -n` trên shell entry points và Python compile checks đều qua.

## Giới hạn và quyết định triển khai

Không có đầu vào keybox được xác minh quyền sử dụng và không có kiểm tra Google
server cho ứng viên này. Build thành công hoặc trạng thái PIF trong Settings
không thể được báo là Play Integrity đạt. Các hook thay đổi luồng key-attestation
có thể tạo kết quả khác với bảo đảm thật của TEE. Lượt runtime đã thất bại trước
khi có thể kiểm tra Settings, crypto hoặc verdict; không có kết quả Integrity
nào được tạo từ ứng viên. G965F hiện ở TWRP và r5 vẫn là gói rollback.

Theo tài liệu Google, `MEETS_DEVICE_INTEGRITY` trên Android 13+ cần bằng chứng
hardware-backed về bootloader khóa và image Android được nhà sản xuất chứng
nhận. Google liệt kê bootloader mở khóa, OS sửa đổi hoặc root là các nguyên nhân
phổ biến khiến thiết bị không được chứng nhận; hướng xử lý chính thức là quay về
trạng thái factory do hãng ký và trạng thái bootloader theo hướng dẫn của hãng.
Nguồn: [Play Integrity verdicts](https://developer.android.com/google/play/integrity/verdicts),
[Play Protect certification](https://support.google.com/googleplay/answer/7165974?hl=en).

## Rollback và bước hợp lệ tiếp theo

Không chạy thêm vòng boot trước khi có logger userspace đủ sớm hoặc một build
đối chứng loại riêng thay đổi Keybox. Rollback đầy đủ phải dùng toàn bộ package
r5 tương ứng, không chỉ BOOT vì BOOT của hai bản có cùng hash. Đối với mục tiêu
integrity, bước có thể xác nhận là kiểm tra certification/boot state với
firmware OEM phù hợp và quy trình chính thức của hãng; nếu tiếp tục phát triển
ROM này, giới hạn ở tương thích Keystore/TEE thực, log chẩn đoán không làm giả
attestation và kiểm thử khởi động/phần cứng độc lập.
