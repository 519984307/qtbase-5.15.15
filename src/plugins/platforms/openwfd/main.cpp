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

#include <qpa/qplatformintegrationplugin.h>
#include "qopenwfdintegration.h"

QT_BEGIN_NAMESPACE

class QOpenWFDIntegrationPlugin : public QPlatformIntegrationPlugin
{
    Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid)
public:
    QPlatformIntegration *create(const QString&, const QStringList&);
};

QPlatformIntegration* QOpenWFDIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    if (!system.compare(QLatin1String("openwfd"), Qt::CaseInsensitive))
        return new QOpenWFDIntegration;

    return 0;
}

QT_END_NAMESPACE
