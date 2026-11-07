# M4 post-boot failures after source0198 — 28 October 2026

The designated G965F reaches SDK37/OneUI90000, sys.boot_completed=1 and
bootanim=stopped with SELinux Enforcing. This is evidence that the earlier
SystemServer/libmeminfo boot blocker is passed. It is not UI/hardware
acceptance: SystemUI and two hardware-facing apps repeatedly crash.

Evidence is private under logs/20261028-m4-meminfo-test. The real SystemServer
PID1830 remains present in all27 boot-complete watcher observations and in
the late337.22s uptime check. Some pidof observations include additional
short-lived matching names; they are not classified solely from pidof.
No SystemServer native abort is recorded in this trial after0198. Window
Manager focuses SecSetupWizard's IntroLanguageSpinnerActivity, but later
mCurrentFocus is null during SystemUI restarts; physical UI visibility has
not been confirmed by the user.

## 1. SystemUI / FaceWidget lifecycle dispatch

systemui-first-crash.txt records:

```
java.lang.reflect.UndeclaredThrowableException
at $Proxy9.setPluginFaceWidgetCallback
at com.android.systemui.facewidget.plugin.PluginFaceWidgetManager$6.handleMessage
Caused by: java.lang.IllegalStateException: Method setCurrentState must be called on the main thread
at androidx.lifecycle.LifecycleRegistry.e
at com.samsung.android.app.facewidget.common.FaceWidgetPluginLifeCycle.<init>
```

The actual BZI6 decoded caller is present at
logs/20261026-m4-product-display/decoded/SystemUI/smali_classes2/com/android/systemui/facewidget/plugin/PluginFaceWidgetManager$6.smali.
The fork's s9_infinity_Wallpaper/customize.sh installs its bundled
AODService_v80 and discards the prior decode cache. That composition must be
checked against BZI6's callback threading contract; the stack proves the
lifecycle constructor is reached off the main thread, not which whole APK
should be replaced. Next: inspect both compiled caller/plugin paths and port
the lifecycle/callback handoff to the correct Looper while preserving the
Infinity/AOD behavior. Do not simply remove LifecycleRegistry's assertion,
catch/ignore the crash or disable SystemUI/AOD to call the gate passed.

## 2. SDHMS / actual thermal-policy schema

sdhms-first-crash.txt records:

```
java.lang.IllegalArgumentException: No enum constant com.sec.android.sdhms.thermal.overheatcontrol.overheatcomplex.OverheatComplexType.DEX
at com.sec.android.sdhms.settings.SiopExpression.M
```

The existing fork's dvfs/assets/siop_default.xml contains an overheatcontrol
complex entry `<DEX formula="SKIN" temp="470" />`. BZI6's SDHMS rejects the
DEX enum. Next: decode the actual BZI6 enum and policy parser, compare donor
and target policies, then adapt their schema with the actual S9 sensor and
thermal limits. Preserve thermal control rather than suppressing exceptions,
disabling SDHMS, or pretending a missing control was applied.

## 3. NFC / target native dependency closure

fast-018-crash.txt:5 and later samples record:

```
java.lang.UnsatisfiedLinkError: dlopen failed: library "libnfc-nci.so" not found: needed by /system/lib64/libnfc_nci_jni.so in namespace clns-shared-9
```

The M4 branch in unica/patches/nfc/customize.sh stages the hash-pinned G965F
PHN JNI. DT_NEEDED on that JNI names libnfc-nci.so, which exists in the same
G965F firmware cache but is not staged by this branch. Its transitive
DT_NEEDED set includes legacy HIDL/ICU/chrome/metrics dependencies; copying
only that one name without resolving symbols and namespace visibility is not
an accepted repair. Next: audit the full target JNI/provider closure against
the actual installed64-bit libraries, preserve the variant-correct HAL,
then test loading, JNI registration and actual service/HAL initialization.
A missing library is not evidence of modem/kernel/radio failure.

## Remaining observations and sequence

The crash buffer snapshot contains126 NFC,54 SDHMS and5 SystemUI process
crashes; these are events inside that snapshot, not total device counts.
One Bixby receiver failure reports `No matching ComponentResult found!`;
its dependencies/relationship to setup and the other failures remain to be
verified. Prioritize SystemUI and SDHMS, then NFC's native closure and Bixby.
Change one component per trial; keep the measured197 networking/198 meminfo
boundary and existing diagnostic kernel fixed. Capture before/after logs,
retain exact rollback files/data snapshots, then run a fresh-data/reboot and
hardware matrix after the composition is stable. No claim of G965N parity,
Play Integrity verdict, live traffic accounting or complete hardware health
is made by this diagnostic.


Detailed next implementation plan: [m4-post-boot-repair-plan.md](m4-post-boot-repair-plan.md). The plan additionally audits the actual plugin4006/SystemUI4022 contract, rejects the stale SDHMS decode and mutable NFC work-tree as sources of truth, and puts minimal thermal runtime acceptance before a long UI session. This is a planned sequence, not a claim that these fixes have been built or flashed.
