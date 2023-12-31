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

#ifndef QPUSHBUTTON_P_H
#define QPUSHBUTTON_P_H

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "private/qabstractbutton_p.h"

QT_REQUIRE_CONFIG(pushbutton);

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

QT_BEGIN_NAMESPACE

class QDialog;
class QPushButton;

class QPushButtonPrivate : public QAbstractButtonPrivate
{
    Q_DECLARE_PUBLIC(QPushButton)
public:
    enum AutoDefaultValue { Off = 0, On = 1, Auto = 2 };

    QPushButtonPrivate()
        : QAbstractButtonPrivate(QSizePolicy::PushButton), autoDefault(Auto),
          defaultButton(false), flat(false), menuOpen(false), hovering(false),
          commandLink(false), lastAutoDefault(false)
    {}

    void init();
    static QPushButtonPrivate* get(QPushButton *b) { return b->d_func(); }
#if QT_CONFIG(menu)
    QPoint adjustedMenuPosition();
#endif
    void resetLayoutItemMargins();
    void _q_popupPressed();
#if QT_CONFIG(dialog)
    QDialog *dialogParent() const;
#else
    QDialog *dialogParent() const { return 0; };
#endif

    QPointer<QMenu> menu;
    uint autoDefault : 2;
    uint defaultButton : 1;
    uint flat : 1;
    uint menuOpen : 1;
    uint hovering : 1;
    uint commandLink : 1;
    mutable uint lastAutoDefault : 1;
};

QT_END_NAMESPACE

#endif // QPUSHBUTTON_P_H
