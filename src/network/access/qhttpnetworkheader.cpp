/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qhttpnetworkheader_p.h"

#include <algorithm>

QT_BEGIN_NAMESPACE

QHttpNetworkHeaderPrivate::QHttpNetworkHeaderPrivate(const QUrl &newUrl)
    :url(newUrl)
{
}

QHttpNetworkHeaderPrivate::QHttpNetworkHeaderPrivate(const QHttpNetworkHeaderPrivate &other)
    :QSharedData(other)
{
    url = other.url;
    fields = other.fields;
}

qint64 QHttpNetworkHeaderPrivate::contentLength() const
{
    bool ok = false;
    // We are not using the headerField() method here because servers might send us multiple content-length
    // headers which is crap (see QTBUG-15311). Therefore just take the first content-length header field.
    QByteArray value;
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = fields.constBegin(),
                                                        end = fields.constEnd();
    for ( ; it != end; ++it)
        if (it->first.compare("content-length", Qt::CaseInsensitive) == 0) {
            value = it->second;
            break;
        }

    qint64 length = value.toULongLong(&ok);
    if (ok)
        return length;
    return -1; // the header field is not set
}

void QHttpNetworkHeaderPrivate::setContentLength(qint64 length)
{
    setHeaderField("Content-Length", QByteArray::number(length));
}

QByteArray QHttpNetworkHeaderPrivate::headerField(const QByteArray &name, const QByteArray &defaultValue) const
{
    QList<QByteArray> allValues = headerFieldValues(name);
    if (allValues.isEmpty())
        return defaultValue;
    else
        return allValues.join(", ");
}

QList<QByteArray> QHttpNetworkHeaderPrivate::headerFieldValues(const QByteArray &name) const
{
    QList<QByteArray> result;
    QList<QPair<QByteArray, QByteArray> >::ConstIterator it = fields.constBegin(),
                                                        end = fields.constEnd();
    for ( ; it != end; ++it)
        if (name.compare(it->first, Qt::CaseInsensitive) == 0)
            result += it->second;

    return result;
}

void QHttpNetworkHeaderPrivate::setHeaderField(const QByteArray &name, const QByteArray &data)
{
    auto firstEqualsName = [&name](const QPair<QByteArray, QByteArray> &header) {
        return name.compare(header.first, Qt::CaseInsensitive) == 0;
    };
    fields.erase(std::remove_if(fields.begin(), fields.end(),
                                firstEqualsName),
                 fields.end());
    fields.append(qMakePair(name, data));
}

void QHttpNetworkHeaderPrivate::prependHeaderField(const QByteArray &name, const QByteArray &data)
{
    fields.prepend(qMakePair(name, data));
}

void QHttpNetworkHeaderPrivate::clearHeaders()
{
    fields.clear();
}

bool QHttpNetworkHeaderPrivate::operator==(const QHttpNetworkHeaderPrivate &other) const
{
   return (url == other.url);
}


QT_END_NAMESPACE
