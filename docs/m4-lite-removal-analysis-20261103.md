# M4 Lite ZIP32 — danh sách lược bỏ đề xuất

03/11/2026. Người dùng chọn ROM Lite một ZIP32 và yêu cầu phân tích trước.
Chỉ đọc source/cây build, giải mã manifest APK vào scratch tự dọn và đo nén;
chưa sửa danh sách build, APK, release hoặc điện thoại. Đây là danh sách ứng
viên, chưa là kết luận runtime an toàn.

## Nền và ngân sách

Nền: M4 `star2lte-1793584471-sBoBYd`, full ZIP 5,204,219,280 byte/4.847GiB.
Chi tiết giới hạn recovery ở `m4-installation-options-20261103.md`.
Cần giảm ít nhất867.13MiB nén để vừa ZIP32; mục tiêu <=3.8GiB cần giảm
khoảng1,071.93MiB. Không buộc xuống2GiB bằng cách bỏ chức năng lõi.
Vùng2–4GiB cần được thử bằng ZIP32 có updater vô hại trước khi cài ROM thật;
helper8stage hiện chỉ chứng minh đường cài <2GiB. Updater target là static
AArch64, nhưng điều đó chưa chứng minh toàn đường recovery xử lý gói lớn.

Cột q6 dưới đây là tổng nén RIÊNG từng file bằng Brotli6, cùng quality của
packager. Đây là chỉ báo ưu tiên, không bằng mức giảm của image/stream OTA.
Mọi dự báo ZIP chỉ được chốt sau rebuild ảnh và package.

## Những gì đã bị bỏ, không tính giảm thêm

Target/general/final debloat đã loại Gmail, Maps, YouTube, Samsung Notes,
Health/Members, phần lớn Link to Windows/SmartThings/AR Emoji, Samsung
Messages, Samsung Internet/Theme Store trong scope đã cấu hình. Cây final
đang đọc là nguồn kiểm dung lượng, không lấy danh sách donor hoặc danh sách
trên mạng để cộng dung lượng app không còn tồn tại.

Tên cũ `DuoStub` và `Chrome` không bắt `product/app/Duo` và `Chrome64` của
A37. Một số feature app như PhotoRemaster được finalizer khôi phục có chủ
đích sau generic debloat; chỉ thêm vào debloat.sh đầu build có thể không đủ.

## Nhóm ứng viên đầu tiên

Đường trong bảng tương đối với logical SYSTEM root ở
`rom/out/target/star2lte/work_dir/system`. Khi triển khai phải dùng helper
metadata của fork, không xóa trực tiếp cây build để thay recipe.

| Đường | Package đã đọc manifest / chức năng | Raw MiB | q6 MiB | Cách xử lý |
| --- | --- | ---: | ---: | --- |
| product/app/Duo | com.google.android.apps.tachyon, Meet/Duo |117.14|46.63|Bỏ preinstall, kiểm intent gọi video/launcher; người dùng có thể tự cài lại bản được store cung cấp |
| product/priv-app/Velvet | com.google.android.googlequicksearchbox |306.70|138.28|Bỏ Google app/Assistant/Lens trong gói; kiểm default assistant, voice search, widget và launcher seed |
| system/priv-app/PhotoRemasterService | com.samsung.android.photoremasterservice |76.32|54.75|Bỏ remaster; xử lý caller/menu Gallery, chặn final restore riêng profile Lite |
| system/app/VideoEditorLite_Dream_N | com.sec.android.app.vepreload |49.64|21.29|Bỏ editor video nâng cao; kiểm Gallery edit intent, không xóa codec/media framework |
| system/priv-app/LiveEffectService | Hiệu ứng ảnh động, tên thư mục mới chỉ kiểm dung lượng |49.44|28.49|Audit manifest/caller trước, bỏ cùng phần hiệu ứng liên quan nếu không làm lỗi Gallery |
| system/priv-app/GalleryWidget | Widget Gallery, chưa audit manifest riêng |13.06|5.08|Ứng viên nhỏ; xử lý picker/restore widget |
| system/priv-app/DynamicLockscreen | com.samsung.android.dynamiclock |20.01|6.89|Bỏ dịch vụ hình nền động tải thêm, khác Infinity; kiểm lockscreen/plugin và UI |
| system/priv-app/DigitalWellbeing | Sức khỏe số, chưa audit manifest riêng |23.59|8.23|Tùy chọn; chỉ bỏ sau audit Settings/usage integration |
| system/hidden/INTERNAL_SDCARD/Music | Nhạc mẫu Over the Horizon |20.59|19.60|Bỏ bản nhạc mẫu, giữ nhạc chuông/báo thức và sound picker |

Tổng chỉ báo nhóm trên329.24MiB nén. Bỏ nhóm này mà giữ nguyên10video nền
chưa đủ làm full ZIP32. Nhóm Gallery đòi kiểm caller/UI; đây không phải
cam kết xóa APK riêng lẻ là an toàn.

## Hình nền: đòn bẩy lớn nhất

`system/priv-app/wallpaper-res/wallpaper-res.apk` 889.28MiB, q6 khoảng864MiB;
10video chiếm881.35MiB. Default hiện là S9 Infinity Blue video001 khoảng
100.20MiB. Đề xuất Lite giữ engine SpriteWallpaper, AODService, policy và
cảm biến đi kèm; catalog chỉ giữ Blue cùng asset/thumbnail thực dùng. Bỏ
9video còn lại và ảnh preview chỉ thuộc các mục đó. Không xóa nguyên APK,
engine hoặc AOD. Mức giảm nén dự kiến khoảng760–780MiB, chưa đo bằng image.

Cần cập nhật catalog/index/default, ký/align APK, manifest checksum theo
profile Lite và gate `source/verify_built_wallpapers.py`: hiện gate yêu cầu
đúng5S9+5UN1CA và hash payload full. Giữ gate10mục cho bản Full; Lite có
contract1mục riêng, vẫn kiểm asset/default/service/thumbnail/hash, không
bỏ gate toàn bộ. Không cần đổi fps/duration/timing của Blue để lược9mục.

Nếu phải giữ đủ10mẫu: thử re-encode bitrate thấp trên bản sao và kiểm chất
lượng/timing/AOD, chưa có mức tiết kiệm đo được. Không hứa chỉ gỡ vài app
là vẫn giữ đủ10mẫu mà xuống ZIP32.

Kết hợp1Infinity+nhóm329MiB có chỉ báo tổng giảm~1,089–1,109MiB nén, ứng
viên~3.76–3.79GiB nếu gần mô hình cộng riêng. Sai khác nén toàn stream và
ext4 có thể làm vượt mục tiêu; không phát hành dựa trên phép cộng này.

## Nhóm bổ sung nếu thiếu dung lượng

| Đường | Raw MiB | q6 MiB | Đánh đổi/ràng buộc |
| --- | ---: | ---: | --- |
| system/priv-app/SamsungSmartSuggestions |172.97|94.15|Manifest có nhiều provider DeepSky/OTP/smart reply/autofill/translation; widget flag false không có nghĩa package không dùng. Audit và xử lý từng caller trước |
| system/app/OCRDataProvider |180.49|134.43|Manifest com.samsung.android.sdk.ocr cung cấp resourcemanager; floating feature có TEXT_RECOGNITION_USING_OCR=true dù camera OCR_UNSUPPORT=true. Phải bỏ trích chữ có kiểm soát, không coi đã tắt |
| system/app/SamsungWeather |43.34|33.35|Mất thời tiết/widget; kiểm launcher và lockscreen/weather caller |
| system/priv-app/Routines |79.08|25.88|Manifest sharedUID systemui, nhiều provider và tích hợp mode/Bixby; giữ vòng Lite đầu |
| product/priv-app/AndroidSystemIntelligence + PrivateComputeServices |102.97|42.92|Hai package com.google.android.as/as.oss, không đồng nhất GMS core nhưng cần audit roles/overlay/caller; giữ vòng đầu |
| system/priv-app/SingleTakeService |32.82|9.80|Manifest cần saiv; kiểm chế độ Camera/Gallery. Không xóa saiv dùng chung |

Nếu thiếu biên, ưu tiên xem SmartSuggestions trước OCR sau khi audit caller,
thay vì đụng system libraries/APEX. Không gỡ các lib Arcsoft/model Gallery
theo tên hoặc kích thước: camera/editor khác có thể dùng chung.

## Những phần giữ cho Lite đầu

- Chrome64 (53.49MiB, q6 25.29): có thể bỏ browser app nhưng hiện Samsung
  Internet đã bị debloat; giữ để có trình duyệt ngay sau cài.
- WebViewGoogle64+TrichromeLibrary64: manifest Chrome dùng static library
  com.google.android.trichromelibrary version782710233; Chromium mô tả shared
  library này phục vụ Chrome/WebView. Không bỏ cả library vì đã bỏ browser.
- Google Messages159.39MiB, package com.google.android.apps.messaging: Samsung
  Messages đã bị loại; giữ SMS/MMS mặc định và telephony/provider/IMS/RIL.
- GMSCore, Google GMS APEX, Play Store, Google Services Framework, accounts,
  Setup Wizard, permissions controller, package installer, downloads,
  DocumentsUI, keyboard, camera/Gallery cơ bản, launcher/SystemUI/Settings.
- NFC/Bixby packages/services/shared libraries/features theo quyết định
  người dùng; không đổi unsupported để tiết kiệm hoặc che lỗi.
- Auto Blocker/ADB rule, PIF/Keybox framework và UI; Lite không đồng nghĩa
  cải thiện hoặc bảo đảm verdict Play Integrity.
- Hybrid Vendor VNDK33/VNDK31 bridge cần thiết, runtime/network APEX đã khóa,
  codec/audio/GPU/camera libs, kernel/DTB và modem nguyên scope hiện tại.
- TTS/TalkBack/accessibility/font/ngôn ngữ Việt/Anh giữ vòng đầu. Không xóa
  font CJK theo dung lượng khi chưa đánh giá fallback/ngôn ngữ/app.
- SDHMS/SSRM/Device Care, battery/thermal/power management: giữ để tránh
  regression boot/nhiệt/reboot. Chưa có lý do gỡ như app quảng cáo.

## Thực hiện trong fork và nghiệm thu

1. Thêm profile Lite song song từ M4 runtime profile đang boot; không sửa
   release Full, không viết build system mới, không đổi donor/kernel/radio.
2. Dùng target debloat và finalization hiện có với điều kiện Lite rõ ràng,
   cập nhật restore/gates tương ứng. Xử lý metadata fs_config/file_context,
   per-package permissions/sysconfig và launcher/widget seed. Không xóa
   blanket XML chứa shared feature: google_duo.xml hiện khai
   com.samsung.feature.SAMSUNG_EXPERIENCE, không chỉ cấu hình riêng app Duo.
3. Mỗi nhóm có lý do/files/test. Kiểm compiled dependency/roles/providers;
   manifest riêng không chứng minh không có package khác gọi vào.
4. Build ext4/OTA trong run riêng; kiểm toàn ZIP và từng entry/offset không
   ZIP64, CRC/chữ ký/hash, dung lượng <=3.8GiB hoặc báo không đạt. Không chỉ
   ép cờ ZIP32 rồi bỏ lỗi overflow.
5. Thử reader/signature/updater bằng ZIP32 lớn có updater vô hại; sau đó mới
   cài ứng viên thật trên G965F, không đổi recovery cho lượt thử Lite đầu.
6. Fresh boot/Setup/ADB ON-OFF/AutoBlocker/Enforcing, SystemUI/AOD/Infinity,
   browser/WebView login, Play Store/app install, SMS/call/data, camera/photo
   view/edit còn giữ, Bluetooth, brightness, reboot và thời gian chờ. Check
   menu dẫn tới component bị bỏ; không chỉ nghiệm thu boot_completed=1.
7. Hash phân vùng bảo vệ và rollback như protocol. Cài từ microSD/OTG đã
   kiểm mount hoặc phương thức không bị format mất file nguồn; full ZIP
   <=3.8GiB vẫn quá lớn để mặc định chép toàn bộ vào /tmp RAM hiện tại.

## Thay đổi chuẩn bị build Lite

Lý do: tạo gói ZIP32 Lite riêng từ profile M4 đã boot, giảm chín video nền và
những ứng viên người dùng đã chấp thuận. File bị ảnh hưởng trong fork:
`target/star2lte/profiles/m4-g965f-a376b-android17-lite.sh`,
`scripts/make_rom.sh`, `scripts/build_flashable_zip.sh`,
`scripts/internal/verify_m4_lite_zip32.py`, và
`unica/mods/s9_infinity_Wallpaper/source/{apply_m4_lite.sh,build_m4_lite_wallpaper.py}`.
Chuyển đổi thực hiện sau full wallpaper gate và APK rebuild, chỉ với đúng
M4/BZI6/G965F. Full và Lite có cache hash khác nhau; payload Full không bị
ghi đè. Helper xóa app dùng fs_config/file_context của fork. Lite catalog có
kiểm tra riêng Blue/engine/signature/assets và app được giữ.

Kiểm chuẩn bị: bash syntax/Python compile PASS; tạo APK Lite trên bản sao,
Blue giữ nguyên byte, `jarsigner -verify` PASS và APK Full hash không đổi.
Verifier ZIP32 chạy trên stage cũ có chữ ký hợp lệ: CRC/OTA signature PASS.
Chưa kiểm APK, menu, ROM Lite hoặc ZIP32 lớn trên điện thoại; cần tiếp tục
theo các bước nghiệm thu ở trên. Các caller Gallery và Settings bị lược vẫn
cần kiểm trực tiếp, không suy static gate là runtime pass.

## Kết quả build 03/11/2026

Build sạch hoàn thành `build_exit=0` sau 41 phút 26 giây. Release độc lập:
`rom/out/releases/M4/star2lte-1793687947-0NFukk/`.
Gói cài `UN1CA_1.4 (stable)-Unofficial-UN1CA 3.2.1_20261103_star2lte-Lite-sign.zip`
chiếm 4,060,299,586 byte = 3.78145 GiB; SHA256:
`5e84d7e4f9a1fa4990a4b981465655eccb7b3ab71dfbb901d8078791a98815d5`.
`SHA256SUMS` đã được đọc lại bằng `sha256sum -c`: cả gói cài và target-files
đều OK. CRC mọi entry, chữ ký whole-file OTA CMS, version/offset/extra ZIP32
và ngưỡng3.8GiB PASS; `system.new.dat.br` 3,473,026,014 byte, extraction
version10. Đã thử 7z trên PC tạo entry ZIP32 lớn hơn2GiB nhưng chưa test
recovery thực. Full release `star2lte-1793584471-sBoBYd` vẫn còn.

Gate trên build pass: wallpaper Lite + app bị loại + NFC/Bixby/Messages còn;
ADB compiled defaults/stored-choice, Keybox compiled, boot contract, SELinux,
ext4/e2fsck, image native32/ownership/context, donor camera và flashable
installer. APK wallpaper Lite zipalign và `jarsigner -verify` pass; jarsigner
cũng đưa cảnh báo test certificate tự ký và JarInputStream tương tự APK Full
nguồn. Log đầy đủ: `logs/20261103-m4-lite-build/build.txt` và `exit.txt`.

Giới hạn: không reboot hay flash máy trong lượt build. TWRP 3.7.0_9-0 chưa
được thử đọc/verify ZIP32 3–4GiB; chưa kiểm bản Lite boot, caller/menu của
những app đã bỏ, hình nền/AOD hoặc Play Integrity runtime. Vì vậy đây là
candidate build đã qua kiểm host, chưa phải bản flash/runtime đã nghiệm thu.

## Nguồn chính

- Local: target/star2lte/debloat.sh, unica/debloat.sh,
  unica/patches/_debloat/customize.sh, exynos9810_finalization/customize.sh,
  make_rom.sh và module s9_infinity_Wallpaper; manifest13APK đã đọc.
- Chromium Trichrome: https://chromium.googlesource.com/chromium/src/+/d92a6c59e16e009d5a14f0be69cdb122df679fcf/docs/android_native_libraries.md
- Shared library/Play filtering: https://developer.android.com/guide/topics/manifest/uses-library-element
- SMS/default roles: https://source.android.com/docs/core/permissions/android-roles
