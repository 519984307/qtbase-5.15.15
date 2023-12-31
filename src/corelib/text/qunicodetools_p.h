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

#ifndef QUNICODETOOLS_P_H
#define QUNICODETOOLS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/private/qglobal_p.h>
#include <QtCore/qchar.h>

QT_BEGIN_NAMESPACE

struct QCharAttributes
{
    uchar graphemeBoundary : 1;
    uchar wordBreak        : 1;
    uchar sentenceBoundary : 1;
    uchar lineBreak        : 1;
    uchar whiteSpace       : 1;
    uchar wordStart        : 1;
    uchar wordEnd          : 1;
    uchar mandatoryBreak   : 1;
};
Q_DECLARE_TYPEINFO(QCharAttributes, Q_PRIMITIVE_TYPE);

namespace QUnicodeTools {

// ### temporary
struct ScriptItem
{
    int position;
    int script;
};

} // namespace QUnicodeTools
Q_DECLARE_TYPEINFO(QUnicodeTools::ScriptItem, Q_PRIMITIVE_TYPE);
namespace QUnicodeTools {

enum CharAttributeOption {
    GraphemeBreaks = 0x01,
    WordBreaks = 0x02,
    SentenceBreaks = 0x04,
    LineBreaks = 0x08,
    WhiteSpaces = 0x10,
    HangulLineBreakTailoring = 0x20,
    DefaultOptionsCompat = GraphemeBreaks | LineBreaks | WhiteSpaces, // ### remove

    DontClearAttributes = 0x1000
};
Q_DECLARE_FLAGS(CharAttributeOptions, CharAttributeOption)

// attributes buffer has to have a length of string length + 1
Q_CORE_EXPORT void initCharAttributes(const ushort *string, int length,
                                      const ScriptItem *items, int numItems,
                                      QCharAttributes *attributes, CharAttributeOptions options = DefaultOptionsCompat);


Q_CORE_EXPORT void initScripts(const ushort *string, int length, uchar *scripts);

} // namespace QUnicodeTools

QT_END_NAMESPACE

#endif // QUNICODETOOLS_P_H
