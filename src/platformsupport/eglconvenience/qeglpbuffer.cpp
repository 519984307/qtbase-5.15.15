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

#include <QtGui/QOffscreenSurface>
#include "qeglpbuffer_p.h"
#include "qeglconvenience_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QEGLPbuffer
    \brief A pbuffer-based implementation of QPlatformOffscreenSurface for EGL.
    \since 5.2
    \internal
    \ingroup qpa

    To use this implementation in the platform plugin simply
    reimplement QPlatformIntegration::createPlatformOffscreenSurface()
    and return a new instance of this class.
*/

QEGLPbuffer::QEGLPbuffer(EGLDisplay display, const QSurfaceFormat &format, QOffscreenSurface *offscreenSurface,
                         QEGLPlatformContext::Flags flags)
    : QPlatformOffscreenSurface(offscreenSurface)
    , m_format(format)
    , m_display(display)
    , m_pbuffer(EGL_NO_SURFACE)
{
    m_hasSurfaceless = !flags.testFlag(QEGLPlatformContext::NoSurfaceless)
        && q_hasEglExtension(display, "EGL_KHR_surfaceless_context");

    // Disable surfaceless contexts on Mesa for now. As of 10.6.0 and Intel at least, some
    // operations (glReadPixels) are unable to work without a surface since they at some
    // point temporarily unbind the current FBO and then later blow up in some seemingly
    // safe operations, like setting the viewport, that apparently need access to the
    // read/draw surface in the Intel backend.
    const char *vendor = eglQueryString(display, EGL_VENDOR); // hard to check for GL_ strings here, so blacklist all Mesa
    if (vendor && strstr(vendor, "Mesa"))
        m_hasSurfaceless = false;

    if (m_hasSurfaceless)
        return;

    EGLConfig config = q_configFromGLFormat(m_display, m_format, false, EGL_PBUFFER_BIT);

    if (config) {
        const EGLint attributes[] = {
            EGL_WIDTH, offscreenSurface->size().width(),
            EGL_HEIGHT, offscreenSurface->size().height(),
            EGL_LARGEST_PBUFFER, EGL_FALSE,
            EGL_NONE
        };

        m_pbuffer = eglCreatePbufferSurface(m_display, config, attributes);

        if (m_pbuffer != EGL_NO_SURFACE)
            m_format = q_glFormatFromConfig(m_display, config);
    }
}

QEGLPbuffer::~QEGLPbuffer()
{
    if (m_pbuffer != EGL_NO_SURFACE)
        eglDestroySurface(m_display, m_pbuffer);
}

bool QEGLPbuffer::isValid() const
{
    return m_pbuffer != EGL_NO_SURFACE || m_hasSurfaceless;
}

QT_END_NAMESPACE
