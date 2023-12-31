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

#ifndef QPIXMAPCACHE_H
#define QPIXMAPCACHE_H

#include <QtGui/qtguiglobal.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_NAMESPACE


class Q_GUI_EXPORT QPixmapCache
{
public:
    class KeyData;
    class Q_GUI_EXPORT Key
    {
    public:
        Key();
        Key(const Key &other);
        Key(Key &&other) noexcept : d(other.d) { other.d = nullptr; }
        Key &operator =(Key &&other) noexcept { swap(other); return *this; }
        ~Key();
        bool operator ==(const Key &key) const;
        inline bool operator !=(const Key &key) const
        { return !operator==(key); }
        Key &operator =(const Key &other);

        void swap(Key &other) noexcept { qSwap(d, other.d); }
        bool isValid() const noexcept;

    private:
        KeyData *d;
        friend class QPMCache;
        friend class QPixmapCache;
    };

    static int cacheLimit();
    static void setCacheLimit(int);
#if QT_DEPRECATED_SINCE(5, 13)
    QT_DEPRECATED_X("Use bool find(const QString &, QPixmap *) instead")
    static QPixmap *find(const QString &key);
    QT_DEPRECATED_X("Use bool find(const QString &, QPixmap *) instead")
    static bool find(const QString &key, QPixmap &pixmap);
#endif
    static bool find(const QString &key, QPixmap *pixmap);
    static bool find(const Key &key, QPixmap *pixmap);
    static bool insert(const QString &key, const QPixmap &pixmap);
    static Key insert(const QPixmap &pixmap);
    static bool replace(const Key &key, const QPixmap &pixmap);
    static void remove(const QString &key);
    static void remove(const Key &key);
    static void clear();

#ifdef Q_TEST_QPIXMAPCACHE
    static void flushDetachedPixmaps();
    static int totalUsed();
#endif
};
Q_DECLARE_SHARED_NOT_MOVABLE_UNTIL_QT6(QPixmapCache::Key)

QT_END_NAMESPACE

#endif // QPIXMAPCACHE_H
