/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#ifndef QXCBNATIVEINTERFACE_H
#define QXCBNATIVEINTERFACE_H

#include <qpa/qplatformnativeinterface.h>
#include <xcb/xcb.h>

#include <QtCore/QRect>

#include "qxcbexport.h"
#include "qxcbconnection.h"

QT_BEGIN_NAMESPACE

class QXcbScreen;
class QXcbNativeInterfaceHandler;

class Q_XCB_EXPORT QXcbNativeInterface : public QPlatformNativeInterface
{
    Q_OBJECT
public:
    enum ResourceType {
        Display,
        Connection,
        Screen,
        AppTime,
        AppUserTime,
        ScreenHintStyle,
        StartupId,
        TrayWindow,
        GetTimestamp,
        X11Screen,
        RootWindow,
        ScreenSubpixelType,
        ScreenAntialiasingEnabled,
        AtspiBus,
        CompositingEnabled,
        VkSurface,
        GeneratePeekerId,
        RemovePeekerId,
        PeekEventQueue
    };

    QXcbNativeInterface();

    void *nativeResourceForIntegration(const QByteArray &resource) override;
    void *nativeResourceForContext(const QByteArray &resourceString, QOpenGLContext *context) override;
    void *nativeResourceForScreen(const QByteArray &resource, QScreen *screen) override;
    void *nativeResourceForWindow(const QByteArray &resourceString, QWindow *window) override;
    void *nativeResourceForBackingStore(const QByteArray &resource, QBackingStore *backingStore) override;
#ifndef QT_NO_CURSOR
    void *nativeResourceForCursor(const QByteArray &resource, const QCursor &cursor) override;
#endif

    NativeResourceForIntegrationFunction nativeResourceFunctionForIntegration(const QByteArray &resource) override;
    NativeResourceForContextFunction nativeResourceFunctionForContext(const QByteArray &resource) override;
    NativeResourceForScreenFunction nativeResourceFunctionForScreen(const QByteArray &resource) override;
    NativeResourceForWindowFunction nativeResourceFunctionForWindow(const QByteArray &resource) override;
    NativeResourceForBackingStoreFunction nativeResourceFunctionForBackingStore(const QByteArray &resource) override;

    QFunctionPointer platformFunction(const QByteArray &function) const override;

    inline const QByteArray &nativeEventType() const { return m_nativeEventType; }

    void *displayForWindow(QWindow *window);
    void *connectionForWindow(QWindow *window);
    void *screenForWindow(QWindow *window);
    void *appTime(const QXcbScreen *screen);
    void *appUserTime(const QXcbScreen *screen);
    void *getTimestamp(const QXcbScreen *screen);
    void *startupId();
    void *x11Screen();
    void *rootWindow();
    void *display();
    void *atspiBus();
    void *connection();
    static void setStartupId(const char *);
    static void setAppTime(QScreen *screen, xcb_timestamp_t time);
    static void setAppUserTime(QScreen *screen, xcb_timestamp_t time);

    static qint32 generatePeekerId();
    static bool removePeekerId(qint32 peekerId);
    static bool peekEventQueue(QXcbEventQueue::PeekerCallback peeker, void *peekerData = nullptr,
                               QXcbEventQueue::PeekOptions option = QXcbEventQueue::PeekDefault,
                               qint32 peekerId = -1);

    Q_INVOKABLE QString dumpConnectionNativeWindows(const QXcbConnection *connection, WId root) const;
    Q_INVOKABLE QString dumpNativeWindows(WId root = 0) const;

    void addHandler(QXcbNativeInterfaceHandler *handler);
    void removeHandler(QXcbNativeInterfaceHandler *handler);
signals:
    void systemTrayWindowChanged(QScreen *screen);

private:
    const QByteArray m_nativeEventType = QByteArrayLiteral("xcb_generic_event_t");

    static QXcbScreen *qPlatformScreenForWindow(QWindow *window);

    QList<QXcbNativeInterfaceHandler *> m_handlers;
    NativeResourceForIntegrationFunction handlerNativeResourceFunctionForIntegration(const QByteArray &resource) const;
    NativeResourceForContextFunction handlerNativeResourceFunctionForContext(const QByteArray &resource) const;
    NativeResourceForScreenFunction handlerNativeResourceFunctionForScreen(const QByteArray &resource) const;
    NativeResourceForWindowFunction handlerNativeResourceFunctionForWindow(const QByteArray &resource) const;
    NativeResourceForBackingStoreFunction handlerNativeResourceFunctionForBackingStore(const QByteArray &resource) const;
    QFunctionPointer handlerPlatformFunction(const QByteArray &function) const;
    void *handlerNativeResourceForIntegration(const QByteArray &resource) const;
    void *handlerNativeResourceForContext(const QByteArray &resource, QOpenGLContext *context) const;
    void *handlerNativeResourceForScreen(const QByteArray &resource, QScreen *screen) const;
    void *handlerNativeResourceForWindow(const QByteArray &resource, QWindow *window) const;
    void *handlerNativeResourceForBackingStore(const QByteArray &resource, QBackingStore *backingStore) const;
};

QT_END_NAMESPACE

#endif // QXCBNATIVEINTERFACE_H