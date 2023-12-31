/****************************************************************************
**
** Copyright (C) 2013 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QOPENGLTIMERQUERY_H
#define QOPENGLTIMERQUERY_H

#include <QtGui/qtguiglobal.h>

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)

#include <QtCore/QObject>
#include <QtGui/qopengl.h>

QT_BEGIN_NAMESPACE

class QOpenGLTimerQueryPrivate;

class Q_GUI_EXPORT QOpenGLTimerQuery : public QObject
{
    Q_OBJECT

public:
    explicit QOpenGLTimerQuery(QObject *parent = nullptr);
    ~QOpenGLTimerQuery();

    bool create();
    void destroy();
    bool isCreated() const;
    GLuint objectId() const;

    void begin();
    void end();
    GLuint64 waitForTimestamp() const;
    void recordTimestamp();
    bool isResultAvailable() const;
    GLuint64 waitForResult() const;

private:
    Q_DECLARE_PRIVATE(QOpenGLTimerQuery)
    Q_DISABLE_COPY(QOpenGLTimerQuery)
};


class QOpenGLTimeMonitorPrivate;

class Q_GUI_EXPORT QOpenGLTimeMonitor : public QObject
{
    Q_OBJECT

public:
    explicit QOpenGLTimeMonitor(QObject *parent = nullptr);
    ~QOpenGLTimeMonitor();

    void setSampleCount(int sampleCount);
    int sampleCount() const;

    bool create();
    void destroy();
    bool isCreated() const;
    QVector<GLuint> objectIds() const;

    int recordSample();

    bool isResultAvailable() const;

    QVector<GLuint64> waitForSamples() const;
    QVector<GLuint64> waitForIntervals() const;

    void reset();

private:
    Q_DECLARE_PRIVATE(QOpenGLTimeMonitor)
    Q_DISABLE_COPY(QOpenGLTimeMonitor)
};

QT_END_NAMESPACE

#endif // QT_NO_OPENGL

#endif // QOPENGLTIMERQUERY_H
