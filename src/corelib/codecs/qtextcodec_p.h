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

#ifndef QTEXTCODEC_P_H
#define QTEXTCODEC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QTextCodec class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/private/qglobal_p.h>
#include <string.h>

QT_BEGIN_NAMESPACE

#if QT_CONFIG(textcodec)

#include "qtextcodec.h"

#if defined(Q_OS_MAC) || defined(Q_OS_ANDROID) || defined(Q_OS_QNX) || defined(Q_OS_WASM)
#define QT_LOCALE_IS_UTF8
#endif

typedef void (*QTextCodecStateFreeFunction)(QTextCodec::ConverterState*);

struct QTextCodecUnalignedPointer
{
    static inline QTextCodecStateFreeFunction decode(const uint *src)
    {
        quintptr data;
        memcpy(&data, src, sizeof(data));
        return reinterpret_cast<QTextCodecStateFreeFunction>(data);
    }
    static inline void encode(uint *dst, QTextCodecStateFreeFunction fn)
    {
        quintptr data = reinterpret_cast<quintptr>(fn);
        memcpy(dst, &data, sizeof(data));
    }
};

bool qTextCodecNameMatch(const char *a, const char *b);

#else // without textcodec:

class QTextCodec
{
public:
    enum ConversionFlag {
        DefaultConversion,
        ConvertInvalidToNull = 0x80000000,
        IgnoreHeader = 0x1,
        FreeFunction = 0x2
    };
    Q_DECLARE_FLAGS(ConversionFlags, ConversionFlag)

    struct ConverterState {
        ConverterState(ConversionFlags f = DefaultConversion)
            : flags(f), remainingChars(0), invalidChars(0), d(nullptr) { state_data[0] = state_data[1] = state_data[2] = 0; }
        ~ConverterState() { }
        ConversionFlags flags;
        int remainingChars;
        int invalidChars;
        uint state_data[3];
        void *d;
    private:
        Q_DISABLE_COPY(ConverterState)
    };
};

#endif // textcodec

QT_END_NAMESPACE

#endif
