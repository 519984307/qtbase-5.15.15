/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the utils of the Qt Toolkit.
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
#include <QtCore/QtCore>

#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    const QStringList args = app.arguments().mid(1);
    if (args.isEmpty()) {
        printf("Usage: ./plugintest libplugin.so...\nThis tool loads a plugin and displays whether QPluginLoader could load it or not.\nIf the plugin could not be loaded, it'll display the error string.\n");
        return 1;
    }

    foreach (QString plugin, args) {
        printf("%s: ", qPrintable(plugin));
        QPluginLoader loader(plugin);
        if (loader.load())
            printf("success!\n");
        else
            printf("failure: %s\n", qPrintable(loader.errorString()));
    }

    return 0;
}

