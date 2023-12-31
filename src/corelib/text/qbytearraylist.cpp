/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2014 by Southwest Research Institute (R)
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <qbytearraylist.h>

QT_BEGIN_NAMESPACE

/*! \typedef QByteArrayListIterator
    \relates QByteArrayList

    The QByteArrayListIterator type definition provides a Java-style const
    iterator for QByteArrayList.

    QByteArrayList provides both \l{Java-style iterators} and
    \l{STL-style iterators}. The Java-style const iterator is simply
    a type definition for QListIterator<QByteArray>.

    \sa QMutableByteArrayListIterator, QByteArrayList::const_iterator
*/

/*! \typedef QMutableByteArrayListIterator
    \relates QByteArrayList

    The QByteArrayListIterator type definition provides a Java-style
    non-const iterator for QByteArrayList.

    QByteArrayList provides both \l{Java-style iterators} and
    \l{STL-style iterators}. The Java-style non-const iterator is
    simply a type definition for QMutableListIterator<QByteArray>.

    \sa QByteArrayListIterator, QByteArrayList::iterator
*/

/*!
    \class QByteArrayList
    \inmodule QtCore
    \since 5.4
    \brief The QByteArrayList class provides a list of byte arrays.

    \ingroup tools
    \ingroup shared
    \ingroup string-processing

    \reentrant

    QByteArrayList is actually just a QList<QByteArray>. It is documented as a
    full class just for simplicity of documenting the member methods that exist
    only in QList<QByteArray>.

    All of QList's functionality also applies to QByteArrayList. For example, you
    can use isEmpty() to test whether the list is empty, and you can call
    functions like append(), prepend(), insert(), replace(), removeAll(),
    removeAt(), removeFirst(), removeLast(), and removeOne() to modify a
    QByteArrayList. In addition, QByteArrayList provides several join()
    methods for concatenating the list into a single QByteArray.

    The purpose of QByteArrayList is quite different from that of QStringList.
    Whereas QStringList has many methods for manipulation of elements within
    the list, QByteArrayList does not.
    Normally, QStringList should be used whenever working with a list of printable
    strings. QByteArrayList should be used to handle and efficiently join large blobs
    of binary data, as when sequentially receiving serialized data through a
    QIODevice.

    \sa QByteArray, QStringList
*/

/*!
    \fn QByteArray QByteArrayList::join() const

    Joins all the byte arrays into a single byte array.
*/

/*!
    \fn QByteArray QByteArrayList::join(const QByteArray &separator) const

    Joins all the byte arrays into a single byte array with each
    element separated by the given \a separator.
*/

/*!
    \fn QByteArray QByteArrayList::join(char separator) const

    Joins all the byte arrays into a single byte array with each
    element separated by the given \a separator.
*/

static int QByteArrayList_joinedSize(const QByteArrayList *that, int seplen)
{
    int totalLength = 0;
    const int size = that->size();

    for (int i = 0; i < size; ++i)
        totalLength += that->at(i).size();

    if (size > 0)
        totalLength += seplen * (size - 1);

    return totalLength;
}

QByteArray QtPrivate::QByteArrayList_join(const QByteArrayList *that, const char *sep, int seplen)
{
    QByteArray res;
    if (const int joinedSize = QByteArrayList_joinedSize(that, seplen))
        res.reserve(joinedSize); // don't call reserve(0) - it allocates one byte for the NUL
    const int size = that->size();
    for (int i = 0; i < size; ++i) {
        if (i)
            res.append(sep, seplen);
        res += that->at(i);
    }
    return res;
}

/*!
    \fn int QByteArrayList::indexOf(const char *needle, int from) const

    Returns the index position of the first occurrence of \a needle in
    the list, searching forward from index position \a from. Returns
    -1 if no item matched.

    \a needle must be NUL-terminated.

    This overload doesn't require creating a QByteArray, thus saving a
    memory allocation and some CPU time.

    \since 5.13
    \overload
*/

int QtPrivate::QByteArrayList_indexOf(const QByteArrayList *that, const char *needle, int from)
{
    const auto it = std::find_if(that->begin() + from, that->end(), [needle](const QByteArray &item) { return item == needle; });
    return it == that->end() ? -1 : int(std::distance(that->begin(), it));
}

QT_END_NAMESPACE
