/****************************************************************************
**
** Copyright (C) 2014 BogDan Vatra <bogdan@kde.org>
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

#ifndef QANDROIDPLATFORMSCREEN_H
#define QANDROIDPLATFORMSCREEN_H

#include <qpa/qplatformscreen.h>
#include <QList>
#include <QPainter>
#include <QTimer>
#include <QWaitCondition>
#include <QtCore/private/qjni_p.h>

#include "androidsurfaceclient.h"

#include <android/native_window.h>

QT_BEGIN_NAMESPACE

class QAndroidPlatformWindow;

class QAndroidPlatformScreen: public QObject, public QPlatformScreen, public AndroidSurfaceClient
{
    Q_OBJECT
public:
    QAndroidPlatformScreen();
    ~QAndroidPlatformScreen();

    QRect geometry() const override { return QRect(QPoint(), m_size); }
    QRect availableGeometry() const override { return m_availableGeometry; }
    int depth() const override { return m_depth; }
    QImage::Format format() const override { return m_format; }
    QSizeF physicalSize() const override { return m_physicalSize; }

    QString name() const override { return m_name; }
    QVector<Mode> modes() const override { return m_modes; }
    int currentMode() const override { return m_currentMode; }
    int preferredMode() const override { return m_currentMode; }
    qreal refreshRate() const override { return m_refreshRate; }

    inline QWindow *topWindow() const;
    QWindow *topLevelAt(const QPoint & p) const override;

    // compositor api
    void addWindow(QAndroidPlatformWindow *window);
    void removeWindow(QAndroidPlatformWindow *window);
    void raise(QAndroidPlatformWindow *window);
    void lower(QAndroidPlatformWindow *window);

    void scheduleUpdate();
    void topWindowChanged(QWindow *w);
    int rasterSurfaces();

public slots:
    void setDirty(const QRect &rect);
    void setPhysicalSize(const QSize &size);
    void setAvailableGeometry(const QRect &rect);
    void setSize(const QSize &size);
    void setSizeParameters(const QSize &physicalSize, const QSize &size,
                           const QRect &availableGeometry);
    void setRefreshRate(qreal refreshRate);
    void setOrientation(Qt::ScreenOrientation orientation);

protected:
    bool event(QEvent *event) override;

    typedef QList<QAndroidPlatformWindow *> WindowStackType;
    WindowStackType m_windowStack;
    QRect m_dirtyRect;
    bool m_updatePending = false;

    QRect m_availableGeometry;
    int m_depth;
    QImage::Format m_format;
    QSizeF m_physicalSize;
    qreal m_refreshRate;
    QString m_name;
    QVector<Mode> m_modes;
    int m_currentMode = 0;

private:
    QDpi logicalDpi() const override;
    QDpi logicalBaseDpi() const override;
    Qt::ScreenOrientation orientation() const override;
    Qt::ScreenOrientation nativeOrientation() const override;
    QPixmap grabWindow(WId window, int x, int y, int width, int height) const override;
    void surfaceChanged(JNIEnv *env, jobject surface, int w, int h) override;
    void releaseSurface();
    void applicationStateChanged(Qt::ApplicationState);
    QPixmap doScreenShot(QRect grabRect = QRect());

private slots:
    void doRedraw(QImage *screenGrabImage = nullptr);

private:
    int m_id = -1;
    QAtomicInt m_rasterSurfaces = 0;
    ANativeWindow* m_nativeSurface = nullptr;
    QWaitCondition m_surfaceWaitCondition;
    QSize m_size;

    QImage m_lastScreenshot;
    QImage::Format m_pixelFormat = QImage::Format_RGBA8888_Premultiplied;
    bool m_repaintOccurred = false;
};

QT_END_NAMESPACE
#endif
