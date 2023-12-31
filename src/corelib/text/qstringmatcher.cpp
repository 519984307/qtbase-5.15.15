/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2019 Mail.ru Group.
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

#include "qstringmatcher.h"

QT_BEGIN_NAMESPACE

static void bm_init_skiptable(const ushort *uc, qsizetype len, uchar *skiptable, Qt::CaseSensitivity cs)
{
    int l = int(qMin(len, qsizetype(255)));
    memset(skiptable, l, 256 * sizeof(uchar));
    uc += len - l;
    if (cs == Qt::CaseSensitive) {
        while (l--) {
            skiptable[*uc & 0xff] = l;
            ++uc;
        }
    } else {
        const ushort *start = uc;
        while (l--) {
            skiptable[foldCase(uc, start) & 0xff] = l;
            ++uc;
        }
    }
}

static inline qsizetype bm_find(const ushort *uc, qsizetype l, qsizetype index, const ushort *puc, qsizetype pl,
                          const uchar *skiptable, Qt::CaseSensitivity cs)
{
    if (pl == 0)
        return index > l ? -1 : index;
    const qsizetype pl_minus_one = pl - 1;

    const ushort *current = uc + index + pl_minus_one;
    const ushort *end = uc + l;
    if (cs == Qt::CaseSensitive) {
        while (current < end) {
            qsizetype skip = skiptable[*current & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (*(current - skip) != puc[pl_minus_one-skip])
                        break;
                    ++skip;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;

                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[*(current - skip) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    } else {
        while (current < end) {
            qsizetype skip = skiptable[foldCase(current, uc) & 0xff];
            if (!skip) {
                // possible match
                while (skip < pl) {
                    if (foldCase(current - skip, uc) != foldCase(puc + pl_minus_one - skip, puc))
                        break;
                    ++skip;
                }
                if (skip > pl_minus_one) // we have a match
                    return (current - uc) - pl_minus_one;
                // in case we don't have a match we are a bit inefficient as we only skip by one
                // when we have the non matching char in the string.
                if (skiptable[foldCase(current - skip, uc) & 0xff] == pl)
                    skip = pl - skip;
                else
                    skip = 1;
            }
            if (current > end - skip)
                break;
            current += skip;
        }
    }
    return -1; // not found
}

/*!
    \class QStringMatcher
    \inmodule QtCore
    \brief The QStringMatcher class holds a sequence of characters that
    can be quickly matched in a Unicode string.

    \ingroup tools
    \ingroup string-processing

    This class is useful when you have a sequence of \l{QChar}s that
    you want to repeatedly match against some strings (perhaps in a
    loop), or when you want to search for the same sequence of
    characters multiple times in the same string. Using a matcher
    object and indexIn() is faster than matching a plain QString with
    QString::indexOf() if repeated matching takes place. This class
    offers no benefit if you are doing one-off string matches.

    Create the QStringMatcher with the QString you want to search
    for. Then call indexIn() on the QString that you want to search.

    \sa QString, QByteArrayMatcher, QRegExp
*/

/*!
    Constructs an empty string matcher that won't match anything.
    Call setPattern() to give it a pattern to match.
*/
QStringMatcher::QStringMatcher()
    : d_ptr(nullptr), q_cs(Qt::CaseSensitive)
{
    memset(q_data, 0, sizeof(q_data));
}

/*!
    Constructs a string matcher that will search for \a pattern, with
    case sensitivity \a cs.

    Call indexIn() to perform a search.
*/
QStringMatcher::QStringMatcher(const QString &pattern, Qt::CaseSensitivity cs)
    : d_ptr(nullptr), q_pattern(pattern), q_cs(cs)
{
    p.uc = pattern.unicode();
    p.len = pattern.size();
    bm_init_skiptable((const ushort *)p.uc, p.len, p.q_skiptable, cs);
}

/*!
    \fn QStringMatcher::QStringMatcher(const QChar *uc, int length, Qt::CaseSensitivity cs)
    \since 4.5

    Constructs a string matcher that will search for the pattern referred to
    by \a uc with the given \a length and case sensitivity specified by \a cs.
*/
QStringMatcher::QStringMatcher(const QChar *uc, int len, Qt::CaseSensitivity cs)
    : QStringMatcher(QStringView(uc, len), cs)
{
}

/*!
    \fn QStringMatcher::QStringMatcher(QStringView pattern, Qt::CaseSensitivity cs = Qt::CaseSensitive)
    \since 5.14

    Constructs a string matcher that will search for \a pattern, with
    case sensitivity \a cs.

    Call indexIn() to perform a search.
*/
QStringMatcher::QStringMatcher(QStringView str, Qt::CaseSensitivity cs)
    : d_ptr(nullptr), q_cs(cs)
{
    p.uc = str.data();
    p.len = int(str.size());
    bm_init_skiptable((const ushort *)p.uc, p.len, p.q_skiptable, cs);
}
/*!
    Copies the \a other string matcher to this string matcher.
*/
QStringMatcher::QStringMatcher(const QStringMatcher &other)
    : d_ptr(nullptr)
{
    operator=(other);
}

/*!
    Destroys the string matcher.
*/
QStringMatcher::~QStringMatcher()
{
    Q_UNUSED(d_ptr);
}

/*!
    Assigns the \a other string matcher to this string matcher.
*/
QStringMatcher &QStringMatcher::operator=(const QStringMatcher &other)
{
    if (this != &other) {
        q_pattern = other.q_pattern;
        q_cs = other.q_cs;
        memcpy(q_data, other.q_data, sizeof(q_data));
    }
    return *this;
}

/*!
    Sets the string that this string matcher will search for to \a
    pattern.

    \sa pattern(), setCaseSensitivity(), indexIn()
*/
void QStringMatcher::setPattern(const QString &pattern)
{
    q_pattern = pattern;
    p.uc = pattern.unicode();
    p.len = pattern.size();
    bm_init_skiptable((const ushort *)pattern.unicode(), pattern.size(), p.q_skiptable, q_cs);
}

/*!
    \fn QString QStringMatcher::pattern() const

    Returns the string pattern that this string matcher will search
    for.

    \sa setPattern()
*/

QString QStringMatcher::pattern() const
{
    if (!q_pattern.isEmpty())
        return q_pattern;
    return QString(p.uc, p.len);
}

/*!
    Sets the case sensitivity setting of this string matcher to \a
    cs.

    \sa caseSensitivity(), setPattern(), indexIn()
*/
void QStringMatcher::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (cs == q_cs)
        return;
    bm_init_skiptable((const ushort *)p.uc, p.len, p.q_skiptable, cs);
    q_cs = cs;
}

/*!
    Searches the string \a str from character position \a from
    (default 0, i.e. from the first character), for the string
    pattern() that was set in the constructor or in the most recent
    call to setPattern(). Returns the position where the pattern()
    matched in \a str, or -1 if no match was found.

    \sa setPattern(), setCaseSensitivity()
*/
int QStringMatcher::indexIn(const QString &str, int from) const
{
    return int(indexIn(QStringView(str), from));
}

/*!
    \since 4.5

    Searches the string starting at \a str (of length \a length) from
    character position \a from (default 0, i.e. from the first
    character), for the string pattern() that was set in the
    constructor or in the most recent call to setPattern(). Returns
    the position where the pattern() matched in \a str, or -1 if no
    match was found.

    \sa setPattern(), setCaseSensitivity()
*/
int QStringMatcher::indexIn(const QChar *str, int length, int from) const
{
    return int(indexIn(QStringView(str, length), from));
}

/*!
    \since 5.14

    Searches the string \a str from character position \a from
    (default 0, i.e. from the first character), for the string
    pattern() that was set in the constructor or in the most recent
    call to setPattern(). Returns the position where the pattern()
    matched in \a str, or -1 if no match was found.

    \sa setPattern(), setCaseSensitivity()
*/
qsizetype QStringMatcher::indexIn(QStringView str, qsizetype from) const
{
    if (from < 0)
        from = 0;
    return bm_find((const ushort *)str.data(), str.size(), from,
                   (const ushort *)p.uc, p.len,
                   p.q_skiptable, q_cs);
}

/*!
    \fn Qt::CaseSensitivity QStringMatcher::caseSensitivity() const

    Returns the case sensitivity setting for this string matcher.

    \sa setCaseSensitivity()
*/

/*!
    \internal
*/

qsizetype qFindStringBoyerMoore(
    QStringView haystack, qsizetype haystackOffset,
    QStringView needle, Qt::CaseSensitivity cs)
{
    uchar skiptable[256];
    bm_init_skiptable((const ushort *)needle.data(), needle.size(), skiptable, cs);
    if (haystackOffset < 0)
        haystackOffset = 0;
    return bm_find((const ushort *)haystack.data(), haystack.size(), haystackOffset,
                   (const ushort *)needle.data(), needle.size(), skiptable, cs);
}

QT_END_NAMESPACE
