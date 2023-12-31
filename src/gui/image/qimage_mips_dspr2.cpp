/****************************************************************************
**
** Copyright (C) 2013 Imagination Technologies Limited, www.imgtec.com
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

#include "qimage.h"
#include <private/qimage_p.h>

QT_BEGIN_NAMESPACE

// Defined in qimage_mips_dspr2_asm.S
//
extern "C" void premultiply_argb_inplace_mips_asm(void*, unsigned, unsigned, int);

bool convert_ARGB_to_ARGB_PM_inplace_mips_dspr2(QImageData *data, Qt::ImageConversionFlags)
{
    Q_ASSERT(data->format == QImage::Format_ARGB32);

    if (!data->width || !data->height)
        return true;

    Q_ASSERT((data->bytes_per_line - (data->width << 2)) >= 0);

    premultiply_argb_inplace_mips_asm(data->data,
                                      data->height,
                                      data->width,
                                      data->bytes_per_line - (data->width << 2));

    data->format = QImage::Format_ARGB32_Premultiplied;
    return true;
}

extern "C" void qt_convert_rgb888_to_rgb32_mips_dspr2_asm(uint *dst, const uchar *src, int len);

void convert_RGB888_to_RGB32_mips_dspr2(QImageData *dest, const QImageData *src, Qt::ImageConversionFlags)
{
    Q_ASSERT(src->format == QImage::Format_RGB888);
    Q_ASSERT(dest->format == QImage::Format_RGB32 || dest->format == QImage::Format_ARGB32 || dest->format == QImage::Format_ARGB32_Premultiplied);
    Q_ASSERT(src->width == dest->width);
    Q_ASSERT(src->height == dest->height);

    const uchar *src_data = (const uchar*) src->data;
    quint32 *dest_data = (quint32*) dest->data;

    for (int i = 0; i < src->height; ++i) {
        qt_convert_rgb888_to_rgb32_mips_dspr2_asm(dest_data, src_data, src->width);
        src_data += src->bytes_per_line;
        dest_data = (quint32*) ((uchar*) dest_data + dest->bytes_per_line);
    }
}

QT_END_NAMESPACE

