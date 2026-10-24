# Recorder visibility gate after probe12

Reason: corrected ptrace probe11 and passive probe12 both lose persisted
visibility around kernel SELinux initialization. Probe12 reproduces the
bootloop without PID1 ptrace. Neither log proves the exact fatal point or a
successful return from loading policy. Another identical ROM build adds no
evidence.

## Source facts and hypotheses

- Current monitors create a private mount namespace and open CACHE/kmsg before
  policy activation. They discard CACHE write/fsync results. Negative kmsg
  reads stop the current drain without recording the error.
- In the pinned DS-ACK source, `security/selinux/hooks.c`,
  `selinux_file_permission()` bypasses revalidation only when the opener SID,
  inode SID and AVC policy sequence still match. An already-open FD alone
  therefore does not demonstrate access after loading a new policy.
- Proc reads succeed at the first exec, then return ENOENT after SwitchRoot.
  Rootfs pathname cleanup or visibility is a hypothesis; it has not been
  isolated. An opened proc root FD must be tested across a simulated cleanup.
- The genfs ENOENT warning is not a captured panic. No kernel change is
  justified by that line alone.

An offline query of the actual compiled M2 policy now confirms a denial of
kernel -> unlabeled file append/write. The saved CACHE logs have an unlabeled
xattr when inspected in recovery. This supports a concrete recorder access
hypothesis but does not establish the runtime monitor domain/enforcement.
See `offline-recorder-policy.md`. M1 DS-ACK exposes no pmsg node; recovery
pmsg is not an established independent channel for an Android boot.

## Scope and implementation of probe13

Only the opt-in diagnostic recorder and its ramdisk may change. Preserve the
pinned kernel, DTB, cmdline, installed SYSTEM/VENDOR/ODM and data. Baseline BOOT
rollback remains mandatory. Do not switch global enforcement, edit policy,
write raw CACHE sectors, or change vendor/radio as a recorder workaround.

1. Retain an opened private proc root directory and use relative openat for
   self/attr/current and PID1 state. Record and test failures explicitly.
2. Check and distinguish kmsg read, CACHE write, fsync and pmsg write results.
   Include sequence/time counters. Record lost access via a separately verified
   channel; do not assume pmsg exists or survives recovery on DS-ACK.
3. Confirm source-domain and file-label transitions where readable. Do not
   silently claim a domain or permission from UID0 or a successful earlier FD.
4. Test directory-FD retention after pathname cleanup, syscall error reporting,
   and normal tracing/selftests without PID1 or device writes first.
5. Calibrate persistence across policy activation on the working M1 control,
   with a bounded BOOT test and baseline rollback. If the channel fails, fix
   instrumentation before another M2 loop.
6. Only after that calibration, run one M2 test and collect evidence. Separate
   last observed phase, recorder failure, PID1 failure, and kernel panic.

Verification required before deployment: static compile; actual AArch64 error
and namespace selftests; parent mountinfo unchanged; full image/CPIO audit;
serial/model/bootloader/hash/partition scope check; confirmed baseline image;
full BOOT readback and rollback. The implementation and selftests below now
exist; M1 calibration and M2 test/capture/rollback are complete. See
`../docs/probe13-report.md` for the actual result.

Files affected by this plan: this document only. Probe11/12 sources and raw
results are retained for comparison. Implementation requires proving the
independent persistence channel, not merely adding more print statements.


## Implementation — 24 October 2026

`bootlogger_pid1_probe13.c` retains a private proc directory FD and uses a
preallocated regular CACHE mmap file (32 slots of 1MiB plus one reserved page).
The parent still execs original init without tracing. Records carry length,
monotonic time and checksum; all file offsets stay inside this dedicated file.
The header records VFS write, kmsg read, msync and fsync results. No raw sector
write is used. Old probe11/12 logs are retained. After 512KiB per boot, kmsg is
filtered to init/SELinux/AVC/fatal/panic/reboot lines to reserve status space;
filtered and dropped counts are explicit.

If kmsg read returns EACCES/EPERM, the recorder logs it in the mmap channel and
makes at most one attempt to set the correct kmsg_device label on its own
private tmpfs kmsg inode. This does not edit policy, global enforcement or
system device nodes. The result and labels are logged; a default header value
of zero alone does not prove a relabel was attempted or successful.

Verification completed before BOOT: static AArch64 compile; warm/cold private
namespace tests on recovery and DS-ACK; proc-FD access after lazy detach;
mapped persistence with the ordinary FD closed and EBADF recording; host
checksum decode and deliberately corrupted-frame rejection; parent PID1
mountinfo hashes unchanged. An initial selftest caught the generic O_DIRECTORY
bit mismatch; it was corrected from the pinned arm64 UAPI definition before
image preparation. An interrupted USB repack produced a DTB mismatch and was
rejected without flashing. The subsequent image audit passes kernel/DTB/cmdline
and CPIO preservation. Current calibration candidate SHA256:
`62f81109e81147e47c9d2e9399e21ee8910e3ee7f7d71bcdee997240f9ab3ad1`.

Reason/files/verification: recorder visibility across policy load; standalone
C source plus read-only `tools/decode_probe13.py` and documentation only;
actual tests, BOOT audits, M1 calibration and M2 capture/rollback completed.
M2 reaches second-stage init and explicitly reboots on vendor BoringSSL
SIGABRT. Both BOOTs are restored; see selected evidence and the report.

Capture limit: after512KiB the kmsg selector retains init/SELinux/AVC/fatal/
panic/reboot text. Some lower-case libc/linker/apexd messages are filtered;
next instrumentation should include those error streams and immediate fatal
sync. Zero dropped frames does not mean zero filtered kmsg records.
