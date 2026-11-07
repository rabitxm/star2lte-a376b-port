# M4 thermal sensor/policy/actuator audit — build0219

Date: 29 October 2026. This is the thermal provenance and controlled actuator
step after RTC patch0218 passed on M4-0219. The sensor/topology checks were
read-only. One short master-brightness cap test was applied and restored using
the framework API. It did not alter the kernel, policy, threshold, brightness
setting or any persistent device file. The user confirmed ordinary brightness
changes work; patch0216 remains unchanged.

## Device and capture

Captured on G965F serial `M2_G965F_SERIAL`; bootloader
`G965FXXUHFVG6`; target device `star2lte`; SDK37; `sys.boot_completed=1`;
SELinux `Enforcing`. The donor reports `ro.product.model=SM-A376B`, as expected
for this port, so the bootloader and target device properties identify the
physical S9+. The phone was not rebooted for this capture.

Raw sensor/topology captures are
`logs/20261029-m4-rtc-label-0219/thermal-audit/snapshot.txt` and
`logs/20261029-m4-rtc-label-0219/thermal-audit/topology.txt`. Follow-up runtime
files are in `logs/20261029-m4-thermal-0219/`. Commands used
`dumpsys thermalservice`, `dumpsys power`, read-only sysfs reads under
`/sys/class/thermal` and CPU cpufreq policy, and a read-only config filename
search under `/system/etc` and `/vendor/etc`.

## Runtime evidence

`dumpsys thermalservice` says Thermal HAL 2.0 is connected and reports live
temperatures: AP42.3°C, PA40.6°C, BAT39.0°C, USB37.0°C and SKIN38.5°C (raw
status value2). The HAL's current cooling-device list and static-threshold list
are empty. This confirms sensor values reach the framework while those two HAL
interfaces provide no entries. It does not prove that all thermal limiting is
absent.

The kernel exposes six thermal zones; zones0–3 explicitly report `mode=enabled`.
At this idle snapshot, BIG was
45°C, LITTLE46°C, G3D43°C, ISP43°C, AC40.8°C and battery38.7°C. Their read-only
trip configuration is:

| Kernel zone | Policy and trip temperatures | Bound cooling device(s) |
| --- | --- | --- |
| BIG | `power_allocator`; 20/60/83/90/100/105/110/115°C; trip2 is `passive`, trip7 `hot` | `thermal-cpufreq-1` at trip2 |
| LITTLE | `step_wise`; 20/76/81/86/91/96/101/115°C; trip7 `hot` | `thermal-cpufreq-0` linked at trips0–7 |
| G3D | `power_allocator`; 20/80/88/95/100/105/110/115°C; trip2 is `passive`, trip7 `hot` | `thermal-gpufreq-0` at trip2 |
| ISP | `step_wise`; 20/95/100/105/110/114°C; reported trip7 files return `Math argument out of domain of func` | `thermal-isp-0` linked at trips0–6 |
| AC | `step_wise`; no trip files reported | none reported |
| battery | `step_wise`; no trip files reported | none reported |

At capture time the four cooling-device current states were all0: GPU max5,
ISP max2, CPU0 max9, CPU1 max17. Their presence and bindings are evidence of a
kernel thermal-control path. They were not triggered in this sample. All
reported cdev weights were0, so the binding tables need source/kernel-semantic
review before describing when those devices activate. Do not equate these
BIG/LITTLE/G3D/ISP temperatures with Samsung AP/SKIN or infer that the kernel
trip table is the SDHMS SIOP policy.

The two CPU cpufreq policies are driven by `exynos_cpufreq` with `schedutil`;
reported policy maxima were1,794,000 and1,924,000 kHz. A maximum frequency by
itself does not show a thermal cap. Device properties included
`dev.ssrm.init=1`, `dev.ssrm.version=4.0`, `dev.ssrm.turbo=false`,
`sys.siop.level=3`, and `init.svc.sec-thermal-1-0=running`. `dumpsys power`
captured the screen asleep and showed no HDR brightness-limit lock; this is not
a controlled observation of the SDHMS master brightness limiter.

The built SDHMS APK contains the exact policy outputs already locked by the
M4-specific helper: `assets/siop_default.xml` SHA256
`94a74d1c278207fbd59eb595db12ad337ccf1a8b8a39797451745e16923347be` and
`assets/ssrm_default.xml` SHA256
`6784ecc18f0326c0599b9278fbd2865c56e3c4d58f2e6ca1f5250280ef78cb96`.
The first omits only the unsupported DEX special node; all remaining policy XML
is preserved by the pinned transform. The separate SSRM asset stays distinct.
The Android runtime capture did not yet prove which policy branches are active
or what actuator values they produce.

## SDHMS sensor path and actuator test

Decoded BZI6 SDHMS `a1/m` obtains temperatures from Samsung
`ISehThermal::sehGetTemperatures`; its callback multiplies each HAL Celsius
float by10, rounds to an integer and stores it by sensor name. The runtime
`SDHMS:y : SIOP::` log reports the same deci-degree format. This is separate
from Android Thermal HAL2.0, whose framework-facing temperatures are reported
in degrees Celsius. The loaded SIOP XML parser reads
`modelsettings/levels/@tempType`; the selected policy says `SKIN`, which is an
actual BZI6 `Temperature` enum member. This confirms parser and sensor names
are compatible at the type level. It does not establish that a particular
SIOP scenario is active. A read-only query of the live SDHMS `settings.db`
returned `thermalControlFlag=0`. BZI6 `k5/w4` maps bit `0x2` to the
`BrightnessControlForTG` boolean and registers/removes that named scenario via
`j5/m4`; so the optional TG brightness scenario was disabled at capture time. This is
not evidence that base SIOP or kernel thermal cooling is disabled. The locked
policy's base `DISPLAY/Brightness` entry is at SKIN380 (38.0°C), while live
samples in this initial audit were SKIN357–362 (35.7–36.2°C). The configured TG
branch has its own temperature/value list, but it was not active with the
captured flag. Initial logs did not show an SDHMS call to the master-brightness
API; a later passive capture did record calls, described below. `dumpsys sdhms`
returns the exact error
`Error with service 'sdhms' while dumping: FAILED_TRANSACTION`, although the
service is present and process `com.sec.android.sdhms` is running. The `LV`
`curLevel` broadcast/property is emitted through a distinct
`OverheatSensorChecker` path, so it cannot by itself prove which SIOP
`tempType` or limiter branch was selected.

The framework API contract is verified in the built source:
`setMasterBrightnessLimit(lowerLimit, upperLimit, period)`; the Binder proxy
writes those three integers in that order. PowerManagerService enforces
`android.permission.CONTROL_DISPLAY_BRIGHTNESS`, and the installed SDHMS package
has that permission granted under shared `android.uid.system`. Before the test,
`dumpsys power` showed no Samsung `BrightnessLimitRunner`, while generic display
thermal clamp remained `cap=1.0, applied=false`.

At 15:47, with manual brightness `63/255` and the display awake, the controlled
call `(-1, 32, 0)` produced runner `lower=-1, upper=32, target/current=32`.
The panel setpoint changed from6224 to3200 and `actual_brightness` from23 to7.
The reset call `(-1, -1, 0)` restored `lower=-1, upper=-1`,
`mIsRunning=false`, `target/current=867`; panel readback returned exactly to
6224/23. The reset value -1 is translated by the controller to its unbounded
maximum, so the original no-cap behavior was restored effectively. The
original state had no runner entry; an idle no-cap runner remains in memory
until the power service/device restarts, with no persistent setting changed.
The harness initially expected255 instead of the observed867 maximum and sent
three identical reset requests before recognizing the correct readback. All
three were unbounded reset requests; final state and panel readback passed. The
test returned the screen to asleep and left `screen_brightness=63` and mode0
unchanged. Exact output and post-restore capture are
`logs/20261029-m4-thermal-0219/actuator-probe.txt`,
`actuator-probe-review.txt`, and `actuator-restored-readback.txt`.

## Passive runtime observation after reconnect

The user reconnected the same G965F while it remained in Android. Read-only
sampling from16:21 to16:28 recorded `PowerManagerService` calls from uid1000,
pid3960, which `/proc/3960/cmdline` identified as
`com.sec.android.sdhms`. The API calls used `lower=-1`, `period=200`, and
upper values306,408,510, or867. Entries typically appear about six seconds
after an `SDHMS:y : SIOP::` temperature line. Examples include SKIN398 then
upper306 at16:21:51–16:21:57, SKIN387 then upper867 at16:22:11–16:22:17, and
SKIN415 then upper867 at16:28:22–16:28:30. Other adjacent samples do not form a
simple monotonic temperature-to-upper mapping. At16:28:50 the live
PowerManager runner reported `mUpperLimit=510`, `mTargetLimit=510`,
`mCurrentLimit=510`, and `mIsRunning=false`. These observations show SDHMS
exercising the framework limiter API during runtime. The log contains no Java
call stack; it does not identify which SDHMS call site supplied these values.
The effective overlay correction below retains both caller families. The optional
`BrightnessControlForTG` flag was read as0 at16:23; `pd_activate_status`
read as null at16:27. Neither read resolves the source of every API call.

### Correction: effective resources retain both SDHMS caller families

The earlier version of this audit incorrectly ruled out `k5/s1` and attributed
the captured non-255 values to `j5/a1`. That inference used the standalone
`framework-res.apk` table, whose brightness maxima are255 and HBM rows are all
`-1`, without resolving active resource overlays. It is retracted. The earlier
description of the other `k5/s1.f` caller as "reset-only" is also corrected:
`k5/p1` supplies `true`, causing `f(IZ)` to return without an API request.

Read-only `cmd overlay lookup --user 0 --verbose com.sec.android.sdhms ...`
now resolves the screen extended maximum to867 and the HBM table below from
`/product/overlay/framework-res__a37xnaxx__auto_generated_rro_product.apk`.
The cover extended maximum remains255 and the PD formula array is empty.
The same values resolve in the `android` context. Saved command output,
configuration and resource provenance are in
`logs/20261029-m4-validation-0219/run-1655/overlay-effective.json`.

| Row | `config_High_Brightness_Mode_Plus_Value` columns0–5 |
| --- | --- |
|0|`-1,-1,-1,-1,510,408`|
|1|`-1,-1,-1,510,408,306`|
|2|`-1,-1,510,408,306,306`|
|3|`-1,510,408,306,306,306`|

Static BZI6 source contains direct framework call sites in `j5/a1`, its owned
deferred runnable `j5/z0`, and `k5/s1`. `j5/o3` registers `j5/a1` as component
`Brightness`; `thermal/siop/scenario/HighBrightness` can feed it through
`j5/o3.i0`. In the inspected framework code, `PowerManager` passes the three
arguments unchanged to Binder, and `PowerManagerService` logs the raw upper
argument before passing it to `BrightnessLimitController`. There is no numeric
conversion before that log in these inspected methods.

`k5/r1` samples SKIN in deci-degrees and retains three samples. In the
inspected code `n5/b0.c()` returns the sentinel `-999`, so its row input is
`newest SKIN - oldest SKIN`: row0 for delta<=2, row1 for3–5, row2 for6–9,
row3 for>=10. Its column is `max(trunc((SKIN - 350) / 10), 0)`. It reads the
cell only if column<6, the table exists and `s1.n` is false. Otherwise it
passes `-1`; `s1.f(value,false)` replaces nonpositive/out-of-range input with
its maximum, which the effective resources make867. With an empty PD formula,
the optional conversion leaves the value unchanged. Thus SKIN398 selects
column4 and permits510/408/306 depending on row; SKIN404 selects column5 and
permits408/306; SKIN415 selects column6 and follows the867 fallback. These
outputs are compatible with the saved API calls. The adjacent SIOP log lines
do not expose the callback's exact sample buffer or prove it made those calls.

The table callback also requires `s1.d=true`, the base `Brightness` component's
`d()` result to be `-1`, `s1.m=true`, and three buffered samples. Non-`-1`
effective table entries allow the `d` registration gate: `n5/f1.s` records
their minimum and the constructor enables the path when that minimum differs
from `Integer.MAX_VALUE`. An actually all-`-1` table with an untouched static
minimum would disable registration, rather than demonstrate a callback sending
255. `m` comes from the screen-interactive event (`a3/g`, `a3/f`, `k5/q1`);
screen-off clears the sample buffer. `n` is updated by the separate
`HighBrightnessForTG` event and true selects the fallback. The earlier
`BrightnessControlForTG` bit snapshot does not establish these gates or show
that all HBM/SIOP limiting is disabled. The column>=6 fallback is static
behavior; its intended interaction with base SIOP remains unverified and does
not itself justify a threshold or kernel change.

Artifact correspondence: the runtime SDHMS APK SHA256 in
`logs/20261029-m4-validation-0219/run-1655/runtime-apk-hashes.json` is
`3e8b54c5520e87b1b5cb7f8f8c6707db1d6bec8ced4608399ba08a97de1bfd84`, identical
to the built APK at
`rom/out/target/star2lte/work_dir/system/system/priv-app/SamsungDeviceHealthManagerService/SamsungDeviceHealthManagerService.apk`.
Its `classes.dex` SHA256 is
`b50ff2419843feedc026ba33c53f27e5177024fa7aefd61b2530680d1918f767`.
The original BZI6 APK at
`rom/out/fw/SM-A376B_XXV_A376BXXU5BZI6/system/system/priv-app/SamsungDeviceHealthManagerService/SamsungDeviceHealthManagerService.apk`
has SHA256 `53c76a214d5e325ed7c16e368392ad37e9de52b68568777a0b6fa29e554fe3f9`,
matching the DVFS lock. Current APKTool Smali for `k5/s1`, `k5/r1`, `k5/p1`,
`n5/f1`, `j5/a1`, `j5/z0`, `j5/o3`, and `HighBrightness` is byte-identical to
the inspected decoded donor tree under
`logs/20261028-m4-postboot-repair/sdhms-bzi6-decoded/smali/`. This ties the
runtime artifact to the built APK and records source correspondence.

A subsequent narrow compiled check independently disassembled13 selected
classes from both that exact built APK and the pinned original donor with the
fork-bundled baksmali. All194 method bodies match after the same output options
and comment/whitespace normalization; instructions, operands, registers,
labels, signatures and directives were retained. This includes `k5/s1`,
`k5/r1`, `k5/p1`, `k5/q1`, `k5/o1`, `n5/f1`, `n5/b0`, `j5/a1`, `j5/z0`,
`j5/o3`, `a3/f`, `a3/g`, and `HighBrightness`. The only selected class-level
encoding difference is `n5/f1`'s explicit null/false static-field defaults in
the donor versus omitted default values in the rebuilt DEX; descriptors and
access flags are unchanged and the VM defaults are equivalent. The whole DEX
hashes differ, so no whole-DEX identity is claimed. This closes the compiled
correspondence gap for these inspected paths; it does not identify their live
gates or API caller. Tool hash, options, per-class method counts/hashes and
result are saved in
`logs/20261029-m4-validation-0219/run-1655/thermal-compiled-check.txt`.
Temporary disassembly files were removed. No Java stack attributes the live
calls to either family.

Kernel sysfs also showed a natural state change. At16:28:02 BIG was79000
(79.0°C), LITTLE75000, and `thermal-cpufreq-1` was state8/17. BIG's cdev0 link
points to that device at trip2, configured at83000 with type `passive`; the
sample does not show when it entered state8. At16:28:50 BIG was46000 and that
cooling device was back at state0; the other three CPU/GPU/ISP cdevs were also
state0. This proves that a kernel cooling-device state was observed active and
later inactive, but it does not establish the activation or release threshold,
cause, or hysteresis. At the latter sample, Android Thermal HAL still reported
temperatures but its cooling-device and static-threshold lists were empty.

No external temperature instrument was available. No deliberate load, heating,
setting change, reboot, or write was performed during this observation. The
data is passive evidence, not a calibrated threshold test. The exact SDHMS
policy branch remains unresolved; do not change the SIOP table or kernel from
this correlation alone. Filtered evidence and readbacks are in
`logs/20261029-m4-thermal-0219/thermal-natural-passive-observation.txt`.

## Finding and next gated action

Current evidence separates two layers:

1. Linux thermal zones and four CPU/GPU/ISP cooling devices exist and are
   enabled/linked. A later passive capture observed `thermal-cpufreq-1` at8/17,
   followed by state0 as BIG temperature fell; crossing/release thresholds were
   not captured.
2. Android's Thermal HAL supplies temperatures but returns no cooling devices
   or static thresholds. Samsung SDHMS/SSRM is running. SDHMS made automatic
   master-brightness API calls during the later capture. Source tracing found
   two SDHMS caller families (`j5/a1` including its helper, and `k5/s1`), with
   `HighBrightness` feeding the former. Corrected overlay-aware tracing shows
   that `k5/s1` can produce every recorded upper value; the earlier elimination
   based on the base resource table is retracted. No Java stack identifies the
   caller or selected scenario. The TG feature bit was0 at the last database
   read, not proven current for every later call or for the other gates.
3. The master-brightness API cap and effective reset were directly verified in
   PowerManager and panel readbacks; no persistent display setting changed.

The Samsung master-brightness API actuator subgate **passes**: cap and
effective no-cap restoration were observed in controller and panel readbacks.
Source tracing resolved the `BrightnessControlForTG` activation condition to
`thermalControlFlag & 0x2`; the captured value was0, and `curLevel` logs come
from the separate OverheatSensorChecker path. Passive automatic API calls and a
kernel cooling-device state transition are now **observed**, while their exact
scenario, activation boundary, release boundary, and hysteresis remain
**unverified**. The thermal gate and M4 are not accepted. The evidence does
not justify changing thresholds, policy, or kernel. Since no independent
temperature instrument is available, do not force heat or start a workload.
Compiled-method correspondence for the selected thermal paths now passes;
trace the live gates/inputs of both caller families and record only natural
transitions until a calibrated, bounded test can be run. Keep the DeX-specific
thermal branch marked unsupported; do not claim DeX thermal protection.

### Files and verification

Reason: prove the M4 thermal API actuator and record passive automatic-control
evidence and source-level caller candidates without overstating threshold
validation; correct the earlier overlay omission and transcription error.
Files affected: this report; `docs/PLAN.md`;
`docs/m4-post-0217-repair-plan.md`; and filtered readback/log files under
`logs/20261029-m4-thermal-0219/`; and the compact compiled check at
`logs/20261029-m4-validation-0219/run-1655/thermal-compiled-check.txt`.
Static code inspected was the decoded BZI6
SDHMS tree, base resource table, effective runtime overlay lookups, matching
built/runtime APK hashes and independent selected-class disassembly. All194
selected compiled method bodies match the pinned donor under documented
normalization. The filtered excerpt's16:22:52 `WIFI` value was
corrected from370 to401 against the original saved logcat; other recorded
events are preserved. No ROM source or image changed. The temporary
SQLite copies used to read the single thermal flag were deleted after query;
no raw settings database is retained. Verification matched the readback to the
selected G965F/bootloader, checked framework and panel values before/during/after,
read the SDHMS permission grant and built APK policy hashes, and confirmed the
display returned to asleep in the saved readback. After the user's reconnect,
`adb devices -l` showed the selected G965F as `device`; a later read-only
snapshot confirmed SDK37, `boot_completed=1`, the expected bootloader and
Enforcing. No reboot or device write was attempted.
