# M1 — cấu hình đối chứng G965F đã chuẩn bị

Người dùng đã chọn gói nguyên bản duhansysl ngày 08/05/2026,
`DS-ACK-V1.12-08.05.2026-Enforcing-OneUI7.zip`, không KernelSU.
SHA-256: `9a6bad44f3a1a14d43f108fcebeeedfc1b924685f7f13931d902077bceb5c072`.
Gói đã tải, CRC/digest khớp. Không sửa/build kernel.

## Profile trong fork

Trong `rom/`:

```bash
source target/star2lte/profiles/m1-g965f-no-ksu.sh
source buildenv.sh --fs-type ext4 star2lte
```

Hai lệnh chuẩn bị config đã kiểm chứng. Profile giữ donor S22, SDK36 và Hybrid
Vendor; chọn kernel/expected hash qua các hook sẵn có, ext4 và model guard chỉ
SM-G965F/F-DS/FD. Không tạo target universal để cài N ở lượt thử F.

Input `EXYNOS9810_BUILD_KERNELSU_RUNTIME=false` được platform chuyển thành
output config `EXYNOS9810_ENABLE_KERNELSU_RUNTIME=false`. Cần tên input riêng vì
buildenv xóa biến của config cũ trước tái sinh. Finalization loại runtime,
manager APK/first-boot installer, ksud, ksu_crown, decoded cache và metadata còn
sót; verifier yêu cầu vắng các thành phần này. Mặc định fork vẫn giữ KernelSU.
Không đụng dữ liệu ứng dụng trên điện thoại.

Đã kiểm config mặc định/profile F, chặn chọn N với firmware F, boolean sai;
kiểm verifier ở cả profile mặc định và không KernelSU; kiểm actual staging
function trên fixture có metadata/cache. Full ROM/package/boot: **NOT_TESTED**.

## Đầu vào và quyền trích xuất

Samsung version.xml với header của samloader xác nhận:
- S22 EUX: S901BXXSOGZH3/S901BOXMOGZH3/S901BXXSOGZH3, thuộc Android16.
- G965F PHN của fork: G965FXXUHFVG4/G965FOXMHFVB4/G965FXXUHFVB4, Android10.

G965F thật đang báo bootloader/vendor FVG6; không coi package PHN FVG4 là dump
byte-identical của máy. Đây là target firmware mặc định của fork cho đối chứng.
Giữ modem đang cài, không dùng CP/BL của package để thay trên điện thoại.

`scripts/download_fw.sh` đang được dùng để tải/kiểm MD5 đầu vào trên PC.
S22 đã tải/giải mã và kiểm MD5 cả năm file Odin thành công, có `.downloaded`.
Lượt target G965F/PHN đầu tiên dừng ở `DownloadBinaryInform returned 408, firmware could
not be found?` (`download-fw.txt:31`); sau khi người dùng cho phép dùng Samsung
serial thật, FUS trả Status=200; gói đã tải/giải mã và kiểm MD5 năm file Odin PASS.
SHA256 từng file ở g965f-downloaded-manifest.json. Giá trị định danh
không đưa vào tài liệu/Git/chat.

Bước kế tiếp theo fork là `./scripts/extract_fw.sh`. Đã đọc code ở dòng
111–157: script yêu cầu sudo, mount/fuse image đọc-only, chép file và đọc UID/GID,
SELinux label để tạo fs_config/file_context. Không thay bằng giải nén thiếu
metadata rồi tuyên bố input sẵn sàng. Công cụ Codex chưa tự xác thực được sudo;
khi đầu vào tải xong cần người dùng chạy bước này trong terminal có nhập password.
Chỉ yêu cầu thao tác PC, không liên quan flash điện thoại.

**S22 đã trích xong; không cần chạy lại khối lệnh dưới.** Lượt người dùng
chạy ban đầu dừng ở binary F2FS tracked (exit139). Patch0004 dựng nguồn đi kèm,
và lượt trích lại đã thành công: metadata đủ năm partition, CRC524 archive PASS.
Giữ lệnh dưới làm tham chiếu; sudo chỉ cần khi nhánh mount/fuse được sử dụng:

```bash
cd '${PROJECT_ROOT}/rom'
source target/star2lte/profiles/m1-g965f-no-ksu.sh && source buildenv.sh --fs-type ext4 star2lte
sudo -v
./scripts/extract_fw.sh --ignore-target > ../logs/20261020-m0-buildhost/extract-s22.txt 2>&1
```

Không chạy cả buildenv bằng sudo. Nhập mật khẩu chỉ ở terminal PC. Sau lệnh,
kiểm exit code/log và `.extracted` cùng fs_config/file_context trước bước build.

Chưa chạy `make_rom`. Mọi lần tạo ZIP phải kiểm model guard, hash kernel,
không có runtime KernelSU, dung lượng/DTB/filesystem và installer. Installer gốc
có auto-repartition; chưa cho phép chạy trên máy thật. Lượt flash do người dùng
thực hiện sau review riêng artifact và đường khôi phục, rồi gửi log theo protocol.

## G965N

Override mới `STAR2LTE_TARGET_FIRMWARE` cho phép khai báo nguồn N mà không xóa
nguồn F. `STAR2LTE_BUILD_MODEL=SM-G965N` từ chối firmware F. Đây chỉ là guard
nguồn/model; chưa phải port N hoàn chỉnh. Còn phải kiểm blob/CSC/RIL N và các
chỗ nhận diện/boot-contract F trong finalization, rồi nghiệm thu riêng bằng log.


## Bước hiện tại: trích G965F trên PC

Gói FVG4 đã đủ. Lượt Codex trích dừng ở `extract-g965f.txt:20`:
`Root permissions are required to unpack OS partitions`. System/vendor ext4 đã
được tách, chưa có `.extracted`. Không thay bằng metadata đoán hoặc marker giả.
Người dùng chạy từ terminal PC, rồi báo exit code; không dán mật khẩu/config:

```bash
cd '${PROJECT_ROOT}/rom' &&
source target/star2lte/profiles/m1-g965f-no-ksu.sh &&
source buildenv.sh --fs-type ext4 star2lte > ../logs/20261020-m0-buildhost/buildenv-extract-g965f-user.txt &&
sudo -v &&
./scripts/extract_fw.sh --ignore-source > ../logs/20261020-m0-buildhost/extract-g965f-user.txt 2>&1
echo "exit=$?"
```

Legacy boot metadata fallback xuất hiện với boot/recovery F, là nhánh có sẵn
của fork; không phải bằng chứng cần sửa/build kernel. Gói kernel M1 vẫn là
DS-ACK nguyên bản đã pin, không lấy kernel stock vừa trích để thay lựa chọn.


## Kết quả trích target và khởi chạy build

Người dùng đã trích bằng sudo. Dòng cuối `No device model value found in "echo"`
là đối số thừa sau khi target đã hoàn tất: marker đúng FVG4, metadata system/vendor
phủ đủ entry, CRC522 archive PASS. Không cần trích lại. `.lost+found` đã bị fork
loại khỏi tree nhưng vẫn có metadata nguồn; audit ghi nhận ngoại lệ này riêng.

Đã khởi chạy trên PC bằng profile ở trên:
`./scripts/make_rom.sh -z --fs-type ext4`, log m1-build.txt. Chưa coi build thành
công trước các verifier và audit ZIP/images; chưa có thử boot hay flash.

Lượt M1 đầu dừng ở rootfs copy do quoting debugfs (m1-build-attempt1.txt:28).
Patch0005 đã kiểm năm file rootfs/hash/metadata; đang chạy lại với
--clean-work-dir để không dùng cache sinh dở. Không đổi donor/vendor/kernel.

Lượt2 dừng ở framework tag rỗng (m1-build-attempt2.txt:58) do GET_PROP tách
đường dẫn tại dấu cách; patch0006 đã kiểm tám case. Lượt3 clean work_dir,
UN1CA_APKTOOL_JOBS=4/THREADS=2 trên host60GiB; donor/vendor/kernel không đổi.

Lượt3 dừng khi modern unpack_bootimg không đọc được header stock legacy
(UnicodeDecodeError); đồng thời log cho thấy Bluetooth dump thiếu file do path.
Patch0007 có kiểm actual blocks và bounds/ELF/hash/metadata, giữ nguyên boot
input và DS-ACK. Lượt4 tiếp tục clean work_dir; không bỏ verifier hay patch
phần cứng để vượt build. Xem binary-extraction-checks.json.

Lượt4 dừng ở tên llvm-readelf trong PATH; binary LLVM21 đã có. Patch0008
discovery compiler-relative và preflight PASS. Rà module cuối phát hiện BOOT
KernelSU hardcoded: patch0009 tách nhánh false sang template kernel-less cùng
hash mà platform installer vốn sử dụng. Kernel thực vẫn lấy nguyên ZIP May,
không build/patch payload. Default true giữ hash cũ; các case được kiểm riêng.
Lượt5 clean work_dir, giữ mọi verifier; dynamic app inputs được snapshot/hash
cục bộ trong m1-network-inputs.json để kiểm provenance.

Lượt5 đã rebuild/ký APK/JAR, giữ15715 Android16 classes và qua boot contract,
release-default và SELinux compile. Package bị chặn bởi một __pycache__ sinh
sau modules: audit tái tạo đúng hash113968... vàb5f773... chỉ bằng cache đó.
Patch0010 bỏ cache khỏi input hash, giữ guard và real inputs; lượt6 chạy
--package-staged chính thức, rerun các verifier, không rebuild lại APK/JAR.

Lượt6 dựng image ext4 và target-files xong, nhưng 7z dừng do path archive
chưa quote ở build_full_ota_zip.sh:301. Patch0011 kiểm hai lệnh thật trong
path đặc biệt PASS. Lượt7 tự dừng trước package để sửa assert: codename
star2lte dùng chung F/N, fallback có thể vượt model-list. Patch0012 bỏ
fallback riêng star2lte khi có model-list; không đổi các target khác.
Lượt8 package-staged, tái dùng target-files có hash/CRC audit riêng; không đổi
APK/JAR, donor, vendor hoặc kernel giữa các lượt đóng gói.

Lượt8 hoàn tất exit0: target-files, flashable ZIP và verifier của fork PASS.
Audit độc lập CRC/sparse/ext4/model/order/kernel hash PASS; CMS whole-file OTA
signature PASS với testkey fork. ZIP 5479024250 byte,
SHA256 `ced0ee387a91a5e6cf908955ecb33f4611f47a4457f676256255f2d90b25207d`.
Chi tiết và tác động dữ liệu tại docs/m1-artifact-review.md. Chưa boot; chờ
người dùng chốt backup USERDATA/chấp thuận repartition/data format và phương
án khôi phục. Không tự chạy installer trên điện thoại.
