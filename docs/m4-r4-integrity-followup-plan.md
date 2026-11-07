# R4 post-Setup: Integrity evidence and next repair

31 October 2026. The user confirms About phone displays `SM-A376B`; live
Setup Wizard is complete. Two actual tests now reproduce no Integrity labels.
This is a diagnosis/plan update; no ROM source patch or flash in this follow-up.

## Fresh tests completed

1. ADB launched the Play-installed checker and pressed CHECK at about20:00:11.
   Its Raw JSON dialog shows app recognition, licensing, Play Protect and
   recent activity `UNEVALUATED`; `deviceRecognitionVerdict` is absent and
   device attributes are empty. This is a fresh r4 observation, not reused
   pre-r3 JSON. It is the checker-displayed response; its backend has not been
   independently audited. Raw screenshot/nonce stays private.
2. Thirty PID samples over about15s capture GMS main and
   `com.google.android.gms.unstable` PID11280. The earlier snapshot lacking
   unstable was not proof the process gate is obsolete. Do not widen it.
3. Play Store About shows **Thiết bị chưa được chứng nhận**. Enabled only the
   Play Store developer menu by tapping its version and used its built-in
   Check integrity at20:03:14. Result: **Labels: []**. No sharing was invoked.
4. The built-in dialog reads fingerprint
   `google/bluejay_beta/bluejay:CANARY/ZP11.260918.007/16484274:user/release-keys`,
   brand `samsung`, device `star2lte`, model `SM-A376B`. This establishes an
   actual Play Store Build fingerprint readback under PIF, not just a property
   or an isolated helper probe. It does not establish equivalent readback in
   GMS unstable or identify which server criterion fails.
5. The same Play Store request logs:

```text
10-31 20:03:14.975 W keystore2: ... Failed to get rpc for sec level r#TRUSTED_ENVIRONMENT
10-31 20:03:14.975 W keystore2: ... Error::Km(r#HARDWARE_TYPE_UNAVAILABLE)
10-31 20:03:14.983 I keystore2: In store_new_key "integrity.api.key.alias", uid=10212, cert=true, cert_chain=true rebound=true
10-31 20:03:14.991 I Finsky: [294] Integrity key attestation record generated successfully.
10-31 20:03:15.862 I Finsky: [59] requestIntegrityToken() finished for com.android.vending.
```

RPC warnings persist but the legacy fallback completes in this request. Do not
report the warning alone as a fatal generation error. Do not treat record
generation as proof of Google trust or hardware certification.

Private evidence: `logs/20261031-m4-r4-integrity-followup/`. Shareable summaries
exclude nonce, test ID, account details and per-request key alias suffixes.
Google account data, Play Protect scanning settings and ROM partitions were
not changed. Play Store developer menu was enabled to run the documented test.

## Local bugs established

The real unstable GMS process logs:

```text
10-31 20:00:11.789 11280 11280 E SamsungPropsHooks: Failed to spoof Build.MODEL
10-31 20:00:11.789 11280 11280 E SamsungPropsHooks: java.lang.IllegalAccessException: Cannot set public static final field java.lang.String android.os.Build.MODEL of class java.lang.Class<android.os.Build>
10-31 20:00:11.789 11280 11280 E SamsungPropsHooks: Failed to spoof Build.DEVICE
10-31 20:00:11.790 11280 11280 E SamsungPropsHooks: Failed to spoof Build.PRODUCT
```

This is the separate SamsungPropsHooks helper, not the PIF helper whose writer
was fixed in r4. Its source has an explicit Google GMS/Play Store/GSF physical
S9 identity override. Blindly fixing its reflective setter could re-enable an
obsolete identity policy contrary to the user's A37 requirement. Audit scope
and call order before implementing a writer correction.

The hide-developer-status Settings page also crashes:

```text
10-31 19:51:22.630 E AndroidRuntime: java.lang.NoSuchFieldError: No field mMainSwitch of type Lcom/android/settings/widget/SettingsMainSwitchBar; in class Lcom/android/settings/SettingsActivity; or its superclasses
10-31 19:51:22.630 E AndroidRuntime: at io.mesalabs.unica.settings.spoof.HideDeveloperStatusFragment.onCreateView(HideDeveloperStatusFragment.java:123)
```

Neither exception is yet proven to cause the empty Google verdict.

## Next implementation sequence

1. Read compiled BZI6 Instrumentation call order, SamsungPropsHooks setters and
   process branches. In the existing M4-scoped adapter, resolve the obsolete
   Google physical-model override while preserving required hardware-specific
   Samsung app behavior, public A37 identity and the existing scoped PIF
   policy. Use the ART writer API already verified on this ART only where a
   real remaining write is required. Pin method preimages and reject drift.
   Source: `rom/unica/mods/settings/smali/system/framework/framework.jar/0002-Introduce-SamsungPropsHooks.patch`,
   the existing M4 Settings/framework staging adapter, and compiled framework.
2. Fix the hide-developer page against the actual donor switch API, without
   guessing another field or returning a dummy switch. Source fragment:
   `rom/unica/mods/settings/SecSettings.apk/smali_classes4/io/mesalabs/unica/settings/spoof/HideDeveloperStatusFragment.smali`;
   donor-specific adaptation in the existing scoped Settings adapter. Verify
   open/back/reopen, saved switch state, app list and no absent field reference.
3. Require compiled positive/negative identity checks: ordinary app and About
   phone remain A37, target/assert/radio/kernel remain G965F, and hook controls
   preserve their intended scope. Test PIF ON/OFF with fresh affected processes
   and restore the user's enabled state; do not clear account/app data.
4. Repeat both checker and built-in Play Store tests with correlated logs.
   If no local exception remains and labels are still empty, report a server
   certification/attestation limitation. Do not promise a further identical
   build or an unverified fingerprint will supply labels. Profile updater
   same-version repair is independently open but current seeded fields exist,
   so it is not an established cause of this failure.

The observed mixed fingerprint/brand/model is an audit input, not proof that a
particular fingerprint replacement would pass. Do not acquire keyboxes or fake
certificate chains to fabricate a result. No kernel/vendor/radio change is
supported by these logs.

Google's documented Android13+ DEVICE criteria include hardware-backed proof
of a locked bootloader and a certified manufacturer image. Public model text
and boot properties cannot establish those facts. An empty device verdict
does not identify which check failed. Strong is not a promised port result.
Sources: [verdict definitions](https://developer.android.com/google/play/integrity/verdicts),
[built-in Play Store test](https://developer.android.com/google/play/integrity/additional-tools).
