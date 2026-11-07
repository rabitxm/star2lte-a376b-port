# M4 network stats runtime fixture

Reason: Android17 StatsPullAtomService calls the newer query API, and generic
transport templates require matching identity, Parcel and disk semantics.

The Smali fixture runs as a normal app_process child after the diagnostic
network APEX is active. It does not run as PID1, call the stats Binder service,
modify network state or write persistent statistics. Its IO tests use byte
arrays. Compile with the fork's bundled apktool/smali API29 (actual DEX039).

Coverage: Long.compare runtime dependency; default/disabled augmentation flags,
invalid flags; missing/legacy transport builder rejection; Wi-Fi positive and
cellular negative filters; legacy identity and template constructors; Parcel
consumption and matching after round-trip; old IdentitySet writer routed to
v8 with transport bits; explicit v7 read with donor legacy-type deduction.

After installing the reviewed diagnostic APEX and staging the generated test
JAR with shell_data_file label under /data/local/tmp, run:

```sh
CLASSPATH=/data/local/tmp/m4-network-stats197-fixture.jar \
    /system/bin/app_process /system/bin M4NetworkStatsFixture
```

Only an exit0 and `M4-STATS-API-SELFTEST PASS` is a pass. The fixture does not
certify live traffic accounting, permissions, all Binder clients or radio.
Private source/JAR hashes, output and trial logs are recorded in
logs/20261028-m4-network-stats and logs/20261028-m4-stats-test. When rolling
back to the old reader, restore the two narrowly scoped netstats-directory
snapshots as documented; do not erase other app data.
