/****************************************************************************
**
** Copyright (C) 2014 Jeremy Lainé <jeremy.laine@m4x.org>
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

#ifndef QSSLSOCKET_MAC_P_H
#define QSSLSOCKET_MAC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QtNetwork library.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtNetwork/private/qtnetworkglobal_p.h>
#include <QtCore/qstring.h>
#include <QtCore/qglobal.h>
#include <QtCore/qlist.h>

#include "qabstractsocket.h"
#include "qsslsocket_p.h"

#include <Security/Security.h>
#include <Security/SecureTransport.h>

QT_BEGIN_NAMESPACE

class QSecureTransportContext
{
public:
    explicit QSecureTransportContext(SSLContextRef context);
    ~QSecureTransportContext();

    operator SSLContextRef () const;
    void reset(SSLContextRef newContext);
private:
    SSLContextRef context;

    Q_DISABLE_COPY_MOVE(QSecureTransportContext)
};

class QSslSocketBackendPrivate : public QSslSocketPrivate
{
    Q_DECLARE_PUBLIC(QSslSocket)
public:
    QSslSocketBackendPrivate();
    virtual ~QSslSocketBackendPrivate();

    // Final-overriders (QSslSocketPrivate):
    void continueHandshake() override;
    void disconnected() override;
    void disconnectFromHost() override;
    QSslCipher sessionCipher() const override;
    QSsl::SslProtocol sessionProtocol() const override;
    void startClientEncryption() override;
    void startServerEncryption() override;
    void transmit() override;

    static QList<QSslError> verify(QList<QSslCertificate> certificateChain,
                                   const QString &hostName);

    static bool importPkcs12(QIODevice *device,
                             QSslKey *key, QSslCertificate *cert,
                             QList<QSslCertificate> *caCertificates,
                             const QByteArray &passPhrase);

    static QSslCipher QSslCipher_from_SSLCipherSuite(SSLCipherSuite cipher);

private:
    // SSL context management/properties:
    bool initSslContext();
    void destroySslContext();
    bool setSessionCertificate(QString &errorDescription,
                               QAbstractSocket::SocketError &errorCode);
    bool setSessionProtocol();
    // Aux. functions to do a verification during handshake phase:
    bool canIgnoreTrustVerificationFailure() const;
    bool verifySessionProtocol() const;
    bool verifyPeerTrust();

    bool checkSslErrors();
    bool startHandshake();

    bool isHandshakeComplete() const {return connectionEncrypted && !renegotiating;}

    // IO callbacks:
    static OSStatus ReadCallback(QSslSocketBackendPrivate *socket, char *data, size_t *dataLength);
    static OSStatus WriteCallback(QSslSocketBackendPrivate *plainSocket, const char *data, size_t *dataLength);

    QSecureTransportContext context;
    bool renegotiating = false;

    Q_DISABLE_COPY_MOVE(QSslSocketBackendPrivate)
};

QT_END_NAMESPACE

#endif
