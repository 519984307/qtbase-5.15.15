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

#ifndef QSSLSOCKET_SCHANNEL_P_H
#define QSSLSOCKET_SCHANNEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_REQUIRE_CONFIG(schannel);

#include <QtNetwork/private/qtnetworkglobal_p.h>

#include "qsslsocket_p.h"

#define SECURITY_WIN32
#include <security.h>
#include <schnlsp.h>
#undef SECURITY_WIN32

QT_BEGIN_NAMESPACE

class QSslSocketBackendPrivate final : public QSslSocketPrivate
{
    Q_DISABLE_COPY_MOVE(QSslSocketBackendPrivate)
    Q_DECLARE_PUBLIC(QSslSocket)
public:
    QSslSocketBackendPrivate();
    ~QSslSocketBackendPrivate();

    // Platform specific functions
    void startClientEncryption() override;
    void startServerEncryption() override;
    void transmit() override;
    void disconnectFromHost() override;
    void disconnected() override;
    QSslCipher sessionCipher() const override;
    QSsl::SslProtocol sessionProtocol() const override;
    void continueHandshake() override;

    static QList<QSslCipher> defaultCiphers();
    static QList<QSslError> verify(const QList<QSslCertificate> &certificateChain,
                                   const QString &hostName);
    static bool importPkcs12(QIODevice *device, QSslKey *key, QSslCertificate *cert,
                             QList<QSslCertificate> *caCertificates, const QByteArray &passPhrase);

private:
    enum class SchannelState {
        InitializeHandshake, // create and transmit context (client)/accept context (server)
        PerformHandshake, // get token back, process it
        VerifyHandshake, // Verify that things are OK
        Done, // Connection encrypted!
        Renegotiate // Renegotiating!
    } schannelState = SchannelState::InitializeHandshake;

    void reset();
    bool acquireCredentialsHandle();
    ULONG getContextRequirements();
    bool createContext(); // for clients
    bool acceptContext(); // for server
    bool performHandshake();
    bool verifyHandshake();
    bool renegotiate();

    bool sendToken(void *token, unsigned long tokenLength, bool emitError = true);
    QString targetName() const;

    bool checkSslErrors();
    void deallocateContext();
    void freeCredentialsHandle();
    void closeCertificateStores();
    void sendShutdown();

    void initializeCertificateStores();
    bool verifyCertContext(CERT_CONTEXT *certContext);

    bool rootCertOnDemandLoadingAllowed();

    bool hasUndecryptedData() override { return intermediateBuffer.size() > 0; }

    SecPkgContext_ConnectionInfo connectionInfo = {};
    SecPkgContext_StreamSizes streamSizes = {};

    CredHandle credentialHandle; // Initialized in ctor
    CtxtHandle contextHandle; // Initialized in ctor

    QByteArray intermediateBuffer; // data which is left-over or incomplete

    QHCertStorePointer localCertificateStore = nullptr;
    QHCertStorePointer peerCertificateStore = nullptr;
    QHCertStorePointer caCertificateStore = nullptr;

    const CERT_CONTEXT *localCertContext = nullptr;

    ULONG contextAttributes = 0;
    qint64 missingData = 0;

    bool renegotiating = false;
};

QT_END_NAMESPACE

#endif // QSSLSOCKET_SCHANNEL_P_H
