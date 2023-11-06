/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QIOSEVENTDISPATCHER_H
#define QIOSEVENTDISPATCHER_H

#include <QtCore/private/qeventdispatcher_cf_p.h>

QT_BEGIN_NAMESPACE

class QIOSEventDispatcher : public QEventDispatcherCoreFoundation
{
    Q_OBJECT

public:
    static QIOSEventDispatcher* create();
    bool processPostedEvents() override;

protected:
    explicit QIOSEventDispatcher(QObject *parent = 0);
};

class QIOSJumpingEventDispatcher : public QIOSEventDispatcher
{
    Q_OBJECT

public:
    QIOSJumpingEventDispatcher(QObject *parent = 0);
    bool processEvents(QEventLoop::ProcessEventsFlags flags) override;

    // Public since we can't friend Objective-C methods
    void handleRunLoopExit(CFRunLoopActivity activity);

    void interruptEventLoopExec();

private:
    uint m_processEventLevel;
    RunLoopObserver<QIOSJumpingEventDispatcher> m_runLoopExitObserver;
};

QT_END_NAMESPACE

#endif // QIOSEVENTDISPATCHER_H