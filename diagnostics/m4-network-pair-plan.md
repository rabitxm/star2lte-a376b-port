# M4 paired networking APEX diagnostic

Reason: M4 reaches post-fs-data after patches0189/0190, then its native
Tethering371021120 requests reboot because NetBpfLoad requires kernel5.10.
A single older Tethering replacement would remove the resolver provider that
the unchanged BZI6 netd needs. The following is a bounded compatibility test,
not acceptance of older networking components as a finished Android17 port.

## Candidate and scope

Use the original, unmodified, signed S22 Tethering360743340 and
Resolver360735100 together. Their manifests pair
`libcom.android.tethering.dns_helper.so` and `libnetd_resolv.so`; all six
versioned LIBNETD_UPDATABLE/LIBNETD_RESOLV symbols required by the actual BZI6
netd are exported by this pair. Each module's public key matches its BZI6
counterpart. Container v3 signatures and payload AVB verify successfully.
This does not prove callback layout, CFI, Java/framework or runtime behavior.

On the designated G965F, replace only these two SYSTEM files:

- system/apex/com.google.android.tethering_compressed.apex
- system/apex/com.google.android.resolv_compressed.apex

Keep BZI6 netd, framework.jar/services.jar, UI and all other SYSTEM components.
Recreate the already tested0189/0190 VENDOR changes and CACHE logger; use the
same pre-existing WIPEONFORK diagnostic BOOT. Relative to that tested baseline,
the only functional variable is the paired networking APEX composition.
Do not disable BPF loading/reboot checks or patch a kernel-version report.
No new kernel build, USERDATA format or protected-partition write is involved.

Hashes:

| Module | Original BZI6 | S22 candidate |
| --- | --- | --- |
| Tethering | 540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d | 601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf |
| Resolver | 0a8fb786a31df2bdeb3ac9ae50fbfb3c219dd5948e71ab3db1174c6afc2bf433 | f1699315c03415a8b8948ef35bf7ac39c37644f3de4ac4487fd05a3324a7c96f |

Original files were pulled from SYSTEM mounted ro,noload and matched the donor.
Mode0644, UID/GID0:0, system_file labels and original mtime are recorded.
Byte-verified originals remain on the PC and are copied into CACHE before the
first SYSTEM write. Raw firmware and signatures remain local, outside Git.

## Evidence and rollback

Require exact model/bootloader/partition checks and old/new hashes before each
write. Verify both candidates before the first replacement; check file hashes
and metadata afterward. Compare netd/framework/services hashes and all measured
partitions outside SYSTEM. Preserve old CACHE logs before the trial so the
new logger cannot append evidence from an earlier composition.

Require actual APEX activation/version evidence, bpfloader exit status and
subsequent init/SystemServer/CFI/linker errors. A boot alone does not accept
networking: mobile data, DHCP/DNS, IPv4/IPv6, hotspot/offload/data saver and
Android17 framework/API behavior need separate tests. Do not claim intact
Android17 networking while using these older diagnostic modules.

After capture, restore the two exact BZI6 files with original owner/mode/label
and mtime, restore the original full BOOT and saved full VENDOR, and verify.
SYSTEM file rollback is a content/metadata rollback, not a promise of an
identical raw ext4 partition hash after allocation/journal changes. USERDATA
APEX caches may change during activation. No EFS/CPEFS restore/write is allowed;
their across-boot hashes are tracked separately.

Local scripts, preflight, identities, original images and runtime results:
`logs/20261027-m4-network-compat/`. No new complete ROM is built by this test.

## Completed first trial —27 October2026

Both candidates activated. Fresh CACHE log records bpfloader exit0, then
SurfaceFlinger/zygote missing common-V6 and the Android17 libandroid_net
missing AConnectivityNative_getNetworkBlockedReason from the older module.
Bootchecker requested recovery; Android did not boot. Exact evidence and
limits are in docs/m4-bpf-runtime-audit.md. Both APEX file bytes/metadata and
full BOOT/VENDOR were restored and verified. Retained releases were unchanged.
Next diagnostic adds only pinned graphics V6 to this same composition; it
still does not promote the older network pair to production or claim its APIs
compatible. EFS changes across Android/recovery remain tracked separately.
