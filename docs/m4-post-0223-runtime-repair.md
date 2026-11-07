# M4 after attempt0223 — runtime repair, 30 October 2026

Scoped runtime repair completed on the G965F test unit; M4 acceptance remains open. Keep the retained signed attempt0223
package unchanged. These are separate one-file trials with USERDATA preserved;
they are not a new complete ROM release and do not accept M4 or G965N.

## New restart cause and patch0221

The longer runtime capture invalidates any long-term stability inference from
the previous 90-second sample. `sys.system_server.start_count` reached 5 in the
same kernel boot. Exact evidence in
`logs/20261030-m4-repair-0224/run-121332/crash.txt:39`:
`java.lang.RuntimeException: Unable to create service android.net.ConnectivityCallListenerService`.
The fatal system-process exception is at line59 and the causal
`ClassNotFoundException` at line73. This is a system-process restart; the
evidence does not establish a kernel reboot/panic.

BZI6 declares the component in framework-res as an InCallService protected by
`android.permission.BIND_INCALL_SERVICE`. The actual compatibility network APEX
is SHA256 `650f75fa946ee5fbe44e38418c4581738e7654abcea527832c97d092d6d7671d`;
its framework-connectivity JAR is
`cb000f867b4a969954b157f50a1c07920fc047a3232ee51086dc0fb0ace541f3` and lacks
the advertised class. The exact BZI6 class exists in the donor JAR and is an
OTT network-slicing listener. Its call callbacks return when feature bit0x4 is
off. The read-only runtime probe returned `backend_feature_bits=0x3` and
`ott_network_slicing=false`; it also confirmed the original component state0.
It does not synthesize a call or expose call/key/identity data.

A diagnostic Package Manager change disabled only this component with
DONT_KILL_APP. In the subsequent 120-second sample, the kernel boot ID, core
PIDs and system-server count5 stayed unchanged. A resolver query returned
`No services found` for this exact disabled service. This supports the causal
repair but does not replace a later real call/hardware test.

Patch0221 adds a scoped finalization helper and a compiled/image gate. It sets
only this manifest service's `android:enabled` attribute to false for the exact
G965F/BZI6 diagnostic network composition. It leaves ordinary Telecom service
declarations, the service permission and intent filter unchanged. It does not
add dummy Binder methods or claim OTT network slicing support.

The existing fork rebuilt/signed framework-res. Comparing all non-signature
ZIP payloads found only `AndroidManifest.xml` changed; compiled XML comparison
confirmed the single enabled attribute. Resources and every other payload
were identical, and the platform signing certificate matched the baseline.

- Baseline framework-res SHA256:
  `b85d746b88d2485c178dda291ed8511bdb8fd7373c102293b64ad9e9087af17b`.
- Candidate SHA256:
  `d965ee80323faa52c6120b0af0a3afc71c0b818c1f09626101d3d3ea5b011fb9`.
- Scope/input/negative/idempotence fixtures and compiled helper check PASS.
- Recovery installed only this SYSTEM file; readback hash, uid0/gid0/mode0644,
  actual `system_file` label and ten protected partition hashes passed.
  EFS/CPEFS/RADIO/RECOVERY/BOTA0–2/BOOT/VENDOR/ODM were unchanged across this
  one-file trial. USERDATA was preserved.
- TWRP's `chcon` returned1 without output while the staged inode already had
  the required label. The installer stopped before rename, then resumed only
  after independently checking the actual label. Its BusyBox stat lacks `-c`;
  the probed `/sbin/toybox stat` verified numeric ownership/mode. The failed
  checks and resumed steps remain in private run logs.

Reboot/default-state verification PASS: Android reached boot_completed at
about45 seconds, with system-server start_count1. The probe restored user-0
component state2 to its original default0. The resolver still returned
`No services found`; explicit service start returned
`Error: Not found; no service started.` No call was placed. All core PIDs,
boot ID and start_count1 stayed fixed in the following120 seconds, and the
new crash buffer had no missing-call-service fatal. ADB screenshot after wake
shows the Welcome page under a wireless-debug authorization dialog; no network
debug permission was granted. The earlier black screenshot was captured while
the display was asleep. Evidence: `call-listener-runtime/` in the run directory.
Real incoming/outgoing call acceptance is still open. Rollback is the exact
baseline APK retained on the PC; do not restore the runtime component to default
while the old enabled manifest is installed unless deliberately reproducing
the fault.

## Bluetooth trials and source patches0222–0223

Runtime maps/hash confirmed the active JNI is the system copy, SHA256
`26585f22ffb02a294578e6b652f3c14ff238989ff674f98050c2aec135db0ecf`.
The APEX donor copy is not mapped. The prior A2DP enqueue skip at0x6979e8 is
present, so attempt0223's timeout was not a missing deployment of that fix.

The first experiment skipped two measured HFP startup sends at0x70c190 and
0x70c1f4. Its JNI SHA256 was
`4413788f0aee41e603ecafb4d097dcca0f5ae57b263774a3859a0bc3f7a92481`.
It loaded successfully but FAILED runtime: Bluetooth crashed5 times and
`bluetooth-trial/runtime/logcat.txt:95186` still records
`Timed out waiting for Unknown OpCode(0xfdf1) for 5000ms`. Disassembly shows
an earlier group initializer in the same enabled DBFW+ path; skipping two
commands did not disable that initialization. The unaccepted source experiment
is archived as `diagnostics/0222-hfp-only-failed.patch` and is not in the active
series. Preserve its logs as a failed trial, not a fix.

During that trial BusyBox cp replaced the destination inode/label, contrary to
the installer assumption. A metadata guard stopped before boot. The probed
`/sbin/toybox chcon` restored the exact system_lib_file label and numeric metadata
was verified before any reboot. Ten protected hashes matched afterward.

The final source patch0222 implements the BZI6 equivalent of the fork's legacy
DBFW+ capability quirk. At0x70c0c8 it changes `strb w0,[x20,#0x48]` to
`strb wzr,[x20,#0x48]`; at0x70c0cc it takes the original unsupported branch
0x70c280. That branch already clears secondary state at global+0x4c, passes
zero/disabled arguments, and returns without the optional group initialization.
The original HFP call bodies remain intact. This avoids pretending that the
controller answered a command or changing SCO transport, bonding keys, vendor,
radio, kernel, or policy.

The helper pins the entire setter, original/replacement instructions, original
branch target, disabled-path bytes, Build ID and composed final JNI hash.
Donor, old A2DP-skip composition, exact two-instruction boundary, idempotence,
changed-context and rejected-HFP-only fixtures PASS. The old FDF1/native-backend
final hash locks and image verifier were extended to check this composition.
Candidate JNI SHA256:
`2b1f7bd88bdc01f8f227a6c6d24f1d87bb0857004d7533f541a7a9762d64571f`.

The HFP-capability trial also FAILED: its two HFP startup messages disappeared,
but the 90-second sample still recorded seven Bluetooth crashes. Exact evidence:
`bluetooth-capability-trial/runtime/logcat.txt:94961`,
`Timed out waiting for Unknown OpCode(0xfdf1) for 5000ms`.
SystemServer stayed at start_count1. This distinguishes the local Bluetooth
failure from the repaired system-process restart. Bluetooth was temporarily
disabled through the ordinary service command to stop the crash loop; its
original ON preference was recorded and will be restored for the next trial.

Patch0223 maps twelve other FDF1 sends with null callbacks, plus the main
DBFW+ capability byte. It changes the main store at0x6ea574 from w8 to wzr,
retains the original non-null callback send at0x98d9a8 behind its existing
zero-capability guard, and NOPs only the twelve audited null-callback calls.
Nine use a direct BL; three use the locked indirect table slot0xe9beb8 whose
stored target is the same enqueue function0x9df9b4. The HFP call bodies remain
intact behind0222's unsupported path. An unrelated store to offset0xb60 at
0x850dc8 uses base0xf23000, not the capability at0xf10b60, and is unchanged.

The [AOSP vendor-send implementation](https://android.googlesource.com/platform/packages/modules/Bluetooth/+/refs/heads/main/system/stack/hcic/hcicmds.cc)
provides the void-return/optional-callback contract; the Samsung call sites,
registers and branch targets are verified from this exact BZI6 ELF. This does
not authorize skipping a call with a real callback or fabricating completion.
The fixture checks all thirteen instruction slots, opcode and null callback
arguments, direct/indirect targets, retained callback guard, exact contexts,
donor output, idempotence and negative mutations. All composed final JNI
verifiers PASS. Candidate SHA256:
`2885d0bcfbbf15a26521f7bf1574e18a52b22dda8c6b00a81e996781a608acdf`.

The guarded one-file recovery trial PASSed startup and stability: thirteen
samples over120 seconds kept the Bluetooth PID and recorded zero crashes and
zero FDF1 timeout. Three ordinary disable/enable cycles PASSed, followed by an
extended same-boot sample at uptime491 seconds with the same system-server and
Bluetooth PIDs. The first test's strict STATE_OFF waiter failed because the
adapter correctly retained BLE_ON for AutoHotspot/MCF scan clients. A corrected
check required the stored toggle0, mEnable=false and BLE_ON/OFF, then ON after
enable; it did not revoke those clients or reset Bluetooth data. The original
ON preference was restored. A subsequent SystemUI-only reboot automatically
enabled Bluetooth and also recorded zero crashes through uptime423 seconds.
Pairing, A2DP playback, HFP call audio and reconnect to a real accessory remain
unverified. Evidence: `bluetooth-complete-dbfw-trial/runtime/`. Rollback JNI files
are retained on the PC for each trial; the original26585f22 baseline is also
preserved. A complete ROM package has not been rebuilt for these source changes.

## SystemUI optional legacy AOD hook — patch0224

The first Bluetooth-fixed boot still records a separate UI fatal in
`bluetooth-complete-dbfw-trial/crash-during.txt:17`:
`java.lang.AbstractMethodError: abstract method "void com.android.systemui.plugins.keyguardstatusview.PluginKeyguardStatusView.disconnectPluginOnBackground()"`.
This is the retained signed Infinity AOD plugin, SHA256
`b374094fe87c6ab9f231096ddad4cd1bb0945be74c3247c9a441af8a3262b9f3`.
It predates the new optional background-disconnect hook. The current BZI6 AOD
implements that hook by notifying its new callback interface; the legacy
callback interface has no corresponding method. Copying that body is invalid.
The ordinary PluginInstance unload already invokes onDestroy; legacy AOD's
onDestroy detaches its view and clears callback/singleton state.

Patch0224 changes only VersionCheckingProxy behavior. It checks the exact
interface, hook name, void return, zero parameters and legacy implementation
class. It queries the actual target object, not the proxy object's class. It
omits only an absent or abstract implementation of this hook. A concrete hook
still follows the original invoke path, including its exceptions. Other
methods and ordinary onDestroy are unchanged. The separate existing
setDarkAmounts(FF) wrapper already falls back to legacy setDarkAmount(F) using
its second/interpolated argument; no new patch is needed for that caught API
mismatch in this trial.

Source/compiled/AOD hashes, idempotence/changed-source rejection and the old
three-class thread/generation guard verifier PASS. Comparing64,508 roundtrip
DEX classes found one semantic change (VersionCheckingProxy) and one unrelated
const-string→const-string/jumbo encoding shift for the same string/register.
Only classes2.dex differs among non-signature ZIP entries; signing certificates
match. Candidate APK SHA256:
`30ccaf1dd9d0327583591e4e81661f0f8226be54e0698648943575bd094ad588`.

The compiled candidate was tested in an ordinary app_process child against
four independent synthetic plugin class loaders: missing hook omitted,
concrete hook called once, concrete exception propagated, another target's
missing hook still errors. All four retained onDestroy side effects. The old
APK failed the first missing-hook negative control. Probe source and variant
fixtures are in `support/diagnostics/m4-facewidget-legacy/`; private results are
in `legacy-hook-probe/runtime-final.json`.

Only the SystemUI APK was installed from TWRP, with exact readback hash,
uid0/gid0/mode0644/system_file and ten unchanged protected partition hashes.
The subsequent boot had boot_completed1, server start_count1 and original
startup SystemUI PID2567. Seven samples/121.77 seconds and five actual
Asleep→Awake cycles kept the same kernel boot ID and all five core PIDs:
system_server1824, SystemUI2567, Bluetooth2456, SurfaceFlinger1529, netd1417.
The later uptime423-second sample still had the same server/UI/Bluetooth PIDs,
Bluetooth ON/zero crash. No SystemUI fatal, missing call-listener fatal or FDF1
timeout appeared. Bixby's separate startup exception remains in the buffer.

A wireless-debug network authorization dialog obscured the Welcome page.
UiAutomator returned a null root node; the first immediate wake/tap did not
close it. After waiting for verified Awake, a Cancel tap closed that exact
dialog. No network debugging authorization was granted. A screenshot then
confirmed the Vietnamese Welcome page and Start button; no account/setup
completion was performed. USB ADB stayed enabled with ro.adb.secure0. Wireless
ADB remains enabled by the fork's existing default and still needs a separate
default/lifecycle audit. Raw screenshots and network identifiers stay private.
A Settings launch while asleep did not establish a Settings acceptance result;
the final foreground is the original Welcome activity.

## Remaining Bixby branch and next repair unit

Exact current boot evidence:
`systemui-legacy-trial/runtime/crash.txt:16`:
`Caused by: java.lang.IllegalStateException: No matching ComponentResult found!`.
The APK is still the original donor-signed BZI6 payload. Its d1.c.g(Context)
selects factory1 when Bixby is enabled and CSC is not SER, then requires
nq.b.N() to return true. That delegates to SecPropertyService.a(), reading
`SEC_FLOATING_FEATURE_SETTINGS_SUPPORT_FUNCTION_KEY_MENU`. The live log at
`systemui-legacy-trial/runtime/logcat.txt:146694` explicitly reports
`Bixby config SideKey : false`. With this false value the factory intentionally
throws instead of choosing a ComponentResult. This is a Side Key configuration
contract, not evidence of a missing DEX/DI registration.

Next unit: map BZI6's SideKeySupportComponentResult component list, the existing
S9 dedicated keylayout/dispatcher and a complete legacy-compatible Bixby payload
before choosing an adapter. Preserve the Samsung certificate/update contract;
the fork explicitly keeps Bixby donor-signed. Do not blindly set the global
Side Key menu feature true, replace the result with the non-Bixby factory, or
catch the exception and claim Bixby works. A future candidate needs isolated
boot-receiver and physical Bixby-button tests while retaining these three
successful startup/UI/Bluetooth repairs.

## Source records and open checks

All214 active source patches replay onto the pinned fork; all158 touched paths match
the working fork byte-for-byte. Evidence is in `source-replay-0221.txt` and
`source-replay-0222-capability.txt`, `source-replay-0223-all.json` and `source-replay-0224-all.json` under the private run directory above. No complete ROM
package was overwritten. The selected kernel and Hybrid Vendor/radio unit stay
fixed.

Bixby ComponentResult,
NFC dump FD/Binder failure, product model/name properties, app networking and
physical hardware acceptance remain open. An NFC dump failure is not evidence
that tag/RF/HCE is broken. The previous fresh-install EFS hash drift remains a
separate unexplained observation; this trial neither writes nor restores EFS.

Change record: reason is to repair the reproduced system-server fatal and
measure the remaining Bluetooth timeout. Fork files affected by0221 are the
finalization helper/lock/call site and SYSTEM image verifier;0222 changes the
Bluetooth capability helper/lock/call site, two composed hash locks and the image gate. Patch0223 adds the sender helper/lock, dispatch, three composed locks and image gate. Patch0224 adds the optional-hook adapter/lock, finalization and compiled/image AOD/SystemUI verification. Project files include the probe,
its README, this report, patch series/matrix, PLAN and bugs. Verification and
rollback are recorded per unit above; raw logs/APKs remain private and out of
Git.

Final read-only follow-up: uptime817.71s retained server1824/UI2567/Bluetooth2456, boot_completed1/server count1, Enforcing and Bluetooth zero crashes. Welcome screenshot was verified after declining the network debug dialog. Physical-panel/accessory confirmation remains pending.

## User confirmation and accessory follow-up —30 October

The user confirmed the physical display shows Welcome/setup, and has a
Bluetooth headset/speaker available for testing. The physical Welcome gate is
therefore confirmed in addition to the ADB screenshot. This does not establish
accessory audio acceptance.

At accessory preflight the PC initially returned `adb: device
'M2_G965F_SERIAL' not found`; after the user reconnected the cable, exact
G965F/star2lte identity and all three runtime repair file hashes matched.
Bluetooth was ON with zero crashes; system_server start_count1. This is a new
kernel boot versus the earlier repair sample. The user has been asked whether
they deliberately rebooted; do not infer a spontaneous reboot without that
answer and boot evidence. A separate bounded15-minute log capture is active
under `logs/20261030-m4-bluetooth-accessory/`, with private baseline/crash/audio,
Bluetooth samples and live logcat. Pairing, playback, volume/pause and accessory
power-cycle reconnect are pending the user's result; no call is placed and no
raw audio is recorded. No partition, kernel, vendor or radio changes were made.

Documentation change record: reason is to record the user's physical-screen
confirmation and prepare accessory validation; affected files are this report,
PLAN and the runtime composition record. Verification is the explicit user
reply plus selected-device identity/hash/baseline checks; accessory results
remain pending.

Accessory preflight's two boot-reason fields both report
`shutdown,[setupwizard_global]_30_minutes_timeout` in
`baseline-identity.json`. This points to Setup Wizard's tagged shutdown path;
manual subsequent power-on versus another reboot sequence still needs user
confirmation and timing. Do not label it a kernel panic or Bluetooth-triggered
reboot. The new-boot crash buffer contains the known Bixby exception, with no
SystemUI or system-process fatal at preflight.

## Completed phone-to-phone transfer follow-up

The user clarified `meizuo` is another phone, paired successfully and sent two
images successfully. The user also confirmed the new boot was their deliberate
reboot. The bounded collector has stopped.34 samples/337.03s retained the same
boot/server/UI/BT PIDs, zero Bluetooth crashes/FDF1 timeout, unchanged crash
buffer. Two incoming file terminations and the session completion report200.
This accepts pairing/incoming OPP file transfer, not audio. See
[m4-bluetooth-phone-opp-validation.md](m4-bluetooth-phone-opp-validation.md).
A2DP/AVRCP/HFP/audio-accessory reconnect remain untested; the earlier pending
physical Welcome confirmation is now satisfied by the explicit user reply.

## Follow-up: product identity and current crash buffer

The installed image still resolves the phone as donor A37 even though its
unqualified target properties are set to G965F. The read-only snapshot at
`logs/20261030-m4-remaining/identity-bixby/live-properties.txt` shows
`ro.product.model=SM-A376B`, `ro.product.name=a37xnaxx`, and
`ro.product.device=star2lte`. The partition-specific values are
`ro.product.system.* = SM-A376B/a37xnaxx/essi` and
`ro.product.system_ext.* = SM-A376B/a37xnaxx/essi`. The property source order
is `odm,vendor,product,system_ext,system`, which explains why the later system
properties alone did not repair the resolved model.

Patch0225 adds a narrow M4/BZI6/G965F finalization adapter that updates only the
`device`, `model`, and `name` values in the system and folded system_ext
`build.prop` files. It retains their Android17 donor fingerprints. The image
gate checks both compiled build.prop files and their contents in the real ext4
SYSTEM image. Fixture verification passed exact donor preimages, six output
properties, wrong-target and changed-preimage rejection, fingerprint
preservation, and idempotence. Ordered replay/build/image/boot verification are
still pending; no device files have been changed by0225.

The current boot's crash buffer has one known fatal entry. Exact lines
`logs/20261030-m4-remaining/identity-bixby/crash-buffer.txt:16-18`:
`Caused by: java.lang.IllegalStateException: No matching ComponentResult found!`,
then `at d1.c.g(SourceFile:92)` and
`at com.samsung.android.bixby.onboarding.receiver.LockedBootCompletedReceiver.onReceive(SourceFile:104)`.
The APK is still the original Samsung-signed BZI6 binary. Its selector requires
the Side Key menu when common Bixby support is true, while the S9 property is
false and remapping remains omitted. No replacement is included because
editing/re-signing Bixby breaks the Samsung signature/update contract, and
forcing the Side Key feature would claim unsupported hardware behavior.

At capture, Android was boot-complete, `sys.system_server.start_count=1`, and
Bluetooth was ON with zero crashes. This does not close the remaining product
gates: Bixby receiver behavior, Wi-Fi application traffic, NFC RF/tag/HCE,
Bluetooth audio profiles, cellular/call functions, and other physical hardware
checks still need separate evidence. The latest Bluetooth peer was another
phone and only two incoming OPP image transfers were tested; no audio result is
inferred from that test.
