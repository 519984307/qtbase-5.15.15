/****************************************************************************
**
** Copyright (C) 2014 Robin Burchell <robin.burchell@viroteck.net>
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qoscmessage_p.h"
#include "qtuio_p.h"

#include <QDebug>
#include <QtEndian>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcTuioMessage, "qt.qpa.tuio.message")

QOscMessage::QOscMessage() {}

// TUIO packets are transmitted using the OSC protocol, located at:
//   http://opensoundcontrol.org/specification
// Snippets of this specification have been pasted into the source as a means of
// easily communicating requirements.

QOscMessage::QOscMessage(const QByteArray &data)
    : m_isValid(false)
{
    qCDebug(lcTuioMessage) << data.toHex();
    quint32 parsedBytes = 0;

    // "An OSC message consists of an OSC Address Pattern"
    QByteArray addressPattern;
    if (!qt_readOscString(data, addressPattern, parsedBytes) || addressPattern.size() == 0)
        return;

    // "followed by an OSC Type Tag String"
    QByteArray typeTagString;
    if (!qt_readOscString(data, typeTagString, parsedBytes))
        return;

    // "Note: some older implementations of OSC may omit the OSC Type Tag string.
    // Until all such implementations are updated, OSC implementations should be
    // robust in the case of a missing OSC Type Tag String."
    //
    // (although, the editor notes one may question how exactly the hell one is
    // supposed to be robust when the behavior is unspecified.)
    if (typeTagString.size() == 0 || typeTagString.at(0) != ',')
        return;

    QList<QVariant> arguments;

    // "followed by zero or more OSC Arguments."
    for (int i = 1; i < typeTagString.size(); ++i) {
        char typeTag = typeTagString.at(i);
        if (typeTag == 's') { // osc-string
            QByteArray aString;
            if (!qt_readOscString(data, aString, parsedBytes))
                return;
            arguments.append(aString);
        } else if (typeTag == 'i') { // int32
            if (parsedBytes > (quint32)data.size() || data.size() - parsedBytes < sizeof(quint32))
                return;

            quint32 anInt = qFromBigEndian<quint32>(data.constData() + parsedBytes);
            parsedBytes += sizeof(quint32);

            // TODO: is int32 in OSC signed, or unsigned?
            arguments.append((int)anInt);
        } else if (typeTag == 'f') { // float32
            if (parsedBytes > (quint32)data.size() || data.size() - parsedBytes < sizeof(quint32))
                return;

            Q_STATIC_ASSERT(sizeof(float) == sizeof(quint32));
            union {
                quint32 u;
                float f;
            } value;
            value.u = qFromBigEndian<quint32>(data.constData() + parsedBytes);
            parsedBytes += sizeof(quint32);
            arguments.append(value.f);
        } else {
            qCWarning(lcTuioMessage) << "Reading argument of unknown type " << typeTag;
            return;
        }
    }

    m_isValid = true;
    m_addressPattern = addressPattern;
    m_arguments = arguments;

    qCDebug(lcTuioMessage) << "Message with address pattern: " << addressPattern << " arguments: " << arguments;
}

QT_END_NAMESPACE

