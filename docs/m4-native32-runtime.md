# M4 BZI6 native32/runtime checkpoint0129 — 26 October 2026

The native32 slice now has an independent M4 lock and is staged and checked by
existing fork entrypoints. This is PC source/APEX/ext4-fixture evidence, not a
complete M4 ROM, ready runtime/kernel profile, image acceptance or boot result.
M2 attempt56 remains the reference. No phone operation, kernel build/change,
vendor/radio binary change or retained-release overwrite occurred.

## Selected inputs and exact output

| Input/output | SHA256 |
| --- | --- |
| BZI6 original com.android.runtime.apex | `f308d89271dbf319dd79b833e41b850bf8de36e96efced954a7d85f7013036c9` |
| Pinned S22 runtime APEX, native32 source | `f3764e9425e8295b2ffe40d5d3520c0aa11feb8a630882b35f17b4de0a64c574` |
| Composed BZI6 runtime APEX | `b6b559bf52dfb07d4c8f1d073b037f1d319b0366e3a732c3b56bbbb55583cdaf` |
| Existing payload public-key identity | `fedf8b2943754e4848f9d3edb25f228adec6a6e26d5b7fa8816851c0cd7155dd` |
| Existing container certificate identity | `ee98540809da3112f84e0e3ab1ae13b9108b5121dd5c2f0c7bea8851e2cf272a` |

The fork's existing `a376b_native32.py::build` regenerates the same BZI6 output
as the earlier preflight. Every original A37 payload file/symlink is unchanged;
only17 S22 paths are added: the lib/lib-bionic directories, three32-bit
executables and12 runtime32 libraries. The manifest, build-info, NOTICE,
resources and AndroidManifest container entries are byte-identical to A37.
AVB payload verification passes and signapk/apksig report
`verified=true v2=false v3=true errors=[]`; payload offset is4096 and uncompressed.
An independent debugfs check confirms UID/GID/mode/SELinux on all45 payload
paths described by fs_config. This does not prove activation by A17 apexd.
The existing local signing identity is reused explicitly; private keys stay
under ignored keys/m2-native32-apex and never enter the source patch.

The separate `a376b_android17_native32.lock.json` retains the already pinned
48 S22 platform32 library hashes and the fork's radio-wrapper hash. All48 are
ELF32 little-endian ARM; DT_NEEDED records are in the local inventory. The
M2 native32/runtime locks remain byte-identical. Application ABI stays
arm64-v8a, zygote64, with no app_process32 or ART32. Native32 exists for legacy
hardware processes; its full Android17 namespace/HAL operation still requires
runtime logs.

## Existing staging and release/image entrypoints

The existing restore module selects the BZI6 lock and a separate
`out/compat/a376b-android17-native32` cache, then uses the same builder and
metadata/property functions as M2. No new build system or standalone ROM
builder was added. Exact M4/F/BZI6/SDK37/target31/board33/shipping36→26/ext4/
no-KSU scope and EXYNOS9810_M4_RUNTIME_COMPAT=true are required. M2 runtime or
bootstrap flags reject. Selecting M1 or M2 resets the new M4 runtime flag.

The existing release-default verifier calls native32 verification in its A17
branch. Its compiled camera/media/product/Settings/ADB gates remain present.
The real ext4 verifier calls the new native32 image subcommand for M4 SYSTEM:
50 locked files (APEX +48 platform32 +radio wrapper), exact bytes, mode0644,
root ownership and SELinux contexts. It also checks build.prop/app_process64/
zygote64 init bytes, app_process32 absence and the exact linker symlink. This
is the native32 slice only; full M4 runtime/vendor/boot/image contracts remain
open and cannot be inferred from this gate.

## Verification and regression evidence

Local evidence: `logs/20261026-m4-runtime/`.

- native32-apex-compose.json and native32-apex-metadata-test.json: repeatable
  exact output, preserved original bytes/container entries,45 metadata paths.
- native32-test.txt: actual M4 apply from an empty cache, idempotence,10 wrong
  scope negatives and bad-APEX rejection before staging. The actual AZH4 M2
  apply/verify produces byte-identical output to the pre-0129 helper.
- native32-shell-test.txt: extracted real restore branch selects the correct
  lock/cache and all48 metadata rows for both donors. Actual M1/M2 profile
  sources clear the M4 flag.
- native32-image-test.txt / native32-test.txt: real128MiB ext4 fixture passes;
  changed file mode and changed SELinux label are rejected. The first fixture
  attempt omitted lost+found metadata and failed with
  `set_selinux_xattr: No such file or directory searching for label "/lost+found"`.
  Fixture metadata was corrected; the complete test then passed. This was a
  fixture-construction error, not a donor/phone failure.
- native32-image-hook-test.txt: actual verify_exynos9810_images.sh reaches the
  M4 native32 gate on that ext4 fixture, including its libPSI/libpsi check.
- replay-through-0129.txt:119 ordered patches from pinned2b25ae6,102 resulting
  text files byte-identical to active source; all118 earlier hashes unchanged.
  Shell syntax, Python AST/JSON and git diff whitespace checks pass.

The ext4 fixture includes only the tested native32/app64 slice and two PSI
inputs needed by the existing image entrypoint. It is not a generated M4 ROM
SYSTEM image. Source/fixtures remain separate from runtime acceptance.

## Runtime exceptions audited next

BZI6 Tethering CAPEX hash is
`540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d`.
Extracted netbpfload contains live code references for kernel checks:
25Q2 message at0xa580,25Q4 at0xa5b4,26Q3 at0xa5dc. The last one logs
`Android 26Q3 requires kernel 5.15.` and follows a conditional kernel compare
at0xa5bc, boolean load0xa5c8 and shared return7 path. Its flag is initialized
at0x1203c–0x1204c with full-API >0xe7d (3709), rather than unconditionally.
The original BZI6 property is sdk_full37.0 and netbpfload.rc starts
`# 2026 2 37 0 0 #`; the 5.15 string therefore does not establish an active
5.15 requirement on this donor. The older conditional5.4/5.10 checks are also
present. This is disassembly/preimage evidence, not a captured M4 boot error.
Do not raise the kernel version or change kernel source on that basis.

BZI6 lacks system64 graphics-common-V6; its original interface set and the
pinned S22 candidate must be reviewed with actual consumer imports. In
particular, the current fork S22 cameraserver imports these two symbols that
BZI6 libaudioclient does not export:

```
_ZN7android11AudioSystem22streamTypeToAttributesE19audio_stream_type_t
_ZN7android11AudioSystem13getParametersEiRKNS_7String8E
```

The audit records dynsym evidence and BZI6 cameraserver/aconfigd/account hashes
in bzi6-runtime-input-audit.json and related files. This is a static ABI
mismatch; no M4 CANNOT LINK EXECUTABLE log exists yet. The AZH4 camera binary
patch is still AZH4-only. Do not transplant it or silently select the S22
server. Next work is the separate M4 Tethering/graphics/aconfig/account path
and A17 camera ABI adaptation, followed by remaining mod dispatch, complete
boot/vendor/image contracts, profile and clean-build/compiled/fresh-boot gates.
ADB/Auto Blocker and the selected Hybrid Vendor/radio contract remain mandatory.

## Patch and rollback

Patch0129 changes six source files: native32 helper, new M4 lock, existing
restore customizer, release-default verifier, image verifier and M1 base
profile. Reason: prevent the Android17 donor using the AZH4 APEX hash/cache and
verify native32 retention in the packed image. Rollback removes0129 and its
series/lock entry, restores the five existing text files to base+0128 and
removes only the new M4 lock. Retained ROMs/firmware caches are untouched.
The M4 composed APEX remains a private fixture, not a public firmware release.


# M4 donor-native runtime closure — checkpoint0130, 26 October 2026

Patch0130 continues the separate BZI6/Android17 path through the fork's existing
restore, finalization, release-default and ext4-image gates. It pins and stages
the donor's cameraserver, SamsungCamera, AudioSystem providers, graphics-common
V7, BZI6 Tethering CAPEX, aconfigd.rc and four system aconfig-v2 tables. It
preserves the matching A37 camera/audio/graphics ABI together; the Android16
S22 cameraserver/APEX and graphics V6 exception are not used for M4. The actual
BZI6 vendor_kg passwd/group rows (UID/GID5959) are merged only after collision
checks. Because BZI6 has no vendor aconfig source directory, the helper stages
the four correctly versioned empty vendor tables and pins their bytes/metadata.

`a376b_metadata_bind.py` now accepts an explicit M2/M4 marker selection. M4
installs its persistent CACHE-to-/metadata bind after vendor baseline restore,
then emits M4-specific `/cache` label aliases. M2 remains the default path and
its generated init/file-context outputs compare byte-for-byte with the
pre-0130 helper.

Patch0130 files and reason:

- `a376b_android17_runtime_compat.py` and its lock: exact source hashes, BZI6
  marker, aconfig-v2 provenance, account merge, and strict M4 scope; stage and
  verify donor-native runtime files without replacing other camera-stack files.
- `a376b_metadata_bind.py` and boot-restore `customize.sh`: retain persistent
  metadata for M4 after vendor restore and set explicit file metadata.
- finalization `customize.sh`: keep BZI6 camera app/server and camera-related
  behavior donor-native; skip M2/S22 camera binary, UHD60 and front-FOV edits
  for this M4 selection. `adb_defaults.py` now has a strict BZI6 lifecycle
  adapter: read stored `adb_enabled` before setting AdbService USB state, while
  retaining the USB property as the fallback and seeding settings only once.
  The source lock also pins BZI6 `services.jar` and `SettingsProvider.apk`.
- `verify_exynos9810_release_defaults.py`, new `verify_m4_runtime_image.py`,
  and `verify_exynos9810_images.sh`: run the work-tree and packed ext4 checks
  from existing fork release/image entrypoints.

Verification evidence is in `logs/20261026-m4-runtime-0130/`:

- `runtime-selftest.txt`: locked BZI6 inputs; staged donor-native camera/audio/
  V7/Tethering, vendor_kg and aconfig-v2; idempotence and collision/overwrite
  negatives all pass.
- `runtime-image-fixture-result.json`: actual128MiB ext4 SYSTEM and VENDOR
  fixture passes; wrong camera mode and wrong SELinux label are rejected.
- `m2-metadata-regression.txt`: all three generated M2 files match the
  pre-0130 helper byte-for-byte.
- `runtime-selftest-final.txt`: final donor/helper positive and negative tests, Python AST, `bash -n`, `git diff --check`, and source-lock hash verification pass.
- `adb-final-retest.txt`: the actual BZI6 SettingsProvider APK and `services.jar`
  were decoded with the fork Apktool, patched, rebuilt, and accepted by the compiled
  verifier. Source layouts are `.locals 12` and `.locals 6`; compiled seed/no-RSA/
  stored-choice checks pass. `adb-preflight-status.txt` records the observer and UI checks. BZI6 Rampart, SecSettings and SUW
  retain the main/strict/ADB/USB keys and separate ON/OFF actions; the settings
  observer still forwards `adb_enabled` changes. AZH4 M2 outputs are byte-identical.
- `replay-through-0130.txt` / `replay-result.json`: the final patch0130 including
  the BZI6 ADB adapter passes all120 ordered patches
  from pinned commit `2b25ae6fc3098650be249bd1898e21e563cd5df1`;105 resulting text
  files match byte-for-byte and all119 older patch hashes stay unchanged.
- The lock pins camera server SHA256
  `45a6c90b8a6a2b157c747f41a9e08fe7dcfa7a527c85a7403039516dd672d3cf`, graphics
  common V7 SHA256 `20da3d67521e859760936e22c0b868544331d93799a7a891f3a785f2acf420d4`,
  and BZI6 Tethering CAPEX SHA256
  `540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d`.

This does not demonstrate Android17 runtime success. No M4 profile or complete
ROM has been built; no device was flashed or booted. Aconfig initialization,
Tethering/BPF, graphics, camera/audio hardware, the Android17 native namespaces,
ADB/Auto Blocker fresh defaults and live toggles, full vendor/boot contracts,
and separate G965F/G965N acceptance still require a complete build and runtime
logs. The source/DEX fixture confirms only that the reviewed default seeds,
no-RSA properties and stored-state lifecycle compile in the donor layout; it is
not a fresh-data or live-device test.
No kernel, Hybrid Vendor, radio, modem, EFS/CPEFS, or retained release was
changed. Rollback is to remove patch0130 and its series/source-lock entry, then
replay through0129; existing M2 files/artifacts and donor caches remain intact.

