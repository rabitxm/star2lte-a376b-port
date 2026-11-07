# M4 BPF restart and networking ABI preflight

27 October 2026. Initial read-only audit and subsequent paired-network diagnostic.
M4 still does not boot Android. A reversible S22 Tethering+Resolver experiment
was captured and rolled back; the retained ROM and normal M4 networking lock
remain unchanged. No loader failure bypass or new kernel-version patch was made. Private evidence: `logs/20261027-m4-bpf-audit/` and
`logs/20261027-m4-ueventd-fix/result/`.

## Device result

M4 ueventd parses `/vendor/etc/ueventd.rc` after patch0190. Logcat_start.txt:5456–5468
records the Trustonic driver version, `TEE is ready`, service startup and
`driver open`, replacing the previous `Permission denied opening admin device`.
Init proceeds to post-fs-data and creates its CACHE marker. The extra diagnostic
device-node listing file is empty, so it is not direct proof of the node's final
mode/owner/label. Successful daemon access establishes that the prior open
failure was cleared; complete Trustonic/security functionality is not accepted.

The new explicit restart trigger, logcat_live.txt:20901–20906:

```text
NetBpfLoad: Android 25Q4 requires kernel 5.10.
init: Service 'bpfloader' ... exited with status 7
init: Service bpfloader has 'reboot_on_failure' option and failed, shutting down system.
init: Got shutdown_command 'reboot,bpfloader-failed' Calling HandlePowerctlMessage()
```

Probe generations9–31 retain repeated boots. Frames decode successfully but
the recorder filters later kmsg; use logcat for the complete selected error
sequence. This is userspace-requested reboot evidence, not proof of a kernel
panic or that every networking requirement can be met by changing a version
check. Both crypto self-tests had already passed on the diagnostic kernel.

## Actual networking inputs

| Input | Identity |
| --- | --- |
| BZI6 Tethering | com.android.tethering version371021120, compressed APEX SHA256540ee27099939f2d1e698228ced265b2505dca1f82af2d785c43b37e0c2b652d |
| Earlier S22 candidate | com.android.tethering version360743340, SHA256601d6c2bb7a81e9d8938ac3681cc95887e49ce1aafe58d3632ba92c586187cbf |
| Installed M4 netd | SHA2569ac97406a17eb142de8c397c2fbb122e4993fbc50bc7884961027bfe84e332e7; matches BZI6 donor |

The two Tethering APEX public keys match, but that does not establish ABI or
framework compatibility. The installed netd has an explicit DT_NEEDED for
`libnetd_resolv.so`. BZI6 Tethering contains/provides this library; S22 Tethering
does not. A scan of all BZI6 APEX manifests finds only Tethering referencing
this library, and there is no system/lib64 copy or symlink. BZI6's separate
com.android.resolv APEX version371020260 has no lib64 directory or native-library
provider declaration. Thus replacing only the full Tethering APEX with S22,
as tested on M2, would remove a required provider for the actual M4 netd.

Native dependency results and protobuf manifests are recorded in
`network-abi-preflight.json`, `BZI6-netd-needed.txt`, manifest comparisons and
payload file inventories. No runtime compatibility is inferred from matching
signing keys. No old APEX has been flashed on M4.

## Next compatibility boundary

Keep the native Android17 netd/resolver and framework interfaces in scope.
Audit the loader, required BPF program/map/helper set and users of those maps
against the actual DS-ACK capability tests before implementing a bounded
backport or an explicitly versioned networking compatibility composition.
Validate native exports and framework/JNI interfaces before a device trial.
The loader's kernel-generation guard alone is insufficient evidence that a
binary branch patch is correct. Do not suppress its failure/reboot handling
or mark networking ready without its required initialization.

Use the new explicit M4 runtime diagnostic profile, which selects the existing
hash-verified WIPEONFORK kernel package and enables CACHE logging. It does not
rebuild the kernel or silently change the original production profile. Actual
BPF/network fixes are not yet implemented; mobile data, DNS, IPv4/IPv6,
hotspot and Android17 app/hardware acceptance remain open.

## Rollback

Diagnostic BOOT and VENDOR were restored after evidence capture: full BOOT
SHA256cc618abc72e8e568e56a8c643c906cff0d7b9143dfc77d86dc645f55776ed01d and
VENDOR SHA256025f7314775af68d0449143328923215fe14af25c076881ef2a7cb0d9ef3c9f3
match the saved originals. Protected hashes did not change during either
rollback. EFS changes across Android/recovery remain separately recorded;
no direct EFS/CPEFS restore/write was issued. The G965F is in TWRP. M1/G965N
were not operated on during these experiments. Retained release files were
not replaced, and no new complete ROM package was built.

## Paired S22 modules — actual M4 runtime trial

Private evidence: `logs/20261027-m4-network-compat/result/`. CACHE/unica was
isolated before boot; recorder generations32–34 belong to this trial. Earlier
mmap generations must not be attributed to it. Original signed S22 Tethering
360743340 and Resolver360735100 activated together (logcat_live.txt:5915–5916
and5940–5941). Container v3 and payload AVB had verified, and all six versioned
native exports needed by the unchanged BZI6 netd were present.

Fresh logcat_live.txt:10914 records:

```text
init: Service 'bpfloader' (pid 1725) exited with status 0 waiting took 0.248000 seconds
```

This clears the prior immediate loader restart for this composition; it is
not acceptance of Android17 networking, offload, IPv6 or hotspot. The old loader
also reports synthetic kernel0x504012e while UprobeStats reports actual
0x4090151; this discrepancy was already observed in earlier M2 logs. No new
version spoof was introduced. Real BPF capabilities and networking remain open.

The next fatal loader boundary is explicit (live:11823,13547):

```text
CANNOT LINK EXECUTABLE "/system/bin/surfaceflinger": library "android.hardware.graphics.common-V6-ndk.so" not found: needed by /system/lib64/libgralloctypes.so in namespace (default)
CANNOT LINK EXECUTABLE "/system/bin/app_process64": library "android.hardware.graphics.common-V6-ndk.so" not found: needed by /system/lib64/libgralloctypes.so in namespace (default)
```

The existing finalizer reintroduces the embedded Hybrid Vendor gralloctypes
with a V6 DT_NEEDED after the M4 runtime helper stages BZI6 V7. V7 does not
satisfy the V6 SONAME. Patch0192 adds the exact already reviewed S22 V6 64-bit
interface alongside V7, with source/work/ext4 gates. Details:
`docs/m4-graphics-v6-runtime.md`.

The network composition also exposes a separate Android17 ABI gap
(live:13933):

```text
CANNOT LINK EXECUTABLE "/vendor/bin/hw/gpsd": cannot locate symbol "AConnectivityNative_getNetworkBlockedReason" referenced by "/system/lib64/libandroid_net.so"...
```

Native netd exports alone therefore do not close the newer platform networking
ABI. Keep this pair diagnostic; do not silently put it into the M4 runtime
lock or replace the whole framework. The final selected restart is
`reboot,recovery, bootchecker R6` (live:60658), after repeated core-service
link failures. No boot_completed or hardware/network acceptance was obtained.

After capture, both original BZI6 APEX files were restored with exact bytes,
owner/mode/label/mtime. BOOT and VENDOR match full baseline hashes shown above;
all helpers report PASS and no measured protected partition changed during
their respective writes. SYSTEM is restored at those file boundaries, not
claimed raw-ext4 identical. USERDATA/APEX caches may have changed at boot.
No EFS/CPEFS/modem restore/write was issued. The next isolated diagnostic keeps
that same pair and diagnostic BOOT/VENDOR, adds only the locked V6 interface,
and will capture/rollback again without a fresh format.

## Android17 native API boundary narrowed

The V6 trial confirms the newer symbol is imported directly by public
libandroid.so during zygote preload, in addition to libandroid_net/gpsd/media.
Replacing only libandroid_net with S22 would therefore leave the zygote fatal
and remove the newer public android_getnetworkblockedreason export.

Read-only disassembly of the actual BZI6 native connectivity provider shows
AConnectivityNative_getNetworkBlockedReason calls getsockopt with SOL_SOCKET
and option0x0ad01d01, returning negative errno on failure and the low result bit
on success. It does not invoke the Java ConnectivityNativeService Binder
interface in that function. Thus an invented Java method or false-success
stub is not justified. Retaining the real BZI6 native provider in an explicitly
composed, signed diagnostic module is the next candidate to audit, including
its socket-option behavior on4.9 and all old/new consumers.

The BZI6 provider preserves the public AConnectivity functions but removes
three old generated C++ default-implementation exports. Additional strong
imports are AIBinder_Class_getDescriptor, AIBinder_getClass, __errno and
getsockopt. Exact native import/export and call evidence is retained in
native-provider-comparison.json/new-api-disassembly.txt. This is not yet a
composed or device-tested APEX; caller ABI/CFI, signing, activation and data
path behavior remain required. Do not downgrade libandroid.so or fabricate
network-blocked state.
