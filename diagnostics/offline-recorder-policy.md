# Offline access check for the early CACHE recorder

24 October 2026. Reason: test a concrete recorder limitation before treating
the last saved log as the exact location of the M2 failure. Files affected:
this note and selected query results only. No phone partition or live policy
is changed by these queries.

The actual compiled M2 policy, also reproduced byte-identically on M1, has
SHA256 `4725362524566498a968fe0fd379668ed1b10b85cc01edd212e3cd70e455dc4b`.
The existing libsepol utility from the fork's external source was compiled
against the host libsepol.so.2. This parses a policy in user space and computes
access vectors; it does not load policy into the PC or phone kernel.

```sh
cc -O2 -Irom/external/erofs-utils/src/selinux/libsepol/include \
  rom/external/erofs-utils/src/selinux/libsepol/utils/sepol_check_access.c \
  -Wl,-l:libsepol.so.2 -o /tmp/sepol-check-access
# POLICY is the locally compiled policy file; obtain inputs independently.
/tmp/sepol-check-access "$POLICY" u:r:kernel:s0 \
  u:object_r:unlabeled:s0 file append
```

Query exactly one permission per invocation. Exit0 means the access vector
allows it; exit7 means a computed denial. The selected results are in
`support/evidence/recorder-policy-access.json`.

| Assumed subject | Object | Class / permission | Offline decision |
| --- | --- | --- | --- |
| kernel | unlabeled | file append, write | denied by te-rule |
| kernel | cache_file | file append, write, open | denied by te-rule |
| kernel | tmpfs | chr_file read | denied by te-rule |
| kernel | tmpfs | chr_file write | allowed |
| kernel | kmsg_device | chr_file read, write | allowed |
| init | unlabeled | file append, write | allowed |
| init | cache_file | file append | denied by te-rule |
| init | cache_file | file write, open | allowed |

The private monitor forks before SELinux setup and does not explicitly change
its own SELinux context. Its CACHE output is opened O_APPEND. Recovery inspection
of both saved probe11 and probe12 files reports `u:object_r:unlabeled:s0`.
That is the file xattr seen after the test; it does not measure the process
SID, in-memory inode SID or enforcement state at the moment logging stops.

The pinned kernel revalidates permissions for already-open files after a
policy sequence change. Consequently a surviving monitor in the kernel domain
could lose CACHE append access. This is supported by source and offline policy
queries, but is **not** a captured runtime AVC or a confirmed reset cause.
The policy query alone also does not account for permissive mode or any
runtime labels that have not been measured.

The booted M1 DS-ACK control has no `/dev/pmsg0`, `/sys/class/pmsg` or
`/sys/class/misc/pmsg0`; its pstore directory is empty. Recovery's pmsg support
must not be assumed for this system kernel. A new recorder must establish a
durable error channel through policy activation before another M2 test.

Verification: all 24 single-permission queries complete with exit0 or exit7,
policy SHA matches the two runtime compiler outputs, and recovery label/node
inspection is read-only. The exact access-vector query results are retained;
there was no policy edit, enforcement change or extra reboot for this check.
