# M2 AZH4 ABI review — 22/10/2026

## Blocking evidence

`logs/20261021-m2-build/finalstages76.txt:90`:

```
Missing system graphics interface: system/system/lib/android.hardware.graphics.mapper@2.0.so
```

The existing fork's final vendor restore requires system graphics interfaces for both lib and lib64. AZH4 source system actually declares arm64-v8a only, an empty ro.system.product.cpu.abilist32, and contains app_process64 but no app_process32. The staged hybrid vendor retains ro.zygote=zygote64_32. Its init.zygote64_32.rc launches /system/bin/app_process32. These are static inconsistencies; M2 has not been flashed and no runtime failure is claimed.

The A37 com.android.runtime APEX payload contains linker64 but no /bin/linker. The stage's /system/bin/linker points to /apex/com.android.runtime/bin/linker. This is insufficient even for a 64-bit-app-only policy: nine vendor/bin executables are ELF32 and request /system/bin/linker. Referenced native hardware services include audio, camera provider, OMX, Widevine, CAS and IVA. The graphics allocator/composer executables audited are ELF64. Full dependency/namespace closure remains to be audited; merely ignoring the missing 32-bit graphics check is not a fix.

Evidence files:
- logs/20261021-m2-inputs/abi-a376b77-audit.json
- logs/20261021-m2-inputs/vendor32-native77-audit.json
- logs/20261021-m2-inputs/runtime-candidates77.json

## Decisions and work required

User was asked whether M2 should retain A37's 64-bit-only application ABI (recommended for this donor milestone), or add Android application 32-bit compatibility now. User confirmed the 64-bit-only application ABI on22/10/2026. Native vendor ELF32 support is mandatory in either case; the question concerns application/ART support, not whether to discard hardware binaries.

For the 64-bit-app route, first prepare a reviewed native 32-bit runtime/linker/Bionic namespace layer while retaining Hybrid Vendor VNDK33 and hardware blobs, then align zygote and application ABI properties with the actual donor. Restrict system interface checks to the required framework architecture only after native vendor dependency closure is satisfied. Add a release gate to reject broken interpreter/zygote/ABI combinations.

The repository already contains a baseline com.android.runtime.apex plus bootstrap linker/Bionic in the embedded general_fixes payload. The extracted pinned S22 firmware also contains a runtime APEX. Neither candidate has been selected or copied to M2. APEX manifest version is 1 in all three packages and is not sufficient to establish platform compatibility. Audit actual ELF exports/dependencies, APEX keys/version constraints and linker namespaces before changing payloads; do not enable the complete old general_fixes overlay as a shortcut. Kernel remains pinned and unchanged.

Adding 32-bit application support additionally needs compatible ART/zygote32, framework native libraries and app ABI declarations; a native vendor runtime alone does not provide that support. This is a larger port scope.

## Current build state

No ABI gate has been bypassed. No complete M2 package exists. The diagnostic stage is incomplete and must not be published. After the ABI decision and native runtime adaptation, run a clean fork build, mandatory compiled ADB/Auto Blocker checks, kernel/vendor/radio audits and retained package publication. No M2 phone action is authorized.

## Native32 adapter prepared — patch0077

User confirmed app64-only and the five optional Settings omissions. The adapter is part of the existing boot-restore module; no new ROM build entrypoint or kernel change. It adds only S22 native32 linker/Bionic/runtime paths to an APEX rebuilt with the fork's existing ext4/AVB/signapk tools. All original A37 payload regular files and symlink targets compare byte-identically; uid/gid/mode/SELinux for original paths also match. This preserves A37's newer64-bit libdl export `__cfi_shadow_load@LIBC_PLATFORM`, which a wholesale S22 runtime swap would lose. The source64 export comparison is runtime64-exports77.json.

The resulting runtime APEX SHA256 is `a4f643533716a6ef7823b6758cde2bdad70c1249ae16c84e1dca2ed91eb49b7f`. AVB payload verification, APK v3 verification and4096-byte payload alignment pass. Two independent builds produced the same hash after normalizing lost+found inode timestamps. Separate locally generated payload/container signing keys are in ignored keys/m2-native32-apex; neither private key is committed. The adapter pins both public signing identities and fails before staging for wrong keys or unreviewed inputs. Native32 input/output and47 copied platform-library hashes are in a376b_native32.lock.json. The fork radio android.hardware.common-V2-ndk.so wrapper is retained byte-identically and separately hash-checked; it is not replaced with S22 bytes.

Static namespace candidate audit now includes164 ELF nodes,46 source32 dependency libraries plus the ld-android stub (47 copied files), with the original fork radio wrapper retained, and has no missing named dependencies or unresolved strong symbols under the audited candidate namespace model. It accounts for AOSP Bionic global/unversioned symbol matching. Actual namespace generation, dlopen paths and hardware execution still require device logs; this is not a runtime success claim.

The module sets zygote64 and explicit global app ABI arm64-v8a/empty32. Release-default verification now rechecks the pinned APEX, all47 copied native libraries and the retained radio wrapper, interpreter symlink, application ABI and zygote. Diagnostic finalstages77.txt confirms the native32 preparation/ABI gate and the previously failing graphics check pass; finalization and a full clean build remain pending.

Signing references: https://source.android.com/docs/core/ota/apex and the fork's existing external/android-tools/vendor/build/tools/releasetools/apex_utils.py. Payload construction follows https://android.googlesource.com/platform/system/apex/+/refs/heads/main/apexer/apexer.py using the already-built fork tools.
