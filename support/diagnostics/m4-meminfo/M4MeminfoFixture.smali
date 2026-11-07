# Runs the actual Debug JNI/ReadProcessGpuUsageKb crash path as an ordinary child.
# Reports the returned value, including unsupported (-1); no fabricated GPU usage.
.class public final LM4MeminfoFixture;
.super Ljava/lang/Object;

.method public static main([Ljava/lang/String;)V
    .locals 5
    const/16 v4, 0x8
    :loop
    invoke-static {}, Landroid/os/Debug;->getGpuTotalUsageKb()J
    move-result-wide v0
    sget-object v2, Ljava/lang/System;->out:Ljava/io/PrintStream;
    new-instance v3, Ljava/lang/StringBuilder;
    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V
    const-string p0, "GPU_TOTAL_KB="
    invoke-virtual {v3, p0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;
    invoke-virtual {v3, v0, v1}, Ljava/lang/StringBuilder;->append(J)Ljava/lang/StringBuilder;
    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;
    move-result-object v3
    invoke-virtual {v2, v3}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    add-int/lit8 v4, v4, -0x1
    if-nez v4, :loop
    new-instance v0, Lcom/android/internal/util/MemInfoReader;
    invoke-direct {v0}, Lcom/android/internal/util/MemInfoReader;-><init>()V
    invoke-virtual {v0}, Lcom/android/internal/util/MemInfoReader;->readMemInfo()V
    invoke-virtual {v0}, Lcom/android/internal/util/MemInfoReader;->getKernelUsedSizeKb()J
    move-result-wide v0
    sget-object v2, Ljava/lang/System;->out:Ljava/io/PrintStream;
    invoke-virtual {v2, v0, v1}, Ljava/io/PrintStream;->println(J)V
    const-string v0, "M4-MEMINFO-SELFTEST PASS real Debug JNI and MemInfoReader returned"
    invoke-virtual {v2, v0}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    return-void
.end method
