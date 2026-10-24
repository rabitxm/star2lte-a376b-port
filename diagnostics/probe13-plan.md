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

## Scope of a future probe13

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
full BOOT readback and rollback. No probe13 image has been built or flashed.

Files affected by this plan: this document only. Probe11/12 sources and raw
results are retained for comparison. Implementation requires proving the
independent persistence channel, not merely adding more print statements.
