# Môi trường build đã khảo sát

Ubuntu 26.04.1 LTS, khoảng 60 GiB RAM; lúc đầu khoảng 400 GiB trống.
Git LFS 3.7.1, OpenJDK/Javac 17.0.20.1, CMake 4.2.3, Clang/LLD 21.1.8,
LZ4 1.10.0, 7-Zip 26.00, patchelf 0.18.0, protoc 3.21.12 đã xác minh.
Clang host dùng dựng công cụ ROM; không phải quyết định toolchain build kernel.

Người dùng cài tool hệ thống vì sudo noninteractive trả
`sudo: interactive authentication is required`. Không gửi mật khẩu qua chat.
Đã xác minh các gói JDK/CMake/Clang/LZ4/dev libs trong tools.lock.json.

## Danh sách phụ thuộc đã đọc từ fork

`scripts/utils/build_utils.sh` kiểm executable, rồi gọi `external/make.sh`.
`scripts/build_dependencies.sh` hiện chứa cùng nội dung dựng tool. CI cài thêm
gói nhưng không là bản mô tả đầy đủ của máy Ubuntu mới.

Các executable riêng cần có: 7z, bc, brotli, clang, cmake, cpio, cwebp, ffmpeg,
getfattr, java, lz4, make, protoc, python3, rsync, xxd, zip, unzip, zstd,
cùng các tiện ích shell cơ bản. Phiên kiểm cuối trước build phải đọc lại đủ list.

`external/android-tools/vendor/CMakeLists.txt` yêu cầu pkg-config cho
libbrotlicommon/libbrotlidec/libbrotlienc, **liblz4**, libpcre2-8 và libzstd.
Protobuf bắt buộc; fmt/libusb có thể build từ source bundled theo script fork.
Gói `lz4` chỉ cung cấp executable; cần **liblz4-dev** cho header/pkg-config.
Lần kiểm gần nhất: `Package 'liblz4' not found`. Đây là prerequisite tool,
không phải lỗi kernel/ROM.

Cập nhật: liblz4-dev đã được người dùng cài và pkg-config nhận 1.10.0. Toàn bộ
dependency tool đã build xong qua script fork; --check-tools exit 0. Apktool-lib
185 tests/0 fail/0 error/0 skip sau patch URI cho test fixture. Hai patch host
được xuất trong patches/fork; không tắt test hay chuyển workspace để né lỗi.

Bộ gói cần kiểm để chuẩn bị (không phải tuyên bố đã build thành công):

```bash
sudo apt-get install -y git-lfs openjdk-17-jdk cmake clang lld lz4 liblz4-dev \
  7zip attr patchelf secilc protobuf-compiler libbrotli-dev libgtest-dev \
  libprotobuf-dev libunwind-dev libpcre2-dev libzstd-dev python3-venv \
  pkg-config brotli webp ffmpeg
```

Không chạy nguyên bước setup CI: có dọn thư mục hệ thống, đổi Git global và
ghi khóa ký. Source/submodules/LFS phải đủ và sạch trước khi build tool, vì
script dựng apktool có reset --hard và android-tools có git am patch dependency.
Không dùng công cụ host khác để bỏ qua verifier của fork.

## Bổ sung F2FS khi trích S22 thực tế

Binary tracked `scripts/internal/f2fs_unpack` của commit đã pin segfault ngay
cả với --help (exit 139); lỗi trích ở extract_fw.sh:106. Nguồn đi kèm trong
scripts/internal/f2fs_unpack_src vẫn giữ nguyên. Patch 0004 mở rộng external/make.sh
để build nguồn này vào out/tools/bin bằng Go >=1.22, CGO và liblz4-dev; extract
gọi tool vừa build và giữ output trong log. Không thay binary tracked.

Go 1.26.8 từ Ubuntu được tải dạng deb, giải nén cục bộ dưới inputs/tooling/go-root;
không cài hệ thống. Khi cần dựng lại trên máy này, thêm PATH/GOROOT và cache:

```bash
export GOROOT="$PWD/../inputs/tooling/go-root/usr/lib/go-1.26"
export PATH="$GOROOT/bin:$PATH" GOCACHE="$PWD/out/go-cache"
./scripts/build_dependencies.sh
```

Lệnh trên chạy từ rom/. Build/check-tools PASS. S22 thực tế gồm năm partition
F2FS nên không cần sudo ở nhánh trích chúng. Metadata phủ đủ các entry và 524
APK/JAR/APEX qua CRC; chưa phải xác minh boot hoặc toàn bộ byte firmware gốc.

LLVM21 đã cài nhưng /usr/bin chỉ có llvm-readelf-21. Patch0008 dùng
clang --print-prog-name=llvm-readelf để thêm bin dir đúng vào PATH, rồi kiểm
llvm-readelf như dependency bắt buộc. Clean-PATH và actual Face ABI checks PASS;
không cài thêm gói hoặc dùng GNU readelf để bỏ kiểm packed relocations.
