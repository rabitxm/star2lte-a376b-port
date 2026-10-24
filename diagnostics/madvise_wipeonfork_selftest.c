/* Raw AArch64 kernel-interface test, outside normal ROM builds.
 * Reason: loader EINVAL requires real WIPEONFORK semantics, not a success stub.
 * Scope: private process memory + anonymous memfd only; no device or disk writes.
 * Verify: parent unchanged, child/grandchild zero-fill, KEEPONFORK undo, and
 * rejection of shared/file mappings. Baseline EINVAL is an explicit rc77.
 */
#define EXYNOS9810_BOOTLOGGER_WRAPPER
#include "bootlogger.c"

static void done(long rc) { syscall6(94,rc,0,0,0,0,0);for(;;){} }
static void say(const char *text) {write_all(1,text,string_length(text));}
static long integer(char *p,long value) {
    char rev[24];long n=0,z=0;unsigned long u;
    if(value<0){p[z++]='-';u=0-(unsigned long)value;}else u=value;
    do{rev[n++]='0'+u%10;u/=10;}while(u);while(n)p[z++]=rev[--n];return z;
}
static void result(const char *name,long value) {
    char line[128];long n=string_length(name);copy_bytes(line,name,n);
    line[n++]='=';n+=integer(line+n,value);line[n++]='\n';write_all(1,line,n);
}
static void require(int truth,const char *what) {if(!truth){say(what);done(1);}}
static char *memory(long flags,long fd) {
    long p=syscall6(222,0,8192,3,flags,fd,0);
    require(!(p<0 && p>=-4095),"FAIL mmap\n");return (char *)p;
}
static int filled(char *p,long n,unsigned char value) {
    for(long i=0;i<n;++i)if((unsigned char)p[i]!=value)return 0;return 1;
}
static void wait_ok(long pid) {
    int status=-1;long r=syscall6(260,pid,(long)&status,0,0,0,0);
    require(r==pid && status==0,"FAIL child exit/status\n");
}
void test_start(void) {
    char *p=memory(0x22,-1);for(long i=0;i<4096;++i)p[i]=0x5a;
    for(long i=4096;i<8192;++i)p[i]=(char)0xa7;
    long r=syscall6(233,(long)p,4096,18,0,0,0);result("MADV_WIPEONFORK_RC",r);
    if(r==-22){say("UNSUPPORTED baseline kernel: WIPEONFORK rejected\n");done(77);}
    require(r==0,"FAIL WIPEONFORK\n");
    for(long iteration=0;iteration<32;++iteration){
        long child=syscall6(220,17,0,0,0,0,0);require(child>=0,"FAIL fork\n");
        if(!child){
            require(filled(p,4096,0),"FAIL child not zero\n");
            require(filled(p+4096,4096,0xa7),"FAIL adjacent page changed\n");
            for(long i=0;i<4096;++i)p[i]=0x33;
            long grandchild=syscall6(220,17,0,0,0,0,0);require(grandchild>=0,"FAIL grandchild fork\n");
            if(!grandchild){require(filled(p,4096,0),"FAIL grandchild not zero\n");done(0);}
            wait_ok(grandchild);require(filled(p,4096,0x33),"FAIL child contents changed by grandchild\n");done(0);
        }
        wait_ok(child);require(filled(p,4096,0x5a),"FAIL parent contents changed\n");
    }
    say("PASS parent preservation + child/grandchild zero-fill (32 rounds)\n");
    r=syscall6(233,(long)p,4096,19,0,0,0);result("MADV_KEEPONFORK_RC",r);require(r==0,"FAIL KEEPONFORK\n");
    long child=syscall6(220,17,0,0,0,0,0);require(child>=0,"FAIL KEEP fork\n");
    if(!child){require(filled(p,4096,0x5a),"FAIL KEEPONFORK did not restore inheritance\n");done(0);}
    wait_ok(child);say("PASS KEEPONFORK restores ordinary inheritance\n");
    char *shared=memory(0x21,-1);r=syscall6(233,(long)shared,4096,18,0,0,0);
    result("SHARED_WIPE_RC",r);require(r==-22,"FAIL shared mapping accepted\n");
    long fd=syscall6(279,(long)"wipeonfork-test",1,0,0,0,0);require(fd>=0,"FAIL anonymous memfd\n");
    require(syscall6(46,fd,8192,0,0,0,0)==0,"FAIL memfd resize\n");
    char *file=memory(2,fd);r=syscall6(233,(long)file,4096,18,0,0,0);
    result("FILE_WIPE_RC",r);require(r==-22,"FAIL file mapping accepted\n");
    say("PASS WIPEONFORK kernel semantics\n");done(0);
}
__asm__(".global _start\n_start:\nbl test_start\n");
