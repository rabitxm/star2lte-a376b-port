#!/usr/bin/env bash
# Reason: reproduce the existing fork plus the reviewed patch series for helpers.
# Files: a new checkout only; no firmware downloads, builds or device operations.
# Verify: pinned HEAD and sequential GNU patch --dry-run before every patch.
set -euo pipefail
project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
fork_dest="${1:-$project_root/rom}"
fork_revision=2b25ae6fc3098650be249bd1898e21e563cd5df1
if [[ -e "$fork_dest" ]]; then
    printf 'Refusing to overwrite existing path: %s\n' "$fork_dest" >&2
    exit 1
fi
GIT_LFS_SKIP_SMUDGE=1 git clone --no-checkout \
    https://github.com/Eend15/Unofficial-Un1ca-9810.git "$fork_dest"
GIT_LFS_SKIP_SMUDGE=1 git -C "$fork_dest" checkout --detach "$fork_revision"
[[ "$(git -C "$fork_dest" rev-parse HEAD)" == "$fork_revision" ]]
while IFS= read -r patch_name || [[ -n "$patch_name" ]]; do
    [[ -z "$patch_name" || "$patch_name" == \#* ]] && continue
    patch --dry-run --batch --forward -d "$fork_dest" -p1 -i "$project_root/patches/fork/$patch_name"
    patch --batch --forward -d "$fork_dest" -p1 -i "$project_root/patches/fork/$patch_name"
done < "$project_root/patches/fork/series"
printf 'Pinned fork and patch series prepared at %s\n' "$fork_dest"
printf 'See docs/build-environment.md for dependencies and firmware inputs. No build or flash has run.\n'
