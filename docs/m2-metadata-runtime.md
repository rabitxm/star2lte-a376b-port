# M2 reaches Welcome after writable metadata diagnostic — 24 October 2026

M2 A376BXXS5AZH4 (Android16 / SDK36.1 / OneUI80500) reached Welcome on the authorized G965F test device. The host observed `sys.boot_completed=1` at 77.7 seconds after reboot. SELinux was Enforcing; SystemServer PID1707, SurfaceFlinger PID1412 and zygote PID1311 remained unchanged through uptime156.61s before the controlled recovery reboot. This is a diagnostic boot success, not acceptance of a production ROM or G965N/Android17.

All temporary SYSTEM files and BOOT have been rolled back and verified. M2 remains in TWRP. M1/G965N were not operated on during this experiment. No data format, vendor/ODM/radio/EFS/CPEFS/modem/bootloader write occurred.

## Cause and evidence

The preceding V6 test passed the Tethering loader and graphics linker but SystemServer crashed:

```text
AconfigFlags: Failed to load aconfig package android.permission.flags
AconfigFlags: ERROR_PACKAGE_NOT_FOUND: package android.permission.flags cannot be found on the device
java.lang.IllegalStateException: Missing permission definition for permission "android.permission.RANGING" associated with app op 151
    at com.android.server.appop.AppOpsService.systemReady(Unknown Source:148)
```

Local source: `logs/20261024-m2-runtime-fix/m2-v6-test/logcat-first-adb.txt:17008` and `:63013`. Earlier lines4576/6024 show `mkdir /metadata/aconfig` failing with `Read-only file system`; line6032 shows failure copying `system/etc/aconfig/package.map` to `/metadata/aconfig/maps/system.package.map`.

The installed framework-res SHA256 equals the final staged APK (`5feaf5a4a7ff4e382873c2f732e0ca4716f169f761bb2163d24a8f0d4e6f02b5`). Its RANGING manifest permission is gated by `android.permission.flags.ranging_permission_enabled`. In decoded framework.jar, `AconfigFlags` loads the new storage through `AconfigPackage`; load failure yields no value and the parser skips the element. Meanwhile the generated `FeatureFlagsImpl.rangingPermissionEnabled()` returns true, so AppOps expects the permission. No AppOps exception or permission check was bypassed.

After the new metadata hook, runtime shows:

```text
tmpfs on /metadata type tmpfs (rw,seclabel,nosuid,nodev,noexec,relatime,size=16384k,mode=755)
android.permission.flags.ranging_permission_enabled  enabled  - default read-only system
Permission [android.permission.RANGING]
  sourcePackage=android
  uid=1000 gids=[] type=0 prot=dangerous
```

The maps/boot values are present with `aconfig_storage_metadata_file` labels; platform RANGING package load failures and the AppOps fatal exception are absent in the captured successful boot. Welcome was captured in `runtime/screen.png`. Boot watch and three samples are retained locally. The last pre-recovery sample still had boot_completed1/SystemServer1707 at uptime156.61s.

## Controlled change

Only new variable relative to `m2-v6-test`: add a 16MiB volatile tmpfs mount at `/metadata` at the start of `on early-init` in the existing `system/etc/init/aconfigd.rc`, then restorecon the mount root before donor mkdir/exec_start. The donor retains its flag data and initialization. The device by-name inventory and installed fstab have no metadata block device/mount. No partition was repurposed.

Exact text diff, reason/files/verification/rollback: `diagnostics/m2-metadata-tmpfs.patch`. This diagnostic patch is outside `patches/fork/series` and is not a normal build-system or production change. The experiment changed the SYSTEM init file; the diagnostic BOOT itself was reused unchanged from the preceding test.

Fixed controls from the previous experiment:

| Input | SHA256 |
| --- | --- |
| Diagnostic WIPEONFORK BOOT | `b01912f62ae111570d9de3e45d799da3da4788f27d86f119f78a06a9efb8abd7` |
| Retained S22 Tethering APEX | `601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf` |
| S22 64-bit graphics common V6 interface | `010bf814b8e62ac739ff228fd6cd84477d40279c6e1c86892f064d750ee4d652` |
| Original aconfigd.rc | `78d6bb311aa7126384da4b59780d4a87d9b321c4fd211d04e1339fbd8975447b` |
| Diagnostic aconfigd.rc | `822b51c7e9d748bceed9aab50a504efd0b43e7c1ca15aa62bdab24bf95222357` |

## Limits and remaining failures

- The tmpfs is deliberately volatile. Persistence of staged/local flag updates and metadata consumers across reboots/OTA is not implemented or accepted. Normal release needs an audited storage lifecycle in the existing fork.
- aconfigd still logs `Failed to initialize platform storage in post-fs: fail to get storage files for vendor` (`runtime/logcat-boot.txt:6058`). It creates system/system_ext/product tables before that failure; their presence must not be described as a clean all-container init.
- Camera is unavailable. The fork's pinned native cameraserver SHA256 `06d1c14dd93b39b399beb97e0eb06e0dd0f08a3da2d643a96f588c488b43d061` imports `AudioSystem::streamTypeToAttributes(audio_stream_type_t)`. Installed A37 libaudioclient SHA256 `372dbb7bd076828a2997b6645fbcf5b5d500efe8ab20f19657541ac26dbb44af` does not export it; S22 libaudioclient exports it. Runtime line10703 says `CANNOT LINK EXECUTABLE "/system/bin/cameraserver": cannot locate symbol "_ZN7android11AudioSystem22streamTypeToAttributesE19audio_stream_type_t"`. Do not replace an entire audio stack or alias a differently shaped function without checking ABI/semantics.
- USB debugging reads1, secure `rampart_main_switch_enabled` reads0, `ro.adb.secure` reads0; ADB worked without an on-screen authorization step. Existing data was reused, so this is not fresh-data default validation or a new-PC key-isolation test. Functional Settings toggles remain untested on M2.
- Hotspot/DHCP/DNS/IPv4/IPv6/data saver, radio, camera, sound and reboot/long-term stability remain open. No production kernel selection changed; no M4/G965N acceptance follows from this test.

## Capture and rollback

Raw evidence: `logs/20261024-m2-runtime-fix/m2-metadata-test/`, including runtime logcat/dmesg/flag list/package dump/screenshot and recovery CACHE/pstore/probe13. Raw identifiers and firmware payloads remain local.

Reboot to TWRP succeeded via ADB. Before rollback, current SYSTEM hashes matched the candidate manifest. The exact original aconfigd.rc and A37 APEX (`b2a8b33db76cc9895ffcbb96bfdc24a085c23e52cf2a3b9e26f953cf6343ff22`) were restored; V6, previously absent, was removed. `rollback-system.json` records PASS after read-only remount/hash verification. BOOT rollback readback is `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; `logs/m2-metadata-boot/m2-rollback/result.json` reports no protected partition changed during the write. No full EFS preservation claim across Android boot is inferred from that write-time snapshot.

Next: audit native camera/audio ABI and persistent metadata/container initialization; add tested changes to the existing fork under M2-only guards, retain the released packages, then build and verify a reproducible M2 artifact. Do not advance to Android17 or N on this diagnostic alone.
