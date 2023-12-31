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

#include "qgraphicsshadereffect_p.h"

#include "qglshaderprogram.h"
#include "gl2paintengineex/qglcustomshaderstage_p.h"
#define QGL_HAVE_CUSTOM_SHADERS 1
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicsitem.h>
#include <private/qgraphicseffect_p.h>

QT_BEGIN_NAMESPACE

/*#
    \class QGraphicsShaderEffect
    \inmodule QtOpenGL
    \brief The QGraphicsShaderEffect class is the base class for creating
    custom GLSL shader effects in a QGraphicsScene.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api

    The specific effect is defined by a fragment of GLSL source code
    supplied to setPixelShaderFragment().  This source code must define a
    function with the signature
    \c{lowp vec4 customShader(lowp sampler2D imageTexture, highp vec2 textureCoords)}
    that returns the source pixel value
    to use in the paint engine's shader program.  The shader fragment
    is linked with the regular shader code used by the GL2 paint engine
    to construct a complete QGLShaderProgram.

    The following example shader converts the incoming pixmap to
    grayscale and then applies a colorize operation using the
    \c effectColor value:

    \snippet code/src_opengl_qgraphicsshadereffect.cpp 0

    To use this shader code, it is necessary to define a subclass
    of QGraphicsShaderEffect as follows:

    \snippet code/src_opengl_qgraphicsshadereffect.cpp 1

    The setUniforms() function is called when the effect is about
    to be used for drawing to give the subclass the opportunity to
    set effect-specific uniform variables.

    QGraphicsShaderEffect is only supported when the GL2 paint engine
    is in use.  When any other paint engine is in use (GL1, raster, etc),
    the drawItem() method will draw its item argument directly with
    no effect applied.

    \sa QGraphicsEffect
*/

static const char qglslDefaultImageFragmentShader[] = "\
    lowp vec4 customShader(lowp sampler2D imageTexture, highp vec2 textureCoords) { \
        return texture2D(imageTexture, textureCoords); \
    }\n";

#ifdef QGL_HAVE_CUSTOM_SHADERS

class QGLCustomShaderEffectStage : public QGLCustomShaderStage
{
public:
    QGLCustomShaderEffectStage
            (QGraphicsShaderEffect *e, const QByteArray& source)
        : QGLCustomShaderStage(),
          effect(e)
    {
        setSource(source);
    }

    void setUniforms(QGLShaderProgram *program) override;

    QGraphicsShaderEffect *effect;
};

void QGLCustomShaderEffectStage::setUniforms(QGLShaderProgram *program)
{
    effect->setUniforms(program);
}

#endif

class QGraphicsShaderEffectPrivate : public QGraphicsEffectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsShaderEffect)
public:
    QGraphicsShaderEffectPrivate()
        : pixelShaderFragment(qglslDefaultImageFragmentShader)
#ifdef QGL_HAVE_CUSTOM_SHADERS
          , customShaderStage(0)
#endif
    {
    }

    QByteArray pixelShaderFragment;
#ifdef QGL_HAVE_CUSTOM_SHADERS
    QGLCustomShaderEffectStage *customShaderStage;
#endif
};

/*#
    Constructs a shader effect and attaches it to \a parent.
*/
QGraphicsShaderEffect::QGraphicsShaderEffect(QObject *parent)
    : QGraphicsEffect(*new QGraphicsShaderEffectPrivate(), parent)
{
}

/*#
    Destroys this shader effect.
*/
QGraphicsShaderEffect::~QGraphicsShaderEffect()
{
#ifdef QGL_HAVE_CUSTOM_SHADERS
    Q_D(QGraphicsShaderEffect);
    delete d->customShaderStage;
#endif
}

/*#
    Returns the source code for the pixel shader fragment for
    this shader effect.  The default is a shader that copies
    its incoming pixmap directly to the output with no effect
    applied.

    \sa setPixelShaderFragment()
*/
QByteArray QGraphicsShaderEffect::pixelShaderFragment() const
{
    Q_D(const QGraphicsShaderEffect);
    return d->pixelShaderFragment;
}

/*#
    Sets the source code for the pixel shader fragment for
    this shader effect to \a code.

    The \a code must define a GLSL function with the signature
    \c{lowp vec4 customShader(lowp sampler2D imageTexture, highp vec2 textureCoords)}
    that returns the source pixel value to use in the paint engine's
    shader program.  The following is the default pixel shader fragment,
    which draws a pixmap with no effect applied:

    \snippet code/src_opengl_qgraphicsshadereffect.cpp 2

    \sa pixelShaderFragment(), setUniforms()
*/
void QGraphicsShaderEffect::setPixelShaderFragment(const QByteArray& code)
{
    Q_D(QGraphicsShaderEffect);
    if (d->pixelShaderFragment != code) {
        d->pixelShaderFragment = code;
#ifdef QGL_HAVE_CUSTOM_SHADERS
        delete d->customShaderStage;
        d->customShaderStage = 0;
#endif
    }
}

/*#
    \reimp
*/
void QGraphicsShaderEffect::draw(QPainter *painter)
{
    Q_D(QGraphicsShaderEffect);

#ifdef QGL_HAVE_CUSTOM_SHADERS
    // Set the custom shader on the paint engine.  The setOnPainter()
    // call may fail if the paint engine is not GL2.  In that case,
    // we fall through to drawing the pixmap normally.
    if (!d->customShaderStage) {
        d->customShaderStage = new QGLCustomShaderEffectStage
            (this, d->pixelShaderFragment);
    }
    bool usingShader = d->customShaderStage->setOnPainter(painter);

    QPoint offset;
    if (sourceIsPixmap()) {
        // No point in drawing in device coordinates (pixmap will be scaled anyways).
        const QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
        painter->drawPixmap(offset, pixmap);
    } else {
        // Draw pixmap in device coordinates to avoid pixmap scaling.
        const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);
        QTransform restoreTransform = painter->worldTransform();
        painter->setWorldTransform(QTransform());
        painter->drawPixmap(offset, pixmap);
        painter->setWorldTransform(restoreTransform);
    }

    // Remove the custom shader to return to normal painting operations.
    if (usingShader)
        d->customShaderStage->removeFromPainter(painter);
#else
    drawSource(painter);
#endif
}

/*#
    Sets the custom uniform variables on this shader effect to
    be dirty.  The setUniforms() function will be called the next
    time the shader program corresponding to this effect is used.

    This function is typically called by subclasses when an
    effect-specific parameter is changed by the application.

    \sa setUniforms()
*/
void QGraphicsShaderEffect::setUniformsDirty()
{
#ifdef QGL_HAVE_CUSTOM_SHADERS
    Q_D(QGraphicsShaderEffect);
    if (d->customShaderStage)
        d->customShaderStage->setUniformsDirty();
#endif
}

/*#
    Sets custom uniform variables on the current GL context when
    \a program is about to be used by the paint engine.

    This function should be overridden if the shader set with
    setPixelShaderFragment() has additional parameters beyond
    those that the paint engine normally sets itself.

    \sa setUniformsDirty()
*/
void QGraphicsShaderEffect::setUniforms(QGLShaderProgram *program)
{
    Q_UNUSED(program);
}

QT_END_NAMESPACE
