# M4 attempt0222 — build/package review

Completed 29 October 2026 in the existing fork. This is a complete host-side
diagnostic candidate for G965F, not acceptance of M4 and not proof that the
recent Bluetooth, network, or reboot issues are fixed. This turn did not flash
or write to a phone.

## Inputs

- Run: `rom/out/releases/M4/star2lte-1793274163-CcPxEn/`.
- Profile: `m4-g965f-a376b-android17-runtime-init-debug`, target `star2lte`,
  model guard `SM-G965F`, ext4, enforcing.
- Donor: SM-A376B/XXV/A376BXXU5BZI6, Android 17 / SDK 37; firmware config also
  retains the previously reviewed S22 EUX extra source.
- Hybrid Vendor VNDK33 and G965F-specific hardware/radio inputs remain selected.
- Kernel: DS-ACK V1.12 G965F WIPEONFORK diagnostic, no KernelSU;
  archive SHA256
  `4cea63f84d5add494bfcc2f9fff10584806a930a24aca975dc261bf6720e20a`.
  This is the already approved diagnostic package; no kernel was built here.

## Host verification

The clean existing-fork build exited 0. The signed flashable ZIP passed the
Exynos9810 verifier as ext4/enforcing with six device payloads, and the full ZIP
passed `unzip -t`. The real SYSTEM/VENDOR/ODM ext4 audits, 50 locked native32
metadata checks, staged SELinux policy compile, A37 app64/native-vendor32 ABI
gate, compiled ADB defaults/no-auth/stored-choice lifecycle, compiled PIF gate,
and locked M4/G965F/BZI6 backend selector all passed.

The network composition gate reports the exact expected BZI6 Tethering APEX
hash `650f75fa946ee5fbe44e38418c4581738e7654abcea527832c97d092d6d7671d`, with
`normal_profile_changed=false`. This proves the locked package composition;
it does not prove ordinary IP/DNS/HTTP/HTTPS behavior on the phone. Likewise,
the Bluetooth backend selector and compiled package checks pass, while the
VaultKeeper-backed key-store path still needs the runtime format/read/write
trial in `docs/m4-post-0219-repair-plan.md`.

The full flashable ZIP is 5,204,423,846 bytes, SHA256
`c3aff071dec34579a69e7e66f88e134a9714e89d850e2a09582667ae367ee23d`.
Target-files is 5,603,405,133 bytes, SHA256
`c741a3d1118169e707d02874d1cc96fc9925db9e1b0101618344e9919c9dbf06`.
Both entries in `SHA256SUMS` were re-read and passed.

The fork splitter created eight ordered, signed legacy ZIP32 stages. All eight
passed CRC, signature and classic-ZIP32 checks; `SHA256SUMS.legacy` and the
legacy manifest passed hash readback. The largest stage is 1,186,925,449 bytes.
The manifest SHA256 is
`e200a708f17b90746d5c4dbb5da1a9a5e983c4cb68fc70e835bd9b88bdd3dd2f`, and its
raw SYSTEM stream hash is
`e3681ef0a688a959e613d9f5e8d7b03acd13c38bc01fce90a81217fc6c053c45`.
`.complete`, `SHA256SUMS`, `SHA256SUMS.legacy`, the split manifest,
`artifact-review.json`, and the selected build config are stored in the run.

Evidence: `logs/20261029-m4-g965f-bt-native-0222/build.txt`,
`split-zip32.txt`, `buildenv.txt`, and `stage-buildenv.txt`.

## Runtime status and limits

No phone was flashed during this build. Do not infer a Bluetooth, network,
Setup Wizard, or boot-loop fix from compiled/image/package checks. M4 remains
unaccepted; continue the one-unit-at-a-time runtime plan on G965F before any
G965N trial. The previously observed Android reboot reason remains
`[SetupWizard_Global] 30 minutes timeout`; the TWRP `last_kmsg` evidence in the
separate reboot-diagnosis logs describes the later recovery reboot, not an
Android kernel panic.

Rollback is to retain/use the previous signed M4 package and rebuild a fresh,
versioned run if this candidate fails runtime. No EFS/CPEFS/RADIO/bootloader or
protected partition was touched here.
