# M4 — kế hoạch Play Integrity từ upstream và báo cáo cộng đồng

> **Trạng thái cập nhật 01/11/2026:** phần adapter BZI6 đã được build riêng
nhưng chưa chia stage/flash/boot-test; không có keybox được cung cấp và không
có verdict server. Không triển khai hook dùng để giả lập hardware attestation.
Xem [báo cáo kết quả và giới hạn](m4-integrity-implementation-status-20261101.md).
Các bước PI-0…PI-4 bên dưới là đề xuất cũ; chúng không được coi là đã chạy.

Ngày kiểm nguồn ban đầu: 01/11/2026. Các phần dưới đây lưu lại nguồn tham khảo
và đề xuất được viết trước lượt port/build; trạng thái thực tế sau đó nằm ở ghi
chú cập nhật đầu tài liệu. Phạm vi dự án vẫn dùng fork, Hybrid Vendor VNDK33,
kernel hiện hành, public SM-A376B và phần cứng G965F.

## Quyết định kỹ thuật đề xuất

Ưu tiên port lại **KeyboxImitationHooks đã có trong fork** cho BZI6/SDK37,
kiểm chứng tương tác với PIF và phục hồi trang nhập keybox. Không coi việc
khôi phục Trustonic KM3 là điều kiện tiên quyết để thử hướng này. Đây là giả
lập attestation; không biến software Keymaster hiện tại thành phần cứng TEE.

Cơ sở: upstream UN1CA mô tả PIF và keybox là hai cơ chế riêng; fingerprint
đơn thuần không đủ cho kiểm DEVICE Android13+ hiện tại. TrickyStore cũng có
chế độ certificate generation cho thiết bị TEE lỗi. [UN1CA #628](https://github.com/salvogiangri/UN1CA/discussions/628),
[TrickyStore](https://github.com/5ec1cff/TrickyStore).

Đường module được giữ làm đối chứng/fallback, không chạy chồng nhiều engine.
Native KM3/TEE là nhánh riêng nhằm khôi phục bảo vệ key thực, vẫn chịu quy tắc
không ghi EFS/CPEFS và chưa có phép thử runtime đủ an toàn.

## Những gì đã xác minh trong dự án

- Máy M4 hiện boot complete, SDK37, PIF ON, kernel
  `4.9.337-DS-ACK-V1.12-G965F-08.05.2026`. Không thấy các thư mục manager
  Magisk/KernelSU/APatch/module ở các đường chuẩn đã kiểm; ADB root không đồng
  nghĩa có môi trường chạy Magisk module.
- Audit r5: hai EC key trả software attestation, `KeyInfo` báo không ở secure
  hardware; chain trung gian hết hạn. PIF ON/OFF đổi fingerprint Store được,
  nhưng checker và Store vẫn trả rỗng. Không suy chain app probe là chain GMS.
- `rom/scripts/internal/apply_modules.sh` chủ động bỏ `mods/keybox` cho M4;
  `rom/unica/mods/settings/a376b_android17_boot_limits.py` ẩn ba mục keybox,
  seed `persist.sys.unica.keybox=false` và gate hiện yêu cầu hook vắng mặt.
- Log r5 `logs/20261031-m4-r5-repair/build-r5.txt:1199`:

```text
BZI6 Keystore2 interfaces differ from framework Keybox hooks; retaining donor key-attestation behavior for M4
```

- Mod đã có tại `rom/unica/mods/keybox/`, gồm hooks proxy Keystore2, generator,
  database, crypto operations và patch MGF. Không cần viết build system mới.
- Preflight trên COPY tạm của các class hiện có: hai proxy
  `IKeystoreService$Stub$Proxy` và `IKeystoreSecurityLevel$Stub$Proxy` thực ở
  `smali_classes4`, patch cũ trỏ DEX3; `KeymasterUtils` thực ở DEX2, patch MGF
  trỏ DEX3. Đây là đổi vị trí DEX bên trong framework.jar, không phải mất class.
- Sau remap đường dẫn trên copy, patch0001 còn `5 out of 24 hunks FAILED` trong
  proxy service; patch0002 dry-run exit0. Các hunk lỗi liên quan nhãn nhảy và
  điểm chèn của `listEntriesBatched`/`updateSubcomponent`; cần port theo method
  và rà mọi hunk khác, không coi hunks áp được là code đã đúng. Log riêng:
  `logs/20261101-m4-integrity-research/keybox-preflight.txt`.
- Upstream từng sửa sai union tag `ORIGIN` ở commit
  `a4c5b07e8f6114dc6dd36665b39d2df37a92e59e`; patch fork hiện đã gọi
  `KeyParameterValue.origin(I)` ở phần tag ORIGIN. Không áp lại bản sửa cũ một
  cách máy móc. [Commit](https://github.com/salvogiangri/UN1CA/commit/a4c5b07e8f6114dc6dd36665b39d2df37a92e59e).

## Chứng cứ từ cộng đồng và mức liên quan

| Nguồn trực tiếp | Nội dung đã đọc | Giới hạn khi áp cho M4 |
| --- | --- | --- |
| [UN1CA #628](https://github.com/salvogiangri/UN1CA/discussions/628), 15/01/2026 | Maintainer hướng dẫn PIF tích hợp + nhập keybox; nêu yêu cầu mới Android13+ | Đúng họ build system nhưng không là xác nhận riêng S9+/Android17; CRL sạch cũng không bảo đảm verdict |
| [KeyAttestation #49](https://github.com/vvb2060/KeyAttestation/issues/49), 29/06–03/07/2025 | Người mở issue báo TB Checker đạt ba nhãn dù app Key Attestation lỗi; bổ sung target làm hết exception, sau đó thấy cert revoked | Chứng cứ lịch sử và tự báo; không phải bảo đảm cấu hình đó còn hoạt động năm2026 |
| [TEESimulator #300](https://github.com/JingMatrix/TEESimulator/issues/300), 28/09–17/10/2026 | Pixel7a Android17, v4.0-67, báo STRONG/Certified với keybox còn hiệu lực; Wallet vẫn có lỗi; các bình luận sau ghi keybox hết hiệu lực rồi thử bản khác | Có cùng SDK, khác SoC/kernel và HAL; chứng minh nhãn Integrity không đồng nghĩa mọi luồng ngân hàng/NFC hoạt động |
| [TEESimulator #286](https://github.com/JingMatrix/TEESimulator/issues/286), 02/09/2026 | Người dùng S25Ultra báo dùng được các app ngân hàng đã thử; đồng thời báo chi phí vòng quét process khoảng1,4% một core | Có Samsung nhưng không có verdict JSON hoặc cùng vendor S9; phải test idle CPU/battery |
| [Báo cáo Exynos S9+](https://www.reddit.com/r/Magisk/comments/1r3cufa/can_i_get_a_bit_of_help_here/) | Comment tự báo dùng TrickyStore trên S9+ và khuyên tắt PIF tích hợp khi dùng module ngoài | Tài khoản comment đã xóa, thiếu version/log, có người làm theo vẫn fail: bằng chứng yếu, chỉ dùng gợi ý đối chứng |

Không tìm thấy báo cáo công khai đã xác minh đủ **G965F + donor BZI6 Android17 +
Hybrid VNDK33 + kernel4.9**. Các thread XDA PIF4607985 và TrickyStore4683446
đã thử mở nhưng công cụ trả502/không truy cập được. Không nhận nội dung XDA
chưa đọc làm chứng cứ thành công. Liên kết để đối chiếu thủ công:
[PIF](https://xdaforums.com/t/module-play-integrity-fix-safetynet-fix.4607985/),
[TrickyStore](https://xdaforums.com/t/tricky-store-bootloader-keybox-spoofing.4683446/).

## Thứ tự thực hiện và điều kiện qua từng bước

### PI-0 — khóa baseline và đầu vào

Lưu hash framework/Settings/BOOT/vendor và trạng thái PIF; giữ ZIP r5 cùng
rollback. Ghi version GMS/Store/checker, nguồn cài checker, trạng thái license,
ngày giờ và verdict gốc. Dùng lại protocol thiết bị đã có; không flash bước này.

Kiểm phần keybox bằng parser ngoại tuyến: XML, EC/RSA private key khớp leaf,
chữ ký từng chain, thời hạn, root và CRL chính thức. Chỉ xuất kết quả kiểm,
không in key/XML/serial vào log công khai hoặc commit. Không tự tải keybox leak,
không dùng keybox test/AOSP làm bằng chứng Google sẽ chấp nhận. Nếu chưa có
keybox phần cứng còn hiệu lực được phép dùng, vẫn làm được port và fixture
crypto; phép thử server tương ứng phải ghi **thiếu đầu vào**, không giả PASS.

### PI-1 — port module keybox trong fork

Port theo descriptor/method chính xác của BZI6, khóa preimage và code đã biên
dịch. Rà `getSecurityLevel`, `generateKey`, `importKey`, `createOperation`,
`getKeyEntry`, `deleteKey`, `listEntries`, `listEntriesBatched`, `grant`,
`ungrant`, `updateSubcomponent` cùng Parcel/union tag. Giữ đường proxy donor
cho request không được hook; không thay transaction IDs bằng giá trị của S22.

File tác động dự kiến, đều nằm trong fork đã đọc:

- `rom/unica/mods/keybox/smali/system/framework/framework.jar/0001-Introduce-KeyboxImitationHooks.patch`
  và `0002-Disable-MGF-digest-param-omit.patch`: chuyển vị trí DEX và port điểm
  móc theo method; nếu cần adapter riêng thì đặt dưới mod hiện có.
- `rom/scripts/internal/apply_modules.sh`: thay nhánh bỏ qua bằng dispatch có
  kiểm scope chỉ sau khi adapter qua kiểm tra.
- `rom/unica/mods/settings/a376b_android17_boot_limits.py`: tách keybox khỏi
  danh sách omitted và gate cấm hook; giữ các tính năng khác vẫn omitted.
- `rom/unica/mods/pif/a376b_android17_pif.py`: bỏ giả định no-keybox khi có
  engine đã kiểm, xác minh tương tác attestation/PIF theo hành vi call-site.

Qua bước khi patch replay, rebuild DEX, descriptor/method reference, nhãn nhảy,
return type, try/catch và `ORIGIN` union đều đúng. Không cho phép bỏ qua gate
để đóng ZIP. Kiểm cả framework.jar cuối, không chỉ cây smali trung gian.

### PI-2 — UI, cấu hình và fixture crypto cục bộ

Khôi phục ba mục nhập/xóa/bật keybox trong trang Spoof Settings. Mặc định
engine không có keybox phải là no-op; bật công tắc không đồng nghĩa hợp lệ hoặc
pass. Kiểm trạng thái thiếu XML, XML lỗi, khóa lệch cert, chain hết hạn, revoked,
chưa kiểm mạng và hợp lệ cục bộ; thông báo phải phản ánh đúng từng trạng thái.
Không lưu private key theo cách cho app thường đọc được; audit storage quyền
đọc và đường xuất log trước dùng keybox thật.

Dùng chain tự tạo **chỉ cho fixture ngoại tuyến**, EC và RSA: tạo key, verify
challenge/SPKI/signature, sign/verify sau đọc lại, alias/list/delete,
certificate update/grant khi có hỗ trợ. Kiểm phần mềm giả lập và app control
không hook riêng biệt; chứng minh chain thay thế không có nghĩa key vào TEE.
Kiểm cách giới hạn GMS/Store/probe trước khi bật cho app khác; nếu hooks hiện
áp toàn cục phải rà phạm vi hoặc thêm allowlist có kiểm chứng.

Qua bước khi bật/tắt/không-keybox đều đúng, key đã có không bị phá, Settings
không crash và app ngoài phạm vi giữ hành vi donor. Chưa cần verdict Google.

### PI-3 — thử server trên G965F với một engine

Dùng keybox đã qua PI-0 và có quyền dùng. Chạy ít nhất hai công cụ độc lập:
checker cài từ Play và test tích hợp Play Store. Ghi `appRecognitionVerdict`,
`appLicensingVerdict`, các label, thời điểm, version và `deviceAttributes`
nếu công cụ trả; không ghi token/nonce/account. License của checker có thể
ảnh hưởng việc BASIC/STRONG được trả. [Định nghĩa Google](https://developer.android.com/google/play/integrity/verdicts).

Thử lần lượt: engine OFF/PIF OFF → engine ON/PIF OFF → engine ON/PIF ON.
Không đổi fingerprint, patch date và engine trong cùng một bước. Phải xác
minh profile đang tác dụng trong GMS unstable, không chỉ Play Store. Không
hạ SDK Store để tạo kết quả legacy rồi nhận là DEVICE của SDK37. Không sửa
ngày patch toàn máy hoặc bịa kernel version nhằm làm đẹp checker.

Mục tiêu đầu: BASIC và DEVICE của phản hồi mới; STRONG ghi riêng nếu thực sự
được trả. Nếu chỉ local crypto qua nhưng server rỗng, kiểm chain thực trên
đường GMS, license/scope/cache/version và keybox; không tự kết luận cần kernel.
Không xóa data GMS/Store theo vòng lặp; chỉ reset cache/data có mục đích khi
đã ghi baseline và hiểu tác động account/key.

Qua bước khi kết quả lặp lại được sau reboot và hai phép thử độc lập, không
thêm keystore/SystemServer crash; smoke test app đang dùng, PIN/vân tay,
Wi-Fi/Bluetooth và ADB/Auto Blocker theo rule. Rollback framework/Settings
nếu có regression; không đổi EFS/CP/bootloader.

### PI-4 — fallback nếu hook framework không hoạt động đúng

Thử **một** engine FOSS có version/source/hash khóa: TrickyStoreOSS generation
mode hoặc TEESimulator generation mode. Đây là phép đối chứng mới; r5 hiện
không có root manager/module framework ở các đường chuẩn đã kiểm. Muốn dùng
ZIP module cần chuẩn bị tích hợp init/SELinux được audit hoặc một cấu hình
Magisk thử riêng; ADB root không tự cung cấp các cơ chế này. Không tự thay
kernel sang KernelSU và không nhúng nguyên ZIP module vào ROM rồi coi đã port.

Nếu dùng engine ngoài, tắt engine native; dùng PIF tích hợp hay PIF module
ngoài phải thử riêng, không cho cả hai ghi field cùng lúc. TrickyStoreOSS có
mode `!` cho certificate generation; TEESimulator v4 dùng profile riêng,
không chép cấu hình3.x. [TrickyStoreOSS README](https://github.com/beakthoven/TrickyStoreOSS),
[TEESimulator README](https://github.com/JingMatrix/TEESimulator).

Version ổn định thấy khi tra cứu: TrickyStoreOSS3.1.0 (25/08/2026),
TEESimulator4.0 (11/08/2026), PlayIntegrityFork18 (29/08/2026). Không coi
latest stable là mặc nhiên đúng SDK37: TrickyStoreOSS #107 đã có fix mới hơn
cho `libcore_bundle.so` ở PR109, merge
`cfb618f73c3494860a7e5d86546b8e5dc86da4cb` ngày12/10. Phải kiểm layout thư
viện thực và chọn commit tương ứng trước build/test. [Issue107](https://github.com/beakthoven/TrickyStoreOSS/issues/107),
[releaseOSS](https://github.com/beakthoven/TrickyStoreOSS/releases/tag/v3.1.0),
[releaseTEE](https://github.com/JingMatrix/TEESimulator/releases/tag/v4.0).

## Các hướng không ưu tiên

- Chỉ đổi fingerprint/model hoặc đổi tên kernel: chưa giải quyết chain hiện
  tại; model công khai SM-A376B đã đúng và không phải chứng nhận Google.
- `spoofVendingSdk=32`: upstream PIFork cảnh báo làm hỏng một phần Store,
  download/login/app variants; chỉ có giá trị đối chứng legacy, không đáp ứng
  mục tiêu SDK37 và tương thích Store lâu dài của dự án. [PIFork](https://github.com/osm0sis/PlayIntegrityFork).
- BootloaderSpoofer cho local-only: không thay thế attestation kiểm trên server.
- Thay policy root/SELinux permissive, đổi kernel hoặc khôi phục EFS để thử
  vận may: không có log yêu cầu; không thuộc kế hoạch này.
- Chain local hợp lệ hoặc CRL sạch không bảo đảm Google cấp verdict. Issue
  TEESimulator293 có Android17 mất kết quả sau khi dùng intermediate hết hạn;
  tương đồng loại lỗi của app probe r5, chưa chứng minh cùng certificate hoặc
  cùng nguyên nhân GMS. [Issue293](https://github.com/JingMatrix/TEESimulator/issues/293).

Kết quả dự kiến của giai đoạn port là engine hoạt động đúng, có import/status
và rollback; nghiệm thu Integrity chỉ dựa trên phản hồi server tại thời điểm
thử. Không biến kết quả giả lập thành tuyên bố TEE thật đã được sửa.
