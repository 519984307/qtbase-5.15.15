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

#include "qscopedvaluerollback.h"

QT_BEGIN_NAMESPACE

/*!
    \class QScopedValueRollback
    \inmodule QtCore
    \brief The QScopedValueRollback class resets a variable to its previous value on destruction.
    \since 4.8
    \ingroup misc

    The QScopedValueRollback class can be used to revert state when an
    exception is thrown without needing to write try-catch blocks.

    It can also be used to manage variables that are temporarily set,
    such as reentrancy guards. By using this class, the variable will
    be reset whether the function is exited normally, exited early by
    a return statement, or exited by an exception.

    The template can only be instantiated with a type that supports assignment.

    \sa QScopedPointer, QScopeGuard
*/

/*!
    \fn template <typename T> QScopedValueRollback<T>::QScopedValueRollback(T &var)

    Stores the previous value of \a var internally, for revert on destruction.
*/

/*!
    \fn template <typename T> QScopedValueRollback<T>::QScopedValueRollback(T &var, T value)

    Assigns \a value to \ var and stores the previous value of \a var
    internally, for revert on destruction.

    \since 5.4
*/

/*!
    \fn template <typename T> QScopedValueRollback<T>::~QScopedValueRollback()

    Assigns the previous value to the managed variable.
    This is the value at construction time, or at the last call to commit()
*/

/*!
    \fn template <typename T> void QScopedValueRollback<T>::commit()

    Updates the previous value of the managed variable to its current value.
*/

QT_END_NAMESPACE
