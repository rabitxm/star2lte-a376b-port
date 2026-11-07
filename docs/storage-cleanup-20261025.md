# Project storage cleanup and second NVMe — 25 October 2026

Reason: the system filesystem was 94% full (about 30 GiB available), while the
project occupied about 373 GiB. The user authorized deletion of redundant and
obsolete project outputs and use of the empty 256 GB M.2 drive.

The KIOXIA drive already has one ext4 partition, UUID
`a26834a0-9fe6-4668-8896-c87a532862d9`. It was mounted with UDisks, without
formatting or partition changes. The active build work tree, extracted build
inputs, source repositories, keys, hardware backups and original runtime logs
stay on the system drive.

Cleanup records are private under `logs/20261025-cleanup/`:

- Ten obsolete top-level `rom/out` M1 ZIP copies were SHA256/size checked
  against the independent M1 archive, removed and replaced with relative
  symlinks. The archive's files remain independent and unchanged. This removes
  16,888,429,178 duplicate bytes while preserving old paths.
- `inputs/donor-a376b/super.img` and `super.raw.img` are reproducible intermediate
  images. Their allocated space was 23,164,952,576 bytes. They were removed only
  after verifying the original Android17 ZIP SHA256
  `b152ca2c87c1b54aee6f606f436f52076633e894a7dea330ee1da87f5117b415`.
  The original ZIP, Odin AP, logical partition images, selected artifacts and
  provenance remain available for M4.
- Two abandoned `.git/objects/pack/tmp_pack_*` files were reported by Git as
  garbage. No pack process was active. Only those two temporary files were
  removed (777,535,488 allocated bytes); repository objects/history were kept.

The retained release directory was copied and verified at
`/run/media/whitemmo/a26834a0-9fe6-4668-8896-c87a532862d9/star2lte-port/releases`.
The whole source/destination tree passed SHA1/metadata comparison and all ten
retained SHA256 manifests passed. The old `rom/out/releases` path is now a
verified symlink; only the redundant source copy was removed. Do not
change release bytes, remove complete releases, or deduplicate the independent
M1 archive against its release copies.

After a PC reboot, run `bash tools/mount-release-storage.sh` before packaging or
reading releases. It verifies the exact UUID, ext4 and expected mount location;
it cannot format a disk. A missing mount leaves the release symlink dangling,
so the existing fork's directory creation fails instead of silently storing
packages on the wrong filesystem. No root fstab change was made.

Rollback of relocation: mount the second drive, copy releases back to a separate
directory on the system drive, verify all bytes/checksums, then replace the
symlink with that verified directory. Do not delete the only remaining release
copy. No phone changes are part of storage cleanup.


Completed result: 16 duplicate/regenerable/temporary files removed, about
38.51 GiB reclaimed, and 136,547,395,090 release bytes relocated. System
filesystem usage changed from94% /30GiB available to57% /195GiB available;
KIOXIA has about95GiB available before attempt55 packaging. All ten old
complete release runs remain. Two identical Smart Suggestions audit APK copies
were replaced with relative symlinks to the SHA256-verified original APK
(520,398,382 bytes reclaimed). The active compile tree was regenerated through
the fork's existing clean-work-dir option. No firmware source ZIP, independent
M1 archive, hardware backup, raw diagnostic log or source history was deleted.

The sequential stored-SHA256 phase was intentionally stopped after three
manifest passes, then only the seven unchecked manifests were resumed with
eight independent workers. The completed whole-tree comparison and successful
results were preserved; no relocation/deletion happened until all ten passed.
See verification-resume-note.txt and release-sha256-validation.json in the
private cleanup evidence directory.


After completed attempt55 ROM, target-files and eight retained stages: system
filesystem57% /195GiB available, second NVMe65% /79GiB available (df -BG).
This is after the new package, not the pre-package95GiB second-drive figure.
