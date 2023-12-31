/***************************************************************************
**
** Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Tobias Koenig <tobias.koenig@kdab.com>
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

#include "qhaikurasterbackingstore.h"
#include "qhaikurasterwindow.h"

#include <Bitmap.h>
#include <View.h>

QT_BEGIN_NAMESPACE

QHaikuRasterBackingStore::QHaikuRasterBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
    , m_bitmap(nullptr)
{
}

QHaikuRasterBackingStore::~QHaikuRasterBackingStore()
{
    delete m_bitmap;
    m_bitmap = nullptr;
}

QPaintDevice *QHaikuRasterBackingStore::paintDevice()
{
    if (!m_bufferSize.isEmpty() && m_bitmap)
        return m_buffer.image();

    return nullptr;
}

void QHaikuRasterBackingStore::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(region);
    Q_UNUSED(offset);

    if (!window)
        return;

    QHaikuRasterWindow *targetWindow = static_cast<QHaikuRasterWindow*>(window->handle());

    BView *view = targetWindow->nativeViewHandle();

    view->LockLooper();
    view->DrawBitmap(m_bitmap);
    view->UnlockLooper();
}

void QHaikuRasterBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
    Q_UNUSED(staticContents);

    if (m_bufferSize == size)
        return;

    delete m_bitmap;
    m_bitmap = new BBitmap(BRect(0, 0, size.width(), size.height()), B_RGB32, false, true);
    m_buffer = QHaikuBuffer(m_bitmap);
    m_bufferSize = size;
}

QT_END_NAMESPACE
