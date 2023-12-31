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

#ifndef QWIDGETWINDOW_P_H
#define QWIDGETWINDOW_P_H

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
#include <QtGui/qwindow.h>

#include <QtCore/private/qobject_p.h>
#include <QtGui/private/qevent_p.h>
#include <QtWidgets/qwidget.h>

QT_BEGIN_NAMESPACE


class QCloseEvent;
class QMoveEvent;
class QWidgetWindowPrivate;

class QWidgetWindow : public QWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWidgetWindow)
public:
    QWidgetWindow(QWidget *widget);
    ~QWidgetWindow();

    QWidget *widget() const { return m_widget; }
#ifndef QT_NO_ACCESSIBILITY
    QAccessibleInterface *accessibleRoot() const override;
#endif

    QObject *focusObject() const override;
    void setNativeWindowVisibility(bool visible);
protected:
    bool event(QEvent *) override;

    void handleCloseEvent(QCloseEvent *);
    void handleEnterLeaveEvent(QEvent *);
    void handleFocusInEvent(QFocusEvent *);
    void handleKeyEvent(QKeyEvent *);
    void handleMouseEvent(QMouseEvent *);
    void handleNonClientAreaMouseEvent(QMouseEvent *);
    void handleTouchEvent(QTouchEvent *);
    void handleMoveEvent(QMoveEvent *);
    void handleResizeEvent(QResizeEvent *);
#if QT_CONFIG(wheelevent)
    void handleWheelEvent(QWheelEvent *);
#endif
#if QT_CONFIG(draganddrop)
    void handleDragEnterEvent(QDragEnterEvent *, QWidget *widget = nullptr);
    void handleDragMoveEvent(QDragMoveEvent *);
    void handleDragLeaveEvent(QDragLeaveEvent *);
    void handleDropEvent(QDropEvent *);
#endif
    void handleExposeEvent(QExposeEvent *);
    void handleWindowStateChangedEvent(QWindowStateChangeEvent *event);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
#if QT_CONFIG(tabletevent)
    void handleTabletEvent(QTabletEvent *);
#endif
#ifndef QT_NO_GESTURES
    void handleGestureEvent(QNativeGestureEvent *);
#endif
#ifndef QT_NO_CONTEXTMENU
    void handleContextMenuEvent(QContextMenuEvent *);
#endif

private slots:
    void updateObjectName();
    void handleScreenChange();

private:
    void repaintWindow();
    bool updateSize();
    void updateMargins();
    void updateNormalGeometry();

    enum FocusWidgets {
        FirstFocusWidget,
        LastFocusWidget
    };
    QWidget *getFocusWidget(FocusWidgets fw);

    QPointer<QWidget> m_widget;
    QPointer<QWidget> m_implicit_mouse_grabber;
#if QT_CONFIG(draganddrop)
    QPointer<QWidget> m_dragTarget;
#endif
};

QT_END_NAMESPACE

#endif // QWIDGETWINDOW_P_H
