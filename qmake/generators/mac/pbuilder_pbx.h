/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef PBUILDER_PBX_H
#define PBUILDER_PBX_H

#include "unixmake.h"

QT_BEGIN_NAMESPACE

class ProjectBuilderMakefileGenerator : public UnixMakefileGenerator
{
    bool writingUnixMakefileGenerator;
    mutable QString pbx_dir;
    int pbuilderVersion() const;
    bool writeSubDirs(QTextStream &);
    bool writeMakeParts(QTextStream &);
    bool writeMakefile(QTextStream &) override;
    bool replaceLibrarySuffix(const QString &lib_file, const ProString &opt, QString &name,
                              QString &library);

    QString pbxbuild();
    QHash<QString, QString> keys;
    QString keyFor(const QString &file);
    QString findProgram(const ProString &prog);
    QString fixForOutput(const QString &file);
    ProStringList fixListForOutput(const char *where);
    ProStringList fixListForOutput(const ProStringList &list);
    int     reftypeForFile(const QString &where);
    QString projectSuffix() const;
    enum { SettingsAsList=0x01, SettingsNoQuote=0x02 };
    inline QString writeSettings(const QString &var, const char *val, int flags=0, int indent_level=0)
        { return writeSettings(var, ProString(val), flags, indent_level); }
    inline QString writeSettings(const QString &var, const QString &val, int flags=0, int indent_level=0)
        { return writeSettings(var, ProString(val), flags, indent_level); }
    inline QString writeSettings(const QString &var, const ProString &val, int flags=0, int indent_level=0)
        { return writeSettings(var, ProStringList(val), flags, indent_level); }
    QString writeSettings(const QString &var, const ProStringList &vals, int flags=0, int indent_level=0);

public:
    bool supportsMetaBuild() override { return false; }
    bool openOutput(QFile &, const QString &) const override;
protected:
    bool doPrecompiledHeaders() const override { return false; }
    bool doDepends() const override { return writingUnixMakefileGenerator && UnixMakefileGenerator::doDepends(); }
    bool inhibitMakeDirOutPath(const ProKey &path) const override;
};
QT_END_NAMESPACE

#endif // PBUILDER_PBX_H
