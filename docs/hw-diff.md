# Phần cứng và các khác biệt cần kiểm chứng

SM-G965N là máy đích chính; SM-G965F thử trước. Cả hai đang recovery,
`uname -r` là 4.9.118-g36e6ddff22b9, TWRP 3.7.0_9-0.
Đây là kernel recovery, không phải kết quả chạy DS-ACK/ROM port.

| Mục | G965N | G965F | Hệ quả |
| --- | --- | --- | --- |
| Bootloader quan sát | G965NKSU5FVG2 | G965FXXUHFVG6 | Chọn biến thể bằng bootloader+serial, không chỉ tên TWRP |
| SYSTEM | 4350 MiB | 4400 MiB | Cả hai nhỏ hơn 9500 MiB của fork |
| VENDOR | 650 MiB | 650 MiB | Nhỏ hơn 1300 MiB của fork |
| ODM | 150 MiB | 646 MiB | Nhỏ hơn 700 MiB của fork |
| CACHE | 300 MiB | 600 MiB | N nhỏ hơn 600 MiB của fork |
| BOOT | 55 MiB | 55 MiB | Kiểm kích thước boot repack riêng |
| EROFS trong /proc/filesystems | Không có | Không có | Recovery đang chạy chưa có hỗ trợ EROFS tích hợp; không suy từ phiên bản TWRP |
| Baseband trong recovery | Trống | Trống | Chưa xác minh modem thực; không sửa string để lấp thông tin |

Dữ liệu thô: logs/20261020-m0-g965n và logs/20261020-m0-g965f. Giá trị /proc/partitions
được đổi từ KiB sang MiB. Chưa đọc PIT/GPT chi tiết hoặc kết luận layout stock
nguyên bản. Không tự repartition để khớp fork.

## Donor A376B → star2lte

Giữ target fingerprint mặt lưng (`google_touch_rear,settings=3,navi=1`), cấu hình
dual-camera star2lte, audio dual speaker, độ phân giải động, 60 Hz và không eSIM
theo config fork đã đọc. Chi tiết SoC/GPU/camera/sensor A37 trong kế hoạch cũ
chưa được xác minh lại bằng firmware thực; không dùng chúng để vá mù.

Kiểm kê tiếp: ABI 32/64-bit, ro.zygote, ELF dependencies, VINTF, policy mapping,
APEX VNDK33 và vendor interfaces, RIL/NFC/CSC riêng N, các thư viện GPU donor còn
tham chiếu từ framework. Tương thích từng HAL với Android 17 **cần xác minh bằng log**.

FOTA `SYSTEM/build.prop` của A376B đã đọc báo system chỉ arm64-v8a, SDK37,
release17, oneui90000. Vendor FOTA có `ro.zygote=zygote64`; cần kiểm cả
framework/APEX/ELF thực trước quyết định xử lý blob 32-bit của target.

Bản sao vendor live F/N có rild và camera.exynos9810.so trùng hash, nhưng
libsec-ril.so và libexynoscamera3.so khác hash; manifest chọn lọc nằm cạnh
recovery-baseline.txt. Không coi việc cùng star2lte là blob mọi phiên bản trùng.
debugfs chưa thấy `lib64/libril_sem.so` ở vendor live cả hai; lỗi thật được lưu:
`/lib64/libril_sem.so: File not found by ext2_lookup`. Thư viện này trong bridge
fork là thành phần tương thích, không tự gọi là blob gốc N.


## Đối chiếu package FVG4/PHN đã tải với vendor live FVG6

Package lấy từ FUS sau khi người dùng cho phép dùng Samsung serial của F; năm
file Odin qua MD5 và có manifest SHA256. Đọc vendor bằng debugfs trên PC:
model SM-G965F, SDK29, fingerprint G965FXXUHFVG4. Hash toàn vendor khác live FVG6.

| Blob chọn lọc | FVG4 package so với live FVG6 |
| --- | --- |
| bin/hw/rild | SHA256 trùng |
| lib64/libsec-ril.so | SHA256 trùng |
| lib64/hw/camera.exynos9810.so | SHA256 trùng |
| lib64/libexynoscamera3.so | SHA256 khác, không tráo tự động |

Hash/size từng file: logs/20261020-m0-buildhost/g965f-package-vendor-audit.json.
Đây là bằng chứng file chọn lọc, không phải chứng nhận toàn vendor live stock
nguyên bản hoặc kết luận runtime camera. Giữ nguồn FVG4 mặc định của fork cho M1.

Đã đọc GPT chính/dự phòng G965F chỉ đọc, CRC và entry-table equality PASS;
25 partition, logical sector4096. Geometry/manifest tại
logs/20261020-m0-buildhost/g965f-gpt-readonly.json, snapshot binary ngoài Git.
Preflight chỉ đọc xác nhận SYSTEM4400/VENDOR650/ODM646/CACHE600MiB; installer
mặc định sẽ rẽ repartition, có xóa/tạo lại USERDATA. Xem m1-artifact-review.md.


## Kiểm runtime M1 F

F đã boot S22Android16 và user xác nhận. ro.boot.em.model=SM-G965F, bootloader
G965FXXUHFVG6, basebandG965FXXUHFVB4 hai slot, SIMABSENT cả hai. VNDK runtime33,
APEXv33 hiện diện; IRadio1.6/Samsung bridge2.0 slot1/2 và HAL camera/audio/Wi-Fi/
Bluetooth/sensors đăng ký. Đây là kiểm đăng ký, không thay ma trận chức năng.
N chưa flash/boot port, không suy từ kết quả F. Chi tiết và tồn đọng EFS/USB/policy:
[m1-boot-report.md](m1-boot-report.md).


21/10/2026: fresh retest chỉ SM-G965F. Runtime Android16/SDK36/OneUI80000,
ro.vndk.version33; DS-ACK G965F08.05.2026 Enforcing giữ nguyên. Bộ radio/CP
không đổi, SIM ABSENT nên chưa kiểm gọi/data/IMS. G965N không bị nhắm tới;
không dùng kết quả fresh F để nghiệm thu blob N. Xem m1-fresh-report.md.

## Đầu vào A376B16 AZH4 thực

System Android16/SDK36.1/OneUI8.5, nhưng vendor donor ghi SDK34/VNDK34.
Target vẫn Hybrid Vendor VNDK33 + F blobs/radio bridge đã khóa cho lượt M2 F;
không dùng A37 vendor34/modem/boot images để thay S9+. N chưa có nguồn đã hash.
A37 nguồn fingerprint optical google_touch_display_optical,settings=3,
120Hz/60,120, eSIM, mDNIe37905; target rear/60Hz/no-eSIM giữ nguyên.
Source HDR Settings getAvailabilityStatus trả3 và SCameraSDKService không có;
khác S22, cần port patch thật, không sửa SOURCE flags để né nhánh thiếu hỗ trợ.
A37 source có runtime Wi-Fi feature/antenna detection: cần log, không suy từ
static default hoặc generic service registration thành nghiệm thu Wi-Fi.
Bluetooth source metadata chưa xác minh không có consumer recipe hiện tại,
để none với guard chống consumer mới; không lấy mặc định S22 làm fact A37.


M2 G965F static adaptation: Samsung framework FP REAR=3/CAPACITANCE=1 maps through existing HIDL config to Android TYPE_REAR=1. Keep native G965F fingerprint HAL. Framework/services/SystemUI optical flags and pinned compatibility BiometricSetting SIDE constant are adapted, with rear selected through floating features. Native panel baseline1440x2960/density560 and target60Hz remain; actual panel, fingerprint, mDNIe and Wi-Fi behavior need runtime logs. S9+ Wi-Fi5 hardware is unchanged; old donor Wi-Fi6 UI patches need a separate semantic port or explicit temporary UI limitation. No result here proves G965N behavior.


M2 UI decision: temporary donor Wi-Fi scan labels/icons retained under0049; S9+ Wi-Fi5 HAL/blob unchanged. Scanned Wi-Fi6 labels cannot be used as evidence of hardware AX support. Full UI correction deferred until after boot test.
