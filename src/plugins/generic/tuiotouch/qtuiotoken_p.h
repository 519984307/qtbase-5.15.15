/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTUIOOBJECT_P_H
#define QTUIOOBJECT_P_H

#include <Qt>
#include <qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \internal

    A fiducial object, or token, represented by 2Dobj in TUIO 1.x and tok in TUIO 2:
    a physical object whose position and rotation can be uniquely tracked
    on the touchscreen surface.
*/
class QTuioToken
{
public:
    QTuioToken(int id = -1)
        : m_id(id)
        , m_classId(-1)
        , m_x(0)
        , m_y(0)
        , m_vx(0)
        , m_vy(0)
        , m_acceleration(0)
        , m_angle(0)
        , m_angularVelocity(0)
        , m_angularAcceleration(0)
        , m_state(Qt::TouchPointPressed)
    {
    }

    int id() const { return m_id; }

    int classId() const { return m_classId; }
    void setClassId(int classId) { m_classId = classId; }

    void setX(float x)
    {
        if (state() == Qt::TouchPointStationary &&
            !qFuzzyCompare(m_x + 2.0, x + 2.0)) { // +2 because 1 is a valid value, and qFuzzyCompare can't cope with 0.0
            setState(Qt::TouchPointMoved);
        }
        m_x = x;
    }
    float x() const { return m_x; }

    void setY(float y)
    {
        if (state() == Qt::TouchPointStationary &&
            !qFuzzyCompare(m_y + 2.0, y + 2.0)) { // +2 because 1 is a valid value, and qFuzzyCompare can't cope with 0.0
            setState(Qt::TouchPointMoved);
        }
        m_y = y;
    }
    float y() const { return m_y; }

    void setVX(float vx) { m_vx = vx; }
    float vx() const { return m_vx; }

    void setVY(float vy) { m_vy = vy; }
    float vy() const { return m_vy; }

    void setAcceleration(float acceleration) { m_acceleration = acceleration; }
    float acceleration() const { return m_acceleration; }

    float angle() const { return m_angle; }
    void setAngle(float angle)
    {
        if (angle > M_PI)
            angle = angle - M_PI * 2.0; // zero is pointing upwards, and is the default; but we want to have negative angles when rotating left
        if (state() == Qt::TouchPointStationary &&
            !qFuzzyCompare(m_angle + 2.0, angle + 2.0)) { // +2 because 1 is a valid value, and qFuzzyCompare can't cope with 0.0
            setState(Qt::TouchPointMoved);
        }
        m_angle = angle;
    }

    float angularVelocity() const { return m_angularVelocity; }
    void setAngularVelocity(float angularVelocity) { m_angularVelocity = angularVelocity; }

    float angularAcceleration() const { return m_angularAcceleration; }
    void setAngularAcceleration(float angularAcceleration) { m_angularAcceleration = angularAcceleration; }

    void setState(const Qt::TouchPointState &state) { m_state = state; }
    Qt::TouchPointState state() const { return m_state; }

private:
    int m_id;       // sessionID, temporary object ID
    int m_classId;  // classID (e.g. marker ID)
    float m_x;
    float m_y;
    float m_vx;
    float m_vy;
    float m_acceleration;
    float m_angle;
    float m_angularVelocity;
    float m_angularAcceleration;
    Qt::TouchPointState m_state;
};
Q_DECLARE_TYPEINFO(QTuioToken, Q_MOVABLE_TYPE); // Q_PRIMITIVE_TYPE: not possible: m_id, m_classId == -1

QT_END_NAMESPACE

#endif // QTUIOOBJECT_P_H
