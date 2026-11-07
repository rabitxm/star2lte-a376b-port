.class public Lcom/samsung/android/app/facewidget/plugin/PluginKeyguardStatusViewImpl;
.super Ljava/lang/Object;
.implements Lcom/android/systemui/plugins/keyguardstatusview/PluginKeyguardStatusView;
.field public static calls:I
.method public constructor <init>()V
.locals 0
invoke-direct {p0}, Ljava/lang/Object;-><init>()V
return-void
.end method
.method public onDestroy()V
.locals 2
sget v0, Lcom/samsung/android/app/facewidget/plugin/PluginKeyguardStatusViewImpl;->calls:I
add-int/lit8 v0, v0, 2
sput v0, Lcom/samsung/android/app/facewidget/plugin/PluginKeyguardStatusViewImpl;->calls:I
return-void
.end method
