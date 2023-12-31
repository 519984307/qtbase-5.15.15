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

#include "qwindowsdirect2dbackingstore.h"
#include "qwindowsdirect2dplatformpixmap.h"
#include "qwindowsdirect2dintegration.h"
#include "qwindowsdirect2dcontext.h"
#include "qwindowsdirect2dpaintdevice.h"
#include "qwindowsdirect2dbitmap.h"
#include "qwindowsdirect2ddevicecontext.h"
#include "qwindowsdirect2dwindow.h"

#include "qwindowscontext.h"

#include <QtGui/qpainter.h>
#include <QtGui/qwindow.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QWindowsDirect2DBackingStore
    \brief Backing store for windows.
    \internal
*/

static inline QWindowsDirect2DPlatformPixmap *platformPixmap(QPixmap *p)
{
    return static_cast<QWindowsDirect2DPlatformPixmap *>(p->handle());
}

static inline QWindowsDirect2DBitmap *bitmap(QPixmap *p)
{
    return platformPixmap(p)->bitmap();
}

static inline QWindowsDirect2DWindow *nativeWindow(QWindow *window)
{
    return static_cast<QWindowsDirect2DWindow *>(window->handle());
}

QWindowsDirect2DBackingStore::QWindowsDirect2DBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
}

QWindowsDirect2DBackingStore::~QWindowsDirect2DBackingStore()
{
}

void QWindowsDirect2DBackingStore::beginPaint(const QRegion &region)
{
    QPixmap *pixmap = nativeWindow(window())->pixmap();
    bitmap(pixmap)->deviceContext()->begin();

    QPainter painter(pixmap);
    QColor clear(Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_Source);

    for (const QRect &r : region)
        painter.fillRect(r, clear);
}

void QWindowsDirect2DBackingStore::endPaint()
{
    bitmap(nativeWindow(window())->pixmap())->deviceContext()->end();
}

QPaintDevice *QWindowsDirect2DBackingStore::paintDevice()
{
    return nativeWindow(window())->pixmap();
}

void QWindowsDirect2DBackingStore::flush(QWindow *targetWindow, const QRegion &region, const QPoint &offset)
{
    if (targetWindow != window()) {
        QSharedPointer<QWindowsDirect2DBitmap> copy(nativeWindow(window())->copyBackBuffer());
        nativeWindow(targetWindow)->flush(copy.data(), region, offset);
    }

    nativeWindow(targetWindow)->present(region);
}

void QWindowsDirect2DBackingStore::resize(const QSize &size, const QRegion &region)
{
    QPixmap old = nativeWindow(window())->pixmap()->copy();

    nativeWindow(window())->resizeSwapChain(size);
    QPixmap *newPixmap = nativeWindow(window())->pixmap();

    if (!old.isNull()) {
        for (const QRect &rect : region)
            platformPixmap(newPixmap)->copy(old.handle(), rect);
    }
}

QImage QWindowsDirect2DBackingStore::toImage() const
{
    return nativeWindow(window())->pixmap()->toImage();
}

QT_END_NAMESPACE
