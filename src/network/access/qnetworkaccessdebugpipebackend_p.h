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

#ifndef QNETWORKACCESSDEBUGPIPEBACKEND_P_H
#define QNETWORKACCESSDEBUGPIPEBACKEND_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtNetwork/private/qtnetworkglobal_p.h>
#include "qnetworkaccessbackend_p.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qtcpsocket.h"

QT_BEGIN_NAMESPACE

#ifdef QT_BUILD_INTERNAL

class QNetworkAccessDebugPipeBackend: public QNetworkAccessBackend
{
    Q_OBJECT
public:
    QNetworkAccessDebugPipeBackend();
    virtual ~QNetworkAccessDebugPipeBackend();

    virtual void open() override;
    virtual void closeDownstreamChannel() override;

    virtual void downstreamReadyWrite() override;

protected:
    void pushFromSocketToDownstream();
    void pushFromUpstreamToSocket();
    void possiblyFinish();

private slots:
    void uploadReadyReadSlot();
    void socketReadyRead();
    void socketBytesWritten(qint64 bytes);
    void socketError();
    void socketDisconnected();
    void socketConnected();

private:
    QTcpSocket socket;
    bool bareProtocol;
    bool hasUploadFinished;
    bool hasDownloadFinished;
    bool hasEverythingFinished;

    qint64 bytesDownloaded;
    qint64 bytesUploaded;
};

class QNetworkAccessDebugPipeBackendFactory: public QNetworkAccessBackendFactory
{
public:
    virtual QStringList supportedSchemes() const override;
    virtual QNetworkAccessBackend *create(QNetworkAccessManager::Operation op,
                                          const QNetworkRequest &request) const override;
};

#endif  // QT_BUILD_INTERNAL

QT_END_NAMESPACE

#endif
