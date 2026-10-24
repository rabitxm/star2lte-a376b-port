# M2 — A37 Android 16 / One UI 8.5

Ngày 21/10/2026: người dùng yêu cầu chuyển sang build M2 trên PC và giữ gói
M1/M2/M4/M5 không ghi đè. Gate boot M1 F đã đạt; M2 chưa build/flash.
Chỉ có A37 Android 17 đã kiểm trong dự án, không dùng gói đó thay đầu vào M2.

## Nguồn đầu vào (đã nhận, xem kết quả bên dưới)

Full Files **SM-A376B / XXV / A376BXXS5AZH4**, khoảng 8,4 GB:
https://samfw.com/firmware/SM-A376B/XXV/A376BXXS5AZH4 .
MD5 do trang liệt kê: `7ffba2650a25ae900424826b8dc997d6`.
Đây là giá trị kỳ vọng từ trang, chưa phải hash file đã tải.
Samsung update note liệt kê build này là Android 16:
https://doc.samsungmobile.com/SM-A376B/038051260423/eng.html .
SamFW ghi One UI 8.5; tên mốc cũ ghi One UI 8. Phải đọc giá trị thực từ payload,
ghi đúng phiên bản, không tự giả định 8.0 hoặc đổi property để khớp tên mốc.

Đường tự tải SamFW dừng ở `Calling Heimdall...`, chưa tạo file tải; ảnh
`logs/20261021-m2-inputs/samfw-waiting.png`. Hai yêu cầu FUS lịch sử dùng identity
rỗng trả `"status": "408"`, `"fields": {}`; không đọc/gửi định danh điện thoại.
Không suy từ đó firmware không tồn tại. User đã trả lời sẽ tải Full Files và
báo khi xong; không cần hỏi lại. Giữ nguyên ZIP trong thư mục dự án.

## Sau khi file có mặt

1. Xác định đúng file thực, kiểm size/MD5/SHA256/CRC và nội dung Odin; giữ gói
   Android 17 riêng. Lock từng file đã xác minh. Không flash BL/CP của A37 lên S9+.
2. Trích qua script của fork sau khi đọc nhánh áp dụng; kiểm SDK36, One UI thực,
   framework/APEX và feature nguồn. Chỉ thêm donor/profile A376B song song S22
   khi có dữ liệu thực; không bịa SOURCE_* hoặc sửa cấu hình S22 thành A37.
3. Audit provenance system cuối: không để bước baseline của fork thay framework
   A37 bằng S22 rồi chỉ đổi model/property. Hàm
   `_EXYNOS9810_RESTORE_BOOTING_SYSTEM_CORE` có definition nhưng chưa thấy call
   trong lần đọc hiện tại; không kết luận nó đang chạy khi chưa có evidence.
4. Giữ Hybrid Vendor VNDK33, nguồn blob F đã kiểm và radio bridge khóa ABI/hash;
   N dùng blob/cấu hình N riêng khi có đầu vào xác minh. Giữ DS-ACK May nguyên
   bản không KernelSU. M2 chỉ thay donor Android16 so với M1.
5. Chạy compiled defaults/ADB guard trên chính donor mới; dừng nếu code layout
   khác. Kiểm firmware provenance, VINTF/ABI, installer, model, CRC/chữ ký và
   kernel hash trước khi coi ROM hoàn chỉnh. Tương thích runtime cần log thật.
6. Package với `ROM_MILESTONE=M2`, lưu run riêng theo release-retention.md.
   M2/M4/M5 hiện chưa có ROM hoàn chỉnh; thư mục dự phòng không phải artifact.

Phạm vi hiện tại là chuẩn bị/build trên PC. Quyền flash M1 F trước đây không
áp dụng M2 hoặc G965N; chưa thực hiện thao tác thiết bị ở lượt này.

## Kết quả nhập và build hiện tại

Đã nhận file ở Downloads và xác minh full/Odin/extraction; không cần tải lại
hoặc trích lại. Manifest và CRC607 ở logs/20261021-m2-inputs. Actual system
OneUI80500/SDK36.1. Profile M2 F: target/star2lte/profiles/
m2-g965f-a376b-android16-no-ksu.sh. Source config song song essi_a376b_android16.sh;
M1 profile reset selector, donor/version tham gia cache. Patch20–22/replay PASS.
Build sạch fork bắt đầu với make_rom.sh --clean-work-dir -z --fs-type ext4,
log logs/20261021-m2-build/attempt1.txt. Không dùng package-staged/cache M1.
M1 archive vẫn nguyên, M2 chưa có full ZIP hoàn chỉnh.

Source flags đọc từ XML, props và decoded classes. Bluetooth legacy source flags
chưa có bằng chứng và không được recipe dùng: none + future-consumer guard.
Wi-Fi LOWLATENCY/TWT ở AZH4 không còn static entries2/3 trong default map;
source config mô tả static fallbackfalse, runtime HAL detection vẫn phải audit.
MIMO giữ antenna-count query, không tuyên bố anten/phần cứng đã nghiệm thu.
Các app/MDF restore S22 còn trong fork được đánh dấu provenance review trước
package; không được biến A37 framework thành S22 để vượt verifier.

ADB patch0021 đã review layout p1 của A37. Fixture compiled classes PASS M1/M2,
nhưng full services DEX041 rebuild/compiled release checks là gate riêng, chưa
được thay bằng fixture039. Không hạ minSdk/SDK label để che lỗi tool.
Các bước tiếp theo: xử lý build lỗi có exactlog, kiểm output donor/ABI/ADB,
package M2 riêng theo retention và audit; không tự flash.

## Lượt build và lỗi đầu tiên

Attempt1 dừng trước package ở MultiMicController.prepare(IIII)V; A376B có
prepare(IIIIZ)V. Patch0023 giữ body và feature refs donor, chỉ guard manager
null; fixtures S22/A376B source+compiled PASS, replay23patch/34file PASS.
Attempt2 đang chạy clean work_dir với cùng donor/kernel/vendor contract.
Full services.jar donor đã thử rebuild riêng với ADB patch, minSdk35 giữ nguyên,
compiled lifecycle đọc được; evidence services-full-rebuild.json. Chưa thay
bằng chứng này cho full ROM compiled verification/boot.

Attempt2 qua camera prepare nhưng dừng optional classifier claim (attempt2:905).
Patch0024 giới hạn claim theo implementation AZH4 thực, không gán source flag
hoặc ghép native khác. Optional action classification chưa hỗ trợ. Five fixtures
và replay24/35 PASS; attempt3 clean đang chạy. Bluetooth native patterns/lens
selector có warnings cần runtime, chi tiết docs/bugs.md. AutoBlocker/SUW source
key tables đã đối chiếu trong policy-bindings-review.json; vẫn cần compiled/
fresh runtime trên chính output M2, chưa có full ZIP.

Attempt3 dừng tại attempt3.txt:914 vì [None] source và target rỗng cho Gallery Pet Cluster; patch0025 coi None/absent là disabled và giữ guard nếu target đòi version chưa có. Attempt4 qua Pet Cluster và dừng tại attempt4.txt với HEX_PATCH báo không tìm thấy /system/system/lib/libstagefright.so. Patch0026 loại bỏ claim ARDoodle legacy không có implementation ở AZH4; patch0027 đã thêm guard ABI stagefright hẹp theo firmware A376B XXV LOCAL SDK36. Evidence pet-cluster-fixtures.json, ardoodle-claim-fixtures.json, stagefright-abi-fixtures.json; replay local patches 27/35 PASS. Attempt5 đang chạy clean. Chưa có ZIP M2, compiled whole-ROM ADB gate, hay runtime.

Attempt5 passed the camera block, patched the existing 64-bit stagefright library, and then stopped at attempt5.txt:941 because DVFS referenced missing smali/U1/w.smali. The decoded AZH4 SamsungDeviceHealthManagerService class matching the fallback is smali/o5/w.smali; patch0028 selects it only for A376B XXV LOCAL SDK36. Evidence dvfs-obfuscation-review.json; replay 28 patches/36 files PASS. Attempt6 is a clean build; still no M2 ZIP.

Attempt6 đã qua DVFS, xác nhận log thay class `smali/o5/w.smali` thành
`ssrm_default`, rồi dừng ở legacy Face HAL bridge. Patch HIDL2.0 của fork lệch
nhiều hunk trong A37 services.jar và thiếu synthetic lambda; xem attempt6.txt
lines 976–1032, face-a37-compat-review.json. Người dùng đang chọn port bridge
để giữ Face Unlock hay tạm bỏ Face Unlock khỏi M2. Chưa tạo ZIP.

Attempt7: người dùng chọn tạm bỏ Face Unlock. Patch0029 xóa khai báo Face HAL
trong manifest/init, giữ permission XML của Android để FaceManager còn hiện
diện, đồng thời bỏ qua hai hook bridge Exynos9810. Attempt7 vẫn lỗi vì hook
phía sau chưa được điều kiện hóa; log có `Failed to patch legacy Exynos9810
face method "getDeclaredInstances()"`. Patch0029 sau đó được sửa để gate các
hook và FaceunlockFix theo profile flag. Attempt8 xác nhận hai dòng skip và
ba khai báo HAL bị gỡ, rồi lỗi ở legacy SehLights backport. Lỗi chính xác tại
attempt8.txt:1104-1105 (`LightsService$LightImpl.smali`, patch does not apply)
và :1132-1133 (`PowerManagerUtil.smali`, patch does not apply). Smali donor
AZH4 đổi control-flow label/register layout; đây là bất tương thích patch
thật, không liên quan Face Unlock/ADB. Không tạo được ZIP M2. Tiếp theo phải
điều chỉnh backport SehLights hiện có cho AZH4 và thêm fixture/replay trước
lượt clean build kế tiếp.

## A376B SehLights AZH4 compatibility — patch0030

The AZH4 donor uses `LightsService$VintfHalCache`; the upstream legacy SehLights
backport expects `Supplier` fields and fails at attempt8 lines 1104–1105 and
1132–1133. Patch0030 adds an A376B Android16-only path in
`unica/patches/legacy/customize.sh`, a focused backlight patch, and
`patch_a376b_sehlights.py`. It reuses the fork's existing backport method body,
adapts cache calls to the donor's `VintfHalCache`, preserves the historical
four-argument `setLightLocked` and two-argument proxy call surfaces, forwards
the extension parameter, and sets the legacy extension feature flag. Other
donors continue through the existing upstream patch unchanged.

Verification: the actual AZH4 `services.jar` decoded with Apktool v3.0.2-SNAPSHOT;
the guarded adapter passed its strict donor-layout checks; the backlight patch
applied; the edited jar assembled successfully with Apktool. Evidence and
hashes are in `logs/20261021-m2-build/sehlights-a376b-preflight.txt`. Shell
syntax, patch-layout, reverse applicability and `sources.lock.json` hash checks
also pass. Clean full-ROM build9 is running. Compiled whole-ROM ADB verification,
package audit and hardware light/boot behavior remain pending.


## M2 attempt10 — MotionPhoto API mismatch

Attempt10 passed the A376B-specific SehLights step and Face omission, then failed at `attempt10.txt:1000`: legacy `VideoTranscoder.configVideoEncoderParameters(...)` is absent from AZH4. The decoded donor retains `EncoderFilter.configCodec(...)` and the `0x7f420888` source constant in MotionPhoto and SamsungCamera. Patch0031 guards only the missing legacy VideoTranscoder edit for `essi_a376b_android16`; the donor app and supported EncoderFilter patches remain. Next: run method/layout preflight, shell and patch replay checks, then a clean build11. No M2 ZIP or device test yet.

## M2 attempt11 — stagefright OMX blocker

Patch0031 đã vượt lỗi MotionPhoto; attempt11 dừng ở `attempt11.txt:1003-1005`:
`system/system/lib/libstagefright.so` không có trong donor AZH4, sau đó fork báo
`No known patch available for the supplied libstagefright.so`. AZH4 có duy nhất
`lib64/libstagefright.so`, nhưng SHA256
`1567ef98eb1ee090c14289c6a1b8cf87e9078a8c5a67495178874e3f998db2e5` không khớp
hai mẫu legacy. G965F target vendor dùng OMX video codecs. Không tiếp tục bằng
cách dùng thư viện hệ thống Android10 hoặc đoán byte mới. Bằng chứng chi tiết:
`logs/20261021-m2-build/media-stagefright-a376b-review.json`. M2 được tiếp tục theo lựa chọn người dùng: port theo disassembly. Patch0032 xác định qua `.rela.dyn` rằng GOT slot `0x2639a0` trỏ tới `MEDIA_MIMETYPE_VIDEO_HEVC`; trong `ACodec::setupVideoEncoder`, AZH4 `0xe5654` có `cbz w0, 0xe6004` là nhánh thoát sớm HEVC tương đương nhánh S22 mà fork NOP. Patch0032 chỉ áp dụng cho profile `essi_a376b_android16`, NOP đúng lệnh đó, và fail-closed nếu layout 32-bit bất ngờ xuất hiện. File ảnh hưởng: `rom/unica/patches/legacy/customize.sh`; audit patch `patches/fork/0032-port-a376b-stagefright-hevc-branch.patch`; hash khóa ở `sources.lock.json`. Đã xác minh signature little-endian khớp đúng một lần, relocation/mime, `bash -n`, và reverse dry-run trong fork. Kế tiếp chạy clean build12, audit lib disassembly đầu ra; chức năng OMX/camera vẫn cần log/runtime trên máy sau này. Chưa có ZIP M2/flash/runtime test.


## M2 attempt12 — Stagefright port passed, GlobalPostProcMgr register mismatch

Build12 log confirms patch0032 applied exact bytes in `/system/system/lib64/libstagefright.so`; it then stopped at `customize.sh:719`: `Failed to replace value "const v3, 0x7f420888"` in GlobalPostProcMgr EncoderFilter.configCodec. AZH4 decoded source uses `const v4, 0x7f420888` at line 1908 and passes that register as the color-format default. Patch0033 selects v4 only for the exact A376B Android16 profile; other donors retain v3. Next: clean build13 and verify assembled APK plus patched native disassembly. No M2 ZIP/runtime.


## M2 attempt13 — absent optional vexfwk_service

Patch0033 passed: attempt13 log shows MotionPhoto, GlobalPostProcMgr, and SamsungCamera EncoderFilter constants all replaced successfully; Stagefright patch also ran. The next error is `attempt13.txt:1007`: `File not found: /system/system/priv-app/vexfwk_service/vexfwk_service.apk`. The extracted AZH4 donor tree contains no such APK. Patch0034 guards this optional target only for the exact A376B16 profile and logs when missing; other donors retain the strict patch. Next: clean build14.


## M2 attempt14 — vendor mismatch warning patch shape

Build14 passed the `vexfwk_service` absence guard and all earlier media operations, then failed while applying the common `Disable-vendor-mismatch-warning.patch`: `attempt14.txt:1035-1061` identifies mismatches in `ActivityManagerService$$ExternalSyntheticLambda6` and `ActivityTaskManagerService$LocalService$$ExternalSyntheticLambda4`. AZH4 instead contains fingerprint warning in `ActivityManagerService$$ExternalSyntheticLambda8`; task dialog lambda layout has different class IDs. Patch0035 skips only this optional warning patch for exact A376B Android16, preserving donor behavior; other donors unchanged. Next clean build15.


## M2 attempt15 — optional Bixby patch layout mismatch

Patch0035 correctly skipped the unrelated vendor-mismatch warning patch and build15 passed all camera encoder operations, including the AZH4 Stagefright and GlobalPostProcMgr adaptations. It then failed at `attempt15.txt:1038-1044` applying `0002-Add-UN1CA-Bixby-key-remap.patch`: expected KeyEvent register context is absent at PhoneWindowManager.smali:11681, where AZH4 uses a KeyGestureEvent path. Patch0036 skips this optional key-remap patch for A376B16 only until event dispatch is reviewed; other donors unchanged. Next clean build16.


## M2 attempt16 — missing HDM constructor target

Patch0035/0036 now skip only the mismatched UI warning and Bixby patches as intended. Attempt16 failed at `attempt16.txt:1043`: replacement of source product API value `36` in `HdmVendorController.<init>()V` was not found. The decoded AZH4 class has no `<init>` method; the two prior HDM class edits succeeded. Patch0037 skips only this absent method substitution for A376B16 while retaining other product API patches. Next clean build17.


## M2 attempt17 — absent Knox TAProxy class

Patch0037 skipped the inapplicable HDM constructor edit and the build then failed at `attempt17.txt:1045`: `Smali not found: .../knox/dar/ddar/ta/TAProxy.smali`. AZH4 services.jar has no TAProxy class. Patch0038 guards only this absent-class edit for A376B16; other MAINLINE_API_LEVEL substitutions remain active. Next clean build18.


## M2 attempt18 — CoreRune resolution patch layout

Build18 passed Product Feature edits after patch0037/0038, then failed applying `0001-Enable-FW_DYNAMIC_RESOLUTION_CONTROL.patch`: `attempt18.txt:1087-1097` shows the S22 context around FW_MULTI_RESOLUTION_POLICY does not match A376B. AZH4 assigns FW_DYNAMIC_RESOLUTION_CONTROL=false in CoreRune; target config enables it and generated floating feature lists `WQHD,FHD,HD`. AZH4's two VRR resolution-policy flags are already false. Patch0039 intercepts only this resolution patch for the exact A376B16 profile and changes the dynamic flag to true, preserving VRR false values. Next clean build19; inspect compiled smali/APK for all three values.


## M2 attempt19 — resolution hook location correction

Attempt19 reproduced the attempt18 failure at `attempt19.txt:1087-1097`. The initial patch0039 dispatch guard was placed in `APPLY_SMALI_PATCHES`, but the resolution module calls `APPLY_PATCH` directly from `product_feature/customize.sh`, so the guard did not intercept it. Patch0039 is corrected to replace the actual module call for AZH4 and directly set the target CoreRune flag; a clean build20 is running next.


## M2 attempt20 — dynamic-resolution framework port passed; GameManager class moved

Attempt20 log confirms the corrected patch0039 route worked: `Porting CoreRune dynamic-resolution flag...` and `FW_DYNAMIC_RESOLUTION_CONTROL` changed `v4` to `v3`. It then failed at `attempt20.txt:1090`: the old GameManager patch cannot find `GameManagerService$DisplayListener.smali`. AZH4 moved the equivalent check to `GameDisplayListener.smali` constructor; it compares `WQHD,FHD,HD` to an empty string and sets `MULTI_RESOLUTION_SUPPORTED`. Patch0040 ports that same comparison to the AZH4 class only. Next clean build21, inspect final smali/APK.


## M2 attempt21 — GameDisplayListener source-string line replacement

Attempt21 confirms CoreRune dynamic resolution patch passed. Patch0040 reached the AZH4 `GameDisplayListener` constructor and `SMALI_PATCH` found the method, but failed because the helper treats empty-string source values as line substitutions: `attempt21.txt:1086` says it could not replace `const-string v1, ""`. Patch0040 now uses its line-replacement mode and preserves indentation. Next clean build22.


## Continuation audit after interrupted attempt22 — 21/10/2026

Attempt22 log stops while decoding SamsungCamera; the process no longer exists and no M2 artifact was produced. Do not count it as a build pass or a new error result. Before another whole-ROM run, a static audit resolved the source-replay problem: patch0030 had an incorrect hunk count and invalid zero-to-zero index headers on two new files. The patch record was corrected without changing recipe/Smali implementation, its lock SHA refreshed, and replay now passes all 40 patches / 40 byte-identical source files (`logs/20261021-m2-inputs/patch40-replay.json`). Shell syntax and the fork's 154-patch layout check pass.

The Stagefright port has before/after disassembly and a four-byte-only difference proof in `logs/20261021-m2-build/stagefright-disassembly/`. The AZH4 branch at VA/file offset `0xe5654` compares HEVC through GOT relocation `0x2639a0`; its taken destination logs `Do not enable feature for HDR10Plus recording` and enters a CHECK(false) path. The NOP mirrors the fork's S22 branch adjustment. It is a narrow assertion-path adjustment; no camera/HEVC functionality is certified by this static audit.

Patch0040's corrected line mode was executed using the fork's actual SMALI_PATCH helper on an independently decoded AZH4 gamemanager.jar. Apktool assembly passes; source/rebuilt SHA and logs are in `continuation-static-audit.json`. This validates the helper correction that attempt22 never reached.

A preflight of the next Settings resolution patch (`resolution/SecSettings.apk/0001-Enable-dynamic-resolution-control.patch`, 57 files, 330602 bytes) against the independently decoded AZH4 SecSettings reports 63 error lines: missing old dex/class paths and incompatible hunks. Log: `resolution-settings-preflight.txt`. AZH4 has a changed resolution UI/controller path and uses Display.Mode APIs; enabling only a feature flag is insufficient to claim legacy S9 WQHD/FHD/HD works. A user question is pending: fully port resolution for M2 or temporarily use a fixed resolution for the boot test. No fixed-resolution fallback has been applied.

The full compiled ADB/Auto Blocker verifier, package audit and hardware/runtime tests remain pending. Review also found that the current release-default verifier assumes FaceunlockFix/Bixby inclusion; it must be reconciled with explicit M2 feature decisions without weakening the mandatory compiled ADB check. This is a source-review finding, not a build/runtime failure log.


## M2 fixed resolution selected — 21/10/2026

User selected temporary fixed resolution for the M2 boot test. Patch0041 sets the existing dynamic-resolution target flag false only for the M2/A376B Android16 profile; no generic resolution/Settings/SystemUI switcher patches run in that profile. The fork's native star2lte baseline is 1440x2960 with ro.sf.lcd_density=560 and ro.sf.init.lcd_density=560; it remains selected without forced-size boot scripts. Clear the inherited WQHD/FHD/HD feature claim early and in both final system/vendor floating_feature.xml files; final audit rejects a nonempty switcher claim. The donor Settings screen-resolution controller remains unavailable; actual panel resolution/visibility requires runtime verification.

Scope: target/profile files and existing device-stack feature edits/audit only. M1 remains dynamic; wrong-donor/milestone fixed mode is rejected. Config fixtures pass M1=true, M2=false and invalid profile rejection. All 41 source patches replay to 40 byte-identical files; shell syntax passes. Mandatory ADB defaults/compiled code checks stay required. Next clean build23 with full ROM packaging requested through the existing fork; no device actions.


## Product-feature continuation after fixed-resolution selection — 21/10/2026

Clean attempt23 skipped the dynamic-resolution recipe as selected, passed camera/legacy steps and stopped at `attempt23.txt:1053`: `Missing SPF patches for condition (SOURCE_COMMON_SUPPORT_HDR_EFFECT: [false], TARGET_COMMON_SUPPORT_HDR_EFFECT: [true]). Aborting`. No ZIP was produced. Subsequent `product-tail-*.sh` runs are diagnostic slices of the existing recipe on the failed work tree, not a packaging entry or completed build. A clean full build remains mandatory.

- Patch0042 resolves only unique unchanged fully qualified A376B class descriptors when their DEX folder moved; ambiguity/missing classes remain fatal. It enables the two existing HDR Settings controllers for the target DisplaySolution/mDNIe stack. Actual helper unique/ambiguous/missing/other-donor fixtures pass. HDR operation is unverified at runtime.
- Patch0043 ports Samsung rear position=3 / capacitive type=1 in framework and optical/in-display flags in services/SystemUI. The fork's hardware mapper already converts Samsung rear to Android TYPE_REAR=1. Device-stack BiometricSetting is the pinned compatibility APK, not the donor APK: SHA256 `bc64ca2e5083ae651a57e523eb68496ccf6f9dcaeabfed32c3adc1998acb877c`. Preserve its feature-driven rear/display/optical paths and clear compiled SIDE=true. Validate its Context constructor rather than apply the absent donor constructor string edit. Five classes changed in preflight; unknown shape fails before any writes. Native fingerprint HAL/blobs remain untouched; enrollment/unlock need runtime logs.
- Patch0044 clears AZH4 brightness thresholds `89,91` / `260,2500`, uses `dumpProductFeature`, and patches VrrManager's actual constructor for mode=0. The existing target default remains60Hz; all HFR recipe edits pass diagnostic execution. No high-refresh or resolution-switching claim.
- Patch0045 restores three hardware mDNIe method bodies and color-table helper from the fork's existing reviewed diff (SHA checked), adds matching curtain state and accessibility flags. Inlines pure formatting rather than borrow changed synthetic method IDs. Donor stub hashes and exact remaining helper signatures are checked before writes. Local actual-tree preflight passes; hardware color functions/SELinux/sysfs need runtime logs.
- Patch0046 accepts an exact byte-identical generated constant-return method on A376B16 after all normal method/type/value checks, fixing already-false StrongBox handling. Other donors retain strict no-change failure.
- Patch0047 empties the verified custom Wi-Fi backoff placeholder in field/log/constructor, so the existing TextUtils.isEmpty branch skips A37 RF tables. It does not change RF numeric values or native target driver/blob.
- Patch0048 makes the user-selected Face omission consistent after late vendor restores and in the release verifier. It rejects restored Face HAL/init/bridge claims, keeps framework FaceManager feature XML, resets M1/M2 flags, and leaves compiled ADB/defaults and other release checks required. Fixtures and actual incomplete-tree omission preflight pass.

Current blocker: Wi-Fi80211AX UI patches are old-donor diffs. `product-tail-wifi.txt:8` reports `.../WifiSlice.smali: No such file or directory`. Even after exact descriptor relocation, Settings resolves8/9 files but has16 error lines (one lambda absent); SystemUI resolves3/3 but has6 error lines. Evidence `wifi-ax-review/preflight.json` and `.check.txt`. A preference question is pending: full UI adaptation now versus temporary donor Wi-Fi UI for M2 boot test, with possible Wi-Fi6 labels on scans despite S9+ Wi-Fi5 hardware. No bypass has been applied.

All48 audit patches replay to44 byte-identical source files; layout154 passes. APK/JAR assembly and compiled ADB recheck are being executed separately below; this does not authorize packaging the incomplete tree. No M2 phone actions; M1 archive preserved.


Checkpoint0048 verification completed: all six actual-tree artifacts (framework.jar, services.jar, BiometricSetting, SettingsProvider, SecSettings, SystemUI) rebuild through the fork's Apktool pipeline with exit0. After applying the existing ADB defaults recipe to this diagnostic tree and rebuilding services/SettingsProvider, the verifier reports `PASS compiled ADB defaults, no-auth properties and stored-choice lifecycle`. Compiled rear-FP constants and actual restored mDNIe methods pass independent disassembly checks; standard Face variant audit passes. These are diagnostic-tree static results, not a clean whole-ROM release pass or runtime result. Evidence `product-feature-checkpoint48.json`, `compiled-hardware-preflight.json`, `adb-current-compiled-verify.txt`, `face-current-compiled-audit.txt`. All10 retained M1 manifest files still exist with original sizes; no archive writes. Current Wi-Fi UI question remains pending; no further clean build/ZIP/flash.


## M2 donor Wi-Fi UI selected — 21/10/2026

User explicitly selected temporary donor Wi-Fi UI for M2. Patch0049 adds `STAR2LTE_M2_KEEP_DONOR_WIFI_UI=true` in M2 and resets it false in M1; the existing product_feature recipe validates M2/A376B16/star2lte/Wi-Fi5 scope and skips only the two old 80211AX UI diffs for Settings/SystemUI. It leaves source metadata, target Wi-Fi flags, driver/HAL/blob, custom-backoff adaptation and all other Wi-Fi recipe branches intact. Donor scan labels/icons may still show Wi-Fi6; that is a known temporary UI limitation, not S9+ hardware support. No extra UI bypasses are authorized by this selection.

Actual branch fixtures PASS selected0patch calls / strict2calls / wrong-donor and wrong-milestone rejection; M2-to-M1 flag reset PASS. All49 audit patches replay to44 byte-identical source files. Clean attempt24 is running through `make_rom.sh --clean-work-dir --build-rom-zip`, log `logs/20261021-m2-build/attempt24.txt`. Full release gates, retained run packaging and M2 runtime remain pending. No phone actions; pinned kernel/Hybrid Vendor/radio unchanged.


## M2 continuation after Wi-Fi UI decision — patches0050–0053

Attempt24 clean stopped at `attempt24.txt` with `Method "getMainInstance()Lcom/samsung/android/hardware/display/RefreshRateConfig;" not found ... SemInputFeatures.smali`. This was a local0044 regression: its METHOD loop variable leaked into SMALI_PATCH replaceall's method check. Patch0050 renames/unsets the loop selector and checks methods only for operations with a method argument; field-only replaceall with an unrelated caller METHOD passes, missing real methods still fail.

Diagnostic continuation then logged the user-selected donor Wi-Fi UI and stopped at `product-tail-after-input.txt:94-95`, `SemWifiInjector.smali:1641: patch does not apply`. Patch0051 ports the existing target APE/personalization=false behavior on actual AZH4: removes only the SHA-checked APE construction region, verifies no external label references, keeps null guards, disables the exact L4s default, disables the Qbox APE observer, and splits only SUPPORT_APE ordinal35 from a shared SQE branch. The moved Settings develop/ApePreferenceController is disabled and its reset seed becomes0; old ape map entries are verified absent. No scan icon/label changes. Four classes change; unknown enum fails before writes. Native/Settings runtime needs logs.

Patch0052 ports SoftAP target differences after old AX/country/power/sharing diffs fail. The AX feature1 case uses AZH4's existing public driver query, with null fallbackfalse. Country fallback applies only for missing/NA HAL values and preserves permission enforcement, valid HAL results and Binder identity cleanup on success/exception. Power support reads the driver string instead of compiledtrue; worker initialization is disabled as in the original fork, and only SQE power command ordinal29 is split to unsupported. Sharing-lite is conditional on sharing and retains carrier/model exclusions. Strict method hashes and command mappings are checked; unused callback classes retained without synthetic-ID guessing. Country/power/sharing fixtures and no-partial-write rejection pass. Actual product-feature diagnostic remainder passes. Separate jar assembly succeeds; actual-work-tree jar rebuild is also being checked. Runtime remains open.

SAIV then stopped at `remaining-modules-preflight.txt:5` missing `vendor/etc/midas/moire_detection/moire_detection.tflite`; A376B stores the identical model under VslMesDetector, SHA256 `88e001d4007446f82bd999d71d358b6f48ae976c0cd38128fcfc77a4ec9060c1`, which matches the older model exactly. Patch0053 verifies/copies the A376B bytes to the path still referenced by donor Midas config, using explicit parent/file metadata. A later missing generic hs_segmenter.info is addressed by validating/copying AZH4's intact two named info/model pairs. SAIV diagnostic preflight now passes; PhotoEditor runtime still needs logs.

All53 patches replay to47 byte-identical source files. Diagnostic continuations use the existing fork module/helper entry points and do not mark .completed or package the partial tree. After remaining module preflights, a fresh full build is still required. Mandatory ADB defaults/compiled audit, pinned duhansysl kernel and Hybrid Vendor/radio remain required. No phone actions or completed M2 ZIP; M1 archive untouched.

## M2 module compatibility continuation — 21/10/2026

Patch0056 preserves AZH4 PackageBlockListPolicy because the old optional blocklist mod changes LDU state and lifecycle wiring (`mods2-preflight.txt:75,136`). Patch0057/0058 skip the optional UN1CA Updates suggestion shortcut because AZH4 has no `Settings$FOTASuggestionActivity` (`attempt25/26.txt:1230`); both customize and duplicate Smali dispatcher paths are covered. Patch0059 retains donor behavior for real-time network-speed Settings because three old classes are missing (`attempt27.txt:1237–1241`). Each patch has its reason/files/check in its header and patch-matrix; series replay now covers59 patches/52 files.

Clean attempt25/26 stopped at the stale suggestion hook; attempt27 passed it and blocklist skip, then stopped at the real-time network speed patch. Attempt28 is the clean build with0059 and must complete Apktool rebuild, compiled ADB defaults, Hybrid Vendor VNDK33, pinned DS-ACK artifact checks, release gates and unique M2 package publication before declaring a complete artifact. No M2 phone action is authorized. M1 archive remains retained.

Patch0059 keeps the donor network-speed implementation after attempt27.txt:1237–1241 showed three absent CSC classes. Patch0060 keeps the donor power-off lock implementation after attempt28.txt:1238 showed its controller absent. The profile-specific dispatcher guards preserve upstream patches for other donors. Replay60 passes; clean attempt29 is running.

Patch0061 keeps the AZH4 donor Secure Folder settings after attempt29.txt:1239,1241 showed the old quick-switch hook classes are absent. Series replay61 passes; clean attempt30 is running. This optional CSC feature remains donor behavior pending runtime review.

Patch0062 preserves AZH4 donor Smart View tile behavior after attempt30.txt:1261 showed the old QpRune context differs. Replay62 passes; clean attempt31 is running. This is a CSC UI limitation, not a Wi-Fi HAL change.

Patch0063 retains the AZH4 Dual Messenger allowlist after attempt31.txt:1236–1454 showed the old allow-all patch no longer matches four DAAgent classes. Replay63 passes; clean attempt32 running.

Patch0064 keeps the rest of Nuke Knox while skipping its absent TAProxy shipping-API substitution on AZH4 (`attempt32.txt`, deknox/customize.sh:103). Replay64 passes; clean attempt33 running.

Patch0065 preserves AZH4's complete Knox stack because the Nuke Knox module has multiple nonmatching framework hunks (`attempt33.txt:1437–1480`). This supersedes the narrower0064 guard for M2 dispatch while retaining its documented absent-class handling for other execution paths. Replay65 passes; clean attempt34 running.

Patch0066 retains AZH4 donor Hide My Applist behavior after its package visibility patches failed in ComputerEngine/ActivityStarter (`attempt34.txt:1281–1295`). Replay66 passes; clean attempt35 running.

Patch0067 retains AZH4 donor key-attestation behavior after the optional keybox module failed on missing keystore2/BouncyCastle classes (`attempt35.txt:1256–1260`). Replay67 passes; clean attempt36 running.

Patch0068 retains AZH4 donor KnoxPatch/attestation behavior for M2. Attempt36 showed stale integrity-attestation call sites across DevicePolicyManagerService, DarManagerService, UserManagerService, AttestParameterSpec, AttestationUtils and SemSsdidManagerService (`attempt36.txt:1259–1260,1265–1310,1311–1348,1349–1388`). The whole optional KnoxPatch module is skipped to avoid a partial integrity bypass; M2 does not include the fork's KnoxPatch spoof/bypass features. Replay68 and shell syntax pass. Starting clean attempt37; no phone action.

Attempt37 passed the KnoxPatch guard, then stopped in the optional Paradigm adaptive-colour-tone feature: AZH4 display-service hunks failed (`attempt37.txt:1261–1340`, including `SemMdnieManagerService.smali:58,101,103`). Patch0069 skips only that feature's framework/Settings/provider/SystemUI/resource changes for M2 and retains AZH4 donor display behavior. Replay69 and shell syntax pass; starting clean attempt38.

Attempt38 was interrupted while the clean build was decoding SamsungCamera (`attempt38.txt:62` is the last emitted line); the process/session ended without a build result or diagnostic. No artifact was published. Retrying as fresh clean attempt39.

Attempt39 passed the display-feature guard but failed later because the generic Smali walker still decoded the omitted `EnvironmentAdaptiveDisplay.apk` (`attempt39.txt:1333: File not found: /system/system/priv-app/EnvironmentAdaptiveDisplay/EnvironmentAdaptiveDisplay.apk`). Patch0070 adds an exact M2/APK path skip in `scripts/internal/apply_modules.sh`; replay70 passes. Starting clean attempt40.

22/10/2026 continuation: attempt40 has no final outcome and no live process; its log ends during the Smart Suggestions download. Started attempt41 using the same existing fork clean-build command under a detached process so chat interruption cannot terminate it. PID and final exit code are recorded separately (`attempt41.pid`, `attempt41.exit`). This changes only process lifetime; no build-system code or phone action.

## 22/10/2026 — clean attempt41 and batch module preflight

Attempt41 ran to completion (exit1, 9min3sec) without chat interruption. It passed the0068–0070 routes and stopped at Settings' inlined constructor (`attempt41.txt:1489`). Patch0071 preserves all211 AZH4 search registrations and adds four UN1CA providers via a static utility; exact four-class hashes, atomic unknown-input rejection and standalone DEX assembly PASS. Settings tail including TopLevelKeysCollector adaptation passes.

Batch preflight finds optional Settings/tweaks differences before another clean rebuild. Patch0072 ports two Separate app sound five-app gates while preserving saving/unlink/footer logic; actual patch and DEX assembly PASS. Patch0073 ports two hash-checked Setup Wizard navigation methods; actual edits and DEX assembly PASS. Patch0074 retains the donor disclaimer because the optional custom branding patch no longer matches. Patch0075 temporarily omits five incompatible optional feature patches and removes their five Extra Settings switches; supported SamsungProps/FloatingFeature/HideDeveloperStatus/ASKS hooks pass. Patch0076 retains donor QP text scrolling and sensor-toggle confirmation while the compatible stepX and already-applied haptics remain. postsetup76 completes tweaks/SystemUI, wallpaper and Weather diagnostics.

Replay76 passes for58 files. These diagnostics modify the incomplete staging tree only; they do not mark `.completed` or publish any ROM. Mandatory compiled ADB/Auto Blocker, pinned non-KSU DS-ACK, VNDK33/radio gates and a new clean build are still required. No M2 phone action.

Final-stage diagnostic stopped at `finalstages76.txt:90`, missing a system32 graphics interface. Follow-up audit found the A37 runtime lacks linker32 while vendor audio/camera/OMX include ELF32 executables and vendor still selects zygote64_32. Gate remains intact. Details and candidate payloads are in docs/m2-abi-review.md; no ABI payload was changed, no clean attempt42 started, no completed M2 artifact. User application-ABI choice is pending; native32 hardware runtime is required under either choice. The five optional Settings omissions in0075 are provisional pending the user's preference, not a user-confirmed feature decision.

## Clean attempt42 — 22/10/2026

User confirmed app64-only M2 and omission/hiding of five unsupported optional Settings controls. Final patch0077 copies47 pinned S22 native32 files while retaining/hash-checking the fork radio common-V2 wrapper. Final static namespace audit covers164 ELF nodes; it is not device runtime evidence. Patches0078–0080 handle the eSIM MEP declaration, SecureFolder namespace move and scoped retention of A37 ObjectCapture/IPService/Rubin behavior. The diagnostic continuation final-last80 exits0 through Infinity and the non-KernelSU DS-ACK BOOT module, but does not establish a complete build.

All80 patches replay to61 byte-identical source files. Clean attempt42 is running detached through the original make_rom.sh --clean-work-dir --build-rom-zip command; log/PID/exit are attempt42.txt/.pid/.exit in logs/20261021-m2-build. Required APK/JAR rebuild, compiled ADB/Auto Blocker gates, final native32/vendor/radio/kernel checks and unique retained package publication remain pending. No complete M2 ZIP or M2 phone action is claimed. M1 archive is retained.

Attempt42 ended exit1 after11min12sec at attempt42.txt:2393: Required Photo Editor component is missing: /system/priv-app/PhotoEditor_AIFull/PhotoEditor_AIFull.apk. The AZH4 donor and staged PhotoEditor_Full APK are present and both hash91a458916c289f9218e1791c0d934cb40de1f538a72ab6f402c97d77008d412e. Patch0081 selects this actual path only for A376B, retaining all ZIP/library/permission checks. Actual inventory helper and replay81/61files PASS. No release was published. M1 retained check42 PASS for all10 archived ZIP hashes/sizes.

Final-tail81 diagnostic exited0 through all remaining finalization helpers and source ADB defaults. readelf warnings originate from the unchanged embedded blank_screen binary (SHA58169bff676f9451743f57eb14ee3e5a0537e4ec038b14192071c4a34fccbc56); its section-header table extends past EOF, while program segments are in bounds. No binary edit; actual execution still requires logs. Clean attempt43 launched detached with81 patches through original pipeline; all compiled/release gates pending.

Attempt43 passed all module staging and reached APK/JAR rebuild. During rebuild, read-only preflight identified an inconsistent boot verifier requiring face-default-sec.rc removed by the selected Face omission; it also retained S22-specific SDP absence assertions despite0065 preserving AZH4 Knox. Patch0082 changes only this host verifier: exact profile validation, existing Face omission verifier and positive AZH4 SDP class inventory. Other donors keep original requirements; all other checks remain. This verifier-only correction was applied while APK rebuild continued; staged payload/source modules were not changed, so no restaging is required. Replay82/62files, shell syntax and3 rejected wrong-profile fixtures PASS; compiled gates pending.

Attempt43 compiled partial audits: rebuilt SettingsProvider fresh-data seed calls/SQL PASS (settingsprovider-compiled43.json): global adb_enabled/development_settings_enabled=1, Rampart defaults0, INSERT OR IGNORE. Actual rebuilt SecSettings resource XML absence audit PASS for all5 omitted optional controls, with Bixby positive-control XML present (optional-controls-compiled43.json). These do not replace the pending full services lifecycle/boot/release checks. Build continues APK/JAR rebuild; no completed M2 ZIP or phone action.

Attempt43 ended exit1 after29min10sec at the compiled boot contract: missing UN1CA Bixby remapper in system/system/framework/services.jar. All APK/JAR rebuilds completed; final wallpaper and actual rebuilt SystemUI Brief/Edge Lighting gates PASS. Earlier0036 intentionally skipped the incompatible A37 key-event remapper, but Settings controls and USER keylayout mapping remained; gate correctly blocks this inconsistent feature. User has been asked whether to omit/hide remap with source keylayout retention for M2 or port it fully; no choice assumed and no further Bixby patch applied.

Independent read-only release-default checks PASS in release-defaults-independent43.txt: pinned native32 payload/zygote, selected Face omission, no-KernelSU runtime, compiled ADB properties/seeds/stored-choice lifecycle, Infinity and PIF defaults. Its Bixby resource-presence check is only UI inventory and does not prove functional remapping; the full boot contract remains FAIL. No complete M2 ZIP and no phone action. Current compiled tree retained; .completed/release publication not bypassed.

User confirmed hiding Bixby remap and original keylayout. Patch0083 updates source recipes and compiled gates. Stage delta from completed attempt43 is recorded in bixby83-stage-delta.json: remove remap selector from decoded Settings, use actual finalization helper to map703 WINK in2 layouts. Vendor Samsung layout is then byte-identical to AZH4; stock G965F Generic also uses WINK. Existing fork --resume-after-mods --build-rom-zip attempt44 rebuilds APK/JARs and reruns all gates; no .completed bypass or direct diagnostic packaging. Old compiled Settings is correctly rejected by updated gate (unsupported unica_bixby_action). Source replay83/62files PASS; full output pending. Helper log originally said USER after changing to WINK; source log corrected to actual selected keycode, stage bytes verified separately.

Attempt44 rebuild was deliberately stopped via its own process group after1min54sec, before gates/package, to use existing fork UN1CA_APKTOOL_JOBS=4 on72 logical CPUs/53GiB available RAM. Its trap misleadingly prints Build completed on SIGTERM; attempt44.interrupted is authoritative, no exit record or package completion exists. Attempt45 is the same original --resume-after-mods --build-rom-zip with4 existing Apktool jobs, detached; no build-system/payload change.

Attempt45 passed all compiled boot/release checks but stopped in SELinux compilation: `Failed to resolve allow statement` for `otapreopt_slot` in merged `plat_sepolicy.cil:78269`; the S22 allow referenced a source domain absent from AZH4. Patch0084 removes exactly this stale allow only for A376B, checks its expected unique occurrence and verifies absence; enforcing remains enabled. The actual finalization helper, Exynos9810 policy verifier, full staged `secilc` compile (`selinux84-compile.txt`) and replay of all84 patches pass.

Attempt46 resumed the existing staged build/package path, with all compiled boot/release gates passing: app64/native-vendor32 ABI, Face omission, compiled ADB defaults/no-auth/stored-choice lifecycle, hidden unsupported Settings controls and original Bixby keylayout, pinned no-KernelSU DS-ACK, Hybrid Vendor VNDK33/radio checks, ext4 filesystem verification, distinct `libPSI.so`/`libpsi.so`, camera matrix and enforcing SELinux. Exit0; flashable package verifier reports `ext4, enforcing, six device payloads`. Retained immutable release: `rom/out/releases/M2/star2lte-1792642788-JFvPsK/`. Both package SHA256 checks and `7z t` pass. Flashable ZIP SHA256 `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`; target-files ZIP SHA256 `d4c3b69fe96f39888f15b3671633b16db6258f99f620263794a7805ebcbb5efe`. This completes PC build/package only; M2 has not been flashed and boot/runtime acceptance is pending user-collected logs. M1 archive remains untouched.


## M2 G965F fresh flash and boot-loop evidence — 22/10/2026

User explicitly authorized a one-time fresh M2 install on G965F serial `M2_G965F_SERIAL`, including USERDATA format; G965N stayed untouched. The retained M2 flashable release SHA256 is `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`. TWRP 3.7.0_9-0 rejected the original >4GiB Zip64 file before updater writes (`Zip file is corrupt!`); the existing fork legacy-split helper produced eight ordered ZIP32 stages. Stages01–07 completed RC=0. First stage08 attempt failed because `/sdcard` was busy at the format boundary (`umount: can't umount /sdcard: Device or resource busy`, then `E9810: could not unmount data aliases`); USERDATA was not formatted by that attempt. A retry from `/tmp` first hit stale kernel installer files (`unzip: can't read standard input: Not a typewriter`, `E9810: Failed to extract kernel installer.`). After removing only those temporary extracted files and retrying stage08, the installer reported `Formatting cache, omr, preload and data...`, `script succeeded: result was [1.000000]`, and `Updater process ended with RC=0`. TWRP subsequently reported `/data` Used:0MB; the new USERDATA UUID is recorded in `postflash-recovery-checks.txt`.

System boot then entered a repeated normal-boot loop. The phone was returned to TWRP by the user. Read-only capture is in `logs/20261022-m2-g965f-flash-01/`: current `/tmp/recovery.log`, `/cache/recovery/log.gz`, `/sys/fs/pstore/pmsg-ramoops-0`, and `/proc/last_kmsg`. The bootloader trace records repeated normal-start entries followed by recovery: its line19649 has `N(2026-10-22-05-33)` through repeated `N(...05-36)` entries; line22318 ends with `N(2026-10-22-05-37)R(2026-10-22-05-37)`. The captured last-kmsg is a 2MiB wrapped Samsung bootloader/early-start trace and ends at `Starting kernel...`; it does not expose the Android kernel/userspace failure. Pstore contains adbd/framebuffer messages only and there is no `console-ramoops` or tombstone. TWRP log confirms bootloader `G965FXXUHFVG6`, fresh empty `/data`, and recovery runtime; this is not evidence of successful M2 boot.

Status: M2 package installation and fresh-data format completed, but M2 boot/runtime acceptance **failed** because the device repeatedly restarted before Android became available. Root cause is **undetermined**; the controlled retry also did not expose ADB (see `controlled-boot-watch.txt`). No `avc: denied`, `CANNOT LINK EXECUTABLE`, `dlopen failed`, `Fatal signal`, VINTF, or init failure from Android was captured, so no cause is inferred. No further flash, format, or reboot was performed after log collection. Next diagnosis requires a log source that survives the early reboot (for example a usable kernel console/pstore capture or an external early-boot capture); do not treat recovery `dmesg` as the failed system boot. M1 release remains untouched.


Continuation audit 22/10/2026: comparison of retained M1 versus M2 flashable ZIP entries confirms `boot.img`, `exynos9810/ramdisk.img`, `kernel.zip`, `install` support/preflight/layout/vendor_bootfix payloads are byte-identical where present; only `exynos9810/format_partitions.sh` differs among the compared installer entries. Exact SHA evidence: `logs/20261022-m2-g965f-flash-01/m1-m2-installer-comparison.json`. This does not prove the on-device BOOT was installed identically and does not establish a cause. Current PC ADB inventory is empty; waiting for G965F TWRP connection to read and unpack actual BOOT and compare against the known-good M1 boot evidence. No kernel change or device write made during this audit. The fork already contains optional early/late diagnostic logger paths; their suitability must be reviewed before any diagnostic-image proposal.


Recovery audit02 — 22/10/2026: G965F reconnected and verified as SM-G965F / G965FXXUHFVG6 / TWRP3.7.0_9-0. Read-only full BOOT capture (57,671,680 bytes) matches the M1 flash03 known-good BOOT **byte for byte**, SHA256 `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Kernel/DTB/ramdisk hashes match M1 independently. Samsung legacy header is not accepted by the AOSP unpack tool (`UnicodeDecodeError` decoding header); used the established M1 verifier offsets to inspect it instead. Nine installed early userspace files (init, bootstrap linker/libraries, vendor fstab pair) match the staged M2 tree; SYSTEM/VENDOR mounted `ro,noload` for hashes then unmounted. Existing recovery-mounted data only contains lost+found/media, with no Android system/misc/tombstone or bootloop logger directories. Evidence: `boot-audit-recovery-02/{boot-comparison.json,early-files-comparison.json,recovery.log,last-kmsg.bin,pstore/}`. No partition writes or reboot in this audit.

Ramdisk `/init` is the unchanged statically linked Android29 first-stage binary. This is inventory only: compatibility with the A37 second-stage init remains unproven and must be diagnosed by early-boot logs. Existing `patch_bootlogger.sh` must not be invoked as-is: it replaces PID1 `/init`, while current `bootlogger_init.c` explicitly implements a standalone logger that must never replace/chain Android init. That source/installer mismatch makes it unsuitable as an unreviewed diagnostic shortcut. No logger image has been installed.


Repeat fresh M2 flash02 (22/10/2026, direct user request): all eight hash-verified existing ZIP32 stages installed sequentially on G965F serialM2_G965F_SERIAL; each updater RC=0 with correct next-stage state. Stage08 ran from RAM without retries. USERDATA UUID changed from `d9175a13-acb8-4ea3-9c85-14333b55afe9` to `74d8755a-7aff-4d40-93da-20b57bda9b97`; preboot read-only mount shows only lost+found. Actual BOOT SHA remains `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Fresh install verified; system boot watcher now running. Evidence: `logs/20261022-m2-g965f-flash-02/`. Runtime result pending; no N actions or kernel changes.


Flash02 runtime follow-up: user confirmed the fresh test still bootloops. System ADB did not become available during the observation window, so Android logcat/dmesg could not be retrieved. User was asked to return G965F to TWRP for post-failure read-only capture. All eight installer stages RC=0, UUID-change and empty-data proof remain valid; installation success does not imply boot success.


Flash02 postboot capture complete: G965F verified again in TWRP (`SM-G965F`, `G965FXXUHFVG6`, serialM2_G965F_SERIAL). Raw read-only logs in `logs/20261022-m2-g965f-flash-02/postboot-recovery/`. 2MiB last-kmsg trace records repeated normal boots 07:31–07:34 UTC then recovery; line22392 is `Starting kernel...`. Pstore only has pmsg-ramoops-0 (16,372 bytes), with adbd FunctionFS/USB endpoint shutdown messages, including `remote usb: read terminated (message): Cannot send after transport endpoint shutdown`; this does not establish a system-boot cause or which boot produced every record. No console-ramoops/panic/Android init failure captured. USERDATA still has the new UUID74d8755a-7aff-4d40-93da-20b57bda9b97 and only lost+found/media after recovery creates its media directory. Fresh install did not resolve the bootloop; runtime gate remains FAIL, root cause unconfirmed. Phone left in TWRP; no additional write/reboot during this capture.


Patch0086 / early diagnostic prepared on PC: new optional offline helper `rom/scripts/internal/prepare_star2lte_boot_diagnostic.py`, no automatic build activation. Only legacy BOOT command-line field changed; pinned kernel/ramdisk/DTB/tail unchanged. Diagnostic SHA `a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9`, rollback SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Wrong-input/no-output, output-collision and syntax checks PASS. No image flashed. Review: `docs/m2-early-diagnostic-review.md`; actual pstore/fatal behavior needs device evidence. Await user authorization for this distinct BOOT diagnostic plus rollback, G965F only.


Early diagnostic test01 authorized and installed on verified G965F on22/10/2026. BOOT full readback matches a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9. Only command-line field changed; no kernel/DTB/ramdisk replacement or data format. Boot observation in progress; preserve captures then restore pinned baseline. Evidence `logs/20261022-m2-early-diag/test-01/`. AGENTS.md now reflects user's project flash authorization; F-first/N-reference plan stays.


Early diagnostic test01 completed22/10/2026: user confirmed bootloop and returned G965F to TWRP. Device identity and diagnostic BOOT SHA were verified; `/proc/last_kmsg`, `/proc/sec_log`, pstore and recovery logs captured under `logs/20261022-m2-early-diag/test-01/postboot/`. No console-ramoops or M2 fatal/panic signature recovered. Logs contain recovery kernel `Linux version 4.9.118-g36e6ddff22b9`; fstab errors (`/etc/recovery.fstab`) and hung-task lines belong to a recovery trace and are not evidence of M2 failure. Diagnostic effectiveness unproven, root cause unresolved. Original BOOT restored and entire partition readback SHA verified `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; no post-rollback reboot. Phone left TWRP. No kernel change, no data format, no N operation. Result JSON in test01. Next investigation must improve capture before/through early kernel/init rather than repeat unchanged fresh installs.


Patch0087 / explicit PID1 pmsg probe02: optional raw-syscall AArch64 monitor in `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`, prepare helper extended for cpio. Original init preserved as /init.real; probe keeps PID1 then execs original with argv/env, child copies selected kmsg errors to pmsg only, no FS mounts/partition access. TWRP fork/wait/exec/env self-test PASS, non-PID1 refusal13. Existing DS-ACK MagiskBoot repacks RAM files. Independent CPIO audit preserves all other data/metadata; kernel/DTB/cmdline unchanged. BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926. Diagnostic only: timing/SELinux can affect capture; compare results cautiously. Plan: one verified G965F boot under current project authorization, capture then rollback baseline cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d. Evidence `logs/20261022-m2-early-diag/pid1-probe-02/`.


Probe02 installed under current project flash authorization on verified G965F; readback full BOOT SHA c1b0f6902fc2fa621659483a4091dac890ebdc5040398bac71e58292dfa51926 PASS. One system boot requested. No system ADB observed in the initial90-second window; waiting for user-assisted return to TWRP to capture pmsg/last_kmsg/sec_log and restore baseline. No new success/root-cause claim; probe effectiveness unknown until logs are collected. Probe remains installed pending recovery access. Patch0087 reverse/apply preflight PASS.


Probe02 completed: verified G965F returned to TWRP; probe BOOT hash matched. Captured pstore (empty), last_kmsg/sec_log/recovery/reset reason under `logs/20261022-m2-early-diag/pid1-probe-02/postboot/`. No M2-PID1 marker or M2 fatal recovered; absence does not distinguish pre-init failure from unavailable pmsg/sysfs/dev nodes or recorder failure. Restored baseline BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d with entire-partition readback PASS, no reboot. Phone TWRP.

Patch0088 is an unflashed follow-up prototype: isolated monitor mount namespace for only tmpfs/sysfs so original init mount table stays untouched. RAM preflight FAILED `NAMESPACE stage=4 errno=016` (sysfs mount EBUSY), exit20; parent mount table SHA unchanged. Do not flash this prototype; no diagnostic image03 created/installed. Evidence `logs/20261022-m2-early-diag/pid1-probe-03/namespace-selftest.txt`. Root cause still unresolved; preserve pinned kernel and baseline BOOT.

Patch0089 / probe04 (22/10/2026): the same isolated monitor now accepts sysfs `EBUSY` only after opening the real `/sys/class/pmsg/pmsg0/dev` node. Warm and cold mount-namespace tests, plus fork/wait/exec/env, passed in TWRP; parent `/proc/mounts` SHA was unchanged. A probe write returned `PROBE-PMSG-WRITE-PASS`; after rebooting recovery, `pmsg-ramoops-0:59` contained `M2-PMSG-RAM-SELFTEST-20261022-PROBE04`. The diagnostic BOOT SHA is `dd4be5b35bd73f6135d8a9d623031439d29652a9e0d4ff1083266b6df9180a68`; kernel, DTB, command line and footer were independently checked unchanged. The G965F prewrite guards and full BOOT readback matched this SHA. One M2 boot was requested; PC observed no Android ADB during 60 seconds (`pid1-probe-04/boot-watch.txt`). Awaiting recovery access for pstore capture and baseline BOOT rollback; no cause claim yet.

Probe04 follow-up: user returned G965F to TWRP after continued bootloop. BOOT readback matched the diagnostic SHA; pstore contained 951 bytes of recovery adbd/graphics records and no `M2-PID1-PROBE` marker. Bootloader trace reached `Starting kernel...` (probe04 last-kmsg line24760); its `pit_check_signature (BOOT) invalid.` at line24633 is also present with the baseline BOOT in fresh M2 flash02, so it does not prove rejection of the diagnostic image. Captured pstore/last-kmsg/sec_log/recovery/reset reason, then restored the full baseline BOOT and verified SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. No new root-cause claim.

Patch0090 / probe05: isolated child monitor can also mount the verified ext4 CACHE partition (`/dev/block/sda21`, runtime dev_t `259:5` in TWRP) and fsync a dedicated `m2-early-boot-probe05.log`; parent mount table remains unchanged. TWRP warm/cold namespace, fork/wait/exec/env and CACHE write tests passed, and the CACHE marker survived a recovery reboot. Diagnostic BOOT SHA `fc54c3b87d3667cdf565d8074629e1c1486808c4c7ede4196b261467e02fe236`; independent audit confirmed unchanged kernel/DTB/cmdline/footer. G965F prewrite guards and full BOOT readback passed. After a 45-second system test the user returned the bootlooping phone to TWRP. CACHE file still contained only the preflight marker `M2-CACHE-RAM-SELFTEST-20261022-PROBE05`; no PID1 runtime marker. Pstore also contained no probe marker. Bootloader trace reached `Starting kernel...` (probe05 last-kmsg line25231, recovery handoff), but no M2 kernel/init fatal line was recovered. The absence of a marker does not distinguish failure before PID1 from a diagnostic monitor failing in the enforcing Android environment. Baseline BOOT was restored and full readback SHA verified. No kernel change, no data format, no G965N operation. Evidence: `logs/20261022-m2-early-diag/pid1-probe-05/`.

### 23/10/2026 — kiểm recorder trên M1 trước lượt M2 mới

M1 archive được cài fresh lại trên G965F `M2_G965F_SERIAL` và boot hoàn tất (`sys.boot_completed=1`, SDK36/OneUI80000). BOOT probe05 cũ vẫn cho M1 boot nhưng không tạo file CACHE. Self-test chạy trong Android báo `NAMESPACE stage=4 errno=002`; kernel DS-ACK system không có `/sys/class/pmsg/pmsg0/dev`. Vì vậy các lượt M2 probe04/05 trước đây không thể dùng việc thiếu marker để kết luận M2 chết trước PID1. Bản probe06 trong patch0091 dùng block sysfs CACHE và xác minh ext4 trước khi ghi, cho phép thiếu pmsg. Trên M1, self-test ghi thấy được từ `/cache`, boot với BOOT probe06 SHA `061470a84dc4d716f8f6e5fd7b0000f3edfdb3f85e9197ef8d76aefdf1b0d34e` hoàn tất và file CACHE có `M2-PID1-PROBE-v4: persistent cache monitor ready` cùng các dòng `init:`. Kernel, DTB và cmdline trùng bản nền; chỉ ramdisk chẩn đoán thay. Chứng cứ: `logs/20261023-m1-probe-control/`, `logs/20261023-m1-probe-calibration/`.

M2 package gốc giữ nguyên SHA `e102bee165410ede2869532d31c8a5b693a5a2c3242a59f5fd1fdf113009d1dc`. Lượt cài fresh thứ ba vào G965F dùng 8 ZIP stage đã khóa hash, 8/8 `RC=0`, USERDATA có UUID mới, các hash EFS/CPEFS/RADIO/RECOVERY/BOTA0-2 không đổi trong lượt cài. Gắn BOOT probe06 sau cài, full-partition readback PASS. Theo dõi 240 giây không thấy ADB (`M2_PROBE06_STATUS TIMEOUT`); đang chờ đưa về TWRP để đọc CACHE và rollback BOOT. Chưa có kết luận nguyên nhân M2 bootloop. Chứng cứ: `logs/20261023-m2-probe06/`.

Postboot capture complete: G965F returned to TWRP with diagnostic BOOT readback matching `061470a8...`, and `/cache/m2-early-boot-probe06.log` contains 23 `M2-PID1-PROBE-v4: persistent cache monitor ready` markers (line2,33,64,...). Each reboot's captured selected kmsg ends at roughly 0.82–0.84 s, e.g. line29 `dwc3 10c00000.dwc3: dwc3_core_init: max speed:5, hibernation:0, nr_scratch:0`. There are no actual `;init:` messages (distinct from kernel symbols containing “init”), `avc: denied`, linker error or kernel panic in this CACHE capture. In the M1 control the same recorder reached 3.14 s and captured 31 actual `;init:` messages. This proves M2's BOOT reaches PID1 wrapper and persistent CACHE, but does not yet prove whether original init fails, a watchdog resets, or the monitor is killed at an early init transition. Last-kmsg bootloader trace reaches `Starting kernel...`; pmsg only contains unrelated/possibly stale adbd records. Baseline BOOT was restored with full-partition SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d` readback PASS; phone left in TWRP with M2 system/data. No kernel, vendor/radio, or G965N write. Next diagnostic should capture unfiltered kmsg/heartbeats around the first-stage init transition on the calibrated M1 control before any further M2 flash; do not infer a kernel fault from the absence of later lines.

### 23/10/2026 — probe07 trên hai G965F song song

Patch0092 đổi probe tùy chọn sang ghi toàn bộ `/dev/kmsg` và heartbeat mỗi 100 ms, file `/cache/m2-early-boot-probe07.log`; bản ROM đóng gói không chứa probe. BOOT chẩn đoán SHA `ea321c27e50cded71fc7fa4f1dbbef727dd76bec92c114a04bcfa2dd13de899b` giữ nguyên kernel/DTB/cmdline/footer theo audit CPIO và legacy header; chỉ thay ramdisk. Source: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0092-unfiltered-kmsg-heartbeat-probe.patch`.

G965F mới serial `M1_G965F_SERIAL` cài archived M1 8/8 stage và boot hoàn tất với probe07 (`sys.boot_completed=1`). Stage1 đầu tiên đã repartition nhưng TWRP còn dùng bảng cũ: `E9810: partition refresh failed`; sau reboot recovery, kiểm layout SYSTEM 9961472000/VENDOR 1363148800/ODM 734003200, format DATA và cài lại 8/8 `RC=0`. EFS thay hash sau hai lần reboot recovery của máy mới; không có lệnh ghi EFS, các phân vùng CPEFS/RADIO/RECOVERY/BOTA giữ hash, và cả bảy hash không đổi trong lượt cài thành công. M1 CACHE có marker probe07, 2 heartbeat, kmsg không lọc đến ~1.397 s; recorder dừng ghi tại first-stage transition nhưng M1 tiếp tục boot. Evidence `logs/20261023-m1-second-probe07/`.

G965F M2 serial `M2_G965F_SERIAL` giữ M2 system/data fresh từ lượt trước; chỉ thay BOOT probe07 và quan sát 240 s. Không có ADB. Sau khi về TWRP, CACHE dài 242180 bytes chứa 19 marker probe07 qua các lần boot, không có heartbeat hay dòng `;init:`. Median timestamp kmsg cuối mỗi segment ~0.326 s, nhưng đây là thời điểm recorder ghi được, không phải thời điểm reset đã chứng minh; việc fsync từng kmsg record cũng có thể làm monitor chậm. Không có `avc: denied`, `CANNOT LINK EXECUTABLE`, `Fatal signal` hay kernel panic trực tiếp trong CACHE. Evidence `logs/20261023-m2-probe07/`. Cả hai BOOT đã rollback về SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d` với full-partition readback PASS; hai máy ở TWRP, G965N không tác động. M2 boot gate vẫn FAIL; log chưa chứng minh cần sửa/build kernel. Bước chẩn đoán probe07 đã hoàn tất; bước sau cần giảm fsync/trang bị dấu mốc quanh `exec /init.real` rồi xác nhận trên M1 trước khi thử M2.

### 23/10/2026 — probe08 giảm tải ghi CACHE

Patch0093 cập nhật recorder tùy chọn: gom kmsg vào buffer 32KiB, fsync khoảng 100ms, heartbeat ghi thời gian monotonic tính từ lúc child chạy và số bản ghi đọc, giới hạn kmsg 8MiB mỗi boot. File duy nhất là `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; BOOT chẩn đoán SHA `4ce09fcb34cd48d8f508d17e7bf8e4f0f0ecd739519d7e4406748434386e5b5a` thay ramdisk, audit xác nhận kernel/DTB/cmdline/footer nguyên bản. Normal ROM không dùng probe.

M1 trên G965F `M1_G965F_SERIAL` boot hoàn tất (`sys.boot_completed=1`) với 17 heartbeat, cuối cùng `M2-PROBE-HEARTBEAT ms=1761 records=4130`; file CACHE 363876 byte và có 31 dòng `;init:`. Đây là mốc đối chứng đạt yêu cầu vượt ~1.4s của probe07. M2 trên G965F `M2_G965F_SERIAL` không xuất hiện ADB trong 240s. CACHE 376143 byte có 19 `M2-PID1-PROBE-v6: batched monitor ready`; các segment có tối đa một heartbeat, ví dụ dòng2337 `M2-PROBE-HEARTBEAT ms=114 records=1536`. Không có `;init:`, `avc: denied`, `CANNOT LINK EXECUTABLE`, `Fatal signal` hoặc kernel panic trong CACHE. Pstore chỉ có pmsg-ramoops-0 951 byte, chưa có fatal rõ. Batching đã cho M1 ghi muộn hơn nhưng M2 vẫn dừng recorder rất sớm; log chưa phân biệt reset thật với init giết child ở first-stage transition. Hai BOOT đã được rollback về baseline SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`, full-partition readback PASS; hai máy ở TWRP, G965N không tác động. Evidence `logs/20261023-probe08/`. M2 boot gate vẫn FAIL; không có chứng cứ sửa/build kernel.

### 23/10/2026 — probe09 xác nhận exec PID1 và rollback

Patch0094 thêm pipe `O_CLOEXEC` giữa wrapper PID1 và recorder để ghi lỗi `execve("/init.real")` nếu exec trả về; EOF cho biết exec thành công hoặc parent kết thúc. File ảnh hưởng: `rom/platform/exynos9810/installer/bootlogger_pid1_probe.c`; patch `patches/fork/0094-report-init-exec-through-cache-child.patch`. Self-test errno `ENOENT` qua pipe PASS; M1 control trên serial `M1_G965F_SERIAL` boot completed. M2 serial `M2_G965F_SERIAL` vẫn không có ADB sau 240 giây. CACHE `/cache/m2-early-boot-probe09.log` ghi nhiều lần `M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1`, không có `M2-INIT-EXEC-FAILED`; do đó `/init.real` được exec ở các lượt ghi được, nhưng log này chưa chứng minh init đã hoàn tất chuyển trạng thái. `pmsg-ramoops-0` được đọc sau khi TWRP đã khởi động; các bản ghi adbd/FunctionFS/USB thuộc phiên recovery hiện tại hoặc không thể gán chắc chắn cho M2, nên không dùng làm bằng chứng M2 đã khởi chạy adbd. Không có panic/fatal M2 xác định được trong các file thu được. CACHE 396373 byte, last_kmsg 2097152 byte, pmsg 1817 byte; hash và bản gốc trong `logs/20261023-probe09/`. Hai máy đã được đưa về TWRP; BOOT cả hai restore về baseline SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`, full-partition readback PASS. M2 gate vẫn FAIL; nguyên nhân sau `execve` còn mở. Không sửa kernel/vendor/radio, không ghi EFS/CPEFS/modem/bootloader.

### 23/10/2026 — audit USERDATA sau probe09

Đã xác minh lại G965F serial `M2_G965F_SERIAL`, model `SM-G965F`, bootloader `G965FXXUHFVG6`, TWRP `3.7.0_9-0`. `/data` và `/cache` đang mount rw bởi TWRP; thao tác chỉ đọc. USERDATA chỉ có `/data/lost+found` và `/data/media/TWRP`; không tồn tại `/data/tombstones`, `/data/anr`, `/data/system/dropbox`, `/data/system/bootstat`, `/data/misc/logd` hay Android system tree. CACHE chỉ có probe09 cùng log TWRP; pstore chỉ có `pmsg-ramoops-0` dài 1817 byte. Vì pmsg được thu trong phiên TWRP, các bản ghi adbd/FunctionFS không thể gán cho M2. Inventory, log TWRP, CACHE và pmsg đã lưu tại `logs/20261023-m2-postboot-userdata/`. Không có log Android hậu khởi động để phân tích. Bước kế tiếp là bật các hook chẩn đoán init/vendor và system logger sẵn có trong fork (`EXYNOS9810_ENABLE_INIT_DEBUG=true`, `EXYNOS9810_ENABLE_SYSTEM_BOOT_DEBUG=true`), build riêng M1 calibration rồi M2, ghi các mốc sớm vào CACHE; không thêm build system mới hay sửa kernel.

### 23/10/2026 — clean build attempt47 complete

Attempt47 used the fork's `make_rom.sh --clean-work-dir --build-rom-zip` with the existing M2 A376B Android16 / G965F / ext4 / no-KernelSU profile; process exit code 0. The clean stage regenerated full CIL inputs and passed `PASS Exynos9810 staged SELinux policy compilation`, so the attempt45 stale `otapreopt_slot` policy failure did not recur. Mandatory static checks passed: compiled keyless ADB/default lifecycle and Auto Blocker off, Exynos9810 boot contract, A37 app64/native-vendor32 ABI, enforcing ext4 image verification, and flashable verifier. Build log additionally records Hybrid Vendor stack VNDK33 (`attempt47.txt:30`), coherent N770F IRadio 1.6/IMS declarations (`:104`) and restore (`:1760`), plus the selected duhansysl DS-ACK V1.12 08/05/2026 enforcing no-KernelSU package (`:2629-2630`). Five unsupported controls and Face Unlock remain omitted per prior decisions; donor Wi-Fi UI and donor Bixby key behavior remain selected.

New immutable output: `rom/out/releases/M2/star2lte-1792735195-f3GTW7/`. Flashable ZIP SHA256 is `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`; target-files ZIP SHA256 is `14ccb6675bcae1ffeddd0b41f4488e3b2bd0aaa4f8c3db00970b0c74c4d474da`; both are recorded in `SHA256SUMS`. The fork's flashable verifier reports `Verified Exynos9810 flashable ... (ext4, enforcing, six device payloads)` and the ZIP integrity test passed. M1 archive and prior M2 release `star2lte-1792642788-JFvPsK` remain intact.

Read-only target-files comparison with the prior M2 artifact: BOOT image is byte-identical (`6a6fe0cd652b22103e88547254c0b8b777aef9612b40309ab1150fe1d1ee410e`); ODM changed `d98fafb2978c470c385f7c9e8a430be50adf3d64e0f06b063aa58264c20d6cd6` -> `9f4ced089346a68f2625e1863490928f17eba0425aa7bffeec90df43cf42321a`; SYSTEM changed `d99110c1e828a8f2ceb220883d2c0479dce58593799449df0a8f6745555b033f` -> `5c6c830a6adeb917e16668ed93a734b69e67a2044944bae3d2eacd247eb89186`; VENDOR changed `15add88c4e0ac0f9c73867053be3b4b2099aede36158b356a9b898c9931870f6` -> `daa4c8f26eb78cf31a256531d6ceb4740aaaaa219a3bb96a4f9ff715daa99460`. This confirms the new images differ while the baseline BOOT image does not. No phone was touched by attempt47; no new runtime result is claimed. Earlier M2 fresh boots still looped with no definitive fatal/init log, so M2 runtime acceptance remains FAIL pending a fresh test of this distinct package and evidence capture.

## 23/10/2026 — M2 attempt47 fresh test

**Patch / change record:** không thay source patch hay build system; chỉ thêm script cài đặt audit cho artifact bất biến của attempt47. Lý do: ZIP Zip64 nguyên gói không tương thích TWRP; mở rộng helper legacy ZIP32 có sẵn của fork. File ảnh hưởng: artifact stage trong `rom/out/releases/M2/star2lte-1792735195-f3GTW7/` và `logs/20261023-m2-attempt47/flash-01/install_m2.py`. Kiểm chứng: source ZIP SHA256 `9d1baaa48ef5ea4caae6028bd7e2c378a270eafc3b1af27008758dd45758df23`; manifest `status=PASS`, 8/8 chữ ký/CRC/Zip32 PASS; script py_compile và preflight pass. Không sửa kernel.

Target đúng G965F serial `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`, TWRP `3.7.0_9-0`. Cài 8/8 stages đều `Updater process ended with RC=0`, stage marker cuối `next=done`. UUID USERDATA đổi từ `615a10c2-626c-4cd7-9bb6-65fe8d5a126b` thành `b2841bc0-1371-4d89-84a7-aa71e91f9f3a`, xác nhận fresh-format. Protected EFS/CPEFS/RADIO/RECOVERY/BOTA hashes giữ nguyên ngay sau cài và trước reboot; BOOT khớp baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Recovery log stage08 ghi format cache/omr/preload/data.

Sau reboot hệ thống không có ADB completed trong 240 giây; người dùng đưa về TWRP bằng phím cứng. Evidence: `logs/20261023-m2-attempt47/flash-01/`. `/proc/last_kmsg` là 2,097,152 byte và chỉ có các trace lặp tới bootloader `Starting kernel...` (extract lần cuối dòng 38986); không có panic/kernel console fatal sau dòng đó. `/sys/fs/pstore/pmsg-ramoops-0` chỉ có 951 byte chứa adbd FunctionFS và ArmToArm64Assembler scanline records; không thể gán chắc chắn từng bản ghi cho một lượt boot, không có lỗi init/HAL cụ thể. `/tmp/recovery.log` là log khởi động TWRP hiện tại, không phải log Android M2. Không có căn cứ để quy lỗi cho kernel hoặc thay kernel.

Post-loop hash đọc được cho CPEFS/RECOVERY/BOTA giữ nguyên. EFS hash sau boot thành `471ca981bf3e02e3747561c8adc99db202ec06af6f8c98479d67ac0be3b41e88`, khác mốc trước cài `27051fae2c6f5be7b85de7860522e1f15a10d52989032bb5d8e629ba08c597b8`; installer không ghi EFS, và tôi không khôi phục hay ghi EFS. BOOT post-loop readback bị ADB transport drop nên chưa xác nhận lại sau boot; trước reboot và ngay trước đó hash bằng baseline. M2 runtime gate vẫn FAIL, root cause chưa xác định. N không bị chạm.

Bước tiếp theo theo kế hoạch chẩn đoán hiện có: bật init/vendor logger và system boot logger bằng build flags trong artifact riêng; trước đó chạy M1 calibration trên máy đối chứng theo checklist để xác nhận recorder/log tồn tại, rồi thử lại M2. Giữ nguyên kernel/vendor/RIL cho tới khi log chỉ rõ lớp gây lỗi.

## 23/10/2026 — attempt48 logger build and staging

**Patch / change record:** thêm profile M2 chẩn đoán để bật hai logger đã có trong fork. Lý do: attempt47 không có Android ADB hay fatal log quy được nguyên nhân. File ảnh hưởng: `rom/target/star2lte/profiles/m2-g965f-a376b-android16-no-ksu-init-debug.sh`; runtime ghi mốc trong CACHE từ init/vendor và system boot logger. Donor, Hybrid Vendor VNDK33, RIL/blob và DS-ACK không đổi. Kiểm chứng: profile preflight xác nhận M2/A37 Android16/G965F/KSU=false/kernel SHA khóa; build exit 0; staged SELinux policy, ABI, ADB defaults và ext4 image checks PASS; flashable verifier xác nhận enforcing/ext4/sáu payload thiết bị. Release bất biến `rom/out/releases/M2/star2lte-1792740549-Bd5wX7/`; flashable SHA256 `64545025727c698e3ff30088076fc059ed8a9e6bf4dc3420147b911a231fba9f`, target-files SHA256 `9ebf35b2af12daa318c7788825e20d67b93b1ebc37b6084e7945a9ee78bdab8d`. Tám stage signed ZIP32 đều pass CRC/signature/Zip32; manifest `logs/20261023-m2-attempt48-init-debug/legacy/m2-attempt48-twrp32-legacy-manifest.json`.

Runtime test đang chờ kết nối recovery: trong lúc đọc hash cuối, TWRP ADB shell treo và thiết bị chuyển `offline`; chưa cài stage nào của attempt48. Bản BOOT đã lưu đủ 57,671,680 byte, SHA256 khớp baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Khi ADB lên lại, xác minh model và hash các partition bảo vệ tuần tự/có byte count rồi mới fresh-install G965F. Không ghi EFS/CPEFS/RADIO/bootloader. Evidence và artifact build ở `logs/20261023-m2-attempt48-init-debug/`.

## 23/10/2026 — attempt48 fresh test, logger did not reach CACHE

Fresh attempt48 was installed on verified G965F `M2_G965F_SERIAL` / `G965FXXUHFVG6` / TWRP 3.7.0_9-0. All 8 ZIP32 stages ended `RC=0`, marker `next=done`; USERDATA UUID changed from `b2841bc0-1371-4d89-84a7-aa71e91f9f3a` to `ec261cc1-b1b1-46ea-9c74-f6c455e8a304`. Exact-size raw reads confirmed EFS/CPEFS/RADIO/RECOVERY/BOTA0-2 hashes unchanged during this install and BOOT remains baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. EFS already differed from the earlier pre-attempt47 value before this install; no EFS write or restore was performed. The other connected G965F M1 control and G965N were not touched.

The user observed attempt48 bootloop and returned the target to TWRP. TWRP inventory has no `/cache/unica` or `/data/bootloop-logs`, so neither existing logger reached its first persistent stage. `last_kmsg.bin` contains eight bootloader occurrences of `Starting kernel...` (last extracted line 21369) but no `UN1CA-9810`/`init.debug` marker, Android panic, `avc: denied`, `CANNOT LINK EXECUTABLE`, or HAL/VINTF failure. It also contains line 14416 `init: Received sys.powerctl='reboot,adb' from pid: 3529 (/sbin/adbd)` and line 14624 `reboot: Restarting system with command 'adb'`, which identify the collected kernel log as the TWRP recovery session used for our reboot request, not attributable Android M2 kernel output. `pmsg-ramoops-0` is 16,372 bytes with repeated adbd/FunctionFS records, no M2 or UN1CA markers; attribution is recovery-only/uncertain. Therefore this attempt adds no specific Android failure cause and does not justify changing the selected DS-ACK kernel. Evidence: `logs/20261023-m2-attempt48-init-debug/postloop/`.

**Next diagnostic change:** use the fork's existing `EXYNOS9810_ENABLE_VENDOR_BOOT_TRACE=true` alongside init/system logger flags. Its finalization hook creates vendor init phase markers from `early-init` through `boot`; this is one configuration variable, with no kernel/blob changes. Verify `unica_exynos9810_trace.rc` is present in the final vendor image and read the kernel's existing pstore console after the next observed bootloop. If no marker persists again, report the diagnostic boundary as unresolved instead of inferring a kernel or framework fault.


## 23/10/2026 — M2 attempt49 vendor trace fresh test

**Patch/configuration record:** the fork's existing `EXYNOS9810_ENABLE_VENDOR_BOOT_TRACE=true` was added on top of the attempt48 init/system logger profile to expose `early-init` through `boot` phase markers through `/dev/kmsg`. Reason: attempt48 had no attributable Android logger output. Affected files: `rom/target/star2lte/profiles/m2-g965f-a376b-android16-no-ksu-pstore-trace.sh`, the generated final vendor init fragment, and this test evidence. No kernel, Hybrid Vendor VNDK33 base, donor, RIL/blob, or DS-ACK payload changed. Verification: final work tree contains `vendor/etc/init/unica_exynos9810_trace.rc` with all eight actions; final vendor metadata lists SELinux `u:object_r:vendor_file:s0`, uid/gid 0 and mode 0644; build's Exynos9810 boot/ABI/ADB/sepolicy gates and filesystem `e2fsck` checks passed.

Clean build completed in 9m11s. Immutable release: `rom/out/releases/M2/star2lte-1792745884-kYwiwg/`. Flashable SHA256 `06f83d578d1e3534af55135c45ae5fc934d331520512e830e5ebae99531cb34d`; target-files SHA256 `a22050b5687c007a9923dfe06b09ef441c0e836d3a2271f5f1201b05e95c9193`. The existing legacy ZIP splitter produced eight signed stages, each below 2 GiB; CRC, signature, and ZIP32 validation passed for all eight. Build selected DS-ACK V1.12 duhansysl 08/05/2026 no-KSU enforcing.

Fresh install on G965F `M2_G965F_SERIAL`, verified in TWRP as `SM-G965F`, `G965FXXUHFVG6`, TWRP `3.7.0_9-0`, `star2lte`. All eight stages ended `Updater process ended with RC=0`; final marker `next=done`. USERDATA UUID changed from `ec261cc1-b1b1-46ea-9c74-f6c455e8a304` to `ffcad648-a144-4f67-8086-c858ec457507`. BOOT remained baseline SHA `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; package boot image SHA matches prior attempt48. EFS, CPEFS, RADIO, RECOVERY and BOTA0-2 were byte-identical before/after this install. EFS hash at attempt49 preflight is different from attempt48 preflight/postflight; this difference predates attempt49's writes. No EFS write/restore was performed, cause is unknown, and no recovery action was taken.

After system reboot, the target did not appear through ADB during the full 240-second watch. Exact watcher output: `2026-10-23T17:04:35+07:00 state=absent` and `2026-10-23T17:08:36+07:00 watch_end`; the M1 control serial remained online. This proves only that Android ADB was not observed in that interval; the target was subsequently returned to TWRP and the postboot capture is documented below. No root cause or kernel fault is inferred. Evidence: `logs/20261023-m2-attempt49-vendor-trace/`.

**Attempt49 postboot capture complete:** user returned the G965F to TWRP after the 240-second ADB timeout. Capture is under `logs/20261023-m2-attempt49-vendor-trace/postboot/`. Identity matches the authorized target. `/proc/last_kmsg` is 2 MiB and contains 68 bootloader `Starting kernel...` entries, the last at line 38096; the visible trace stops at the bootloader-to-kernel handoff, with no subsequent Linux kernel console, Android phase marker, or fatal. `/sys/fs/pstore` has only a 951-byte `pmsg-ramoops-0` with adbd FunctionFS/graphics records; `console-ramoops-0`, `/cache/unica`, and `/data/bootloop-logs` are absent. `/proc/sec_log` and recovery logs are TWRP records, including `init: Command 'mount functionfs adb /dev/usb-ffs/adb uid=2000,gid=2000' action=fs (/init.rc:55) took 0ms and failed: mount() failed: Device or resource busy` at captured sec_log line 4537; this is recovery init, not evidence of Android M2 failure. A read-only TWRP mount of installed VENDOR (/dev/block/sda19, ro,noload) confirmed /vendor/etc/init/unica_exynos9810_trace.rc exists with all eight intended write /dev/kmsg actions; captured copy is logs/20261023-m2-attempt49-vendor-trace/postboot/installed-vendor-trace.rc (SHA256 4da50683a0fd7aa3d64951c4961482e70e1de75deb0c44d2d6a57fd8fc627b32). This proves the trace payload was installed, not that Android init parsed or executed it. Static audit of installed /system/bin/init (read via read-only SYSTEM mount) identifies a 6,985,976-byte ELF64 AArch64 PIE for Android 36, dynamically linked through /system/bin/bootstrap/linker64. Its listed non-bionic libraries exist in the installed SYSTEM image; bootstrap libc.so, libm.so, and libdl.so also exist. This file-presence check does not execute the linker or prove runtime loading / SELinux policy success. Probe09 exact retained record is M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=1 at logs/20261023-probe09/m2-cache.log:3; no M2-INIT-EXEC-FAILED, Attempted to kill init, or second-stage init: line is present in the retained M2 records. Thus exec of /init.real was reported as successful, while the handoff to Android 16 second-stage init remains unverified. Attempt49 already included patch0095 and is the fresh test of it; repeating the same flags as a new attempt50 would not distinguish the remaining hypotheses. Audit: logs/20261023-m2-attempt49-vendor-trace/postboot/system-init-elf.txt.\n\nPostboot read-only device-side hash spot check found BOOT still at baseline `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. EFS was `76b914…` both before and after attempt49 installation, then two consecutive device-side SHA256 reads after the failed boot/TWRP return gave `a0892e10e469aa350be69fcf597a0f6b04e2a915d35d81e552e00e1f172c9040`. Cause and exact change time are unknown; Codex issued no EFS write or restore. A postboot host `dd` read was short (9,437,184/20,971,520 bytes) and was discarded. Preserve EFS as-is; do not infer a repair from hashes alone.


## 23/10/2026 — attempt49 Android 16 linker runtime check

Read-only runtime test used the installed M2 SYSTEM/VENDOR/ODM images. On G965F in TWRP, the A37 Android 36 bootstrap linker aborted before listing init, shell, or secilc, each with the exact output arc4random data MADV_WIPEONFORK failed: Invalid argument; libc: arc4random data MADV_WIPEONFORK failed: Invalid argument; Aborted; exit 134. TWRP kernel was 4.9.118-g36e6ddff22b9, so that result alone does not represent the M2 Android kernel.

To compare on the real DS-ACK runtime, copied only the attempt49 A37 linker and init binaries to M1 G965F /data/local/tmp, ran them while M1 stayed booted on 4.9.337-DS-ACK-V1.12-G965F-08.05.2026, then deleted the temporary directory (verified absent; M1 still reports sys.boot_completed=1). M1 native linker successfully listed its native init (exit 0). The A37 linker again aborted with MADV_WIPEONFORK failed: Invalid argument, exit 134, both for --list of the A37 init and for a harmless shell invocation. Thus the DS-ACK kernel returns EINVAL for a madvise operation required during A37 linker startup; this happens before ELF dependency listing or A37 init execution. It is not evidence of SIGILL, and it directly explains why /cache/unica post-fs markers cannot appear if this same A37 linker is used for second-stage init. Exact outputs and binary hashes: logs/20261023-m2-attempt49-vendor-trace/postboot/linker-runtime-check.txt, m1-linker-comparison.txt, and system-init-elf.txt.

Because Step 1 found a reproducible runtime blocker, the conditional ptrace probe10 step was not run. Ptrace would not add useful evidence before the loader can start; trapping PID1 risks hanging the test device. No kernel/vendor/radio changes, reboot, flash, or EFS operation were made. Next engineering decision is whether to add the missing MADV_WIPEONFORK behavior to the pinned DS-ACK kernel (requires a separately reviewed kernel change) or use a compatible Android 16 runtime; this diagnostic turn does not make that change.

## 23/10/2026 — direct M2 linker runtime check and M1 bootstrap overlay control

Mounted installed M2 SYSTEM/VENDOR/ODM on G965F `M2_G965F_SERIAL` read-only with `ro,noload`; the native A37 bootstrap linker again aborted before listing installed `init`, `sh` or `secilc`. Exact lines for each run were `arc4random data MADV_WIPEONFORK failed: Invalid argument`, `libc: arc4random data MADV_WIPEONFORK failed: Invalid argument`, and `Aborted`, each exit 134. This run used the TWRP 4.9.118 recovery kernel, so it confirms the exact installed binary failure but not by itself the Android DS-ACK runtime.

For a control on the same Exynos 9810 device, a temporary M1 bootstrap linker/libc/libm/libdl/libdl_android overlay was copied into TWRP tmpfs and bind-mounted over the read-only M2 SYSTEM paths. The M1 linker listed the installed A37 `init` and `sh` dependencies successfully (exit 0), and executed the harmless A37 shell command `echo M2_LOADER_SH_OK` (exit 0). `secilc --help` reached usage output under that loader but returned 1; no policy compilation or init mode was run. Temporary mounts were fully removed; no ROM partition, CACHE, EFS, kernel, vendor, radio or boot image was written, and no reboot was requested.

This confirms a plausible diagnostic path: use the compatible M1 Android16 bootstrap linker/bionic only for M2's bootstrap runtime, then build a distinct artifact and validate it before any device test. It does not establish that init's SELinux setup or second stage will succeed. Since the A37 loader failure is reproduced, the user's conditional probe10 path remains inapplicable. Exact commands/results and donor file hashes: `logs/20261023-m2-linker-runtime-step1/runtime-test.txt`. Build/flash of an overlay has not been started.

## 24/10/2026 — actual policy compilation and probe10 correction

Attempt50's actual installed A37 secilc and CIL inputs were compiled in a
read-only TWRP chroot, with output only in temporary RAM storage: exit0, output
1,528,889 bytes, SHA256 `4725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b`.
The same binary/CIL were copied temporarily to the booted M1 control and run
under its native M1 bootstrap stack on pinned DS-ACK4.9.337: exit0 and identical
policy bytes/hash. M1 temporary files were removed and boot_completed remained1.
Neither policy was loaded into a live kernel. Full local evidence is under
`logs/20261024-secilc-runtime/`; selected sanitized evidence is in `support/evidence/`.

A source audit found probe10 decoded the stop signal as `status & 0x7f` instead
of `(status >> 8) & 0xff`. The real AArch64 self-test on TWRP and DS-ACK prints
`OLD-DECODE signal=127 PTRACE_CONT errno=5`, then the corrected implementation
prints `PROBE-SIGCHLD-FORWARD-AND-EXIT-PASS` with rc0. The old monitor ignored
PTRACE_CONT failure; hence its apparent post-compile stall can be induced by
instrumentation, and is not reliable evidence that SELinux compilation stalls.
Its private namespace also lacked procfs, explaining the empty proc snapshots.

Probe11 correction source and test limits are in `diagnostics/README.md`;
no probe11 BOOT was built/flashed. M2 BOOT was already restored after probe10
and remains at pinned baseline. Next useful runtime work is a corrected trace
through compiler completion, policy loading and second-stage handoff.

## 24/10/2026 — probe11 first PID1 boot test in progress

Prepared a BOOT-only diagnostic from pinned baseline. Image SHA256
`aa78cd9aa517dfec4847ff65d843fc06aaab654911e008462659c292906b870c`,
57,671,680 bytes; full readback matches. The image audit preserves kernel/DTB/
cmdline and every CPIO entry except the new wrapper/original-init rename.
Five selftests (SIGCHLD forwarding, attach, exec-error, warm/cold private proc)
pass on recovery and DS-ACK, with unchanged parent mountinfo hashes. SIGCHLD
siginfo/status/wchan/children snapshots are enabled for this test. Target is
the verified M2 G965F; M1 remains booted. Reboot observation is in progress;
postboot log capture and baseline BOOT rollback remain pending. No SYSTEM/
VENDOR/ODM changes or fresh-data format. Local record: logs/20261024-probe11/.


## 24/10/2026 — probe11 and passive probe12 completed, BOOT restored

The preceding in-progress probe11 entry is closed. Its eight captured boot
sequences pass compiler completion (`child_status=0`) and enter kernel SELinux
initialization. User reports bootloop; recovery capture/rollback completed.
Passive probe12 then removes all PID1 ptrace operations while keeping the same
installed attempt50, kernel/DTB/cmdline and private CACHE monitor. Image SHA256
`ec061a8fe7a26cdc29fa6a1546fb2884853565dd78ef39508147d69f8c38ba60`;
image/CPIO audit and full BOOT readback pass. Namespace and exec-error tests
pass on M2 recovery; private namespace test passes on booted M1. M1 remains
booted and G965N is untouched.

Probe12 captured 14 first-stage starts, 11 `init: Loading SELinux policy`
markers and 10 `SELinux: Setting up existing superblocks.` markers. No
second-stage start, explicit fatal/panic, or load-policy success return was
captured. The user returned M2 to TWRP after a bootloop. CACHE log is 5,151,108
bytes, SHA256 `09e296536e634168903b538db977d82277c60189e9781d8ff3d03b000f3a8201`.
BOOT rollback readback matches baseline
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.
All measured protected hashes are unchanged during rollback; EFS changed
across the Android/recovery cycle only, cause unverified, with no EFS write or
restore performed. All other measured protected hashes match across the cycle.

Reason/files/verification for the passive diagnostic: isolate PID1 tracer
influence; add `diagnostics/bootlogger_pid1_probe12_passive.c` only, outside
normal ROM recipes. Static compile, actual selftests, baseline-vs-probe image
audit, controlled boot, captured evidence and rollback are complete. The loop
reproduces without PID1 ptrace. Next gate is recorder visibility through SELinux
activation, not another identical ROM build. Raw evidence remains local under
`logs/20261024-probe11/` and `logs/20261024-probe12-passive/`; selected sanitized
text/hash summaries are under `support/evidence/`.


24/10/2026 — offline recorder access check: user-space libsepol against the
actual compiled M2 policy returns `requested permission append denied by
te-rule` for kernel -> unlabeled:file, and likewise denies write. Recovery
reports both saved probe11/12 CACHE files with an unlabeled xattr. Runtime
monitor SID/enforcement/inode labels remain unconfirmed; this is a recorder
visibility hypothesis, not a captured AVC or proven bootloop cause. M1 DS-ACK
has no pmsg node, unlike recovery. No live policy load, edit, enforcement
change, device write or reboot was performed. See
`diagnostics/offline-recorder-policy.md` and selected query evidence.


24/10/2026 — probe13 M1 calibration PASS. The passive recorder now preallocates
and mmap-writes a dedicated regular CACHE file, with checksum frames and checked
msync/fsync. Actual namespace/detach/FD-close/error tests pass on recovery and
DS-ACK. M1 boots completed1/enforcing and observes recorder u:r:kernel:s0;
`KMSG_READ_ERROR=-13` and `VFS_WRITE_RC=-13` confirm runtime access loss for
ordinary I/O. A one-time correction of the recorder-private kmsg inode label
succeeds, kmsg reading resumes, and `init: init second stage started!` is saved.
Mapped output survives recovery with msync0/fsync0, heartbeat68410ms and zero
dropped frames. Thus the old recorder's visibility boundary is real on M1,
not proof of a fatal at policy loading. M1 BOOT rolled back to baseline with
full hash verification and returned to Android. M2 probe13 test is now in
progress after fresh serial/model/hash checks; no M2 result is claimed yet.
No kernel/vendor/radio/policy/normal-ROM change or data format. Local evidence:
logs/20261024-probe13/; selected evidence: support/evidence/probe13-m1-*.


24/10/2026 — probe13 completed; preceding in-progress entries are closed.
M2 persisted30 checksum-valid boot slots, all reaching second-stage init.
23 retain `Service boringssl_self_test64_vendor has 'reboot_on_failure' option
and failed, shutting down system.` and22 retain `Got shutdown_command
'reboot,boringssl-self-check-failed'`. This is an explicit reboot trigger;
the old policy boundary was a recorder failure. Normal runtime APEX linker
from installed M2 independently reproduces MADV_WIPEONFORK EINVAL/exit134 on
both recovery and pinned DS-ACK. Bootstrap-only patch0096 leaves that runtime
untouched. A37 init's linkerconfig path is in APEX, so missing system-bin
linkerconfig alone is not an established cause of its exec ENOENT.
Both BOOT rollbacks verify baseline cc618abc...776ed01d; M1 boots baseline,
M2 remains in TWRP, N untouched. Protected hashes match during rollback; EFS
changes across boot/recovery remain unverified with no EFS write/restore.
No kernel/vendor/radio/policy/self-test change or ROM rebuild. Full report,
limits and next implementation decision: docs/probe13-report.md. Selected
counts, exact lines, source hashes and loader outputs are in support/evidence/.
