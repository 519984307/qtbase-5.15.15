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

#include <QtTest/private/qbenchmarkevent_p.h>
#include <QtTest/private/qbenchmark_p.h>
#include <QtTest/private/qbenchmarkmetric_p.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

QBenchmarkEvent::QBenchmarkEvent() = default;

QBenchmarkEvent::~QBenchmarkEvent() = default;

void QBenchmarkEvent::start()
{
    eventCounter = 0;
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

qint64 QBenchmarkEvent::checkpoint()
{
    return eventCounter;
}

qint64 QBenchmarkEvent::stop()
{
    QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);
    return eventCounter;
}

// It's very tempting to simply reject a measurement if 0 events
// where counted, however that is a possible situation and returning
// false here will create a infinite loop. Do not change this.
bool QBenchmarkEvent::isMeasurementAccepted(qint64 measurement)
{
    Q_UNUSED(measurement);
    return true;
}

int QBenchmarkEvent::adjustIterationCount(int suggestion)
{
    return suggestion;
}

int QBenchmarkEvent::adjustMedianCount(int suggestion)
{
    Q_UNUSED(suggestion);
    return 1;
}

QTest::QBenchmarkMetric QBenchmarkEvent::metricType()
{
    return QTest::Events;
}

// This could be done in a much better way, this is just the beginning.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool QBenchmarkEvent::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
#else
bool QBenchmarkEvent::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
#endif
{
    Q_UNUSED(eventType);
    Q_UNUSED(message);
    Q_UNUSED(result);

    eventCounter++;
    return false;
}

QT_END_NAMESPACE
