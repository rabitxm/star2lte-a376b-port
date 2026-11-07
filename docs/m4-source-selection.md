# M4 BZI6 source selection and initial UI policy — checkpoint0134, 26/10/2026

Attempt03 passed the Android17 NFC APEX and camera dispatches, then failed at
the old synthetic-password adapter. Patch0134 keeps BZI6's new Keystore-backed
`createSyntheticPasswordBlob` / `unwrapSyntheticPasswordBlob` path only for the
reviewed M4 G965F SDK37 profile; out-of-scope Android17 profiles fail closed.
Source dispatch fixtures pass and M2's legacy crypto patch remains active.
Credential creation/unlock and the effective Keystore security level remain
runtime gates; no complete M4 image or device test exists yet.

Checkpoint0128 adds the isolated Android17 donor configuration and source/
compiled-fixture checks for eSE, retained Wi-Fi UI and initial Face/Bixby/
optional Settings selection. Checkpoint0131 adds a scoped G965F/no-KSU M4 profile.
Clean build attempt01 reached the NFC adapter and exposed an Android16-only path
guard; attempt02 passed NFC after0132, then stopped at the One UI8-only camera
`PackageUtil.smali` lookup. Patch0133 skips that old adapter only for the exact
M4 profile while existing finalization keeps the locked BZI6 Android17 camera
app/server. No complete M4 image, flash or runtime proof exists yet. M2
attempt56 remains the reference. This report does not establish M4 boot,
hardware eSE/NFC, camera, Wi-Fi or Face behavior.

Update0129: the native32 lock/cache/staging slice listed as next work below is
now implemented and checked; full runtime exceptions/profile remain pending.
Current evidence: [m4-native32-runtime.md](m4-native32-runtime.md). The0128
results below retain their original checkpoint scope.

## Donor selection

`rom/unica/configs/essi_a376b_android17.sh` is now a separate registry entry.
It selects SM-A376B/XXV/LOCAL with cache suffix A376BXXU5BZI6 and records the
previously audited BZI6 values: framework SDK37, shipping36, board-first34,
OneUI90000 input, group_basic, RecordAlive07010, optical fingerprint, mDNIe/
brightness/HFR source values, dynamic SIM tray detection and source WLAN flags.
S22 remains only the native32 compatibility input. M1/S22 and M2/AZH4 configs
remain unchanged. The four unknown Bluetooth metadata values stay `none` and a
new recipe consumer blocks config loading rather than inventing their values.
Target vendor/board33, target shipping26 and correct F hardware remain required.

The actual fork config generator now runs the real M2 profile, the scoped M4
profile and M2 again in isolated output folders. Cache roots/markers select
AZH4 → BZI6 → AZH4, source SDK is 36 →37 →36, and both resulting M2 configs
match except timestamps. Selecting M2 after M4 clears all M4-only flags; the
original `rom/out/config.sh` hash is unchanged. `make_rom.sh`'s donor hash
includes the M4 runtime/fixed/Wi-Fi-UI/boot-limit selections. This validates
source/config/cache dispatch, not a compiled work-tree or ROM build. Full module,
Knox and final boot/vendor/image dispatch gates remain before a clean build.

## eSE policy retained from M2

Keep target COS/vendor as `none`; do not infer a COS from G965F's multi-COS
lookup or empty runtime SKU. The old generic BZI6 rule fails at
`supportEseHal()Z`: `Failed to replace value "JCOP6.5U"` in
`logs/20261026-m4-profile-preflight/ese-original-rule.txt`. BZI6 places the value
inside `", eSE_COS: JCOP6.5U"`, not an exact standalone literal.

0125 pins four entire classes before edits: SecureElement UtilExtension,
framework SemService/SemServiceManager and UCM CredentialManagerService. It
performs only the existing target-none COS/vendor substitutions, including the
new prefixed log. SKU/country/feature detection and all other instructions are
byte-identical to their donor inputs. Blank COS disables the compiled eSE HAL
flag and SemService REE-SPI path through the existing checks. It neither emulates
eSE nor claims hardware acceptance; OMAPI UICC/NFC code and permissions are not
removed. Source/DEX/gate fixtures pass; an actual unmodified SecureElement donor
APK fails the new compiled gate. M2's dispatch trace remains identical.

## Initial Wi-Fi UI selection

0126 adds STAR2LTE_M4_KEEP_DONOR_WIFI_UI, separate from M2's flag. Source target
11ax and6GHz remain false; the Hybrid Vendor Wi-Fi5 hardware policy is unchanged.
Both S22 UI-removal patches are omitted only after exact BZI6/F/M4/SDK37 scope
and13 class input checks. BZI6 replaced the old WifiEntriesFilter synthetic
lambda with a main class and a Kotlin coroutine; their actual classes and the
old lambda's absence are checked.

A compiled gate compares all13 retained UI class bodies after normalizing only
source comments/markers, hidden-API access labels and const-string/jumbo pool
encoding. Instructions, labels, constants, fields and annotations otherwise
remain checked. All13 actual classes assemble and round-trip through DEX039.
Removing semIsWifi6Network, as the old S22 patch would do, fails the gate.
Retaining AP labels/icons is an initial UI limitation; it does not advertise
11ax support in target hardware configuration or prove actual network behavior.

## Face, Bixby and optional Settings

0127 uses the separate STAR2LTE_M4_BOOT_LIMITS selection in the exact BZI6/F/M4
scope. Six original patch preflights fail on the actual donor code; their raw
`patch does not apply` diagnostics are saved in `optional-original-preflight.txt`.
The selected initial path leaves donor behavior intact and hides Google Photos
spoofing, secure-window override, downgrade, target-SDK bypass, screen-capture
notification override and Bixby remap controls. Other controls are retained.

The actual patch dispatcher skips exactly those six patches in the selected
scope. Physical key703 retains WINK through both existing stack/final stages;
compiled XML/keylayout gates cover M4 as well as the existing M2 selection.
A compiled services gate rejects a restored UnicaBixbyRunnable.

Face remains omitted through the existing HAL/init cleanup, keeping the
framework FaceManager feature XML needed by SystemUI. New make_rom and boot/
release guards accept only the reviewed initial M4 selection. Shared omission
fixtures are idempotent and preserve an unrelated fingerprint HAL. Full M4
boot-contract acceptance remains open; its Knox SDP and other runtime-specific
branches have not been broadly converted from M2.

BZI6 search keeps211 donor providers and appends4 UN1CA providers through its
pinned factory/base/mobile classes. Settings Intelligence now has36 original
keys, including top_level_buds_unite. The old35-key/register edit is replaced
with a pinned constructor edit that reserves a fresh v37 local and appends
only top_level_unica. All36 donor keys remain. The compiled gate checks both
registration counts and the37-key array. Actual AZH4 control fixtures produce
byte-identical output before/after the search-adapter extension.

## Verification evidence and remaining work

Evidence is under `logs/20261026-m4-profile-preflight/`:

- `ese-method-audit.txt`, `ese-bzi6-test.txt`, `ese-original-rule.txt`.
- `wifi-ui-inputs.json`, `wifi-ui-test.txt`: source pins, DEX round-trip,
  removal negative and actual shell dispatch; M2 trace unchanged.
- `optional-original-preflight.txt/json`, `settings-limits-test.txt`,
  `search-m2-regression.txt`, `boot-selection-test.txt`: source rejects without
  partial writes, real DEX search gates, scoped skips, keylayout and shared Face
  omission fixtures. Compiled XML/keylayout checks are wired for M4 but still
  require a complete rebuilt Settings APK/work tree.
- `source-selection-test.txt`, `config-m2-first/`, `config-m4/`,
  `config-m2-again/`: actual config generator and cache-marker selection.
- `replay-through-0128.txt`:118 patches replay from pinned2b25ae6;
  101 resulting text files match the active fork byte for byte. All114 hashes
  locked before this checkpoint remain unchanged. Shell/Python/JSON/whitespace
  checks pass.

Historical next gate before checkpoints0129–0131: add the isolated native32/runtime
locks, audit retained donor exceptions, finish generic module/mod dispatches, and
create a scoped M4 profile before any clean build. The native32/runtime locks and
profile now exist; remaining module/Knox and final boot/vendor/image contracts
are still required. ADB/Auto
Blocker fresh defaults, compiled lifecycle/no-RSA and runtime toggle tests remain
mandatory. No kernel/vendor/radio/phone write or retained-release overwrite
occurred in this checkpoint.
