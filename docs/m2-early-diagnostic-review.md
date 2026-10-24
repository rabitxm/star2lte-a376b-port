# M2 G965F early diagnostic review — 22/10/2026

Status: TEST01 COMPLETE; bootloop persisted, no M2 fatal/panic recovered. Logs saved; baseline BOOT restored with full partition SHA verification. Phone remains TWRP. The prior retained full ROM is unchanged.

Target: SM-G965F, serial `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`. G965N excluded. Existing M2 system/vendor/data remain in place; no repartition or format is part of this proposal.

Artifact directory: `logs/20261022-m2-early-diag/cmdline-diag-01/`.

| File | SHA256 |
| --- | --- |
| diagnostic-boot.img | a4d9fe0783459335d506c6ea61fcf6b37a4d4f95cb72982fbee222bbe187e3c9 |
| rollback-boot.img | cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d |

Both files are full 57,671,680-byte BOOT partition images. The only changed bytes are in the legacy header command-line field (offset64, length512); kernel, ramdisk, DTB, payload offsets, image ID and partition tail are byte-identical. No SELinux mode change or ADB change.

Command line before: `androidboot.selinux=enforcing`.

Added parameters: `loglevel=7 ignore_loglevel androidboot.init_fatal_reboot_target=recovery androidboot.init_fatal_panic=true`.

Purpose: increase log output and, where supported by init, turn its fatal path into panic/pstore evidence or recovery redirection. This is diagnostic behavior: if a fatal error happens, the phone may panic, restart, enter upload/download or require hard keys to recovery. These controls do not fix a boot error. No memory address or ramoops reserved-memory layout is guessed or modified.

Evidence: extracted IKCONFIG from the pinned DS-ACK kernel enables PSTORE/PSTORE_RAM/PSTORE_CONSOLE and MAGIC_SYSRQ. The old Android29 ramdisk init contains `androidboot.init_fatal_reboot_target=`; A37 init additionally contains `androidboot.init_fatal_panic` and `/proc/sysrq-trigger`. Local AOSP reference `rom/external/android-tools/vendor/core/init/reboot_utils.cpp` shows the intended handling, but is not claimed as the exact Samsung binary source. Real acceptance of parameters, fatal classification, panic behavior and persistent log survival require the device test. Old first-stage init may ignore fatal-panic. Failure before init or a nonfatal reset may still leave no useful log.

The previous logger script that replaces `/init` is not used. The safe logger module expects a root module/service path, absent from this no-KernelSU profile, and starts too late for the current failure; it is not installed.

Verification: offline helper exact base/header check; per-component SHA proof in manifest; malformed-input rejection without output; existing-output collision rejected without mutation; Python compilation PASS. Optional utility only; not wired into normal ROM builds. Patch0086 carries reason/files/checks. No kernel build.

Proposed authorized test: recheck serial/model/bootloader and current BOOT hash in TWRP; transfer diagnostic image to RAM and verify hash; write only the BOOT partition; read back the full partition hash; boot once with host ADB observation. On failure, return to TWRP and capture `/sys/fs/pstore`, `/proc/last_kmsg`, `/proc/sec_log`, reset reason and recovery log before further reboots. Keep raw logs private.

Rollback: in verified G965F TWRP, transfer `rollback-boot.img` to RAM, verify its SHA, write only BOOT and read back expected `cc618abc...776ed01d`. This restores the exact pre-diagnostic BOOT; M2's underlying bootloop may remain. Do not use the G965N serial, and do not wipe data. A diagnostic installation must include authorization to restore this known BOOT after capture.
