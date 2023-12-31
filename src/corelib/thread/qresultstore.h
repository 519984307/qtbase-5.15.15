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

#ifndef QTCORE_RESULTSTORE_H
#define QTCORE_RESULTSTORE_H

#include <QtCore/qmap.h>
#include <QtCore/qdebug.h>

QT_REQUIRE_CONFIG(future);

QT_BEGIN_NAMESPACE


/*
    ResultStore stores indexed results. Results can be added and retrieved
    either individually batched in a QVector. Retriveing results and checking
    which indexes are in the store can be done either by iterating or by random
    accees. In addition results kan be removed from the front of the store,
    either individually or in batches.
*/


namespace QtPrivate {

class ResultItem
{
public:
    ResultItem(const void *_result, int _count) : m_count(_count), result(_result) { } // contruct with vector of results
    ResultItem(const void *_result) : m_count(0), result(_result) { } // construct with result
    ResultItem() : m_count(0), result(nullptr) { }
    bool isValid() const { return result != nullptr; }
    bool isVector() const { return m_count != 0; }
    int count() const { return (m_count == 0) ?  1 : m_count; }
    int m_count;          // result is either a pointer to a result or to a vector of results,
    const void *result; // if count is 0 it's a result, otherwise it's a vector.
};

class Q_CORE_EXPORT ResultIteratorBase
{
public:
    ResultIteratorBase();
    ResultIteratorBase(QMap<int, ResultItem>::const_iterator _mapIterator, int _vectorIndex = 0);
    int vectorIndex() const;
    int resultIndex() const;

    ResultIteratorBase operator++();
    int batchSize() const;
    void batchedAdvance();
    bool operator==(const ResultIteratorBase &other) const;
    bool operator!=(const ResultIteratorBase &other) const;
    bool isVector() const;
    bool canIncrementVectorIndex() const;
protected:
    QMap<int, ResultItem>::const_iterator mapIterator;
    int m_vectorIndex;
public:
    template <typename T>
    const T &value() const
    {
        return *pointer<T>();
    }

    template <typename T>
    const T *pointer() const
    {
        if (mapIterator.value().isVector())
            return &(reinterpret_cast<const QVector<T> *>(mapIterator.value().result)->at(m_vectorIndex));
        else
            return reinterpret_cast<const T *>(mapIterator.value().result);
    }
};

class Q_CORE_EXPORT ResultStoreBase
{
public:
    ResultStoreBase();
    void setFilterMode(bool enable);
    bool filterMode() const;
    int addResult(int index, const void *result);
    int addResults(int index, const void *results, int vectorSize, int logicalCount);
    ResultIteratorBase begin() const;
    ResultIteratorBase end() const;
    bool hasNextResult() const;
    ResultIteratorBase resultAt(int index) const;
    bool contains(int index) const;
    int count() const;
    virtual ~ResultStoreBase();

protected:
    int insertResultItem(int index, ResultItem &resultItem);
    void insertResultItemIfValid(int index, ResultItem &resultItem);
    void syncPendingResults();
    void syncResultCount();
    int updateInsertIndex(int index, int _count);

    QMap<int, ResultItem> m_results;
    int insertIndex;     // The index where the next results(s) will be inserted.
    int resultCount;     // The number of consecutive results stored, starting at index 0.

    bool m_filterMode;
    QMap<int, ResultItem> pendingResults;
    int filteredResults;

public:
    template <typename T>
    int addResult(int index, const T *result)
    {
        if (result == nullptr)
            return addResult(index, static_cast<void *>(nullptr));
        else
            return addResult(index, static_cast<void *>(new T(*result)));
    }

    template <typename T>
    int addResults(int index, const QVector<T> *results)
    {
        if (results->empty()) // reject if results are empty
            return -1;

        return addResults(index, new QVector<T>(*results), results->count(), results->count());
    }

    template <typename T>
    int addResults(int index, const QVector<T> *results, int totalCount)
    {
        // reject if results are empty, and nothing is filtered away
        if ((m_filterMode == false || results->count() == totalCount) && results->empty())
            return -1;

        if (m_filterMode == true && results->count() != totalCount && 0 == results->count())
            return addResults(index, nullptr, 0, totalCount);
        else
            return addResults(index, new QVector<T>(*results), results->count(), totalCount);
    }

    int addCanceledResult(int index)
    {
        return addResult(index, static_cast<void *>(nullptr));
    }

    template <typename T>
    int addCanceledResults(int index, int _count)
    {
        QVector<T> empty;
        return addResults(index, &empty, _count);
    }

    template <typename T>
    void clear()
    {
        QMap<int, ResultItem>::const_iterator mapIterator = m_results.constBegin();
        while (mapIterator != m_results.constEnd()) {
            if (mapIterator.value().isVector())
                delete reinterpret_cast<const QVector<T> *>(mapIterator.value().result);
            else
                delete reinterpret_cast<const T *>(mapIterator.value().result);
            ++mapIterator;
        }
        resultCount = 0;
        m_results.clear();
    }
};

} // namespace QtPrivate

Q_DECLARE_TYPEINFO(QtPrivate::ResultItem, Q_PRIMITIVE_TYPE);


QT_END_NAMESPACE

#endif
