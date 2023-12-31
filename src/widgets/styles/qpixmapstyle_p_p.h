/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QPIXMAPSTYLE_P_H
#define QPIXMAPSTYLE_P_H

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include "qpixmapstyle_p.h"
#include "qcommonstyle_p.h"

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

struct QPixmapStyleDescriptor
{
    QString fileName;
    QSize size;
    QMargins margins;
    QTileRules tileRules;
};

struct QPixmapStylePixmap
{
    QPixmap pixmap;
    QMargins margins;
};

class QPixmapStylePrivate : public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QPixmapStyle)

public:
    QHash<QPixmapStyle::ControlDescriptor, QPixmapStyleDescriptor> descriptors;
    QHash<QPixmapStyle::ControlPixmap, QPixmapStylePixmap> pixmaps;

    static QPixmap scale(int w, int h, const QPixmap &pixmap, const QPixmapStyleDescriptor &desc);

    QPixmap getCachedPixmap(QPixmapStyle::ControlDescriptor control,
                            const QPixmapStyleDescriptor &desc,
                            const QSize &size) const;

    QSize computeSize(const QPixmapStyleDescriptor &desc, int width, int height) const;
};

QT_END_NAMESPACE

#endif // QPIXMAPSTYLE_P_H
