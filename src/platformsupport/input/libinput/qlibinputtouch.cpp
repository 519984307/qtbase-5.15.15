/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins module of the Qt Toolkit.
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

#include "qlibinputtouch_p.h"
#include "qtouchoutputmapping_p.h"
#include <libinput.h>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/private/qhighdpiscaling_p.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(qLcLibInput)

QWindowSystemInterface::TouchPoint *QLibInputTouch::DeviceState::point(int32_t slot)
{
    const int id = qMax(0, slot);

    for (int i = 0; i < m_points.count(); ++i)
        if (m_points.at(i).id == id)
            return &m_points[i];

    return nullptr;
}

QLibInputTouch::DeviceState *QLibInputTouch::deviceState(libinput_event_touch *e)
{
    libinput_device *dev = libinput_event_get_device(libinput_event_touch_get_base_event(e));
    return &m_devState[dev];
}

QPointF QLibInputTouch::getPos(libinput_event_touch *e)
{
    DeviceState *state = deviceState(e);
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!state->m_screenName.isEmpty()) {
        if (!m_screen) {
            const QList<QScreen *> screens = QGuiApplication::screens();
            for (QScreen *s : screens) {
                if (s->name() == state->m_screenName) {
                    m_screen = s;
                    break;
                }
            }
        }
        if (m_screen)
            screen = m_screen;
    }
    const QRect geom = screen ? QHighDpi::toNativePixels(screen->geometry(), screen) : QRect();
    const double x = libinput_event_touch_get_x_transformed(e, geom.width());
    const double y = libinput_event_touch_get_y_transformed(e, geom.height());
    return geom.topLeft() + QPointF(x, y);
}

void QLibInputTouch::registerDevice(libinput_device *dev)
{
    struct udev_device *udev_device;
    udev_device = libinput_device_get_udev_device(dev);
    QString devNode = QString::fromUtf8(udev_device_get_devnode(udev_device));
    QString devName = QString::fromUtf8(libinput_device_get_name(dev));

    qCDebug(qLcLibInput, "libinput: registerDevice %s - %s",
            qPrintable(devNode), qPrintable(devName));

    QTouchOutputMapping mapping;
    if (mapping.load()) {
        m_devState[dev].m_screenName = mapping.screenNameForDeviceNode(devNode);
        if (!m_devState[dev].m_screenName.isEmpty())
            qCDebug(qLcLibInput, "libinput: Mapping device %s to screen %s",
                    qPrintable(devNode), qPrintable(m_devState[dev].m_screenName));
    }

    QTouchDevice *&td = m_devState[dev].m_touchDevice;
    td = new QTouchDevice;
    td->setName(devName);
    td->setType(QTouchDevice::TouchScreen);
    td->setCapabilities(QTouchDevice::Position | QTouchDevice::Area);
    QWindowSystemInterface::registerTouchDevice(td);
}

void QLibInputTouch::unregisterDevice(libinput_device *dev)
{
    Q_UNUSED(dev);
    // There is no way to remove a QTouchDevice.
}

void QLibInputTouch::processTouchDown(libinput_event_touch *e)
{
    int slot = libinput_event_touch_get_slot(e);
    DeviceState *state = deviceState(e);
    QWindowSystemInterface::TouchPoint *tp = state->point(slot);
    if (tp) {
        qWarning("Incorrect touch state");
    } else {
        QWindowSystemInterface::TouchPoint newTp;
        newTp.id = qMax(0, slot);
        newTp.state = Qt::TouchPointPressed;
        newTp.area = QRect(0, 0, 8, 8);
        newTp.area.moveCenter(getPos(e));
        state->m_points.append(newTp);
    }
}

void QLibInputTouch::processTouchMotion(libinput_event_touch *e)
{
    int slot = libinput_event_touch_get_slot(e);
    DeviceState *state = deviceState(e);
    QWindowSystemInterface::TouchPoint *tp = state->point(slot);
    if (tp) {
        Qt::TouchPointState tmpState = Qt::TouchPointMoved;
        const QPointF p = getPos(e);
        if (tp->area.center() == p)
            tmpState = Qt::TouchPointStationary;
        else
            tp->area.moveCenter(p);
        // 'down' may be followed by 'motion' within the same "frame".
        // Handle this by compressing and keeping the Pressed state until the 'frame'.
        if (tp->state != Qt::TouchPointPressed && tp->state != Qt::TouchPointReleased)
            tp->state = tmpState;
    } else {
        qWarning("Inconsistent touch state (got 'motion' without 'down')");
    }
}

void QLibInputTouch::processTouchUp(libinput_event_touch *e)
{
    int slot = libinput_event_touch_get_slot(e);
    DeviceState *state = deviceState(e);
    QWindowSystemInterface::TouchPoint *tp = state->point(slot);
    if (tp) {
        tp->state = Qt::TouchPointReleased;
        // There may not be a Frame event after the last Up. Work this around.
        Qt::TouchPointStates s;
        for (int i = 0; i < state->m_points.count(); ++i)
            s |= state->m_points.at(i).state;
        if (s == Qt::TouchPointReleased)
            processTouchFrame(e);
    } else {
        qWarning("Inconsistent touch state (got 'up' without 'down')");
    }
}

void QLibInputTouch::processTouchCancel(libinput_event_touch *e)
{
    DeviceState *state = deviceState(e);
    if (state->m_touchDevice)
        QWindowSystemInterface::handleTouchCancelEvent(nullptr, state->m_touchDevice, QGuiApplication::keyboardModifiers());
    else
        qWarning("TouchCancel without registered device");
}

void QLibInputTouch::processTouchFrame(libinput_event_touch *e)
{
    DeviceState *state = deviceState(e);
    if (!state->m_touchDevice) {
        qWarning("TouchFrame without registered device");
        return;
    }
    if (state->m_points.isEmpty())
        return;

    QWindowSystemInterface::handleTouchEvent(nullptr, state->m_touchDevice, state->m_points,
                                             QGuiApplication::keyboardModifiers());

    for (int i = 0; i < state->m_points.count(); ++i) {
        QWindowSystemInterface::TouchPoint &tp(state->m_points[i]);
        if (tp.state == Qt::TouchPointReleased)
            state->m_points.removeAt(i--);
        else if (tp.state == Qt::TouchPointPressed)
            tp.state = Qt::TouchPointStationary;
    }
}

QT_END_NAMESPACE
