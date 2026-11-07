# M4 — kiểm chứng ghép đôi và nhận file với điện thoại,30/10/2026

## Kết quả

**PASS trong phạm vi ghép đôi và nhận hai ảnh qua Bluetooth.** Người dùng xác
nhận thiết bị có tên Bluetooth `meizuo` là một điện thoại khác, đã ghép đôi và
truyền thử hai ảnh thành công. Đây không phải lượt thử tai nghe/loa; không dùng
kết quả này để nghiệm thu A2DP/AVRCP/HFP hoặc kết nối lại phụ kiện âm thanh.
Người dùng cũng xác nhận màn hình thật đã lên Chào mừng/thiết lập, và đã chủ
động reboot sau lượt sửa trước.

G965F đang dùng cùng composition runtime0221–0224. Preflight đối chiếu đúng
bootloader F, star2lte, boot_completed1 và cả ba hash file sửa. Lượt này không
flash, format hay thay kernel/vendor/radio. USERDATA chỉ có các thay đổi bình
thường do ghép đôi/nhận file. G965N và máy M1 không bị tác động.

## Bằng chứng

Private run: `logs/20261030-m4-bluetooth-accessory/run-141335/`.
Ảnh, tên file, URI và địa chỉ Bluetooth không đưa vào báo cáo/source evidence.
Không đọc nội dung hai ảnh.

- Người dùng xác nhận ghép đôi và gửi hai ảnh đều thành công.
- `live-logcat.txt:66611`: `onDeviceAdded: cached = meizuo, bonded = 12`.
- `live-logcat.txt:71278` và`:83094`:
  `OppTransfer receiving termination status : 200` — hai kết thúc nhận file.
- `live-logcat.txt:83200`:
  `receive MSG_SESSION_COMPLETE mStatus : 200` — hoàn tất phiên.
- 34 file mẫu trong337,03 giây, uptime282,55→619,58s: cùng boot ID,
  system_server1835/SystemUI2618/Bluetooth2529, server start_count1.
- Tất cả mẫu và final dump đều ghi `Bluetooth crashed 0 times`.
  Live log không có `Timed out waiting for Unknown OpCode(0xfdf1)`.
- Crash buffer cuối giống hệt ban đầu; vẫn có exception Bixby đã biết,
  không có fatal Java mới trong cửa sổ đo.
- `result.json` và `complete.json` lưu kết quả. Số mẫu lấy từ34 file thực tế;
  đã sửa lỗi metadata đếm dư một ở nhánh dừng sớm của collector. Dữ liệu mẫu
  và log truyền file không thay đổi. Collector đã kết thúc.

Preflight không thấy active A2DP device. Điều đó không tạo ra verdict lỗi âm
thanh: người dùng đang thử truyền file với điện thoại, không thử audio sink.
Hướng nhận vào G965F có bằng chứng server/receive; hướng gửi ngược chưa được
đo trong lượt này.

## Reboot và phạm vi chấp nhận

Boot ID khác lượt sửa trước vì người dùng xác nhận đã chủ động reboot. Không
đánh dấu thay đổi boot này là spontaneous reboot. Hai boot-reason fields còn
ghi `shutdown,[setupwizard_global]_30_minutes_timeout`; giữ nguyên bằng chứng
lịch sử đó, không suy diễn thành kernel panic hoặc Bluetooth-triggered reboot.
Các file sửa vẫn đúng hash sau reboot, system_server start_count1 và UI lên
được theo xác nhận người dùng.

Trạng thái Bluetooth hiện tại:

| Hạng mục | Kết quả |
| --- | --- |
| Startup / bật-tắt / giữ qua reboot | PASS theo các lượt0223–0224 và preflight hiện tại |
| Ghép đôi với điện thoại | PASS |
| Nhận hai ảnh qua OBEX/OPP | PASS |
| Gửi file ngược / mở và đối chiếu nội dung hai đầu | Chưa đo |
| A2DP phát nhạc, AVRCP pause/volume | Chưa thử với phụ kiện âm thanh |
| HFP mic/cuộc gọi | Chưa thử |
| Tắt/bật phụ kiện âm thanh rồi kết nối lại | Chưa thử |

M4 chưa nghiệm thu tổng thể. Bixby Side Key, NFC typed/RF, app network,
model properties và các gate phần cứng khác còn theo
[PLAN](PLAN.md) và [runtime repair report](m4-post-0223-runtime-repair.md).
Khi có tai nghe/loa, mở lượt đo mới cho A2DP/AVRCP/reconnect; không cần lặp lại
bài ghép đôi với điện thoại chỉ để gọi đó là kiểm thử âm thanh.

## Ghi nhận thay đổi

Lý do: ghi đúng loại thiết bị và phạm vi phép thử sau phản hồi người dùng;
không bỏ qua kết quả truyền file có giá trị và không đánh đồng nó với audio.
File ảnh hưởng: báo cáo này, PLAN/bugs/STATUS/README, source composition record
và bản evidence đã lọc. Kiểm chứng: xác nhận người dùng,34 mẫu ổn định, hai
status200 và một session-complete200, final Bluetooth/crash dump. Không có
patch code hoặc image mới; full ROM attempt0223 vẫn chưa chứa0221–0224.
