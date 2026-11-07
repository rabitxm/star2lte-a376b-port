# M4 r5 — kiểm chứng sau Setup Wizard, 01/11/2026

**Audit tiếp cùng ngày:** hai app-probe key độc lập xác nhận software
attestation; KeyInfo báo key không ở secure hardware. Trung gian hết hạn và
root chain probe không khớp Google Hardware Attestation Root hiện công bố.
Finalizer của fork chủ động chỉ giữ software KM4, loại service KM3. Chain GMS
và runtime readback các field GMS chưa được quan sát trực tiếp. Xem
[audit và giới hạn](m4-r5-attestation-audit.md); các bước dự kiến bên dưới
được cập nhật theo kết quả đó.

## Kết quả

Người dùng đã hoàn tất Setup Wizard và báo Play Integrity chưa đạt. Đã xác
minh trực tiếp trên G965F M4 đang chạy r5: `user_setup_complete=1`,
`sys.boot_completed=1`, public model `SM-A376B`, physical model `SM-G965F`,
SELinux Enforcing, `remote_provisioning.tee.rkp_only=0`. Core PID cuối vẫn là
SystemServer2209/SystemUI6252, trùng snapshot đầu lượt kiểm tra.

**Integrity còn mở:** hai công cụ độc lập đều không trả nhãn; PIF ON/OFF có
tác dụng thật lên fingerprint của tiến trình Play Store nhưng không thay đổi
kết quả server. Không có build hoặc flash mới trong lượt này. PIF đã được
khôi phục **ON** bằng chính công tắc Settings. Không xóa dữ liệu GMS/Play Store,
tài khoản hay ứng dụng; không ghi phân vùng ROM.

## Bốn yêu cầu mới, không tái dùng kết quả r4

| Công cụ | PIF | Thời điểm hoàn tất 01/11 | Kết quả |
| --- | --- | --- | --- |
| Checker cài qua Play Store | ON | 13:32:58.551 | Không có `deviceRecognitionVerdict`; app/license/Play Protect `UNEVALUATED` |
| Checker | OFF | 13:35:30.682 | Cùng dạng JSON, không có nhãn |
| Play Store, Developer options → Check integrity | OFF | 13:38:46.789 | `Labels: []`; fingerprint Samsung BZI6 |
| Play Store, Developer options → Check integrity | ON | 13:39:50.459 | `Labels: []`; fingerprint Google bluejay từ profile PIF |

JSON là nội dung được checker hiển thị, chưa audit backend riêng của checker.
Phép thử tích hợp Play Store độc lập cũng trả rỗng. Trang About của Play Store
báo **Thiết bị chưa được chứng nhận**. Chỉ bật menu developer của Play Store
để dùng công cụ được Google mô tả; không thay đổi Play Protect scanning.

Readback thực từ dialog Play Store:

- OFF: `samsung/a37xnaxx/star2lte:17/CP2A.260605.016/A376BXXU5BZI6:user/release-keys`.
- ON: `google/bluejay_beta/bluejay:CANARY/ZP11.260918.007/16484274:user/release-keys`.
- Cả hai giữ brand `samsung`, device `star2lte`, model `SM-A376B`.

Đây là bằng chứng hook fingerprint và công tắc có tác dụng trong Play Store.
Không suy từ đó rằng mọi field trong GMS unstable đã được readback. GMS unstable
thực có mặt trong capture checker ON, PID13970; không có căn cứ nới allowlist
chỉ vì snapshot trước từng không thấy tiến trình này. PIF toggle force-stop
GMS/Play Store theo controller hiện có; các lượt OFF/ON dùng tiến trình mới.

## Log quyết định và giới hạn

```text
11-01 13:32:57.675 W keystore2: ... Failed to get rpc for sec level r#TRUSTED_ENVIRONMENT
11-01 13:32:57.675 W keystore2: ... Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)
11-01 13:32:57.707 I Finsky: [312] Integrity key attestation record generated successfully.
11-01 13:32:58.551 I Finsky: [62] requestIntegrityToken() finished for gr.nikolasspyr.integritycheck.
11-01 13:35:29.582 I Finsky: [105] Integrity key attestation record generated successfully.
11-01 13:35:30.682 I Finsky: [62] requestIntegrityToken() finished for gr.nikolasspyr.integritycheck.
11-01 13:38:46.063 I Finsky: [208] Integrity key attestation record generated successfully.
11-01 13:38:46.789 I Finsky: [61] requestIntegrityToken() finished for com.android.vending.
11-01 13:39:49.577 I Finsky: [208] Integrity key attestation record generated successfully.
11-01 13:39:50.459 I Finsky: [62] requestIntegrityToken() finished for com.android.vending.
```

RPC warning vẫn có trên stack legacy không đăng ký IRPC, nhưng yêu cầu sau đó
tạo record thành công. Không báo warning riêng thành fatal key-generation
failure; cũng không xem record thành công là Google đã chấp nhận chuỗi tin cậy.

Trong các cửa sổ yêu cầu đã thu không thấy lại `Failed to spoof`, `Failed to
set prop`, lỗi ART static-field writer, `IllegalAccessException`,
`NoSuchFieldError` hoặc `ProviderException`. Không suy absence thành readback
đầy đủ của GMS. Crash buffer cuối vẫn chỉ có lỗi smdexe lúc21:20:09.914 ngày31/10
và Bixby lúc21:22:32.246 đã ghi trước; không có crash Settings/core mới.

`UNEVALUATED` không tự chứng minh app bị sideload hoặc tài khoản không có
license. Verdict rỗng cũng không chỉ ra chính xác điều kiện server nào thất
bại. Google mô tả Android13+ DEVICE cần bằng chứng phần cứng về bootloader khóa
và OS được hãng chứng nhận; BASIC có yêu cầu Google attestation root of trust.
Chưa phân tích chuỗi thực ở r5 nên không kết luận factory chain hiện tại có
root nào hoặc bị revoke. Nguồn: [verdict definitions](https://developer.android.com/google/play/integrity/verdicts),
[phép thử Play Store](https://developer.android.com/google/play/integrity/additional-tools).

## Trang Ẩn trạng thái nhà phát triển

Đã mở đúng `Settings$UnicaHideDevSettingsActivity` trên r5:

1. Hiện switch thật và danh sách ứng dụng, không crash vì `mMainSwitch`.
2. OFF → ON ghi `settings system unica_hide_dev=1`.
3. Back/reopen giữ ON.
4. Chọn checker ghi package vào secure `unica_hide_dev_list`; back/reopen giữ
   checkbox.
5. Bỏ chọn và OFF ghi0; sau đó khôi phục trạng thái system setting vắng mặt và
   danh sách secure rỗng như ban đầu.

Đóng lỗi crash/trình bày/lưu lựa chọn của trang này theo phạm vi đã thử. Chưa
đo riêng giá trị Developer options mà app đã chọn đọc được; không gọi đó là
kiểm chứng toàn bộ hành vi ẩn.

## Bước tiếp theo có ích trước một bản ROM khác

1. **Phân tích attestation của stack thật, không thay chứng chỉ.** Dùng probe
   key tạm trong app context để tạo challenge rồi đọc chain/extension thực:
   issuer/root, chữ ký và validity, security level, verified-boot state,
   `deviceLocked`, OS/vendor/boot patch levels. Chỉ xóa alias do probe tạo;
   không xuất private key, không đọc key của GMS, không đụng EFS. Đối chiếu
   mô tả root chính thức. Chain của app probe không tự đại diện chain GMS.
   Lưu bản raw riêng; báo cáo công khai chỉ ghi kết luận đã lược định danh.
2. **Xác minh đầy đủ scope GMS nếu còn nghi hook.** Đọc compiled
   `PlayIntegrityHooks.setProps/spoofBuildGMS` và control hiện có. Nếu cần
   runtime instrumentation, thêm diagnostic readback có giới hạn vào chính
   helper của fork, ghi tên process/giá trị field đã áp, không ghi token,
   account, nonce hoặc attestation ID. Giữ exact-preimage/compiled gate,
   không mở allowlist ra mọi process. ON/OFF và app thường phải là controls.
   Các file dự kiến nếu log chứng minh cần sửa:
   `rom/unica/mods/pif/a376b_android17_pif.py`, helper patch PIF và verifier
   hiện có; không tạo build system mới.
3. **Chỉ sửa nguyên nhân có thể kiểm được.** Nếu có lỗi field/profile thực,
   sửa đúng method và replay/build/test. Nếu chain/profile hợp lệ về cấu trúc
   nhưng Google vẫn không cấp nhãn, ghi giới hạn chứng nhận/attestation của
   port; không đổi fingerprint ngẫu nhiên hoặc build lại cùng cấu hình với
   lời hứa PASS. Không dùng keybox/chứng chỉ giả, không fake kết quả checker.

Chưa có log nào trong lượt này yêu cầu đổi kernel, Hybrid Vendor, radio, hoặc
quy tắc ADB/Auto Blocker. Giữ NFC/Bixby compatibility theo quyết định hiện hành.
DEVICE/STRONG không được hứa từ model hiển thị hoặc công tắc PIF.

## Bằng chứng và tài liệu thay đổi

Bằng chứng riêng: `logs/20261101-m4-r5-post-setup/` (thư mục0700, file0600),
gồm request logs, process samples, screenshot và kết quả switch test. Không
đẩy raw screenshot/nonce/TestId/account lên GitHub. Hash ROM r5 giữ nguyên như
[báo cáo build/flash](m4-r5-local-repair.md).

Lý do cập nhật: đóng các phép thử chờ Setup Wizard và ghi đúng r5 thay vì suy
từ r4. File ảnh hưởng: báo cáo này, `docs/m4-r5-local-repair.md`, `docs/PLAN.md`,
`docs/bugs.md`, `README.md`. Kiểm chứng: đối chiếu4 thời điểm request, screenshot
ON/OFF, file kết quả Settings và live final-state; không có patch runtime mới.
