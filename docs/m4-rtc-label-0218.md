# M4 RTC sysfs labels — patch0218 / build0219

Updated 29/10/2026. This is one isolated fix after KG30 passed its first runtime
gate in M4-0218. It changes only the three exact RTC labels; it does not change
brightness scaling, thermal thresholds, kernel, vendor blobs, radio or the
protected partitions.

## Evidence on M4-0218

The fresh G965F M4-0218 logcat records:

```text
180708: 10-29 13:31:27.514 ... E SDHMS:i : e = /sys/class/rtc/rtc0/date: fstat failed: EACCES (Permission denied)
180710: 10-29 13:31:27.514 ... E SDHMS:i : e = /sys/class/rtc/rtc0/time: fstat failed: EACCES (Permission denied)
180711: 10-29 13:31:27.514 ... E SDHMS:i : e = /sys/class/rtc/rtc0/since_epoch: fstat failed: EACCES (Permission denied)
```

Each class symlink resolves beneath
`/sys/devices/platform/141c0000.speedy/i2c-17/17-0000/s2mps18-rtc/rtc/rtc0/`.
The three read-only nodes had label `u:object_r:sysfs:s0` and mode0444. The
loaded vendor CIL has existing `system_app_33_0` directory and file permissions
for `sysfs_rtc_33_0`, including `search`, `read`, `getattr`, and `open`, while
the actual RTC type is the existing `sysfs_rtc` type. No exact genfscon exists
for these three paths in the M4-0218 policy.

An in-place runtime label trial was attempted under Enforcing. The system root
shell returned:

```text
chcon: '.../rtc0/date' to u:object_r:sysfs_rtc_33_0:s0: Permission denied
```

The trial's exit trap restored the original `sysfs` context to all three nodes;
readback confirmed that. This means runtime `chcon` cannot test the new label on
this sysfs filesystem. The candidate therefore stages the exact genfscon rules
in the existing final vendor restore hook, then verifies the compiled policy,
packed vendor image and runtime labels on device. The rule target type is
`sysfs_rtc`, not the versioned attribute name `sysfs_rtc_33_0`.

## Change and verification

Patch0218 extends the existing
`a376b_android17_thermal.py` helper and lock with these exact paths:

- `.../rtc/rtc0/date`
- `.../rtc/rtc0/time`
- `.../rtc/rtc0/since_epoch`

Only three `genfscon` lines are added to `vendor_sepolicy.cil`; there are no new
`allow` rules. The previous exact backlight labels stay intact. The helper
checks all five nodes for missing, duplicate and conflicting rules, validates
the pre-existing display/RTC permissions, and rejects missing permissions.

Host checks completed before packaging:

- Python syntax and JSON parsing pass.
- Helper self-test passes all five node duplicate/conflict fixtures, all four
  missing-permission fixtures, idempotence and exact-scope checks.
- Applying the helper to a temporary copy of the real M4-0218 vendor CIL adds
  exactly three RTC `genfscon` lines and removes nothing; it adds no `allow`.
- `verify_exynos9810_selinux.py` compiles the complete split policy with
  `secilc` after staging the rules.
- Patch0218 reverse/reapply replay reproduces the helper and lock byte-for-byte.
- Full ordered replay from the pinned fork commit passes for208 patches and144
  touched paths; replayed files are byte-identical to the current fork checkout.

## Build0219 and packed-image gate — PASS

Run: `rom/out/releases/M4/star2lte-1793258408-cX3l8d/`.

- Build exit is `0`; `.complete` exists. The existing fork reports
  `Verified Exynos9810 flashable ... (ext4, enforcing, six device payloads)`.
- Full signed ZIP SHA256:
  `13c54978789c8921618731988b8176936f823880da2ea852de6abbdc9cc87e5d`.
- Target-files ZIP SHA256:
  `fb8b7e7a20580726c15dd701cb71d27fef83ecde6e2211b3db279062bd61a4a7`.
- All eight signed ZIP32 stages pass CRC, signature, range and size checks.
  Their hashes are in `SHA256SUMS.legacy`; its manifest SHA256 is
  `9206ccf255e35a60762ba38050486da85ef392c0df0b189c8a9d04818ac3d608`.
- The final target-files VENDOR image was converted from sparse ext4 and read
  with `debugfs`. Each of the three exact RTC rules occurs once in the packed
  CIL; no `allow` was added. Audit:
  `logs/20261029-m4-rtc-label-0219/rtc-final-vendor-image-audit.txt`.
- Compiled policy, ext4/e2fsck, metadata, ADB and release gates pass. The final
  VENDOR sparse image SHA256 is
  `9b338df3d38fb1859165d0200375062604e8ee1418d74c39c8bf9f7feaa002ac`.

## Fresh install and RTC runtime gate — PASS

The package was fresh-installed on G965F serial `M2_G965F_SERIAL`, bootloader
`G965FXXUHFVG6`, with a fresh USERDATA format. All eight installer stages
returned `RC=0`; post-install hashes for EFS/CPEFS/RADIO/RECOVERY/BOTA0-2 match
their pre-install hashes. The partition table was already the exact required
size, so no repartition was performed. Full evidence is under
`logs/20261029-m4-rtc-label-0219/attempt0219/`.

Both first and second cold boots reached `sys.boot_completed=1` under SELinux
Enforcing. On each boot all three real S2MPS18 nodes carried
`u:object_r:sysfs_rtc:s0`, were readable, and `since_epoch` advanced by two
seconds. SDHMS RTC `fstat failed: EACCES` count is zero in both stability
windows; SystemServer PID stayed stable in the recorded first window
15:22:47–15:24:20 and second window 15:26:13–15:27:45 on 29/10/2026 (+0700).
The second-window result records zero ANR/watchdog matches and zero native
`Fatal signal` matches; this is an RTC stability result, not clean-full-boot or
whole-M4 acceptance. Exact captures:
`rtc-runtime-first-boot.txt`, `rtc-runtime-second-boot.txt`,
`firstboot-stability.txt`, `secondboot-stability.txt`, and the two logcat files.

The logcat snapshots also retain earlier buffered startup events. AOD has a
Java `FATAL EXCEPTION: PluginFaceWidget-BG` caused by
`AbstractMethodError ... disconnectPluginOnBackground()` on both boots:
first at 15:21:35.127 (`logcat-first-boot.txt:155762–155777`), second at
15:25:34.216 (`logcat-secondboot-90s.txt:116092–116107`). The second boot also
has a Bixby receiver Java fatal at 15:25:51.417, with
`IllegalStateException: No matching ComponentResult found!`
(`logcat-secondboot-90s.txt:163163–163179`). These faults precede their later
stability windows and remain separate open issues; their presence in the
buffer must not be counted as new events inside those windows or hidden by
the native-fatal counter.

The first-boot log has a separate GNSS HAL crash at
`attempt0219/logcat-first-boot.txt:75181`: `Fatal signal 11 (SIGSEGV)` in
`vendor.samsung.hardware.gnss@2.0-service`, with a null dereference at
`fault addr 0x4`. The second-boot capture has no native GNSS `Fatal signal`
and the GNSS service is running. This does not change the RTC result; GPS
satellite acquisition and fix capability still need their own test.

## Scope and next unit

RTC label repair is complete. No brightness scale, thermal threshold, kernel,
vendor blob, radio, EFS/CPEFS or other protected partition was changed. The
user confirmed brightness changes normally; patch0216 remains untouched.
M4 as a whole is still not accepted. The separate thermal audit is next; passing
RTC access does not prove the limiter or panel thermal cap works. If a future
RTC regression appears, remove the three exact `genfscon` entries from patch0218
and build a separately versioned candidate; preserve current M4-0219 until a
replacement passes package and runtime gates.

### Report clarification — 29/10/2026

Reason: distinguish the exact RTC stability windows and native-fatal pattern
counts from buffered Java startup crashes and full-runtime acceptance. File
affected: `docs/m4-rtc-label-0218.md` only. Verification: compared
`firstboot-stability.txt` and `secondboot-stability.txt` start/end timestamps,
SystemServer PID and `Fatal_signal_count` with the exact AOD/Bixby/GNSS events
in the cited `attempt0219` logcat files. RTC PASS is unchanged; no ROM source
or device state changed for this clarification.
