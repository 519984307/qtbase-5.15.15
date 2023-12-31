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

#ifndef QWINDOWSSCREEN_H
#define QWINDOWSSCREEN_H

#include "qtwindowsglobal.h"

#include <QtCore/qlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qpair.h>
#include <QtCore/qscopedpointer.h>
#include <qpa/qplatformscreen.h>

QT_BEGIN_NAMESPACE

struct QWindowsScreenData
{
    enum Flags
    {
        PrimaryScreen = 0x1,
        VirtualDesktop = 0x2,
        LockScreen = 0x4 // Temporary screen existing during user change, etc.
    };

    QRect geometry;
    QRect availableGeometry;
    QDpi dpi{96, 96};
    QSizeF physicalSizeMM;
    int depth = 32;
    QImage::Format format = QImage::Format_ARGB32_Premultiplied;
    unsigned flags = VirtualDesktop;
    QString name;
    Qt::ScreenOrientation orientation = Qt::LandscapeOrientation;
    qreal refreshRateHz = 60;
    HMONITOR hMonitor = nullptr;
};

class QWindowsScreen : public QPlatformScreen
{
public:
#ifndef QT_NO_CURSOR
    using CursorPtr = QScopedPointer<QPlatformCursor>;
#endif

    explicit QWindowsScreen(const QWindowsScreenData &data);

    QRect geometry() const override { return m_data.geometry; }
    QRect availableGeometry() const override { return m_data.availableGeometry; }
    int depth() const override { return m_data.depth; }
    QImage::Format format() const override { return m_data.format; }
    QSizeF physicalSize() const override { return m_data.physicalSizeMM; }
    QDpi logicalDpi() const override { return m_data.dpi; }
    QDpi logicalBaseDpi() const override { return QDpi(96, 96); };
    qreal devicePixelRatio() const override { return 1.0; }
    qreal refreshRate() const override { return m_data.refreshRateHz; }
    QString name() const override { return m_data.name; }
    Qt::ScreenOrientation orientation() const override { return m_data.orientation; }
    QList<QPlatformScreen *> virtualSiblings() const override;
    QWindow *topLevelAt(const QPoint &point) const override;
    static QWindow *windowAt(const QPoint &point, unsigned flags);

    QPixmap grabWindow(WId window, int qX, int qY, int qWidth, int qHeight) const override;
    QPlatformScreen::SubpixelAntialiasingType subpixelAntialiasingTypeHint() const override;

    static Qt::ScreenOrientation orientationPreference();
    static bool setOrientationPreference(Qt::ScreenOrientation o);

    inline void handleChanges(const QWindowsScreenData &newData);

    HMONITOR handle() const;

#ifndef QT_NO_CURSOR
    QPlatformCursor *cursor() const override { return m_cursor.data(); }
    const CursorPtr &cursorPtr() const { return m_cursor; }
#else
    QPlatformCursor *cursor() const               { return 0; }
#endif // !QT_NO_CURSOR

    const QWindowsScreenData &data() const  { return m_data; }

    static QRect virtualGeometry(const QPlatformScreen *screen);

private:
    QWindowsScreenData m_data;
#ifndef QT_NO_CURSOR
    const CursorPtr m_cursor;
#endif
};

class QWindowsScreenManager
{
public:
    using WindowsScreenList = QVector<QWindowsScreen *>;

    QWindowsScreenManager();

    void clearScreens();

    bool handleScreenChanges();
    bool handleDisplayChange(WPARAM wParam, LPARAM lParam);
    const WindowsScreenList &screens() const { return m_screens; }

    const QWindowsScreen *screenAtDp(const QPoint &p) const;
    const QWindowsScreen *screenForHwnd(HWND hwnd) const;

    static bool isSingleScreen();

private:
    void removeScreen(int index);

    WindowsScreenList m_screens;
    int m_lastDepth = -1;
    WORD m_lastHorizontalResolution = 0;
    WORD m_lastVerticalResolution = 0;
};

QT_END_NAMESPACE

#endif // QWINDOWSSCREEN_H
