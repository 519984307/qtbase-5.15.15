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

#ifndef QWINDOWSTHEME_H
#define QWINDOWSTHEME_H

#include <qpa/qplatformtheme.h>

#include <QtCore/qsharedpointer.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QWindow;

class QWindowsTheme : public QPlatformTheme
{
    Q_DISABLE_COPY_MOVE(QWindowsTheme)
public:
    QWindowsTheme();
    ~QWindowsTheme() override;

    static QWindowsTheme *instance() { return m_instance; }

    bool usePlatformNativeDialog(DialogType type) const override;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const override;
#if QT_CONFIG(systemtrayicon)
    QPlatformSystemTrayIcon *createPlatformSystemTrayIcon() const override;
#endif
    QVariant themeHint(ThemeHint) const override;
    const QPalette *palette(Palette type = SystemPalette) const override
        { return m_palettes[type]; }
    const QFont *font(Font type = SystemFont) const override
        { return m_fonts[type]; }

    QPixmap standardPixmap(StandardPixmap sp, const QSizeF &size) const override;

    QIcon fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions = {}) const override;

    void windowsThemeChanged(QWindow *window);
    void displayChanged() { refreshIconPixmapSizes(); }

    QList<QSize> availableFileIconSizes() const { return m_fileIconSizes; }

    QPlatformMenuItem *createPlatformMenuItem() const override;
    QPlatformMenu *createPlatformMenu() const override;
    QPlatformMenuBar *createPlatformMenuBar() const override;
    void showPlatformMenuBar() override;

    static bool useNativeMenus();
    static bool queryDarkMode();
    static bool queryHighContrast();

    void refreshFonts();
    void refresh();

    static const char *name;

private:
    void clearPalettes();
    void refreshPalettes();
    void clearFonts();
    void refreshIconPixmapSizes();

    static QWindowsTheme *m_instance;
    QPalette *m_palettes[NPalettes];
    QFont *m_fonts[NFonts];
    QList<QSize> m_fileIconSizes;
};

QT_END_NAMESPACE

#endif // QWINDOWSTHEME_H
