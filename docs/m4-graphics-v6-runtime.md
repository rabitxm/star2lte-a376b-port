# M4 Hybrid Vendor graphics V6 dependency —27 October2026

Reason: the first paired Tethering+Resolver diagnostic passes bpfloader but
fresh logcat reports SurfaceFlinger, zygote and media failing to load because
`android.hardware.graphics.common-V6-ndk.so` is absent. The retained embedded
64-bit libgralloctypes imports V6; M4's prior V7-only contract omitted that
additive dependency. This is actual runtime evidence, not a guessed HAL upgrade.

## Patch0192

Keep A37 BZI6 V7 and the Hybrid Vendor graphics mapper unchanged. Add only the
existing S22 64-bit common-V6 interface, SHA256
`010bf814b8e62ac739ff228fd6cd84477d40279c6e1c86892f064d750ee4d652`,
from the already extracted `SM-S901B_EUX/system/system/lib64` source.
The source cache/path/hash are explicit in the M4 lock. No rename of V7 to V6,
ABI shim, sepolicy relaxation, new kernel build or radio change is involved.

Affected fork files:

- a376b_android17_runtime_compat.py: stage/verify the additive interface; reject
  unreviewed source/destination bytes and symlinks. Exact M4/F/BZI6 scope remains.
- a376b_android17_runtime_compat.lock.json: separate compatibility_files entry;
  retain all BZI6 native/runtime/APEX hashes and V7.
- exynos9810_boot_restore/customize.sh: existing M4 metadata loop assigns
  root:root0644/system_lib_file to the added interface.
- scripts/internal/verify_m4_runtime_image.py: include the added interface in
  the final real-ext4 byte/owner/mode/label checks.

The M2 helper/lock and M1 profiles are unchanged. Networking remains a separate
open diagnostic; native BZI6 Tethering is still required by the normal M4 lock.

## PC verification

Private evidence: `logs/20261027-m4-graphics-v6/`.

- Helper apply/verify/idempotence pass; bad V6 bytes and symlink destinations
  reject without overwriting them; wrong milestone rejects. Python/JSON/shell
  syntax pass.
- Read-only hashes from installed SYSTEM match the retained gralloctypes and
  the actual binder_ndk/common-V2/libc++ providers. All22 non-libc strong imports
  of the V6 candidate are provided. This symbol audit is not a namespace/CFI or
  hardware guarantee.
- The actual M4 image verifier passes a256MiB SYSTEM ext4 fixture with the
  installed compiled M4 Camera APK, V6/V7, donor runtime/aconfig and NFC files.
  Deliberate V6 mode and SELinux-label corruption each fail at the actual image
  gate. This is a component fixture, not a complete ROM or production image.
  Initial fixture setup incorrectly passed Python `0o...` mode notation to
  debugfs; it left cameraserver0664 and correctly failed `M4 image mode/type
  mismatch`. Correct octal notation fixed the fixture; failure log is retained.
- Full182-patch replay from pinned fork commit produces126 byte-identical text
  files. All181 previous patch hashes remain unchanged.

## Device comparison and rollback

The next bounded trial uses the exact same paired S22 APEX and pre-existing
WIPEONFORK/probe13 BOOT plus tested vendor0189/0190/logger. Only V6 is added
relative to the previous trial. Preserve fresh CACHE evidence; verify model,
serial, block sizes, source/destination hashes, owner/mode/label before boot.
No data format. Require actual linker/core-service progression and collect
any newer Java/native error; do not call the earlier network ABI gap solved.

After capture, remove only the exact added V6 file after hash validation,
restore both original BZI6 APEX files with metadata, and restore full BOOT and
VENDOR originals. SYSTEM rollback is file-level, not raw ext4 equivalence.
No EFS/CPEFS/modem/bootloader write or restore. Retained M1/M2/M4 packages stay
untouched. Runtime result will be appended after the diagnostic finishes.

## Actual V6 device trial

The same paired-network composition plus only additive V6 was applied on the
designated G965F. All guarded writes pass; bpfloader exits0. The device returned
to recovery after170seconds without Android ADB. Fresh CACHE log has no
`common-V6-ndk.so not found` line. SurfaceFlinger actually executes into
`SurfaceFlinger::init` / display creation, then aborts:

```text
logcat_live.txt:11119 F GraphicBufferMapper: gralloc-mapper is missing
logcat_live.txt:12473 Abort message: 'gralloc-mapper is missing'
```

Zygote gets farther into ART public-library initialization, then aborts on
the separate network ABI gap:

```text
logcat_live.txt:16444 Abort message: 'Error preloading public library libandroid.so: dlopen failed: cannot locate symbol "AConnectivityNative_getNetworkBlockedReason" referenced by "/system/lib64/libandroid.so"...'
```

No Android boot or graphics/hardware acceptance is claimed. Evidence:
`logs/20261027-m4-network-v6/result/`; mmap generations35–37 are the new trial,
not older slots. The decoder validates committed frames/checksums.

## Patch0193: exact legacy mapper selection boundary

The installed libui BuildId matches BZI6
`59328e1f5a64a87fcb434f67a8250ab5`. It still contains Gralloc2/3 implementations
and matching allocator constructors; they were not removed. Disassembly at
0x326e4 calls android_get_device_api_level, compares against35 and branches to
the fatal path when the OS API is greater. Thus on this Android17/API37 image,
mapper5/4 absence skips the retained mapper3/2 fallback entirely. Do not infer
from this failure that a new HAL or kernel rebuild is required.

Patch0193 is limited to that four-byte conditional in the locked M4/F/BZI6
libui. It preserves mapper5/4 preference, then tries genuine Gralloc3/2 and
keeps their isLoaded checks plus allocator checks. It does not pretend a mapper
loaded, suppress a fatal error, change version properties or alter HAL blobs.
Source hash4bc342521a9745c3c5fa7b8bc9987e8437a043ef16bfb4a2e1f98ec6ecbf0122;
output hashac1c5a4cbfc370002d1c173d8f201a4cefe7459282180636bed5adf8ed50ade7.
At VA0x326ec, ac030054 (b.gt0x32760) becomes1f2003d5 (nop).

The same four existing M4 helper/lock/restore/image files are extended. Full
source/output hashes, ELF64 AArch64 executable-segment mapping, surrounding
instruction context and decoded branch target are mandatory before staging.
Only those four bytes differ and reversing them recreates the original.
Unknown destinations reject; M2/N scope rejects; apply/verify/idempotence pass.
The actual M4 real-ext4 gate passes the patched libui fixture and rejects
wrong mode/label.183 patches replay to126 byte-identical files;182 older
hashes unchanged. This prepares the next diagnostic; actual mapper fallback
runtime acceptance remains open until its trial.

Rollback restores the original libui bytes/metadata (or reverses0193 and its
series/lock entry on the PC) alongside the existing diagnostic rollback.
The separate networking ABI gap remains open; no network API stub returning
false success or fabricated blocked-state result was added.

V6-trial rollback completed: original two APEX bytes/metadata restored, V6
removed, full BOOT cc618abc...776ed01d and full VENDOR025f7314...c9f3 verified.
All guarded per-write protected comparisons pass. Across Android/recovery,
ODM and EFS raw hashes changed; those are separately retained and their exact
cause is not established. BOOT differs in that cross-snapshot comparison by
design because the diagnostic image was installed between the snapshots.
No direct ODM/EFS/CPEFS/modem write or restore was issued by these diagnostic
helpers. Do not equate per-write audits with across-boot raw immutability.

## Actual0193 trial and correction0194

The one-site0193 diagnostic was applied and read back, but it did not clear
SurfaceFlinger's failure. Fresh CACHE evidence in
`logs/20261027-m4-mapper-test/result/` records bpfloader exit0 (live:10353),
`GraphicBufferMapper: gralloc-mapper is missing` (live:11488) and the same zygote
native-API preload abort (live:15141). The device returned to recovery after170s;
no Android boot completed. The tombstone again locates the fatal inside
`GraphicBuffer::GraphicBuffer`, not the out-of-line mapper constructor.

A complete executable call-site audit then found three API-level calls, each
comparing35 and guarding the same legacy mapper path:

| Call / conditional VA | Site |
| --- | --- |
| 0x326e4 /0x326ec | out-of-line GraphicBufferMapper constructor; covered0193 |
| 0x332a0 /0x332a8 | inlined constructor inside GraphicBuffer; missed0193 |
| 0x586a8 /0x586b0 | preloadHal for retained mapper2/3; missed0193 |

Patch0194 corrects the coverage in the same M4 helper and lock. Per-site
executable mapping, exact context and decoded B.GT targets are verified, then
all three guards become NOP. Only12 instruction bytes differ from the original
BZI6 library. Independent decoding of every executable BL to the actual API
PLT finds exactly those three calls and verifies every guard is covered.
New full output SHA256
`ef711d73fea58a7b234ff35cf66e2ef8b7bfae37cd8923d8adf8cf751e0bb606`.
Mapper5/4 preference and actual mapper/allocator load checks remain intact.

Helper stage/verify/idempotence and replacement rejection pass. The actual
M4 real-ext4 gate accepts the new output and rejects incorrect mode/label.
184 patches replay to126 byte-identical source files;183 prior hashes remain
unchanged.0193 remains in history as an insufficient device-tested diagnostic;
0194 supersedes its generated libui output. Do not claim the12-byte output
works on the phone until the separate next trial confirms it. No kernel,
HAL/RIL blob, OS version property or networking API workaround is added here.

## Actual0194 trial — display initialization boundary cleared

Fresh CACHE evidence `logs/20261027-m4-mapper-inline-test/result/` records
bpfloader exit0 (live:10388), SurfaceFlinger using default HWComposer
(live:11465), display0 as pacesetter (live:11588), primeCache creation
(live:11606) and hardware dimensions1440x2960 (live:11611). No missing-mapper
fatal remains in that fresh log. Some older HWC connection-type queries still
return UNSUPPORTED; complete rendering/touch/hardware acceptance is not claimed.

Bootanimation and zygote instead fail on the known Android17 native symbol
(live:12694/15188). The device returns to recovery after165s without boot
completion. The next comparison deliberately reuses this measured diagnostic
BOOT/VENDOR/graphics/resolver and changes only the Tethering APEX provider.
Full rollback is deferred until that consecutive comparison's evidence is
captured; originals remain verified on PC and the guarded rollback is prepared
under logs/20261027-m4-native-provider-test. No data format or new kernel build.
