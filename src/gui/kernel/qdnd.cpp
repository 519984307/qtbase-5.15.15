/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qdnd_p.h"

#include "qguiapplication.h"
#include <ctype.h>
#include <qpa/qplatformdrag.h>
#include <qpa/qplatformintegration.h>
#include <private/qguiapplication_p.h>

QT_BEGIN_NAMESPACE

// the universe's only drag manager
QDragManager *QDragManager::m_instance = nullptr;

QDragManager::QDragManager()
    : QObject(qApp), m_currentDropTarget(nullptr),
      m_platformDrag(QGuiApplicationPrivate::platformIntegration()->drag()),
      m_object(nullptr)
{
    Q_ASSERT(!m_instance);
}

QDragManager::~QDragManager()
{
    m_instance = nullptr;
}

QDragManager *QDragManager::self()
{
    if (!m_instance && !QGuiApplication::closingDown())
        m_instance = new QDragManager;
    return m_instance;
}

QObject *QDragManager::source() const
{
    if (m_object)
        return m_object->source();
    return nullptr;
}

void QDragManager::setCurrentTarget(QObject *target, bool dropped)
{
    if (m_currentDropTarget == target)
        return;

    m_currentDropTarget = target;
    if (!dropped && m_object) {
        m_object->d_func()->target = target;
        emit m_object->targetChanged(target);
    }
}

QObject *QDragManager::currentTarget() const
{
    return m_currentDropTarget;
}

Qt::DropAction QDragManager::drag(QDrag *o)
{
    if (!o || m_object == o)
         return Qt::IgnoreAction;

    if (!m_platformDrag || !o->source()) {
        o->deleteLater();
        return Qt::IgnoreAction;
    }

    if (m_object) {
        qWarning("QDragManager::drag in possibly invalid state");
        return Qt::IgnoreAction;
    }

    m_object = o;

    m_object->d_func()->target = nullptr;

    QGuiApplicationPrivate::instance()->notifyDragStarted(m_object.data());
    const Qt::DropAction result = m_platformDrag->drag(m_object);
    if (!m_object.isNull() && !m_platformDrag->ownsDragObject())
        m_object->deleteLater();

    m_object.clear();
    return result;
}

QT_END_NAMESPACE

#include "moc_qdnd_p.cpp"
