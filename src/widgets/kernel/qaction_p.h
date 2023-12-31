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

#ifndef QACTION_P_H
#define QACTION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "QtWidgets/qaction.h"
#if QT_CONFIG(menu)
#include "QtWidgets/qmenu.h"
#endif
#if QT_CONFIG(graphicsview)
#include "private/qgraphicswidget_p.h"
#endif
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACTION


class QShortcutMap;

class Q_WIDGETS_EXPORT QActionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QAction)
public:
    QActionPrivate();
    ~QActionPrivate();

    static QActionPrivate *get(QAction *q)
    {
        return q->d_func();
    }

    bool showStatusText(QWidget *w, const QString &str);

    QPointer<QActionGroup> group;
    QString text;
    QString iconText;
    QIcon icon;
    QString tooltip;
    QString statustip;
    QString whatsthis;
#if QT_CONFIG(shortcut)
    QKeySequence shortcut;
    QList<QKeySequence> alternateShortcuts;
#endif
    QVariant userData;
#if QT_CONFIG(shortcut)
    int shortcutId = 0;
    QVector<int> alternateShortcutIds;
    Qt::ShortcutContext shortcutContext = Qt::WindowShortcut;
    uint autorepeat : 1;
#endif
    QFont font;
    QPointer<QMenu> menu;
    uint enabled : 1, forceDisabled : 1;
    uint visible : 1, forceInvisible : 1;
    uint checkable : 1;
    uint checked : 1;
    uint separator : 1;
    uint fontSet : 1;

    int iconVisibleInMenu : 2;  // Only has values -1, 0, and 1
    int shortcutVisibleInContextMenu : 2; // Only has values -1, 0, and 1

    QAction::MenuRole menuRole = QAction::TextHeuristicRole;
    QAction::Priority priority = QAction::NormalPriority;

    QWidgetList widgets;
#if QT_CONFIG(graphicsview)
    QList<QGraphicsWidget *> graphicsWidgets;
#endif
#if QT_CONFIG(shortcut)
    void redoGrab(QShortcutMap &map);
    void redoGrabAlternate(QShortcutMap &map);
    void setShortcutEnabled(bool enable, QShortcutMap &map);
#endif // QT_NO_SHORTCUT

    void sendDataChanged();
};

#endif // QT_NO_ACTION

QT_END_NAMESPACE

#endif // QACTION_P_H
