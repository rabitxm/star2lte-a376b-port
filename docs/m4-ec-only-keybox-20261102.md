# M4 G965F — thử hỗ trợ keybox chỉ có ECDSA

Lý do: XML người dùng cung cấp chỉ có ECDSA; validator Settings và
`DefaultKeyboxProvider.hasKeybox()` yêu cầu cả RSA lẫn EC. Kiểm offline cho
thấy khóa khớp chứng chỉ đầu, các chữ ký chuỗi hợp lệ, root khớp pin sẵn có
của Settings và không có số serial trong danh sách thu hồi Google tại thời
điểm kiểm. Chứng chỉ đầu hết hạn 04/11/2026 10:54:52 UTC. Không đưa XML,
khóa hoặc serial chứng chỉ vào repo/log chia sẻ.

File ảnh hưởng: `rom/unica/mods/settings/customize.sh`,
`rom/unica/mods/keybox/a376b_android17_keybox.py` và helper mới
`rom/unica/mods/keybox/a376b_android17_ec_only.py`. Giới hạn M4 donor BZI6 /
G965F. Khi không có RSA hoàn toàn, Settings dùng bản sao danh sách EC trong
biến kiểm tra cục bộ, vẫn chạy kiểm pin root gốc và giữ nguyên XML lưu vào
Settings. Provider nhận EC đầy đủ; các yêu cầu RSA thiếu key/chain trả về
`null` để proxy chuyển về keystore ban đầu trước mọi thay đổi database.
Các trường hợp RSA chỉ có một phần vẫn bị validator gốc từ chối.

Kiểm chứng ban đầu: Python compile, bash syntax, và lắp ráp DEX từ các class
đã sửa bằng smali API 29 (minSdk của framework) PASS. Phải tiếp tục build sạch,
kiểm compiled code của Settings/framework, kiểm boot và rollback, import UI,
kiểm trạng thái và verdict. Chưa có kết luận tương thích runtime hay Play
Integrity; không sửa kernel, EFS hoặc modem. Rollback về release M4
`star2lte-1793544961-rXbfep` và bỏ các thay đổi source trên.

Build sạch thành công với profile runtime G965F. Gate compiled kiểm provider,
nhánh fallback RSA trong generate/import và validator Settings PASS; gate ADB,
SELinux và các image ext4 của fork PASS. Release
`rom/out/releases/M4/star2lte-1793584471-sBoBYd/`, flashable SHA256
`41a666c005031115d563475246bbe2722d55b3b391027d70d0500318a1bd7162`.
Tám stage được kiểm CRC/chữ ký/ZIP32 và hash đọc lại; cả tám cài exit 0.
EFS/CPEFS/RADIO/RECOVERY/BOTA khớp hash trước/sau flash; BOOT khớp bản
diagnostic đã chọn. Lượt fresh boot sau flash đạt `sys.boot_completed=1`,
model SM-A376B, hardware SM-G965F, SELinux Enforcing. Setup Wizard chưa hoàn
tất; chưa import XML hay kiểm verdict sau bản sửa. Log flash tại
`logs/20261102-m4-ec-keybox-flash/`.

## Kiểm runtime sau Setup Wizard

Ngày 02/11/2026 đã nhập file ECDSA người dùng cung cấp qua giao diện Settings.
Settings đổi từ không có dữ liệu sang `Đã qua kiểm tra cục bộ; chưa xác nhận
Play Integrity`. Đã xóa bản sao XML tạm khỏi thư mục Download trên máy; file
gốc PC và dữ liệu người dùng chủ động nhập được giữ, không ghi khóa vào log.
Lượt Checker mới lúc 10:02–10:03 hiển thị BASIC đạt, DEVICE và STRONG không
đạt; JSON cho thấy `deviceRecognitionVerdict=[MEETS_BASIC_INTEGRITY]` và
`appLicensingVerdict=LICENSED`. Log đã lọc tại
`logs/20261102-m4-ec-keybox-runtime/filtered-log.txt:2-4` ghi Play Store tạo bản
ghi attestation và callback kiểm tra hoàn tất. Kết quả này chứng minh import
EC-only và verdict BASIC ở lượt thử đó; không xác định keybox là nguyên nhân
duy nhất so với lượt cũ vì vừa fresh flash, thiết lập và đăng nhập lại. RSA
fallback thực thi và các ứng dụng khác chưa được thử độc lập.

## Đối chứng EC/RSA và reboot

Probe app UID tạo alias EC có challenge riêng; logcat 10:08:53–54 ghi
`key_inside_secure_hardware:true`, `PASS key generation and sign/verify`,
`certificate_chain_count:5`, `PASS challenge bytes present in attestation
extension` và `PASS temporary alias deleted`. Certificate thứ hai trong chuỗi
khớp certificate đầu của file keybox đã nhập; chữ ký các mắt xích qua kiểm.
KeyInfo và extension EC khai TEE/verified/locked là kết quả qua hook giả lập,
không chứng minh hardware hoặc trạng thái boot thực.

Biến thể probe RSA tạo alias thử riêng, kết quả trong
`logs/20261102-m4-ec-keybox-probes/rsa-filtered.txt`: KeyInfo false, chain 3,
sign/verify và challenge PASS, alias đã xóa. Chuỗi RSA có software attestation,
extension khai software/unverified và root không khớp root Google công bố.
Điều này phù hợp fallback về software keystore; không chứng minh mọi thao tác
RSA hay mọi app hoạt động. Cả hai alias probe đã được xóa và APK probe đã gỡ.
`run-as` gặp `Segmentation fault`; adb root đọc certificate probe riêng để
phân tích, không đọc hay xuất khóa riêng. Certificate probe lưu tại log private,
không commit hoặc chia sẻ.

Sau reboot chủ động, Android boot hoàn tất, trạng thái keybox/PIF vẫn bật và
dữ liệu keybox còn hiện diện. Checker mới lúc 13:06 ngày 02/11 hiển thị BASIC
đạt, DEVICE và STRONG không đạt; `reboot-check-filtered.txt:2-3` ghi request
hoàn thành/callback. Chưa kiểm trực tiếp dữ liệu thực bên trong tiến trình GMS;
không suy chuỗi của app probe thành chuỗi gửi Google hoặc kết luận nguyên nhân
duy nhất của verdict. XML người dùng gần hết hạn như ghi ở trên.
