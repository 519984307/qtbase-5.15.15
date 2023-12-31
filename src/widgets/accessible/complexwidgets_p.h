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

#ifndef COMPLEXWIDGETS_H
#define COMPLEXWIDGETS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include <QtCore/qpointer.h>
#include <QtWidgets/qaccessiblewidget.h>
#if QT_CONFIG(itemviews)
#include <QtWidgets/qabstractitemview.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

class QAbstractButton;
class QHeaderView;
class QTabBar;
class QComboBox;
class QTitleBar;
class QAbstractScrollArea;
class QScrollArea;

#if QT_CONFIG(scrollarea)
class QAccessibleAbstractScrollArea : public QAccessibleWidget
{
public:
    explicit QAccessibleAbstractScrollArea(QWidget *widget);

    enum AbstractScrollAreaElement {
        Self = 0,
        Viewport,
        HorizontalContainer,
        VerticalContainer,
        CornerWidget,
        Undefined
    };

    QAccessibleInterface *child(int index) const override;
    int childCount() const override;
    int indexOfChild(const QAccessibleInterface *child) const override;
    bool isValid() const override;
    QAccessibleInterface *childAt(int x, int y) const override;
    QAbstractScrollArea *abstractScrollArea() const;

private:
    QWidgetList accessibleChildren() const;
    AbstractScrollAreaElement elementType(QWidget *widget) const;
    bool isLeftToRight() const;
};

class QAccessibleScrollArea : public QAccessibleAbstractScrollArea
{
public:
    explicit QAccessibleScrollArea(QWidget *widget);
};
#endif // QT_CONFIG(scrollarea)

#if QT_CONFIG(tabbar)
class QAccessibleTabBar : public QAccessibleWidget
{
public:
    explicit QAccessibleTabBar(QWidget *w);
    ~QAccessibleTabBar();

    QAccessibleInterface *focusChild() const override;
    int childCount() const override;
    QString text(QAccessible::Text t) const override;

    QAccessibleInterface* child(int index) const override;
    int indexOfChild(const QAccessibleInterface *child) const override;

protected:
    QTabBar *tabBar() const;
    mutable QHash<int, QAccessible::Id> m_childInterfaces;
};
#endif // QT_CONFIG(tabbar)

#if QT_CONFIG(combobox)
class QAccessibleComboBox : public QAccessibleWidget
{
public:
    explicit QAccessibleComboBox(QWidget *w);

    int childCount() const override;
    QAccessibleInterface *childAt(int x, int y) const override;
    int indexOfChild(const QAccessibleInterface *child) const override;
    QAccessibleInterface* child(int index) const override;

    QString text(QAccessible::Text t) const override;

    // QAccessibleActionInterface
    QStringList actionNames() const override;
    QString localizedActionDescription(const QString &actionName) const override;
    void doAction(const QString &actionName) override;
    QStringList keyBindingsForAction(const QString &actionName) const override;

protected:
    QComboBox *comboBox() const;
};
#endif // QT_CONFIG(combobox)

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

#endif // COMPLEXWIDGETS_H
