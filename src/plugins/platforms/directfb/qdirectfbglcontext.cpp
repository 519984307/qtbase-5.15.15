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

#include "qdirectfbglcontext.h"

#include <directfbgl.h>
#include <dlfcn.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

QDirectFbGLContext::QDirectFbGLContext(IDirectFBGL *glContext)
    : m_dfbGlContext(glContext)
{
    DFBResult result;
    DFBGLAttributes glAttribs;
    result = m_dfbGlContext->GetAttributes(glContext, &glAttribs);
    if (result == DFB_OK) {
        m_windowFormat.setDepthBufferSize(glAttribs.depth_size);
        m_windowFormat.setStencilBufferSize(glAttribs.stencil_size);

        m_windowFormat.setRedBufferSize(glAttribs.red_size);
        m_windowFormat.setGreenBufferSize(glAttribs.green_size);
        m_windowFormat.setBlueBufferSize(glAttribs.blue_size);
        m_windowFormat.setAlphaBufferSize(glAttribs.alpha_size);

        m_windowFormat.setAccumBufferSize(glAttribs.accum_red_size);
        m_windowFormat.setAlpha(glAttribs.accum_alpha_size);

        m_windowFormat.setDoubleBuffer(glAttribs.double_buffer);
        m_windowFormat.setStereo(glAttribs.stereo);
    }
}

void QDirectFbGLContext::makeCurrent()
{
    QPlatformOpenGLContext::makeCurrent();
    m_dfbGlContext->Lock(m_dfbGlContext);
}

void QDirectFbGLContext::doneCurrent()
{
    QPlatformOpenGLContext::doneCurrent();
    m_dfbGlContext->Unlock(m_dfbGlContext);
}

QFunctionPointer QDirectFbGLContext::getProcAddress(const char *procName)
{
    void *proc;
    DFBResult result = m_dfbGlContext->GetProcAddress(m_dfbGlContext, procName, &proc);
    if (result == DFB_OK)
        return (QFunctionPointer) proc;
    return dlsym(RTLD_DEFAULT, procName);
}

void QDirectFbGLContext::swapBuffers()
{
//    m_dfbGlContext->Unlock(m_dfbGlContext); //maybe not in doneCurrent()
    qDebug("Swap buffers");
}

QPlatformWindowFormat QDirectFbGLContext::platformWindowFormat() const
{
    return m_windowFormat;
}

QT_END_NAMESPACE
