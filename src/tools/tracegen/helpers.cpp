/****************************************************************************
**
** Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Rafael Roquetto <rafael.roquetto@kdab.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "helpers.h"
#include <qdebug.h>

QString includeGuard(const QString &filename)
{
    QString guard = filename.toUpper();

    for (int i = 0; i < guard.size(); ++i) {
        if (!guard.at(i).isLetterOrNumber())
            guard[i] = QLatin1Char('_');
    }

    return guard;
}

template <typename T>
static QString joinArguments(const QVector<Tracepoint::Argument> &args, T joinFunction)
{
    QString ret;
    bool first = true;

    for (const Tracepoint::Argument &arg : args) {
        if (!first)
            ret += QLatin1String(", ");

        ret += joinFunction(arg);

        first = false;
    }

    return ret;
}

QString formatFunctionSignature(const QVector<Tracepoint::Argument> &args)
{
    return joinArguments(args, [](const Tracepoint::Argument &arg) {
            return QStringLiteral("%1 %2").arg(arg.type).arg(arg.name);
    });
}

QString formatParameterList(const QVector<Tracepoint::Argument> &args, ParamType type)
{
    if (type == LTTNG) {
        QString ret;

        for (const Tracepoint::Argument &arg : args)
            ret += QLatin1String(", ") + arg.name;

        return ret;
    }

    return joinArguments(args, [](const Tracepoint::Argument &arg) { return arg.name; });
}
