/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#include "qstyleplugin.h"
#include "qstyle.h"

QT_BEGIN_NAMESPACE

/*!
    \class QStylePlugin
    \brief The QStylePlugin class provides an abstract base for custom QStyle plugins.

    \ingroup plugins
    \inmodule QtWidgets

    QStylePlugin is a simple plugin interface that makes it easy
    to create custom styles that can be loaded dynamically into
    applications using the QStyleFactory class.

    Writing a style plugin is achieved by subclassing this base class,
    reimplementing the pure virtual create() function, and
    exporting the class using the Q_PLUGIN_METADATA() macro.

    \snippet qstyleplugin/main.cpp 0

    The json metadata file \c mystyleplugin.json for the plugin needs
    to contain information about the names of the styles the plugins
    supports as follows:

    \quotefile qstyleplugin/mystyleplugin.json

    See \l {How to Create Qt Plugins} for details.

    \sa QStyleFactory, QStyle
*/

/*!
    \fn QStyle *QStylePlugin::create(const QString& key)

    Creates and returns a QStyle object for the given style \a key.
    If a plugin cannot create a style, it should return 0 instead.

    The style key is usually the class name of the required
    style. Note that the keys are case insensitive. For example:

    \snippet qstyleplugin/main.cpp 1
*/

/*!
    Constructs a style plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    moc generated code that exports the plugin, so there is no need for calling it
    explicitly.
*/
QStylePlugin::QStylePlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the style plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QStylePlugin::~QStylePlugin()
{
}

QT_END_NAMESPACE

#include "moc_qstyleplugin.cpp"
