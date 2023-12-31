/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2012 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Christoph Schleifenbaum <christoph.schleifenbaum@kdab.com>
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

#ifndef QCOCOASYSTEMTRAYICON_P_H
#define QCOCOASYSTEMTRAYICON_P_H

#include <QtCore/qglobal.h>
#include <QtGui/qtguiglobal.h>

#if QT_CONFIG(systemtrayicon)

#include <QtCore/qstring.h>
#include <QtGui/qpa/qplatformsystemtrayicon.h>

#include "qcocoamenu.h"

QT_FORWARD_DECLARE_CLASS(QCocoaSystemTrayIcon);

@interface QT_MANGLE_NAMESPACE(QStatusItemDelegate) : NSObject <NSUserNotificationCenterDelegate>
- (instancetype)initWithSysTray:(QCocoaSystemTrayIcon *)platformSystemTray;
@property (nonatomic, assign) QCocoaSystemTrayIcon *platformSystemTray;
@end

QT_NAMESPACE_ALIAS_OBJC_CLASS(QStatusItemDelegate);

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QCocoaSystemTrayIcon : public QPlatformSystemTrayIcon
{
public:
    QCocoaSystemTrayIcon() {}

    void init() override;
    void cleanup() override;
    void updateIcon(const QIcon &icon) override;
    void updateToolTip(const QString &toolTip) override;
    void updateMenu(QPlatformMenu *menu) override;
    QRect geometry() const override;
    void showMessage(const QString &title, const QString &msg,
                     const QIcon& icon, MessageIcon iconType, int msecs) override;

    bool isSystemTrayAvailable() const override;
    bool supportsMessages() const override;

    void statusItemClicked();

private:
    NSStatusItem *m_statusItem = nullptr;
    QStatusItemDelegate *m_delegate = nullptr;
    QCocoaMenu *m_menu = nullptr;
};

QT_END_NAMESPACE

#endif // QT_NO_SYSTEMTRAYICON

#endif // QCOCOASYSTEMTRAYICON_P_H
