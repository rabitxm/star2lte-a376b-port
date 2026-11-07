# M4 post-Setup Wizard checks — 31 October 2026

Scope update31/10: the user confirms current apps download/run and does not
want further NFC/Bixby RF/voice porting. Preserve their advertised support and
dependencies; defer the physical tests below under that decision. Identity/PIF
repair now follows [checkpoint0232](m4-identity-integrity-plan-0232.md).

Device: G965F `M2_G965F_SERIAL`, recovery identity previously verified as
SM-G965F / G965FXXUHFVG6. This was a read-only ADB inspection after the user
completed Setup Wizard. No settings were changed, no test call was placed, and
no Bluetooth accessory was connected during this pass. Sanitized evidence is
in `logs/20261031-m4-postsetup-0230/summary.txt`.

## Results

- Android 17 SDK 37 / One UI 90000, `sys.boot_completed=1`, target identity
  `SM-G965F` / `star2lte`, SELinux Enforcing. SystemServer PID 2211, SystemUI
  PID 6268 and Bluetooth PID 6174 were unchanged between the 09:03:59 and
  09:04:15 snapshots. Window manager reports no ANR since boot.
- ADB is enabled and keyless: global `adb_enabled=1`,
  `development_settings_enabled=1`, `ro.adb.secure=0`, and USB config
  `sec_charging,adb`. Secure settings show Rampart main switch, ADB blocking,
  USB transfer blocking and SUW main switch all `0`. This confirms Auto
  Blocker is currently off. The Rampart screen was opened read-only and a
  screenshot visibly shows the Vietnamese `Tắt` switch plus its explanation,
  including the USB command-blocking behavior. The UI toggle itself was not
  changed during this inspection; runtime ON/OFF behavior remains untested.
  `adb_wifi_enabled=1` is also set; the required keyless USB path was verified,
  while wireless-debugging behavior was not separately tested.
- Bluetooth is ON with `Bluetooth crashed 0 times`. Current log review contains
  the previously accepted Bixby receiver crash at
  `10-31 08:47:08.942`: `java.lang.IllegalStateException: No matching
  ComponentResult found!`. No Bluetooth crash or FDF1 timeout appeared in the
  current post-setup sample. OPP transfer to another phone passed earlier. The
  user now accepts Bluetooth as PASS; A2DP/AVRCP/HFP audio and reconnect were
  not separately measured and are not a blocker under that acceptance.
- Wi-Fi is connected and Connectivity marks the network validated. No app-level
  network request was made as part of this read-only check. Cellular reports
  the inserted SIM loaded, Viettel LTE in service, voice-call state idle; an
  actual call/SMS/data-app test remains open.
- Camera service is registered, reports three camera devices and zero camera
  error traces. Devices were closed; no photo/video capture was performed.
- NFC service and Samsung adapter are registered; `init.svc.nfc_hal_service`
  is `running` and `nfc.initialized=true`. `service call nfc 6` returns state3
  (ON); `dumpsys nfc` still returns `Error with service 'nfc' while dumping:
  FAILED_TRANSACTION`, which is a dump-path failure and does not mean the
  adapter is off. The user tested a tag/card another phone reads, with the S9+
  screen awake and unlocked; the user held the known-good tag against the
  phone's bottom on two taps while a filtered NFC log capture was active. The
  S9+ gave no response and no matching tag/discovery lines were captured. The
  bottom placement is not confirmed to align with the documented antenna area,
  so an RF failure is not yet conclusive. Logcat previously recorded
  `NFA_EeSetDefaultProtoRouting: Bad ee_handle` and failure to set the T3T
  default route. Corrected bus mapping: the recurring `14370000.hsi2c` NO-ACK is
  on `/hsi2c@14370000/mfc-charger@3b` (I2C-16), while the NFC controller is
  `/hsi2c@108E0000/sec-nfc@27` (I2C-8, `sec-nfc` driver). The NO-ACK must not
  be attributed to NFC. Exact findings and limits are in
  [NFC test 0231](m4-nfc-functional-test-0231.md).
- Location providers are registered and enabled, but the GNSS provider had no
  active request during inspection. No satellite fix was tested.
- Thermal HAL 2.0 is connected. Snapshot values were AP 57.5 C, skin 41.8 C,
  battery 37.9 C and thermal status 2. These are system-reported values under
  the current workload, not an external thermal measurement or a long-duration
  stress test. Brightness was already confirmed visually by the user.
- Crash buffer contains the accepted Bixby exception above; it has no other
  fatal entry in this capture. `dumpsys window` reports `<no ANR has occurred
  since boot>`. AVC denials from the inspection commands running as `su` are
  command-probe artifacts and are not counted as ROM app denials.

## Remaining test plan

1. Compare the installed BZI6 Android17 NFC config/APEX JNI and S9+ `sec-nfc`
   HAL config, firmware and RF calibration against the pinned source. The
   controlled known-good tag test failed with no matching discovery line, but
   current evidence does not distinguish a framework/HAL/configuration fault
   from NFC hardware failure. Do not use unrelated `mfc-charger` I2C errors to
   justify an NFC/kernel change.
2. Run user-directed SIM tests: outgoing/incoming call, SMS, mobile-data app
   access and idle/reconnect. Current LTE registration is only a readiness
   signal, not call or data acceptance.
3. Launch Camera and capture front/back images and video; verify save/reopen and
   inspect camera-provider logs. Current service enumeration is not capture QA.
4. Test GNSS outdoors with a real fix; compare satellite state and location
   updates. Provider registration alone is not GNSS acceptance.
5. Auto Blocker UI/default and explanation are visually verified OFF. To test
   the user's choice behavior, manually toggle it ON then back OFF in the
   visible Rampart screen and confirm ADB blocking follows each state. The
   switch was not remotely toggled because enabling its USB restriction can
   disconnect this diagnostic channel; current OFF preference remains intact.
6. Continue monitoring reboot, core service PIDs, ANR/crash buffers and thermal
   readings during normal use. Do not call thermal hardware accepted without
   the user's physical observation; no external thermometer is available.

No source patch was made from these results. Code changes should follow a
reproducible runtime failure and remain isolated to M4/BZI6/G965F. The accepted
Bixby donor APK remains unchanged per user direction.
