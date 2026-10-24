# Probe13 completed — second-stage init and explicit reboot cause captured

24 October 2026. M2 remains a failed boot test. The diagnostic step is complete:
recorder calibration, M2 boot/capture, full BOOT rollback, and a direct runtime
linker check. Kernel, DTB, cmdline, SYSTEM/VENDOR/ODM, radio and enforcement
were not changed. No data format or ROM rebuild was performed.

## Result

| Check | M1 control | M2 attempt50 |
| --- | --- | --- |
| Second-stage init | Captured; boot_completed1/enforcing | Captured in all30 retained boots |
| Ordinary recorder I/O | kmsg read and CACHE write return EACCES | Same failures captured |
| mmap channel | Persists after recovery; msync/fsync0 | All30 slots validate; msync/fsync0 |
| Restart trigger | Normal Android boot | 23 BoringSSL SIGABRT/reboot-on-failure records;22 shutdown commands |
| BOOT rollback | Baseline full hash verified; booted again | Baseline full hash verified; left in TWRP |

The M2 first retained boot contains:

```text
init: init second stage started!
init: failed to execute linkerconfig: No such file or directory
init: Service 'boringssl_self_test64_vendor' (pid 941) received SIGABRT waiting took 0.003000 seconds
init: Service boringssl_self_test64_vendor has 'reboot_on_failure' option and failed, shutting down system.
init: Got shutdown_command 'reboot,boringssl-self-check-failed' Calling HandlePowerctlMessage()
```

Original decoded line numbers are in
`../support/evidence/probe13-m2-selected.txt`. The self-test service is defined
by installed `vendor/etc/init/boringssl_self_test.rc`, including
`reboot_on_failure reboot,boringssl-self-check-failed`. This establishes an
explicit userspace restart trigger, not a demonstrated kernel panic at policy
loading. The other boots do not all retain the final shutdown tail.

## Recorder correction and calibration

The passive wrapper retains a private proc directory FD, preallocates a regular
CACHE mmap file and records length/time/checksum frames. msync/fsync and normal
VFS write/kmsg read results are checked. At an actual kmsg EACCES, it corrects
the label of only its private tmpfs kmsg inode, preserving system nodes, policy
and enforcement. It does not trace PID1 or write raw CACHE sectors.

M1 establishes the prior blind spot at runtime: monitor domain kernel;
`KMSG_READ_ERROR=-13`, successful private kmsg label correction and
`VFS_WRITE_RC=-13`, while mmap output continues through second-stage and boot.
Recovery readback preserves the output with heartbeat68410ms and no dropped
frames. Selftests passed on recovery and DS-ACK for proc FD retention after
lazy detach, warm/cold private namespaces, and mmap persistence after ordinary
FD close with EBADF reporting. The decoder rejects a corrupted actual frame.
Parent PID1 mountinfo hashes are unchanged by the selftests.

Retaining proc preserves self access but does not grant access to PID1 under
hidepid/MAC checks. Booted M1 reports proc gid3009/hidepid2; the M2 policy denies
kernel-to-init ptrace offline. PID1 paths still return ENOENT in the recorder.

## Runtime APEX check

The actual installed vendor self-test ELF requests `/system/bin/linker64`,
which points to `/apex/com.android.runtime/bin/linker64`. Extracting installed
SYSTEM's `com.android.runtime.apex` read-only and running that exact linker
against the identical vendor ELF reproduces:

```text
arc4random data MADV_WIPEONFORK failed: Invalid argument
RUNTIME_LINKER_RC=134
```

This reproduces both on M2 recovery4.9.118 and, crucially, on booted M1's pinned
4.9.337-DS-ACK-V1.12-G965F-08.05.2026 kernel. Temporary test files are removed.
All SYSTEM/VENDOR/ODM/APEX mounts were read-only and cleaned afterward. Exact
hashes/output are in `../support/evidence/probe13-runtime-linker.json`.

The existing patch0096 replaces only bootstrap linker/bionic. The runtime APEX
is still A37 and its loader independently aborts before self-test main. This is
a concrete compatibility failure relevant to the observed service SIGABRT;
the cryptographic self-test itself has not been validated with a corrected
runtime. Do not call this a proven algorithm failure or bypass the test.

The linkerconfig error also needs investigation. A37 init contains
`/apex/com.android.runtime/bin/linkerconfig`; that binary exists in the runtime
APEX and requests `/system/bin/linker64`. Therefore absence of a system-bin
linkerconfig symlink alone is not a demonstrated cause. Verify live early APEX
mounts and the complete exec/interpreter path before proposing a path patch.

## Retention and limits

Diagnostic BOOT SHA256:
`62f81109e81147e47c9d2e9399e21ee8910e3ee7f7d71bcdee997240f9ab3ad1`.
Both rollback BOOT readbacks:
`cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`.
Image audits preserve kernel/DTB/cmdline and all non-init CPIO metadata/data.
An earlier interrupted USB repack failed the DTB audit and was never flashed.

M2 raw mmap size33,558,528 bytes, SHA256
`41394e2d964617c55ab00dc54b01489ffecd72c1e95e19f3f087c4350f88dff9`.
All30 slots pass frame checksums, final msync/fsync0, zero dropped frames.
However,8249 kmsg records were intentionally filtered after the per-boot512KiB
threshold; zero drops is not a claim of complete console retention. Future
selection should include libc/linker/apexd errors and immediate fatal syncing.
No recorder guarantees capture through an abrupt panic or reset.

Measured protected hashes are unchanged during BOOT writes/rollbacks. EFS raw
hash changes across Android/recovery remain unverified; there was no EFS write
or restore. Other measured protected hashes match. G965N is untouched. M1 is
booted at baseline; M2 is in TWRP at baseline. Raw logs/images stay local in
`logs/20261024-probe13/`; only source and selected evidence are shared.

## Next implementation decision

Review runtime APEX/Bionic compatibility versus a correct MADV_WIPEONFORK
kernel backport under the project's evidence gate. Preserve actual wipe-on-fork
semantics and all crypto checks. Investigate the APEX linkerconfig execution
path separately. No kernel backport, runtime replacement, self-test disable,
or linkerconfig path patch has been applied by this diagnostic step.

Reason/files/verification: diagnose the post-policy recorder blind spot and
restart cause; standalone recorder C, read-only decoder, selected evidence and
documentation; real AArch64 selftests, audited BOOT payloads, M1 calibration,
M2 capture, actual runtime linker comparison and full rollback completed.
