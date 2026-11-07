# M4 r3: About phone model and Play Integrity after Setup Wizard

31 October 2026. Scope: G965F `M2_G965F_SERIAL`, Android17 BZI6, r3.
User finished Setup Wizard, reports no Play Integrity device labels, and sees
`SM-G965F` in About phone. ADB confirms `user_setup_complete=1`,
`ro.product.model=SM-A376B`, `ro.boot.em.model=SM-G965F` and
`settings global device_name=Galaxy A37 5G`.

## Findings

* Build r3 log says `Replacing value "ro.product.model" ... with
  "ro.boot.em.model"` in `ModelNameGetter.getModelName()`. Decoded r3 code
  confirms that Settings displays the bootloader's physical model. This is the
  fork's existing deliberate behavior, not evidence the public product property
  reverted.
* During the user's check r3 emitted `E PlayIntegrityHooks: Failed to set prop
  FINGERPRINT` and `java.lang.IllegalAccessException: ART static final Build
  field write failed`. A separate probe in an isolated `app_process` showed
  `java.lang.NoSuchMethodError: No virtual method staticFieldBase(...) in class
  Lsun/misc/Unsafe;`. The wrapper had discarded the original exception.
  The separate fork helper `SamsungPropsHooks` also emitted
  `IllegalAccessException: Cannot set public static final field java.lang.String
  android.os.Build.MODEL` (and DEVICE/PRODUCT) in other app processes. Its
  behavior is an additional open issue; the PIF helper change does not repair it.
* Android's installed ART has `jdk.internal.misc.Unsafe.staticFieldBase` and
  `staticFieldOffset`. Isolated probes using that implementation changed and
  read back a test string, `Build.FINGERPRINT`, and the integer
  `Build.VERSION.DEVICE_INITIAL_SDK_INT` inside their own short-lived process.
  This does not establish that the actual GMS hook or Google verdict works.
* The same runtime captured `keystore2: Failed to get rpc for sec level
  r#TRUSTED_ENVIRONMENT`, `Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)`, and
  `KeyAttestationHandler: java.security.ProviderException: Failed to generate
  key pair.` `remote_provisioning.tee.rkp_only` is still `1`.
* An initial root `app_process` keystore probe could not reach the service:
  `KeyStoreException: ... Transaction failed on small parcel`, with nearby
  `avc: denied { find } ... keystore_service ... scontext=u:r:su:s0`. It was not
  a valid application test. A temporary, independently signed diagnostic app
  was then installed under its own UID and removed after testing. Its temporary
  EC key without a challenge passed key generation and sign/verify both before
  and during the property test. With `rkp_only=1`, a challenge key failed with
  `Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)` (PID24768). With the property set to
  `0`, generation/sign/verify passed, the 3-certificate chain's signatures
  verified, the supplied challenge bytes appeared in the attestation extension,
  and the temporary alias was deleted (PID24819). Restoring `1` reproduced the
  original failure (PID24888). The property was restored to `1` after the test.
  Chain signatures and challenge bytes alone do not establish root trust or a
  Google Integrity verdict.
* A preliminary probe was assembled as DEX version 041 with an old header;
  Android rejected it with `Header size is 112 but 120 was expected`, and that
  standalone `app_process` aborted with `ClassNotFoundException: PifUnsafeProbe`.
  Reassembling as API28 produced the valid probe above. SystemServer stayed up.

Sanitized log lines and probe sources/results are in
`logs/20261031-m4-r3-post-setup/`; raw token or account data is not recorded here.

## Source changes and checks

| File | Reason | Verification |
| --- | --- | --- |
| `rom/unica/mods/settings/customize.sh` | Do not replace donor `ro.product.model` with physical `ro.boot.em.model` for M4; existing behavior remains for other donors. | Test source with M4 scope; compiled and actual About phone checks required. |
| `rom/unica/mods/settings/a376b_android17_boot_limits.py` | Pin the donor model getter preimage and reject a compiled M4 Settings getter that reads the boot model. | Positive donor fixture passed; old r3 physical-model fixture rejected; compiled APK gate pending. |
| `rom/unica/mods/pif/a376b_android17_pif.py` | Use the ART API actually present and preserve the underlying exception as the cause. | String/int process probes passed; transformed hook assembled and disassembled with expected calls. Actual GMS process readback, OFF/allowlist controls and fresh server request remain required. |
| `rom/unica/mods/settings/a376b_android17_boot_limits.py`, `rom/scripts/internal/verify_exynos9810_release_defaults.py`, `rom/scripts/internal/verify_m4_runtime_image.py` | On the scoped M4/G965F profile set the sole product `remote_provisioning.tee.rkp_only` to `0`, allowing the tested factory-Keymaster attestation path; reject duplicate property sources. | App-context 1→0→1 controlled comparison above; fixture tests cover idempotence and reject competing/unreviewed property values. Work/image gates and final boot still required. |

The first clean r4 build was deliberately stopped before packaging to incorporate
the validated RKP fallback. The replacement build uses the existing fork/profile
and pinned diagnostic kernel; r3 full release remains available during build.
No EFS, modem, radio or kernel
change is part of these patches. Do not mark M4 or Integrity passed until runtime
checks have been completed. Google states Android 13+ device-integrity verdicts
require hardware-backed proof of a locked bootloader and certified manufacturer
OS; a custom ROM's hook cannot by itself establish that property.

References: [Google Play Integrity verdicts](https://developer.android.com/google/play/integrity/verdicts).
