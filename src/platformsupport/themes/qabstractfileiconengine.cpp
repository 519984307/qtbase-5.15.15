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

#include "qabstractfileiconengine_p.h"

#include <qpixmapcache.h>

QT_BEGIN_NAMESPACE

/*!
    \class QAbstractFileIconEngine
    \brief Helper base class for retrieving icons for files for usage by QFileIconProvider and related.

    Reimplement availableSizes() and new virtual filePixmap() and return icons created
    with this engine from QPlatformTheme::fileIcon().

    Note: The class internally caches pixmaps for files by suffix (with the exception
    of some files on Windows), but not for directories (since directory icons may have
    overlay icons on Windows). You might want to cache pixmaps for directories
    in your implementation.

    \since 5.8
    \internal
    \sa QFileIconProvider::DontUseCustomDirectoryIcons, QPlatformTheme
    \ingroup qpa
*/
QPixmap QAbstractFileIconEngine::pixmap(const QSize &size, QIcon::Mode mode,
                                        QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    if (!size.isValid())
        return QPixmap();

    QString key = cacheKey();
    if (key.isEmpty())
        return filePixmap(size, mode, state);

    key += QLatin1Char('_') + QString::number(size.width());

    QPixmap result;
    if (!QPixmapCache::find(key, &result)) {
        result = filePixmap(size, mode, state);
        if (!result.isNull())
            QPixmapCache::insert(key, result);
    }

    return result;
}

QSize QAbstractFileIconEngine::actualSize(const QSize &size, QIcon::Mode mode,
                                          QIcon::State state)
{
    const QList<QSize> &sizes = availableSizes(mode, state);
    const int numberSizes = sizes.length();
    if (numberSizes == 0)
        return QSize();

    // Find the smallest available size whose area is still larger than the input
    // size. Otherwise, use the largest area available size. (We don't assume the
    // platform theme sizes are sorted, hence the extra logic.)
    const int sizeArea = size.width() * size.height();
    QSize actualSize = sizes.first();
    int actualArea = actualSize.width() * actualSize.height();
    for (int i = 1; i < numberSizes; ++i) {
        const QSize &s = sizes.at(i);
        const int a = s.width() * s.height();
        if ((sizeArea <= a && a < actualArea) || (actualArea < sizeArea && actualArea < a)) {
            actualSize = s;
            actualArea = a;
        }
    }

    if (!actualSize.isNull() && (actualSize.width() > size.width() || actualSize.height() > size.height()))
        actualSize.scale(size, Qt::KeepAspectRatio);

    return actualSize;
}

/* Reimplement to return a cache key for the entry. An empty result indicates
 * the icon should not be cached (for example, directory icons having custom icons). */
QString QAbstractFileIconEngine::cacheKey() const
{
    if (!m_fileInfo.isFile() || m_fileInfo.isSymLink() || m_fileInfo.isExecutable())
        return QString();

    const QString &suffix = m_fileInfo.suffix();
    return QLatin1String("qt_.")
        + (suffix.isEmpty() ? m_fileInfo.fileName() : suffix); // handle "Makefile"                                    ;)
}

QT_END_NAMESPACE
