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

#ifndef QOPENGLTEXTUREUPLOADER_P_H
#define QOPENGLTEXTUREUPLOADER_P_H

#include <QtCore/qsize.h>
#include <QtGui/private/qtguiglobal_p.h>
#include <QtGui/private/qopenglcontext_p.h>

QT_BEGIN_NAMESPACE

class QImage;

class Q_GUI_EXPORT QOpenGLTextureUploader
{
public:
    enum BindOption {
        NoBindOption                            = 0x0000,
        PremultipliedAlphaBindOption            = 0x0001,
        UseRedForAlphaAndLuminanceBindOption    = 0x0002,
        SRgbBindOption                          = 0x0004,
        PowerOfTwoBindOption                    = 0x0008
    };
    Q_DECLARE_FLAGS(BindOptions, BindOption)
    Q_FLAGS(BindOptions)

    static qsizetype textureImage(GLenum target, const QImage &image, BindOptions options, QSize maxSize = QSize());

};

Q_DECLARE_OPERATORS_FOR_FLAGS(QOpenGLTextureUploader::BindOptions)

QT_END_NAMESPACE

#endif

