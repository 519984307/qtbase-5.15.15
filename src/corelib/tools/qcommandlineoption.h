/****************************************************************************
**
** Copyright (C) 2013 Laszlo Papp <lpapp@kde.org>
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

#ifndef QCOMMANDLINEOPTION_H
#define QCOMMANDLINEOPTION_H

#include <QtCore/qstringlist.h>
#include <QtCore/qshareddata.h>

QT_REQUIRE_CONFIG(commandlineparser);

QT_BEGIN_NAMESPACE

class QCommandLineOptionPrivate;

class Q_CORE_EXPORT QCommandLineOption
{
public:
    enum Flag {
        HiddenFromHelp = 0x1,
        ShortOptionStyle = 0x2
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    explicit QCommandLineOption(const QString &name);
    explicit QCommandLineOption(const QStringList &names);
    /*implicit*/ QCommandLineOption(const QString &name, const QString &description,
                                const QString &valueName = QString(),
                                const QString &defaultValue = QString());
    /*implicit*/ QCommandLineOption(const QStringList &names, const QString &description,
                                const QString &valueName = QString(),
                                const QString &defaultValue = QString());
    QCommandLineOption(const QCommandLineOption &other);

    ~QCommandLineOption();

    QCommandLineOption &operator=(const QCommandLineOption &other);
    QCommandLineOption &operator=(QCommandLineOption &&other) noexcept { swap(other); return *this; }

    void swap(QCommandLineOption &other) noexcept
    { qSwap(d, other.d); }

    QStringList names() const;

    void setValueName(const QString &name);
    QString valueName() const;

    void setDescription(const QString &description);
    QString description() const;

    void setDefaultValue(const QString &defaultValue);
    void setDefaultValues(const QStringList &defaultValues);
    QStringList defaultValues() const;

    Flags flags() const;
    void setFlags(Flags aflags);

#if QT_DEPRECATED_SINCE(5, 8)
    QT_DEPRECATED_X("Use setFlags() with HiddenFromHelp")
    void setHidden(bool hidden);
    QT_DEPRECATED_X("Use flags() and HiddenFromHelp")
    bool isHidden() const;
#endif


private:
    QSharedDataPointer<QCommandLineOptionPrivate> d;
};

Q_DECLARE_SHARED(QCommandLineOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCommandLineOption::Flags)


QT_END_NAMESPACE

#endif // QCOMMANDLINEOPTION_H
