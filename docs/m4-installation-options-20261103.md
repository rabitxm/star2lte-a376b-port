# M4 — đánh giá cách cài ROM lớn trên TWRP

Ngày 03/11/2026. Phạm vi: đọc artifact/source/log, đo dung lượng và lập phương
án; chưa sửa package, gỡ app, build recovery hay ghi điện thoại.

## Bằng chứng hiện tại

Release đang xét: `rom/out/releases/M4/star2lte-1793584471-sBoBYd/`.
Full ZIP `UN1CA_1.4 (stable)-Unofficial-UN1CA 3.2.1_20261102_star2lte-sign.zip`:

- 5,204,219,280 byte = 5.204 GB = 4.847 GiB.
- Manifest source SHA256:
  `41a666c005031115d563475246bbe2722d55b3b391027d70d0500318a1bd7162`.
  Lượt này đối chiếu manifest và kích thước/central directory; không tính lại
  SHA toàn bộ gói hoặc chạy lại CRC toàn bộ payload.
- `system.new.dat.br`: 4,616,947,120 byte; entry vượt giới hạn ZIP32, extract
  version 45. `vendor.new.dat.br`: 529,261,425 byte. Nhiều entry phía sau có
  offset lớn hơn 4 GiB. Chia entry bên trong mà giữ outer ZIP lớn không đủ
  tránh mọi vấn đề ZIP64.
- Log lỗi thật: `logs/20261022-m2-g965f-flash-01/twrp-install-first-attempt.txt:165`
  ghi `Zip file is corrupt!`, trước updater. Reader cùng lượt hiển thị
  `4294967295 ... system.new.dat.br` và `unzip: invalid zip magic AE9D8EE9` tại
  `twrp-zip-reader-and-postattempt-state.txt`. M1 còn ghi nhận full size bị
  lệch 2^32 trong `docs/m1-artifact-review.md`.
- Tám stage hiện có: tổng 5,330,201,426 byte; lớn nhất 1,186,979,767 byte.
  Manifest status PASS; `logs/20261102-m4-ec-keybox-flash/flash-status.json`
  ghi đủ 8 stage `install_exit=0`, `completed=true`. Đây là bằng chứng cài
  thành công của các gói stage, không phải full ZIP trực tiếp.

Giới hạn <2 GiB là biên bảo thủ đang được helper kiểm và đã cài thật. Không
khẳng định TWRP này tuyệt đối từ chối mọi ZIP 2–4 GiB: muốn dùng vùng đó phải
kiểm thêm reader/updater. Để ZIP32, toàn bộ kích thước/offset/entry cũng phải
vừa định dạng; chỉ giảm một entry là chưa đủ.

## Những việc installer đang làm ngoài ghi image

Đọc trực tiếp updater-script trong release và source
`target/star2lte/installer/install-start.edify`, `install-end.edify`,
`platform/exynos9810/installer/repartition/vendor_bootfix.sh`:

1. Assert phần cứng G965F/FD/FDS thực, không dùng model donor SM-A376B.
2. Kiểm/chia layout, dọn filesystem khi layout thay đổi.
3. Ghi SYSTEM/VENDOR/ODM và chạy vendor boot fixes, chọn cấu hình SIM;
   EFS chỉ được đọc ro,noload khi cần.
4. Ghi template ramdisk rồi chạy kernel installer để tạo BOOT hoàn chỉnh.
5. Format CACHE/OMR/HIDDEN/USERDATA cuối lượt; internal storage nằm trên
   USERDATA và không phải nơi giữ payload an toàn trong suốt fresh install.

Không được đem riêng `boot.img` 2,084,880 byte trong ZIP vào Odin và coi đó là
BOOT hoàn chỉnh: installer còn lắp kernel ở bước sau. Không đổi tên ZIP thành
TAR để Odin flash. Không đóng gói dump nguyên máy chứa EFS/định danh/dữ liệu.

## So sánh phương án

| Hướng | Giữ đủ ROM | Công việc/giới hạn | Đánh giá |
| --- | --- | --- | --- |
| Chuẩn hóa bộ cài PC chạy tự động 8 stage | Có | Đưa runner đã dùng vào scripts của fork; model/hash/order/rollback/log guard; vẫn cần PC/cáp | Ưu tiên triển khai trước, dựa trên quy trình đã cài thật |
| ZIP điều khiển nhỏ + stage ngoài trên microSD/OTG | Có | TWRP Install một ZIP; dispatcher gọi updater của từng stage, cần kiểm UI fd/return code/mount/format và hash trước ghi | Ưu tiên nếu muốn cài bằng giao diện TWRP, không cần PC |
| ROM Lite một ZIP32 | Tùy phần lược bỏ | Cần giảm >867 MiB nén chỉ để dưới 4 GiB; nên có biên; cần kiểm vùng 2–4 GiB hoặc giữ nhiều stage | Tùy chọn dung lượng, không nên là điều kiện để cài ROM |
| Recovery star2lte sửa ZIP64 | Có | Sửa/kiểm cả reader/verifier/sideload và updater; mount/layout/fstab, RAM và image size; thêm artifact phải bảo trì | Khả thi, chưa là ưu tiên |
| Odin/Heimdall image package | Có thể | Image cuối phải hoàn chỉnh, đúng layout/PIT; tái hiện bootfix/SIM/kernel/format; chưa có exporter trong fork | Để sau, phù hợp hơn cho layout đã chuẩn hóa |

ZIP điều khiển + payload là một lần thao tác Install nhưng nhiều file phát
hành, không phải một ZIP tự chứa toàn ROM. Không nhét lại cả tám stage vào
một outer ZIP >4 GiB rồi trông đợi reader cũ mở được. Không gọi đệ quy
`twrp install` từ trong update-binary đang chạy; thiết kế dispatcher phải
thử gọi updater trực tiếp và chuyển đúng fd giao diện.

microSD/OTG là thiết bị ngoài USERDATA; phải kiểm mount thật và khả năng đọc
trên máy test trước khi chọn phương thức. Không đoán mountpoint. Recovery
device tree upstream có các entry /external_sd và /usb-otg; đây chưa là phép
thử trực tiếp thiết bị lưu trữ của người dùng.

TWRP chính thức cho star2lte hiện vẫn liệt kê 3.7.0_9-0 ở đầu danh sách. Tree
Android9 có `BOARD_HAS_LARGE_FILESYSTEM := true`, nên chỉ bật cờ đó không là
bản sửa ZIP64. Kernel recovery là vấn đề riêng kernel ROM; chưa có bằng
chứng yêu cầu build kernel mới cho vấn đề cài ZIP này. Gói recovery sửa riêng
có thể dùng prebuilt phù hợp nếu audit chứng minh đủ.

## Kiểm kê app và giới hạn ước lượng

Đo từ `rom/out/target/star2lte/work_dir` sau build. Cột Brotli là nén riêng
từng file bằng `brotli --quality=6 --stdout`, cùng quality với packager; chỉ
là chỉ báo khả năng giảm. Ext4 layout và nén toàn stream khác, nên không lấy
tổng cột này làm dung lượng ZIP sau rebuild.

| Thư mục | File hiện tại (MiB) | Nén riêng q6 (MiB) | Tác động cần đánh giá |
| --- | ---: | ---: | --- |
| system/system/priv-app/wallpaper-res | 889.28 | 864.00 | 10 video nền, catalog/Infinity/AOD đi cùng nhau; ưu tiên tối ưu video, không xóa APK mù |
| system/product/priv-app/Velvet | 306.70 | 138.28 | Google app/Assistant, tìm kiếm/voice integration |
| system/product/app/Duo | 117.14 | 46.63 | Meet/Duo; ứng viên tùy chọn sau kiểm manifest |
| system/system/app/OCRDataProvider | 180.49 | 134.43 | Trích chữ/OCR của Camera/Gallery và nơi gọi |
| system/system/priv-app/SamsungSmartSuggestions | 172.97 | 94.15 | Gợi ý thông minh, tích hợp bàn phím/launcher |
| system/system/priv-app/PhotoRemasterService | 76.32 | 54.75 | Remaster Gallery và mục UI liên quan |
| system/system/app/VideoEditorLite_Dream_N | 49.64 | 21.29 | Chỉnh video từ Gallery |

TrichromeLibrary64 194.57 MiB, WebViewGoogle64 173.75 MiB có dung lượng lớn
nhưng không chọn gỡ để xử lý cài đặt. Không gỡ GMS/Play Store/Setup Wizard,
framework/APEX, telephony, SystemUI/Settings/launcher, Hybrid Vendor/blob,
NFC/Bixby compatibility declarations/services/packages để lấy dung lượng.
Các app trong bảng chưa được kết luận độc lập hoặc an toàn để xóa.

Full ZIP phải giảm ít nhất 909,251,985 byte (867.13 MiB) để chỉ vừa dưới
2^32−1 byte. Mục tiêu 3.8 GiB cần giảm khoảng 1,071.93 MiB. Bỏ toàn bộ bộ
hình nền vẫn chưa đủ theo chỉ báo nén riêng, đồng thời phá tính năng đã port.
Đưa gói về <2 GiB còn đòi giảm khoảng 2.85 GiB nén, không phù hợp với mục
tiêu giữ trải nghiệm One UI đầy đủ.

Trong wallpaper APK, 10 video chiếm khoảng 881 MiB. ffprobe mẫu cho thấy
video001 H264 1440x3120 60fps, 1021 frame, 17.016667s, tổng bitrate49.39Mbps;
video006 cùng resolution/fps,1020frame,17s,35.56Mbps. Có thể thử giảm bitrate
trên bản sao, giữ timing/frame/keyframe và catalog; nếu giảm fps/duration
phải xử lý lại thumbnail/transition frame. Cần đo chất lượng và lock${LOCAL_ABSOLUTE_PATH}
trên máy thật. Chưa re-encode, chưa đo mức tiết kiệm của encode mới.

## Kế hoạch ưu tiên và kiểm chứng

1. Đưa runner cài PC đã dùng trong logs thành command bảo trì của fork;
   tham số release + serial rõ ràng, đọc manifest, không hardcode bản mới
   nhất. Không tạo build system mới. Trước ghi: model/bootloader/layout/hash,
   protected-partition snapshots, BOOT/GPT rollback và khả năng lưu log.
2. Kiểm fixture âm: thiếu stage, sai hash, sai thứ tự, sai thiết bị, thiếu
   RAM/storage đều dừng trước ghi. Mất USB không tự bỏ qua hoặc reboot vào
   SYSTEM chưa cài xong. Recovery reboot giữa stage thì phải chạy lại từ1;
   không hứa resume xuyên recovery reboot.
3. Nếu ưu tiên TWRP một lần Install: mở rộng packaging hiện có tạo launcher
   ZIP32 ký và manifest hash ràng buộc đủ8 stage. Kiểm toàn bộ payload trước
   ghi; nguồn phải ngoài các phân vùng sẽ format. Thử dispatcher trên file
   scratch trước, sau đó một lượt G965F, kiểm return code + marker + image
   readback + protected hashes + boot. Giữ bộ cài cũ làm rollback.
4. Debloat/video optimization là profile tùy chọn song song. Mỗi nhóm có lý
   do, file bị ảnh hưởng và test riêng; không làm chung với đổi recovery hay
   backend ghi Odin. Chỉ công bố kích thước sau package/CRC/signature/hash.
5. Recovery ZIP64 và Odin để riêng giai đoạn sau nếu còn nhu cầu. Recovery
   phải kiểm cả ZIP lớn/entry lớn/offset lớn và updater, không chỉ mở ZIP UI.
   Odin cần image tổng hợp từ đầu ra sạch, không lấy dump máy đã nhập keybox;
   giữ modem/bootloader/EFS/RECOVERY ngoài scope gói ROM và chốt layout trước.

## Nguồn upstream đã đọc

- TWRP star2lte: https://twrp.me/samsung/samsunggalaxys9plus.html
- Download/build hiện có: https://dl.twrp.me/star2lte/
- Device tree/build recipe: https://github.com/TeamWin/android_device_samsung_star2lte
- BoardConfig: https://github.com/TeamWin/android_device_samsung_star2lte/blob/android-9.0/BoardConfig.mk
- Recovery fstab: https://github.com/TeamWin/android_device_samsung_star2lte/blob/android-9.0/recovery.fstab
- TWRP reader/installer: https://github.com/TeamWin/android_bootable_recovery/blob/android-9.0/twinstall.cpp
- ZipWrap: https://github.com/TeamWin/android_bootable_recovery/blob/android-9.0/zipwrap.cpp
- Heimdall partition/PIT workflow: https://github.com/Benjamin-Dobell/Heimdall/blob/master/Linux/README
