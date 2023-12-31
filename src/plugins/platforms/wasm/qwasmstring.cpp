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
****************************************************************************/

#include "qwasmstring.h"

QT_BEGIN_NAMESPACE

using namespace emscripten;

val QWasmString::fromQString(const QString &str)
{
    static const val UTF16ToString(
        val::global("Module")["UTF16ToString"]);

    auto ptr = quintptr(str.utf16());
    return UTF16ToString(val(ptr));
}

QString QWasmString::toQString(const val &v)
{
    QString result;
    if (!v.isString())
        return result;

    static const val stringToUTF16(
        val::global("Module")["stringToUTF16"]);
    static const val length("length");

    int len = v[length].as<int>();
    result.resize(len);
    auto ptr = quintptr(result.utf16());
    stringToUTF16(v, val(ptr), val((len + 1) * 2));
    return result;
}

QT_END_NAMESPACE
