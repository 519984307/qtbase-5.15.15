/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2015 ownCloud Inc
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

//#define QSSLSOCKET_DEBUG
//#define QT_DECRYPT_SSL_TRAFFIC

#include "qssl_p.h"
#include "qsslsocket.h"

#ifndef QT_NO_OPENSSL
#   include "qsslsocket_openssl_p.h"
#   include "qsslsocket_openssl_symbols_p.h"
#endif

#include "qsslcertificate_p.h"

#ifdef Q_OS_DARWIN
#   include <private/qcore_mac_p.h>
#endif

#include <QtCore/qdebug.h>

#ifdef Q_OS_MACOS
#   include <Security/Security.h>
#endif


QT_BEGIN_NAMESPACE

#ifdef Q_OS_MACOS
namespace {

bool hasTrustedSslServerPolicy(SecPolicyRef policy, CFDictionaryRef props) {
    QCFType<CFDictionaryRef> policyProps = SecPolicyCopyProperties(policy);
    // only accept certificates with policies for SSL server validation for now
    if (CFEqual(CFDictionaryGetValue(policyProps, kSecPolicyOid), kSecPolicyAppleSSL)) {
        CFBooleanRef policyClient;
        if (CFDictionaryGetValueIfPresent(policyProps, kSecPolicyClient, reinterpret_cast<const void**>(&policyClient)) &&
            CFEqual(policyClient, kCFBooleanTrue)) {
            return false; // no client certs
        }
        if (!CFDictionaryContainsKey(props, kSecTrustSettingsResult)) {
            // as per the docs, no trust settings result implies full trust
            return true;
        }
        CFNumberRef number = static_cast<CFNumberRef>(CFDictionaryGetValue(props, kSecTrustSettingsResult));
        SecTrustSettingsResult settingsResult;
        CFNumberGetValue(number, kCFNumberSInt32Type, &settingsResult);
        switch (settingsResult) {
        case kSecTrustSettingsResultTrustRoot:
        case kSecTrustSettingsResultTrustAsRoot:
            return true;
        default:
            return false;
        }
    }
    return false;
}

bool isCaCertificateTrusted(SecCertificateRef cfCert, int domain)
{
    QCFType<CFArrayRef> cfTrustSettings;
    OSStatus status = SecTrustSettingsCopyTrustSettings(cfCert, SecTrustSettingsDomain(domain), &cfTrustSettings);
    if (status == noErr) {
        CFIndex size = CFArrayGetCount(cfTrustSettings);
        // if empty, trust for everything (as per the Security Framework documentation)
        if (size == 0) {
            return true;
        } else {
            for (CFIndex i = 0; i < size; ++i) {
                CFDictionaryRef props = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(cfTrustSettings, i));
                if (CFDictionaryContainsKey(props, kSecTrustSettingsPolicy)) {
                    if (hasTrustedSslServerPolicy((SecPolicyRef)CFDictionaryGetValue(props, kSecTrustSettingsPolicy), props))
                        return true;
                }
            }
        }
    } else {
        qCWarning(lcSsl, "Error receiving trust for a CA certificate");
    }
    return false;
}

} // anon namespace
#endif // Q_OS_MACOS

QList<QSslCertificate> QSslSocketPrivate::systemCaCertificates()
{
    ensureInitialized();

    QList<QSslCertificate> systemCerts;
    // SecTrustSettingsCopyCertificates is not defined on iOS.
#ifdef Q_OS_MACOS
    // iterate through all enum members, order:
    // kSecTrustSettingsDomainUser, kSecTrustSettingsDomainAdmin, kSecTrustSettingsDomainSystem
    for (int dom = kSecTrustSettingsDomainUser; dom <= int(kSecTrustSettingsDomainSystem); dom++) {
        QCFType<CFArrayRef> cfCerts;
        OSStatus status = SecTrustSettingsCopyCertificates(SecTrustSettingsDomain(dom), &cfCerts);
        if (status == noErr) {
            const CFIndex size = CFArrayGetCount(cfCerts);
            for (CFIndex i = 0; i < size; ++i) {
                SecCertificateRef cfCert = (SecCertificateRef)CFArrayGetValueAtIndex(cfCerts, i);
                QCFType<CFDataRef> derData = SecCertificateCopyData(cfCert);
                if (isCaCertificateTrusted(cfCert, dom)) {
                    if (derData == NULL) {
                        qCWarning(lcSsl, "Error retrieving a CA certificate from the system store");
                    } else {
                        systemCerts << QSslCertificate(QByteArray::fromCFData(derData), QSsl::Der);
                    }
                }
            }
        }
    }
#endif
    return systemCerts;
}

QT_END_NAMESPACE
