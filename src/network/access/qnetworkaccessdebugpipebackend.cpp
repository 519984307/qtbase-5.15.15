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

#include "qnetworkaccessdebugpipebackend_p.h"
#include "QtCore/qdatastream.h"
#include <QCoreApplication>
#include <QStringList>
#include <QUrlQuery>
#include "private/qnoncontiguousbytedevice_p.h"

QT_BEGIN_NAMESPACE

#ifdef QT_BUILD_INTERNAL

enum {
    ReadBufferSize = 16384,
    WriteBufferSize = ReadBufferSize
};

QStringList QNetworkAccessDebugPipeBackendFactory::supportedSchemes() const
{
    return QStringList(QStringLiteral("debugpipe"));
}

QNetworkAccessBackend *
QNetworkAccessDebugPipeBackendFactory::create(QNetworkAccessManager::Operation op,
                                              const QNetworkRequest &request) const
{
    // is it an operation we know of?
    switch (op) {
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PutOperation:
        break;

    default:
        // no, we can't handle this operation
        return nullptr;
    }

    QUrl url = request.url();
    if (url.scheme() == QLatin1String("debugpipe"))
        return new QNetworkAccessDebugPipeBackend;
    return nullptr;
}

QNetworkAccessDebugPipeBackend::QNetworkAccessDebugPipeBackend()
    : bareProtocol(false), hasUploadFinished(false), hasDownloadFinished(false),
    hasEverythingFinished(false), bytesDownloaded(0), bytesUploaded(0)
{
}

QNetworkAccessDebugPipeBackend::~QNetworkAccessDebugPipeBackend()
{
    // this is signals disconnect, not network!
    socket.disconnect(this);    // we're not interested in the signals at this point
}

void QNetworkAccessDebugPipeBackend::open()
{
    socket.connectToHost(url().host(), url().port(12345));
    socket.setReadBufferSize(ReadBufferSize);

    // socket ready read -> we can push from socket to downstream
    connect(&socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
    connect(&socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), SLOT(socketError()));
    connect(&socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
    connect(&socket, SIGNAL(connected()), SLOT(socketConnected()));
    // socket bytes written -> we can push more from upstream to socket
    connect(&socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));

    bareProtocol = QUrlQuery(url()).queryItemValue(QLatin1String("bare")) == QLatin1String("1");

    if (operation() == QNetworkAccessManager::PutOperation) {
        createUploadByteDevice();
        QObject::connect(uploadByteDevice.data(), SIGNAL(readyRead()), this, SLOT(uploadReadyReadSlot()));
        QMetaObject::invokeMethod(this, "uploadReadyReadSlot", Qt::QueuedConnection);
    }
}

void QNetworkAccessDebugPipeBackend::socketReadyRead()
{
    pushFromSocketToDownstream();
}

void QNetworkAccessDebugPipeBackend::downstreamReadyWrite()
{
    pushFromSocketToDownstream();
}

void QNetworkAccessDebugPipeBackend::socketBytesWritten(qint64)
{
    pushFromUpstreamToSocket();
}

void QNetworkAccessDebugPipeBackend::uploadReadyReadSlot()
{
    pushFromUpstreamToSocket();
}

void QNetworkAccessDebugPipeBackend::pushFromSocketToDownstream()
{
    QByteArray buffer;

    if (socket.state() == QAbstractSocket::ConnectingState) {
        return;
    }

    forever {
        if (hasDownloadFinished)
            return;

        buffer.resize(ReadBufferSize);
        qint64 haveRead = socket.read(buffer.data(), ReadBufferSize);

        if (haveRead == -1) {
            hasDownloadFinished = true;
            // this ensures a good last downloadProgress is emitted
            setHeader(QNetworkRequest::ContentLengthHeader, QVariant());
            possiblyFinish();
            break;
        } else if (haveRead == 0) {
            break;
        } else {
            // have read something
            buffer.resize(haveRead);
            bytesDownloaded += haveRead;

            QByteDataBuffer list;
            list.append(buffer);
            buffer.clear(); // important because of implicit sharing!
            writeDownstreamData(list);
        }
    }
}

void QNetworkAccessDebugPipeBackend::pushFromUpstreamToSocket()
{
    // FIXME
    if (operation() == QNetworkAccessManager::PutOperation) {
        if (hasUploadFinished)
            return;

        forever {
            if (socket.bytesToWrite() >= WriteBufferSize)
                return;

            qint64 haveRead;
            const char *readPointer = uploadByteDevice->readPointer(WriteBufferSize, haveRead);
            if (haveRead == -1) {
                // EOF
                hasUploadFinished = true;
                emitReplyUploadProgress(bytesUploaded, bytesUploaded);
                possiblyFinish();
                break;
            } else if (haveRead == 0 || readPointer == nullptr) {
                // nothing to read right now, we will be called again later
                break;
            } else {
                qint64 haveWritten;
                haveWritten = socket.write(readPointer, haveRead);

                if (haveWritten < 0) {
                    // write error!
                    QString msg = QCoreApplication::translate("QNetworkAccessDebugPipeBackend", "Write error writing to %1: %2")
                                  .arg(url().toString(), socket.errorString());
                    error(QNetworkReply::ProtocolFailure, msg);
                    finished();
                    return;
                } else {
                    uploadByteDevice->advanceReadPointer(haveWritten);
                    bytesUploaded += haveWritten;
                    emitReplyUploadProgress(bytesUploaded, -1);
                }

                //QCoreApplication::processEvents();

            }
        }
    }
}

void QNetworkAccessDebugPipeBackend::possiblyFinish()
{
    if (hasEverythingFinished)
        return;
    hasEverythingFinished = true;

    if ((operation() == QNetworkAccessManager::GetOperation) && hasDownloadFinished) {
        socket.close();
        finished();
    } else if ((operation() == QNetworkAccessManager::PutOperation) && hasUploadFinished) {
        socket.close();
        finished();
    }


}

void QNetworkAccessDebugPipeBackend::closeDownstreamChannel()
{
    qWarning("QNetworkAccessDebugPipeBackend::closeDownstreamChannel() %d",operation());;
    //if (operation() == QNetworkAccessManager::GetOperation)
    //    socket.disconnectFromHost();
}


void QNetworkAccessDebugPipeBackend::socketError()
{
    qWarning("QNetworkAccessDebugPipeBackend::socketError() %d",socket.error());
    QNetworkReply::NetworkError code;
    switch (socket.error()) {
    case QAbstractSocket::RemoteHostClosedError:
        return;                 // socketDisconnected will be called

    case QAbstractSocket::NetworkError:
        code = QNetworkReply::UnknownNetworkError;
        break;

    default:
        code = QNetworkReply::ProtocolFailure;
        break;
    }

    error(code, QNetworkAccessDebugPipeBackend::tr("Socket error on %1: %2")
          .arg(url().toString(), socket.errorString()));
    finished();
    disconnect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

}

void QNetworkAccessDebugPipeBackend::socketDisconnected()
{
    pushFromSocketToDownstream();

    if (socket.bytesToWrite() == 0) {
        // normal close
    } else {
        // abnormal close
        QString msg = QNetworkAccessDebugPipeBackend::tr("Remote host closed the connection prematurely on %1")
                             .arg(url().toString());
        error(QNetworkReply::RemoteHostClosedError, msg);
        finished();
    }
}

void QNetworkAccessDebugPipeBackend::socketConnected()
{
}


#endif

QT_END_NAMESPACE
