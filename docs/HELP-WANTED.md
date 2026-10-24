# Help wanted: A37 Android 16 runtime on Exynos 9810

Current status, 24 October 2026: M1 S22/Android16 boots on G965F. M2 A37
A376BXXS5AZH4 (Android16, SDK36.1, OneUI80500) bootloops after reaching
second-stage init. Final goal remains A37 OneUI9/Android17 on G965N/G965F;
Android17 has not been booted. The latest completed diagnostic is probe13.

## Confirmed current evidence

1. Probe13's mmap recorder was calibrated on booted M1 before M2. It measures
   monitor domain kernel and actual `KMSG_READ_ERROR=-13` /
   `VFS_WRITE_RC=-13` at policy activation. Mapped output persists through
   recovery with checked msync/fsync0. Correcting only the child-private kmsg
   inode label restores kmsg reads. M1 reaches completed1/enforcing.
2. M2 produced30 checksum-validated boot slots. All30 reach
   `init: init second stage started!`;23 capture vendor BoringSSL SIGABRT and
   its reboot-on-failure message,22 capture shutdown_command
   `reboot,boringssl-self-check-failed`. No kernel panic is needed to explain
   the observed restart trigger. Selected exact lines are in
   `support/evidence/probe13-m2-selected.txt` and counts/hashes in the summary.
3. The installed vendor self-test ELF interpreter is `/system/bin/linker64`,
   a symlink to `/apex/com.android.runtime/bin/linker64`. That actual A37
   runtime linker was extracted from installed SYSTEM read-only and tested
   against the identical vendor ELF on both recovery and booted M1 DS-ACK.
   Both abort before loading the program with
   `arc4random data MADV_WIPEONFORK failed: Invalid argument`, exit134.
   See `support/evidence/probe13-runtime-linker.json`.
4. Attempt50 already overlays the M1 **bootstrap** linker/libc/libm/libdl/
   libdl_android (patch0096). It does not replace the runtime APEX linker or
   runtime bionic. The runtime kernel-interface failure is independently
   reproduced; cryptographic self-test main has not been validated under a
   corrected runtime. Do not describe this as a demonstrated crypto failure.
5. Each retained M2 boot also reports `failed to execute linkerconfig: No such
   file or directory`. A37 init contains the APEX linkerconfig path, and that
   binary exists in the runtime APEX payload. Missing `/system/bin/linkerconfig`
   alone is not a demonstrated cause or sufficient proposed fix. Verify the
   early APEX mounts, interpreter resolution and linker namespace setup.
6. The actual A37 secilc/CIL compiles on both recovery and DS-ACK with exit0 and
   identical policy hash `4725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b`.
   Probe10 had a reproduced signal-decoding bug; probe11 fixed it and probe12
   reproduced the loop without ptrace. Their apparent policy-load stop was
   not evidence of a policy compiler hang. Probe13 now passes that boundary.
7. Both BOOT rollbacks match pinned baseline. M1 is booted; M2 is in TWRP.
   No kernel, vendor/radio, policy/enforcement change, self-test bypass, data
   format or ROM rebuild was made for probe13. EFS hash changes across
   Android/recovery cycles remain unverified; no EFS write/restore was done.

## Next correction needs review

Address runtime APEX/Bionic compatibility with the pinned4.9 kernel, preserving
real MADV_WIPEONFORK semantics and the BoringSSL self-test. Compare a scoped,
ABI-audited compatible runtime against a proper kernel backport before choosing
the implementation; the runtime EINVAL is now specific evidence relevant to
the project's kernel-change gate. No backport has been built/applied here.
Verify the APEX linkerconfig execution path separately. Do not remove
`reboot_on_failure`, bypass the crypto test, or treat madvise failure as success.

The recorder filters kmsg after512KiB per boot to preserve status space.
Consequently its trace does not retain every libc/linker/apexd stderr message.
Before another runtime test, extend selected errors and synchronously persist
fatal events if needed. A kernel panic or sudden reset is never guaranteed to
be captured. PID1 proc paths still encounter hidepid/MAC restrictions; the
retained proc FD preserves self access without granting extra process rights.

## Reproduction and constraints

Extend Eend15/Unofficial-Un1ca-9810 Android-16 at
`2b25ae6fc3098650be249bd1898e21e563cd5df1`. `tools/prepare_fork.sh` applies
88 active patches numbered through0098; omitted numbers are deliberate.
Patch0098 records five preexisting checkout files omitted from earlier replay.
The optional recorder lives in diagnostics/, outside normal build recipes.

Use Hybrid Vendor VNDK33 and variant-correct F/N hardware/RIL blobs, keeping
the fork's locked radio bridge where needed. The kernel remains original
 duhansysl DS-ACK V1.12 08/05/2026 enforcing/no-KernelSU. Do not write EFS/CPEFS,
modem or bootloader. G965N has not been flashed. M2 is not accepted for boot.

Start with README.md, docs/probe13-report.md, docs/repo-map.md and
 docs/patch-matrix.md. diagnostics/ explains source/tests/limits and
support/evidence/ contains selected text and hashes. Full firmware, images,
raw logs, credentials and physical identifiers are excluded from this source
snapshot. Acquire proprietary inputs and toolchains independently. Exported
device commands use role aliases; verify local targets before any operation.

8. The WIPEONFORK diagnostic backport passes real M1 semantics and makes both
   M2 BoringSSL self-test processes exit0. M2 then reaches `post-fs-data`,
   generates `/linkerconfig`, but reboots because the installed A37 Tethering
   APEX identifies as Android25Q4 and logs `Android 25Q4 requires kernel 5.10.`
   `bpfloader` exits7 with `reboot,bpfloader-failed`. The kernel is the pinned
   DS-ACK4.9.337. Help needed: trace the exact APEX build/source revision,
   required BPF maps/programs and kernel APIs; assess any bounded safe backport
   to4.9 versus a donor-compatible Tethering APEX. Do not only spoof uname or
   skip the reboot gate. Runtime evidence: `logs/20261024-m2-runtime-fix/`;
   summary: `docs/m2-runtime-kernel-fix.md`.

The retained M1 S22 Tethering APEX version360743340 (2025Q2/API36.0) gets past
its own BPF kernel-version gate on the S9+ DS-ACK environment and `bpfloader`
exits0; A37's version361153320 (25Q4/API36.1) requires kernel5.10. This makes
an isolated old-APEX-on-A37 test a plausible next experiment, but API and
hotspot functionality are unproven. APEX hashes and extracted BPF object layout
are in `docs/m2-runtime-kernel-fix.md`. Please assess module compatibility and
which connectivity tests must gate adoption.
