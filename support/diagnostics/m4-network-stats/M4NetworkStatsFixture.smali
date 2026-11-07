# Reason: exercise real Android stats flags, transport matching, Parcel and v7/v8 disk IO.
# Verify: run only as a normal app_process child, never PID1; no Binder/network/data writes.
.class public final LM4NetworkStatsFixture;
.super Ljava/lang/Object;

.method private static eq(II)V
    .locals 1
    if-eq p0, p1, :ok
    new-instance v0, Ljava/lang/AssertionError;
    invoke-direct {v0}, Ljava/lang/AssertionError;-><init>()V
    throw v0
    :ok
    return-void
.end method

.method private static flags()V
    .locals 4
    new-instance v0, Landroid/app/usage/NetworkStatsManager;
    const/4 v1, 0x0
    invoke-direct {v0, v1, v1}, Landroid/app/usage/NetworkStatsManager;-><init>(Landroid/content/Context;Landroid/net/INetworkStatsService;)V
    const/4 v1, 0x3
    invoke-virtual {v0, v1}, Landroid/app/usage/NetworkStatsManager;->sanitizeQueryFlags(I)I
    move-result v2
    const/4 v3, 0x7
    invoke-static {v2, v3}, LM4NetworkStatsFixture;->eq(II)V
    const/4 v1, 0x0
    invoke-virtual {v0, v1}, Landroid/app/usage/NetworkStatsManager;->setAugmentWithSubscriptionPlan(Z)V
    const/4 v1, 0x3
    invoke-virtual {v0, v1}, Landroid/app/usage/NetworkStatsManager;->sanitizeQueryFlags(I)I
    move-result v2
    invoke-static {v2, v1}, LM4NetworkStatsFixture;->eq(II)V
    :try_start
    const/4 v1, 0x4
    invoke-virtual {v0, v1}, Landroid/app/usage/NetworkStatsManager;->sanitizeQueryFlags(I)I
    :try_end
    .catch Ljava/lang/IllegalArgumentException; {:try_start .. :try_end} :expected
    new-instance v1, Ljava/lang/AssertionError;
    invoke-direct {v1}, Ljava/lang/AssertionError;-><init>()V
    throw v1
    :expected
    move-exception v1
    return-void
.end method

.method private static template(I)Landroid/net/NetworkTemplate;
    .locals 1
    new-instance v0, Landroid/net/NetworkTemplate$Builder;
    invoke-direct {v0}, Landroid/net/NetworkTemplate$Builder;-><init>()V
    invoke-virtual {v0, p0}, Landroid/net/NetworkTemplate$Builder;->setTransportType(I)Landroid/net/NetworkTemplate$Builder;
    invoke-virtual {v0}, Landroid/net/NetworkTemplate$Builder;->build()Landroid/net/NetworkTemplate;
    move-result-object v0
    return-object v0
.end method

.method private static identity(II)Landroid/net/NetworkIdentity;
    .locals 3
    new-instance v0, Landroid/net/NetworkIdentity$Builder;
    invoke-direct {v0}, Landroid/net/NetworkIdentity$Builder;-><init>()V
    invoke-virtual {v0, p0}, Landroid/net/NetworkIdentity$Builder;->setType(I)Landroid/net/NetworkIdentity$Builder;
    invoke-static {p1}, Ljava/lang/Integer;->valueOf(I)Ljava/lang/Integer;
    move-result-object v1
    invoke-static {v1}, Ljava/util/Collections;->singleton(Ljava/lang/Object;)Ljava/util/Set;
    move-result-object v1
    invoke-virtual {v0, v1}, Landroid/net/NetworkIdentity$Builder;->setTransportTypes(Ljava/util/Set;)Landroid/net/NetworkIdentity$Builder;
    invoke-virtual {v0}, Landroid/net/NetworkIdentity$Builder;->build()Landroid/net/NetworkIdentity;
    move-result-object v0
    return-object v0
.end method

.method private static parcel(Landroid/net/NetworkTemplate;)Landroid/net/NetworkTemplate;
    .locals 4
    invoke-static {}, Landroid/os/Parcel;->obtain()Landroid/os/Parcel;
    move-result-object v0
    const/4 v1, 0x0
    invoke-virtual {p0, v0, v1}, Landroid/net/NetworkTemplate;->writeToParcel(Landroid/os/Parcel;I)V
    invoke-virtual {v0, v1}, Landroid/os/Parcel;->setDataPosition(I)V
    sget-object v1, Landroid/net/NetworkTemplate;->CREATOR:Landroid/os/Parcelable$Creator;
    invoke-interface {v1, v0}, Landroid/os/Parcelable$Creator;->createFromParcel(Landroid/os/Parcel;)Ljava/lang/Object;
    move-result-object v1
    check-cast v1, Landroid/net/NetworkTemplate;
    invoke-virtual {v0}, Landroid/os/Parcel;->dataPosition()I
    move-result v2
    invoke-virtual {v0}, Landroid/os/Parcel;->dataSize()I
    move-result v3
    invoke-static {v2, v3}, LM4NetworkStatsFixture;->eq(II)V
    invoke-virtual {v0}, Landroid/os/Parcel;->recycle()V
    return-object v1
.end method

.method private static disk(Landroid/net/NetworkIdentity;Z)Landroid/net/NetworkIdentity;
    .locals 7
    new-instance v0, Landroid/net/connectivity/android/net/NetworkIdentitySet;
    invoke-direct {v0}, Landroid/net/connectivity/android/net/NetworkIdentitySet;-><init>()V
    invoke-virtual {v0, p0}, Landroid/net/connectivity/android/net/NetworkIdentitySet;->add(Ljava/lang/Object;)Z
    new-instance v1, Ljava/io/ByteArrayOutputStream;
    invoke-direct {v1}, Ljava/io/ByteArrayOutputStream;-><init>()V
    new-instance v2, Ljava/io/DataOutputStream;
    invoke-direct {v2, v1}, Ljava/io/DataOutputStream;-><init>(Ljava/io/OutputStream;)V
    if-eqz p1, :old_format
    invoke-virtual {v0, v2}, Landroid/net/connectivity/android/net/NetworkIdentitySet;->writeToStream(Ljava/io/DataOutput;)V
    goto :written
    :old_format
    const/4 v3, 0x0
    invoke-virtual {v0, v2, v3}, Landroid/net/connectivity/android/net/NetworkIdentitySet;->writeToStream(Ljava/io/DataOutput;Z)V
    :written
    invoke-virtual {v2}, Ljava/io/DataOutputStream;->flush()V
    invoke-virtual {v1}, Ljava/io/ByteArrayOutputStream;->toByteArray()[B
    move-result-object v3
    new-instance v4, Ljava/io/ByteArrayInputStream;
    invoke-direct {v4, v3}, Ljava/io/ByteArrayInputStream;-><init>([B)V
    new-instance v5, Ljava/io/DataInputStream;
    invoke-direct {v5, v4}, Ljava/io/DataInputStream;-><init>(Ljava/io/InputStream;)V
    invoke-virtual {v5}, Ljava/io/DataInputStream;->readInt()I
    move-result v6
    const/4 v0, 0x7
    if-eqz p1, :check_version
    const/16 v0, 0x8
    :check_version
    invoke-static {v6, v0}, LM4NetworkStatsFixture;->eq(II)V
    invoke-virtual {v4}, Ljava/io/ByteArrayInputStream;->reset()V
    new-instance v0, Landroid/net/connectivity/android/net/NetworkIdentitySet;
    invoke-direct {v0, v5}, Landroid/net/connectivity/android/net/NetworkIdentitySet;-><init>(Ljava/io/DataInput;)V
    invoke-virtual {v5}, Ljava/io/DataInputStream;->available()I
    move-result v1
    const/4 v2, 0x0
    invoke-static {v1, v2}, LM4NetworkStatsFixture;->eq(II)V
    invoke-virtual {v0}, Landroid/net/connectivity/android/net/NetworkIdentitySet;->iterator()Ljava/util/Iterator;
    move-result-object v0
    invoke-interface {v0}, Ljava/util/Iterator;->next()Ljava/lang/Object;
    move-result-object v0
    check-cast v0, Landroid/net/NetworkIdentity;
    return-object v0
.end method

.method private static legacy()V
    .locals 10
    new-instance v0, Landroid/net/NetworkIdentity;
    const/4 v1, 0x1
    const/4 v2, -0x1
    const/4 v3, 0x0
    const/4 v4, 0x0
    const/4 v5, 0x0
    const/4 v6, 0x0
    const/4 v7, 0x1
    const/4 v8, 0x0
    const/4 v9, -0x1
    invoke-direct/range {v0 .. v9}, Landroid/net/NetworkIdentity;-><init>(IILjava/lang/String;Ljava/lang/String;ZZZII)V
    const/4 v1, 0x1
    invoke-static {v1}, LM4NetworkStatsFixture;->template(I)Landroid/net/NetworkTemplate;
    move-result-object v2
    invoke-virtual {v2, v0}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v3
    invoke-static {v3, v1}, LM4NetworkStatsFixture;->eq(II)V
    return-void
.end method


.method private static legacyTemplate()V
    .locals 11
    const/4 v1, 0x1
    invoke-static {v1, v1}, LM4NetworkStatsFixture;->identity(II)Landroid/net/NetworkIdentity;
    move-result-object v10
    new-instance v0, Landroid/net/NetworkTemplate;
    const/4 v1, 0x4
    const/4 v9, 0x0
    new-array v2, v9, [Ljava/lang/String;
    new-array v3, v9, [Ljava/lang/String;
    const/4 v4, -0x1
    const/4 v5, -0x1
    const/4 v6, -0x1
    const/4 v7, -0x1
    const/4 v8, -0x1
    invoke-direct/range {v0 .. v8}, Landroid/net/NetworkTemplate;-><init>(I[Ljava/lang/String;[Ljava/lang/String;IIIII)V
    invoke-virtual {v0, v10}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v1
    const/4 v2, 0x1
    invoke-static {v1, v2}, LM4NetworkStatsFixture;->eq(II)V
    return-void
.end method

.method private static builderRejects()V
    .locals 2
    new-instance v0, Landroid/net/NetworkTemplate$Builder;
    invoke-direct {v0}, Landroid/net/NetworkTemplate$Builder;-><init>()V
    :try_empty
    invoke-virtual {v0}, Landroid/net/NetworkTemplate$Builder;->build()Landroid/net/NetworkTemplate;
    :end_empty
    .catch Ljava/lang/IllegalArgumentException; {:try_empty .. :end_empty} :got_empty
    new-instance v1, Ljava/lang/AssertionError;
    invoke-direct {v1}, Ljava/lang/AssertionError;-><init>()V
    throw v1
    :got_empty
    move-exception v1
    new-instance v0, Landroid/net/NetworkTemplate$Builder;
    const/4 v1, 0x4
    invoke-direct {v0, v1}, Landroid/net/NetworkTemplate$Builder;-><init>(I)V
    :try_legacy
    const/4 v1, 0x1
    invoke-virtual {v0, v1}, Landroid/net/NetworkTemplate$Builder;->setTransportType(I)Landroid/net/NetworkTemplate$Builder;
    :end_legacy
    .catch Ljava/lang/IllegalArgumentException; {:try_legacy .. :end_legacy} :got_legacy
    new-instance v1, Ljava/lang/AssertionError;
    invoke-direct {v1}, Ljava/lang/AssertionError;-><init>()V
    throw v1
    :got_legacy
    move-exception v1
    return-void
.end method

.method public static main([Ljava/lang/String;)V
    .locals 5
    :try_start
    const-wide/16 v0, 0x0
    const-wide/16 v2, 0x1
    invoke-static {v0, v1, v2, v3}, Ljava/lang/Long;->compare(JJ)I
    move-result v0
    const/4 v1, -0x1
    invoke-static {v0, v1}, LM4NetworkStatsFixture;->eq(II)V
    invoke-static {}, LM4NetworkStatsFixture;->legacyTemplate()V
    invoke-static {}, LM4NetworkStatsFixture;->builderRejects()V
    invoke-static {}, LM4NetworkStatsFixture;->flags()V
    invoke-static {}, LM4NetworkStatsFixture;->legacy()V
    const/4 v4, 0x1
    invoke-static {v4}, LM4NetworkStatsFixture;->template(I)Landroid/net/NetworkTemplate;
    move-result-object v0
    invoke-static {v4, v4}, LM4NetworkStatsFixture;->identity(II)Landroid/net/NetworkIdentity;
    move-result-object v1
    invoke-virtual {v0, v1}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v2
    invoke-static {v2, v4}, LM4NetworkStatsFixture;->eq(II)V
    const/4 v2, 0x0
    invoke-static {v2, v2}, LM4NetworkStatsFixture;->identity(II)Landroid/net/NetworkIdentity;
    move-result-object v2
    invoke-virtual {v0, v2}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v2
    const/4 v3, 0x0
    invoke-static {v2, v3}, LM4NetworkStatsFixture;->eq(II)V
    invoke-static {v0}, LM4NetworkStatsFixture;->parcel(Landroid/net/NetworkTemplate;)Landroid/net/NetworkTemplate;
    move-result-object v0
    invoke-virtual {v0, v1}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v2
    invoke-static {v2, v4}, LM4NetworkStatsFixture;->eq(II)V
    invoke-static {v1, v4}, LM4NetworkStatsFixture;->disk(Landroid/net/NetworkIdentity;Z)Landroid/net/NetworkIdentity;
    move-result-object v2
    invoke-virtual {v0, v2}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v2
    invoke-static {v2, v4}, LM4NetworkStatsFixture;->eq(II)V
    invoke-static {v1, v3}, LM4NetworkStatsFixture;->disk(Landroid/net/NetworkIdentity;Z)Landroid/net/NetworkIdentity;
    move-result-object v2
    invoke-virtual {v0, v2}, Landroid/net/NetworkTemplate;->matches(Landroid/net/NetworkIdentity;)Z
    move-result v2
    invoke-static {v2, v4}, LM4NetworkStatsFixture;->eq(II)V
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;
    const-string v1, "M4-STATS-API-SELFTEST PASS flags transport Parcel v7/v8 legacy-entry-points"
    invoke-virtual {v0, v1}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V
    :try_end
    .catch Ljava/lang/Throwable; {:try_start .. :try_end} :failed
    return-void
    :failed
    move-exception v0
    invoke-virtual {v0}, Ljava/lang/Throwable;->printStackTrace()V
    const/4 v1, 0x1
    invoke-static {v1}, Ljava/lang/System;->exit(I)V
    return-void
.end method
