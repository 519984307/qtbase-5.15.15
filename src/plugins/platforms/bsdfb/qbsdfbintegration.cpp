/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2015-2016 Oleksandr Tymoshenko <gonzo@bluezbox.com>
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

#include "qbsdfbintegration.h"
#include "qbsdfbscreen.h"

#include <QtFontDatabaseSupport/private/qgenericunixfontdatabase_p.h>
#include <QtServiceSupport/private/qgenericunixservices_p.h>
#include <QtEventDispatcherSupport/private/qgenericunixeventdispatcher_p.h>

#include <QtFbSupport/private/qfbvthandler_p.h>
#include <QtFbSupport/private/qfbbackingstore_p.h>
#include <QtFbSupport/private/qfbwindow_p.h>
#include <QtFbSupport/private/qfbcursor_p.h>

#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatforminputcontext.h>
#include <qpa/qplatforminputcontextfactory_p.h>
#include <qpa/qwindowsysteminterface.h>

#if QT_CONFIG(tslib)
#include <QtInputSupport/private/qtslib_p.h>
#endif

QT_BEGIN_NAMESPACE

QBsdFbIntegration::QBsdFbIntegration(const QStringList &paramList)
{
    m_fontDb.reset(new QGenericUnixFontDatabase);
    m_services.reset(new QGenericUnixServices);
    m_primaryScreen.reset(new QBsdFbScreen(paramList));
}

QBsdFbIntegration::~QBsdFbIntegration()
{
    QWindowSystemInterface::handleScreenRemoved(m_primaryScreen.take());
}

void QBsdFbIntegration::initialize()
{
    if (m_primaryScreen->initialize())
        QWindowSystemInterface::handleScreenAdded(m_primaryScreen.data());
    else
        qWarning("bsdfb: Failed to initialize screen");

    m_inputContext.reset(QPlatformInputContextFactory::create());
    m_nativeInterface.reset(new QPlatformNativeInterface);
    m_vtHandler.reset(new QFbVtHandler);

    if (!qEnvironmentVariableIntValue("QT_QPA_FB_DISABLE_INPUT"))
        createInputHandlers();
}

bool QBsdFbIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps:
        return true;
    case WindowManagement:
        return false;
    default:
        return QPlatformIntegration::hasCapability(cap);
    }
}

QPlatformBackingStore *QBsdFbIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new QFbBackingStore(window);
}

QPlatformWindow *QBsdFbIntegration::createPlatformWindow(QWindow *window) const
{
    return new QFbWindow(window);
}

QAbstractEventDispatcher *QBsdFbIntegration::createEventDispatcher() const
{
    return createUnixEventDispatcher();
}

QList<QPlatformScreen *> QBsdFbIntegration::screens() const
{
    QList<QPlatformScreen *> list;
    list.append(m_primaryScreen.data());
    return list;
}

QPlatformFontDatabase *QBsdFbIntegration::fontDatabase() const
{
    return m_fontDb.data();
}

QPlatformServices *QBsdFbIntegration::services() const
{
    return m_services.data();
}

void QBsdFbIntegration::createInputHandlers()
{
#if QT_CONFIG(tslib)
    const bool useTslib = qEnvironmentVariableIntValue("QT_QPA_FB_TSLIB");
    if (useTslib)
        new QTsLibMouseHandler(QLatin1String("TsLib"), QString());
#endif
}

QPlatformNativeInterface *QBsdFbIntegration::nativeInterface() const
{
    return m_nativeInterface.data();
}

QT_END_NAMESPACE
