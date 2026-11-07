# M4 brightness scale repair 0216

Date: 28 October 2026. Scope: G965F `star2lte`, donor A376B BZI6 SDK37, existing
legacy SehLights path. This is a focused framework-source change; M4 package
0215 remains the current recovery candidate until the new build and runtime gate
complete.

## Cause and change

On fresh package0215, Settings brightness60/180 had produced raw panel values
59/180. The installed `services.jar` matches the staged jar
(`fb1a8088…459e`). Its `PowerManagerUtil` selects auto-brightness type3, so the
legacy adapter used factor1. The running kernel config has
`CONFIG_PANEL_BACKLIGHT_PAC_3_0=y`; PAC3 defines scale100. The S6E3HA8 table
starts at raw0/700/1400, so raw59,180,230 all resolve to luminance3. The pinned
lights HAL passes the nonnegative extension value through to the brightness
sysfs writer without multiplying it.

Patch0216 changes the existing BZI6 backlight adapter so its normalized main
backlight value is multiplied by255 and100. It leaves the global auto
brightness type/algorithm, other light types, SurfaceControl path, color/off
state, extended brightness cache, mute/unmute behavior, lights HAL, vendor,
radio, and kernel untouched. The change is scoped by the existing BZI6 helper
checks for M4 source config, star2lte G965F, SDK37, and firmware cache suffix.

## Files

- `rom/unica/patches/legacy/lights/services.jar/0003-BZI6-legacy-SehLights-backlight.patch` — fixed PAC3 factor at100 for this path.
- `rom/unica/patches/legacy/lights/patch_bzi6_sehlights.py` — pins the revised nested patch SHA256.
- `patches/fork/0216-fix-m4-bzi6-pac3-brightness-scale.patch`, `patches/fork/series`, and `sources.lock.json` — reproducible fork patch and lock.

## Verification and build status

Passed: reconstructed the exact original decoded adapter from staged package
0215 using the old patch in reverse, checked the helper-pinned preimage hash,
regenerated the unified diff, and applied it at fuzz0. The clean pinned fork
replay passed all 206 patches; 97 touched paths were byte-identical to the
working fork. That replay exposed patch0214's stale hunk context (`Hunk #3
FAILED at 177`), which was corrected to match the pinned source line without
changing verifier behavior. Evidence: `logs/m4-brightness-0216/source-transform.json`
and `logs/m4-brightness-0216/full-fork-replay-final.log`.

The compiled adapter smali hash is
`5887361cb9d89c07cde0827cc22ae1f3ce00af1f538c2f948fddb663c00418d5`, matching
the reviewed output. It contains `const/16 p2, 0x64` and no
`USE_PERSONAL_AUTO_BRIGHTNESS_V3` reference. The rebuilt `services.jar` in the
image hashes to
`f7fa3b4df8c153028754164d1c69e7535876b9fc0b77e1107c5ac8a72ff7d001`.

M4 package run `rom/out/releases/M4/star2lte-1793199175-ncgBlt/` completed with
`build_exit=0` and `.complete`. SYSTEM, VENDOR, and ODM passed ext4 checks;
staged SELinux compilation, the M4 donor/runtime and compiled app checks,
ADB defaults/no-auth/stored-choice checks, and the Exynos9810 flashable verifier
passed. The selected kernel is the existing duhansysl DS-ACK V1.12 08/05/2026
Enforcing-OneUI7 package without KernelSU.

Full signed ZIP SHA256:
`828a491708c47e967cbea9164faaebfd17f1bb9a90b470a52a420c8737d5acf7`.
Target-files ZIP SHA256:
`7f882c3d8f5dc4bda2db3b2806918c8c4ba344f85b6b34e9aa7bd1656b3bfffc`.
The full-package `SHA256SUMS` verified. All eight signed ZIP32 stages passed
CRC, signature, ZIP32-size, ordered-range, and intermediate no-BOOT checks;
their hashes are in `SHA256SUMS.legacy` and the splitter report is
`M4-0216-ZIP32-legacy-manifest.json`.

## Runtime validation in isolated package M4-0217

M4-0217 used the same build configuration and embedded kernel archive as the
known-boot M4-0215, with patch0216 as the sole source-payload delta. Its signed
ZIP, target-files and eight ZIP32 stages passed verification; exact hashes,
install scope, and logs are in
`logs/20261029-m4-brightness-isolation-0217/result.md`.

Fresh install on G965F `M2_G965F_SERIAL` reached Android after about six
minutes and completed boot (`sys.boot_completed=1`). The Vietnamese Setup Wizard
welcome screen was captured. The cache logger reached `post-fs`,
`post-fs-data`, and boot-completed; its logcat snapshots were pulled from
`/cache/unica`.

With the display awake and manual mode, settled panel raw values were 1907 at
Settings20, 5923 at Settings60, and 11947 at Settings120 (panel maximum44800).
The original Settings110 value was restored and returned raw10943; the temporary
USB stay-awake setting was also restored. This verifies the tested PAC3 scale is
monotonic and near the expected 100x path. Settings0/off, screen off/wake and
entry into auto mode were also tested and restored. The follow-up validation
adds5Dozing→Awake cycles without new crash-buffer lines; panel raw returns
0→18301 at the user's current setting183. The user directly confirms physical
brightness changes clearly and normally. Multi-lux response, HBM/bounds and
thermal/brightness limiting remain unvalidated; no extra high-brightness
stress test was run.

The first short logcat snapshot ended before subsequent Java crashes and its
pattern scan omitted a first-boot SystemServer watchdog. The fuller follow-up
has an authoritative KG30-blocked ANR, two SystemUI crashes and Bluetooth/network
failures. This patch's successful brightness test therefore does not establish
a clean M4 boot. Current status and exact evidence are in
[validation0217](m4-validation-after-0217.md) and the
[repair plan0217](m4-post-0217-repair-plan.md). Keep the selected rollback while
the remaining runtime checks are pending.

## Rollback

Before install, preserve package0215's exact SHA and all current system/runtime
metadata. If the new framework fails to boot or scaling/off behavior is wrong,
restore the saved package0215 system state through the existing recovery
procedure, verify its `services.jar` hash, and remove0216 from the source series
and lock. Do not modify kernel, vendor, modem, EFS, or CPEFS for this repair.
