# M4 optional GPU meminfo runtime diagnostic — 28 October 2026

Source0197's real network stats/transport/Parcel/v7-v8 fixture passes exit0,
then SystemServer aborts in ActivityManager after94s. Fresh tombstone_00 under
logs/20261028-m4-stats-test/result/tombstones records:

```
signal 6 (SIGABRT), code -1 (SI_QUEUE)
#01 pc 00000000000199e4 /system/lib64/libmeminfo.so (BpfMapRO<unsigned long,unsigned long>::abortOnMismatch(bool) const+464)
#02 pc 0000000000015c54 /system/lib64/libmeminfo.so (ReadProcessGpuUsageKb(unsigned int,unsigned int,unsigned long*)+140)
```

The caller is actual Debug.getGpuTotalUsageKb JNI → MemInfoReader → AppProfiler.
Disassembly ties0x199e4 specifically to the negative-fd branch at0x19830,
not the later valid-map key/value-size checks. BPF_OBJ_GET fails before the
reader's existing error/false path is reached. Its exact SystemServer errno
is not captured; no kernel feature/map-size mismatch is claimed from the
function name alone. The actual pin string is
`/sys/fs/bpf/map_gpuMem_gpu_mem_total_map`; both GPU readers use that path.

S22's libmeminfo also aborts on a negative fd in its corresponding verifier.
Therefore copying the complete S22 library is neither a proved remedy nor
needed for this bounded experiment. No new kernel is built.

## Source0198 scope

Two new text files inside the existing exynos9810_boot_restore module:
a376b_android17_meminfo.py and its fixed lock. This is an explicit diagnostic
helper; normal profiles and complete retained ROMs are not changed.

One4-byte instruction at VA0x19830 changes TBNZ w8,bit31's target from0x199e4
(abort) to0x199cc (the existing ABI/PAC-preserving epilogue). Only2 file bytes
actually differ. No instruction is NOPed and no success/zero reading is
injected. Both existing callers reload/test the fd after the void verifier
returns, then use their existing logging/false/errno paths. All code for
valid opened maps, map flags/key/value validation, map data reads, aborts on
actual validation failure, and GPU results remain byte-identical.

| File | SHA256 |
| --- | --- |
| Original BZI6 libmeminfo (186424 bytes) | db2ec28e3f2609c4b5ae3442b4f60cc2be50a66ab76921aeb1f35d6347df6ec1 |
| Optional-map diagnostic | b19a1c37a8ebed72676739a12ab46be31dd3c74d4ba1c605f655a62661d70fe9 |
| S22 comparison only | 8a780b05a00308cd7d76ddb94d29d228f8e594f53031e425375f1f583e723453 |

Before emitting output, verify the exact source/output hashes, ELF64/AArch64,
executable VA mapping, decoded branch sign bit and target, both actual direct
BL callers, prologue/epilogue and error-path contexts. Negative source,
branch target, caller set and output hash cases reject. Final disassembly
confirms the new destination. All188 patches replay to132 matching files;
all187 previous patch hashes are unchanged. PC evidence:
logs/20261028-m4-meminfo. No binary/firmware is included in the patch.

## Actual-device procedure and rollback

The previous197 trial was fully rolled back before this next trial. Read-only
ro,noload inspection of the installed libmeminfo verifies the exact donor
hash,186424 bytes, UID/GID0:0,mode644, system_lib_file and original mtime.
A first local audit helper unmounted before its second read; its empty
capture was rejected. The corrected single-command mount/read/unmount passes;
no device file was written by either audit. A first fixture compiler command
selected a tiny apktool launcher JAR and reports ClassNotFoundException; the
actual bundled22MB CLI assembles DEX039 successfully. Both failed PC logs
are retained and are not Android runtime failures.

The new designated G965F trial is logs/20261028-m4-meminfo-test. Keep the same
0197 network APEX, resolver, V6/libui, vendor mount/ueventd/logger and existing
WIPEONFORK/probe13 BOOT. The only new runtime delta is this libmeminfo branch.
Snapshot the same two exact netstats directories; no data format. Reverify
identity/hash/partition scope on every write. No EFS/CPEFS/radio/bootloader
write or G965N operation is authorized by this experiment.

Source fixtures under support/diagnostics/m4-meminfo invoke the real Debug
JNI eight times and MemInfoReader's kernel-used path as a child. The separate
freestanding ARM64 probe only opens the actual BPF pin (OBJ_GET with the
library's flags8, plus flags0 comparison), prints raw syscall results, and
closes any returned fd; it creates/loads/attaches no BPF object. Its UID and
SELinux domain must be recorded; shell-domain errno is not SystemServer errno.
An unsupported reading (-1) is reported as such, never as successful zero.
Actual SystemServer survival and boot_completed require independent logs;
a child fixture alone cannot certify valid-map statistics or full boot.

On failure capture CACHE/ANR/pstore/mmap/tombstones and fixture results first,
then restore exact original libmeminfo and all earlier trial files, full
BOOT/VENDOR and narrow netstats snapshots. SYSTEM rollback is file-level,
not a claim of identical raw ext4 bytes. Actual runtime outcome is recorded below.


## Actual0198 result

On the designated G965F, watcher first sees sys.boot_completed=1 at128.3s;
bootanim=stopped and Enforcing. The watcher records27 boot-complete samples;
PID1830 is present in each and still present at late uptime337.22s. Transient
additional pidof matches are recorded without inferring they are new servers.
The real Debug JNI/MemInfoReader fixture passes exit0 and eight GPU calls
return-1 (unsupported), not synthetic0. The raw BPF pin probe running uid0
SELinux su reports-2 ENOENT for flags8 and0; ls reports the path absent in
that same domain. SystemServer main log reports `Can't open file: /sys/fs/bpf/
map_gpuMem_gpu_mem_total_map` and survives. This measures the independent
probe errno; it is not a claim that SystemServer errno itself was captured.

Runtime source/hash and SDK37/OneUI90000 were read on Android. Read-only
Settings report adb_enabled1, rampart_suw_main_on0, main_switch0, strict0,
ro.adb.secure0, ro.debuggable1. This is an existing-data read, not a fresh
seed or GUI-toggle/new-PC RSA test. No fresh format or kernel build occurred.

SystemUI's FaceWidget lifecycle fails off the main thread; SDHMS rejects
OverheatComplexType.DEX; NFC cannot find libnfc-nci.so. Bixby has one receiver
failure. Therefore full M4/UI/hardware acceptance remains open. Exact errors,
actual source paths and next verification sequence:
[m4-post-boot-blockers.md](m4-post-boot-blockers.md).

All Android logs were captured before guarded recovery reboot. The recovery
capture contains189 files, including15 CACHE logger files, mmap generations
through48,55 ANR files,64 tombstone files and independent fixture outputs.
Old mmap slots/tombstones are retained evidence, not all fresh0198 failures.
Capture pull summaries pass. Final rollback is complete and verified:

- Original libmeminfo db2ec28e and exact original SYSTEM APEX pair/libui
  restored; additiveV6 absent. File metadata/core-file scope checks pass.
- Full BOOT cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d
  and full VENDOR025f7314775af68d0449143328923215fe14af25c076881ef2a7cb0d9ef3c9f3
  read back exactly.
- Both netstats snapshots restore byte/UID/GID/mode/SELinux/mtime exactly.
- Every per-write protected-partition comparison passes; no direct EFS,
  CPEFS,radio,bootloader,RECOVERY/BOTA operation is performed. Across-boot
  EFS/ODM hash changes are not mislabeled as writes by these file operations.
- operation-status.json reports rollback_pending=false. Device is verified
  in TWRP3.7.0_9-0 on the same G965F/bootloader. No operation remains running.

This leaves the retained complete attempt37 ROM unchanged. The measured
195–198 diagnostic composition is not yet a new packaged release; normal
M4 runtime still needs the reviewed networking/meminfo integration plus the
post-boot fixes and fresh-data/hardware acceptance. No kernel build, new
production kernel selection, G965N test or complete M4 acceptance is claimed.
