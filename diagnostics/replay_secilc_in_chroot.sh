#!/sbin/sh
# Reason: reproduce init's secilc invocation against installed CIL, without policy load.
# Requires the deliberately prepared read-only /tmp/m2-secilc/root chroot and
# a tmpfs output directory bind-mounted at root/debug_ramdisk.
# Verification: capture argv/stdout/stderr/status/output hash, then remove mounts.
ROOT=/tmp/m2-secilc/root
OUT=/tmp/m2-secilc/out
for partition in "$ROOT" "$ROOT/vendor" "$ROOT/odm"; do
    awk -v target="$partition" '$2==target && $4 ~ /^ro,/ {ok=1} END {exit !ok}' /proc/mounts || exit 70
done
[ -f "$ROOT/system/bin/secilc" ] && [ -d "$OUT" ] || exit 71
VERSION=$(cat "$ROOT/vendor/etc/selinux/plat_sepolicy_vers.txt")
set -- /system/bin/secilc /system/etc/selinux/plat_sepolicy.cil -m -M true -G -N -c 30 /system/etc/selinux/mapping/$VERSION.cil -o /debug_ramdisk/sepolicy -f /debug_ramdisk/file_contexts
for spec in \
 system/etc/selinux/mapping/$VERSION.compat.cil:/system/etc/selinux/mapping/$VERSION.compat.cil \
 system/system_ext/etc/selinux/system_ext_sepolicy.cil:/system_ext/etc/selinux/system_ext_sepolicy.cil \
 system/system_ext/etc/selinux/mapping/$VERSION.cil:/system_ext/etc/selinux/mapping/$VERSION.cil \
 system/system_ext/etc/selinux/mapping/$VERSION.compat.cil:/system_ext/etc/selinux/mapping/$VERSION.compat.cil \
 product/etc/selinux/product_sepolicy.cil:/product/etc/selinux/product_sepolicy.cil \
 product/etc/selinux/mapping/$VERSION.cil:/product/etc/selinux/mapping/$VERSION.cil \
 vendor/etc/selinux/plat_pub_versioned.cil:/vendor/etc/selinux/plat_pub_versioned.cil \
 vendor/etc/selinux/vendor_sepolicy.cil:/vendor/etc/selinux/vendor_sepolicy.cil \
 odm/etc/selinux/odm_sepolicy.cil:/odm/etc/selinux/odm_sepolicy.cil \
 system/etc/selinux/plat_sepolicy_genfs_202404.cil:/system/etc/selinux/plat_sepolicy_genfs_202404.cil; do
    disk=${spec%%:*}
    arg=${spec#*:}
    if [ -f "$ROOT/$disk" ]; then set -- "$@" "$arg"; fi
done
printf '%s\n' "$@" > "$OUT/argv.txt"
cat /proc/uptime > "$OUT/uptime-start.txt"
timeout -t 90 -s KILL chroot "$ROOT" "$@" > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
RC=$?
printf '%s\n' "$RC" > "$OUT/exit-status.txt"
cat /proc/uptime > "$OUT/uptime-end.txt"
if [ -f "$OUT/sepolicy" ]; then wc -c "$OUT/sepolicy" > "$OUT/output-size.txt"; sha256sum "$OUT/sepolicy" > "$OUT/output-sha256.txt"; fi
cat "$OUT/exit-status.txt" "$OUT/stderr.txt"
exit 0
