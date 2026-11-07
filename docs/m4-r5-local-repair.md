# M4 r5 local fixes — 31 October 2026

**Update 01/11/2026:** Setup Wizard complete. The repaired hide-developer page
passes open/toggle/back/reopen/app-selection persistence tests. Four fresh
requests (checker and built-in Play Store, PIF ON/OFF) still yield no Integrity
labels. Actual Play Store fingerprint readback changes with the switch; no old
field-write errors recur in these request captures. PIF is restored ON.
Readback of all fields inside GMS and chain trust analysis remain open; do not
infer Google acceptance from key generation. See [post-setup validation and
next steps](m4-r5-post-setup-validation.md). Pending statements below describe
the initial boot checkpoint and are superseded by this update.

## SamsungPropsHooks

Reason: r4 GMS unstable logged `Failed to spoof Build.MODEL` and
`IllegalAccessException` at 20:00:11.789. The prior GMS/Play/GSF branch
also targets the physical S9 identity, superseded by the A37 public identity
requirement. In M4 only, remove that obsolete branch, retain the existing
Samsung app branches, and use the ART final-field writer already validated
for r4 PIF. Instrumentation calls SamsungPropsHooks before PIF.

Affected files: `rom/unica/mods/settings/a376b_android17_boot_limits.py` and
`rom/scripts/internal/apply_modules.sh`. The latter calls the scoped adapter
after the fork's Settings smali patch pass; other targets keep existing code.
Verification: pinned method preimages, wrong/repeated preimage rejection,
smali assembly, compiled hook scope/order and writer checks, then live process
readback and PIF ON/OFF test. The Google verdict remains an independent test;
no claim of passing Integrity is made from this fix.

## Hide developer status page

Reason: r4 Settings crashed at 19:51:22.630 with `NoSuchFieldError: No field
mMainSwitch` in `HideDeveloperStatusFragment.onCreateView`. BZI6 donor
`PrintServiceSettingsFragment.onCreateView` inflates its own
`sec_settings_main_switch_bar` into the fragment view; use that donor layout
for this fragment while retaining the existing listener and saved setting.

Affected file: `rom/unica/mods/settings/a376b_android17_boot_limits.py`.
Verification: pin original fragment method, validate donor layout, assemble
smali, gate compiled replacement and then open/toggle/reopen on device.

Both source transformations passed isolated smali assembly and reject repeated
application. Clean build passed the compiled Settings/framework, PIF, keyless
ADB and SELinux gates, ext4 image gates, and flashable verifier. Independent
SHA256 checks passed the target-files and full flashable; the eight ZIP32 stages
passed signature/CRC and independent hashes. Full flashable SHA256:
`48d488548ffdbc823cf8d64baca5e12c2e516c05d8ded1c0e6fdcc30c6caba34`.
Release: `rom/out/releases/M4/star2lte-1793452083-5ITTem/`.

All eight stages were installed in order on G965F `M2_G965F_SERIAL` in TWRP;
stage08 formatted USERDATA. EFS/CPEFS/RADIO/RECOVERY/BOTA0-2 pre/post hashes
match. `e2fsck -fn` passed SYSTEM/VENDOR/ODM/USERDATA/CACHE/OMR/HIDDEN; the
installed SecSettings APK and framework JAR hashes match the build. Boot reached
`sys.boot_completed=1`, ADB keyless, Enforcing, public `SM-A376B` and hardware
`SM-G965F`, with Setup Wizard incomplete. An isolated on-device probe called
the installed Samsung hook's writer and read back Build.MODEL (String) and
SEM_FIRST_SDK_INT (int) successfully. This does not establish live GMS behavior.

Before flashing, actual UFS logical block size was found to be **4096** bytes;
GPT signature at byte offset4096 confirmed this. Both new full snapshots pass
header and partition-array CRC and contain identical primary/backup arrays.
Earlier r4 snapshots described as 512-byte sectors are not a complete GPT
backup for this device; preserve the new 4096-byte snapshots. No GPT restore
was performed. Evidence is in `logs/20261031-m4-r5-repair/`.

Initial boot logged `PlayIntegrityHooks: shouldBypassTaskPermission: unable to
get gms uid` with a caught `NullPointerException` because PackageManager was
not yet available. SystemServer subsequently reached boot complete; the log
does not establish impact on later requests. One linker startup error occurred:
`CANNOT LINK EXECUTABLE "/system/bin/smdexe": cannot locate symbol
"_ZNSt3__18ios_base7failureD0Ev" referenced by "/system/lib64/libc++.so"...`.
A later `smdexe` process exists and telephony snapshot says voice/data LTE
IN_SERVICE; this does not prove call/data testing or the reason for the early
failure. The user-accepted Bixby receiver still logs `No matching
ComponentResult found!`. Settings UI switch/list interaction and actual GMS
ON/OFF and server verdict tests remain pending completion of Setup Wizard. No
kernel, Hybrid Vendor, radio or protected partition write is part of these
source fixes. Rollback: retain current r4 package and existing partition backups;
revert the scoped staging changes and build again.
