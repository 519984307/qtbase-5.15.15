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

#include "qglobal.h"
#include "qdesktopwidget.h"
#include "qdesktopwidget_p.h"
#include "qscreen.h"
#include "qwidget_p.h"
#include "qwindow.h"

#include <private/qhighdpiscaling_p.h>
#include <qpa/qplatformscreen.h>

QT_BEGIN_NAMESPACE

QDesktopScreenWidget::QDesktopScreenWidget(QScreen *screen, const QRect &geometry)
    : QWidget(nullptr, Qt::Desktop), m_screen(screen)
{
    setVisible(false);
    if (QWindow *winHandle = windowHandle())
        winHandle->setScreen(screen);
    setScreenGeometry(geometry);
}

void QDesktopScreenWidget::setScreenGeometry(const QRect &geometry)
{
    m_geometry = geometry;
    setGeometry(geometry);
}

int QDesktopScreenWidget::screenNumber() const
{
    const QDesktopWidgetPrivate *desktopWidgetP
        = static_cast<const QDesktopWidgetPrivate *>(qt_widget_private(QApplication::desktop()));
    return desktopWidgetP->screens.indexOf(const_cast<QDesktopScreenWidget *>(this));
}

const QRect QDesktopWidget::screenGeometry(const QWidget *widget) const
{
    return QDesktopWidgetPrivate::screenGeometry(widget);
}

const QRect QDesktopWidgetPrivate::screenGeometry(const QWidget *widget)
{
    if (Q_UNLIKELY(!widget)) {
        qWarning("QDesktopWidget::screenGeometry(): Attempt "
                 "to get the screen geometry of a null widget");
        return QRect();
    }
    QRect rect = QWidgetPrivate::screenGeometry(widget);
    if (rect.isNull())
        return screenGeometry(screenNumber(widget));
    else return rect;
}

const QRect QDesktopWidget::availableGeometry(const QWidget *widget) const
{
    return QDesktopWidgetPrivate::availableGeometry(widget);
}

const QRect QDesktopWidgetPrivate::availableGeometry(const QWidget *widget)
{
    if (Q_UNLIKELY(!widget)) {
        qWarning("QDesktopWidget::availableGeometry(): Attempt "
                 "to get the available geometry of a null widget");
        return QRect();
    }
    QRect rect = QWidgetPrivate::screenGeometry(widget);
    if (rect.isNull())
        return availableGeometry(screenNumber(widget));
    else
        return rect;
}

QDesktopScreenWidget *QDesktopWidgetPrivate::widgetForScreen(QScreen *qScreen) const
{
    foreach (QDesktopScreenWidget *widget, screens) {
        if (widget->assignedScreen() == qScreen)
            return widget;
    }
    return nullptr;
}

void QDesktopWidgetPrivate::_q_updateScreens()
{
    Q_Q(QDesktopWidget);
    const QList<QScreen *> screenList = QGuiApplication::screens();
    const int targetLength = screenList.length();
    bool screenCountChanged = false;

    // Re-build our screens list. This is the easiest way to later compute which signals to emit.
    // Create new screen widgets as necessary. While iterating, keep the old list in place so
    // that widgetForScreen works.
    // Furthermore, we note which screens have changed, and compute the overall virtual geometry.
    QList<QDesktopScreenWidget *> newScreens;
    QList<int> changedScreens;
    QRegion virtualGeometry;

    for (int i = 0; i < targetLength; ++i) {
        QScreen *qScreen = screenList.at(i);
        const QRect screenGeometry = qScreen->geometry();
        QDesktopScreenWidget *screenWidget = widgetForScreen(qScreen);
        if (screenWidget) {
            // an old screen. update geometry and remember the index in the *new* list
            if (screenGeometry != screenWidget->screenGeometry()) {
                screenWidget->setScreenGeometry(screenGeometry);
                changedScreens.push_back(i);
            }
        } else {
            // a new screen, create a widget and connect the signals.
            screenWidget = new QDesktopScreenWidget(qScreen, screenGeometry);
            QObject::connect(qScreen, SIGNAL(geometryChanged(QRect)),
                             q, SLOT(_q_updateScreens()), Qt::QueuedConnection);
            QObject::connect(qScreen, SIGNAL(availableGeometryChanged(QRect)),
                             q, SLOT(_q_availableGeometryChanged()), Qt::QueuedConnection);
            QObject::connect(qScreen, SIGNAL(destroyed()),
                             q, SLOT(_q_updateScreens()), Qt::QueuedConnection);
            screenCountChanged = true;
        }
        // record all the screens and the overall geometry.
        newScreens.push_back(screenWidget);
        virtualGeometry += screenGeometry;
    }

    // Now we apply the accumulated updates.
    screens.swap(newScreens); // now [newScreens] is the old screen list
    Q_ASSERT(screens.size() == targetLength);
    q->setGeometry(virtualGeometry.boundingRect());

    // Delete the QDesktopScreenWidget that are not used any more.
    foreach (QDesktopScreenWidget *screen, newScreens) {
        if (!screens.contains(screen)) {
            delete screen;
            screenCountChanged = true;
        }
    }

    // Finally, emit the signals.
    if (screenCountChanged) {
        // Notice that we trigger screenCountChanged even if a screen was removed and another one added,
        // in which case the total number of screens did not change. This is the only way for applications
        // to notice that a screen was swapped out against another one.
#if QT_DEPRECATED_SINCE(5, 11)
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
        emit q->screenCountChanged(targetLength);
QT_WARNING_POP
#endif
    }
#if QT_DEPRECATED_SINCE(5, 11)
    foreach (int changedScreen, changedScreens)
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
        emit q->resized(changedScreen);
QT_WARNING_POP
#endif
}

void QDesktopWidgetPrivate::_q_availableGeometryChanged()
{
#if QT_DEPRECATED_SINCE(5, 11)
    Q_Q(QDesktopWidget);
    if (QScreen *screen = qobject_cast<QScreen *>(q->sender()))
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
        emit q->workAreaResized(QGuiApplication::screens().indexOf(screen));
QT_WARNING_POP
#endif
}

QDesktopWidget::QDesktopWidget()
    : QWidget(*new QDesktopWidgetPrivate, nullptr, Qt::Desktop)
{
    Q_D(QDesktopWidget);
    setObjectName(QLatin1String("desktop"));
    d->_q_updateScreens();
    connect(qApp, SIGNAL(screenAdded(QScreen*)), this, SLOT(_q_updateScreens()));
#if QT_DEPRECATED_SINCE(5, 11)
    connect(qApp, SIGNAL(primaryScreenChanged(QScreen*)), this, SIGNAL(primaryScreenChanged()));
#endif
}

QDesktopWidget::~QDesktopWidget()
{
}

#if QT_DEPRECATED_SINCE(5, 11)
bool QDesktopWidget::isVirtualDesktop() const
{
    return QDesktopWidgetPrivate::isVirtualDesktop();
}
#endif

bool QDesktopWidgetPrivate::isVirtualDesktop()
{
    return QGuiApplication::primaryScreen()->virtualSiblings().size() > 1;
}

QRect QDesktopWidgetPrivate::geometry()
{
    return QGuiApplication::primaryScreen()->virtualGeometry();
}

QSize QDesktopWidgetPrivate::size()
{
    return geometry().size();
}

int QDesktopWidgetPrivate::width()
{
    return geometry().width();
}

int QDesktopWidgetPrivate::height()
{
    return geometry().height();
}

#if QT_DEPRECATED_SINCE(5, 11)
int QDesktopWidget::primaryScreen() const
{
    return QDesktopWidgetPrivate::primaryScreen();
}
#endif

int QDesktopWidgetPrivate::primaryScreen()
{
    return 0;
}

int QDesktopWidgetPrivate::numScreens()
{
    return qMax(QGuiApplication::screens().size(), 1);
}

#if QT_DEPRECATED_SINCE(5, 11)
int QDesktopWidget::numScreens() const
{
    return QDesktopWidgetPrivate::numScreens();
}

QWidget *QDesktopWidget::screen(int screen)
{
    Q_D(QDesktopWidget);
    if (screen < 0 || screen >= d->screens.length())
        return d->screens.at(0);
    return d->screens.at(screen);
}

const QRect QDesktopWidget::availableGeometry(int screenNo) const
{
    return QDesktopWidgetPrivate::availableGeometry(screenNo);
}
#endif

const QRect QDesktopWidgetPrivate::availableGeometry(int screenNo)
{
    const QScreen *screen = QDesktopWidgetPrivate::screen(screenNo);
    return screen ? screen->availableGeometry() : QRect();
}

#if QT_DEPRECATED_SINCE(5, 11)
const QRect QDesktopWidget::screenGeometry(int screenNo) const
{
    return QDesktopWidgetPrivate::screenGeometry(screenNo);
}
#endif

const QRect QDesktopWidgetPrivate::screenGeometry(int screenNo)
{
    const QScreen *screen = QDesktopWidgetPrivate::screen(screenNo);
    return screen ? screen->geometry() : QRect();
}

int QDesktopWidget::screenNumber(const QWidget *w) const
{
    return QDesktopWidgetPrivate::screenNumber(w);
}

int QDesktopWidgetPrivate::screenNumber(const QWidget *w)
{
    if (!w)
        return primaryScreen();

    const QList<QScreen *> allScreens = QGuiApplication::screens();
    QList<QScreen *> screens = allScreens;
    if (screens.isEmpty()) // This should never happen
        return primaryScreen();

    // If there is more than one virtual desktop
    if (screens.count() != screens.constFirst()->virtualSiblings().count()) {
        // Find the root widget, get a QScreen from it and use the
        // virtual siblings for checking the window position.
        if (const QScreen *winScreen = qt_widget_private(const_cast<QWidget *>(w))->associatedScreen())
            screens = winScreen->virtualSiblings();
    }

    // Get the screen number from window position using screen geometry
    // and proper screens.
    QRect frame = w->frameGeometry();
    if (!w->isWindow())
        frame.moveTopLeft(w->mapToGlobal(QPoint(0, 0)));

    QScreen *widgetScreen = nullptr;
    int largestArea = 0;
    foreach (QScreen *screen, screens) {
        const QRect deviceIndependentScreenGeometry =
            QHighDpi::fromNativePixels(screen->handle()->geometry(), screen);
        const QRect intersected = deviceIndependentScreenGeometry.intersected(frame);
        int area = intersected.width() * intersected.height();
        if (largestArea < area) {
            widgetScreen = screen;
            largestArea = area;
        }
    }
    return allScreens.indexOf(widgetScreen);
}

#if QT_DEPRECATED_SINCE(5, 11)
int QDesktopWidget::screenNumber(const QPoint &p) const
{
    return QDesktopWidgetPrivate::screenNumber(p);
}
#endif

int QDesktopWidgetPrivate::screenNumber(const QPoint &p)
{
    QScreen *screen = QGuiApplication::screenAt(p);
    return screen ? QGuiApplication::screens().indexOf(screen) : primaryScreen();
}

QScreen *QDesktopWidgetPrivate::screen(int screenNo)
{
    QList<QScreen *> screens = QGuiApplication::screens();
    if (screenNo == -1)
        screenNo = 0;
    if (screenNo < 0 || screenNo >= screens.size())
        return nullptr;
    return screens.at(screenNo);
}

void QDesktopWidget::resizeEvent(QResizeEvent *)
{
}

QT_END_NAMESPACE

#include "moc_qdesktopwidget.cpp"
#include "moc_qdesktopwidget_p.cpp"
