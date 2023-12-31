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

#ifndef QWINRTTHEME_H
#define QWINRTTHEME_H

#include <qpa/qplatformtheme.h>
#include <qpa/qplatformintegration.h>
#include <QtCore/QLoggingCategory>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcQpaTheme)

class QWinRTThemePrivate;
class QWinRTTheme : public QPlatformTheme
{
public:
    QWinRTTheme();

    bool usePlatformNativeDialog(DialogType type) const override;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const override;

    const QPalette *palette(Palette type = SystemPalette) const override;
    const QFont *font(Font type = SystemFont) const override;

    static QVariant styleHint(QPlatformIntegration::StyleHint hint);
    QVariant themeHint(ThemeHint hint) const override;
private:
    QScopedPointer<QWinRTThemePrivate> d_ptr;
    Q_DECLARE_PRIVATE(QWinRTTheme)
};

QT_END_NAMESPACE

#endif // QWINRTTHEME_H
