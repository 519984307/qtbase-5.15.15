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

#ifndef QGLGRADIENTCACHE_P_H
#define QGLGRADIENTCACHE_P_H

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

#include <QMultiHash>
#include <QObject>
#include <QtOpenGL/QtOpenGL>
#include <private/qgl_p.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

class QGL2GradientCache : public QOpenGLSharedResource
{
    struct CacheInfo
    {
        inline CacheInfo(QGradientStops s, qreal op, QGradient::InterpolationMode mode) :
            stops(s), opacity(op), interpolationMode(mode) {}

        GLuint texId;
        QGradientStops stops;
        qreal opacity;
        QGradient::InterpolationMode interpolationMode;
    };

    typedef QMultiHash<quint64, CacheInfo> QGLGradientColorTableHash;

public:
    static QGL2GradientCache *cacheForContext(const QGLContext *context);

    QGL2GradientCache(QOpenGLContext *);
    ~QGL2GradientCache();

    GLuint getBuffer(const QGradient &gradient, qreal opacity);
    inline int paletteSize() const { return 1024; }

    void invalidateResource() override;
    void freeResource(QOpenGLContext *ctx) override;

private:
    inline int maxCacheSize() const { return 60; }
    inline void generateGradientColorTable(const QGradient& gradient,
                                           uint *colorTable,
                                           int size, qreal opacity) const;
    GLuint addCacheElement(quint64 hash_val, const QGradient &gradient, qreal opacity);
    void cleanCache();

    QGLGradientColorTableHash cache;
    QMutex m_mutex;
};

QT_END_NAMESPACE

#endif // QGLGRADIENTCACHE_P_H
