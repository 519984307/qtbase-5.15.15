/****************************************************************************
**
** Copyright (C) 2015-2016 Oleksandr Tymoshenko <gonzo@bluezbox.com>
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

#ifndef QBSDMOUSE_H
#define QBSDMOUSE_H

#include <QString>
#include <QScopedPointer>
#include <QSocketNotifier>

#include <qobject.h>

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class QBsdMouseHandler : public QObject
{
    Q_OBJECT
public:
    QBsdMouseHandler(const QString &key, const QString &specification);
    ~QBsdMouseHandler() override;

private:
    void readMouseData();

private:
    QScopedPointer<QSocketNotifier> m_notifier;
    int m_devFd = -1;
    int m_packetSize = 0;
    int m_x = 0;
    int m_y = 0;
    int m_xOffset = 0;
    int m_yOffset = 0;
    Qt::MouseButtons m_buttons = Qt::NoButton;
};

QT_END_NAMESPACE

#endif // QBSDMOUSE_H
