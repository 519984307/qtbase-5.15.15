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

#include "qfbwindow_p.h"
#include "qfbscreen_p.h"

#include <QtGui/QScreen>
#include <qpa/qwindowsysteminterface.h>

QT_BEGIN_NAMESPACE

static QBasicAtomicInt winIdGenerator = Q_BASIC_ATOMIC_INITIALIZER(0);

QFbWindow::QFbWindow(QWindow *window)
    : QPlatformWindow(window), mBackingStore(0), mWindowState(Qt::WindowNoState)
{
    mWindowId = winIdGenerator.fetchAndAddRelaxed(1) + 1;
}

QFbWindow::~QFbWindow()
{
}

QFbScreen *QFbWindow::platformScreen() const
{
    return static_cast<QFbScreen *>(window()->screen()->handle());
}

void QFbWindow::setGeometry(const QRect &rect)
{
    // store previous geometry for screen update
    mOldGeometry = geometry();

    QWindowSystemInterface::handleGeometryChange(window(), rect);

    QPlatformWindow::setGeometry(rect);

    if (mOldGeometry != rect)
        QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));
}

void QFbWindow::setVisible(bool visible)
{
    QRect newGeom;
    QFbScreen *fbScreen = platformScreen();
    if (visible) {
        bool convOk = false;
        static bool envDisableForceFullScreen = qEnvironmentVariableIntValue("QT_QPA_FB_FORCE_FULLSCREEN", &convOk) == 0 && convOk;
        const bool platformDisableForceFullScreen = fbScreen->flags().testFlag(QFbScreen::DontForceFirstWindowToFullScreen);
        const bool forceFullScreen = !envDisableForceFullScreen && !platformDisableForceFullScreen && fbScreen->windowCount() == 0;
        if (forceFullScreen || (mWindowState & Qt::WindowFullScreen))
            newGeom = platformScreen()->geometry();
        else if (mWindowState & Qt::WindowMaximized)
            newGeom = platformScreen()->availableGeometry();
    }
    QPlatformWindow::setVisible(visible);

    if (visible)
        fbScreen->addWindow(this);
    else
        fbScreen->removeWindow(this);

    if (!newGeom.isEmpty())
        setGeometry(newGeom); // may or may not generate an expose

    if (newGeom.isEmpty() || newGeom == mOldGeometry) {
        // QWindow::isExposed() maps to QWindow::visible() by default so simply
        // generating an expose event regardless of this being a show or hide is
        // just what is needed here.
        QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));
    }
}

void QFbWindow::setWindowState(Qt::WindowStates state)
{
    QPlatformWindow::setWindowState(state);
    mWindowState = state;
}

void QFbWindow::setWindowFlags(Qt::WindowFlags flags)
{
    mWindowFlags = flags;
}

Qt::WindowFlags QFbWindow::windowFlags() const
{
    return mWindowFlags;
}

void QFbWindow::raise()
{
    platformScreen()->raise(this);
    QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));
}

void QFbWindow::lower()
{
    platformScreen()->lower(this);
    QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));
}

void QFbWindow::repaint(const QRegion &region)
{
    const QRect currentGeometry = geometry();
    const QRect dirtyClient = region.boundingRect();
    const QRect dirtyRegion = dirtyClient.translated(currentGeometry.topLeft());
    const QRect oldGeometryLocal = mOldGeometry;
    mOldGeometry = currentGeometry;
    // If this is a move, redraw the previous location
    if (oldGeometryLocal != currentGeometry)
        platformScreen()->setDirty(oldGeometryLocal);
    platformScreen()->setDirty(dirtyRegion);
}

QT_END_NAMESPACE
