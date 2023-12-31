/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
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

#ifndef QJSON_P_H
#define QJSON_P_H

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

#include <qjsonvalue.h>
#include <qcborvalue.h>
#include <private/qcborvalue_p.h>

QT_BEGIN_NAMESPACE

namespace QJsonPrivate {

template<typename Element, typename ElementsIterator>
struct ObjectIterator
{
    using pointer = Element *;

    struct value_type;
    struct reference {
        reference(Element &ref) : m_key(&ref) {}

        reference() = delete;
        ~reference() = default;

        reference(const reference &other) = default;
        reference(reference &&other) = default;

        reference &operator=(const value_type &value);
        reference &operator=(const reference &other)
        {
            if (m_key != other.m_key) {
                key() = other.key();
                value() = other.value();
            }
            return *this;
        }

        reference &operator=(reference &&other)
        {
            key() = other.key();
            value() = other.value();
            return *this;
        }

        Element &key() { return *m_key; }
        Element &value() { return *(m_key + 1); }

        const Element &key() const { return *m_key; }
        const Element &value() const { return *(m_key + 1); }


    private:
        Element *m_key;
    };

    struct value_type {
        value_type(reference ref) : m_key(ref.key()), m_value(ref.value()) {}

        Element key() const { return m_key; }
        Element value() const { return m_value; }
    private:
        Element m_key;
        Element m_value;
    };

    using difference_type = typename QVector<Element>::difference_type;
    using iterator_category = std::random_access_iterator_tag;

    ObjectIterator() = default;
    ObjectIterator(ElementsIterator it) : it(it) {}
    ElementsIterator elementsIterator() { return it; }

    ObjectIterator operator++(int) { ObjectIterator ret(it); it += 2; return ret; }
    ObjectIterator &operator++() { it += 2; return *this; }
    ObjectIterator &operator+=(difference_type n) { it += 2 * n; return *this; }

    ObjectIterator operator--(int) { ObjectIterator ret(it); it -= 2; return ret; }
    ObjectIterator &operator--() { it -= 2; return *this; }
    ObjectIterator &operator-=(difference_type n) { it -= 2 * n; return *this; }

    reference operator*() const { return *it; }
    reference operator[](int n) const { return it[n * 2]; }

    bool operator<(ObjectIterator other) const { return it < other.it; }
    bool operator>(ObjectIterator other) const { return it > other.it; }
    bool operator<=(ObjectIterator other) const { return it <= other.it; }
    bool operator>=(ObjectIterator other) const { return it >= other.it; }

private:
    ElementsIterator it;
};

template<typename Element, typename ElementsIterator>
inline ObjectIterator<Element, ElementsIterator> operator+(
        ObjectIterator<Element, ElementsIterator> a,
        typename ObjectIterator<Element, ElementsIterator>::difference_type n)
{
    return {a.elementsIterator() + 2 * n};
}
template<typename Element, typename ElementsIterator>
inline ObjectIterator<Element, ElementsIterator> operator+(
        int n, ObjectIterator<Element, ElementsIterator> a)
{
    return {a.elementsIterator() + 2 * n};
}
template<typename Element, typename ElementsIterator>
inline ObjectIterator<Element, ElementsIterator> operator-(
        ObjectIterator<Element, ElementsIterator> a,
        typename ObjectIterator<Element, ElementsIterator>::difference_type n)
{
    return {a.elementsIterator() - 2 * n};
}
template<typename Element, typename ElementsIterator>
inline int operator-(
        ObjectIterator<Element, ElementsIterator> a,
        ObjectIterator<Element, ElementsIterator> b)
{
    return (a.elementsIterator() - b.elementsIterator()) / 2;
}
template<typename Element, typename ElementsIterator>
inline bool operator!=(
        ObjectIterator<Element, ElementsIterator> a,
        ObjectIterator<Element, ElementsIterator> b)
{
    return a.elementsIterator() != b.elementsIterator();
}
template<typename Element, typename ElementsIterator>
inline bool operator==(
        ObjectIterator<Element, ElementsIterator> a,
        ObjectIterator<Element, ElementsIterator> b)
{
    return a.elementsIterator() == b.elementsIterator();
}

using KeyIterator = ObjectIterator<QtCbor::Element, QVector<QtCbor::Element>::iterator>;
using ConstKeyIterator = ObjectIterator<const QtCbor::Element, QVector<QtCbor::Element>::const_iterator>;

template<>
inline KeyIterator::reference &KeyIterator::reference::operator=(const KeyIterator::value_type &value)
{
    *m_key = value.key();
    *(m_key + 1) = value.value();
    return *this;
}

inline void swap(KeyIterator::reference a, KeyIterator::reference b)
{
    KeyIterator::value_type t = a;
    a = b;
    b = t;
}

class Value
{
public:
    static QCborContainerPrivate *container(const QCborValue &v) { return v.container; }

    static QJsonValue fromTrustedCbor(const QCborValue &v)
    {
        QJsonValue result;
        result.d = v.container;
        result.n = v.n;
        result.t = v.t;
        return result;
    }
};

class Variant
{
public:
    static QJsonObject toJsonObject(const QVariantMap &map);
    static QJsonArray toJsonArray(const QVariantList &list);
};

} // namespace QJsonPrivate

QT_END_NAMESPACE

#endif // QJSON_P_H
