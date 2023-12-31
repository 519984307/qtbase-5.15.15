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

#include "qpictureformatplugin.h"
#if !defined(QT_NO_PICTURE)
#include "qpicture.h"

QT_BEGIN_NAMESPACE

/*!
    \obsolete

    \class QPictureFormatPlugin
    \brief The QPictureFormatPlugin class provides an abstract base
    for custom picture format plugins.

    \ingroup plugins
    \inmodule QtGui

    The picture format plugin is a simple plugin interface that makes
    it easy to create custom picture formats that can be used
    transparently by applications.

    Writing an picture format plugin is achieved by subclassing this
    base class, reimplementing the pure virtual functions
    loadPicture(), savePicture(), and installIOHandler(), and
    exporting the class with the Q_PLUGIN_METADATA() macro.

    The json file containing the metadata should contain one entry
    with the list of picture formats supported by the plugin:

    \code
    { "Keys": [ "mypictureformat" ] }
    \endcode

    \sa {How to Create Qt Plugins}
*/

/*!
    \fn bool QPictureFormatPlugin::installIOHandler(const QString &format)

    Installs a QPictureIO picture I/O handler for the picture format \a
    format. Returns \c true on success.
*/


/*!
    Constructs an picture format plugin with the given \a parent.
    This is invoked automatically by the moc generated code that exports the plugin.
*/
QPictureFormatPlugin::QPictureFormatPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the picture format plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QPictureFormatPlugin::~QPictureFormatPlugin()
{
}


/*!
    Loads the picture stored in the file called \a fileName, with the
    given \a format, into *\a picture. Returns \c true on success;
    otherwise returns \c false.

    \sa savePicture()
*/
bool QPictureFormatPlugin::loadPicture(const QString &format, const QString &fileName, QPicture *picture)
{
    Q_UNUSED(format)
    Q_UNUSED(fileName)
    Q_UNUSED(picture)
    return false;
}

/*!
    Saves the given \a picture into the file called \a fileName,
    using the specified \a format. Returns \c true on success; otherwise
    returns \c false.

    \sa loadPicture()
*/
bool QPictureFormatPlugin::savePicture(const QString &format, const QString &fileName, const QPicture &picture)
{
    Q_UNUSED(format)
    Q_UNUSED(fileName)
    Q_UNUSED(picture)
    return false;
}

#endif // QT_NO_PICTURE

QT_END_NAMESPACE
