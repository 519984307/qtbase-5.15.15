/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QANDROIDBEARERENGINE_H
#define QANDROIDBEARERENGINE_H

#include "../../qbearerengine_impl.h"

#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>
#include <QtCore/private/qjni_p.h>

#ifndef QT_NO_BEARERMANAGEMENT

QT_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate;
class QNetworkSessionPrivate;
class AndroidConnectivityManager;

class QAndroidBearerEngine : public QBearerEngineImpl
{
    Q_OBJECT

public:
    explicit QAndroidBearerEngine(QObject *parent = 0);
    ~QAndroidBearerEngine() override;

    QString getInterfaceFromId(const QString &id) override;
    bool hasIdentifier(const QString &id) override;
    void connectToId(const QString &id) override;
    void disconnectFromId(const QString &id) override;
    QNetworkSession::State sessionStateForId(const QString &id) override;
    QNetworkConfigurationManager::Capabilities capabilities() const override;
    QNetworkSessionPrivate *createSessionBackend() override;
    QNetworkConfigurationPrivatePointer defaultConfiguration() override;
    bool requiresPolling() const override;
    quint64 bytesWritten(const QString &id) override;
    quint64 bytesReceived(const QString &id) override;
    quint64 startTime(const QString &id) override;

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void requestUpdate();

private Q_SLOTS:
    void updateConfigurations();

private:
    QJNIObjectPrivate m_networkReceiver;
    AndroidConnectivityManager *m_connectivityManager;
    QMap<QString, QString> m_configurationInterface;
};


QT_END_NAMESPACE

#endif // QT_NO_BEARERMANAGEMENT

#endif // QANDROIDBEARERENGINE_H
