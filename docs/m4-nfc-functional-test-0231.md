# M4 NFC tag check — 31/10/2026

Scope update after this test: the user does not use NFC/Bixby and asks to stop
porting their RF/voice functions, while retaining advertised support and app
dependencies. Apps currently download/run normally. The test result below is
retained as an unresolved functional limitation, not a blocker for the new
identity/PIF work and not a reason to declare NFC unsupported. Current plan:
[checkpoint0232](m4-identity-integrity-plan-0232.md).

Device: G965F `M2_G965F_SERIAL`, M4 attempt0229, running Android 17 / One UI 9.
No phone partition or setting was modified during this check.

## User test and qualification

The user confirmed NFC was enabled, the screen was awake and unlocked, and the
tag/card reads on another phone. The S9+ produced no visible reaction. The tag
was held against the **bottom of the phone** on two attempts during the
09:21–09:22 log-capture window. Samsung's S9+ user manual says to align a tag
with the NFC antenna area on the back and depicts the NFC antenna/wireless
charging coil as a marked region; it does not identify the bottom edge as the
antenna area. Therefore this is a valid negative test for the lower back, but
not yet a conclusive RF failure until the tag is tested over the marked
antenna/coil region. See the [Samsung Galaxy S9+ user manual, NFC instructions](https://images.samsung.com/is/content/samsung/p5/ca/support/mobile-devices/galaxy-s9-plus-how-do-i-access-the-user-manual-on-my-samsung-galaxy-s9-plus/pdf/SM-G96X_UG_EN4.pdf).

## Runtime evidence

- `service call nfc 6` returned `Parcel( 00000000 00000003 ...)`; the decoded
  adapter state is 3 (ON). `nfc.initialized=true`, and
  `init.svc.nfc_hal_service=running`.
- `/dev/sec-nfc` exists with owner/group `nfc:nfc`. The live device tree names
  `/hsi2c@108E0000/sec-nfc@27`; sysfs binds I2C device `8-0027` to driver
  `sec-nfc`.
- `dumpsys nfc` and `dumpsys samsungnfc` return
  `FAILED_TRANSACTION`. This is a Binder dump failure; the typed state query
  above still succeeds.
- Earlier logcat contains the exact route failure
  `NFA_EeSetDefaultProtoRouting: Bad ee_handle`, followed by
  `RoutingManager::updateDefaultProtocolRoute: Fail to set default proto
  routing for T3T` (10-31 09:06:07.364). This identifies a routing error, but
  does not by itself explain tag polling.
- The filtered live log capture contained zero matching NFC/tag-discovery
  records during the two taps. It does not establish whether the RF field was
  reached because both taps were at the phone's bottom, away from the
  documented antenna area.
- Corrected earlier interpretation: the recurring HSI2C NO-ACK at
  `14370000.hsi2c` belongs to `/hsi2c@14370000/mfc-charger@3b` (I2C-16), whose
  driver is `mfc-charger`. NFC is on `/hsi2c@108E0000/sec-nfc@27` (I2C-8).
  Those charger errors are not NFC evidence and must not motivate an NFC kernel
  change.

## Next test and repair gate

1. Keep NFC on and the screen awake/unlocked. Sweep the tag over the middle of
   the back where the manual marks the NFC antenna/wireless charging coil, hold
   it briefly at each position, and avoid the USB cable/metal objects. Repeat
   with the same tag that reads on the other phone.
2. Capture logcat during that placement. If a response appears, record the NFC
   placement test as PASS and keep the T3T route error as a separate eSE/routing
   issue. If there is still no response and no tag callback, compare the
   installed BZI6 NFC APEX/JNI and `/system/etc/libnfc-nci.conf` against the
   hash-pinned donor, then compare the target `libnfc-sec-vendor.conf`,
   S3NRN82 firmware and RF calibration against the G965F source package.
3. Only patch the exact mismatch reproduced by that comparison. Do not alter
   kernel, NFC device tree, radio, EFS, or hardware configuration based on the
   unrelated MFC-charger NO-ACK.

## Evidence files

Sanitized state snapshots are in
`logs/20261031-m4-postsetup-0230/nfc-followup/pre-state.txt` and
`post-state.txt`. The filtered capture window contained no matching lines;
the empty `tag-correlated.log` records that result. No source patch was made.
