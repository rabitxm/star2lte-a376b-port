# Early init diagnostic sources

These are opt-in diagnostic sources, outside the ROM patch series. They include
`bootlogger.c` from the pinned, patched fork at
`rom/platform/exynos9810/installer/`. They do not change normal ROM builds.

`bootlogger_pid1_probe10.c` is retained as **known broken evidence**. Its monitor
decoded stopped wait status with `status & 0x7f`, which returns the stop marker
127 rather than WSTOPSIG. It then passed 127 to PTRACE_CONT, whose rejection was
ignored. The private monitor namespace also lacked procfs, so its proc snapshots
were empty. Do not deploy probe10 again or interpret its post-compile silence
as evidence that the policy compiler hung.

`bootlogger_pid1_probe11.c` is the boot-tested correction. Reason: fix signal
decoding, report resume errors and detach on failure, and mount read-only
procfs inside the recorder's private mount namespace. No normal fork source,
kernel, vendor, radio, or device partition is changed by keeping this file.

Compile using the installed host Clang/LLD, after preparing the fork:

```sh
clang --target=aarch64-linux-gnu -O2 -nostdlib -static -fuse-ld=lld \
  -fno-stack-protector -fno-builtin \
  -Irom/platform/exynos9810/installer -Wl,-e,_start \
  diagnostics/bootlogger_pid1_probe11.c -o /tmp/init-probe11
```

Verification on 24 October 2026: the actual AArch64 binary passed
`--signal-forwarding-selftest` and `--ptrace-selftest` on both M2 TWRP 4.9.118
and booted M1 DS-ACK 4.9.337. The first self-test reproduces errno 5 from the
old signal=127 decoder, then correctly forwards SIGCHLD and reaps a normal
exit. Temporary binaries were removed; M1 remained booted. See
`support/evidence/probe11-selftests.txt`.

Probe11 PID1 boot test completed on 24 October: eight boot sequences reached
compiler exit0 and kernel SELinux initialization, but M2 still bootlooped.
The BOOT image was rolled back and readback matched baseline. Proc snapshots
worked at the first exec, then returned ENOENT after SwitchRoot. No fatal or
second-stage marker was captured; post-enforcement recorder access remains
unverified. See `support/evidence/probe11-summary.json`.

The actual boot-tested build additionally logs SIGCHLD siginfo (compiler child
PID/status), PID1 status/wchan/children, and explicit proc-read errors. All
signal/attach/exec/warm/cold namespace tests passed on both recovery and DS-ACK;
parent PID1 mountinfo hashes remained unchanged. The image audit preserves
kernel/DTB/cmdline and all non-init CPIO entries.

`bootlogger_pid1_probe12_passive.c` is the completed passive comparison: the same private
CACHE recorder without ptrace operations against PID1. It retains harmless
non-PID1 selftests, but its actual PID1 and monitor paths do not attach or wait
on PID1. Reason: distinguish tracer interaction from the policy-load observation
boundary. Source audit, static compile, private namespace and exec-error tests
pass. Its BOOT test captured 14 first-stage starts, 11 policy-load markers,
and 10 kernel existing-superblock initialization markers. M2 still bootlooped;
no second-stage or explicit fatal/panic was captured. BOOT was restored to the
pinned baseline with full-partition hash verification. See
`support/evidence/probe12-summary.json`. No normal ROM build changes.

Neither recorder claims syscall-level reboot interception or guaranteed
capture through kernel panic or a loss of recorder permissions.

Next instrumentation gate: preserve an open proc root FD across SwitchRoot,
report kmsg read / CACHE write / fsync return values, and verify a durable
error channel through policy activation. The current recorder ignores CACHE
write and fsync failures and stops reading kmsg when read returns a negative
value. An opened FD is not proof of access after SELinux policy changes:
`selinux_file_permission()` in the pinned kernel revalidates access when the
policy sequence changes. This is a source-derived limitation, not a captured
AVC or confirmed explanation of the bootloop. See `probe13-plan.md`.
