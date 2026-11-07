# M4 NFC integration — BZI6 Android17 / S3NRN82

Status: attempt16 confirmed the G965F PHN/BZI6 sidecar dispatch: build log
line1038 records `G965F PHN has no NFC extension sidecars; retaining hash-pinned
BZI6 donor extensions with the target JNI/HAL stack`. Patch0144 corrected the
target-SDK guard to the generated SDK31; positive, wrong-model, wrong-SDK and
changed-JNI fixtures pass. Attempt16 then stopped at a separate display adapter
error (`m4-build-attempt16.txt:1080`) before image creation. Hardware NFC behavior
remains untested; the build has not reached final SYSTEM/VENDOR image creation or
runtime NFC/credential testing. This report is limited to the M4 G965F/BZI6
profile; it does not claim G965N support.

Attempt01 stopped at `logs/20261026-m4-profile-0131/m4-build-attempt01.txt:62`:

```text
(unica/patches/exynos9810_device_stack/customize.sh:4539:_EXYNOS9810_APPLY_NFC_STACK) Android 16 NfcNci.apk is missing from the system image
```

The adapter assumed Android16's `/system/priv-app/NfcNci/NfcNci.apk`. BZI6
Android17 stores NFC in `system/system/apex/com.android.nfcservices.capex`. Its
embedded `NfcNciApex.apk` is package `com.android.nfc`, version17, minSDK36,
compile/target SDK37. The paired APEX JNI is also version37.

The source lock is
`rom/unica/patches/exynos9810_device_stack/nfc/a376b_android17_nfc.lock.json`:

- CAPEX SHA256: `4a809ebc2d99b5f68066788bd5eb9710139487c55bf1523ee945d0692463af53`.
- Embedded NFC APK SHA256: `1252b5df4f366d0c9ecde59f8e884f4f155164d76c087324711037299fff0470`.
- Embedded `libnfc_nci_jni.so` SHA256: `e312f4fd01044e2bb3ee26900df73090a4a657ed6292d4e0e056fa73b23e5d81`.

The JNI contains static references to HIDL NFC 1.0/1.2 and AIDL `INfc`. The
selected S9+ stack exposes HIDL `android.hardware.nfc` 1.0/1.1/1.2 and Samsung
`vendor.samsung.hardware.nfc@2.0::ISehNfc`; it launches the target S3NRN82
service and uses the target-specific firmware/RF calibration. This is enough to
stop discarding the donor's correct Android17 NFC framework package, but static
symbols do not prove interface negotiation, HAL startup, tag I/O, eSE/card
behavior, SELinux access or Samsung NFC UI integration. Those require device
logs after a complete image is built and tested.

The separate BZI6 `SecNfc.apk` (SHA256
`ab7bffc2f12562bf25b349007baaa0276ec7f5e26c32c6bcc5e1319a6b24508f`) is
package `com.samsung.android.nfc` v17, min/target SDK37, and references both
`vendor.samsung.hardware.nfc@2.0::ISehNfc` and the Samsung NFC AIDL interface.
The selected target service advertises Samsung HIDL 2.0 only. The final-image
gate pins this donor APK too, so a boot test evaluates the actual selected
package. Whether the app chooses its HIDL path and whether enhanced Samsung NFC
UI functions work remain runtime questions.

Patch0132 changes the existing Exynos9810 customizer to accept the APEX only
when the BZI6 source, M4 milestone, SDK37 and `EXYNOS9810_M4_RUNTIME_COMPAT`
match. Other donor profiles keep the existing Android16 `NfcNci.apk` behavior.
The existing `verify_m4_runtime_image.py` now pins both donor/work NFC files
and compares their packed SYSTEM ext4 bytes, mode, root ownership and SELinux
label. Its positive fixture accepts both exact files, and a mutated work
`SecNfc.apk` is rejected. All122 ordered patches replay on pinned fork commit
`2b25ae6fc3098650be249bd1898e21e563cd5df1`; all four 0132 source files match
the replay byte-for-byte. See `logs/20261026-m4-nfc-0132/replay-local-0133-success.txt`.
See `docs/patch-matrix.md` for the full reason/files/verification/rollback
record.

Checks passed: `aapt2` package/version/SDK inspection, nested APEX extraction,
APK/JNI/CAPEX/SecNfc hash checks, HIDL/AIDL symbol inspection, Python/shell/JSON
syntax, positive exact-file gate fixture and mutated-work SecNfc negative check.
The full fork patch replay passed through0133; the clean build rerun and
packed-image check remain pending. No image was created and no phone operation
occurred during this checkpoint; kernel/vendor/radio/modem/EFS were not changed.
Audit output: `logs/20261026-m4-nfc-0132/nfc-source-audit.json` and
`logs/20261026-m4-nfc-0132/camera-source-audit.json`.
