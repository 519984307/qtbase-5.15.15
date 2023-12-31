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

#ifndef QEGLCONVENIENCE_H
#define QEGLCONVENIENCE_H

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

#include <QtGui/QSurfaceFormat>
#include <QtCore/QVector>
#include <QtCore/QSizeF>
#include <QtEglSupport/private/qt_egl_p.h>

QT_BEGIN_NAMESPACE

QVector<EGLint> q_createConfigAttributesFromFormat(const QSurfaceFormat &format);
bool q_reduceConfigAttributes(QVector<EGLint> *configAttributes);
EGLConfig q_configFromGLFormat(EGLDisplay display, const QSurfaceFormat &format, bool highestPixelFormat = false, int surfaceType = EGL_WINDOW_BIT);
QSurfaceFormat q_glFormatFromConfig(EGLDisplay display, const EGLConfig config, const QSurfaceFormat &referenceFormat = QSurfaceFormat());
bool q_hasEglExtension(EGLDisplay display,const char* extensionName);
void q_printEglConfig(EGLDisplay display, EGLConfig config);

#ifdef Q_OS_UNIX
QSizeF q_physicalScreenSizeFromFb(int framebufferDevice, const QSize &screenSize = QSize());
QSize q_screenSizeFromFb(int framebufferDevice);
int q_screenDepthFromFb(int framebufferDevice);
qreal q_refreshRateFromFb(int framebufferDevice);
#endif

class QEglConfigChooser
{
public:
    QEglConfigChooser(EGLDisplay display);
    virtual ~QEglConfigChooser();

    EGLDisplay display() const { return m_display; }

    void setSurfaceType(EGLint surfaceType) { m_surfaceType = surfaceType; }
    EGLint surfaceType() const { return m_surfaceType; }

    void setSurfaceFormat(const QSurfaceFormat &format) { m_format = format; }
    QSurfaceFormat surfaceFormat() const { return m_format; }

    void setIgnoreColorChannels(bool ignore) { m_ignore = ignore; }
    bool ignoreColorChannels() const { return m_ignore; }

    EGLConfig chooseConfig();

protected:
    virtual bool filterConfig(EGLConfig config) const;

    QSurfaceFormat m_format;
    EGLDisplay m_display;
    EGLint m_surfaceType;
    bool m_ignore;

    int m_confAttrRed;
    int m_confAttrGreen;
    int m_confAttrBlue;
    int m_confAttrAlpha;
};


QT_END_NAMESPACE

#endif //QEGLCONVENIENCE_H
