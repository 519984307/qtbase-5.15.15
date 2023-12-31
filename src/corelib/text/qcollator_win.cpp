/****************************************************************************
**
** Copyright (C) 2020 Aleix Pol Gonzalez <aleixpol@kde.org>
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

#include "qcollator_p.h"
#include "qlocale_p.h"
#include "qstringlist.h"
#include "qstring.h"

#include <QDebug>

#include <qt_windows.h>
#include <qsysinfo.h>

QT_BEGIN_NAMESPACE

//NOTE: SORT_DIGITSASNUMBERS is available since win7
#ifndef SORT_DIGITSASNUMBERS
#define SORT_DIGITSASNUMBERS 8
#endif

// implemented in qlocale_win.cpp
extern LCID qt_inIsoNametoLCID(const char *name);

void QCollatorPrivate::init()
{
    collator = 0;
    if (isC())
        return;

#ifndef USE_COMPARESTRINGEX
    localeID = qt_inIsoNametoLCID(QLocalePrivate::get(locale)->bcp47Name().constData());
#else
    localeName = locale.bcp47Name();
#endif

    if (caseSensitivity == Qt::CaseInsensitive)
        collator |= NORM_IGNORECASE;

    // WINE does not support SORT_DIGITSASNUMBERS :-(
    // (and its std::sort() crashes on bad comparisons, QTBUG-74209)
    if (numericMode)
        collator |= SORT_DIGITSASNUMBERS;

    if (ignorePunctuation)
        collator |= NORM_IGNORESYMBOLS;

    dirty = false;
}

void QCollatorPrivate::cleanup()
{
}

int QCollator::compare(QStringView s1, QStringView s2) const
{
    if (!s1.size())
        return s2.size() ? -1 : 0;
    if (!s2.size())
        return +1;

    if (d->isC())
        return s1.compare(s2, d->caseSensitivity);

    if (d->dirty)
        d->init();

    //* from Windows documentation *
    // Returns one of the following values if successful. To maintain the C
    // runtime convention of comparing strings, the value 2 can be subtracted
    // from a nonzero return value. Then, the meaning of <0, ==0, and >0 is
    // consistent with the C runtime.
    // [...] The function returns 0 if it does not succeed.
    // https://docs.microsoft.com/en-us/windows/desktop/api/stringapiset/nf-stringapiset-comparestringex#return-value

#ifndef USE_COMPARESTRINGEX
    const int ret = CompareString(d->localeID, d->collator,
                                  reinterpret_cast<const wchar_t *>(s1.data()), s1.size(),
                                  reinterpret_cast<const wchar_t *>(s2.data()), s2.size());
#else
    const int ret = CompareStringEx(LPCWSTR(d->localeName.utf16()), d->collator,
                                    reinterpret_cast<LPCWSTR>(s1.data()), s1.size(),
                                    reinterpret_cast<LPCWSTR>(s2.data()), s2.size(),
                                    nullptr, nullptr, 0);
#endif
    if (Q_LIKELY(ret))
        return ret - 2;

    switch (DWORD error = GetLastError()) {
    case ERROR_INVALID_FLAGS:
        qWarning("Unsupported flags (%d) used in QCollator", int(d->collator));
        break;
    case ERROR_INVALID_PARAMETER:
        qWarning("Invalid parameter for QCollator::compare()");
        break;
    default:
        qWarning("Failed (%ld) comparison in QCollator::compare()", long(error));
        break;
    }
    // We have no idea what to return, so pretend we think they're equal.
    // At least that way we'll be consistent if we get the same values swapped ...
    return 0;
}

QCollatorSortKey QCollator::sortKey(const QString &string) const
{
    if (d->dirty)
        d->init();
    if (d->isC())
        return QCollatorSortKey(new QCollatorSortKeyPrivate(string));

#ifndef USE_COMPARESTRINGEX
    int size = LCMapStringW(d->localeID, LCMAP_SORTKEY | d->collator,
                           reinterpret_cast<const wchar_t*>(string.constData()), string.size(),
                           0, 0);
#else
    int size = LCMapStringEx(LPCWSTR(d->localeName.utf16()), LCMAP_SORTKEY | d->collator,
                           reinterpret_cast<LPCWSTR>(string.constData()), string.size(),
                           0, 0, NULL, NULL, 0);
#endif
    QString ret(size, Qt::Uninitialized);
#ifndef USE_COMPARESTRINGEX
    int finalSize = LCMapStringW(d->localeID, LCMAP_SORTKEY | d->collator,
                           reinterpret_cast<const wchar_t*>(string.constData()), string.size(),
                           reinterpret_cast<wchar_t*>(ret.data()), ret.size());
#else
    int finalSize = LCMapStringEx(LPCWSTR(d->localeName.utf16()), LCMAP_SORTKEY | d->collator,
                           reinterpret_cast<LPCWSTR>(string.constData()), string.size(),
                           reinterpret_cast<LPWSTR>(ret.data()), ret.size(),
                           NULL, NULL, 0);
#endif
    if (finalSize == 0) {
        qWarning()
            << "there were problems when generating the ::sortKey by LCMapStringW with error:"
            << GetLastError();
    }
    return QCollatorSortKey(new QCollatorSortKeyPrivate(std::move(ret)));
}

int QCollatorSortKey::compare(const QCollatorSortKey &otherKey) const
{
    return d->m_key.compare(otherKey.d->m_key);
}

QT_END_NAMESPACE
