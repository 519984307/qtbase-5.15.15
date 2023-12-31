/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QGLPIXELBUFFER_H
#define QGLPIXELBUFFER_H

#include <QtOpenGL/qgl.h>
#include <QtGui/qpaintdevice.h>

QT_BEGIN_NAMESPACE


class QGLPixelBufferPrivate;

class Q_OPENGL_EXPORT QGLPixelBuffer : public QPaintDevice
{
    Q_DECLARE_PRIVATE(QGLPixelBuffer)
public:
    QGLPixelBuffer(const QSize &size, const QGLFormat &format = QGLFormat::defaultFormat(),
                   QGLWidget *shareWidget = nullptr);
    QGLPixelBuffer(int width, int height, const QGLFormat &format = QGLFormat::defaultFormat(),
                   QGLWidget *shareWidget = nullptr);
    virtual ~QGLPixelBuffer();

    bool isValid() const;
    bool makeCurrent();
    bool doneCurrent();

    QGLContext *context() const;

    GLuint generateDynamicTexture() const;
    bool bindToDynamicTexture(GLuint texture);
    void releaseFromDynamicTexture();
    void updateDynamicTexture(GLuint texture_id) const;

    GLuint bindTexture(const QImage &image, GLenum target = GL_TEXTURE_2D);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target = GL_TEXTURE_2D);
    GLuint bindTexture(const QString &fileName);
    void deleteTexture(GLuint texture_id);

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);

    QSize size() const;
    Qt::HANDLE handle() const;
    QImage toImage() const;

    QPaintEngine *paintEngine() const override;
    QGLFormat format() const;

    static bool hasOpenGLPbuffers();

protected:
    int metric(PaintDeviceMetric metric) const override;
    int devType() const override { return QInternal::Pbuffer; }

private:
    Q_DISABLE_COPY(QGLPixelBuffer)
    QScopedPointer<QGLPixelBufferPrivate> d_ptr;
    friend class QGLDrawable;
    friend class QGLPaintDevice;
    friend class QGLPBufferGLPaintDevice;
    friend class QGLContextPrivate;
};

QT_END_NAMESPACE

#endif // QGLPIXELBUFFER_H
