# M4 legacy AOD optional-hook runtime probe

Reason: the pinned legacy Infinity AOD implementation predates the BZI6
`disconnectPluginOnBackground()` interface method. The unadapted invocation
throws `AbstractMethodError` and terminates SystemUI. This probe verifies the
compiled VersionCheckingProxy bridge in an ordinary app_process child, without
constructing an actual AOD lifecycle or changing a service/component setting.

Files: `M4LegacyHookProbe.smali` and four synthetic plugin variants. They contain
no donor firmware. The variant classes deliberately model the old interface
mismatch; missing methods are intentional and must not be filled with dummy
implementations. Each variant belongs in its own DEX/JAR/class loader.

Use the fork's existing assembler:

```bash
java -cp rom/out/tools/bin/apktool.jar com.android.tools.smali.smali.Main assemble \
  support/diagnostics/m4-facewidget-legacy/M4LegacyHookProbe.smali -o classes.dex
zip m4-hook-probe.jar classes.dex
# Assemble variants/missing, concrete, throwing and other independently by the
# same command, packaging classes.dex as m4-hook-<variant>.jar.
```

After checking G965F/star2lte, candidate APK and every transfer hash, place the
five JARs plus the candidate SystemUI APK in `/data/local/tmp`, mode0444. Run:

```bash
adb -s <verified-G965F-serial> shell \
 'CLASSPATH=/data/local/tmp/m4-hook-probe.jar:/data/local/tmp/candidate-SystemUI-legacy.apk app_process /system/bin M4LegacyHookProbe'
```

The probe checks that the missing legacy hook is omitted, a concrete hook is
invoked once, an exception from a concrete hook propagates, and a missing hook
on another target still throws. All four cases invoke the existing onDestroy
method and require its side effect. The concrete exception is a test sentinel,
not an error to suppress. The outer main catches a failed assertion and exits1
so the negative control does not submit an uncaught process crash to Android.
Repeat against the baseline APK: it must fail on the missing-hook case. A
candidate that passes with a baseline that also passes is not a valid test.

Verification: all four compiled cases and the baseline negative control passed
on the selected G965F in the30 October run. The old manager/background/main
handler guard verifier also passed unchanged. Only VersionCheckingProxy has a
semantic DEX change; one unrelated const-string switches to jumbo encoding
because its string index moved. Actual AOD reconnect, sleep/wake, fresh boot
and lock-screen behavior remain separate runtime acceptance gates. Preserve
PluginInstance's ordinary onDestroy teardown and the signed AOD payload.
