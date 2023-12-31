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


#ifndef QSSLKEY_OPENSSL_P_H
#define QSSLKEY_OPENSSL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qsslcertificate.cpp.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include <QtNetwork/private/qtnetworkglobal_p.h>
#include "qsslkey.h"
#include "qsslsocket_p.h" // includes wincrypt.h

#ifndef QT_NO_OPENSSL
#include <openssl/rsa.h>
#include <openssl/dsa.h>
#endif

QT_BEGIN_NAMESPACE

class QSslKeyPrivate
{
public:
    inline QSslKeyPrivate()
        : algorithm(QSsl::Opaque)
        , opaque(nullptr)
    {
        clear(false);
    }

    inline ~QSslKeyPrivate()
    { clear(); }

    void clear(bool deep = true);

#ifndef QT_NO_OPENSSL
    bool fromEVP_PKEY(EVP_PKEY *pkey);
#endif
    void decodeDer(const QByteArray &der, const QByteArray &passPhrase = {}, bool deepClear = true);
    void decodePem(const QByteArray &pem, const QByteArray &passPhrase, bool deepClear = true);
    QByteArray pemHeader() const;
    QByteArray pemFooter() const;
    QByteArray pemFromDer(const QByteArray &der, const QMap<QByteArray, QByteArray> &headers) const;
    QByteArray derFromPem(const QByteArray &pem, QMap<QByteArray, QByteArray> *headers) const;

    int length() const;
    QByteArray toPem(const QByteArray &passPhrase) const;
    Qt::HANDLE handle() const;

    bool isEncryptedPkcs8(const QByteArray &der) const;
#if !QT_CONFIG(openssl)
    QByteArray decryptPkcs8(const QByteArray &encrypted, const QByteArray &passPhrase);
    bool isPkcs8 = false;
#endif

    bool isNull;
    QSsl::KeyType type;
    QSsl::KeyAlgorithm algorithm;

    enum Cipher {
        DesCbc,
        DesEde3Cbc,
        Rc2Cbc,
        Aes128Cbc,
        Aes192Cbc,
        Aes256Cbc
    };

    Q_AUTOTEST_EXPORT static QByteArray decrypt(Cipher cipher, const QByteArray &data, const QByteArray &key, const QByteArray &iv);
    Q_AUTOTEST_EXPORT static QByteArray encrypt(Cipher cipher, const QByteArray &data, const QByteArray &key, const QByteArray &iv);

#ifndef QT_NO_OPENSSL
    union {
        EVP_PKEY *opaque;
        RSA *rsa;
        DSA *dsa;
        DH *dh;
#ifndef OPENSSL_NO_EC
        EC_KEY *ec;
#endif
    };
#else
    Qt::HANDLE opaque;
    QByteArray derData;
    int keyLength;
#endif

    QAtomicInt ref;

private:
    Q_DISABLE_COPY_MOVE(QSslKeyPrivate)
};

QT_END_NAMESPACE

#endif // QSSLKEY_OPENSSL_P_H
