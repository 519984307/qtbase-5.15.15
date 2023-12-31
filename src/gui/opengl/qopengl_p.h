/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QOPENGL_P_H
#define QOPENGL_P_H

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

#include <QtGui/private/qtguiglobal_p.h>
#include <qopengl.h>
#include <private/qopenglcontext_p.h>
#include <QtCore/qset.h>
#include <QtCore/qstring.h>
#include <QtCore/qversionnumber.h>

QT_BEGIN_NAMESPACE

class QJsonDocument;

class Q_GUI_EXPORT QOpenGLExtensionMatcher
{
public:
    QOpenGLExtensionMatcher();

    bool match(const QByteArray &extension) const
    {
        return m_extensions.contains(extension);
    }

    QSet<QByteArray> extensions() const { return m_extensions; }

private:
    QSet<QByteArray> m_extensions;
};

class Q_GUI_EXPORT QOpenGLConfig
{
public:
    struct Q_GUI_EXPORT Gpu {
        Gpu() : vendorId(0), deviceId(0) {}
        bool isValid() const { return deviceId || !glVendor.isEmpty(); }
        bool equals(const Gpu &other) const {
            return vendorId == other.vendorId && deviceId == other.deviceId && driverVersion == other.driverVersion
                && driverDescription == other.driverDescription && glVendor == other.glVendor;
        }

        uint vendorId;
        uint deviceId;
        QVersionNumber driverVersion;
        QByteArray driverDescription;
        QByteArray glVendor;

        static Gpu fromDevice(uint vendorId, uint deviceId, QVersionNumber driverVersion, const QByteArray &driverDescription) {
            Gpu gpu;
            gpu.vendorId = vendorId;
            gpu.deviceId = deviceId;
            gpu.driverVersion = driverVersion;
            gpu.driverDescription = driverDescription;
            return gpu;
        }

        static Gpu fromGLVendor(const QByteArray &glVendor) {
            Gpu gpu;
            gpu.glVendor = glVendor;
            return gpu;
        }

        static Gpu fromContext();
    };

    static QSet<QString> gpuFeatures(const Gpu &gpu,
                                     const QString &osName, const QVersionNumber &kernelVersion, const QString &osVersion,
                                     const QJsonDocument &doc);
    static QSet<QString> gpuFeatures(const Gpu &gpu,
                                     const QString &osName, const QVersionNumber &kernelVersion, const QString &osVersion,
                                     const QString &fileName);
    static QSet<QString> gpuFeatures(const Gpu &gpu, const QJsonDocument &doc);
    static QSet<QString> gpuFeatures(const Gpu &gpu, const QString &fileName);
};

inline bool operator==(const QOpenGLConfig::Gpu &a, const QOpenGLConfig::Gpu &b)
{
    return a.equals(b);
}

inline bool operator!=(const QOpenGLConfig::Gpu &a, const QOpenGLConfig::Gpu &b)
{
    return !a.equals(b);
}

inline uint qHash(const QOpenGLConfig::Gpu &gpu)
{
    return qHash(gpu.vendorId) + qHash(gpu.deviceId) + qHash(gpu.driverVersion);
}

QT_END_NAMESPACE

#endif // QOPENGL_H
