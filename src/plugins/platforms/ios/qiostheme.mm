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

#include "qiostheme.h"

#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/private/qcore_mac_p.h>

#include <QtGui/QFont>
#include <QtGui/private/qcoregraphics_p.h>

#include <QtFontDatabaseSupport/private/qcoretextfontdatabase_p.h>
#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include <UIKit/UIFont.h>
#include <UIKit/UIInterface.h>

#ifndef Q_OS_TVOS
#include "qiosmenu.h"
#include "qiosfiledialog.h"
#include "qiosmessagedialog.h"
#endif

QT_BEGIN_NAMESPACE

const char *QIOSTheme::name = "ios";

QIOSTheme::QIOSTheme()
{
    initializeSystemPalette();
}

QIOSTheme::~QIOSTheme()
{
    qDeleteAll(m_fonts);
}

QPalette QIOSTheme::s_systemPalette;

void QIOSTheme::initializeSystemPalette()
{
    Q_DECL_IMPORT QPalette qt_fusionPalette(void);
    s_systemPalette = qt_fusionPalette();

    if (@available(ios 13.0, *)) {
        s_systemPalette.setBrush(QPalette::Window, qt_mac_toQBrush(UIColor.systemGroupedBackgroundColor.CGColor));
        s_systemPalette.setBrush(QPalette::Active, QPalette::WindowText, qt_mac_toQBrush(UIColor.labelColor.CGColor));

        s_systemPalette.setBrush(QPalette::Base, qt_mac_toQBrush(UIColor.secondarySystemGroupedBackgroundColor.CGColor));
        s_systemPalette.setBrush(QPalette::Active, QPalette::Text, qt_mac_toQBrush(UIColor.labelColor.CGColor));

        s_systemPalette.setBrush(QPalette::Button, qt_mac_toQBrush(UIColor.secondarySystemBackgroundColor.CGColor));
        s_systemPalette.setBrush(QPalette::Active, QPalette::ButtonText, qt_mac_toQBrush(UIColor.labelColor.CGColor));

        s_systemPalette.setBrush(QPalette::Active, QPalette::BrightText, qt_mac_toQBrush(UIColor.lightTextColor.CGColor));
        s_systemPalette.setBrush(QPalette::Active, QPalette::PlaceholderText, qt_mac_toQBrush(UIColor.placeholderTextColor.CGColor));

        s_systemPalette.setBrush(QPalette::Active, QPalette::Link, qt_mac_toQBrush(UIColor.linkColor.CGColor));
        s_systemPalette.setBrush(QPalette::Active, QPalette::LinkVisited, qt_mac_toQBrush(UIColor.linkColor.CGColor));

        s_systemPalette.setBrush(QPalette::Highlight, QColor(11, 70, 150, 60));
        s_systemPalette.setBrush(QPalette::HighlightedText, qt_mac_toQBrush(UIColor.labelColor.CGColor));
    } else {
        s_systemPalette.setBrush(QPalette::Highlight, QColor(204, 221, 237));
        s_systemPalette.setBrush(QPalette::HighlightedText, Qt::black);
    }
}

const QPalette *QIOSTheme::palette(QPlatformTheme::Palette type) const
{
    if (type == QPlatformTheme::SystemPalette)
        return &s_systemPalette;
    return 0;
}

QPlatformMenuItem* QIOSTheme::createPlatformMenuItem() const
{
#ifdef Q_OS_TVOS
    return 0;
#else
    return new QIOSMenuItem();
#endif
}

QPlatformMenu* QIOSTheme::createPlatformMenu() const
{
#ifdef Q_OS_TVOS
    return 0;
#else
    return new QIOSMenu();
#endif
}

bool QIOSTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const
{
    switch (type) {
    case FileDialog:
    case MessageDialog:
        return !qt_apple_isApplicationExtension();
    default:
        return false;
    }
}

QPlatformDialogHelper *QIOSTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const
{
    switch (type) {
#ifndef Q_OS_TVOS
    case FileDialog:
        return new QIOSFileDialog();
        break;
    case MessageDialog:
        return new QIOSMessageDialog();
        break;
#endif
    default:
        return 0;
    }
}

QVariant QIOSTheme::themeHint(ThemeHint hint) const
{
    switch (hint) {
    case QPlatformTheme::StyleNames:
        return QStringList(QStringLiteral("fusion"));
    case KeyboardScheme:
        return QVariant(int(MacKeyboardScheme));
    default:
        return QPlatformTheme::themeHint(hint);
    }
}

const QFont *QIOSTheme::font(Font type) const
{
    if (m_fonts.isEmpty()) {
        QCoreTextFontDatabase *ctfd = static_cast<QCoreTextFontDatabase *>(QGuiApplicationPrivate::platformIntegration()->fontDatabase());
        m_fonts = ctfd->themeFonts();
    }

    return m_fonts.value(type, 0);
}

QT_END_NAMESPACE
