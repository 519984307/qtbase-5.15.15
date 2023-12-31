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

#include <QtTest/private/qbenchmarktimemeasurers_p.h>
#include <QtTest/private/qbenchmark_p.h>
#include <QtTest/private/qbenchmarkmetric_p.h>
#include <QtTest/qbenchmark.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

// QBenchmarkTimeMeasurer implementation

void QBenchmarkTimeMeasurer::start()
{
    time.start();
}

qint64 QBenchmarkTimeMeasurer::checkpoint()
{
    return time.elapsed();
}

qint64 QBenchmarkTimeMeasurer::stop()
{
    return time.elapsed();
}

bool QBenchmarkTimeMeasurer::isMeasurementAccepted(qint64 measurement)
{
    return (measurement > 50);
}

int QBenchmarkTimeMeasurer::adjustIterationCount(int suggestion)
{
    return suggestion;
}

bool QBenchmarkTimeMeasurer::needsWarmupIteration()
{
    return true;
}

int QBenchmarkTimeMeasurer::adjustMedianCount(int)
{
    return 1;
}

QTest::QBenchmarkMetric QBenchmarkTimeMeasurer::metricType()
{
    return QTest::WalltimeMilliseconds;
}

#ifdef HAVE_TICK_COUNTER // defined in 3rdparty/cycle_p.h

void QBenchmarkTickMeasurer::start()
{
    startTicks = getticks();
}

qint64 QBenchmarkTickMeasurer::checkpoint()
{
    CycleCounterTicks now = getticks();
    return qRound64(elapsed(now, startTicks));
}

qint64 QBenchmarkTickMeasurer::stop()
{
    CycleCounterTicks now = getticks();
    return qRound64(elapsed(now, startTicks));
}

bool QBenchmarkTickMeasurer::isMeasurementAccepted(qint64)
{
    return true;
}

int QBenchmarkTickMeasurer::adjustIterationCount(int)
{
    return 1;
}

int QBenchmarkTickMeasurer::adjustMedianCount(int)
{
    return 1;
}

bool QBenchmarkTickMeasurer::needsWarmupIteration()
{
    return true;
}

QTest::QBenchmarkMetric QBenchmarkTickMeasurer::metricType()
{
    return QTest::CPUTicks;
}

#endif


QT_END_NAMESPACE
