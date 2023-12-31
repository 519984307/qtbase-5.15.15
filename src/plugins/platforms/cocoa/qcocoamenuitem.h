/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author James Turner <james.turner@kdab.com>
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

#ifndef QCOCOAMENUITEM_H
#define QCOCOAMENUITEM_H

#include <qpa/qplatformmenu.h>
#include <QtGui/QImage>

//#define QT_COCOA_ENABLE_MENU_DEBUG

Q_FORWARD_DECLARE_OBJC_CLASS(NSMenuItem);
Q_FORWARD_DECLARE_OBJC_CLASS(NSMenu);
Q_FORWARD_DECLARE_OBJC_CLASS(NSObject);
Q_FORWARD_DECLARE_OBJC_CLASS(NSView);

QT_BEGIN_NAMESPACE

enum {
    AboutAppMenuItem = 0,
    PreferencesAppMenuItem,
    ServicesAppMenuItem,
    HideAppMenuItem,
    HideOthersAppMenuItem,
    ShowAllAppMenuItem,
    QuitAppMenuItem
};

QString qt_mac_applicationmenu_string(int type);

class QCocoaMenu;

class QCocoaMenuObject
{
public:
    void setMenuParent(QObject *o)
    {
        parent = o;
    }

    QObject *menuParent() const
    {
        return parent;
    }

private:
    QPointer<QObject> parent;
};

class QCocoaMenuItem : public QPlatformMenuItem, public QCocoaMenuObject
{
public:
    QCocoaMenuItem();
    ~QCocoaMenuItem();

    void setText(const QString &text) override;
    void setIcon(const QIcon &icon) override;
    void setMenu(QPlatformMenu *menu) override;
    void setVisible(bool isVisible) override;
    void setIsSeparator(bool isSeparator) override;
    void setFont(const QFont &font) override;
    void setRole(MenuRole role) override;
#ifndef QT_NO_SHORTCUT
    void setShortcut(const QKeySequence& shortcut) override;
#endif
    void setCheckable(bool checkable) override { Q_UNUSED(checkable) }
    void setChecked(bool isChecked) override;
    void setEnabled(bool isEnabled) override;
    void setIconSize(int size) override;

    void setNativeContents(WId item) override;

    inline QString text() const { return m_text; }
    inline NSMenuItem * nsItem() { return m_native; }
    NSMenuItem *sync();

    void syncMerged();
    void setParentEnabled(bool enabled);

    inline bool isMerged() const { return m_merged; }
    inline bool isEnabled() const { return m_enabled && m_parentEnabled; }
    inline bool isSeparator() const { return m_isSeparator; }
    inline bool isVisible() const { return m_isVisible; }

    QCocoaMenu *menu() const { return m_menu; }
    MenuRole effectiveRole() const;
    void resolveTargetAction();

private:
    QString mergeText();
    QKeySequence mergeAccel();

    NSMenuItem *m_native;
    NSView *m_itemView;
    QString m_text;
    QIcon m_icon;
    QPointer<QCocoaMenu> m_menu;
    MenuRole m_role;
    MenuRole m_detectedRole;
#ifndef QT_NO_SHORTCUT
    QKeySequence m_shortcut;
#endif
    int m_iconSize;
    bool m_textSynced:1;
    bool m_isVisible:1;
    bool m_enabled:1;
    bool m_parentEnabled:1;
    bool m_isSeparator:1;
    bool m_checked:1;
    bool m_merged:1;
};

QT_END_NAMESPACE

#endif
