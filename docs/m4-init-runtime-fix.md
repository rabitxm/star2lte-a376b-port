# M4 init mount ownership and Trustonic startup diagnosis

27 October 2026. Continuation of attempt37 runtime testing on G965F only.
M4 has not reached Android. Device evidence stays private under
`logs/20261027-m4-init-fix/` and the earlier first-flash directory.

## Observed failures and limits

The WIPEONFORK diagnostic BOOT reaches second-stage init and both BoringSSL
self-tests exit0. The same boot parses the canonical vendor hw init and the
legacy top-level vendor init; both execute early and late fstab mounts. The
second early mount reports `Device or resource busy` for CACHE/USERDATA and
`do_mount_all(917) fs_mgr_mount_all returned an error`.

The last recorded late-fs action is from
`/vendor/etc/init/pa_daemon_kinibi.rc:11`. Its next command is
`wait_for_prop vendor.sys.mobicoredaemon.enable true`. Meanwhile init records
`Service 'mobicore' ... exited with status 1` repeatedly. This is a concrete
startup dependency to investigate; deleting the wait would hide the failure.
Mount duplication is independently demonstrated but is not yet proven to be
the sole cause of failed boot.

The first candidate's M4 profile inherits `EXYNOS9810_ENABLE_INIT_DEBUG=false`.
Its missing `/cache/unica` is therefore expected, including after a successful
post-fs transition. The earlier recorder is passive and filters some kmsg;
logcat must be captured to diagnose the daemon itself.

## Patch0189

Reason: establish a single owner for fstab mounts without discarding differing
legacy hardware actions. Affected fork files:

- `a376b_metadata_bind.py`: M4-only transformation replaces exactly the early
  and late `mount_all` commands in the secondary rc with ownership comments;
  canonical rc retains both mounts and the existing CACHE metadata bind.
- `a376b_android17_runtime_compat.py`: final runtime gate checks that ownership;
  its fixture includes both early and late mounts. The existing real-ext4
  gate calls this verifier and then compares image rc bytes with the work tree.

All other secondary rc commands remain byte-identical; no merge of encryption,
CPU controls or hardware services is performed. M2 transformation is preserved.
Unknown or extra mount commands are rejected. Source patch:
`patches/fork/0189-deduplicate-m4-fstab-mount-actions.patch`.

Verification: fixtures from actual installed rc files check idempotence,
unrelated-command preservation, rejection of an extra fstab mount and missing
markers/canonical mounts, plus M2 byte preservation. Full M4 runtime self-test
passes. All179 source patches replay from the pinned fork into125 files
identical to the checkout; previous178 patch hashes are unchanged.

## Controlled device test

Save and hash the complete current VENDOR before edits; preserve the original
M4 BOOT rollback image. The only functional VENDOR edit is the two secondary
mount commands above. Add the existing fork's unchanged debug rc/logger as
diagnostic instrumentation, with the same UID/GID/mode/SELinux metadata used
by its M2 logger. Use the already calibrated WIPEONFORK diagnostic BOOT so
kernel/ramdisk match the immediately preceding diagnostic trial. SYSTEM/ODM
and radio are not edited. Do not remove the Trustonic readiness wait or bypass
the cryptographic checks.

Capture probe/CACHE/logcat before rollback. Restore original BOOT and the saved
VENDOR; compare their full hashes and all measured protected partitions during
each write/rollback. EFS hashes can change across Android/recovery and remain
unverified; no direct EFS/CPEFS write/restore is allowed. A runtime mount is
not a guarantee that these partitions are unchanged.

VENDOR backup is byte-verified (1,363,148,800 bytes). The diagnostic apply
initially stopped because recovery's BusyBox `chcon` returned1 at a file whose
readback already had the requested vendor_file context. It had not yet set the
new debug rc's label. No reboot was issued. That label was set successfully;
all three files' bytes, mode, owner/group and contexts are separately verified
with SHA256, toybox stat and label readback. Recovery's BusyBox stat does not
support `-c`, so the audit explicitly uses its verified toybox alternative.
These recovery-tool corrections do not relax the intended image metadata.

Preboot audit passed file bytes/UID/GID/mode/contexts and whole SYSTEM/ODM/protected partition hash equality. The BOOT diagnostic write/readback passed with no protected changes; the controlled boot is running. No successful M4 boot is claimed.


Runtime window:120 seconds elapsed without any ADB Android transport and without
boot_completed evidence. A physical TWRP return was requested; PC currently sees
no device. BOOT remains the calibrated diagnostic b01912f6 and VENDOR contains
only the two-command mount edit plus the fork logger. BOOT/VENDOR rollback is
pending recovery connection and evidence capture; do not report it completed.
Verified rollback inputs and scripts are retained in the private diagnostic
directory. No new complete ROM package was created and the retained M4 release
is unchanged.


## Runtime result after patch0189

Recovered from TWRP with a real `/cache/unica` marker and nonempty start/15s/60s
logcat. The unchanged fork logger exits0; init-context dmesg/props remain empty.
Probe generations7/8 decode with zero reported dropped frames, but filter857/880
kmsg records; each records about129 seconds. Neither repeats the secondary
early mount or `fs_mgr_mount_all returned an error`, which was present in
generation6. No post-fs-data transition or Android boot is established.

Selected logcat evidence from result/unica/logcat_start.txt:

- 4414–4415: `ueventd: Parsing file /vendor/etc/ueventd.rc...` then
  `Unable to read config file '/vendor/etc/ueventd.rc': open() failed: No such file or directory`.
- 5461: `TeeMcDaemon: setup: Permission denied opening admin device`
  (`SecureWorld.cpp:281`).
- 5462: `TeeMcDaemon: Can't open secure world`
  (`MobiCoreDriverDaemon.cpp:254`), followed by mobicore exit1.

The daemon ELF contains `/dev/mobicore` as its admin device path. The saved
VENDOR's `/ueventd.rc` is byte-identical to the fork's base/common overlay
(SHA256628e3fee174b841c0cf771092034e9e043264c8232e707b4710bba2b0a69645a).
It assigns `/dev/mobicore 0700 system system`; M4 never imports this old path.
Missing hardware node permissions are a supported explanation for the observed
EACCES; exact runtime node metadata/readiness must still be validated.

Original M4 BOOT cc618abc was restored with no protected hash changes during
that write. EFS changed across the Android/recovery cycle; CPEFS/radio and the
other measured protected partitions retained hashes. VENDOR restoration is
being verified before the next isolated trial.

## Patch0190 — M4 imported ueventd path

Reason: publish the existing pinned S9+ node rules at the path M4 logs it imports.
Affected: M4 runtime apply/verify, final boot_restore fs_config/file_context and
real-ext4 verifier. Preserve `/vendor/ueventd.rc`; create `/vendor/etc/ueventd.rc`
with identical bytes, root:root0644 and vendor_configs_file context. No daemon,
radio or hardware blob replacement; no widened node permissions beyond the
existing S9+ rules. Reject unreviewed source or an existing different destination.

Verification: full runtime apply/idempotence/verify and destination drift
rejection pass;180 source patches replay to125 identical files,179 older locks
unchanged. Device test and new actual image acceptance are pending. Rollback is
the saved VENDOR and original M4 BOOT, or reversal of0190 followed by rebuild.


Rollback of the patch0189 trial is complete: BOOT cc618abc and VENDOR
025f7314775af68d0449143328923215fe14af25c076881ef2a7cb0d9ef3c9f3 match the
saved originals; protected hashes are unchanged during each rollback. The
following0190 trial reuses that same original VENDOR snapshot without another
firmware copy. It adds only the imported ueventd file relative to the previous
functional test. Diagnostic logger additionally lists the two Trustonic device
nodes; it does not open them or change their permissions itself.


Patch0190 preboot: all four modified/diagnostic files passed exact bytes,
UID/GID/mode and context readback. Whole SYSTEM/ODM and measured protected
partition hashes were unchanged by the VENDOR edit. Diagnostic BOOT readback
is b01912f6, the same as the preceding trial, with no protected changes during
that write. The controlled runtime test is running; rollback is pending its
evidence capture.


## Final result of the0190 trial and source0191

The imported S9+ ueventd file is parsed. Logcat records `TEE is ready`, driver
open and daemon service startup; the former admin-device EACCES no longer
blocks init. Post-fs-data is reached. The extra node-listing snapshot is empty,
so no final node metadata is inferred from it. The next explicit reboot is
`NetBpfLoad: Android 25Q4 requires kernel 5.10.`, bpfloader exit7 and
`reboot,bpfloader-failed`. Full networking ABI evidence and the rejected simple
S22 APEX substitution are in docs/m4-bpf-runtime-audit.md.

Rollback is complete: BOOT cc618abc and VENDOR025f7314 match the saved originals;
protected hashes did not change during rollback. G965F remains in TWRP. EFS
changes across Android/recovery remain unverified; no direct EFS write/restore
was issued. No new complete ROM release has been built.

Patch0191 adds `m4-g965f-a376b-android17-runtime-init-debug.sh`, selecting the
already calibrated archive4cea63f8 and the existing CACHE logger. The original
M4/M1 production profiles are preserved. The real-ext4 M4 gate now requires
logger bytes and root-owned config/root:shell executable metadata when the
diagnostic flag is selected. Shell syntax and diagnostic-to-production/M1
reset tests pass;181 patches replay to126 identical files,180 previous locks
unchanged. The first replay destination was unavailable on the secondary mount;
the identical text replay succeeded in the local ignored diagnostic directory.
This is source/profile validation; a new packed-image gate must still run when
next building a ROM. Removing0191 or selecting the original profile reverses
that selection. No kernel rebuild was performed.
