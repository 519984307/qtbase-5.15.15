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

#ifndef QEVENTDISPATCHER_UNIX_P_H
#define QEVENTDISPATCHER_UNIX_P_H

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

#include "QtCore/qabstracteventdispatcher.h"
#include "QtCore/qlist.h"
#include "private/qabstracteventdispatcher_p.h"
#include "private/qcore_unix_p.h"
#include "QtCore/qvarlengtharray.h"
#include "private/qtimerinfo_unix_p.h"

QT_BEGIN_NAMESPACE

class QEventDispatcherUNIXPrivate;

struct Q_CORE_EXPORT QSocketNotifierSetUNIX final
{
    inline QSocketNotifierSetUNIX() noexcept;

    inline bool isEmpty() const noexcept;
    inline short events() const noexcept;

    QSocketNotifier *notifiers[3];
};

Q_DECLARE_TYPEINFO(QSocketNotifierSetUNIX, Q_PRIMITIVE_TYPE);

struct QThreadPipe
{
    QThreadPipe();
    ~QThreadPipe();

    bool init();
    pollfd prepare() const;

    void wakeUp();
    int check(const pollfd &pfd);

    // note for eventfd(7) support:
    // if fds[1] is -1, then eventfd(7) is in use and is stored in fds[0]
    int fds[2];
    QAtomicInt wakeUps;

#if defined(Q_OS_VXWORKS)
    static const int len_name = 20;
    char name[len_name];
#endif
};

class Q_CORE_EXPORT QEventDispatcherUNIX : public QAbstractEventDispatcher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QEventDispatcherUNIX)

public:
    explicit QEventDispatcherUNIX(QObject *parent = nullptr);
    ~QEventDispatcherUNIX();

    bool processEvents(QEventLoop::ProcessEventsFlags flags) override;
    bool hasPendingEvents() override;

    void registerSocketNotifier(QSocketNotifier *notifier) final;
    void unregisterSocketNotifier(QSocketNotifier *notifier) final;

    void registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object) final;
    bool unregisterTimer(int timerId) final;
    bool unregisterTimers(QObject *object) final;
    QList<TimerInfo> registeredTimers(QObject *object) const final;

    int remainingTime(int timerId) final;

    void wakeUp() override;
    void interrupt() final;
    void flush() override;

protected:
    QEventDispatcherUNIX(QEventDispatcherUNIXPrivate &dd, QObject *parent = nullptr);
};

class Q_CORE_EXPORT QEventDispatcherUNIXPrivate : public QAbstractEventDispatcherPrivate
{
    Q_DECLARE_PUBLIC(QEventDispatcherUNIX)

public:
    QEventDispatcherUNIXPrivate();
    ~QEventDispatcherUNIXPrivate();

    int activateTimers();

    void markPendingSocketNotifiers();
    int activateSocketNotifiers();
    void setSocketNotifierPending(QSocketNotifier *notifier);

    QThreadPipe threadPipe;
    QVector<pollfd> pollfds;

    QHash<int, QSocketNotifierSetUNIX> socketNotifiers;
    QVector<QSocketNotifier *> pendingNotifiers;

    QTimerInfoList timerList;
    QAtomicInt interrupt; // bool
};

inline QSocketNotifierSetUNIX::QSocketNotifierSetUNIX() noexcept
{
    notifiers[0] = nullptr;
    notifiers[1] = nullptr;
    notifiers[2] = nullptr;
}

inline bool QSocketNotifierSetUNIX::isEmpty() const noexcept
{
    return !notifiers[0] && !notifiers[1] && !notifiers[2];
}

inline short QSocketNotifierSetUNIX::events() const noexcept
{
    short result = 0;

    if (notifiers[0])
        result |= POLLIN;

    if (notifiers[1])
        result |= POLLOUT;

    if (notifiers[2])
        result |= POLLPRI;

    return result;
}

QT_END_NAMESPACE

#endif // QEVENTDISPATCHER_UNIX_P_H
