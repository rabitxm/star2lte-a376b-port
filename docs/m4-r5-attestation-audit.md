# M4 r5 — audit app Key Attestation và Keymaster, 01/11/2026

## Kết luận có bằng chứng

G965F `M2_G965F_SERIAL` đang chạy M4 r5 sử dụng **software Keymaster 4**
trong Hybrid Vendor. Hai EC key thử độc lập tạo qua AndroidKeyStore ở app UID
có public key khác nhau; mỗi key ký/kiểm chữ ký và trả đúng challenge ngẫu
nhiên. Probe gọi `KeyInfo.isInsideSecureHardware()` trên key thứ hai và nhận
`false`. Hai extension attestation đều khai `attestationSecurityLevel=software`
và `keymasterSecurityLevel=software`. Đây là phát hiện bảo mật thực của stack
đang chạy, không chỉ là vấn đề chữ trong About phone.

Chuỗi app probe có ba certificate. Hai chữ ký giữa các certificate hợp lệ,
nhưng certificate trung gian hết hạn **2026-01-08 00:46:09 UTC**; root tự ký
không khớp public key của hai Google Hardware Attestation Root được công bố
trong danh sách chính thức, và certificate cuối không được các root đó ký trực
tiếp. OpenSSL với đầy đủ chain và root Google trả `error 19 at 2 depth lookup:
self-signed certificate in certificate chain`. Khi tự tin cậy root của chain
để kiểm riêng hạn, OpenSSL trả `error 10 at 1 depth lookup: certificate has
expired`. Kiểm riêng chữ ký bằng root chain với `-no_check_time` PASS, nhưng
điều đó không biến root này thành Google Hardware Attestation Root.

Tải danh sách root và danh sách trạng thái công khai từ Google về PC, chỉ đối
chiếu local; không gửi serial certificate hay challenge cho Google. Ba serial
không nằm trong status list tại lúc kiểm. Kết quả này không chứng nhận chain
đáng tin, và không loại trừ các lý do từ chối khác. Nguồn định nghĩa và root:
[Google Key Attestation](https://developer.android.com/privacy-and-security/security-key-attestation),
[danh sách root](https://android.googleapis.com/attestation/root),
[status list](https://android.googleapis.com/attestation/status).

Extension probe cho biết `deviceLocked=false`, `verifiedBootState=unverified`
ở **softwareEnforced**, không có hardwareEnforced root-of-trust. Những trường
này là báo cáo software của probe; không suy chúng thành phép đo độc lập về
fuse/hardware. Live `ro.boot.verifiedbootstate=green` và
`ro.boot.flash.locked=1` chỉ là property, không phủ định nội dung extension.
OS patch software báo `202609`, vendor patch `20260505`, boot patch `20260901`;
đây là dữ liệu của probe, không kết luận dữ liệu GMS gửi Google.

Chain này thuộc **app probe**, không phải chứng chỉ được lấy từ GMS/Play Store.
Mọi verdict Play Integrity là quyết định phía Google: hai lượt ON/OFF vẫn rỗng,
nhưng chưa có cơ sở khẳng định chính xác điều kiện nào là nguyên nhân duy nhất
trong các request đó. Theo [Play Integrity](https://developer.android.com/google/play/integrity/verdicts),
Android13+ BASIC yêu cầu Google root of trust và DEVICE yêu cầu bằng chứng
phần cứng về bootloader khóa và image được hãng chứng nhận. Software chain này
không đáp ứng kiểm tra Google Hardware Attestation Root của probe; không hứa
rằng khôi phục một HAL sẽ tự làm BASIC/DEVICE/STRONG PASS.

## Phạm vi stack đã đối chiếu

`lshal` chỉ liệt kê `android.hardware.keymaster@4.0::IKeymasterDevice/default`;
process `android.hardware.keymaster@4.0-service` chạy và nạp
`/vendor/lib64/libpuresoftkeymasterdevice.so`. SHA-256 live của service và thư
viện khớp từng file payload trong
`rom/unica/patches/exynos9810_device_stack/keymaster4/vendor/`.

Đọc fork cho thấy stage đầu có chuẩn bị KM3, nhưng
`exynos9810_boot_restore/customize.sh` gọi
`_EXYNOS9810_APPLY_FINAL_SECURITY_STACK` sau restore và xóa service, rc, impl,
và khai báo KM3. Comment trong chính finalizer nói service TEE bị lỗi phiên sau
khi bỏ VaultKeeper và từng làm keystore2/vold chờ. Log cũ có dòng
`10-24 04:18:50.625 ... Trustonic TEE: client_get_session: session 0 not found`;
chưa liên kết độc lập dòng đó với một request Keymaster cụ thể. Không khôi phục
KM3/VaultKeeper bằng cách chỉ thêm lại manifest hoặc vài blob vì chưa kiểm đủ
dependency, secure-world ABI, tình trạng key đang có và khả năng boot.

Đã bắt đầu kiểm kê theo đúng firmware F: service KM3 và rc trong embedded
baseline khớp SHA-256 với cache stock `SM-G965F_PHN`; `mcDriverDaemon` cũng
khớp. Hai thư viện KM3 64-bit `libkeymaster3device.so`,
`libskeymaster3device.so` và `android.hardware.keymaster@3.0-impl.so` embedded
có hash khác stock F, nhưng so sánh từng byte cho thấy **toàn bộ nội dung file
stock là prefix giống hệt của file embedded**, phần duy nhất thêm vào là 23 byte
`duhansysl@DuhanROM-V4.3`. Các tập symbol động import/export và `DT_NEEDED`
cũng trùng; hai thư viện đầu đang cài live khớp hash embedded. So sánh này
chưa kiểm chứng được việc service hoạt động với Trustonic/SELinux hiện hành.
`vendor/bin/vaultkeeperd` có trong cache stock F, không có
ở đường embedded vendor mà bước restore shim đầu tham chiếu và không chạy trên
máy; finalizer cũng chủ động gỡ nó khỏi vendor cuối.
Stock F khai báo VaultKeeper HIDL **1.0** trong `vaultkeeper_common.rc`; binary
service 1.0 có mặt còn service 2.0 không có trong cache stock. Điều này khác
danh sách shim 2.0 ở stage của fork. `vaultkeeperd` stock phụ thuộc
`libvkservice.so` và `libdsms_vendor.so`; `libvkservice.so` phụ thuộc
`libMcClient.so`. Chỉ kiểm `DT_NEEDED` tĩnh, chưa chứng minh transitive symbols,
namespace/linker, TA, SELinux hoặc service khởi động được trên Hybrid Vendor.
Chi tiết hash, chênh lệch byte, symbol và `DT_NEEDED` nằm trong các file
`km3-*-inventory.txt` / `km3-*-differences.txt` / `km3-elf-comparison.txt`
của thư mục bằng chứng; VaultKeeper có bảng riêng
`vaultkeeper-f-stock-inventory.txt`. Đây là điểm cần rà trước một cấu hình TEE,
không phải chỉ dẫn copy trực tiếp blob stock vào vendor lai.

Log keystore của probe có `r#TRUSTED_ENVIRONMENT` ở dòng generate request dù
`KeyInfo` và extension nói software. Tên mức trong log request không chứng minh
key đã được hardware bảo vệ. `remote_provisioning.tee.rkp_only=0` cho phép đường
tạo key hoàn tất sau warning RPC; nó không biến software Keymaster thành TEE.
Điều này sửa cách hiểu quá rộng của các tài liệu r3/r4 dùng cụm “legacy factory
fallback”: chúng mới chứng minh API tạo chain thành công, chưa chứng minh
factory hardware attestation.

## PIF scope đã rà

SHA-256 `/system/framework/framework.jar` live khớp file `work_dir` của r5.
Đã disassemble chính class đã biên dịch và đối chiếu `Instrumentation`,
`PlayIntegrityHooks` và `AndroidKeyStoreSpi`:

- `setProps` kiểm công tắc; `com.android.vending` gọi `spoofBuildGms`, còn
  `com.google.android.gms` đi qua `shouldTryToCertifyDevice`, kiểm tên process
  chứa `unstable` và kiểm màn hình thêm tài khoản.
- Helper có setter cho các field profile; fingerprint/security patch/first API
  có giá trị live. Các field profile khác không được đặt; không kết luận đó là
  lỗi chỉ từ việc giá trị rỗng.
- `dlog` trong class đã biên dịch chỉ `return-void`. Do đó không thể lấy readback
  GMS từ log debug hiện tại. GMS không khai báo debuggable; chưa inject agent,
  chưa đổi framework để log và chưa khẳng định mọi field runtime của GMS.
  Lượt trước đã kiểm ON/OFF thực trong Play Store và thấy GMS unstable tồn tại.
- `shouldBlockKeyAttestation` compiled trả true; call site tương ứng return
  trước nhánh block, phù hợp cấu hình không có keybox.

Không có căn cứ từ audit này để đổi profile ngẫu nhiên hoặc nới allowlist.
Nếu cần xác minh field thực trong GMS, dùng diagnostic patch giới hạn package,
process và thời điểm, kiểm exact preimage/compiled gate, test ON/OFF và app
thường, giữ rollback; không ghi nonce, tài khoản hoặc chứng chỉ lên log công khai.
Đó là phép thử chưa thực hiện trong lượt này.

## Các bước TEE tiếp theo trước khi sửa

1. Kiểm kho blob G965F và các log chạy KM3 cũ theo hash, dependency ELF,
   manifest/init/SELinux, cấu hình `mcDriverDaemon`, Trustonic TA, VaultKeeper
   và ABI của Hybrid Vendor. Cần chỉ ra chuỗi lỗi và thành phần đã thiếu trên
   đúng build; không suy một dòng TEE chung là chứng minh root cause.
2. Thiết kế một lượt thử KM3/TEE có rollback đầy đủ và phạm vi phân vùng rõ;
   xác minh tương tác keys đang có và FBE trước khi ghi hoặc boot. Test boot,
   sign/verify, `KeyInfo` và challenge từ app UID. Chỉ giữ thay đổi nếu log
   chứng minh service TEE thực và không phá các luồng đang hoạt động.
3. Kiểm lại chain bằng root/status chính thức từ máy khác và chạy lại checker
   cùng Play Store. Tiếp tục ghi nhận bootloader/certification là các điều kiện
   độc lập; không sửa bootloader, EFS, modem hay thay fake certificate/keybox.

## Kiểm thử, dọn dẹp và file thay đổi

Probe dựa trên `KeyStoreProbe.smali` cũ, được sao chép vào thư mục log riêng;
hai challenge mới ngẫu nhiên, hai alias có kiểm tra collision và đã xóa. Ứng
dụng thử được cài tạm, certificate công khai được kéo từ thư mục riêng của app,
rồi đã uninstall. Không thay các key/tài khoản/app khác. `run-as` hiện lỗi
`Segmentation fault`, exit139; shell ADB root đọc được file probe nên phép thử
tiếp tục. Đây là lỗi công cụ riêng chưa chẩn đoán, không dùng làm kết luận về
Keymaster. PIF vẫn ON, boot complete, SystemServer2209/SystemUI6252 giữ nguyên.

Raw certificate, challenge, screenshots, apk và log nằm trong
`logs/20261101-m4-r5-attestation-probe/` mode0700/0600; không đưa raw lên Git.
Kiểm chứng độc lập: Python `cryptography` xác minh từng chữ ký và parse DER;
OpenSSL xác nhận chữ ký, lỗi hết hạn và lỗi anchor. Hai key khác nhau trả cùng
intermediate/root. File tài liệu sửa trong fork:
`rom/unica/patches/exynos9810_device_stack/keymaster4/README.md`, vì mô tả cũ
nói KM3 còn trong bản cuối trái với finalizer và runtime. Kiểm bằng source
finalizer, SHA-256 service/library live và KeyInfo. Không có patch runtime, build
ROM hay flash trong lượt audit.
