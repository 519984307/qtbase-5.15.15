/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Giuseppe D'Angelo <giuseppe.dangelo@kdab.com>
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

#ifndef QSTRINGITERATOR_H
#define QSTRINGITERATOR_H

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
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QStringIterator
{
    QString::const_iterator i, pos, e;
    Q_STATIC_ASSERT((std::is_same<QString::const_iterator, const QChar *>::value));
    static bool less(const QChar *lhs, const QChar *rhs) noexcept
    { return std::less<const QChar *>{}(lhs, rhs); }
public:
    explicit QStringIterator(QStringView string, qsizetype idx = 0)
        : i(string.begin()),
          pos(i + idx),
          e(string.end())
    {
    }

    inline explicit QStringIterator(const QChar *begin, const QChar *end)
        : i(begin),
          pos(begin),
          e(end)
    {
    }

    inline explicit QStringIterator(const QChar *begin, int idx, const QChar *end)
        : i(begin),
          pos(begin + idx),
          e(end)
    {
    }

    inline QString::const_iterator position() const
    {
        return pos;
    }

    inline int index() const
    {
        return pos - i;
    }

    inline void setPosition(QString::const_iterator position)
    {
        Q_ASSERT_X(!less(position, i) && !less(e, position),
                   Q_FUNC_INFO, "position out of bounds");
        pos = position;
    }

    // forward iteration

    inline bool hasNext() const
    {
        return less(pos, e);
    }

    inline void advance()
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        if (Q_UNLIKELY((pos++)->isHighSurrogate())) {
            if (Q_LIKELY(pos != e && pos->isLowSurrogate()))
                ++pos;
        }
    }

    inline void advanceUnchecked()
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        if (Q_UNLIKELY((pos++)->isHighSurrogate()))
            ++pos;
    }

    inline uint peekNextUnchecked() const
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        if (Q_UNLIKELY(pos->isHighSurrogate()))
            return QChar::surrogateToUcs4(pos[0], pos[1]);

        return pos->unicode();
    }

    inline uint peekNext(uint invalidAs = QChar::ReplacementCharacter) const
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        if (Q_UNLIKELY(pos->isSurrogate())) {
            if (Q_LIKELY(pos->isHighSurrogate())) {
                const QChar *low = pos + 1;
                if (Q_LIKELY(low != e && low->isLowSurrogate()))
                    return QChar::surrogateToUcs4(*pos, *low);
            }
            return invalidAs;
        }

        return pos->unicode();
    }

    inline uint nextUnchecked()
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        const QChar cur = *pos++;
        if (Q_UNLIKELY(cur.isHighSurrogate()))
            return QChar::surrogateToUcs4(cur, *pos++);
        return cur.unicode();
    }

    inline uint next(uint invalidAs = QChar::ReplacementCharacter)
    {
        Q_ASSERT_X(hasNext(), Q_FUNC_INFO, "iterator hasn't a next item");

        const QChar uc = *pos++;
        if (Q_UNLIKELY(uc.isSurrogate())) {
            if (Q_LIKELY(uc.isHighSurrogate() && hasNext() && pos->isLowSurrogate()))
                return QChar::surrogateToUcs4(uc, *pos++);
            return invalidAs;
        }

        return uc.unicode();
    }

    // backwards iteration

    inline bool hasPrevious() const
    {
        return less(i, pos);
    }

    inline void recede()
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        if (Q_UNLIKELY((--pos)->isLowSurrogate())) {
            const QChar *high = pos - 1;
            if (Q_LIKELY(high != i - 1 && high->isHighSurrogate()))
                --pos;
        }
    }

    inline void recedeUnchecked()
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        if (Q_UNLIKELY((--pos)->isLowSurrogate()))
            --pos;
    }

    inline uint peekPreviousUnchecked() const
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        if (Q_UNLIKELY(pos[-1].isLowSurrogate()))
            return QChar::surrogateToUcs4(pos[-2], pos[-1]);
        return pos[-1].unicode();
    }

    inline uint peekPrevious(uint invalidAs = QChar::ReplacementCharacter) const
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        if (Q_UNLIKELY(pos[-1].isSurrogate())) {
            if (Q_LIKELY(pos[-1].isLowSurrogate())) {
                const QChar *high = pos - 2;
                if (Q_LIKELY(high != i - 1 && high->isHighSurrogate()))
                    return QChar::surrogateToUcs4(*high, pos[-1]);
            }
            return invalidAs;
        }

        return pos[-1].unicode();
    }

    inline uint previousUnchecked()
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        const QChar cur = *--pos;
        if (Q_UNLIKELY(cur.isLowSurrogate()))
            return QChar::surrogateToUcs4(*--pos, cur);
        return cur.unicode();
    }

    inline uint previous(uint invalidAs = QChar::ReplacementCharacter)
    {
        Q_ASSERT_X(hasPrevious(), Q_FUNC_INFO, "iterator hasn't a previous item");

        const QChar uc = *--pos;
        if (Q_UNLIKELY(uc.isSurrogate())) {
            if (Q_LIKELY(uc.isLowSurrogate() && hasPrevious() && pos[-1].isHighSurrogate()))
                return QChar::surrogateToUcs4(*--pos, uc);
            return invalidAs;
        }

        return uc.unicode();
    }
};

QT_END_NAMESPACE

#endif // QSTRINGITERATOR_H
