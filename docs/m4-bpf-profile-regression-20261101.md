# M4: đối chứng profile và lỗi bpfloader — 01/11/2026

## Lý do và bằng chứng

Ứng viên dùng profile `m4-g965f-a376b-android17-no-ksu.sh`, thiếu lựa chọn
kernel WIPEONFORK, logger và bộ mạng tương thích đã hiệu chuẩn trong profile
`m4-g965f-a376b-android17-runtime-init-debug.sh`. Trên cấu hình lỗi, log
`logs/20261101-m4-vendor-compare/cache-unica/logcat_live.txt:6854` ghi
`NetBpfLoad: Android 25Q4 requires kernel 5.10.`; các dòng tiếp theo của cùng
lượt ghi `bpfloader` thoát status 7 và `reboot_on_failure` yêu cầu khởi động
lại. Bằng chứng này xác định một nguyên nhân bootloop, không quy lỗi cho
framework keybox hay xác nhận các lỗi khác đã hết.

## Thay đổi chẩn đoán trên máy và kiểm chứng

Trên G965F `M2_G965F_SERIAL`, đã khôi phục hai file logger VENDOR từ r5,
giữ nguyên init keymaster của ứng viên để không khôi phục các lệnh liên quan
EFS. Đã kiểm hash, quyền, nhãn SELinux, `e2fsck -fn` và các phân vùng bảo vệ
trước/sau lượt ghi. Đã lưu bản VENDOR trước thay đổi, không ghi EFS/CPEFS/RADIO/
RECOVERY/BOTA. Hash EFS thay đổi qua các lượt boot; không suy diễn tình trạng
dữ liệu bên trong từ hash đó.

Thay đồng bộ bốn file SYSTEM bằng bản r5 đã khóa hash: tethering APEX,
`lib64/libmeminfo.so`, `lib64/libnetd_resolv.so` và `etc/linker.config.pb`.
Đã lưu ba file gốc; `libnetd_resolv.so` vốn không có. Đọc lại hash/nhãn và
kiểm filesystem thành công. BOOT r5, bộ mạng r5 và framework ứng viên cho
`sys.boot_completed=1`, SELinux Enforcing, vào Setup Wizard; log
`logs/20261101-m4-vendor-compare/candidate-framework-live-logcat.txt:8865`
ghi Tethering APEX version 360743340 và dòng 12367 ghi `bpfloader` exit 0.
Máy đang dùng tổ hợp chẩn đoán, không phải một gói ROM đã nghiệm thu.

Rollback: các file SYSTEM gốc lưu trong `logs/20261101-m4-vendor-compare/`
với tiền tố `candidate-`; xóa resolver được bổ sung khi hoàn nguyên. Ảnh
VENDOR trước logger và dữ liệu đối chiếu nằm trong thư mục scratch
`vendor-control.GIsvFs` trên ổ dự án thứ hai. Bản r5 hoàn chỉnh được giữ lại.

## Build bền vững và giới hạn

Build lại dùng profile runtime đã kiểm kernel diagnostic sẵn có, logger và bộ
mạng tương thích; không build kernel mới. Release
`rom/out/releases/M4/star2lte-1793544961-rXbfep/` đã hoàn tất; SHA256 flashable
`154c9493625c238974e2fd4b256e03bd4d03c308c756633b55d9a48e647e1040`,
target-files `3b27ff439b0e1e8d500d15532f5d2dfb55f2a36dc699aadcc17e29cebcbda640`;
hai hash đã đọc lại thành công. Tám stage ZIP32 đã qua CRC/chữ ký, hash từng
stage đã đọc lại khớp manifest; `SHA256SUMS.legacy` lưu kết quả. Các gate
compiled ADB/Keybox/PIF và metadata/logger vendor trong image đều PASS.
Người dùng yêu cầu dừng sau chia và kiểm hash, để mai flash: chưa cài release
này lên máy. Keybox
mặc định tắt trong lượt boot này; chưa kiểm attestation hay verdict Google.
Setup Wizard chưa hoàn tất khi kiểm ADB. Bixby donor còn lỗi đã được người dùng
chấp nhận; không gỡ khai báo hỗ trợ NFC/Bixby.

## Fresh flash 02/11/2026

Đã cài tám stage của release trên G965F `M2_G965F_SERIAL`, TWRP 3.7.0,
bootloader `G965FXXUHFVG6`; tất cả exit 0 và marker cuối `next=done`.
Installer báo format USERDATA; UUID sau cài là
`c0297687-6fbb-4787-89b5-8518fd3b88b9`. Các hash EFS/CPEFS/RADIO/RECOVERY/
BOTA trước và ngay sau cài khớp; BOOT đọc lại đúng
`55f3af8c3dc9c5115fd80f944849c8205c2d2bc5459bb2ecc7114d842669fe77`.
Đã lưu BOOT và GPT trước cài tại `logs/20261102-m4-correct-profile-flash/`.
Đã reboot Android; PC thấy Samsung USB nhưng chưa thấy ADB, trạng thái màn hình
và kết quả khởi động còn chờ xác nhận. Chưa có kết quả Play Integrity.

Sau đó người dùng xác nhận boot thành công; ADB Android đã xuất hiện và đo được
`sys.boot_completed=1`, Setup Wizard hoàn tất, launcher foreground, Android 17 /
One UI 9, `SM-A376B`, thiết bị thực `SM-G965F`, SELinux Enforcing. Log
`logs/20261102-m4-correct-profile-runtime/logcat.txt:6349` ghi tethering APEX
360743340, dòng 6767 ghi bpfloader thoát status 0. Log còn ba lần abort của
`app_process` lúc đầu boot, ví dụ dòng 21693: `Codec2 service "default"does not
have IConfigurable.`; chưa xác định tác động đến phát video. Bixby crash dòng
42337 `No matching ComponentResult found!` là giới hạn đã chấp nhận. PIF và
Keybox property đều true khi đo, nhưng không suy ra keybox hợp lệ hay verdict
Play Integrity; chưa kiểm attestation server. Các thử nghiệm phần cứng vẫn
cần được tiến hành cho release mới.

## Kiểm thử sau boot 02/11/2026

Chạy lại Play Integrity API Checker qua ADB trên Wi-Fi đã validated. Checker
hiển thị không đạt BASIC/DEVICE/STRONG; chi tiết JSON cho thấy không có nhãn
device integrity, `appRecognitionVerdict=UNEVALUATED` và
`appLicensingVerdict=UNEVALUATED`. Đây là kết quả của checker ở thời điểm thử,
không xác định được lý do duy nhất. Log local
`logs/20261102-m4-correct-profile-runtime/integrity-video-logcat.txt:25036`
ghi `Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)`; dòng 25047 ghi
`Integrity key attestation record generated successfully.` Hai dòng không
chứng minh attestation hardware hay keybox hợp lệ. Property Keybox và PIF đều
true nhưng trạng thái import/hiệu lực keybox chưa được xác minh.

Phát clip thử H.264 640x360 trên Samsung Video: hình đã hiển thị, log dòng
29647 và 29655 ghi `OMX.Exynos.avc.dec` chuyển sang `Executing`. Đây là một
phép thử ngắn không có âm thanh, không xác minh codec, định dạng khác hay sửa
được lỗi Codec2 đầu boot. SIM hiển thị `LOADED,ABSENT` và mạng Wi-Fi được
validated; chưa thử cuộc gọi. File video tạm trên điện thoại đã xóa.

Camera Samsung mở được active client camera ID 20; chưa chụp/đánh giá ảnh.
Một lần reboot chủ động tiếp theo đổi boot ID và trở lại
`sys.boot_completed=1`, model SM-A376B, SIM `LOADED,ABSENT`, enforcing;
`reboot-logcat.txt:10387` xác nhận bpfloader exit 0. Chưa kiểm loa/micro,
cuộc gọi, Bluetooth audio, nhiều codec hoặc ổn định dài hạn trên build này.

Kiểm PIF/Keybox sau đó: màn hình UN1CA báo `Không có dữ liệu keybox`. Bốn
property PIF (version 20261011, fingerprint, security patch và first API) khớp
payload `UN1CA/static_resources` nhánh sixteen. Nút cập nhật báo đang dùng
phiên bản mới nhất; lần Checker tiếp theo vẫn không có nhãn BASIC/DEVICE/STRONG.
Không có keybox được nhập và không suy ra nguyên nhân duy nhất của verdict.

## Thử nhập XML cộng đồng 02/11/2026

Người dùng cung cấp XML tại Downloads. Kiểm offline không in nội dung khóa:
XML parse được, có một key ECDSA và bốn chứng chỉ, public key khớp chứng chỉ
đầu; chứng chỉ đầu hết hạn 2026-11-04 10:54:52 UTC. Settings không nhận file,
vẫn hiển thị không có dữ liệu và secure setting vẫn rỗng. Fork
`KeyboxUtils.smali` tại validateXml yêu cầu đủ cả private key và chuỗi chứng
chỉ cho RSA lẫn ECDSA; file chỉ có ECDSA nên không qua điều kiện này. Log còn
có `Unable to check Uri permission because caller is holding WM lock; assuming
permission denied`; chưa chứng minh lỗi URI là nguyên nhân độc lập. Không bỏ
qua validator hoặc tự thêm RSA. Đã xóa bản sao XML tạm trên điện thoại; file
gốc người dùng trên PC được giữ. Chưa có phép thử verdict sau khi nhập thành công.
