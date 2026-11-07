/* Reason: report the real BPF_OBJ_GET result for the actual libmeminfo pin.
 * Freestanding ARM64 read-only probe; no libc, map creation, attach or load.
 * Run as a regular child. Its UID/domain is reported separately; a shell
 * result must not be represented as SystemServer's errno. */
typedef unsigned long u64;
typedef unsigned int u32;
static long sc(long nr,long a,long b,long c) {
 register long x0 __asm__("x0")=a,x1 __asm__("x1")=b,x2 __asm__("x2")=c,x8 __asm__("x8")=nr;
 __asm__ volatile("svc 0" : "+r"(x0) : "r"(x1),"r"(x2),"r"(x8) : "memory","cc");
 return x0;
}
static void text(const char *s) { long n=0;while(s[n])n++;sc(64,1,(long)s,n); }
static void number(long n) { char a[24];int i=24;if(n<0){text("-");n=-n;}do{a[--i]='0'+n%10;n/=10;}while(n);sc(64,1,(long)(a+i),24-i); }
static void probe(const char *pin,u32 flags) {
 union { u64 align; unsigned char bytes[168]; } a;
 for(int i=0;i<168;i++)a.bytes[i]=0;
 *(u64*)a.bytes=(u64)pin;*(u32*)(a.bytes+12)=flags;
 long fd=sc(280,7,(long)&a,168);
 text("BPF_OBJ_GET pin=");text(pin);text(" flags=");number(flags);text(" raw_result=");number(fd);text("\n");
 if(fd>=0)sc(57,fd,0,0);
}
__attribute__((noreturn)) void _start(void) {
 probe("/sys/fs/bpf/map_gpuMem_gpu_mem_total_map",8);
 probe("/sys/fs/bpf/map_gpuMem_gpu_mem_total_map",0);
 text("M4-GPU-BPF-READONLY probe finished; negative raw_result=-errno\n");
 sc(93,0,0,0);__builtin_unreachable();
}
