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

#include "qshapedpixmapdndwindow_p.h"

#include "qplatformwindow.h"

#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/QPalette>
#include <QtGui/QBitmap>

QT_BEGIN_NAMESPACE

QShapedPixmapWindow::QShapedPixmapWindow(QScreen *screen)
    : m_useCompositing(true)
{
    setScreen(screen);
    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    setFormat(format);
    setFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint
             | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
}

QShapedPixmapWindow::~QShapedPixmapWindow()
{
}

void QShapedPixmapWindow::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    if (!m_useCompositing) {
        const QBitmap mask = m_pixmap.mask();
        if (!mask.isNull()) {
            if (!handle())
                create();
            if (auto platformWindow = handle()) {
                const auto pixmapDpr = m_pixmap.devicePixelRatio();
                const auto winDpr = devicePixelRatio();
                const auto maskSize = (QSizeF(m_pixmap.size()) * winDpr / pixmapDpr).toSize();
                platformWindow->setMask(QBitmap(mask.scaled(maskSize)));
            }
        }
    }
}

void QShapedPixmapWindow::setHotspot(const QPoint &hotspot)
{
    m_hotSpot = hotspot;
}

void QShapedPixmapWindow::paintEvent(QPaintEvent *)
{
    if (!m_pixmap.isNull()) {
        const QRect rect(QPoint(0, 0), size());
        QPainter painter(this);
        if (m_useCompositing)
            painter.setCompositionMode(QPainter::CompositionMode_Source);
        else
            painter.fillRect(rect, QGuiApplication::palette().base());
        painter.drawPixmap(rect, m_pixmap);
    }
}

void QShapedPixmapWindow::updateGeometry(const QPoint &pos)
{
    QSize size(1, 1);
    if (!m_pixmap.isNull()) {
        size = qFuzzyCompare(m_pixmap.devicePixelRatio(), qreal(1.0))
            ? m_pixmap.size()
            : (QSizeF(m_pixmap.size()) / m_pixmap.devicePixelRatio()).toSize();
    }
    setGeometry(QRect(pos - m_hotSpot, size));
}

QT_END_NAMESPACE

#include "moc_qshapedpixmapdndwindow_p.cpp"
