/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QDATASTREAM_P_H
#define QDATASTREAM_P_H

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

#include <QtCore/private/qglobal_p.h>
#include <qdatastream.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_DATASTREAM) || defined(QT_BOOTSTRAPPED)
class QDataStreamPrivate
{
public:
    QDataStreamPrivate() : floatingPointPrecision(QDataStream::DoublePrecision),
        transactionDepth(0) { }

    QDataStream::FloatingPointPrecision floatingPointPrecision;
    int transactionDepth;
};
#endif

QT_END_NAMESPACE

#endif // QDATASTREAM_P_H
