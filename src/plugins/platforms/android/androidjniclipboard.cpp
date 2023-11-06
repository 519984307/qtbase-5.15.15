/****************************************************************************
**
** Copyright (C) 2012 BogDan Vatra <bogdan@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "androidjniclipboard.h"
#include <QtCore/QUrl>
#include <QtCore/private/qjni_p.h>

QT_BEGIN_NAMESPACE

using namespace QtAndroid;
namespace QtAndroidClipboard
{
    QAndroidPlatformClipboard *m_manager = nullptr;

    static JNINativeMethod methods[] = {
        {"onClipboardDataChanged", "()V", (void *)onClipboardDataChanged}
    };

    void setClipboardManager(QAndroidPlatformClipboard *manager)
    {
        m_manager = manager;
        QJNIObjectPrivate::callStaticMethod<void>(applicationClass(), "registerClipboardManager");
        jclass appClass = QtAndroid::applicationClass();
        QJNIEnvironmentPrivate env;
        if (env->RegisterNatives(appClass, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
            __android_log_print(ANDROID_LOG_FATAL,"Qt", "RegisterNatives failed");
            return;
        }
    }
    void clearClipboardData()
    {
        QJNIObjectPrivate::callStaticMethod<void>(applicationClass(), "clearClipData");
    }
    void setClipboardMimeData(QMimeData *data)
    {
        clearClipboardData();
        if (data->hasUrls()) {
            QList<QUrl> urls = data->urls();
            for (const auto &u : qAsConst(urls)) {
                QJNIObjectPrivate::callStaticMethod<void>(applicationClass(), "setClipboardUri",
                                                          "(Ljava/lang/String;)V",
                                                          QJNIObjectPrivate::fromString(u.toEncoded()).object());
            }
        } else if (data->hasText()) { // hasText || hasUrls, so the order matter here.
            QJNIObjectPrivate::callStaticMethod<void>(applicationClass(),
                                                      "setClipboardText", "(Ljava/lang/String;)V",
                                                      QJNIObjectPrivate::fromString(data->text()).object());
        } else if (data->hasHtml()) {
            QJNIObjectPrivate::callStaticMethod<void>(applicationClass(),
                                                      "setClipboardHtml",
                                                      "(Ljava/lang/String;Ljava/lang/String;)V",
                                                      QJNIObjectPrivate::fromString(data->text()).object(),
                                                      QJNIObjectPrivate::fromString(data->html()).object());
        }
    }

    QMimeData *getClipboardMimeData()
    {
        QMimeData *data = new QMimeData;
        if (QJNIObjectPrivate::callStaticMethod<jboolean>(applicationClass(), "hasClipboardText")) {
            data->setText(QJNIObjectPrivate::callStaticObjectMethod(applicationClass(),
                                                                    "getClipboardText",
                                                                    "()Ljava/lang/String;").toString());
        }
        if (QJNIObjectPrivate::callStaticMethod<jboolean>(applicationClass(), "hasClipboardHtml")) {
            data->setHtml(QJNIObjectPrivate::callStaticObjectMethod(applicationClass(),
                                                                    "getClipboardHtml",
                                                                    "()Ljava/lang/String;").toString());
        }
        if (QJNIObjectPrivate::callStaticMethod<jboolean>(applicationClass(), "hasClipboardUri")) {
            QJNIObjectPrivate uris = QJNIObjectPrivate::callStaticObjectMethod(applicationClass(),
                                                                               "getClipboardUris",
                                                                               "()[Ljava/lang/String;");
            if (uris.isValid()) {
                QList<QUrl> urls;
                QJNIEnvironmentPrivate env;
                jobjectArray juris = static_cast<jobjectArray>(uris.object());
                const jint nUris = env->GetArrayLength(juris);
                urls.reserve(static_cast<int>(nUris));
                for (int i = 0; i < nUris; ++i)
                    urls << QUrl(QJNIObjectPrivate(env->GetObjectArrayElement(juris, i)).toString());
                data->setUrls(urls);
            }
        }
        return data;
    }

    void onClipboardDataChanged(JNIEnv */*env*/, jobject /*thiz*/)
    {
        m_manager->emitChanged(QClipboard::Clipboard);
    }
}

QT_END_NAMESPACE
