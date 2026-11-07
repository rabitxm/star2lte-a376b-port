# M4 attempt37 — G965F first flash and runtime test

Status: eight-part installation completed; first boot loop observed; both diagnostic BOOT trials completed and original BOOT restored. This document does not establish a successful Android17 boot. Private raw device/partition logs and BOOT captures are retained under `logs/20261027-m4-g965f-flash-01/`, excluded from Git.

## Installation and preboot evidence

- The user's direct instruction authorizes flash/testing and the previously authorized fresh USERDATA format on the G965F test device. Recovery reconfirmed SM-G965F / G965FXXUHFVG6 / TWRP3.7.0_9-0. The M4 full ZIP SHA256 is `b8c70dfded510beed8e14207e0fecbe33e98d3061408c6f10fdc3b1925135f3e`; all8 stage hashes matched their recorded manifest.
- Layout before install: SYSTEM 9,961,472,000 bytes, VENDOR 1,363,148,800 bytes, ODM 734,003,200 bytes, CACHE 629,145,600 bytes. Stage1 recovery log reports each layout check and `E9810: partition layout is ready`, avoiding repartition.
- Stages1–7 passed their recovery success/marker checks. Stage8 host exit0 and marker `next=done` were initially marked inconclusive because `recovery.log` shortened while CACHE was formatted. The complete current log independently records `E9810: formatting data`, `script succeeded:`, `Updater process ended with RC=0`, and `Done processing script file`. That false-negative and recovery evidence are preserved, not overwritten.
- USERDATA reports ext4 and a new UUID; old Settings XML, packages.xml and adb_keys are absent. TWRP remounted the new `/data`, so a duplicate read-only mount returned `Device or resource busy`; the current mounted source was matched to the USERDATA block and inspected read-only instead.
- CPEFS, RADIO, RECOVERY and BOTA0/1/2 SHA256 values match their pre-install values. **EFS hash differs**, cause unknown. Its post-install hash was stable across repeated reads; `e2fsck -fn` returned0 without changing that hash. This filesystem check does not establish preservation of identity, calibration or modem data. No EFS image was restored and no explicit EFS write was issued. The user had previously directed continuation on this dev device after an EFS hash change.
- Installed BOOT full-partition SHA256 `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`. Kernel `cdaa4b6d42f9a5cfdfba0e7cee5327e55b50a396e595ecd4c82ae0cc477102a6`, DTB `20917e427ea02fd219bff84afa119593b5225dec1b7b0347481f9e2596b7cc9b`, ramdisk and enforcing cmdline match the packaged original DS-ACK inputs. Pre-install M2 BOOT was saved and byte-verified for diagnostics; complete M2 rollback requires its matching SYSTEM/VENDOR/ODM package as well.

## Causal limitation

The running M2 baseline used the previously calibrated WIPEONFORK diagnostic kernel (BOOT hash `55f3af8c...`); the M4 artifact uses the original duhansysl 08/05 kernel. Thus this flash changes both donor/version and kernel compared with M2, despite the original one-variable plan. Do not attribute any M4 runtime failure to the donor or Android17 alone. This discrepancy was discovered and reported during installation; no new kernel was built for M4.

## Runtime

First boot monitoring ended after240 seconds with `NO_BOOT_COMPLETED_WITHIN_LIMIT`; the selected phone never appeared through ADB during that window and no runtime log was captured. This does not by itself prove a bootloop, kernel panic or a specific failure. Recovery and CACHE/pstore capture are required. The previously M1-calibrated probe13 was checked to match this exact original BOOT/kernel/DTB/cmdline as a potential later diagnostic; it has not been written.


## First boot recovery and probe13

The user observed a repeated boot logo and returned the phone to TWRP. `/cache/unica` was absent. The captured `/proc/last_kmsg` includes TWRP recovery activity (for example `init: processing action (sys.usb.config=mtp,adb && sys.usb.ffs.ready=1) from (/init.recovery.usb.rc:24)`), so it must not be presented as the M4 failure. Available pstore and kernel logs do not yet demonstrate a cause. Raw captures and hashes are private under `logs/20261027-m4-g965f-flash-01/first-boot-recovery/`.

The EFS raw hash changed again across the M4 boot/recovery cycle; cause and effects remain unknown. CPEFS, RADIO, RECOVERY and BOTA hashes still match the post-install state. No EFS write or restore was issued.

For diagnosis, the previously M1-calibrated probe13, whose base is the exact installed M4 BOOT, was written after capturing first-boot evidence. Its only image change is the diagnostic ramdisk; kernel, DTB, and cmdline remain identical. Full readback verified probe SHA256 `62f81109e81147e47c9d2e9399e21ee8910e3ee7f7d71bcdee997240f9ab3ad1`; EFS and the other guarded partition hashes did not change during this BOOT write. Rollback BOOT `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d` is saved and verified. Runtime observations and rollback must be recorded after capture.


### M4 probe result and targeted linker comparison

Five probe13 slots decoded with valid frame checksums, 2,832 frames each, zero reported dropped/filtered records and last heartbeat 107–108ms. They record `INIT_EXEC_PIPE_EOF=0` and stop before useful M4 init fatal/shutdown messages; this does not prove exactly where or why PID1 fails. Guarded rollback restored the original M4 BOOT, hash `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`, with no protected hash changes during the rollback.

The installed M4 SYSTEM was mounted `ro,noload` in recovery and its bootstrap linker executed in chroot on the S9+ CPU. Each `--list` attempt for `/system/bin/init`, `/system/bin/sh`, and `/system/bin/secilc` produced `arc4random data MADV_WIPEONFORK failed: Invalid argument`, exit134 on TWRP's 4.9.118 kernel. The mount was successfully unmounted on retry. This is not itself a test of the ROM kernel. Prior M1 control had independently reproduced the same MADV error on the original DS-ACK kernel using the M2 runtime linker; that historical evidence does not prove M4's actual boot failure.

A one-variable diagnostic comparison is underway using the pre-existing, M1-calibrated WIPEONFORK-backport BOOT `b01912f62ae111570d9de3e45d799da3da4788f27d86f119f78a06a9efb8abd7`. Its previously audited ramdisk, DTB and cmdline match probe13, differing in kernel only; no new kernel was built. Readback matched and protected partition hashes did not change during the BOOT write. M4 SYSTEM/VENDOR/ODM remain unchanged. Restore the original M4 BOOT after collecting runtime evidence.


### WIPEONFORK diagnostic result

The user observed a repeated logo and returned the machine to TWRP. Probe slot6 recorded 105,278ms and 7,336 frames with no dropped frames, but filtered 889 kmsg records. It reached `init: init second stage started!` and recorded both `boringssl_self_test64_vendor` and `boringssl_self_test64` exiting status0. This demonstrates substantially later progress than the original-kernel probe, not successful M4 boot or proof of every original-kernel failure.

The same boot parsed `/vendor/etc/init/hw/init.samsungexynos9810.rc` and `/vendor/etc/init/init.samsungexynos9810.rc`; both run `mount_all /vendor/etc/fstab.${ro.hardware} --early`. The second run logged `__mount return -1(source=CACHE,target=/cache,...): Device or resource busy`, the same for USERDATA, and `do_mount_all(917) fs_mgr_mount_all returned an error`. It also repeatedly logged `Service 'mobicore' ... exited with status 1`. These are actual failures; causality for failure to reach Android remains to be tested. The two rc files are not identical: the canonical one also contains the CACHE metadata bind and differs in encryption and CPU control commands. Do not simply delete one without preserving intended settings and testing. The unmodified fork's metadata finalizer only removes a duplicate metadata bind, not the duplicate mount_all actions.

The observed first fs pass mounted EFS successfully; the installed fstab has no `ro` flag for EFS, so Android can write it. The EFS raw hash changed across this trial as well, with no direct EFS partition write or restore issued by the operator. CPEFS mount attempts reported `Read-only file system`; its raw hash was unchanged. Do not infer EFS identity integrity or claim its hash change is harmless. Guarded BOOT rollback returned to `cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d`; all protected hashes were unchanged during that rollback itself. Device is in TWRP.

Next diagnosis: inspect the two init scripts and `/vendor/etc/fstab.samsungexynos9810` with the existing fork build logic, preserve the canonical metadata bind, and establish a controlled single-mount test with explicit EFS/CPEFS safety checks. Investigate `mobicore` and post-fs-data/logcat separately; the recorder filtered some late kmsg, so absence of additional messages is not evidence of absence.
