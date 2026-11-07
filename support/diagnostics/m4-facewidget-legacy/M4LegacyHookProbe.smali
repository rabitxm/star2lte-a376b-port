.class public LM4LegacyHookProbe;
.super Ljava/lang/Object;

.method private static check(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
    .locals 10
    const/4 v3, 0x0
    const-class v4, LM4LegacyHookProbe;
    invoke-virtual {v4}, Ljava/lang/Class;->getClassLoader()Ljava/lang/ClassLoader;
    move-result-object v4
    const-string v2, "/data/local/tmp/m4-legacy-probe-oat"
    new-instance v0, Ldalvik/system/DexClassLoader;
    invoke-direct {v0, p0, v2, v3, v4}, Ldalvik/system/DexClassLoader;-><init>(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V
    invoke-virtual {v0, p1}, Ljava/lang/ClassLoader;->loadClass(Ljava/lang/String;)Ljava/lang/Class;
    move-result-object v5
    invoke-virtual {v5}, Ljava/lang/Class;->newInstance()Ljava/lang/Object;
    move-result-object v6
    const-class v4, Lcom/android/systemui/plugins/keyguardstatusview/PluginKeyguardStatusView;
    const/4 v0, 0x0
    new-array v2, v0, [Ljava/lang/Class;
    const-string v1, "disconnectPluginOnBackground"
    invoke-virtual {v4, v1, v2}, Ljava/lang/Class;->getMethod(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;
    move-result-object v8
    new-instance v7, Lcom/android/systemui/plugins/annotations/VersionCheckingProxy;
    invoke-direct {v7, v4, v6, v3}, Lcom/android/systemui/plugins/annotations/VersionCheckingProxy;-><init>(Ljava/lang/Class;Ljava/lang/Object;Ljava/util/function/Supplier;)V
    new-array v2, v0, [Ljava/lang/Object;
    :try_start
    invoke-virtual {v7, v3, v8, v2}, Lcom/android/systemui/plugins/annotations/VersionCheckingProxy;->invoke(Ljava/lang/Object;Ljava/lang/reflect/Method;[Ljava/lang/Object;)Ljava/lang/Object;
    :try_end
    .catch Ljava/lang/reflect/InvocationTargetException; {:try_start .. :try_end} :expected_throw
    const-string v1, ""
    invoke-virtual {v1, p2}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v1
    if-eqz v1, :fail
    goto :check_count
    :expected_throw
    move-exception v9
    invoke-virtual {v9}, Ljava/lang/reflect/InvocationTargetException;->getCause()Ljava/lang/Throwable;
    move-result-object v9
    invoke-virtual {v9}, Ljava/lang/Object;->getClass()Ljava/lang/Class;
    move-result-object v9
    invoke-virtual {v9}, Ljava/lang/Class;->getName()Ljava/lang/String;
    move-result-object v9
    invoke-virtual {p2, v9}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v1
    if-eqz v1, :fail
    :check_count
    const-string v1, "calls"
    invoke-virtual {v5, v1}, Ljava/lang/Class;->getField(Ljava/lang/String;)Ljava/lang/reflect/Field;
    move-result-object v9
    invoke-virtual {v9, v3}, Ljava/lang/reflect/Field;->getInt(Ljava/lang/Object;)I
    move-result v1
    if-ne v1, p3, :fail
    const/4 v0, 0x0
    new-array v2, v0, [Ljava/lang/Class;
    const-string v1, "onDestroy"
    invoke-virtual {v4, v1, v2}, Ljava/lang/Class;->getMethod(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;
    move-result-object v8
    new-array v2, v0, [Ljava/lang/Object;
    invoke-virtual {v7, v3, v8, v2}, Lcom/android/systemui/plugins/annotations/VersionCheckingProxy;->invoke(Ljava/lang/Object;Ljava/lang/reflect/Method;[Ljava/lang/Object;)Ljava/lang/Object;
    invoke-virtual {v9, v3}, Ljava/lang/reflect/Field;->getInt(Ljava/lang/Object;)I
    move-result v1
    add-int/lit8 p3, p3, 0x2
    if-ne v1, p3, :fail
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;
    invoke-virtual {v0, p0}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    return-void
    :fail
    new-instance v0, Ljava/lang/AssertionError;
    const-string v1, "Optional-hook or onDestroy semantics failed"
    invoke-direct {v0, v1}, Ljava/lang/AssertionError;-><init>(Ljava/lang/Object;)V
    throw v0
.end method

.method private static run([Ljava/lang/String;)V
    .locals 5
    const-string v0, "ro.boot.bootloader"
    invoke-static {v0}, Landroid/os/SystemProperties;->get(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v0
    const-string v1, "G965F"
    invoke-virtual {v0, v1}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z
    move-result v0
    if-eqz v0, :wrong_device
    const-string v0, "ro.product.device"
    invoke-static {v0}, Landroid/os/SystemProperties;->get(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v0
    const-string v1, "star2lte"
    invoke-virtual {v1, v0}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v0
    if-eqz v0, :wrong_device
    const-string v0, "/data/local/tmp/m4-hook-missing.jar"
    const-string v1, "com.samsung.android.app.facewidget.plugin.PluginKeyguardStatusViewImpl"
    const-string v2, ""
    const/4 v3, 0x0
    invoke-static {v0, v1, v2, v3}, LM4LegacyHookProbe;->check(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
    const-string v0, "/data/local/tmp/m4-hook-concrete.jar"
    const/4 v3, 0x1
    invoke-static {v0, v1, v2, v3}, LM4LegacyHookProbe;->check(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
    const-string v0, "/data/local/tmp/m4-hook-throwing.jar"
    const-string v2, "java.lang.IllegalStateException"
    invoke-static {v0, v1, v2, v3}, LM4LegacyHookProbe;->check(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
    const-string v0, "/data/local/tmp/m4-hook-other.jar"
    const-string v1, "probe.OtherPlugin"
    const-string v2, "java.lang.AbstractMethodError"
    const/4 v3, 0x0
    invoke-static {v0, v1, v2, v3}, LM4LegacyHookProbe;->check(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;
    const-string v1, "PASS missing hook omitted; concrete hook called; concrete exception and other target error propagated; onDestroy always called"
    invoke-virtual {v0, v1}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    return-void
    :wrong_device
    new-instance v0, Ljava/lang/IllegalStateException;
    const-string v1, "G965F/star2lte only"
    invoke-direct {v0, v1}, Ljava/lang/IllegalStateException;-><init>(Ljava/lang/String;)V
    throw v0
.end method

.method public static main([Ljava/lang/String;)V
    .locals 1
    :main_try
    invoke-static {p0}, LM4LegacyHookProbe;->run([Ljava/lang/String;)V
    :main_end
    .catch Ljava/lang/Throwable; {:main_try .. :main_end} :main_fail
    return-void
    :main_fail
    move-exception v0
    invoke-virtual {v0}, Ljava/lang/Throwable;->printStackTrace()V
    const/4 v0, 0x1
    invoke-static {v0}, Ljava/lang/System;->exit(I)V
    return-void
.end method
