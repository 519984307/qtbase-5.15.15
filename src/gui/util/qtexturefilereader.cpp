/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qtexturefilereader_p.h"

#include "qpkmhandler_p.h"
#include "qktxhandler_p.h"
#include "qastchandler_p.h"

#include <QFileInfo>

QT_BEGIN_NAMESPACE

QTextureFileReader::QTextureFileReader(QIODevice *device, const QString &fileName)
    : m_device(device), m_fileName(fileName)
{
}

QTextureFileReader::~QTextureFileReader()
{
    delete m_handler;
}

QTextureFileData QTextureFileReader::read()
{
    if (!canRead())
        return QTextureFileData();
    return m_handler->read();
}

bool QTextureFileReader::canRead()
{
    if (!checked) {
        checked = true;
        if (!init())
            return false;

        QByteArray headerBlock = m_device->peek(64);
        QFileInfo fi(m_fileName);
        QByteArray suffix = fi.suffix().toLower().toLatin1();
        QByteArray logName = fi.fileName().toUtf8();

        // Currently the handlers are hardcoded; later maybe a list of plugins
        if (QPkmHandler::canRead(suffix, headerBlock)) {
            m_handler = new QPkmHandler(m_device, logName);
        } else if (QKtxHandler::canRead(suffix, headerBlock)) {
            m_handler = new QKtxHandler(m_device, logName);
        } else if (QAstcHandler::canRead(suffix, headerBlock)) {
            m_handler = new QAstcHandler(m_device, logName);
        }
        // else if OtherHandler::canRead() ...etc.
    }
    return (m_handler != nullptr);
}

QList<QByteArray> QTextureFileReader::supportedFileFormats()
{
    // Hardcoded for now
    return {QByteArrayLiteral("pkm"), QByteArrayLiteral("ktx"), QByteArrayLiteral("astc")};
}

bool QTextureFileReader::init()
{
    if (!m_device)
        return false;
    return m_device->isReadable();
}

QT_END_NAMESPACE
