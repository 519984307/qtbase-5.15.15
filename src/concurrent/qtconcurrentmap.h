/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtConcurrent module of the Qt Toolkit.
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

#ifndef QTCONCURRENT_MAP_H
#define QTCONCURRENT_MAP_H

#include <QtConcurrent/qtconcurrent_global.h>

#if !defined(QT_NO_CONCURRENT) || defined(Q_CLANG_QDOC)

#include <QtConcurrent/qtconcurrentmapkernel.h>
#include <QtConcurrent/qtconcurrentreducekernel.h>
#include <QtConcurrent/qtconcurrentfunctionwrappers.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE



namespace QtConcurrent {

// map() on sequences
template <typename Sequence, typename MapFunctor>
QFuture<void> map(Sequence &sequence, MapFunctor map)
{
    return startMap(sequence.begin(), sequence.end(), QtPrivate::createFunctionWrapper(map));
}

// map() on iterators
template <typename Iterator, typename MapFunctor>
QFuture<void> map(Iterator begin, Iterator end, MapFunctor map)
{
    return startMap(begin, end, QtPrivate::createFunctionWrapper(map));
}

// mappedReduced() for sequences.
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, ResultType>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
}

template <typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
}

// mappedReduced() for iterators
template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor>
QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
}

// mapped() for sequences
template <typename Sequence, typename MapFunctor>
QFuture<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType> mapped(const Sequence &sequence, MapFunctor map)
{
    return startMapped<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType>(sequence, QtPrivate::createFunctionWrapper(map));
}

// mapped() for iterator ranges.
template <typename Iterator, typename MapFunctor>
QFuture<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType> mapped(Iterator begin, Iterator end, MapFunctor map)
{
    return startMapped<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType>(begin, end, QtPrivate::createFunctionWrapper(map));
}

// blockingMap() for sequences
template <typename Sequence, typename MapFunctor>
void blockingMap(Sequence &sequence, MapFunctor map)
{
    QFuture<void> future = startMap(sequence.begin(), sequence.end(), QtPrivate::createFunctionWrapper(map));
    future.waitForFinished();
}

// blockingMap() for iterator ranges
template <typename Iterator, typename MapFunctor>
void blockingMap(Iterator begin, Iterator end, MapFunctor map)
{
    QFuture<void> future = startMap(begin, end, QtPrivate::createFunctionWrapper(map));
    future.waitForFinished();
}

// blockingMappedReduced() for sequences
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    QFuture<ResultType> future =
    QtConcurrent::startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, ResultType>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
    return future.result();
}

template <typename MapFunctor, typename ReduceFunctor, typename Sequence>
typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> future =
    QtConcurrent::startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
    return future.result();
}

// blockingMappedReduced() for iterator ranges
template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    QFuture<ResultType> future =
    QtConcurrent::startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
    return future.result();
}

template <typename Iterator, typename MapFunctor, typename ReduceFunctor>
typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    QFuture<typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType> future =
    QtConcurrent::startMappedReduced<typename QtPrivate::MapResultType<void, MapFunctor>::ResultType, typename QtPrivate::ReduceResultType<ReduceFunctor>::ResultType>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::createFunctionWrapper(reduce),
         options);
    return future.result();
}

// mapped() for sequences with a different putput sequence type.
template <typename OutputSequence, typename InputSequence, typename MapFunctor>
OutputSequence blockingMapped(const InputSequence &sequence, MapFunctor map)
{
    return blockingMappedReduced<OutputSequence>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::PushBackWrapper(),
         QtConcurrent::OrderedReduce);
}

template <typename MapFunctor, typename InputSequence>
typename QtPrivate::MapResultType<InputSequence, MapFunctor>::ResultType blockingMapped(const InputSequence &sequence, MapFunctor map)
{
    typedef typename QtPrivate::MapResultType<InputSequence, MapFunctor>::ResultType OutputSequence;
    return blockingMappedReduced<OutputSequence>
        (sequence,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::PushBackWrapper(),
         QtConcurrent::OrderedReduce);
}

// mapped()  for iterator ranges
template <typename Sequence, typename Iterator, typename MapFunctor>
Sequence blockingMapped(Iterator begin, Iterator end, MapFunctor map)
{
    return blockingMappedReduced<Sequence>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::PushBackWrapper(),
         QtConcurrent::OrderedReduce);
}

template <typename Iterator, typename MapFunctor>
typename QtPrivate::MapResultType<Iterator, MapFunctor>::ResultType blockingMapped(Iterator begin, Iterator end, MapFunctor map)
{
    typedef typename QtPrivate::MapResultType<Iterator, MapFunctor>::ResultType OutputSequence;
    return blockingMappedReduced<OutputSequence>
        (begin, end,
         QtPrivate::createFunctionWrapper(map),
         QtPrivate::PushBackWrapper(),
         QtConcurrent::OrderedReduce);
}

} // namespace QtConcurrent


QT_END_NAMESPACE

#endif // QT_NO_CONCURRENT

#endif
