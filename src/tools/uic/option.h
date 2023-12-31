/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef OPTION_H
#define OPTION_H

#include <qstring.h>
#include <qdir.h>

QT_BEGIN_NAMESPACE

struct Option
{
    unsigned int headerProtection : 1;
    unsigned int copyrightHeader : 1;
    unsigned int generateImplemetation : 1;
    unsigned int generateNamespace : 1;
    unsigned int autoConnection : 1;
    unsigned int dependencies : 1;
    unsigned int limitXPM_LineLength : 1;
    unsigned int implicitIncludes: 1;
    unsigned int idBased: 1;
    unsigned int fromImports: 1;

    QString inputFile;
    QString outputFile;
    QString qrcOutputFile;
    QString indent;
    QString prefix;
    QString postfix;
    QString translateFunction;
    QString includeFile;

    Option()
        : headerProtection(1),
          copyrightHeader(1),
          generateImplemetation(0),
          generateNamespace(1),
          autoConnection(1),
          dependencies(0),
          limitXPM_LineLength(0),
          implicitIncludes(1),
          idBased(0),
          fromImports(0),
          prefix(QLatin1String("Ui_"))
    { indent.fill(QLatin1Char(' '), 4); }

    QString messagePrefix() const
    {
        return inputFile.isEmpty() ?
               QString(QLatin1String("stdin")) :
               QDir::toNativeSeparators(inputFile);
    }
};

QT_END_NAMESPACE

#endif // OPTION_H
