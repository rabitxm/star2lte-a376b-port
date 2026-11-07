#!/usr/bin/env bash
# Reason: retained releases live on the existing second ext4 NVMe filesystem.
# Files affected: mount state only; never format or partition a disk.
# Verify: UUID, filesystem type, expected mount path and writable project root.
set -euo pipefail
release_uuid=a26834a0-9fe6-4668-8896-c87a532862d9
release_mount="/run/media/$(id -un)/$release_uuid"
if ! findmnt -rn -S "UUID=$release_uuid" > /dev/null; then
    udisksctl mount -b "/dev/disk/by-uuid/$release_uuid"
fi
[[ "$(findmnt -rn -S "UUID=$release_uuid" -o TARGET)" == "$release_mount" ]]
[[ "$(findmnt -rn -S "UUID=$release_uuid" -o FSTYPE)" == ext4 ]]
[[ -d "$release_mount/star2lte-port" && -w "$release_mount/star2lte-port" ]]
printf 'Release storage ready: %s\n' "$release_mount/star2lte-port"
