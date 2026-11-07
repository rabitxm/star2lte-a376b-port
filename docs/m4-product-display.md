# M4 BZI6 product/display adaptation — 26/10/2026

Checkpoint0124 completes the reviewed source shipping/HDR/HFR/brightness and
initial fixed-resolution branch. It is a PC source/DEX fixture checkpoint.
No complete Android17 profile, ROM build, image or flash has run. M2 attempt56
remains the control. Runtime display behavior is untested on M4.

## Inputs and order

Actual inputs come from the isolated extracted BZI6 cache
`rom/out/fw/SM-A376B_XXV_A376BXXU5BZI6`. Nine APK/JAR input hashes and exact
paths are recorded in `logs/20261026-m4-product-display/decode-manifest.json`.
All use the fork's `apktool --no-debug-info -r` decode; scratch fixtures from
25/10 in `/tmp` were gone, so they were regenerated from the extracted inputs.
The pinned star2lte BiometricSetting compatibility APK was separately decoded
for the composition test; its previously locked SHA256 remains
bc64ca2e5083ae651a57e523eb68496ccf6f9dcaeabfed32c3adc1998acb877c.

The fixture applies the real helpers in order:0118 lights →0121 shipping/HDR
→0119 rear fingerprint →0122/0124 display. Shipping requires PowerManagerUtil
with the earlier lights addition; display requires that shipping change and
LsRune's earlier rear-FP change. Unknown method/field preimages reject before
any decoded class is written. Decoding and DEX assembly still use the fork's
existing tools; there is no new build system.

## Resulting source behavior

| Area | Reviewed BZI6 behavior and adaptation |
| --- | --- |
| Shipping API | Change only compiled hardware shipping36 to26 in EsecommAdapter, HdmSakManager, PowerManagerUtil, EngmodeTimeThread and the SystemServer log/threshold block; keep framework SDK37. Pin HdmVendorController's constructor absence and reject an unexpected TAProxy. This does not resolve the separate eSE/COS module audit. |
| HDR/video brightness | Both complete donor controller classes are present but return3/unsupported. Pin both classes, change only availability to0 and set the existing HDR floating feature. Their observers, preference behavior and setting writes are preserved; hardware effect still needs a live M4 test. |
| HFR/brightness | Pin15 classes/19 methods, change25 selected string instructions plus two input-service field defaults. Select default60, HFR mode0, no adaptive rate list or donor passive thresholds; brightness protocol3. Keep other control flow, thresholds, HAL calls and sensor queries. |
| Settings sensors | BZI6 Rune no longer has a compiled SPF string5 in its initializer. Its pinned supportAutoBrightness/supportLightSensor/supportCameraSensor methods query actual sensors. Preserve TYPE_LIGHT=5 and the camera-sensor fallback; never replace sensor type5 with configuration value3. |
| DEX relocation | Scoped BZI6/F/M4/SDK37 resolver uses the exact fully qualified descriptor, searches all DEX folders and rejects missing/duplicate definitions. No basename or synthetic-number guessing; unchanged replacement/return checks still apply. |
| Initial resolution | New STAR2LTE_M4_FIXED_RESOLUTION selection is separate from M2 flags and restricted to BZI6/F/M4. Keep native1440x2960/density560, remove switcher advertising in both feature copies, retain the donor controller's unsupported3 return. Actual M1/M2 selection clears a stale M4 flag. |

Original-rule reproduction is in `brightness-old-rule.txt`. Before0123,
line2 reports `Smali not found` for the old DEX4 Rune path; the real class is
in DEX3. With the previous A16 resolver enabled solely to isolate the second
problem, line10 reports `Failed to replace value "5" of method "<clinit>()V"`.
Both failed probes leave Rune byte-identical. These are source preflight
failures, not device boot logs or evidence to change the kernel.

## Verification and limits

Evidence under `logs/20261026-m4-product-display/`:

- `shipping-hdr-audit.txt`, `hfr-method-audit.txt`, `display-preimages.json`:
  actual methods, paths, hashes and constants used for the port.
- `shipping-hdr-test.txt`: five shipping classes and two HDR controllers,
  lights composition, wrong scope/changed preimage rejection, DEX039 assembly;
  original M2 shipping/HDR shell-dispatch traces remain byte-identical.
- `display-fixed-test.txt`: all15 class/preimage negatives; actual lights,
  shipping/HDR and rear-FP helpers compose with13 changed display classes.
  Affected folders across ten fixture artifacts assemble with the fork's
  existing API29/Dex039 writer. The three Rune methods and resolution controller
  are checked unchanged. These fixture APKs/JARs are not full signed packages.
- `smali-dispatch-test.txt`: actual moved BZI6 Settings class resolves from
  the old path; wrong SDK/model/milestone, ambiguity and absence reject. The
  existing A16 fallback still works.
- `fixed-config-test.txt`: M1 dynamic/M2 fixed/M4 fixed, invalid selection
  rejection, actual M1/M2 profile reset and finalizer fixed-mode block.
- New release-default gate reads the actual DEX methods and final system
  floating-feature XML. It checks shipping26, HDR availability, display
  constants/fields, preserved runtime sensor paths and absent switcher/rate
  advertising. It passes the composed fixture and rejects a real donor
  SemImsRune with stale HFR2 and final XML restored to mode2.
- `replay-through-0124.txt`: all114 patches replay from pinned2b25ae6;
  95 resulting text files match the active fork byte for byte. All110 hashes
  locked before this checkpoint are unchanged. Shell syntax, Python parsing,
  JSON parsing and whitespace checks pass.

No kernel, vendor/radio blob or phone partition was changed in this step.
No retained ROM package was overwritten. Full source-profile switching,
optional Face/Wi-Fi/UN1CA UI selection, eSE behavior, native32/runtime/APEX
composition, final boot/vendor contracts and mandatory ADB compiled/fresh-data
runtime gates remain before the first M4 build/acceptance. This checkpoint
establishes neither hardware HDR/60Hz behavior nor Android17 boot success.

Patch-specific reason/files/verification/rollback: `docs/patch-matrix.md`,
0121–0124. Current gate ledger: `docs/m4-dispatch-ledger.md`.
