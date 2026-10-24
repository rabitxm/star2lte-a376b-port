# M2 native camera and persistent aconfig — 24 October 2026

The authorized G965F M2 test reached Android with Enforcing, retained aconfig
storage across reboots, and captured decodable JPEGs from both rear and front
cameras. These are live diagnostic results, not acceptance of a newly packaged
ROM. No complete ROM was rebuilt or overwritten in this experiment. The source
fixes extend the existing fork under M2/A376B guards.

## Camera ABI and application fixes

The fork's retained S22 cameraserver cannot link against A37 libaudioclient:

```text
CANNOT LINK EXECUTABLE "/system/bin/cameraserver": cannot locate symbol "_ZN7android11AudioSystem22streamTypeToAttributesE19audio_stream_type_t"
```

Evidence: `logs/20261024-m2-runtime-fix/m2-metadata-test/runtime/logcat-boot.txt:10703`.
Static symbol comparison also finds an OutputConfiguration constructor ABI
difference. Aliasing a similarly named function would not establish compatible
arguments or object layout. A37's native cameraserver already matches its
framework/audio libraries, so those libraries are retained.

Comparison with the original S22 server proves the fork server differs by only
eight bytes: its existing fallback for a legacy HAL reporting max_buffers=0.
The same error string and control flow were located in A37 disassembly.
Patch0099 ports that fallback at offset0x27a314, replacing
`21eeff90212c3691` with `28008052e5ffff17` (`mov w8,#1`, branch to the existing
continuation0x27a2ac). Input SHA and surrounding instructions are checked;
the output length and all other bytes are unchanged. Unknown input fails.

| Server | SHA256 |
| --- | --- |
| Original S22 donor | `ac72e751b65f04d29c6ed47ebcc681b841c610a3301d0d00f07774886bf465a6` |
| Previously installed fork server | `06d1c14dd93b39b399beb97e0eb06e0dd0f08a3da2d643a96f588c488b43d061` |
| Original A37 AZH4 | `a85c42524f3a33c9cdc327bd7435a5495ed4d60e08222d1e30b84bb20d887ee2` |
| Tested A37 adapted server | `73cba451858345a2860966cb0a7e7faff64abb4f4946193956571476e0ddad7b` |

The retained Camera APK then exposed a separate version-enum failure:

```text
java.lang.IllegalArgumentException: get : argument is invalid - 80500
    at x1.k.a(Unknown Source:29)
```

Evidence: `logs/20261024-m2-runtime-fix/m2-camera-a37-test/runtime/camera-log-final.txt:128`.
The hash-pinned enum adapter recognizes exact80500 using this APK's existing
V8_0 feature set. Other versions follow the original branches, and unknown
versions still throw. The actual system OneUI property remains80500.
Compiled DEX inspection confirms the branch. Only classes3.dex changes; all
resources, native libraries and other DEX entries remain identical. The signer
certificate is unchanged. Installed input SHA is
`db5fcc3135d879baa4415b3df5c73ea2bf3ce7b53efcf2e657f9f34c6cf878f9`;
tested signed candidate is
`f829ed7d3f1fffd79624341cba0ea2df30d533190e4e5eff38fb8294f802daa3`.

Opening the camera next identified one missing native32 compatibility wrapper:

```text
Could not load /vendor/lib/hw/gralloc.exynos9810.so ... dlopen failed: library "libGLESv1_CM.so" not found ... namespace (default)
GraphicBufferMapper: gralloc-mapper is missing
Fatal signal 6 (SIGABRT) ... camera provider
```

Evidence: `logs/20261024-m2-camera-metadata/camera-app/boot/runtime/logcat.txt:142030`.
Patch0101 adds only the pinned S22 system32 libGLESv1_CM.so to the fork's
existing native32 lock, alongside its existing EGL/GLES2/GLES3 compatibility
libraries. SHA256 is
`37c20346dcf16ec784d1734548550be8967a5eff09c9b66f7817bf7a0095e989`.
No vendor gralloc/driver, audio stack, radio blob, app_process32 or ART32 is added.

## Persistent metadata and actual flagless vendor

The fork already has an optional `/cache/metadata` bind for devices without a
metadata partition, but its final vendor baseline restoration lost that init
block. The staged file contained it while both installed init files did not.
Patch0100 reapplies the existing bind to canonical vendor `etc/init/hw/` after
restoration, immediately after the fs-stage early mount_all. It removes only
the exact duplicate block from the secondary init file if present. Donor
system aconfigd.rc stays original; the previous diagnostic tmpfs is removed.

Core init restorecons CACHE in post-fs. Twenty donor `/metadata` label entries
are therefore mirrored at `/cache/metadata`, preserving the original aconfig
types through either path. No new partition or filesystem is introduced.
M2 enables the existing opt-in; M1 explicitly resets it false. Unknown anchors
or preexisting unreviewed label/mount definitions fail before writing files.

Two successive normal boots reached boot_completed1 under Enforcing. Actual
`/metadata` is `/dev/block/sda21` ext4, the existing CACHE partition. A marker
written through `/metadata/aconfig` kept the same dev66309/inode13 and SHA256
`66ad8b86a1b006d4a45c4286f783c5c8367e81eeda3c135c37277ef85b219fe6`
through reboot; the CACHE alias addresses the same inode. Evidence:
`persistent-boot1/`, `persistent-boot2/` and `capture-smoke/final-state.txt`
under `logs/20261024-m2-camera-metadata/`.

The legacy Hybrid Vendor has no aconfig packages. Previously donor aconfigd
initialized other containers then failed:

```text
Failed to initialize platform storage in post-fs: fail to get storage files for vendor
```

Patch0100 describes the actual zero-package/zero-flag vendor using valid empty
v2 package.map, flag.map, flag.val and flag.info files. It copies no A37 hardware
flags, invents no flag values, and refuses unknown/nonempty vendor storage.
The serialization follows the existing AOSP platform/build source pinned at
`b815dded1eafbf06191a6ae306956bb6ed6fb415`, under
`rom/external/signapk/external/platform_build/tools/aconfig/aconfig_storage_file/src/`.
Independent upstream Java readers parse counts0 and absent lookup returns null.
Files are mode0444 with vendor_aconfig_storage_file labels registered through
the existing fork metadata helper. Actual donor runtime confirms success:

```text
init: Service 'system_aconfigd_platform_init' (pid 897) exited with status 0
```

Evidence: `empty-vendor-boot/runtime/logcat.txt:6054`. The former missing-vendor
platform-init error is absent. RANGING remains registered as dangerous with the
original enabled/default/read-only donor flag; no permission check is bypassed.

## Hardware smoke test and limits

Final test retained SystemServer1713 and cameraserver1486 through uptime397.34s,
with boot_completed1 and Enforcing. Three public camera devices enumerate;
the underlying legacy provider reports seven HAL entries. Rear device0 and
front device1 both produced preview buffers delivered through cameraserver to
SurfaceFlinger. The final dump contains zero camera error traces.

Rear preview evidence: `gles32-awake/logcat.txt:9226`–`:9232` under the same
local log directory. Front preview: `capture-smoke/front-logcat.txt:9958`.
The first immediate post-boot launch was paused by Setup Wizard, and a later
launch while the display slept was rejected by camera policy. Waking the
display and reopening Camera produced a stable session; these lifecycle events
are not evidence of a failed HAL stream configuration. No policy was disabled.

| Test | Result |
| --- | --- |
| Rear preview/device0 | First HAL buffer and SurfaceView frame observed |
| Rear still photo | JPEG4032×3024, 3,275,539 bytes, full PC decode PASS |
| Front preview/device1 | First HAL buffer observed; active client retained |
| Front still photo | JPEG2640×1980, 1,723,797 bytes, full PC decode PASS |
| Platform aconfig initialization | Actual service exits0 |
| Metadata persistence | Same marker hash/inode across normal reboot |
| Source replay | 91 patches / 72 touched paths byte-identical |

Photos and raw logs stay local. Camera still logs unsupported optional vendor
tags; do not claim all modes work. Video, microphones/audio playback, telephoto,
flash, portrait/night modes, long sessions and camera lifecycle after Gallery
remain unaccepted. Audio service dumps were captured, but this is not an audio
hardware test. The arm64-only application ABI remains selected.

CACHE clearing/recovery wipes, OTA/staged flag updates and other metadata
consumers need explicit lifecycle tests. Boot dependencies from the preceding
diagnostics remain required: isolated WIPEONFORK BOOT b01912f6..., S22 Tethering
APEX601d6c..., and system64 graphics V6 interface010bf8.... They are not silently
selected as production inputs by patches0099–0101. A fresh build/package of the
complete combination, ADB default/switch acceptance, connectivity and remaining
F/N hardware tests are still required. No Android17 or G965N claim follows.

## Verification, files and rollback

Patch0099 changes existing native-camera finalization and adds two adapters.
Patch0100 changes existing boot_restore finalization, M1/M2 profiles and adds
two storage adapters. Patch0101 changes one native32 lock entry. Each patch
header records reason, affected files, checks and rollback. Full SHA locks are
updated in sources.lock.json. Helper idempotence, wrong-input rejection, Bash
syntax, compiled enum and M2-to-M1 profile reset pass. Full patch replay matches
the modified fork; no replacement build system was introduced.

The test device was selected by serial/model/bootloader and expected hashes
before writes. All live edits were rolled back: original Camera APK/server,
vendor init, plat_file_contexts, aconfigd.rc and A37 Tethering APEX restored;
new GLES1/V6 libraries and four generated vendor files removed. BOOT readback
returned to `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.
Protected partition hashes were unchanged during the BOOT rollback write;
that snapshot is not a claim that Android never updates EFS while running.
M2 remains in TWRP. M1 and G965N were not operated on. No USERDATA format,
ODM, modem/bootloader, EFS/CPEFS or radio write was issued.

`/cache/metadata` was absent before this experiment. Its new contents were
pulled before cleanup; all215 file hashes matched the local backup before
removing only that test directory. metadata-cache-cleanup.json records PASS.
Source patches and diagnostic inputs remain available
locally for the next retained build. Existing complete ROM archives are intact.
