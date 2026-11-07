# M4 — kế hoạch nhận diện A37 và sửa PIF/keystore, 31/10/2026

Trạng thái cập nhật31/10/2026: **r3 đã build, qua image/flashable gates, fresh-flash
và boot tới Android trên G965F; Setup Wizard chưa hoàn tất nên chưa tạo request
Play Integrity mới**. Baseline trước lượt này là M4 attempt0229. Bằng chứng cục bộ:
`logs/20261031-m4-identity-integrity-0232/`. Ảnh JSON thô/nonce và bản decode nằm
trong thư mục log riêng; tài liệu chia sẻ chỉ ghi các verdict đã lược định danh.

Live r3 sau flash: `sys.boot_completed=1`, SDK37, `ro.product.model=SM-A376B`,
`ro.product.name=a37xnaxx`, `ro.product.device=star2lte`,
`ro.boot.em.model=SM-G965F`, build `A376BXXU5BZI6`; Settings device name
`Galaxy A37 5G`. PIF default bật với version `20261011`, security patch
`2026-10-05`, first API level `32`. USB debugging=1, `ro.adb.secure=0`,
Auto Blocker/Rampart secure settings=0. NFC feature declarations/shared
libraries và Bixby agent+wakeup vẫn hiện trong thiết bị. SystemServer PID2216,
SystemUI PID6238 tại sample đầu; boot complete. Một crash Bixby đã biết và được
người dùng chấp nhận: `No matching ComponentResult found!`. Setup Wizard vẫn
đang foreground; chưa thể kiểm checker/Play Store hoặc xác minh Build.MODEL từ
app thường trong trạng thái này.

## 1. Yêu cầu mới nhất và phạm vi

- Người dùng xác nhận Shopee, TikTok và các app đang dùng **vẫn tải/cài/chạy
  bình thường**. Không ghi chúng thành lỗi Play Store đã tái hiện.
- Không dành thời gian port thêm NFC RF hoặc Bixby voice/remap. Giữ khai báo hỗ
  trợ, shared library, service và package cần cho tương thích app. Không xóa
  feature hoặc đổi thành unsupported để đóng bug. Lỗi chức năng chưa dùng được
  ghi riêng, không tự nhận chức năng RF/voice đã PASS.
- Tên hiển thị yêu cầu: Samsung / Galaxy A37 5G; model ứng dụng đọc được:
  `SM-A376B`. Đây là nhận diện công khai của ROM. Phần cứng thực, chọn blob,
  installer, recovery/assert và radio vẫn là `star2lte`/G965F/Exynos9810.
- Tập trung sửa lỗi PIF thực thi và cấu hình keystore đã có bằng chứng. Không
  cam kết một công tắc có thể tạo cả ba nhãn Integrity từ phía Google.
- Người dùng đã yêu cầu triển khai kế hoạch và đã có quyền flash G965F trong
  AGENTS.md. Attempt r3 fresh-format USERDATA, repartition guard, ghi SYSTEM/
  VENDOR/ODM/BOOT và format CACHE/OMR/PRELOAD/DATA; không có lệnh ghi EFS/CPEFS/
  RADIO/RECOVERY/BOTA. Protected-partition SHA256 pre/post khớp hoàn toàn.
  Stage08 chạy từ TWRP `/tmp`, tránh lỗi lượt cũ khi package nằm trong `/sdcard`.

## 2. Kết quả kiểm tra hiện tại

| Hạng mục | Bằng chứng | Kết luận giới hạn |
| --- | --- | --- |
| Model | `ro.product.model=SM-G965F`; system/system_ext cũng G965F | Đúng là đang hiện S9+, không phải cache UI đơn lẻ |
| Tên | `settings global device_name=Galaxy S9+`; `SEC_FLOATING_FEATURE_SETTINGS_CONFIG_BRAND_NAME=Galaxy S9+` | Cần sửa cả model, tên Settings và default tên trong data |
| Nguồn đổi model | `_EXYNOS9810_FINAL_ENSURE_TARGET_IDENTITY`, adapter patch0225 | Code đã chủ động thay A37 thành S9+; yêu cầu mới thay mục tiêu này |
| NFC app contract | `android.hardware.nfc`, `.any`, `.hce`, `.hcef`, `.uicc`, `.ese`; library `com.android.nfc_extras`, `com.samsung.android.nfc.adapter` | Hiện không bị gỡ khai báo hỗ trợ |
| Bixby app contract | APK 4.1.15.4/411504000 vẫn installed/enabled mặc định, Samsung-signed; `COMMON_SUPPORT_BIXBY=TRUE` | Giữ composition này; remap/Side Key là hạng mục riêng |
| PIF toggle | `persist.sys.unica.pif=true`, version `20261011` | Công tắc bật thật, nhưng chưa chứng minh hook áp được |
| PIF runtime | `IllegalAccessException` khi đặt `Build.FINGERPRINT` | Hook hiện bị lỗi thật trên Android17 |
| PIF defaults | Chỉ fingerprint/version được seed; `persist.sys.pif.security_patch` và `.first_api_level` rỗng | Default thiếu hai trường có trong profile cập nhật |
| Updater đã biên dịch | Nếu version local >= remote thì bỏ qua `setPIFProps` | Version bằng nhau có thể khiến dữ liệu thiếu không được sửa |
| Keystore | `remote_provisioning.tee.rkp_only=1`, chỉ có Keymaster HIDL4.0, không có IRPC/default | Cấu hình RKP-only của donor không phù hợp stack hiện đăng ký |
| Checker | v2.2, installer `com.android.vending`; ba dấu X trên màn hình | Không kết luận checker bị sideload hoặc không có license từ nguồn cài |
| JSON thực | app/license/Play Protect `UNEVALUATED`; `deviceRecognitionVerdict` vắng mặt | Không có nhãn PASS; cần giữ JSON gốc khi đánh giá, không chỉ ba biểu tượng |
| ABI | `arm64-v8a`, `zygote64` | Không mở thêm port app32 trong việc sửa này; app hiện dùng đã được người dùng xác nhận |

### Log quyết định

```text
10-31 10:36:06.974 E PlayIntegrityHooks: Failed to set prop FINGERPRINT
10-31 10:36:06.974 E PlayIntegrityHooks: java.lang.IllegalAccessException: Cannot set public static final field java.lang.String android.os.Build.FINGERPRINT of class java.lang.Class<android.os.Build>
10-31 10:36:07.246 E keystore2: ... Trying to get attestation key from RKPD.
10-31 10:36:07.246 E keystore2: ... Failed to get rpc for sec level r#TRUSTED_ENVIRONMENT
10-31 10:36:07.246 E keystore2: ... Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)
10-31 10:36:07.258 E Finsky: Integrity key attestation record generation failed.
10-31 10:36:07.258 E Finsky: ... java.security.ProviderException: Failed to generate key pair.
```

Đã pull tạm rồi kiểm SHA256 với file trên máy và decode trực tiếp:

- `framework.jar`: `f0e0d9533b09f31af04bd76675ad00306eb7a2149e627c6f3a24ec8cd914916a`.
- `SecSettings.apk`: `5a9ef54250c51fada3cd21f54bdc04f57f26bd4c53d4e6d93071d0cd2d3e07db`.
- `PlayIntegrityHooks.setPropValue` gọi `setAccessible(true)` rồi `Field.set`;
  `Build.FINGERPRINT` vẫn là public static final. Đây là mã cài thực tế, không
  chỉ nội dung patch nguồn.
- `PIFUtils.lambda$updatePIF$0` có nhánh version `if-ge` đúng như nguồn.
- `shouldBlockKeyAttestation()` trong bản cài trả true; theo call site, nhánh
  này đi thẳng return và **không** chặn `getCertificateChain`. Không suy tên
  method thành ngữ nghĩa ngược. Lỗi native `generateKey` cần xử lý riêng.
- Hai APK/JAR pull tạm đã xóa sau khi ghi hash và decode, tránh giữ firmware
  trùng. Log riêng giữ mode0600 trong thư mục0700.

## 3. Giữ tương thích NFC/Bixby — giới hạn công việc

Không viết patch port NFC/Bixby mới cho yêu cầu hiện tại. Thêm kiểm hồi quy vào
gate hiện có để các thay đổi identity/PIF không vô tình xóa:

1. NFC feature XML, hai shared library nói trên, service/HAL và package hiện
   cài. Adapter phải tiếp tục trả trạng thái thật; không stub giao dịch thành
   công khi chưa thực hiện được RF.
2. Bixby donor có chữ ký Samsung, `COMMON_SUPPORT_BIXBY=TRUE`, khai báo
   permission/sysconfig và các dependency đang có. Không gỡ package hay tắt
   toàn bộ component để che receiver crash đã được người dùng chấp nhận.
3. Không thêm một feature Android tên Bixby tự đặt; kiểm theo contract Samsung
   thực có. Giữ Side Key/remap đúng quyết định trước, không bật capability mới
   chỉ vì đổi model sang A37.

Google Play dùng feature, thư viện, ABI, SDK và quy tắc phân phối để lọc app;
giữ khai báo là cần cho app có yêu cầu đó. Hiện không có lỗi tải Shopee/TikTok
để sửa. Xem [Google Play filters](https://developer.android.com/google/play/filters).

## 4. Lượt sửa thứ nhất — cấu hình keystore đúng Keymaster cũ

**Lý do:** lỗi tạo key xảy ra trước khâu lấy certificate chain. Chỉ sửa hook
fingerprint không xử lý được `HARDWARE_TYPE_UNAVAILABLE`.

1. Kiểm nhánh đọc `remote_provisioning.tee.rkp_only` trong binary keystore Samsung
   đã khóa hash. AOSP cho phép fallback về factory key khi RKP không có và
   `rkp_only=false`; đây là cơ sở cho phép thử, chưa phải bằng chứng binary
   Samsung đã thực hiện fallback thành công.
2. Thử một biến trên baseline: lưu `rkp_only=1`, tạm đặt0 nếu property được đọc
   runtime, chạy diagnostic app với key alias tạm để kiểm EC/RSA generate,
   sign/verify và attestation challenge, rồi khôi phục property/alias. Không
   xóa database keystore hoặc key của ứng dụng.
3. Nếu hết lỗi RKP-only và legacy Keymaster trả certificate chain hợp lệ theo
   API, đưa cấu hình0 vào finalizer cho đúng M4/G965F/legacy Keymaster. Nếu lỗi
   chuyển sang factory attestation unavailable, ghi nguyên lỗi mới; không tạo
   HAL hay chứng chỉ giả để báo PASS.
4. Không đổi `first_api_level=26` của phần cứng, VNDK33, vendor security patch,
   modem, bootloader hoặc EFS nhằm bắt chước phần cứng A37.

**File dự kiến:**
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`
và gate `rom/scripts/internal/verify_m4_runtime_image.py`; kiểm file
`product/etc/build.prop` thực tế sau fold/restore của fork, không sửa cache
firmware donor `out/fw`.

**Kiểm chứng:** native API tạo/sign key bình thường vẫn hoạt động, lỗi RKP-only
không còn trên đúng phép thử, chain/challenge được phân tích thật, không tuyên
bố root trust đạt Google chỉ vì API hết lỗi. **Rollback:** khôi phục property1
trong thử tạm; đảo patch finalizer và artifact SYSTEM nếu đã đóng gói.

Nguồn cho cơ chế fallback: [AOSP remote_provisioning.rs](https://android.googlesource.com/platform/system/security/+/refs/heads/main/keystore2/src/remote_provisioning.rs).

## 5. Lượt sửa thứ hai — PIF chạy được trên ART Android17

**Lý do:** compiled hook có mặt nhưng runtime từ chối ghi static final field.

1. Port đường ghi field của helper hiện có sang cơ chế phù hợp ART BZI6. Trước
   khi chọn cách sửa framework, dùng fixture trong process thử để kiểm field
   write và readback trên chính thiết bị. Không chọn mẹo chỉ vì assemble DEX
   thành công; không nới reflection/final-field cho toàn bộ framework.
2. Phạm vi override vẫn theo package/process allowlist của fork. App thông
   thường phải tiếp tục nhận public model A37 sau bước7; system_server/HAL
   không nhận profile Google của PIF. Test process ngoài allowlist và chế độ
   PIF OFF là negative controls bắt buộc.
3. Kiểm cả các field String/int mà profile thật dùng, không chỉ FINGERPRINT.
   Nếu không áp được, helper phải ghi lỗi và trạng thái applied=false rõ ràng.
4. Rà nhánh keystore/call-site đã biên dịch để giữ Google account, TLS,
   Cryptauth và key API thông thường. Không phục hồi blanket block mọi chứng
   chỉ GMS/Play Store; finalizer hiện đã có gate cấm đường đó.

**File dự kiến:** `rom/unica/mods/pif/a376b_android17_pif.py`, patch helper
`rom/unica/mods/pif/framework.jar/0001-Introduce-PlayIntegrityHooks.patch`,
`rom/unica/mods/pif/customize.sh` và compiled verifier hiện có. Nếu cần chạm
`Build`/`Build.VERSION`, phải thêm exact method/field preimage và giới hạn
phạm vi; phương pháp cụ thể chỉ chốt sau fixture ART.

**Kiểm chứng:** không còn `IllegalAccessException`; readback trong đúng process
khớp profile; OFF và app ngoài allowlist không đổi; không phát sinh bootloop,
GMS crash hoặc lỗi tạo key thường. **Rollback:** framework.jar baseline đã khóa
hash/ROM attempt0229; không thay kernel/vendor.

## 6. Lượt sửa thứ ba — profile đầy đủ và cập nhật cùng version

**Lý do:** build chỉ seed VERSION/FINGERPRINT. Profile upstream version20261011
còn có SECURITY_PATCH/DEVICE_INITIAL_SDK_INT, nhưng updater bỏ qua khi version
bằng nhau; bấm bật PIF không sửa được thiếu dữ liệu này.

1. Pin payload profile/schema/hash trong nguồn. Seed đủ các field profile thật
   công bố; các field spoof chỉ dùng trong process PIF, không sửa patch level
   hoặc API đầu đời thật của vendor.
2. Updater so nội dung và độ đầy đủ ngoài số ngày version. Cùng version nhưng
   thiếu field phải có đường sửa; không tự ghi đè custom profile của người dùng.
3. Validate toàn payload trước khi ghi các property. Lỗi mạng/schema giữ cấu
   hình cũ; không để một nửa profile mới và một nửa profile cũ. Không có script
   mỗi boot ép lại lựa chọn của người dùng.
4. Settings phân biệt Enabled / profile version / applied-or-error. Dòng
   “đã cập nhật” chỉ nói profile, không đổi thành cam kết Integrity PASS.

**File dự kiến:** seed trong
`rom/unica/patches/exynos9810_device_stack/customize.sh`,
`rom/unica/mods/settings/SecSettings.apk/smali_classes4/io/mesalabs/unica/settings/pif/PIFUtils.smali`,
controllers PIF tại cùng thư mục, `a376b_android17_pif.py`,
`rom/scripts/internal/verify_exynos9810_release_defaults.py`.

**Kiểm chứng:** fresh/default đủ field; same-version incomplete repaired;
same-version complete idempotent; custom/off được giữ; bad schema/network
không làm mất profile cũ. **Rollback:** restore bộ property trước thử và APK
Settings baseline. Không cần wipe Google account/data để bắt đầu.

Upstream đã đọc trong lượt kiểm tra:
[UN1CA pif.json](https://raw.githubusercontent.com/UN1CA/static_resources/refs/heads/sixteen/pif/pif.json).
Nội dung đó chưa chứng minh Google đang chấp nhận profile.

## 7. Lượt sửa thứ tư — tên/model A37 theo yêu cầu

Giữ adapter0225 vì nó xử lý được precedence và có negative gates, nhưng đổi
mục tiêu của adapter cho profile M4 này; không chỉ revert mù sáu property.

| Giá trị | Mục tiêu |
| --- | --- |
| `Build.MODEL` / `ro.product.model` và nguồn model cấp cho app | `SM-A376B` |
| Settings brand/marketing name | `Galaxy A37 5G` (Samsung) |
| Tên mặc định trong `settings global device_name` | `Galaxy A37 5G` |
| Tên người dùng tự đặt | Giữ nguyên; chỉ migrate khi còn đúng default S9+ cũ |
| `TARGET_CODENAME`, chọn blob/kernel/DTB, hardware/board, installer/assert | Giữ target S9+ thực |
| `Build.DEVICE` / catalog brand+device | Giữ `star2lte` trong lượt đầu; apps hiện đã tương thích, không cần đổi catalog để đạt tên/model |
| PIF fingerprint | Chỉ thuộc profile process PIF, tách khỏi tên/model chung |

1. Rà các nơi đọc model/codename trong fork trước khi chọn property writer;
   không thay `STAR2LTE_BUILD_MODEL=SM-G965F` thành A37 vì biến đó chọn phần
   cứng thật. Donor system ghi `device=essi`, còn vendor/ODM donor ghi `a37x`;
   không dùng `essi` như tên phần cứng A37 để thay mù target.
2. Sửa `_EXYNOS9810_FINAL_ENSURE_TARGET_IDENTITY` và
   `exynos9810_finalization/a376b_android17_target_identity.py` để public model
   resolve A37 sau tất cả restore/finalize. Giữ fingerprint OS baseline trong
   phép thử model đầu, tránh đồng thời đổi nhận diện và attestation.
3. Sau restore floating_feature, đặt brand name A37 ở các file thật được dùng;
   nguồn donor BZI6 đã có đúng string. Tên trong userdata cần migration một lần
   theo default cũ, không script mỗi boot đè tên người dùng.
4. Module `rom/unica/mods/device_image/customize.sh` đang pin ảnh SCPM về mã
   G965F. Nếu đổi ảnh About cho đồng bộ A37, dùng model-code donor đã xác minh
   trong request của Settings hoặc bỏ pin S9+ theo scope. Không đổi
   `ril.product_code` thật của radio để lấy ảnh.
5. Cập nhật image/compiled gates đang yêu cầu modelG965F; giữ separate assert
   phần cứng G965F cho flash. Không gỡ gate chỉ để build qua.

**Kiểm chứng:** `getprop`, một app thường đọc `Build.MODEL`, About phone và tên
Bluetooth/thiết bị đều đúng mục tiêu; after-reboot vẫn đúng; tên custom không
bị đè; NFC/Bixby declaration không đổi; SIM/camera/display vẫn chọn S9+.
**Rollback:** SYSTEM/Settings baseline và giá trị device_name đã lưu; không
rollback EFS/radio hoặc xóa data.

## 8. Kiểm sau sửa, thứ tự và tiêu chí dừng

Thử từng biến: **RKP fallback → PIF field writer → profile/updater → public
model/name**. Mỗi lượt lưu patch reason, file list, hash trước/sau, log và
rollback; chỉ ghép package cuối sau khi các lượt riêng có bằng chứng. Mở rộng
fork hiện tại, không tạo build system mới và không build kernel cho lỗi này.

- Sau mỗi lượt, giữ cùng checker/version và đọc JSON đã lược nonce, ghi thật
  các nhãn được server trả. `UNEVALUATED` không được đổi thành PASS trong UI.
- Kiểm Play Store certification và Google sign-in bằng UI, không đọc token hay
  dữ liệu tài khoản. Checker hiện cài từ Play nhưng license vẫn UNEVALUATED;
  chưa có bằng chứng để đổ lỗi sideload hoặc yêu cầu người dùng cài lại.
- Mục tiêu đầu là API/key generation và hook hết lỗi, rồi đo lại BASIC. DEVICE
  và STRONG không được cam kết từ một property: Google yêu cầu tin cậy/chứng
  nhận và bằng chứng phần cứng cho các mức đó. `green`/`flash.locked=1` do fork
  set không là chứng minh boot chain. Xem [Google verdicts](https://developer.android.com/google/play/integrity/verdicts).
- Nếu hook/API sửa xong nhưng server vẫn không nhận, báo nhãn/JSON và nguyên
  nhân còn thiếu. Không dùng keybox bên ngoài hoặc tự tạo kết quả checker để
  hoàn tất gate. Nếu app hiện dùng vẫn chạy, giữ kết quả đó riêng với Integrity.
- Hồi quy Shopee/TikTok/app ngân hàng người dùng đang dùng: vẫn tìm/tải/update
  và mở app; không cần kích hoạt NFC/Bixby để nghiệm thu phạm vi này. Không
  gỡ/cài lại hoặc xóa data app ngân hàng để thử.
- Giữ rule ADB/Auto Blocker, SELinux Enforcing, Bluetooth đã được chấp nhận;
  kiểm core service không restart và hash/phạm vi phần cứng trước mọi lượt ghi.
- Build/replay theo fork, gate compiled và image ext4 thật; package run riêng,
  hash/manifest/ZIP32. Giữ attempt0229 tới khi artifact mới hoàn chỉnh; chỉ giữ
  bản mới nhất mỗi milestone theo retention hiện hành.

## 9. Bản ghi chỉnh tài liệu và trạng thái triển khai

Lý do: yêu cầu đã rõ rằng app hiện tương thích, không port tiếp NFC/Bixby;
nhận diện A37 là mục tiêu và Play Integrity đang không có nhãn PASS.

File nguồn ảnh hưởng: `rom/unica/patches/exynos9810_device_stack/customize.sh`,
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/customize.sh`,
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/adb_defaults.py`,
`rom/unica/patches/exynos9810_device_stack/exynos9810_finalization/a376b_android17_target_identity.py`,
`rom/unica/mods/pif/a376b_android17_pif.py`,
`rom/scripts/internal/verify_exynos9810_boot_contract.sh`,
`rom/scripts/internal/verify_exynos9810_release_defaults.py`,
`rom/scripts/internal/verify_m4_runtime_image.py`.

Lý do: đổi public identity từ S9+ sang A37, sửa Unsafe field writer cho ART17,
seed thêm các trường profile PIF, giữ NFC/Bixby compatibility flags,
đặt metadata SELinux chính xác cho bốn XML NFC trên vendor ext4. Build r1 lỗi
`RuntimeError: Unexpected seed SQL count: global`; r2 thiếu Bixby feature trong
vendor feature file rồi tiếp tục lỗi `M4 image SELinux label mismatch` cho
`android.hardware.nfc.hce.xml`. Gate đã sửa để đối chiếu seed và finalizer thêm
đúng feature/metadata; r3 qua các gate compiled và image thật.

Đính chính sau rà source: `PIFUtils.lambda$updatePIF$0` vẫn có nhánh
`if-ge v1, v0, :cond_0`; việc tự sửa profile thiếu trường khi cùng version chưa
được chứng minh hoặc hoàn tất. Seed r3 có thêm các trường trên data mới, nhưng
không coi seed đó là sửa updater. Xem
[chẩn đoán r3 mới hơn](m4-r3-post-setup-diagnosis.md).

Artifact r3: `rom/out/releases/M4/star2lte-1793426231-65eoO4/`, full flashable
SHA256 `45d34b63ee275b0123fd7fca6c9e868a7a4d12c6bd8069f9c80dcb7356d33daf`,
target-files SHA256 `664dcf2599a17b6f70bc52649f3e1beafbffd86b54452bc39b56478a845ef04a`.
8/8 ZIP32 stages, CRC/signature/manifest và hai checksum list đều đã xác minh.
R3 dùng gói DS-ACK V1.12 G965F WIPEONFORK diagnostic có sẵn theo bằng chứng
`MADV_WIPEONFORK`; không build kernel. Sau khi r3 hoàn chỉnh/hash/manifest pass,
attempt0229 và các run cũ đã được prune theo retention; rollback partition images
và GPT vẫn giữ trong logs.

Flash/live verification: `sha256sum -c SHA256SUMS` và `SHA256SUMS.legacy` đều
PASS; `verify_exynos9810_flashable.sh` PASS (ext4, enforcing, 6 device payloads);
`e2fsck -fn` PASS cho SYSTEM/VENDOR/ODM/USERDATA/CACHE/OMR/HIDDEN. Bảy protected
partition hashes trùng trước/sau. Live getprop/Settings xác nhận A37 public
identity, phần cứng G965F/star2lte, PIF defaults, ADB keyless, Rampart toggles
off; NFC features/libraries và Bixby packages còn. Chưa chạy checker vì Setup
Wizard chưa xong; chưa có bằng chứng runtime mới cho PIF Unsafe hook hoặc Google
server Integrity verdict. `HARDWARE_TYPE_UNAVAILABLE`/RKP-only vẫn là lỗi mở:
property hiện `remote_provisioning.tee.rkp_only=1`, stack đăng ký Keymaster
HIDL4.0 nhưng không có IRPC. Không tuyên bố Play Integrity PASS.

Bước tiếp: sau khi người dùng hoàn tất Setup Wizard (và tùy chọn đăng nhập
Google), thu log mới, thử PIF hook/profile bằng checker, kiểm app đọc
`Build.MODEL`, rà keystore/RKP bằng request key test không phá dữ liệu. Không
gỡ NFC/Bixby flags, không thay kernel/vendor/radio; nếu cần sửa keystore, thử
riêng property fallback trước khi build patch cố định. Rollback images, GPT,
protected hashes và scripts của attempt0229 vẫn còn trong
`logs/20261030-m4-g965f-test-0229/`; EFS chưa được dump hay ghi.
