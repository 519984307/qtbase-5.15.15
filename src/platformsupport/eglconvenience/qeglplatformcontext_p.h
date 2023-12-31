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

#ifndef QEGLPLATFORMCONTEXT_H
#define QEGLPLATFORMCONTEXT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qtextstream.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformopenglcontext.h>
#include <QtCore/QVariant>
#include <QtEglSupport/private/qt_egl_p.h>

QT_BEGIN_NAMESPACE

class QEGLPlatformContext : public QPlatformOpenGLContext
{
public:
    enum Flag {
        NoSurfaceless = 0x01
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    QEGLPlatformContext(const QSurfaceFormat &format, QPlatformOpenGLContext *share, EGLDisplay display,
                        EGLConfig *config = nullptr, const QVariant &nativeHandle = QVariant(),
                        Flags flags = { });
    ~QEGLPlatformContext();

    void initialize() override;
    bool makeCurrent(QPlatformSurface *surface) override;
    void doneCurrent() override;
    void swapBuffers(QPlatformSurface *surface) override;
    QFunctionPointer getProcAddress(const char *procName) override;

    QSurfaceFormat format() const override;
    bool isSharing() const override { return m_shareContext != EGL_NO_CONTEXT; }
    bool isValid() const override { return m_eglContext != EGL_NO_CONTEXT; }

    EGLContext eglContext() const;
    EGLDisplay eglDisplay() const;
    EGLConfig eglConfig() const;

protected:
    virtual EGLSurface eglSurfaceForPlatformSurface(QPlatformSurface *surface) = 0;
    virtual EGLSurface createTemporaryOffscreenSurface();
    virtual void destroyTemporaryOffscreenSurface(EGLSurface surface);
    virtual void runGLChecks();

private:
    void init(const QSurfaceFormat &format, QPlatformOpenGLContext *share);
    void adopt(const QVariant &nativeHandle, QPlatformOpenGLContext *share);
    void updateFormatFromGL();

    EGLContext m_eglContext;
    EGLContext m_shareContext;
    EGLDisplay m_eglDisplay;
    EGLConfig m_eglConfig;
    QSurfaceFormat m_format;
    EGLenum m_api;
    int m_swapInterval;
    bool m_swapIntervalEnvChecked;
    int m_swapIntervalFromEnv;
    Flags m_flags;
    bool m_ownsContext;
    QVector<EGLint> m_contextAttrs;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QEGLPlatformContext::Flags)

QT_END_NAMESPACE

#endif //QEGLPLATFORMCONTEXT_H
