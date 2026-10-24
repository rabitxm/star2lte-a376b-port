# M2 runtime APEX / MADV_WIPEONFORK diagnostic backport

24 October 2026. This is an isolated kernel compatibility experiment prompted
by captured runtime evidence. It does not change the normal ROM kernel default,
the 88-patch ROM series, A37 runtime APEX, BoringSSL test, policy, hardware blobs
or radio bridge. M2 is not accepted as booting at this point.

## Reason and controlled comparison

Probe13 captures `boringssl_self_test64_vendor` SIGABRT and the explicit
`reboot,boringssl-self-check-failed` command. Its actual A37 runtime linker also
reproduces `arc4random data MADV_WIPEONFORK failed: Invalid argument`, exit134,
on the original pinned DS-ACK kernel. This is the evidence that permits a
limited kernel change under the project rules. A success stub would lose the
zero-on-fork security semantics; disabling the cryptographic self-test would
hide the observed failure. Neither is used.

Use the existing DS-ACK source Makefile and its preset machinery. First build
an unpatched source baseline, then apply the three patches and rebuild with
identical real .config/compiler. Package both using exactly the probe13
ramdisk, original DTB/cmdline and existing MagiskBoot. Calibrate the baseline
and backport on M1 before one controlled M2 test. Every write is restricted
to BOOT, guarded by current-image hash and serial/model/partition verification;
rollback is the retained full image SHA256
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.

## Source, config and toolchain provenance

- DS-ACK: [duhansysl/exynos9810-kernel](https://github.com/duhansysl/exynos9810-kernel),
  commit `914eabe7b6799ab0eb159360a5f9731dd50d0f1d`.
- Existing input source had a six-file draft WIPEONFORK diff; preserve it and
  export a fresh clean source from the pinned commit. Apply only
  `patches/kernel/series` to the isolated copy.
- Kernel Makefile rejects spaces in its main path. The isolated source/output
  are placed under the user's cache, recorded in the local build-path manifest;
  the original input tree and firmware remain untouched.
- `kernel/Makefile:121` builds config_data.gz from **stock_defconfig**, not
  the actual `.config`. The live M1 `/proc/config.gz` is byte-identical to that
  embedded file. It cannot reproduce the actual release build configuration.
  Preserve this source behavior during this comparison; use the real output
  `.config` and manifest instead. Do not claim the new kernel config is a
  byte-for-byte reproduction of the original binary's build config.
- Select the pinned source's exynos9810, star2lte and apollo preset fragments,
  normalize CRLF, merge through existing `scripts/kconfig/merge_config.sh`,
  and select STAR2LTE_EUR_OPEN, Mali R38P1, KSU=n, ALWAYS_PERMISSIVE=n through
  existing scripts/config; use ANDROID_MAJOR_VERSION=q as in its build script.
  Original Image contains Mali `bv_r38p1`; the embedded stock config's older
  GPU selection is another reason to reject it as build truth.
- Real final config SHA256:
  `e71a7e50519aec68eb2208a0a25f4eb1319f18501e00183832be4fc016bc85b6`.
  Both images use the same config. Their exported CRC audit compares 10,189
  symbols with no differences; neither has `ksu_` symbols.
- The tree has a pinned KernelSU-Next gitlink
  `1ce19e536de730829b62d0d23dbd96c6ec809e2f`; the exact source is present only
  so Kconfig can parse its unconditional source line. KSU is disabled, not
  installed or enabled on the phones. No external setup script is run.
- Official AOSP clang-r547379, prebuilts commit
  `2d4dcbff7cb42f360f368bacf51cfe711fd1cb0f`, Android build13290119,
  LLVM source `b718bcaf8c198c82f3021447d943401e3ab5bd54`.
  Archive SHA256 `ef475266981f2c676384429bda896dbb43bf1e623221e1bd958f35cdc5fae006`.
  LLVM source matches the original compiler; its build ID differs from original
  13174946. Thus this is a rebuild with an explicit control, not binary reproduction.
- Keep release string `4.9.337-DS-ACK-V1.12-G965F-08.05.2026` unchanged.
  New artifacts have separate SHA256 and `codex@star2lte-port` build metadata.
  Do not identify loaded images from uname-r alone.

## Patch record

| Patch | Reason and affected files | Verification |
| --- | --- | --- |
| 0000 | Reserve legacy VMA bit; `arch/x86/Kconfig`, `include/linux/mm.h` | Upstream prerequisite, corrected high-bit mask, replay and ARM64 build |
| 0001 | Real WIPE/KEEPONFORK semantics; proc flags, mm headers, fork, madvise | Replay/build; parent/child/grandchild and rejection tests on actual DS-ACK |
| 0002 | Defer child anon_vma setup until fault; `kernel/fork.c` | Upstream follow-up; repeated nested fork and KEEP tests |

Upstream references: [feature d2cd9ede](https://github.com/torvalds/linux/commit/d2cd9ede6e193dd7d88b6d27399e96229a551b19),
[prerequisite df3735c5](https://github.com/torvalds/linux/commit/df3735c5b40fad8d0d28eb8ab065fe955b3347ee),
[anon_vma correction 93949bb2](https://github.com/torvalds/linux/commit/93949bb21b52aae924fd26f9775c8655dd9e885e).
The x86 high-bit definition uses the Linux v4.14 mask form rather than the
initial upstream bit-index typo. No x86 runtime is being certified.

## Artifacts and gates

| Artifact | SHA256 |
| --- | --- |
| Unpatched rebuilt Image | `49e8130721e7422a9b7c1a581a323d756cb1fb4c3636a7937e52c40c63467e77` |
| Backport Image | `92b14be170225954476869d1fd7700f9c2c92d98eb6447d0181c50a11e80fcdd` |
| Unpatched rebuilt diagnostic BOOT | `499f171d6819dd7b2bb46a51049e890e0d3c8d1ce4dfd163893ab42c4d5775d3` |
| Backport diagnostic BOOT | `b01912f62ae111570d9de3e45d799da3da4788f27d86f119f78a06a9efb8abd7` |

Each Image is 37,605,888 bytes. BOOT is 57,671,680 bytes; CPIO, DTB and cmdline
are identical across the two candidates and the known probe13 diagnostic.
Local evidence is under `logs/20261024-m2-runtime-fix/`; binary artifacts stay
local and are not exported to the source assistance repository.

Build and static gates PASS, including the original source's FIPS integrity
step. M1 on the rebuilt unpatched baseline reaches boot_completed1/enforcing
and still returns WIPEONFORK -22 (self-test rc77), as expected. Backport runtime
calibration is in progress. The standalone raw AArch64 test performs 32 rounds
of child and grandchild zero-fill, parent preservation, an unaffected adjacent
page, KEEPONFORK undo, shared-map rejection and private-file-map rejection.
It must actually pass on the test kernel before an M2 boot test is accepted.

Normal ROM packaging remains unchanged. No fresh format, repartition, kernel
module replacement, EFS/CPEFS/modem/bootloader write or G965N test is part of
this experiment. A successful M1 calibration does not prove M2 or N boots;
APEX/linkerconfig ordering and subsequent service failures require fresh logs.

## M2 runtime result — next blocking requirement

The audited feature BOOT was tested on G965F M2. Probe13 generation33 is
checksum-valid with 6,446 frames through heartbeat 17,773ms and no dropped
frames. It records `init second stage started!`; at ms2493 init logs
`linkerconfig generated /linkerconfig with mounted APEX modules info`. Both
`boringssl_self_test64_vendor` and `boringssl_self_test64` exit status0 at
ms2647 and ms2862 respectively. The MADV_WIPEONFORK compatibility fix therefore
removed the previously captured BoringSSL restart trigger in this attempt.

The next required service fails. `/cache/unica/logcat_live.txt` line 24286 says
`NetBpfLoad: Android 25Q4 requires kernel 5.10.`; lines 24287–24292 record
`bpfloader` exit status7, `reboot_on_failure`, and
`Got shutdown_command 'reboot,bpfloader-failed'`. The linkerconfig error from
the old attempts is also resolved in this boot. The installed A37
`com.android.tethering` APEX is version361153320 and reports API36/3610; the
loader identifies its kernel as 5.4.302 even though this device runs DS-ACK
4.9.337. This is an explicit compatibility gate, not evidence that changing
only a version string will make the required BPF maps/programs work. AOSP's
NetBpfLoad source documents that later Android releases gate required BPF
features by kernel generation and can fail if BPF programs cannot load
([upstream source](https://android.googlesource.com/platform/packages/modules/Connectivity/%2B/5f020a5ab853acf2a5a3ece263de716987c3f0d0/bpf/loader/NetBpfLoad.cpp)).

M2 rolled itself back into TWRP after about130 seconds. Probe13 and
`/cache/unica` were pulled before BOOT rollback. BOOT is restored to the
retained baseline SHA256 `cc618abc...776ed01d`; the guarded write reports no
protected partition changed. No userdata format or system/vendor/ODM/radio/EFS
write occurred. Full selected boot lines and raw local output are in
`logs/20261024-m2-runtime-fix/m2-feature-run/`.

Next technical gate is a source-level compatibility audit: identify the exact
25Q4 BPF objects/maps and kernel features required by this APEX, compare them
to the pinned4.9 DS-ACK capabilities/config, and assess whether a safe,
reviewable backport is bounded. Do not bypass `bpfloader`'s reboot-on-failure,
fake its detected kernel version, or claim the kernel backport is production
ready. Keep the successful WIPEONFORK patch separate from any BPF work.

## APEX comparison for the next candidate

The A37 firmware carries Tethering APEX version `361153320` (25Q4 / API36.1),
compressed file SHA256
`b2a8b33db76cc9895ffcbb96bfdc24a085c23e52cf2a3b9e26f953cf6343ff22`.
The retained M1 S22 firmware carries version `360743340` (2025Q2 / API36.0),
compressed file SHA256
`601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf`.
Both payload images were extracted read-only on the PC for this audit. Their
BPF object layouts differ: A37 ships `bpf/mainline/{clatd,dscpPolicy,netd,
offload,test}.o`; M1's older APEX has `bpf/{net_shared,netd_shared,tethering}`
objects. The A37 offload ELF contains sections tagged for `4_14`, `5_4` and
`5_10` variants. Presence of old-kernel variants alone does not negate the
25Q4 loader's explicit 5.10 policy.

There is useful runtime evidence for a carefully scoped compatibility test:
on M1 the older APEX logs `detected 5 of 5: 2025Q2 api:36.0.0` and
`bpfloader` exits0 on the same S9+ DS-ACK environment, while other older system
BPF loads still report `Operation not permitted` and are nonfatal. This proves
the old APEX gets past its own version gate and basic Tethering BPF loader
self-test; it does not prove all tethering/offload features work or that its
APIs remain compatible with A37 Android16 QPR2. The next candidate is therefore
a reversible M2 test using only the older Tethering APEX, followed by compiled
APEX/manifest/linker checks and runtime tests for Wi-Fi hotspot, IPv4/IPv6
forwarding, DHCP, DNS and data saver. Keep every other system/vendor/ODM input
fixed during that comparison. Do not call it accepted until those checks pass.


## Follow-up outcome — diagnostic Welcome boot

The retained S22 Tethering APEX did pass bpfloader on M2 (runtime logs version360743340 and detects 2025Q4/API36.1 from the surrounding A37 environment). This is not a claim that its package was rebuilt as25Q4. The next surfaceflinger linker failure was its missing S22 graphics common V6 interface dependency. Adding the exact matching64-bit interface exposed an AppOps RANGING failure, traced to aconfig flag storage under read-only /metadata. A volatile tmpfs hook before donor aconfigd then brought M2 to Welcome/boot_completed1 with Enforcing. Runtime crypto and linkerconfig gates pass on the diagnostic kernel; camera/audio ABI, vendor aconfig initialization, persistent metadata and hardware acceptance remain open. All temporary SYSTEM edits/diagnostic BOOT have been rolled back and read back; M2 is in TWRP. Production kernel selection is unchanged. Exact sequence, scope, rollback and hashes: docs/m2-metadata-runtime.md.
