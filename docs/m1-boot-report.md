# M1 G965F — boot thực đã xác nhận

Ngày 20/10/2026: người dùng xác nhận “đã boot thành công”. ADB của đúng F
(serial kết thúc 17ece, USB 2-7) xác nhận `ro.boot.em.model=SM-G965F`,
bootloader `G965FXXUHFVG6`, `sys.boot_completed=1`, `dev.bootcomplete=1`,
`init.svc.bootanim=stopped`. Màn hình chụp là Welcome/Start của Setup Wizard.
Sự kiện boot-completed được watcher quan sát ở giây 165; không phải phép đo
thời gian boot chính xác. Đây là M1 S22 One UI 8/Android 16; chưa phải A37/Android 17.

## Bằng chứng và giới hạn

- Runtime Android `16`, SDK `36`, One UI `80000`.
- Kernel `4.9.337-DS-ACK-V1.12-G965F-08.05.2026`; BOOT sau cài khớp
  SHA256 của repack RAM đầy đủ partition, kernel/DTB/ramdisk khớp payload đã khóa.
  Không sửa/build kernel, không đổi chuỗi phiên bản. Gói chọn là duhansysl May
  Enforcing-OneUI7 không KernelSU; kiểm tra các đường dẫn runtime KSU đã chọn không thấy mục nào; không phải quét toàn bộ thiết bị.
- `getenforce=Enforcing`; runtime `ro.debuggable=1`, `ro.adb.secure=0`,
  shell ADB có UID0 và context `u:r:su:s0`. Log có denial của `su`
  `permissive=1`, vì vậy chưa nghiệm thu policy release/toàn bộ domain enforcing.
- APEX VNDK33 mount thành công theo log bên dưới. Fingerprint vendor Android10
  không chứng minh vendor stock nguyên bản: output M1 vẫn dùng Hybrid Vendor của fork.
- Baseband báo `G965FXXUHFVB4,G965FXXUHFVB4`; SIM `ABSENT,ABSENT`.
  Có phiên RIL thoát/restart trong boot; chưa kết luận gọi/data/IMS hoạt động.
- Thu 151903 dòng logcat, 4515 dòng dmesg; pstore rỗng. Trong snapshot này
  không tìm thấy mẫu `FATAL EXCEPTION`, `Fatal signal`, `am_crash:`, `am_anr:`,
  VINTF incompatible hoặc kernel panic/BUG/Call trace. Giới hạn ở log đã thu,
  không phải chứng nhận mọi dịch vụ/phần cứng ổn định.
- ADB trở lại ở giây150, mất ở160, trở lại165; sau lần capture có ngắt tiếp.
  Các lệnh health/lshal/service-list bổ sung đầu tiên trả device not found;
  lần quan sát bổ sung lưu riêng, không ghi đè bằng chứng boot thành công.

## Snapshot bổ sung sau boot

Tại uptime342.48s, `sys.boot_completed=1`, `ro.vndk.version=33` và
`ro.vendor.build.version.sdk=33`; `/apex/com.android.vndk.v33` và bản `@1`
hiện diện. `ril-daemon`, zygote32/64, audioserver, cameraserver, surfaceflinger,
servicemanager/hwservicemanager/vndservicemanager đều running. PID core có mặt.
`lshal` đăng ký `android.hardware.radio@1.6::IRadio` và Samsung radio bridge2.0
cho slot1/slot2; HAL audio6.0, Bluetooth1.0, camera2.4–2.6, sensors1.0,
Wi-Fi1.0–1.4 và composer2.1/2.2 hiện diện. Chỉ xác nhận đăng ký dịch vụ,
chưa phải nghiệm thu chức năng. Snapshot logcat bổ sung không có mẫu fatal/ANR
hoặc VINTF incompatible; RIL có đúng một exit0 đã thấy ở boot, sau đó chạy PID2091.

Nguồn riêng: `inputs/m1-runtime-logs/flash-01/postboot-observe-01/health-selected.txt`,
`hal-list.txt`, `core-service-list.txt`, `logcat.txt`. Kết nối phải thử35 lần mới
thu được health; dmesg bổ sung thất bại do ADB mất sau khi lấy logcat. Bản dmesg
ban đầu vẫn đủ4515 dòng, không bị ghi đè. Không diễn giải ADB mất là máy reboot:
uptime snapshot342.48s tiếp nối bản đầu176.21s.

## Dòng lỗi/thông tin cần theo dõi

Các số dòng dưới thuộc `inputs/m1-runtime-logs/flash-01/boot-completed-01/logcat.txt`
(mode600, bỏ Git). Không tự sửa kernel từ các thông báo này.

### vndk33_mounted

- Dòng 4543: `10-20 21:00:00.794     0     0 I [7:          apexd:  810] apexd-bootstrap: Successfully mounted package /system/system_ext/apex/com.android.vndk.v33.apex on /apex/com.android.vndk.v33@1 duration=51`
- Dòng 6657: `10-20 21:00:15.814     0     0 I [6:          apexd:  940] apexd: Successfully mounted package /system/system_ext/apex/com.android.vndk.v33.apex on /apex/com.android.vndk.v33@1 duration=13`

### usb_udc

- Dòng 15012: `10-20 21:00:58.708     0     0 I [5:           init:    1] init: Command 'write /config/usb_gadget/g1/UDC ${sys.usb.controller}' action=sys.usb.config=mtp,adb && sys.usb.ffs.ready=1 (/vendor/etc/init/init.samsungexynos9810.usb.rc:117) took 0ms and failed: Unable to write to file '/config/usb_gadget/g1/UDC': Unable to write file contents: No such device`
- Dòng 18469: `10-20 21:00:59.638     0     0 I [7:           init:    1] init: Command 'write /config/usb_gadget/g1/UDC ${sys.usb.controller}' action=sys.usb.config=mtp,adb && sys.usb.ffs.ready=1 (/vendor/etc/init/init.samsungexynos9810.usb.rc:117) took 0ms and failed: Unable to write to file '/config/usb_gadget/g1/UDC': Unable to write file contents: No such device`

### usb_duplicate_link

- Dòng 18460: `10-20 21:00:59.637     0     0 I [7:           init:    1] init: Command 'symlink /config/usb_gadget/g1/configs/b.1 /config/usb_gadget/g1/os_desc/b.1' action=sys.usb.config=mtp,adb && sys.usb.ffs.ready=1 (/vendor/etc/init/init.samsungexynos9810.usb.rc:116) took 0ms and failed: symlink() failed: File exists`
- Dòng 21298: `10-20 21:01:00.685     0     0 I [4:           init:    1] init: Command 'symlink /config/usb_gadget/g1/configs/b.1 /config/usb_gadget/g1/os_desc/b.1' action=sys.usb.config=mtp,adb && sys.usb.ffs.ready=1 (/vendor/etc/init/init.samsungexynos9810.usb.rc:116) took 0ms and failed: symlink() failed: File exists`

### su_permissive_denial

- Dòng 14637: `10-20 21:00:58.578     0     0 E [5:           init:  749] audit: avc:  denied  { set } for property=sys.usb.ffs.ready pid=1771 uid=0 gid=0 scontext=u:r:su:s0 tcontext=u:object_r:ffs_control_prop:s0 tclass=property_service permissive=1`
- Dòng 14638: `10-20 21:00:58.578     0     0 I [5:           init:  749] avc: denied  { set } for property=sys.usb.ffs.ready pid=1771 uid=0 gid=0 scontext=u:r:su:s0 tcontext=u:object_r:ffs_control_prop:s0 tclass=property_service permissive=1`

### wifi_modprobe_nonzero

- Dòng 4644: `10-20 21:00:01.078     0     0 I [5:           init:    1] init: Service 'exec 2 (/vendor/bin/modprobe -a -d /vendor/lib/modules dhd.ko)' (pid 841) exited with status 1 waiting took 0.010000 seconds`

### ril_restart

- Dòng 26881: `10-20 21:01:05.397     0     0 I [2:           init:    1] init: Service 'ril-daemon' (pid 1805) exited with status 0`

### ril_missing_restart_service

- Dòng 26887: `10-20 21:01:05.400     0     0 I [3:           init:    1] init: Command 'restart cpboot-daemon5g' action=onrestart (<Service 'ril-daemon' onrestart>:2) took 0ms and failed: service cpboot-daemon5g not found`

### optional_linker_load

- Dòng 12043: `10-20 21:00:56.363  1603  1603 E HYPER-HAL: [HandlerQC.cpp]HandlerQC(): unable to open vendor/lib64/libqti-perfd-client.so: dlopen failed: library "libqti-perfd-client.so" not found`
- Dòng 12044: `10-20 21:00:56.363  1603  1603 E HYPER-HAL: [HandlerMtk.cpp]HandlerMtk(): dlopen error: dlopen failed: library "libmtkperf_client_vendor.so" not found`

## EFS và audit trước boot

`preboot-audit.json` giữ nguyên **FAIL**: EFS đổi raw SHA256 trước lần boot đầu.
CPEFS/RADIO/RECOVERY/BOTA0/1/2 không đổi; hai GPT CRC/table PASS, prefix entry1–17
không đổi, BOOT/kernel/DTB/ramdisk PASS, USERDATA ext4. Recovery từng mount EFS
ordered-data/journal theo kernel log; chưa chứng minh thay đổi chỉ là metadata.
Sau khi được báo vấn đề và hỏi đường dẫn backup, người dùng trả lời
“tiếp tục đi, máy dev thì backup làm gì”. Ngoại lệ cụ thể được ghi trong
`efs-boot-exception.json`, ràng buộc hash audit. Đã tiếp tục boot theo chỉ thị,
không khôi phục EFS, không biến FAIL thành PASS hoặc thay baseline hash.

## Thay đổi trong lượt boot/log

Lý do: hoàn thành lượt M1 F đã được ủy quyền và lưu bằng chứng runtime chính xác.
File ảnh hưởng: metadata/evidence trong flash-01, `docs/m1-boot-report.md`,
`docs/m1-artifact-review.md`, `docs/PLAN.md`, `docs/bugs.md`, `docs/hw-diff.md`,
`README.md`, `STATUS.md`, `firmware.lock.json`. Không có patch fork/kernel mới
trong lượt đọc log này. Kiểm chứng: đối chiếu ADB serial/model/BL, properties,
ảnh Welcome, raw log với số dòng; JSON parse; giữ nguyên audit/exception.

M1 **qua gate boot do người dùng xác nhận**; nghiệm thu chức năng còn mở.
G965N chưa flash; chưa tự flash milestone tiếp theo. Muốn lên A37 cần giữ một
biến mỗi mốc theo PLAN, đồng thời xử lý/đánh giá các tồn đọng runtime trên.

## M1 diagnostic logger / keyless ADB — 23/10/2026

Artifact M1 build0095 được cài fresh trên G965F phụ serial `M1_G965F_SERIAL`
theo authorization đã lưu; tám legacy stage đều `host_exit=0`, marker chạy tuần
tự `next=2…done`, USERDATA được format ext4. M1 lên Android thành công:
`sys.boot_completed=1`, `ro.boot.em.model=SM-G965F`, `ro.product.device=star2lte`,
SDK36, One UI `80000`. Android báo model S22 donor `SM-S901B` ở
`ro.product.model`; đây là model system property, không phải nhận dạng bootloader.

ADB không hỏi RSA: `ro.adb.secure=0`, `ro.debuggable=1`, USB state
`sec_charging,adb`, `adb_enabled=1`, thư mục `/data/misc/adb` không có key, và PC
nhận thiết bị trực tiếp ở trạng thái `device`; `adb shell id` là root. Giá trị
fresh-data Rampart/Auto Blocker đều `0`: system `rampart_suw_main_on`, secure
`rampart_main_switch_enabled`, `rampart_strict_protection_switch_enabled`,
`rampart_blocked_adb_cmd`, `rampart_blocked_usb_data_transfer`. Đây xác nhận
seed/default và kết nối không cần key; chưa phải thử tương tác UI bật/tắt.

Logger ghi marker post-fs/post-fs-data, cmdline, partition map và logcat 15/60s
vào CACHE. File tổng hợp đầu boot lại có các phần getprop/dmesg/proc rỗng;
chạy đúng logger thủ công sau boot trong thư mục thử `late_test` cho kết quả
đầy đủ (`props_start.txt` 58,798 byte, `dmesg_start.txt` 386,188 byte,
`init_debug_start.txt` 22,419 byte). Vì vậy lỗi nằm ở thời điểm/chính sách chạy
service sớm; chưa sửa logger khi chưa xác định chính xác nguyên nhân. Logcat
15/60s có sẵn để chẩn đoán. UDC lặp lỗi bind nhưng ADB cuối cùng hoạt động;
ví dụ `runtime/logcat-all.txt:15074`: `udc 10c00000.dwc3: failed to start g1: -19`,
và dòng15076: `Unable to write to file '/config/usb_gadget/g1/UDC': ... No such device`.
Log ở các phút đầu có AVC `permissive=1`; đây không phải release SELinux PASS.

Preboot audit xác nhận layout đúng kích thước, BOOT sau cài vẫn bằng baseline
raw hash, CPEFS/RADIO/RECOVERY/BOTA không đổi. EFS là hash duy nhất lệch so với
preflight, dù installer và gói kernel đã audit không có lệnh ghi EFS; không đọc,
restore hay ghi EFS. Người dùng đã yêu cầu tiếp tục sau khi được báo sai khác;
đây là điều chưa giải thích và không chứng minh hỏng dữ liệu.

Log và audit đầy đủ: `logs/20261023-m1-init-debug/flash-01/`; package retained tại
`rom/out/releases/M1/star2lte-1792729136-64VYgG/`. G965N không bị chạm. Gate
M1 fresh boot và ADB defaults/keyless đạt; UI toggle, release policy, radio/SIM
và nghiệm thu phần cứng vẫn là các gate riêng.
