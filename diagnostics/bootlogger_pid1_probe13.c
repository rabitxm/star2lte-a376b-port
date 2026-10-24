/* Opt-in passive recorder: preallocated mmap CACHE channel across policy load.
 * Reason: probe11/12 ignored VFS write errors and lost proc path visibility.
 * Scope: diagnostic ramdisk + regular CACHE files only. No policy/enforcement,
 * kernel, vendor, radio or raw CACHE sector changes. Original init is execed.
 * Verify: real AArch64 namespace/FD/error/persistence tests, then M1 calibration
 * before M2; preserve kernel/DTB/cmdline and roll BOOT back after each test.
 */
#define EXYNOS9810_BOOTLOGGER_WRAPPER
#include "bootlogger.c"

#define O_RDWR 2
/* arm64/include/uapi/asm/fcntl.h overrides the asm-generic directory bit. */
#define O_DIRECTORY 040000
#define SLOT_SIZE (1024UL * 1024)
#define SLOT_COUNT 32
#define MAP_SIZE (4096UL + SLOT_SIZE * SLOT_COUNT)
#define SLOT_MAGIC 0x31544f4c53333150UL
#define FRAME_MAGIC 0x1331cafeUL

struct slot_header {
    unsigned long magic, generation, used, frames;
    long msync_rc, fsync_rc, vfs_rc, kmsg_rc, label_rc;
    unsigned long dropped, filtered, heartbeat_ms;
};
struct frame_header { unsigned long magic, length, ms, checksum; };
static char *mapping;
static struct slot_header *slot;
static long mapped_fd = -1, proc_root = -1, namespace_step;
static unsigned long sync_start = 4096, start_ms;

static void finish(long code) { syscall6(94,code,0,0,0,0,0); for (;;) {} }
static int equal(const char *a,const char *b) {
    while (*a && *a==*b) {++a;++b;} return *a==*b;
}
static int contains(const char *s,long n,const char *needle) {
    long z=string_length(needle);
    for(long i=0;i+z<=n;++i){long j=0;while(j<z && s[i+j]==needle[j])++j;if(j==z)return 1;}
    return 0;
}
static long number(char *dst,long v) {
    char r[24];long n=0,z=0;unsigned long u;
    if(v<0){dst[z++]='-';u=0-(unsigned long)v;}else u=v;
    do{r[n++]='0'+u%10;u/=10;}while(u);
    while(n)dst[z++]=r[--n];return z;
}
static long now_ms(void) {
    struct timespec t;if(syscall6(113,1,(long)&t,0,0,0,0)<0)return -1;
    return t.tv_sec*1000+t.tv_nsec/1000000;
}
static unsigned long checksum(const char *p,unsigned long n) {
    unsigned long h=14695981039346656037UL;
    for(unsigned long i=0;i<n;++i){h^=(unsigned char)p[i];h*=1099511628211UL;}return h;
}
static void barrier(void){__asm__ volatile("dmb ish" ::: "memory");}
static long mapped_emit(const char *p,unsigned long n) {
    if(!slot)return -9;
    unsigned long full=(sizeof(struct frame_header)+n+7)&~7UL;
    if(n>65536 || slot->used+full>SLOT_SIZE){++slot->dropped;return -28;}
    struct frame_header h={FRAME_MAGIC,n,now_ms()-start_ms,checksum(p,n)};
    char *out=(char *)slot+slot->used;
    copy_bytes(out,(const char *)&h,sizeof(h));copy_bytes(out+sizeof(h),p,n);
    barrier();slot->used+=full;++slot->frames;return 0;
}
static void event(const char *tag,long rc) {
    char b[192];long n=string_length(tag);if(n>140)n=140;
    copy_bytes(b,tag,n);b[n++]='=';n+=number(b+n,rc);b[n++]='\n';mapped_emit(b,n);
}
static long mapped_sync(void) {
    if(!slot)return -9;
    unsigned long end=(slot->used+4095)&~4095UL;
    if(sync_start>end)sync_start=4096;
    long r=0;
    if(end>sync_start)r=syscall6(227,(long)((char *)slot+sync_start),end-sync_start,4,0,0,0);
    slot->msync_rc=r;
    slot->fsync_rc=syscall6(SYS_fsync,mapped_fd,0,0,0,0,0);
    barrier();long h=syscall6(227,(long)slot,4096,4,0,0,0);
    /* Retain the final partial page in the next sync range. */
    if(r==0 && h==0)sync_start=slot->used&~4095UL;
    return r<0?r:(h<0?h:slot->fsync_rc);
}
static long mapped_open(const char *path) {
    mapped_fd=open_file(path,O_RDWR|O_CREAT,0600);if(mapped_fd<0)return mapped_fd;
    long size=syscall6(62,mapped_fd,0,2,0,0,0);
    if(size!=0 && size!=MAP_SIZE)return -22;
    if(size==0){long r=syscall6(46,mapped_fd,MAP_SIZE,0,0,0,0);if(r<0)return r;}
    long r=syscall6(47,mapped_fd,0,0,MAP_SIZE,0,0);if(r<0)return r;
    r=syscall6(SYS_fsync,mapped_fd,0,0,0,0,0);if(r<0)return r;
    r=syscall6(222,0,MAP_SIZE,3,1,mapped_fd,0);if(r<0 && r>=-4095)return r;
    mapping=(char *)r;unsigned long latest=0;
    for(unsigned long i=0;i<SLOT_COUNT;++i){
        struct slot_header *s=(void *)(mapping+4096+i*SLOT_SIZE);
        if(s->magic==SLOT_MAGIC && s->generation>latest && s->used>=4096 && s->used<=SLOT_SIZE)
            latest=s->generation;
    }
    if(latest==~0UL)return -75;
    unsigned long generation=latest+1;
    slot=(void *)(mapping+4096+((generation-1)%SLOT_COUNT)*SLOT_SIZE);
    for(unsigned long i=0;i<4096;++i)((char *)slot)[i]=0;
    slot->generation=generation;slot->used=4096;barrier();slot->magic=SLOT_MAGIC;
    start_ms=now_ms();sync_start=4096;
    return mapped_sync();
}
static long sysfs_device(const char *path) {
    long fd=open_file(path,O_RDONLY,0);if(fd<0)return fd;
    char text[32];long n=syscall6(SYS_read,fd,(long)text,31,0,0,0);
    syscall6(SYS_close,fd,0,0,0,0,0);unsigned long major=0,minor=0;long i=0;
    if(n<3)return -22;
    while(i<n && text[i]>='0' && text[i]<='9')major=major*10+text[i++]-'0';
    if(i>=n || text[i++]!=':')return -22;
    while(i<n && text[i]>='0' && text[i]<='9')minor=minor*10+text[i++]-'0';
    if(!major || major>4095 || minor>1048575)return -22;
    return (minor&255)|(major<<8)|((minor&~255UL)<<12);
}
static long private_namespace(int cold) {
    namespace_step=1;long r=syscall6(97,0x20000,0,0,0,0,0);if(r<0)return r;
    namespace_step=2;r=syscall6(40,0,(long)"/",0,(1UL<<18)|16384,0,0);if(r<0)return r;
    namespace_step=3;r=syscall6(34,AT_FDCWD,(long)"/proc",0555,0,0,0);
    if(r<0 && r!=-17)return r;
    r=syscall6(40,(long)"proc",(long)"/proc",(long)"proc",1|2|4|8,0,0);
    if(r<0 && r!=-16)return r;
    proc_root=open_file("/proc",O_RDONLY|O_DIRECTORY,0);if(proc_root<0)return proc_root;
    namespace_step=4;r=syscall6(40,(long)"tmpfs",(long)"/dev",(long)"tmpfs",2,(long)"mode=0755,size=1m",0);
    if(r<0)return r;
    namespace_step=5;
    if(cold){r=syscall6(39,(long)"/sys",2,0,0,0,0);if(r<0)return r;}
    r=syscall6(40,(long)"sysfs",(long)"/sys",(long)"sysfs",0,0,0);if(r<0 && r!=-16)return r;
    namespace_step=6;r=syscall6(33,AT_FDCWD,(long)"/dev/kmsg",020600,(1<<8)|11,0,0);
    return r<0 && r!=-17?r:0;
}
static long cache_prepare(void) {
    long dev=sysfs_device("/sys/class/block/sda21/dev");if(dev<0)return dev;
    long r=syscall6(33,AT_FDCWD,(long)"/dev/probe13-cache-block",060600,dev,0,0);
    if(r<0 && r!=-17)return r;
    r=syscall6(34,AT_FDCWD,(long)"/dev/probe13-cache",0700,0,0,0);if(r<0 && r!=-17)return r;
    r=syscall6(40,(long)"/dev/probe13-cache-block",(long)"/dev/probe13-cache",(long)"ext4",2|4|1024,(long)"errors=remount-ro",0);
    const char *base=r==-16?"/cache":"/dev/probe13-cache";
    if(r<0 && r!=-16)return r;
    unsigned long statbuf[32];r=syscall6(43,(long)base,(long)statbuf,0,0,0,0);
    if(r<0 || statbuf[0]!=0xef53)return r<0?r:-22;
    const char *path=base[1]=='c'?"/cache/m2-early-boot-probe13.mmap":"/dev/probe13-cache/m2-early-boot-probe13.mmap";
    return mapped_open(path);
}
static long read_proc(const char *relative,char *buf,long len) {
    long fd=syscall6(SYS_openat,proc_root,(long)relative,O_RDONLY,0,0,0);
    if(fd<0)return fd;long n=syscall6(SYS_read,fd,(long)buf,len,0,0,0);
    syscall6(SYS_close,fd,0,0,0,0,0);return n;
}
static void proc_record(const char *relative) {
    char buf[2048];long n=read_proc(relative,buf,sizeof(buf));
    event(relative,n);if(n>0)mapped_emit(buf,n);
}
static void fd_label(const char *tag,long fd) {
    char buf[256];long n=syscall6(10,fd,(long)"security.selinux",(long)buf,sizeof(buf),0,0);
    event(tag,n);if(n>0)mapped_emit(buf,n);
}
static void monitor(long exec_pipe,long ack) {
    long r=private_namespace(0);if(r>=0)r=cache_prepare();
    if(r<0){write_all(ack,(const char *)&r,sizeof(r));finish(22);}
    const char *mark="PROBE13 MMAP PASSIVE kernel/dtb/cmdline unchanged\n";
    mapped_emit(mark,string_length(mark));event("proc_root_fd",proc_root);
    long in=open_file("/dev/kmsg",O_RDONLY|O_NONBLOCK,0);event("kmsg_open",in);
    long regular=open_file("/dev/probe13-cache/m2-early-boot-probe13-vfs.log",O_WRONLY|O_CREAT|O_APPEND,0600);
    if(regular<0)regular=open_file("/cache/m2-early-boot-probe13-vfs.log",O_WRONLY|O_CREAT|O_APPEND,0600);
    event("vfs_open",regular);fd_label("cache_label",mapped_fd);fd_label("kmsg_label",in);
    proc_record("self/attr/current");proc_record("1/comm");mapped_sync();
    r=1;write_all(ack,(const char *)&r,sizeof(r));syscall6(SYS_close,ack,0,0,0,0,0);
    long prev_read=0,prev_write=999,prev_sync=999,last=now_ms(),last_proc=last;
    int exec_done=0,relabel_attempted=0;
    char buf[4096];
    for(long tick=0;tick<12000;++tick){
        if(!exec_done){long err=0;long n=syscall6(SYS_read,exec_pipe,(long)&err,sizeof(err),0,0,0);
            if(n==0 || n==sizeof(err)){event(n==0?"INIT_EXEC_PIPE_EOF":"INIT_EXEC_ERROR",err);exec_done=1;}}
        if(in>=0)for(int batch=0;batch<256;++batch){
            long n=syscall6(SYS_read,in,(long)buf,sizeof(buf),0,0,0);slot->kmsg_rc=n;
            if(n>0){
                /* Reserve space for status/error evidence during a long M1 boot. */
                if(slot->used<512*1024 || contains(buf,n,"init:") || contains(buf,n,"avc:") ||
                   contains(buf,n,"SELinux:") || contains(buf,n,"Fatal") || contains(buf,n,"panic") ||
                   contains(buf,n,"reboot"))mapped_emit(buf,n);else ++slot->filtered;
                continue;
            }
            if(n!=-11 && n!=prev_read){event("KMSG_READ_ERROR",n);prev_read=n;}
            if(n==-32)continue;
            if((n==-13 || n==-1) && !relabel_attempted){
                /* Correct only this child-private kmsg inode, no policy edit. */
                relabel_attempted=1;fd_label("kmsg_before_relabel",in);
                const char *ctx="u:object_r:kmsg_device:s0";
                slot->label_rc=syscall6(7,in,(long)"security.selinux",(long)ctx,string_length(ctx)+1,0,0);
                event("PRIVATE_KMSG_RELABEL",slot->label_rc);fd_label("kmsg_after_relabel",in);
            }
            break;
        }
        long now=now_ms();
        if(now-last>=100){
            const char *beat="PROBE13 VFS CHECK\n";
            long w=write_all(regular,beat,string_length(beat));slot->vfs_rc=w;
            if(w!=prev_write){event("VFS_WRITE_RC",w);prev_write=w;}
            long f=syscall6(SYS_fsync,regular,0,0,0,0,0);
            if(f!=prev_sync){event("VFS_FSYNC_RC",f);prev_sync=f;}
            slot->heartbeat_ms=now-start_ms;
            event("HEARTBEAT_MS",now-start_ms);mapped_sync();last=now;
        }
        if(now-last_proc>=1000){proc_record("self/attr/current");proc_record("1/comm");
            proc_record("1/wchan");fd_label("cache_label",mapped_fd);last_proc=now;}
        sleep_milliseconds(10);
    }
    event("MONITOR_COMPLETE",0);mapped_sync();finish(0);
}
void probe_start(unsigned long *stack) {
    unsigned long argc=stack[0];char **argv=(char **)(stack+1),**env=argv+argc+1;
    long pid=syscall6(172,0,0,0,0,0,0);
    if(pid!=1){
        if(argc==2 && (equal(argv[1],"--namespace-test") || equal(argv[1],"--cold-namespace-test"))){
            long r=private_namespace(equal(argv[1],"--cold-namespace-test"));
            if(r<0){char b[128];long n=number(b,namespace_step);b[n++]=' ';n+=number(b+n,r);b[n++]='\n';write_all(1,b,n);finish(31);}
            char b[1024];if(read_proc("self/status",b,sizeof(b))<=0)finish(32);
            r=syscall6(39,(long)"/proc",2,0,0,0,0);if(r<0)finish(33);
            long fd=open_file("/proc/self/status",O_RDONLY,0);
            if(fd>=0)finish(34);
            if(read_proc("self/status",b,sizeof(b))<=0)finish(35);
            const char *ok="PROBE13 PROC_FD_SURVIVES_DETACH PASS\n";write_all(1,ok,string_length(ok));finish(0);
        }
        if(argc==3 && equal(argv[1],"--sink-test")){
            if(mapped_open(argv[2])<0)finish(40);
            const char *mark="PROBE13 MMAP SELFTEST BEFORE_FD_CLOSE\n";mapped_emit(mark,string_length(mark));
            if(mapped_sync()<0)finish(41);
            long fd=mapped_fd;syscall6(SYS_close,fd,0,0,0,0,0);
            long w=write_all(fd,"x",1),f=syscall6(SYS_fsync,fd,0,0,0,0,0);
            if(w!=-9 || f!=-9)finish(42);
            event("EXPECTED_VFS_EBADF",w);event("EXPECTED_FSYNC_EBADF",f);
            mark="PROBE13 MMAP SELFTEST AFTER_FD_CLOSE\n";mapped_emit(mark,string_length(mark));
            mapped_fd=-1;long r=mapped_sync();
            if(r!=-9 || slot->msync_rc!=0)finish(43);
            const char *ok="PROBE13 MMAP_PERSISTS_AFTER_CLOSE PASS\n";write_all(1,ok,string_length(ok));finish(0);
        }
        finish(13);
    }
    int p[2]={-1,-1},ack[2]={-1,-1};
    long r=syscall6(59,(long)p,0x80000|O_NONBLOCK,0,0,0,0);
    long a=syscall6(59,(long)ack,0x80000,0,0,0,0);long child=-1;
    if(r>=0 && a>=0)child=syscall6(220,17,0,0,0,0,0);
    if(child==0){syscall6(SYS_close,p[1],0,0,0,0,0);syscall6(SYS_close,ack[0],0,0,0,0,0);monitor(p[0],ack[1]);}
    if(child>0){syscall6(SYS_close,p[0],0,0,0,0,0);syscall6(SYS_close,ack[1],0,0,0,0,0);
        long ready=-1;syscall6(SYS_read,ack[0],(long)&ready,sizeof(ready),0,0,0);}
    if(ack[0]>=0)syscall6(SYS_close,ack[0],0,0,0,0,0);
    if(ack[1]>=0)syscall6(SYS_close,ack[1],0,0,0,0,0);
    long err=syscall6(221,(long)"/init.real",(long)argv,(long)env,0,0,0);
    if(child>0){write_all(p[1],(const char *)&err,sizeof(err));sleep_milliseconds(1000);}finish(127);
}
__asm__(".global _start\n_start:\nmov x0, sp\nbl probe_start\n");
