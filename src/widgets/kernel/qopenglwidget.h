/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QOPENGLWIDGET_H
#define QOPENGLWIDGET_H

#include <QtWidgets/qtwidgetsglobal.h>

#ifndef QT_NO_OPENGL

#include <QtWidgets/QWidget>
#include <QtGui/QSurfaceFormat>
#include <QtGui/qopengl.h>

QT_BEGIN_NAMESPACE

class QOpenGLWidgetPrivate;

class Q_WIDGETS_EXPORT QOpenGLWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QOpenGLWidget)

public:
    enum UpdateBehavior {
        NoPartialUpdate,
        PartialUpdate
    };

    explicit QOpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QOpenGLWidget();

    void setUpdateBehavior(UpdateBehavior updateBehavior);
    UpdateBehavior updateBehavior() const;

    void setFormat(const QSurfaceFormat &format);
    QSurfaceFormat format() const;

    GLenum textureFormat() const;
    void setTextureFormat(GLenum texFormat);

    bool isValid() const;

    void makeCurrent();
    void doneCurrent();

    QOpenGLContext *context() const;
    GLuint defaultFramebufferObject() const;

    QImage grabFramebuffer();

Q_SIGNALS:
    void aboutToCompose();
    void frameSwapped();
    void aboutToResize();
    void resized();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    bool event(QEvent *e) override;

    int metric(QPaintDevice::PaintDeviceMetric metric) const override;
    QPaintDevice *redirected(QPoint *p) const override;
    QPaintEngine *paintEngine() const override;

private:
    Q_DISABLE_COPY(QOpenGLWidget)
};

QT_END_NAMESPACE

#endif // QT_NO_OPENGL

#endif // QOPENGLWIDGET_H
