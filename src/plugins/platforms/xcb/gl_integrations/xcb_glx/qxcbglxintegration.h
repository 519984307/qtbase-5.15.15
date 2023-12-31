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

#ifndef QXCBGLXINTEGRATION_H
#define QXCBGLXINTEGRATION_H

#include "qxcbglintegration.h"

QT_BEGIN_NAMESPACE

class QXcbNativeInterfaceHandler;

class QXcbGlxIntegration : public QXcbGlIntegration
{
public:
    QXcbGlxIntegration();
    ~QXcbGlxIntegration();

    bool initialize(QXcbConnection *connection) override;
    bool handleXcbEvent(xcb_generic_event_t *event, uint responseType) override;

    QXcbWindow *createWindow(QWindow *window) const override;
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const override;
    QPlatformOffscreenSurface *createPlatformOffscreenSurface(QOffscreenSurface *surface) const override;

    bool supportsThreadedOpenGL() const override;
    bool supportsSwitchableWidgetComposition() const override;

private:
    QXcbConnection *m_connection;
    uint32_t m_glx_first_event;

    QScopedPointer<QXcbNativeInterfaceHandler> m_native_interface_handler;
};

QT_END_NAMESPACE

#endif //QXCBGLXINTEGRATION_H
