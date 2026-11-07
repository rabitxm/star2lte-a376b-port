/* Reason: gpuMem/gpuWork fail at BPF_PROG_LOAD with EINVAL on attempt54.
 * Scope: load a two-instruction return-0 program, close its FD; no attachment,
 * pinned map, file, kernel config, policy or phone partition changes.
 * Verification: socket-filter control distinguishes syscall/permission failure
 * from unavailable tracepoint/perf-event program types. Raw arm64 syscall ABI
 * and bpf_attr layout match the pinned local kernel UAPI.
 */
typedef unsigned int u32;
typedef unsigned long u64;
struct insn { unsigned char code, regs; short off; int imm; };
static const struct insn instructions[]={{0xb7,0,0,0},{0x95,0,0,0}};
static const char license[]="GPL";
static char log_buffer[8192];
static struct {
    u32 type, count; u64 insns, license; u32 log_level, log_size;
    u64 log_buffer; u32 kernel_version, flags; char name[16];
    u32 ifindex, expected_attach_type; u64 zeros[6];
} attr;
static long call(long n,long a,long b,long c) {
    register long x0 __asm__("x0")=a;
    register long x1 __asm__("x1")=b;
    register long x2 __asm__("x2")=c;
    register long x8 __asm__("x8")=n;
    __asm__ volatile("svc 0":"+r"(x0):"r"(x1),"r"(x2),"r"(x8):"memory");
    return x0;
}
static void say(const char *s) {
    long n=0;while(s[n])++n;call(64,1,(long)s,n);
}
static void number(long value) {
    char out[32],rev[32];long n=0,z=0;unsigned long u;
    if(value<0){out[z++]='-';u=0-(unsigned long)value;}else u=value;
    do{rev[n++]='0'+u%10;u/=10;}while(u);
    while(n)out[z++]=rev[--n];out[z++]='\n';call(64,1,(long)out,z);
}
static void test(u32 type,const char *name) {
    for(long i=0;i<(long)sizeof(log_buffer);++i)log_buffer[i]=0;
    attr.type=type;attr.count=2;attr.insns=(u64)instructions;
    attr.license=(u64)license;attr.log_level=1;attr.log_size=sizeof(log_buffer);
    attr.log_buffer=(u64)log_buffer;
    long fd=call(280,5,(long)&attr,sizeof(attr));
    say(name);number(fd);
    if(log_buffer[0]){say("verifier_log:\n");say(log_buffer);say("\n");}
    if(fd>=0)call(57,fd,0,0);
}
void test_start(void) {
    test(1,"SOCKET_FILTER_RAW_RC=");
    test(5,"TRACEPOINT_RAW_RC=");
    test(7,"PERF_EVENT_RAW_RC=");
    call(94,0,0,0);for(;;){}
}
__asm__(".global _start\n_start:\nbl test_start\n");
