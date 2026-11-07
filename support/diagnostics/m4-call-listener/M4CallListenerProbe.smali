# Reason: inspect the missing BZI6 OTT-slicing service and its actual backend flag;
# an optional diagnostic disable changes only this component with DONT_KILL_APP.
# Verify: query first, retain original state, use exact G965F/SDK37 identity and
# runtime JAR/APK hashes; restore returns only the recorded default state.
.class public final LM4CallListenerProbe;
.super Ljava/lang/Object;

.method private static print(Ljava/lang/String;)V
    .locals 1
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;
    invoke-virtual {v0, p0}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    return-void
.end method

.method public static main([Ljava/lang/String;)V
    .locals 10
    const-string v0, "ro.bootloader"
    invoke-static {v0}, Landroid/os/SystemProperties;->get(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v0
    const-string v1, "G965F"
    invoke-virtual {v0, v1}, Ljava/lang/String;->startsWith(Ljava/lang/String;)Z
    move-result v0
    if-eqz v0, :bad
    const-string v0, "ro.product.device"
    invoke-static {v0}, Landroid/os/SystemProperties;->get(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v0
    const-string v1, "star2lte"
    invoke-virtual {v1, v0}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v0
    if-eqz v0, :bad
    sget v0, Landroid/os/Build$VERSION;->SDK_INT:I
    const/16 v1, 0x25
    if-ne v0, v1, :bad

    const-string v0, "connectivity"
    invoke-static {v0}, Landroid/os/ServiceManager;->getService(Ljava/lang/String;)Landroid/os/IBinder;
    move-result-object v0
    invoke-static {v0}, Landroid/net/IConnectivityManager$Stub;->asInterface(Landroid/os/IBinder;)Landroid/net/IConnectivityManager;
    move-result-object v0
    invoke-interface {v0}, Landroid/net/IConnectivityManager;->getEnabledConnectivityManagerFeatures()J
    move-result-wide v6
    invoke-static {v6, v7}, Ljava/lang/Long;->toHexString(J)Ljava/lang/String;
    move-result-object v0
    const-string v1, "backend_feature_bits=0x"
    invoke-virtual {v1, v0}, Ljava/lang/String;->concat(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v0
    invoke-static {v0}, LM4CallListenerProbe;->print(Ljava/lang/String;)V
    const-wide/16 v8, 0x4
    and-long/2addr v6, v8
    const-wide/16 v8, 0x0
    cmp-long v6, v6, v8
    if-nez v6, :feature_on
    const-string v0, "ott_network_slicing=false"
    goto :print_feature
    :feature_on
    const-string v0, "ott_network_slicing=true"
    :print_feature
    invoke-static {v0}, LM4CallListenerProbe;->print(Ljava/lang/String;)V

    invoke-static {}, Landroid/app/AppGlobals;->getPackageManager()Landroid/content/pm/IPackageManager;
    move-result-object v0
    new-instance v1, Landroid/content/ComponentName;
    const-string v2, "android"
    const-string v3, "android.net.ConnectivityCallListenerService"
    invoke-direct {v1, v2, v3}, Landroid/content/ComponentName;-><init>(Ljava/lang/String;Ljava/lang/String;)V
    const/4 v4, 0x0
    invoke-interface {v0, v1, v4}, Landroid/content/pm/IPackageManager;->getComponentEnabledSetting(Landroid/content/ComponentName;I)I
    move-result v7
    invoke-static {v7}, Ljava/lang/Integer;->toString(I)Ljava/lang/String;
    move-result-object v2
    const-string v3, "component_state_before="
    invoke-virtual {v3, v2}, Ljava/lang/String;->concat(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v2
    invoke-static {v2}, LM4CallListenerProbe;->print(Ljava/lang/String;)V

    array-length v2, p0
    if-eqz v2, :done
    const/4 v2, 0x0
    aget-object v8, p0, v2
    const-string v3, "disable"
    invoke-virtual {v3, v8}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v3
    if-eqz v3, :restore
    if-nez v7, :bad
    const/4 v2, 0x2
    goto :set_state
    :restore
    const-string v3, "restore"
    invoke-virtual {v3, v8}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z
    move-result v3
    if-eqz v3, :bad
    const/4 v3, 0x2
    if-ne v7, v3, :bad
    const/4 v2, 0x0
    :set_state
    const/4 v3, 0x1
    const/4 v4, 0x0
    const-string v5, "com.android.shell"
    invoke-interface/range {v0 .. v5}, Landroid/content/pm/IPackageManager;->setComponentEnabledSetting(Landroid/content/ComponentName;IIILjava/lang/String;)V
    invoke-interface {v0, v1, v4}, Landroid/content/pm/IPackageManager;->getComponentEnabledSetting(Landroid/content/ComponentName;I)I
    move-result v3
    if-ne v2, v3, :bad
    invoke-static {v3}, Ljava/lang/Integer;->toString(I)Ljava/lang/String;
    move-result-object v2
    const-string v3, "component_state_after="
    invoke-virtual {v3, v2}, Ljava/lang/String;->concat(Ljava/lang/String;)Ljava/lang/String;
    move-result-object v2
    invoke-static {v2}, LM4CallListenerProbe;->print(Ljava/lang/String;)V
    :done
    return-void
    :bad
    new-instance v0, Ljava/lang/IllegalStateException;
    const-string v1, "Wrong target, mode or component state; no broad package operation allowed"
    invoke-direct {v0, v1}, Ljava/lang/IllegalStateException;-><init>(Ljava/lang/String;)V
    throw v0
.end method
