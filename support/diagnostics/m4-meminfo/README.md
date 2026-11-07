# BZI6 GPU meminfo diagnostic

Reason: source0197 SystemServer native SIGABRT reaches the invalid-fd branch
of BpfMapRO's verifier before the existing optional GPU reader error path.
The fixture invokes the actual Debug JNI eight times and MemInfoReader's
kernel-used calculation as a normal app_process child. It only reads memory
statistics; it neither creates BPF maps nor changes GPU/kernel state.

Assemble with the fork's bundled smali API29/DEX039 and stage the JAR with
shell_data_file label. Invoke with CLASSPATH set to that JAR, then
`/system/bin/app_process /system/bin M4MeminfoFixture`.

Only exit0 and the explicit PASS marker pass. GPU_TOTAL_KB=-1 is a reported
unsupported measurement, not a successful zero-byte GPU measurement. A shell
fixture alone does not certify the SystemServer SELinux domain, valid-map
data retrieval, overall boot or graphics correctness. Capture the real
SystemServer runtime independently. Private artifacts/logs remain in logs/.

The freestanding bpf_gpu_readonly.c uses ARM64 raw syscalls only. Compile with
Clang --target=aarch64-linux-android29 -nostdlib -static -fuse-ld=lld
-fno-builtin -fno-stack-protector -O2 -Wl,-e,_start. It opens the actual
libmeminfo pin with flags8 (library behavior) and flags0 (comparison), reports
raw_result=-errno on failure, and closes successful fds. Record the executing
UID/SELinux domain alongside the output; do not call it SystemServer errno.
