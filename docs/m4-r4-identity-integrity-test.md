# M4 r4: model, PIF writer and legacy attestation fallback

31 October 2026; G965F `M2_G965F_SERIAL`, bootloader `G965FXXUHFVG6`.
R4 was built, fresh-flashed and booted to Android17 on the test device.
Setup Wizard is now complete (`user_setup_complete=1`, checked after the user
report). The user visually confirms About phone displays `SM-A376B`, but
reports no Play Integrity labels; fresh checker and built-in Play Store tests
now reproduce that result. This is not final M4 acceptance or a Google
Play Integrity PASS.

## Changes and checks

Detailed reasons, affected files and isolated tests are recorded in
[r3 post-setup diagnosis](m4-r3-post-setup-diagnosis.md). The changes retain
the donor Settings model getter on M4, replace the PIF helper's unavailable
`sun.misc.Unsafe` API with the tested ART `jdk.internal.misc.Unsafe` methods,
and set `remote_provisioning.tee.rkp_only=0` for the scoped legacy G965F
Keymaster path. The fallback had passed an app-UID 1→0→1 controlled challenge
test before build; the property on the previous boot was restored afterwards.

The signed compiled SecSettings getter reads `ro.product.model`, not
`ro.boot.em.model`. The signed compiled `framework.jar` contains the intended
Unsafe calls and preserves the underlying exception. A probe calling the
installed PIF helper in its own `app_process` successfully wrote/read back
`Build.FINGERPRINT` and `Build.VERSION.DEVICE_INITIAL_SDK_INT`. This does not
substitute for a GMS process request, OFF/allowlist controls, or Google's verdict.

The work-tree, compiled boot contract, PIF, ADB-default, SELinux and ext4
image gates passed. NFC permission/library/HAL compatibility checks passed;
Bixby packages/declarations remain. No new NFC RF or Bixby voice port was
attempted. No kernel was built; this build uses the already selected G965F
DS-ACK WIPEONFORK diagnostic package, not a G965N kernel release.

## Artifact and flash

Release run: `rom/out/releases/M4/star2lte-1793437348-S45zbY/`.
Full ZIP SHA256:
`237aa4167e34bc71c42c2663920b47df84a66a710b30a31d0cdf2469657e3753`.
Target-files SHA256:
`c4d410a7dd50e9ac143123a85435cb7d98b1733344071737ca1fc7b89e73c05f`.
Both SHA256 checks passed; all eight ZIP32 stage hashes, signatures, CRCs and
manifest passed. The main installer scripts matched r3 byte-for-byte. Its EFS
lookup mounts read-only with `ro,noload` if needed.

All eight stages completed from recovery `/tmp` with independent transfer
hashes, verified markers, and install exit 0; stage8 formatted USERDATA, CACHE,
OMR and PRELOAD. Before/after hashes of EFS, CPEFS, RADIO, RECOVERY and BOTA0–2
matched. GPT snapshots were saved; no protected partition was written. The
initial GPT capture accidentally included `dd` stderr in the binary and failed
the length check; it was immediately recaptured with remote stderr suppressed,
correct lengths and before any flash. Read-only e2fsck passed SYSTEM, VENDOR,
ODM, USERDATA, CACHE, OMR and HIDDEN. A read-only SYSTEM mount after flash
confirmed the Settings APK and framework JAR match the built files and the
installed product property is `rkp_only=0`.

## Initial boot and remaining tests

Android reports `sys.boot_completed=1`, `bootanim=stopped`, SELinux Enforcing,
`ro.product.model=SM-A376B`, `ro.product.device=star2lte`, physical
`ro.boot.em.model=SM-G965F`, `remote_provisioning.tee.rkp_only=0`, and PIF
enabled. ADB defaults are enabled and keyless; Rampart/Auto Blocker main and
strict defaults are `0`. Three samples over 40 seconds kept SystemServer PID
2223 and SystemUI PID 6287 with `boot_completed=1`. The later follow-up confirms
Setup Wizard is complete and the user visually confirms the model. Integrity
remains failing by the user report; details follow below.

The known user-accepted Bixby receiver crash remains:
`10-31 17:15:21.628 ... IllegalStateException: No matching ComponentResult found!`.
The separate SamsungPropsHooks helper also still logs `IllegalAccessException`
for `Build.MODEL`/DEVICE/PRODUCT in some processes. No claim is made that these
known limitations are repaired by the PIF writer change.

Evidence is local under `logs/20261031-m4-r3-post-setup/`. Full r3 is retained
temporarily as a rollback while the new runtime tests are pending; older
partition images and GPT rollback evidence remain under
`logs/20261030-m4-g965f-test-0229/`. Follow latest-only retention after r4 is
accepted for use; never restore EFS/CPEFS/RADIO/RECOVERY/BOTA.

## Post-Setup follow-up, 31 October, 19:56–19:59

The user confirms the physical About phone UI now displays `SM-A376B`; close
the public model display bug for r4. Live checks show Setup Wizard complete,
`sys.boot_completed=1`, `rkp_only=0`, PIF enabled and the expected seeded profile
version `20261011`. The checker is installed by `com.android.vending`.
The user reports no Integrity labels; a fresh decoded verdict has not yet been
captured during this follow-up. Do not reuse the pre-r3 JSON as the r4 result.

Actual GMS logs now include:

```text
10-31 19:51:35.259 5151 5318 W KeyAttestationHandler: Certificate chain has 3 numbers of certificates [CONTEXT service_id=130 ]
10-31 19:55:18.516 5151 8556 W KeyAttestationHandler: Certificate chain has 3 numbers of certificates [CONTEXT service_id=130 ]
10-31 19:55:22.867 5151 8558 W KeyAttestationHandler: Certificate chain has payload byte size 2030 [CONTEXT service_id=130 ]
```

PID5151 maps to `com.google.android.gms`. These lines establish that the
observed GMS calls obtained a chain, unlike the earlier key-generation failure.
They do not establish its Google trust status or that these timestamps map to
a particular checker request. The PIF-only history contains no old field-write failure; buffers are finite
and absence is not proof the hook ran successfully. The expanded keystore
tag capture DOES contain RPC warnings, followed by successful fallback:

```text
10-31 19:55:18.474 W keystore2: Failed to get rkpd key: ... Trying to get IRPC name.
10-31 19:55:18.474 W keystore2: ... Failed to get rpc for sec level r#TRUSTED_ENVIRONMENT
10-31 19:55:18.474 W keystore2: ... Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)
10-31 19:55:18.487 I keystore2: In store_new_key "checkin_attestation", uid=10253, cert=true, cert_chain=true rebound=false
10-31 19:55:18.516 W KeyAttestationHandler: Certificate chain has 3 numbers of certificates [CONTEXT service_id=130 ]
```

The alias identifies GMS check-in attestation, not a proven checker request.
Do not call the warning a fatal key-generation failure in this sequence.

The source PIF `dlog` method is a no-op. The GMS build rewrite also requires a
process name containing `unstable`. At the snapshot only GMS main and `:car`
were present. This raises a process-scope question; it does not prove a
regression or justify broadening the allowlist. Correlate a fresh request with
process lifetime before any change.

An independent Settings crash is newly observed:

```text
10-31 19:51:22.630 E AndroidRuntime: Process: com.android.settings, PID: 3556
10-31 19:51:22.630 E AndroidRuntime: java.lang.NoSuchFieldError: No field mMainSwitch of type Lcom/android/settings/widget/SettingsMainSwitchBar; in class Lcom/android/settings/SettingsActivity; or its superclasses
10-31 19:51:22.630 E AndroidRuntime: at io.mesalabs.unica.settings.spoof.HideDeveloperStatusFragment.onCreateView(HideDeveloperStatusFragment.java:123)
```

The fork fragment references an absent donor field. No source change or flash
was made in this follow-up. Plan and limits are in
[r4 follow-up plan](m4-r4-integrity-followup-plan.md). Private tagged evidence:
`logs/20261031-m4-r4-integrity-followup/`. UIAutomator still returns
`ERROR: null root node returned by UiTestAutomationBridge`; visual model
confirmation is from the user, not a successful UIAutomator dump.

## Fresh requests completed at20:00 and20:03

The subsequent ADB-controlled checker request reproduces three X marks; Raw
JSON shows no `deviceRecognitionVerdict` and app/license/Play Protect
`UNEVALUATED`. Thirty process samples catch GMS unstable PID11280, so the
earlier no-unstable snapshot is not a process-scope regression.

The official built-in Play Store test returns `Labels: []`; About reports the
device uncertified. Its result dialog reads the actual PIF fingerprint while
retaining brand Samsung/modelA37/device star2lte. During that request Finsky
logs `20:03:14.991 ... Integrity key attestation record generated successfully.`
and `20:03:15.862 ... requestIntegrityToken() finished for com.android.vending.`
The preceding missing-RPC warning is followed by successful legacy creation.
SamsungPropsHooks exceptions are captured in GMS unstable at20:00:11; the
separate PIF writer does not emit its previous failure in these captures.
See the [updated detailed plan](m4-r4-integrity-followup-plan.md) for exact logs,
source targets and verification. Earlier paragraphs describing a pending
fresh JSON capture refer to the start of this follow-up and are superseded
by this section. No new source repair, build or flash has been performed.
