# M4 optional connectivity call-listener probe

Reason: the BZI6 framework manifest advertises
`android.net.ConnectivityCallListenerService`, but the pinned S22-based
diagnostic network APEX lacks this class. Telecom binding it throws
`ClassNotFoundException` in `system_server`. The donor class implements OTT
network slicing and guards its call callbacks with feature bit `0x4`.

`M4CallListenerProbe.smali` runs in an ordinary `app_process` child. With no
arguments it reads the actual connectivity backend's feature mask and the
user-0 component enabled setting. It does not place or simulate a phone call.
Its device guard requires a G965F bootloader, `star2lte` and SDK37.

The diagnostic `disable` mode permits only the exact component's state
`DEFAULT(0) → DISABLED(2)`. `restore` permits only `2 → 0`. Both pass
`DONT_KILL_APP(1)` to Package Manager. They do not disable the android package
or another service. Query and retain the original state before changing it;
do not use restore to overwrite an unrelated user choice.

Build with the fork's existing `apktool.jar` Smali assembler, package its
`classes.dex` in a small JAR, verify the transfer hash, and run with
`CLASSPATH=/data/local/tmp/<probe>.jar app_process /system/bin M4CallListenerProbe`.
Only use an explicitly selected ADB serial after checking the physical variant
through its bootloader and the current framework-res/network APEX hashes.

On the reviewed M4 G965F runtime the query returned `backend_feature_bits=0x3`,
`ott_network_slicing=false`, `component_state_before=0`. A temporary disable
stopped the observed system-server restart cycle. Source patch0221 makes only
the matching manifest declaration disabled by default, scoped to the exact
diagnostic network APEX. After installing that APK, restore the runtime override
to default and verify the component still does not resolve.

Files affected: this README and the diagnostic Smali source. Verification:
existing-fork assembler exit0, runtime query, guarded state readback, negative
target/state guards, no call traffic, and reboot/default-state tests recorded in
`docs/m4-post-0223-runtime-repair.md`. This probe does not write a partition,
kernel, radio configuration or key store.
