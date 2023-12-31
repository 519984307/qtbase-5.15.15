/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusvirtualobject.h"

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

/*!
    Constructs a QDBusVirtualObject with \a parent.
*/
QDBusVirtualObject::QDBusVirtualObject(QObject *parent) :
    QObject(parent)
{
}

/*!
    Destroys the object, deleting all of its child objects.
*/
QDBusVirtualObject::~QDBusVirtualObject()
{
}

QT_END_NAMESPACE

#include "moc_qdbusvirtualobject.cpp"

/*!
    \class QDBusVirtualObject
    \inmodule QtDBus
    \since 5.1

    \brief The QDBusVirtualObject class is used to handle several DBus paths with one class.
*/

/*!
    \fn bool QDBusVirtualObject::handleMessage(const QDBusMessage &message, const QDBusConnection &connection) = 0

    This function needs to handle all messages to the path of the
    virtual object, when the SubPath option is specified.
    The service, path, interface and methos are all part of the \a message.
    Parameter \a connection is the connection handle.
    Must return \c true when the message is handled, otherwise \c false (will generate dbus error message).
*/


/*!
    \fn QString QDBusVirtualObject::introspect(const QString &path) const

    This function needs to handle the introspection of the
    virtual object on \a path. It must return xml of the form:

    \code
<interface name="org.qtproject.QtDBus.MyObject" >
    <property access="readwrite" type="i" name="prop1" />
</interface>
    \endcode

    If you pass the SubPath option, this introspection has to include all child nodes.
    Otherwise QDBus handles the introspection of the child nodes.
*/

#endif // QT_NO_DBUS
