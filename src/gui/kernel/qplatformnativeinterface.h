/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPLATFORMNATIVEINTERFACE_H
#define QPLATFORMNATIVEINTERFACE_H

//
//  W A R N I N G
//  -------------
//
// This file is part of the QPA API and is not meant to be used
// in applications. Usage of this API may make your code
// source and binary incompatible with future versions of Qt.
//

#include <QtGui/qtguiglobal.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/QObject>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE


class QOpenGLContext;
class QScreen;
class QWindow;
class QPlatformWindow;
class QBackingStore;

class Q_GUI_EXPORT QPlatformNativeInterface : public QObject
{
    Q_OBJECT
public:
    virtual void *nativeResourceForIntegration(const QByteArray &resource);
    virtual void *nativeResourceForContext(const QByteArray &resource, QOpenGLContext *context);
    virtual void *nativeResourceForScreen(const QByteArray &resource, QScreen *screen);
    virtual void *nativeResourceForWindow(const QByteArray &resource, QWindow *window);
    virtual void *nativeResourceForBackingStore(const QByteArray &resource, QBackingStore *backingStore);
#ifndef QT_NO_CURSOR
    virtual void *nativeResourceForCursor(const QByteArray &resource, const QCursor &cursor);
#endif

    typedef void * (*NativeResourceForIntegrationFunction)();
    typedef void * (*NativeResourceForContextFunction)(QOpenGLContext *context);
    typedef void * (*NativeResourceForScreenFunction)(QScreen *screen);
    typedef void * (*NativeResourceForWindowFunction)(QWindow *window);
    typedef void * (*NativeResourceForBackingStoreFunction)(QBackingStore *backingStore);
    virtual NativeResourceForIntegrationFunction nativeResourceFunctionForIntegration(const QByteArray &resource);
    virtual NativeResourceForContextFunction nativeResourceFunctionForContext(const QByteArray &resource);
    virtual NativeResourceForScreenFunction nativeResourceFunctionForScreen(const QByteArray &resource);
    virtual NativeResourceForWindowFunction nativeResourceFunctionForWindow(const QByteArray &resource);
    virtual NativeResourceForBackingStoreFunction nativeResourceFunctionForBackingStore(const QByteArray &resource);

    virtual QFunctionPointer platformFunction(const QByteArray &function) const;

    virtual QVariantMap windowProperties(QPlatformWindow *window) const;
    virtual QVariant windowProperty(QPlatformWindow *window, const QString &name) const;
    virtual QVariant windowProperty(QPlatformWindow *window, const QString &name, const QVariant &defaultValue) const;
    virtual void setWindowProperty(QPlatformWindow *window, const QString &name, const QVariant &value);

Q_SIGNALS:
    void windowPropertyChanged(QPlatformWindow *window, const QString &propertyName);
};

QT_END_NAMESPACE

#endif // QPLATFORMNATIVEINTERFACE_H
