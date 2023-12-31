/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
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

#include "qdbusserver.h"
#include "qdbusconnection_p.h"
#include "qdbusconnectionmanager_p.h"
#include "qdbusutil_p.h"

#include <QtCore/private/qlocking_p.h>

#ifndef QT_NO_DBUS

QT_BEGIN_NAMESPACE

/*!
    \class QDBusServer
    \inmodule QtDBus

    \brief The QDBusServer class provides peer-to-peer communication
    between processes on the same computer.
*/

/*!
    Constructs a QDBusServer with the given \a address, and the given
    \a parent.
*/
QDBusServer::QDBusServer(const QString &address, QObject *parent)
    : QObject(parent), d(nullptr)
{
    if (address.isEmpty())
        return;

    if (!qdbus_loadLibDBus())
        return;

    QDBusConnectionManager *instance = QDBusConnectionManager::instance();
    if (!instance)
        return;

    emit instance->serverRequested(address, this);
    QObject::connect(d, SIGNAL(newServerConnection(QDBusConnectionPrivate*)),
                     this, SLOT(_q_newConnection(QDBusConnectionPrivate*)), Qt::QueuedConnection);
}

/*!
    Constructs a QDBusServer with the given \a parent. The server will listen
    for connections in \c {/tmp} (on Unix systems) or on a TCP port bound to
    localhost (elsewhere).
*/
QDBusServer::QDBusServer(QObject *parent)
    : QObject(parent), d(nullptr)
{
#ifdef Q_OS_UNIX
    // Use Unix sockets on Unix systems only
    const QString address = QStringLiteral("unix:tmpdir=/tmp");
#else
    const QString address = QStringLiteral("tcp:");
#endif

    if (!qdbus_loadLibDBus())
        return;

    QDBusConnectionManager *instance = QDBusConnectionManager::instance();
    if (!instance)
        return;

    emit instance->serverRequested(address, this);
    QObject::connect(d, SIGNAL(newServerConnection(QDBusConnectionPrivate*)),
                     this, SLOT(_q_newConnection(QDBusConnectionPrivate*)), Qt::QueuedConnection);
}

/*!
    Destructs a QDBusServer
*/
QDBusServer::~QDBusServer()
{
    QMutex *managerMutex = nullptr;
    if (QDBusConnectionManager::instance())
        managerMutex = &QDBusConnectionManager::instance()->mutex;
    QMutexLocker locker(managerMutex);
    QWriteLocker writeLocker(&d->lock);
    if (QDBusConnectionManager::instance()) {
        for (const QString &name : qAsConst(d->serverConnectionNames))
            QDBusConnectionManager::instance()->removeConnection(name);
        d->serverConnectionNames.clear();
        locker.unlock();
    }
    d->serverObject = nullptr;
    d->ref.storeRelaxed(0);
    d->deleteLater();
}

/*!
    Returns \c true if this QDBusServer object is connected.

    If it isn't connected, you need to call the constructor again.
*/
bool QDBusServer::isConnected() const
{
    return d && d->server && q_dbus_server_get_is_connected(d->server);
}

/*!
    Returns the last error that happened in this server.

    This function is provided for low-level code.
*/
QDBusError QDBusServer::lastError() const
{
    return d ? d->lastError : QDBusError(QDBusError::Disconnected, QDBusUtil::disconnectedErrorMessage());
}

/*!
    Returns the address this server is associated with.
*/
QString QDBusServer::address() const
{
    QString addr;
    if (d && d->server) {
        char *c = q_dbus_server_get_address(d->server);
        addr = QString::fromUtf8(c);
        q_dbus_free(c);
    }

    return addr;
}

/*!
    \since 5.3

    If \a value is set to true, an incoming connection can proceed even if the
    connecting client is not authenticated as a user.

    By default, this value is false.

    \sa isAnonymousAuthenticationAllowed()
*/
void QDBusServer::setAnonymousAuthenticationAllowed(bool value)
{
    d->anonymousAuthenticationAllowed = value;
}

/*!
    \since 5.3

    Returns true if anonymous authentication is allowed.

    \sa setAnonymousAuthenticationAllowed()
*/
bool QDBusServer::isAnonymousAuthenticationAllowed() const
{
    return d->anonymousAuthenticationAllowed;
}

/*!
  \fn void QDBusServer::newConnection(const QDBusConnection &connection)

  This signal is emitted when a new client connection \a connection is
  established to the server.
 */

QT_END_NAMESPACE

#include "moc_qdbusserver.cpp"

#endif // QT_NO_DBUS
