/***************************************************************************
**
** Copyright (C) 2014 BlackBerry Limited. All rights reserved.
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

#ifndef QQNXLGMON_H
#define QQNXLGMON_H

#include <qglobal.h>

#if defined(QQNX_LGMON)
#include <lgmon.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(QQNX_LGMON)

extern bool qqnxLgmonFirstFrame;

inline void qqnxLgmonInit()
{
    lgmon_supported(getpid());
}

inline void qqnxLgmonFramePosted(bool isCover)
{
    if (qqnxLgmonFirstFrame && !isCover) {
        qqnxLgmonFirstFrame = false;
        lgmon_app_ready_for_user_input(getpid());
    }
}

#else

inline void qqnxLgmonInit() {}
inline void qqnxLgmonFramePosted(bool /*isCover*/) {}

#endif

QT_END_NAMESPACE

#endif  // QQNXLGMON_H

