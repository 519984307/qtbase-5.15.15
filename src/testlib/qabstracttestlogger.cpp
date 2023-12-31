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

#include <QtTest/private/qabstracttestlogger_p.h>
#include <QtTest/qtestassert.h>
#include <qtestresult_p.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

#ifdef Q_OS_ANDROID
#include <sys/stat.h>
#endif

QT_BEGIN_NAMESPACE

QAbstractTestLogger::QAbstractTestLogger(const char *filename)
{
    if (!filename) {
        stream = stdout;
        return;
    }
#if defined(_MSC_VER)
    if (::fopen_s(&stream, filename, "wt")) {
#else
    stream = ::fopen(filename, "wt");
    if (!stream) {
#endif
        fprintf(stderr, "Unable to open file for logging: %s\n", filename);
        ::exit(1);
    }
#ifdef Q_OS_ANDROID
    else {
        // Make sure output is world-readable on Android
        ::chmod(filename, 0666);
    }
#endif
}

QAbstractTestLogger::~QAbstractTestLogger()
{
    QTEST_ASSERT(stream);
    if (stream != stdout) {
        fclose(stream);
    }
    stream = nullptr;
}

bool QAbstractTestLogger::isLoggingToStdout() const
{
    return stream == stdout;
}

void QAbstractTestLogger::filterUnprintable(char *str) const
{
    unsigned char *idx = reinterpret_cast<unsigned char *>(str);
    while (*idx) {
        if (((*idx < 0x20 && *idx != '\n' && *idx != '\t') || *idx == 0x7f))
            *idx = '?';
        ++idx;
    }
}

void QAbstractTestLogger::outputString(const char *msg)
{
    QTEST_ASSERT(stream);
    QTEST_ASSERT(msg);

    char *filtered = new char[strlen(msg) + 1];
    strcpy(filtered, msg);
    filterUnprintable(filtered);

    ::fputs(filtered, stream);
    ::fflush(stream);

    delete [] filtered;
}

void QAbstractTestLogger::startLogging()
{
}

void QAbstractTestLogger::stopLogging()
{
}

void QAbstractTestLogger::addMessage(QtMsgType type, const QMessageLogContext &context,
                                     const QString &message)
{
    QAbstractTestLogger::MessageTypes messageType = [=]() {
        switch (type) {
        case QtDebugMsg: return QAbstractTestLogger::QDebug;
        case QtInfoMsg: return QAbstractTestLogger::QInfo;
        case QtCriticalMsg: return QAbstractTestLogger::QSystem;
        case QtWarningMsg: return QAbstractTestLogger::QWarning;
        case QtFatalMsg: return QAbstractTestLogger::QFatal;
        }
        Q_UNREACHABLE();
        return QAbstractTestLogger::QFatal;
    }();

    QString formattedMessage = qFormatLogMessage(type, context, message);

    // Note that we explicitly ignore the file and line of the context here,
    // as that's what QTest::messageHandler used to do when calling the same
    // overload directly.
    addMessage(messageType, formattedMessage);
}

namespace QTest
{

extern void filter_unprintable(char *str);

/*!
    \fn int QTest::qt_asprintf(QTestCharBuffer *buf, const char *format, ...);
    \internal
 */
int qt_asprintf(QTestCharBuffer *str, const char *format, ...)
{
    static const int MAXSIZE = 1024*1024*2;

    Q_ASSERT(str);

    int size = str->size();

    va_list ap;
    int res = 0;

    for (;;) {
        va_start(ap, format);
        res = qvsnprintf(str->data(), size, format, ap);
        va_end(ap);
        str->data()[size - 1] = '\0';
        if (res >= 0 && res < size) {
            // We succeeded
            break;
        }
        // buffer wasn't big enough, try again.
        // Note, we're assuming that a result of -1 is always due to running out of space.
        size *= 2;
        if (size > MAXSIZE) {
            break;
        }
        if (!str->reset(size))
            break; // out of memory - take what we have
    }

    return res;
}

}

namespace QTestPrivate
{

void generateTestIdentifier(QTestCharBuffer *identifier, int parts)
{
    const char *testObject = parts & TestObject ? QTestResult::currentTestObjectName() : "";
    const char *testFunction = parts & TestFunction ? (QTestResult::currentTestFunction() ? QTestResult::currentTestFunction() : "UnknownTestFunc") : "";
    const char *objectFunctionFiller = parts & TestObject && parts & (TestFunction | TestDataTag) ? "::" : "";
    const char *testFuctionStart = parts & TestFunction ? "(" : "";
    const char *testFuctionEnd = parts & TestFunction ? ")" : "";

    const char *dataTag = (parts & TestDataTag) && QTestResult::currentDataTag() ? QTestResult::currentDataTag() : "";
    const char *globalDataTag = (parts & TestDataTag) && QTestResult::currentGlobalDataTag() ? QTestResult::currentGlobalDataTag() : "";
    const char *tagFiller = (dataTag[0] && globalDataTag[0]) ? ":" : "";

    QTest::qt_asprintf(identifier, "%s%s%s%s%s%s%s%s",
        testObject, objectFunctionFiller, testFunction, testFuctionStart,
        globalDataTag, tagFiller, dataTag, testFuctionEnd);
}

}

QT_END_NAMESPACE
