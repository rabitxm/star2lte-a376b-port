# Help wanted: A37 Android 16 userspace on Exynos 9810

Current status, 24 October 2026: M1 S22/Android 16 boots on G965F; M2 A37
A376BXXS5AZH4 (Android 16, SDK 36.1, One UI 80500) bootloops. The final goal is
A37 One UI 9/Android 17 on S9+ G965N/G965F, but Android 17 has not been booted.

The build system is the existing Eend15/Unofficial-Un1ca-9810 Android-16 fork,
pinned to `2b25ae6fc3098650be249bd1898e21e563cd5df1`. Apply
`patches/fork/series` with `tools/prepare_fork.sh`; do not replace the build system.
The 88 active patches are numbered through 0098; omitted numbers are deliberate. Patch0098 records five existing local source files omitted from earlier replay; it changes no code in the tested checkout.

Hardware constraints: use the fork's Hybrid Vendor VNDK33, hardware blobs for
the correct F/N variant, and the fork's locked radio bridge when needed. Keep
the original duhansysl DS-ACK V1.12 08/05/2026 enforcing/no-KernelSU binary.
Do not modify EFS/CPEFS, modem or bootloader. G965N has not been flashed.

## Evidence available

1. The native A37 bootstrap linker aborts on the pinned DS-ACK kernel with
   `arc4random data MADV_WIPEONFORK failed: Invalid argument`, exit 134.
   This is a reproduced kernel-interface failure, not evidence of SIGILL.
2. Diagnostic attempt50 uses the hash-pinned M1 Android 16 bootstrap
   linker/libc/libm/libdl/libdl_android overlay (patch0096). Kernel, framework,
   Hybrid Vendor and radio are retained. Attempt50 still bootloops.
3. The recorder captured first-stage init and successful SYSTEM/ODM/VENDOR
   mounts, then `Opening SELinux policy`, `Compiling SELinux policy`, and
   `Missing /system/etc/selinux/plat_sepolicy_genfs_202404.cil; skipping`.
   Filtered original line numbers are in `support/evidence/probe10-init.txt`.
4. On 24 October, the actual A37 secilc and installed policy CIL inputs were
   executed using attempt50's bootstrap stack in a read-only TWRP chroot.
   Exit 0; policy output size 1,528,889 bytes. The same compiler and CIL were
   run on the booted M1 control using its pinned DS-ACK kernel and native
   bootstrap stack: exit 0, identical policy SHA256
   `4725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b`.
   No compiled policy was loaded into either running kernel.
5. Probe10 contains an instrumentation bug: stopped wait status was decoded
   with `status & 0x7f`, producing 127 instead of WSTOPSIG. PTRACE_CONT with
   127 fails with errno 5; the result was ignored. A reproduced SIGCHLD
   self-test demonstrates the failure and correct resumption. Therefore the
   probe10 post-compile silence is not reliable evidence of a compiler hang.
   Its private namespace also had no procfs, explaining empty proc snapshots.
6. Probe11 correction source fixes decoding, logs/detaches on resume failure,
   and adds private read-only procfs. Actual AArch64 signal-forwarding and
   attach self-tests pass on both recovery 4.9.118 and M1 DS-ACK 4.9.337.
   **No probe11 BOOT has been built or tested.** It uses PTRACE_CONT and does
   not implement syscall-level reboot trapping.

## Useful next investigation

Run a corrected recorder to obtain PID1 signal stops, proc snapshots, compiler
child completion and the actual next init phase. Distinguish policy compilation
from policy loading into the kernel and the later second-stage handoff. Do not
attribute the loop to the optional missing genfs CIL without a failure log.
A future kernel backport candidate is retained as source under `patches/kernel/`,
but it has not been applied or built and is not a validated remedy for M2.

## Reproduction and sharing

Start with README.md, docs/repo-map.md, docs/patch-matrix.md and docs/m2-build.md.
`support/evidence/` contains selected text and hashes, not raw device dumps.
`diagnostics/README.md` explains the recorder tests and limits. Historical docs
are chronological; this file and current AGENTS.md decisions take precedence
when older notes describe a superseded plan or authorization.

This handoff contains patches and source only. Acquire proprietary firmware,
LFS payloads, toolchains and the pinned DS-ACK package independently. Physical
device identifiers are replaced with role aliases in the exported snapshot;
verify local targets before using any device command. Raw logs, firmware,
keys, images and EFS data remain local.
