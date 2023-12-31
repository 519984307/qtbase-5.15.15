/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#import <UIKit/UIKit.h>

#include <QtCore/qstandardpaths.h>
#include <QtGui/qwindow.h>
#include <QDebug>

#include <QtCore/private/qcore_mac_p.h>

#include "qiosfiledialog.h"
#include "qiosintegration.h"
#include "qiosoptionalplugininterface.h"
#include "qiosdocumentpickercontroller.h"

QIOSFileDialog::QIOSFileDialog()
    : m_viewController(nullptr)
{
}

QIOSFileDialog::~QIOSFileDialog()
{
    [m_viewController release];
}

void QIOSFileDialog::exec()
{
    m_eventLoop.exec(QEventLoop::DialogExec);
}

bool QIOSFileDialog::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(windowFlags);
    Q_UNUSED(windowModality);

    bool acceptOpen = options()->acceptMode() == QFileDialogOptions::AcceptOpen;
    QString directory = options()->initialDirectory().toLocalFile();

    if (acceptOpen) {
        if (directory.startsWith(QLatin1String("assets-library:")))
            return showImagePickerDialog(parent);
        else
            return showNativeDocumentPickerDialog(parent);
    }

    return false;
}

bool QIOSFileDialog::showImagePickerDialog(QWindow *parent)
{
    if (!m_viewController) {
        QFactoryLoader *plugins = QIOSIntegration::instance()->optionalPlugins();
        for (int i = 0; i < plugins->metaData().size(); ++i) {
            QIosOptionalPluginInterface *plugin = qobject_cast<QIosOptionalPluginInterface *>(plugins->instance(i));
            m_viewController = [plugin->createImagePickerController(this) retain];
            if (m_viewController)
                break;
        }
    }

    if (!m_viewController) {
        qWarning() << "QIOSFileDialog: Could not resolve Qt plugin that gives access to photos on iOS";
        return false;
    }

    UIWindow *window = parent ? reinterpret_cast<UIView *>(parent->winId()).window
        : qt_apple_sharedApplication().keyWindow;
    [window.rootViewController presentViewController:m_viewController animated:YES completion:nil];

    return true;
}

bool QIOSFileDialog::showNativeDocumentPickerDialog(QWindow *parent)
{
#ifndef Q_OS_TVOS
    if (options()->fileMode() == QFileDialogOptions::Directory ||
        options()->fileMode() == QFileDialogOptions::DirectoryOnly)
        return false;

    m_viewController = [[QIOSDocumentPickerController alloc] initWithQIOSFileDialog:this];

    UIWindow *window = parent ? reinterpret_cast<UIView *>(parent->winId()).window
        : qt_apple_sharedApplication().keyWindow;
    [window.rootViewController presentViewController:m_viewController animated:YES completion:nil];

    return true;
#else
    return false;
#endif
}

void QIOSFileDialog::hide()
{
    // QFileDialog will remember the last directory set, and open subsequent dialogs in the same
    // directory for convenience. This works for normal file dialogs, but not when using native
    // pickers. Those can only be used for picking specific types, without support for normal file
    // system navigation. To avoid showing a native picker by accident, we change directory back
    // before we return. More could have been done to preserve the "last directory" logic here, but
    // navigating the file system on iOS is not recommended in the first place, so we keep it simple.
    emit directoryEntered(QUrl::fromLocalFile(QDir::currentPath()));

    [m_viewController dismissViewControllerAnimated:YES completion:nil];
    [m_viewController release];
    m_viewController = nullptr;
    m_eventLoop.exit();
}

QList<QUrl> QIOSFileDialog::selectedFiles() const
{
    return m_selection;
}

void QIOSFileDialog::selectedFilesChanged(const QList<QUrl> &selection)
{
    m_selection = selection;
    emit filesSelected(m_selection);
    if (m_selection.count() == 1)
        emit fileSelected(m_selection[0]);
}
