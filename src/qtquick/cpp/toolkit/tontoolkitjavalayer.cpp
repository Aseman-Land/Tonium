/*
    Copyright (C) 2017 TonToolkit Team
    http://tontoolkit.co

    TonToolkitQtTools is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TonToolkitQtTools is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tontoolkitjavalayer.h"

#include <QDebug>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QSet>
#include <QPair>
#include <QList>
#include <QDateTime>
#include <QFile>
#include <QCoreApplication>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QPointer>
#include <QColor>

#include <jni.h>

static QSet<TonToolkitJavaLayer*> java_layers_objects;
static QList< QPair<QString,QString> > java_layer_inc_share_buffer;
static QList< QString > java_layer_inc_image_buffer;

static bool tontoolkit_jlayer_registerNativeMethods();
static bool tontoolkit_jlayer_native_methods_registered = tontoolkit_jlayer_registerNativeMethods();

class TonToolkitJavaLayerPrivate
{
public:
    QAndroidJniObject object;
    QAndroidJniEnvironment env;
};

TonToolkitJavaLayer::TonToolkitJavaLayer() :
    QObject()
{
    p = new TonToolkitJavaLayerPrivate;
    p->object = QAndroidJniObject("io/tontoolkit/android/TonToolkitJavaLayer");

#if defined(Q_OS_ANDROID) && defined(TONTOOLKIT_STATIC_BUILD)
    QMetaObject::invokeMethod( this, "registerObject", Qt::QueuedConnection );
#else
    registerObject();
#endif

    QMetaObject::invokeMethod( this, "load_buffer", Qt::QueuedConnection );

    setImplemented(true);
}

void TonToolkitJavaLayer::registerObject()
{
    java_layers_objects.insert(this);
}

TonToolkitJavaLayer *TonToolkitJavaLayer::instance()
{
    if(!tontoolkit_jlayer_native_methods_registered)
        tontoolkit_jlayer_registerNativeMethods();

    static QPointer<TonToolkitJavaLayer> java_layer_instance;
    if( !java_layer_instance )
    {
        java_layer_instance = new TonToolkitJavaLayer();
        if(QCoreApplication::instance())
            QObject::connect(QCoreApplication::instance(), &QCoreApplication::destroyed, java_layer_instance, &TonToolkitJavaLayer::deleteLater);
    }

    return java_layer_instance;
}

bool TonToolkitJavaLayer::sharePaper(const QString &title, const QString &msg)
{
    QAndroidJniObject jtitle = QAndroidJniObject::fromString(title.toUtf8());
    QAndroidJniObject jmsg = QAndroidJniObject::fromString(msg.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;Ljava/lang/String;)Z", jtitle.object<jstring>(), jmsg.object<jstring>() );
    return res;
}

bool TonToolkitJavaLayer::shareFile(const QString &path, const QString &type)
{
    QAndroidJniObject jpath = QAndroidJniObject::fromString(path.toUtf8());
    QAndroidJniObject jtype = QAndroidJniObject::fromString(type.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;Ljava/lang/String;)Z", jpath.object<jstring>(), jtype.object<jstring>() );
    return res;
}

bool TonToolkitJavaLayer::openFile(const QString &path, const QString &type)
{
    QAndroidJniObject jpath = QAndroidJniObject::fromString(path.toUtf8());
    QAndroidJniObject jtype = QAndroidJniObject::fromString(type.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;Ljava/lang/String;)Z", jpath.object<jstring>(), jtype.object<jstring>() );
    return res;
}

bool TonToolkitJavaLayer::startCamera(const QString &output)
{
    QAndroidJniObject joutput = QAndroidJniObject::fromString(output.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;)Z", joutput.object<jstring>() );
    return res;
}

bool TonToolkitJavaLayer::callNumber(const QString &number)
{
    QAndroidJniObject jnumber = QAndroidJniObject::fromString(number.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;)Z", jnumber.object<jstring>() );
    return res;
}

bool TonToolkitJavaLayer::getOpenPictures()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z");
    return res;
}

bool TonToolkitJavaLayer::startForeground(qint32 id, const QString &title, const QString &msg, const QString &iconPath, const QString &icon, const QString &channelId)
{
    jint jid = id;
    QAndroidJniObject jtitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jmsg = QAndroidJniObject::fromString(msg);
    QAndroidJniObject jicon = QAndroidJniObject::fromString(icon);
    QAndroidJniObject jiconPath = QAndroidJniObject::fromString(iconPath);
    QAndroidJniObject jchannelId = QAndroidJniObject::fromString(channelId);
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
                                                  jid, jtitle.object<jstring>(), jmsg.object<jstring>(),
                                                  jiconPath.object<jstring>(), jicon.object<jstring>(),
                                                  jchannelId.object<jstring>());
    return res;
}

bool TonToolkitJavaLayer::stopForeground(bool removeNotification)
{
    jboolean jremoveNotification = removeNotification;
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Z)Z", jremoveNotification);
    return res;
}

bool TonToolkitJavaLayer::startNotification(qint32 id, const QString &title, const QString &msg, const QString &iconPath, const QString &icon, const QString &channelId, bool sound, bool vibrate)
{
    jint jid = id;
    QAndroidJniObject jtitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jmsg = QAndroidJniObject::fromString(msg);
    QAndroidJniObject jicon = QAndroidJniObject::fromString(icon);
    QAndroidJniObject jiconPath = QAndroidJniObject::fromString(iconPath);
    QAndroidJniObject jchannelId = QAndroidJniObject::fromString(channelId);
    jboolean jsound = sound;
    jboolean jvibrate = vibrate;
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ)Z",
                                                  jid, jtitle.object<jstring>(), jmsg.object<jstring>(),
                                                  jiconPath.object<jstring>(), jicon.object<jstring>(),
                                                  jchannelId.object<jstring>(), jsound, jvibrate);
    return res;
}

bool TonToolkitJavaLayer::stopNotification(qint32 id)
{
    jint jid = id;
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(I)Z", jid);
    return res;
}

QString TonToolkitJavaLayer::createNotificationChannel(const QString &channelId, const QString &channelName, int importance)
{
    QAndroidJniObject jchannelId = QAndroidJniObject::fromString(channelId);
    QAndroidJniObject jchannelName = QAndroidJniObject::fromString(channelName);
    jint jimportance = importance;
    QString res = p->object.callObjectMethod(__FUNCTION__, "(Ljava/lang/String;Ljava/lang/String;I)Ljava/lang/String;",
                                             jchannelId.object<jstring>(), jchannelName.object<jstring>(), jimportance).toString();
    return res;
}

bool TonToolkitJavaLayer::transparentStatusBar()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z");
    return res;
}

bool TonToolkitJavaLayer::transparentNavigationBar()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z");
    return res;
}

bool TonToolkitJavaLayer::startService()
{
    return startQtService();
}

bool TonToolkitJavaLayer::stopService()
{
    return stopQtService();
}

bool TonToolkitJavaLayer::killService(const QString &serviceName)
{
    QAndroidJniObject jserviceName = QAndroidJniObject::fromString(serviceName.toUtf8());
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "(Ljava/lang/String;)Z", jserviceName.object<jstring>());
    return res;
}

bool TonToolkitJavaLayer::startQtService()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z" );
    return res;
}

bool TonToolkitJavaLayer::stopQtService()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z" );
    return res;
}

int TonToolkitJavaLayer::densityDpi()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

int TonToolkitJavaLayer::screenSizeWidth()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

int TonToolkitJavaLayer::screenSizeHeight()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

int TonToolkitJavaLayer::getSizeName()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

int TonToolkitJavaLayer::statusBarHeight()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

int TonToolkitJavaLayer::navigationBarHeight()
{
    jint res = p->object.callMethod<jint>(__FUNCTION__, "()I" );
    return res;
}

bool TonToolkitJavaLayer::isTablet()
{
    jboolean res = p->object.callMethod<jboolean>(__FUNCTION__, "()Z");
    return res;
}

qreal TonToolkitJavaLayer::density()
{
    jfloat res = p->object.callMethod<jfloat>(__FUNCTION__, "()F" );
    return res;
}

QString TonToolkitJavaLayer::packageName()
{
    QString res = p->object.callObjectMethod(__FUNCTION__, "()Ljava/lang/String;" ).toString();
    return res;
}

QString TonToolkitJavaLayer::deviceName()
{
    QString res = p->object.callObjectMethod(__FUNCTION__, "()Ljava/lang/String;" ).toString();
    return res;
}

QString TonToolkitJavaLayer::deviceId()
{
    QString res = p->object.callObjectMethod(__FUNCTION__, "()Ljava/lang/String;" ).toString();
    return res;
}

QRect TonToolkitJavaLayer::keyboardRect()
{
    jint jheight = p->object.callMethod<jfloat>("menuHeight", "()I" );
    int menuheight = jheight;

    const QList<QScreen*> & screens = QGuiApplication::screens();
    if( screens.isEmpty() )
        return QRect();

    QScreen *screen = screens.first();
    QRect rect = screen->availableGeometry();
    QRect geom = screen->geometry();

    rect.moveTop(rect.top() + menuheight);
    geom.setTop(geom.top() + menuheight);

    QRect final;

    if (rect != geom)
    {
        int ftop, fleft, fwidth, fheight;

        geom.getRect(&fleft, &ftop, &fwidth, &fheight);

        if (rect.top() != ftop)
            fheight = rect.top();
        else if (rect.left() != fleft)
            fwidth = rect.left();
        else if (rect.height() != fheight)
            ftop = rect.height();
        else if (rect.width() != fwidth)
            fleft = rect.width();

        final = QRect(fleft, ftop, fwidth - fleft, fheight - ftop);
    }

    return final;
}

QString TonToolkitJavaLayer::getLastImages(qint32 offset, qint32 count)
{
    jint joffset = offset;
    jint jcount = count;
    QString res = p->object.callObjectMethod(__FUNCTION__, "(II)Ljava/lang/String;", joffset, jcount).toString();
    return res;
}

QString TonToolkitJavaLayer::getContactList()
{
    QString res = p->object.callObjectMethod(__FUNCTION__, "()Ljava/lang/String;").toString();
    return res;
}

void TonToolkitJavaLayer::setKeepScreenOn(bool stt)
{
    jboolean jstt = stt;
    p->object.callMethod<jboolean>(__FUNCTION__, "(Z)V", jstt );
}

bool TonToolkitJavaLayer::setTransparentNavigationBar(bool stt)
{
    jboolean jstt = stt;
    return p->object.callMethod<jboolean>(__FUNCTION__, "(Z)Z", jstt );
}

bool TonToolkitJavaLayer::setTransparentStatusBar(bool stt)
{
    jboolean jstt = stt;
    return p->object.callMethod<jboolean>(__FUNCTION__, "(Z)Z", jstt );\
}

void TonToolkitJavaLayer::load_buffer()
{
    while( !java_layer_inc_share_buffer.isEmpty() )
    {
        const QPair<QString,QString> & pair = java_layer_inc_share_buffer.takeFirst();
        Q_EMIT incomingShare( pair.first, pair.second );
    }
}

void TonToolkitJavaLayer::setImplemented(bool stt)
{
    jboolean jstt = stt;
    p->object.callMethod<jboolean>(__FUNCTION__, "(Z)Z", jstt );
}

TonToolkitJavaLayer::~TonToolkitJavaLayer()
{
    java_layers_objects.remove(this);
    delete p;
}

static void noteRecieved( JNIEnv *env, jobject obj ,jstring title, jstring msg )
{
    Q_UNUSED(obj)
    jboolean a;
    jboolean b;
    const char *t = env->GetStringUTFChars(title,&a);
    const char *m = env->GetStringUTFChars(msg,&b);

    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->incomingShare( QString(t), QString(m) );

    if( java_layers_objects.isEmpty() )
        java_layer_inc_share_buffer << QPair<QString,QString>( QString(t), QString(m) );
}

static void imageRecieved( JNIEnv *env, jobject obj ,jstring jpath )
{
    Q_UNUSED(obj)
    jboolean a;
    const char *p = env->GetStringUTFChars(jpath,&a);

    QString path = QString("/sdcard/TonToolkit/%1.jpeg").arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    QFile().rename(QString(p),path);

    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->incomingImage(path);

    if( java_layers_objects.isEmpty() )
        java_layer_inc_image_buffer << path;
}

static void selectImageResult( JNIEnv *env, jobject obj ,jstring path )
{
    Q_UNUSED(obj)
    jboolean a;
    const char *p = env->GetStringUTFChars(path,&a);

    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->selectImageResult( QString(p) );
}

static void activityPaused( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityPaused();
}

static void activityStopped( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityStopped();
}

static void activityResumed( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityResumed();
}

static void activityStarted( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityStarted();
}

static void activityRestarted( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityRestarted();
}

static void activityDestroyed( JNIEnv *env, jobject obj )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->activityDestroyed();
}

static void keyboardVisiblityChanged( JNIEnv *env, jobject obj, jint height )
{
    Q_UNUSED(env)
    Q_UNUSED(obj)
    for(TonToolkitJavaLayer *sjl: java_layers_objects)
        Q_EMIT sjl->keyboardVisiblityChanged(height);
}

bool tontoolkit_jlayer_registerNativeMethods() {
    if(tontoolkit_jlayer_native_methods_registered)
        return true;

    JNINativeMethod methods[] {{"_sendNote", "(Ljava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void *>(noteRecieved)},
                               {"_sendImage", "(Ljava/lang/String;)V", reinterpret_cast<void *>(imageRecieved)},
                               {"_selectImageResult", "(Ljava/lang/String;)V", reinterpret_cast<void *>(selectImageResult)},
                               {"_activityPaused", "()V", reinterpret_cast<void *>(activityPaused)},
                               {"_activityStopped", "()V", reinterpret_cast<void *>(activityStopped)},
                               {"_activityResumed", "()V", reinterpret_cast<void *>(activityResumed)},
                               {"_activityStarted", "()V", reinterpret_cast<void *>(activityStarted)},
                               {"_activityRestarted", "()V", reinterpret_cast<void *>(activityRestarted)},
                               {"_activityDestroyed", "()V", reinterpret_cast<void *>(activityDestroyed)},
                               {"_keyboardVisiblityChanged", "(I)V", reinterpret_cast<void *>(keyboardVisiblityChanged)}};

    QAndroidJniObject javaClass("io/tontoolkit/android/TonToolkitJavaLayer");
    QAndroidJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());

    env->RegisterNatives(objectClass, methods, sizeof(methods) / sizeof(methods[0]));

    env->DeleteLocalRef(objectClass);
    return true;
}
