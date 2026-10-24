/* Passive M2 control; never bundled by normal builds.
 * PID1 execs original init unchanged. The child only records kmsg to CACHE
 * using private mounts. There is no ptrace request/wait against PID1.
 * Reason: compare SELinux loading with the traced probe11 test, preserving
 * kernel, DTB, cmdline and installed SYSTEM/VENDOR/ODM. No formats.
 */
#define EXYNOS9810_BOOTLOGGER_WRAPPER
#include "bootlogger.c"

static int contains(const char *s, long n, const char *needle)
{
    size_t z = string_length(needle);
    for (long i = 0; i + (long)z <= n; ++i) {
        size_t j = 0;
        while (j < z && s[i + j] == needle[j]) ++j;
        if (j == z) return 1;
    }
    return 0;
}

/* WSTOPSIG semantics: the low byte is the stop marker 0x7f, not the signal. */
static long trace_stop_signal(long status)
{
    return ((unsigned long)status >> 8) & 0xff;
}

static void finish(long code)
{
    syscall6(94, code, 0, 0, 0, 0, 0); /* exit_group */
    for (;;) {}
}

static long sysfs_device(const char *path)
{
    /* Dynamic majors differ between kernels; read the running kernel. */
    long fd = open_file(path, O_RDONLY, 0);
    if (fd < 0) return -1;
    char text[32];
    long n = syscall6(SYS_read, fd, (long)text, 31, 0, 0, 0);
    syscall6(SYS_close, fd, 0, 0, 0, 0, 0);
    unsigned long major = 0, minor = 0;
    long i = 0;
    if (n < 3) return -1;
    while (i < n && text[i] >= '0' && text[i] <= '9')
        major = major * 10 + text[i++] - '0';
    if (i >= n || text[i++] != ':') return -1;
    while (i < n && text[i] >= '0' && text[i] <= '9')
        minor = minor * 10 + text[i++] - '0';
    if (!major || major > 4095 || minor > 1048575) return -1;
    return (minor & 255) | (major << 8) | ((minor & ~255UL) << 12);
}

static long pmsg_open(void)
{
    long fd = open_file("/dev/pmsg0", O_WRONLY | O_NONBLOCK, 0);
    if (fd >= 0) return fd;
    long dev = sysfs_device("/sys/class/pmsg/pmsg0/dev");
    if (dev < 0) return dev;
    syscall6(33, AT_FDCWD, (long)"/dev/m2-pmsg-probe", 020600, dev, 0, 0);
    return open_file("/dev/m2-pmsg-probe", O_WRONLY | O_NONBLOCK, 0);
}

static long cache_open(void)
{
    /* Child-private /dev mountpoint; only the dedicated CACHE file persists. */
    long dev = sysfs_device("/sys/class/block/sda21/dev");
    if (dev < 0) return dev;
    long r = syscall6(33, AT_FDCWD, (long)"/dev/m2-cache-block", 060600, dev, 0, 0);
    if (r < 0 && r != -17) return r;
    r = syscall6(34, AT_FDCWD, (long)"/dev/m2-cache", 0700, 0, 0, 0);
    if (r < 0 && r != -17) return r;
    r = syscall6(40, (long)"/dev/m2-cache-block", (long)"/dev/m2-cache",
                 (long)"ext4", 2 | 4 | 1024, (long)"errors=remount-ro", 0);
    const char *mounted = "/dev/m2-cache";
    if (r == -16) {
        /* Already mounted by Android. Never write into an unmounted tmpfs. */
        mounted = "/cache";
    } else if (r < 0) return r;
    unsigned long statbuf[32] = {0};
    r = syscall6(43, (long)mounted, (long)statbuf, 0, 0, 0, 0);
    if (r < 0) return r;
    if (statbuf[0] != 0xef53) return -22; /* EXT4_SUPER_MAGIC */
    const char *path = (mounted[1] == 'c')
        ? "/cache/m2-early-boot-probe12.log"
        : "/dev/m2-cache/m2-early-boot-probe12.log";
    return open_file(path,
                     O_WRONLY | O_CREAT | O_APPEND, 0600);
}

static long namespace_step;
static char cache_batch[32768];

static long append_decimal(char *dst, long value)
{
    char reversed[24];
    long count = 0;
    do { reversed[count++] = '0' + value % 10; value /= 10; } while (value && count < 24);
    for (long i = 0; i < count; ++i) dst[i] = reversed[count - i - 1];
    return count;
}

static long append_hex(char *dst, unsigned long value)
{
    static const char digits[] = "0123456789abcdef";
    char reversed[2 * sizeof(value)];
    long count = 0;
    do { reversed[count++] = digits[value & 15]; value >>= 4; } while (value && count < (long)sizeof(reversed));
    for (long i = 0; i < count; ++i) dst[i] = reversed[count - i - 1];
    return count;
}

static long tracee_file(long cache, const char *path, long limit)
{
    char buffer[2048];
    long fd = open_file(path, O_RDONLY, 0);
    if (fd < 0) {
        char line[128]; long z=0;
        const char *tag="PROC-READ-FAILED errno=";
        copy_bytes(line+z,tag,string_length(tag));z+=string_length(tag);
        z+=append_decimal(line+z,-fd);line[z++]='\n';write_all(cache,line,z);
        return 0;
    }
    long total = 0;
    while (total < limit) {
        long want = sizeof(buffer);
        if (want > limit - total) want = limit - total;
        long n = syscall6(SYS_read, fd, (long)buffer, want, 0, 0, 0);
        if (n <= 0) break;
        if (write_all(cache, buffer, n) < 0) break;
        total += n;
    }
    syscall6(SYS_close, fd, 0, 0, 0, 0, 0);
    return total;
}

static void trace_regs(long cache, long pid)
{
    struct { unsigned long base; unsigned long len; } iov;
    unsigned long regs[34] = {0};
    iov.base = (unsigned long)regs; iov.len = sizeof(regs);
    long r = syscall6(117, 0x4204, pid, 1, (long)&iov, 0, 0); /* ptrace GETREGSET, NT_PRSTATUS */
    char line[160]; long n = 0;
    const char *tag = "REGSET rc="; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_decimal(line+n, r); tag = " len="; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_decimal(line+n, iov.len); tag = " pc=0x"; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_hex(line+n, regs[32]); tag = " lr=0x"; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_hex(line+n, regs[30]); tag = " sp=0x"; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_hex(line+n, regs[31]); line[n++] = '\n'; write_all(cache, line, n);
}

static void trace_snapshot(long cache, long out, long pid, long status, long event)
{
    char line[192]; long n = 0;
    const char *tag = "\nPID1-STOP pid="; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_decimal(line+n, pid); tag = " status=0x"; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_hex(line+n, (unsigned long)status); tag = " event="; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_decimal(line+n, event); line[n++]='\n'; write_all(cache, line, n); if (out >= 0) write_all(out, line, n);
    unsigned long event_msg = 0;
    syscall6(117, 0x4201, pid, 0, (long)&event_msg, 0, 0); /* PTRACE_GETEVENTMSG */
    n = 0; tag = "PTRACE_EVENTMSG=0x"; copy_bytes(line+n, tag, string_length(tag)); n += string_length(tag);
    n += append_hex(line+n, event_msg); line[n++]='\n'; write_all(cache, line, n);
    tag = "--- /proc/1/cmdline ---\n"; write_all(cache, tag, string_length(tag)); tracee_file(cache, "/proc/1/cmdline", 4096);
    tag = "\n--- /proc/1/maps ---\n"; write_all(cache, tag, string_length(tag)); tracee_file(cache, "/proc/1/maps", 65536);
    const char *extra="\n--- /proc/1/status ---\n";
    write_all(cache,extra,string_length(extra));tracee_file(cache,"/proc/1/status",8192);
    extra="\n--- /proc/1/wchan ---\n";
    write_all(cache,extra,string_length(extra));tracee_file(cache,"/proc/1/wchan",512);
    extra="\n--- /proc/1/task/1/children ---\n";
    write_all(cache,extra,string_length(extra));tracee_file(cache,"/proc/1/task/1/children",1024);
    write_all(cache,"\n",1);
    unsigned long si_words[16]={0};
    long si_rc=syscall6(117,0x4202,pid,0,(long)si_words,0,0); /* GETSIGINFO */
    if (si_rc == 0) {
        int *si=(int *)si_words;
        n=0;tag="SIGNAL_INFO signo=";copy_bytes(line+n,tag,string_length(tag));n+=string_length(tag);
        n+=append_decimal(line+n,si[0]);tag=" code=";copy_bytes(line+n,tag,string_length(tag));n+=string_length(tag);
        if (si[2]<0) {line[n++]='-';n+=append_decimal(line+n,-(long)si[2]);} else n+=append_decimal(line+n,si[2]);
        if (si[0] == 17) {
            tag=" child_pid=";copy_bytes(line+n,tag,string_length(tag));n+=string_length(tag);n+=append_decimal(line+n,si[4]);
            tag=" child_status=";copy_bytes(line+n,tag,string_length(tag));n+=string_length(tag);n+=append_decimal(line+n,si[6]);
        }
        line[n++]='\n';write_all(cache,line,n);
    }
    trace_regs(cache, pid);
    long kmsg = open_file("/dev/kmsg", O_RDONLY | O_NONBLOCK, 0);
    if (kmsg >= 0) {
        char buf[4096];
        for (long i=0; i<256; ++i) { long z=syscall6(SYS_read,kmsg,(long)buf,sizeof(buf),0,0,0); if (z<=0) break; write_all(cache,buf,z); if (out>=0) write_all(out,buf,z); }
        syscall6(SYS_close,kmsg,0,0,0,0,0);
    }
    syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
}

static long monotonic_milliseconds(void)
{
    struct timespec now;
    if (syscall6(113, 1, (long)&now, 0, 0, 0, 0) < 0) return -1;
    return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}
static long monitor_namespace(int cold_sysfs)
{
    /* Child-only mount namespace. PID1's mount table is never changed. */
    namespace_step = 1;
    long r = syscall6(97, 0x20000, 0, 0, 0, 0, 0); /* unshare(CLONE_NEWNS) */
    if (r < 0) return r;
    namespace_step = 2;
    r = syscall6(40, 0, (long)"/", 0, (1UL << 18) | 16384, 0, 0);
    if (r < 0) return r; /* MS_PRIVATE | MS_REC */
    /* The monitor namespace is private before PID1 mounts procfs. */
    r = syscall6(34, AT_FDCWD, (long)"/proc", 0555, 0, 0, 0);
    if (r < 0 && r != -17) return r;
    r = syscall6(40, (long)"proc", (long)"/proc", (long)"proc", 1 | 2 | 4 | 8, 0, 0);
    if (r < 0 && r != -16) return r;
    namespace_step = 3;
    r = syscall6(40, (long)"tmpfs", (long)"/dev", (long)"tmpfs", 2,
                 (long)"mode=0755,size=1m", 0);
    if (r < 0) return r;
    namespace_step = 4;
    if (cold_sysfs) {
        r = syscall6(39, (long)"/sys", 2, 0, 0, 0, 0); /* umount2 MNT_DETACH */
        if (r < 0) return r;
    }
    r = syscall6(40, (long)"sysfs", (long)"/sys", (long)"sysfs", 0, 0, 0);
    if (r < 0 && r != -16) return r; /* EBUSY: inherited sysfs mount */
    /* pmsg is optional on the pinned system kernel; CACHE block sysfs is not. */
    long sysfd = open_file("/sys/class/block/sda21/dev", O_RDONLY, 0);
    if (sysfd < 0) return sysfd;
    syscall6(SYS_close, sysfd, 0, 0, 0, 0, 0);
    syscall6(33, AT_FDCWD, (long)"/dev/kmsg", 020600, (1 << 8) | 11, 0, 0);
    return 0;
}

static void monitor(long exec_pipe, long attach_read, long ack_write)
{
    char buf[4096], pending[8192];
    long used = 0, in = -1, out = -1, cache = -1;
    long batched = 0, records = 0, cache_bytes = 0;
    long start = monotonic_milliseconds(), last_sync = start;
    long exec_result = 0, exec_reported = 0;
    long ns = monitor_namespace(0);
    if (ns < 0) {
        long failed = ns;
        write_all(ack_write, (const char *)&failed, sizeof(failed));
        finish(22);
    }
    cache = cache_open();
    if (cache >= 0) {
        const char *mark = "\nM2-PID1-PROBE12: passive monitor ready\n";
        write_all(cache, mark, string_length(mark));
        syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
    }
    char start_signal;
    if (syscall6(SYS_read, attach_read, (long)&start_signal, 1, 0, 0, 0) != 1) {
        long failed = -5; write_all(ack_write, (const char *)&failed, sizeof(failed)); finish(23);
    }
    /* Passive control: no ptrace operation against PID1 and no wait on it. */
    long attached = 1;
    write_all(ack_write, (const char *)&attached, sizeof(attached));
    syscall6(SYS_close, attach_read, 0, 0, 0, 0, 0);
    syscall6(SYS_close, ack_write, 0, 0, 0, 0, 0);
    if (cache >= 0) {
        const char *msg="PASSIVE-CONTROL ptrace_disabled=1\n";
        write_all(cache,msg,string_length(msg));syscall6(SYS_fsync,cache,0,0,0,0,0);
    }
    for (long tick = 0; tick < 6000; ++tick) {
        if (exec_pipe >= 0 && !exec_result) {
            long result;
            long n = syscall6(SYS_read, exec_pipe, (long)&result, sizeof(result), 0, 0, 0);
            if (n == sizeof(result) && result < 0) exec_result = result;
            else if (n == 0) exec_result = 1; /* CLOEXEC closed writer. */
        }
        if (cache < 0) {
            cache = cache_open();
            if (cache >= 0) {
                const char *mark = "\nM2-PID1-PROBE-v12: exec-report monitor ready\n";
                write_all(cache, mark, string_length(mark));
                syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
            }
        }
        if (cache >= 0 && exec_result && !exec_reported) {
            char msg[128];
            const char *tag = exec_result < 0 ? "M2-INIT-EXEC-FAILED errno=" : "M2-INIT-EXEC-PIPE-EOF cloexec_or_parent_exit=";
            long n = string_length(tag);
            copy_bytes(msg, tag, n);
            n += append_decimal(msg + n, exec_result < 0 ? -exec_result : exec_result);
            msg[n++] = '\n';
            write_all(cache, msg, n);
            syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
            exec_reported = 1;
        }
        if (out < 0) {
            out = pmsg_open();
            if (out >= 0) {
                const char *mark = "\nM2-PID1-PROBE-v2: private monitor ready\n";
                write_all(out, mark, string_length(mark));
                if (used) write_all(out, pending, used);
                used = 0;
            }
        }
        if (in < 0) in = open_file("/dev/kmsg", O_RDONLY | O_NONBLOCK, 0);
        if (in >= 0) {
            for (int batch = 0; batch < 256; ++batch) {
                long n = syscall6(SYS_read, in, (long)buf, sizeof(buf), 0, 0, 0);
                if (n == -32) continue; /* EPIPE: ring overrun, next read recovers */
                if (n <= 0) break;
                ++records;
                if (out >= 0) write_all(out, buf, n);
                if (cache >= 0 && cache_bytes < 8 * 1024 * 1024) {
                    if (batched + n > (long)sizeof(cache_batch)) {
                        write_all(cache, cache_batch, batched);
                        cache_bytes += batched;
                        batched = 0;
                    }
                    copy_bytes(cache_batch + batched, buf, n);
                    batched += n;
                }
                else {
                    if (used + n > (long)sizeof(pending)) used = 0;
                    copy_bytes(pending + used, buf, n); used += n;
                }
            }
        }
        long now = monotonic_milliseconds();
        if (cache >= 0 && now >= 0 && (last_sync < 0 || now - last_sync >= 100)) {
            if (batched) {
                write_all(cache, cache_batch, batched);
                cache_bytes += batched;
                batched = 0;
            }
            char beat[96];
            long n = 0;
            const char *tag = "M2-PROBE-HEARTBEAT ms=";
            copy_bytes(beat, tag, string_length(tag)); n += string_length(tag);
            n += append_decimal(beat + n, now - start);
            tag = " records="; copy_bytes(beat + n, tag, string_length(tag)); n += string_length(tag);
            n += append_decimal(beat + n, records);
            beat[n++] = '\n';
            write_all(cache, beat, n);
            syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
            last_sync = now;
        }
        sleep_milliseconds(10);
    }
    finish(0);
}

void probe_start(unsigned long *stack)
{
    unsigned long argc = stack[0];
    char **argv = (char **)(stack + 1);
    char **env = argv + argc + 1;
    long pid = syscall6(172, 0, 0, 0, 0, 0, 0);
    if (pid != 1) {
        if (argc == 2 && contains(argv[1], string_length(argv[1]), "--signal-forwarding-selftest")) {
            int gate[2] = {-1, -1};
            if (syscall6(59, (long)gate, 0x80000, 0, 0, 0, 0) < 0) finish(60);
            long child = syscall6(220, 17, 0, 0, 0, 0, 0);
            if (child < 0) finish(61);
            if (!child) {
                syscall6(SYS_close, gate[1], 0, 0, 0, 0, 0);
                char go;
                if (syscall6(SYS_read, gate[0], (long)&go, 1, 0, 0, 0) != 1) finish(62);
                long self = syscall6(172,0,0,0,0,0,0);
                syscall6(129,self,17,0,0,0,0); /* SIGCHLD: default action ignores it. */
                finish(0);
            }
            syscall6(SYS_close,gate[0],0,0,0,0,0);
            if (syscall6(117,0x4206,child,0,0,0,0) < 0) finish(63);
            char go='G'; write_all(gate[1],&go,1);
            syscall6(SYS_close,gate[1],0,0,0,0,0);
            int status=0;
            if (syscall6(260,child,(long)&status,0x40000000,0,0,0) != child) finish(64);
            if ((status & 0xff) != 0x7f || trace_stop_signal(status) != 17) finish(65);
            long rejected = syscall6(117,7,child,0,status & 0x7f,0,0);
            if (rejected >= 0) finish(66);
            char line[128]; long n=0;
            const char *tag="OLD-DECODE signal=127 PTRACE_CONT errno=";
            copy_bytes(line+n,tag,string_length(tag));n+=string_length(tag);
            n+=append_decimal(line+n,-rejected);line[n++]='\n';write_all(1,line,n);
            if (syscall6(117,7,child,0,trace_stop_signal(status),0,0) < 0) finish(67);
            if (syscall6(260,child,(long)&status,0x40000000,0,0,0) != child || status) finish(68);
            const char *ok="PROBE-SIGCHLD-FORWARD-AND-EXIT-PASS\n";
            write_all(1,ok,string_length(ok));finish(0);
        }
        if (argc == 2 && contains(argv[1], string_length(argv[1]), "--ptrace-selftest")) {
            int gate[2] = {-1, -1};
            if (syscall6(59, (long)gate, 0x80000, 0, 0, 0, 0) < 0) finish(40);
            long child = syscall6(220, 17, 0, 0, 0, 0, 0);
            if (child < 0) finish(41);
            if (!child) {
                syscall6(SYS_close, gate[1], 0, 0, 0, 0, 0);
                char go;
                if (syscall6(SYS_read, gate[0], (long)&go, 1, 0, 0, 0) != 1) finish(42);
                long r = syscall6(117, 0x4206, pid, 0, 0x10 | 0x40 | 0x1, 0, 0);
                if (r < 0) finish(43);
                r = syscall6(117, 0x4207, pid, 0, 0, 0, 0);
                if (r < 0) finish(44);
                int status = 0;
                if (syscall6(260, pid, (long)&status, 0x40000000, 0, 0, 0) != pid || (status & 0xff) != 0x7f) finish(45);
                if (syscall6(117, 7, pid, 0, 0, 0, 0) < 0) finish(46);
                finish(0);
            }
            syscall6(SYS_close, gate[0], 0, 0, 0, 0, 0);
            long r = syscall6(167, 0x59616d61, child, 0, 0, 0, 0); /* PR_SET_PTRACER */
            char go = 'G';
            write_all(gate[1], &go, 1);
            syscall6(SYS_close, gate[1], 0, 0, 0, 0, 0);
            int status = -1;
            long waited = syscall6(260, child, (long)&status, 0, 0, 0, 0);
            if (waited != child || status) {
                char result[96]; long n=0; const char *tag="PTRACER_EXIT waited=";
                copy_bytes(result+n,tag,string_length(tag)); n+=string_length(tag); n+=append_decimal(result+n,waited);
                tag=" status="; copy_bytes(result+n,tag,string_length(tag)); n+=string_length(tag); n+=append_decimal(result+n,status);
                tag=" prctl="; copy_bytes(result+n,tag,string_length(tag)); n+=string_length(tag); n+=append_decimal(result+n,r);
                result[n++]='\n'; write_all(1,result,n); finish(47);
            }
            const char *ok = "PROBE-PTRACE-SEIZE-INTERRUPT-CONT-PASS\n";
            write_all(1, ok, string_length(ok));
            finish(0);
        }
        if (argc == 2 && contains(argv[1], string_length(argv[1]), "--exec-report-test")) {
            int channel[2];
            if (syscall6(59, (long)channel, 0x80000, 0, 0, 0, 0) < 0) finish(30);
            long child = syscall6(220, 17, 0, 0, 0, 0, 0);
            if (child < 0) finish(31);
            if (!child) {
                syscall6(SYS_close, channel[1], 0, 0, 0, 0, 0);
                long err = 0;
                long n = syscall6(SYS_read, channel[0], (long)&err, sizeof(err), 0, 0, 0);
                finish(n == sizeof(err) && err == -2 ? 0 : 32);
            }
            syscall6(SYS_close, channel[0], 0, 0, 0, 0, 0);
            long err = syscall6(221, (long)"/__m2_probe_nonexistent_init__", (long)argv, (long)env, 0, 0, 0);
            write_all(channel[1], (const char *)&err, sizeof(err));
            int status = -1;
            if (syscall6(260, child, (long)&status, 0, 0, 0, 0) != child || status) finish(33);
            const char *ok = "PROBE-EXEC-ENOENT-PIPE-PASS\n";
            write_all(1, ok, string_length(ok));
            finish(0);
        }
        if (argc == 2 && (contains(argv[1], string_length(argv[1]), "--namespace-test") ||
                          contains(argv[1], string_length(argv[1]), "--namespace-cold-test") ||
                          contains(argv[1], string_length(argv[1]), "--pmsg-write-test") ||
                          contains(argv[1], string_length(argv[1]), "--cache-write-test"))) {
            int cold = contains(argv[1], string_length(argv[1]), "--namespace-cold-test");
            int write_test = contains(argv[1], string_length(argv[1]), "--pmsg-write-test");
            int cache_test = contains(argv[1], string_length(argv[1]), "--cache-write-test");
            long r = monitor_namespace(cold);
            if (r < 0) {
                char msg[] = "NAMESPACE stage=0 errno=000\n";
                msg[16] = '0' + namespace_step;
                long err = -r;
                msg[26] = '0' + err % 10;
                msg[25] = '0' + (err / 10) % 10;
                msg[24] = '0' + (err / 100) % 10;
                write_all(1, msg, sizeof(msg) - 1);
                finish(20);
            }
            long in = open_file("/dev/kmsg", O_RDONLY | O_NONBLOCK, 0);
            long out = pmsg_open();
            if (in < 0 || (write_test && out < 0)) finish(21);
            if (cache_test) {
                long cache = cache_open();
                if (cache < 0) finish(24);
                const char *mark = "M2-CACHE-RAM-SELFTEST-20261022-PROBE05\n";
                if (write_all(cache, mark, string_length(mark)) < 0) finish(25);
                syscall6(SYS_fsync, cache, 0, 0, 0, 0, 0);
                syscall6(SYS_close, cache, 0, 0, 0, 0, 0);
                const char *written = "PROBE-CACHE-WRITE-PASS\n";
                write_all(1, written, string_length(written));
                finish(0);
            }
            if (write_test) {
                const char *mark = "M2-PMSG-RAM-SELFTEST-20261022-PROBE04\n";
                if (write_all(out, mark, string_length(mark)) < 0) finish(23);
                syscall6(SYS_close, out, 0, 0, 0, 0, 0);
                const char *written = "PROBE-PMSG-WRITE-PASS\n";
                write_all(1, written, string_length(written));
                finish(0);
            }
            if (tracee_file(1,"/proc/1/status",8192) <= 0) finish(72);
            const char *proc_ok="\nPROBE-PRIVATE-PROC-READ-PASS\n";
            write_all(1,proc_ok,string_length(proc_ok));
            const char *ok = "PROBE-PRIVATE-TMPFS-SYSFS-KMSG-OPEN-PASS\n";
            write_all(1, ok, string_length(ok));
            finish(0); /* No pmsg write in self-test. Mounts vanish at exit. */
        }
        if (argc == 2 && contains(argv[1], string_length(argv[1]), "--self-test")) {
            long child = syscall6(220, 17, 0, 0, 0, 0, 0);
            if (child < 0) finish(10);
            if (!child) finish(0);
            int status = -1;
            if (syscall6(260, child, (long)&status, 0, 0, 0, 0) != child || status) finish(11);
            char *test[] = {"/sbin/sh", "-c", "test \"$M2_PROBE_TEST\" = env-preserved && echo PROBE-FORK-WAIT-EXEC-ENV-PASS", 0};
            syscall6(221, (long)test[0], (long)test, (long)env, 0, 0, 0);
            finish(12);
        }
        finish(13); /* never execute init from a non-PID1 invocation */
    }
    int exec_pipe[2] = {-1, -1};
    int attach_cmd[2] = {-1, -1}, attach_ack[2] = {-1, -1};
    syscall6(59, (long)exec_pipe, 0x80000 | O_NONBLOCK, 0, 0, 0, 0); /* pipe2 CLOEXEC */
    syscall6(59, (long)attach_cmd, 0x80000, 0, 0, 0, 0);
    syscall6(59, (long)attach_ack, 0x80000, 0, 0, 0, 0);
    long child = syscall6(220, 17, 0, 0, 0, 0, 0);
    if (!child) {
        if (exec_pipe[1] >= 0) syscall6(SYS_close, exec_pipe[1], 0, 0, 0, 0, 0);
        if (attach_cmd[1] >= 0) syscall6(SYS_close, attach_cmd[1], 0, 0, 0, 0, 0);
        if (attach_ack[0] >= 0) syscall6(SYS_close, attach_ack[0], 0, 0, 0, 0, 0);
        monitor(exec_pipe[0], attach_cmd[0], attach_ack[1]);
    }
    if (exec_pipe[0] >= 0) syscall6(SYS_close, exec_pipe[0], 0, 0, 0, 0, 0);
    if (attach_cmd[0] >= 0) syscall6(SYS_close, attach_cmd[0], 0, 0, 0, 0, 0);
    if (attach_ack[1] >= 0) syscall6(SYS_close, attach_ack[1], 0, 0, 0, 0, 0);
    long pr = 0; /* Passive PID1 path does not grant or request tracing. */
    char go = 'G';
    if (attach_cmd[1] >= 0) write_all(attach_cmd[1], &go, 1);
    long attach_result = -1;
    if (attach_ack[0] >= 0) syscall6(SYS_read, attach_ack[0], (long)&attach_result, sizeof(attach_result), 0, 0, 0);
    if (pr < 0 && attach_result == 0) attach_result = pr;
    if (attach_cmd[1] >= 0) syscall6(SYS_close, attach_cmd[1], 0, 0, 0, 0, 0);
    if (attach_ack[0] >= 0) syscall6(SYS_close, attach_ack[0], 0, 0, 0, 0, 0);
    /* Even clone failure must not prevent original init. No wait in PID1. */
    long exec_error = syscall6(221, (long)"/init.real", (long)argv, (long)env, 0, 0, 0);
    if (exec_pipe[1] >= 0 && child > 0) {
        write_all(exec_pipe[1], (const char *)&exec_error, sizeof(exec_error));
        sleep_milliseconds(1000); /* Failure only: allow child to persist errno. */
    }
    long out = pmsg_open();
    if (out >= 0) {
        const char *err = "\nM2-PID1-PROBE-v2: exec /init.real FAILED\n";
        write_all(out, err, string_length(err));
    }
    finish(127);
}

__asm__(".global _start\n_start:\nmov x0, sp\nbl probe_start\n");
