/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2011 Thiago Macieira <thiago@kde.org>
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

#ifndef QATOMIC_BOOTSTRAP_H
#define QATOMIC_BOOTSTRAP_H

#include <QtCore/qgenericatomic.h>

QT_BEGIN_NAMESPACE

#if 0
// silence syncqt warnings
QT_END_NAMESPACE
#pragma qt_sync_skip_header_check
#pragma qt_sync_stop_processing
#endif

template <typename T> struct QAtomicOps: QGenericAtomicOps<QAtomicOps<T> >
{
    typedef T Type;

    static bool ref(T &_q_value) noexcept
    {
        return ++_q_value != 0;
    }
    static bool deref(T &_q_value) noexcept
    {
        return --_q_value != 0;
    }

    static bool testAndSetRelaxed(T &_q_value, T expectedValue, T newValue, T *currentValue = nullptr) noexcept
    {
        if (currentValue)
            *currentValue = _q_value;
        if (_q_value == expectedValue) {
            _q_value = newValue;
            return true;
        }
        return false;
    }

    static T fetchAndStoreRelaxed(T &_q_value, T newValue) noexcept
    {
        T tmp = _q_value;
        _q_value = newValue;
        return tmp;
    }

    template <typename AdditiveType> static
    T fetchAndAddRelaxed(T &_q_value, AdditiveType valueToAdd) noexcept
    {
        T returnValue = _q_value;
        _q_value += valueToAdd;
        return returnValue;
    }
};

QT_END_NAMESPACE

#endif // QATOMIC_BOOTSTRAP_H
