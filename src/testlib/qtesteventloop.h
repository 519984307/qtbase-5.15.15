/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTEVENTLOOP_H
#define QTESTEVENTLOOP_H

#include <QtTest/qttestglobal.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qthread.h>

QT_BEGIN_NAMESPACE


class Q_TESTLIB_EXPORT QTestEventLoop : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    inline void enterLoopMSecs(int ms);
    inline void enterLoop(int secs) { enterLoopMSecs(secs * 1000); }

    inline void changeInterval(int secs)
    { killTimer(timerId); timerId = startTimer(secs * 1000); }

    inline bool timeout() const
    { return _timeout; }

    inline static QTestEventLoop &instance()
    {
        static QPointer<QTestEventLoop> testLoop;
        if (testLoop.isNull())
            testLoop = new QTestEventLoop(QCoreApplication::instance());
        return *static_cast<QTestEventLoop *>(testLoop);
    }

public Q_SLOTS:
    inline void exitLoop();

protected:
    inline void timerEvent(QTimerEvent *e) override;

private:
    Q_DECL_UNUSED_MEMBER bool inLoop; // ### Qt 6: remove
    bool _timeout = false;
    int timerId = -1;

    QEventLoop *loop = nullptr;
};

inline void QTestEventLoop::enterLoopMSecs(int ms)
{
    Q_ASSERT(!loop);

    QEventLoop l;

    _timeout = false;

    timerId = startTimer(ms);

    loop = &l;
    l.exec();
    loop = nullptr;
}

inline void QTestEventLoop::exitLoop()
{
    if (thread() != QThread::currentThread())
    {
        QMetaObject::invokeMethod(this, "exitLoop", Qt::QueuedConnection);
        return;
    }

    if (timerId != -1)
        killTimer(timerId);
    timerId = -1;

    if (loop)
        loop->exit();
}

inline void QTestEventLoop::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != timerId)
        return;
    _timeout = true;
    exitLoop();
}

QT_END_NAMESPACE

#endif
