/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
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

#ifndef QSTRINGLITERAL_H
#define QSTRINGLITERAL_H

#include <QtCore/qarraydata.h>

#if 0
#pragma qt_class(QStringLiteral)
#endif

QT_BEGIN_NAMESPACE

typedef QTypedArrayData<ushort> QStringData;

// all our supported compilers support Unicode string literals,
// even if their Q_COMPILER_UNICODE_STRING has been revoked due
// to lacking stdlib support. But QStringLiteral only needs the
// core language feature, so just use u"" here unconditionally:

typedef char16_t qunicodechar;

Q_STATIC_ASSERT_X(sizeof(qunicodechar) == 2,
        "qunicodechar must typedef an integral type of size 2");

#define QT_UNICODE_LITERAL(str) u"" str
#define QStringLiteral(str) \
    ([]() noexcept -> QString { \
        enum { Size = sizeof(QT_UNICODE_LITERAL(str))/2 - 1 }; \
        static const QStaticStringData<Size> qstring_literal = { \
            Q_STATIC_STRING_DATA_HEADER_INITIALIZER(Size), \
            QT_UNICODE_LITERAL(str) }; \
        QStringDataPtr holder = { qstring_literal.data_ptr() }; \
        return QString(holder); \
    }()) \
    /**/

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, offset) \
    { Q_REFCOUNT_INITIALIZE_STATIC, size, 0, 0, offset } \
    /**/

#define Q_STATIC_STRING_DATA_HEADER_INITIALIZER(size) \
    Q_STATIC_STRING_DATA_HEADER_INITIALIZER_WITH_OFFSET(size, sizeof(QStringData)) \
    /**/

#if QT_DEPRECATED_SINCE(5, 14)
# define QStringViewLiteral(str) QStringView(QT_UNICODE_LITERAL(str), QtPrivate::Deprecated)
#endif

template <int N>
struct QStaticStringData
{
    QArrayData str;
    qunicodechar data[N + 1];

    QStringData *data_ptr() const
    {
        Q_ASSERT(str.ref.isStatic());
        return const_cast<QStringData *>(static_cast<const QStringData*>(&str));
    }
};

struct QStringDataPtr
{
    QStringData *ptr;
};

QT_END_NAMESPACE

#endif // QSTRINGLITERAL_H
