# M4 attempt0223 — fresh install and first-boot check, 30/10/2026

**Status: package/install PASS; Android boot PASS; M4 remains unaccepted.** The test was performed on the G965F test unit in TWRP. The package completed all eight legacy stages, formatted USERDATA as configured, and Android reached `sys.boot_completed=1`. The Setup Wizard language page is the foreground activity in ADB; no physical screen observation or wizard interaction was done.

## Package and install

Full signed ZIP SHA-256: `a35d05fc9707930a1f747a10008ede86b62ec88273e35bca5319126ef865e4fa`. Target-files SHA-256: `cad170afbf0269567be12e989549b38f6f48db37e0cc539f000df497924af78e`. Legacy manifest SHA-256: `a71183ab9b1adc432cea6124fbcf3cfa906508367e3fe0d3bed1e5fb383f6152`. All eight stage ZIP32/CRC/signature and package checks passed before installation; readback sums passed for the full ZIP, eight stages and manifest.

The first stage-2 sideload trial ended with `failed to read from adb host: Connection reset by peer` at `logs/20261030-m4-g965f-flash-0223/stage02-sideload.txt:46`; TWRP then logged `Updater process ended with signal: 7` at line 50. SYSTEM had been partially updated, so installation stopped. SYSTEM was restored from the prewrite image and read back as 9,961,472,000 bytes with the exact prewrite SHA-256 `b72b6be2272a7180c5ab666c3d1fde9fa82cabd3b148ece9082f4efa0c034256` (`flash-01/restore-system.log`). No other partition was restored in that operation.

The retry used hash-verified 128 MiB ADB-shell chunks, then verified each reconstructed stage ZIP before TWRP installed it. Stage receipts and recovery logs show 8/8 ordered stages with `Updater process ended with RC=0`; stage 8 formatted data (`flash-01/stage08-recovery.log`). Layout preflight recorded `repartition_expected=false`; the final partition sizes match the audited target. USERDATA UUID changed across the fresh format. The installer did not write EFS, CPEFS, RADIO, RECOVERY or BOTA partitions.

The initial protected-hash comparison found EFS changed from the first prewrite hash (`2973fbdc…`) to `3a45e6a1…`; CPEFS, RADIO, RECOVERY and BOTA0–2 still matched. This raw EFS hash drift is unexplained and is not treated as proof of corruption. No EFS write or restore was attempted. The old and continuation hashes are preserved in private run logs at `flash-01/prewrite/protected-hashes.txt`, `flash-01/protected-hash-audit-after-system-restore.json`, and `flash-01/protected-hash-continuation.json`. The remaining protected partitions, including EFS, matched the explicit continuation baseline at the pre-reboot check.

## First boot and short runtime sample

Live properties after fresh boot: Android 17, SDK 37, One UI 90000, SELinux Enforcing, `sys.boot_completed=1`. During the 90-second stability window, `system_server` stayed PID 2205, SystemUI stayed PID 10090, and SurfaceFlinger stayed PID 1881. No device reboot was observed in that window.

Target identity needs review: the connected handset is confirmed by the preflash TWRP capture as G965F, and live properties show `ro.bootloader=G965FXXUHFVG6`, `ro.boot.hardware=samsungexynos9810`, and `ro.product.device=star2lte`; however, `ro.product.model=SM-A376B` and `ro.product.name=a37xnaxx`. This may be inherited donor branding, but the profile/build-property path must be audited before deciding whether it is intentional. Captured values: `flash-01/runtime/target-device-identity.txt`.

The fresh install confirms the requested initial ADB and Auto Blocker defaults: `ro.adb.secure=0`, `ro.debuggable=1`, `adb_enabled=1`, no `/data/misc/adb/adb_keys` file, `rampart_suw_main_on=0`, `rampart_main_switch_enabled=0`, `rampart_strict_protection_switch_enabled=0`, and both Rampart ADB/USB blocking values are 0. This checks the first-boot values on the connected PC; a separate new-PC authorization test and interactive toggle test remain open.

Three startup issues remain:

- Bluetooth restarted repeatedly; the initial sample says `Bluetooth crashed 5 times` at `flash-01/runtime/stability-window/000s-shell.txt:31`, and the 90-second sample continues to show `Disable CRASH` / `Enable RESTARTED` at `flash-01/runtime/stability-window/090s-shell.txt:19-33`. The current-boot log records `Timed out waiting for Unknown OpCode(0xfdf1)` at `flash-01/runtime/logcat-all.txt:114250`. Bluetooth was not paired or exercised with an accessory.
- SystemUI FaceWidget had an `AbstractMethodError` for `PluginKeyguardStatusView.disconnectPluginOnBackground()` at `flash-01/runtime/stability-window/090s-crash-buffer.txt:17`. Its crash is at line 2. SystemUI stayed up during the subsequent 90-second sample.
- Bixby crashed with `IllegalStateException: No matching ComponentResult found!` at `flash-01/runtime/stability-window/090s-crash-buffer.txt:33`; the fatal exception begins at line 19.

A read-only NFC service dump returned `Error with service 'nfc' while dumping: FAILED_TRANSACTION` at `flash-01/runtime/final-live-state.txt:30`; no RF, tag, HCE, SIM or payment test was performed. Wi-Fi/app networking, camera, GNSS, cellular/RIL, calls/audio and physical display behavior were not tested. The M4 package is therefore bootable on this G965F but is not accepted as a working port.

## Artifacts and next work

The private device/run evidence is under `logs/20261030-m4-g965f-flash-0223/flash-01/`; the first failed transport and split/hash evidence are alongside it under `logs/20261030-m4-g965f-flash-0223/`. The test did not change the selected DS-ACK kernel, Hybrid Vendor or radio bridge and did not touch G965N.

Next, audit the donor model/name values against the star2lte profile, then fix or isolate the exact FaceWidget and Bixby startup failures, address the reproduced Bluetooth HCI timeout, and retest NFC service startup. Keep the unexplained EFS delta recorded and preserve the no-EFS-write rule. Do not call M4 complete until fresh boot, core services and requested hardware checks pass.

## Change record

Reason: record the fresh attempt0223 install, boot result, EFS hash drift, target-identity discrepancy and service failures. Files affected: this report, `docs/PLAN.md`, `docs/bugs.md`, and the read-only `flash-01/runtime/target-device-identity.txt` capture; no ROM source or release artifact was modified. Verification: compare the package/stage hashes, stage receipts, `stage08-recovery.log`, boot properties, 90-second process snapshots, and exact runtime log lines cited above. Rollback: none; the G965F was left booted at the Setup Wizard.
