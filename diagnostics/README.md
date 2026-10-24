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

`bootlogger_pid1_probe11.c` is a correction candidate. Reason: fix signal
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

Full PID1 boot behavior, private proc snapshots during first-stage init, and
rollback for a probe11 BOOT image **have not been tested**. No probe11 BOOT
image has been built or flashed. This recorder uses PTRACE_CONT; it does not
claim to intercept reboot syscalls or guarantee every fatal path is captured.
