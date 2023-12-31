/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qnetworkaccesscache_p.h"
#include "QtCore/qpointer.h"
#include "QtCore/qdatetime.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkreply_p.h"
#include "qnetworkrequest.h"

#include <vector>

QT_BEGIN_NAMESPACE

enum ExpiryTimeEnum {
    ExpiryTime = 120
};

namespace {
    struct Receiver
    {
        QPointer<QObject> object;
        const char *member;
    };
}

// idea copied from qcache.h
struct QNetworkAccessCache::Node
{
    QDateTime timestamp;
    std::vector<Receiver> receiverQueue;
    QByteArray key;

    Node *older, *newer;
    CacheableObject *object;

    int useCount;

    Node()
        : older(nullptr), newer(nullptr), object(nullptr), useCount(0)
    { }
};

QNetworkAccessCache::CacheableObject::CacheableObject()
{
    // leave the members uninitialized
    // they must be initialized by the derived class's constructor
}

QNetworkAccessCache::CacheableObject::~CacheableObject()
{
#if 0 //def QT_DEBUG
    if (!key.isEmpty() && Ptr()->hasEntry(key))
        qWarning() << "QNetworkAccessCache: object" << (void*)this << "key" << key
                   << "destroyed without being removed from cache first!";
#endif
}

void QNetworkAccessCache::CacheableObject::setExpires(bool enable)
{
    expires = enable;
}

void QNetworkAccessCache::CacheableObject::setShareable(bool enable)
{
    shareable = enable;
}

QNetworkAccessCache::QNetworkAccessCache()
    : oldest(nullptr), newest(nullptr)
{
}

QNetworkAccessCache::~QNetworkAccessCache()
{
    clear();
}

void QNetworkAccessCache::clear()
{
    NodeHash hashCopy = hash;
    hash.clear();

    // remove all entries
    NodeHash::Iterator it = hashCopy.begin();
    NodeHash::Iterator end = hashCopy.end();
    for ( ; it != end; ++it) {
        it->object->key.clear();
        it->object->dispose();
    }

    // now delete:
    hashCopy.clear();

    timer.stop();

    oldest = newest = nullptr;
}

/*!
    Appends the entry given by \a key to the end of the linked list.
    (i.e., makes it the newest entry)
 */
void QNetworkAccessCache::linkEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return;

    Node *const node = &it.value();
    Q_ASSERT(node != oldest && node != newest);
    Q_ASSERT(node->older == nullptr && node->newer == nullptr);
    Q_ASSERT(node->useCount == 0);

    if (newest) {
        Q_ASSERT(newest->newer == nullptr);
        newest->newer = node;
        node->older = newest;
    }
    if (!oldest) {
        // there are no entries, so this is the oldest one too
        oldest = node;
    }

    node->timestamp = QDateTime::currentDateTimeUtc().addSecs(ExpiryTime);
    newest = node;
}

/*!
    Removes the entry pointed by \a key from the linked list.
    Returns \c true if the entry removed was the oldest one.
 */
bool QNetworkAccessCache::unlinkEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return false;

    Node *const node = &it.value();

    bool wasOldest = false;
    if (node == oldest) {
        oldest = node->newer;
        wasOldest = true;
    }
    if (node == newest)
        newest = node->older;
    if (node->older)
        node->older->newer = node->newer;
    if (node->newer)
        node->newer->older = node->older;

    node->newer = node->older = nullptr;
    return wasOldest;
}

void QNetworkAccessCache::updateTimer()
{
    timer.stop();

    if (!oldest)
        return;

    int interval = QDateTime::currentDateTimeUtc().secsTo(oldest->timestamp);
    if (interval <= 0) {
        interval = 0;
    } else {
        // round up the interval
        interval = (interval + 15) & ~16;
    }

    timer.start(interval * 1000, this);
}

bool QNetworkAccessCache::emitEntryReady(Node *node, QObject *target, const char *member)
{
    if (!connect(this, SIGNAL(entryReady(QNetworkAccessCache::CacheableObject*)),
                 target, member, Qt::QueuedConnection))
        return false;

    emit entryReady(node->object);
    disconnect(SIGNAL(entryReady(QNetworkAccessCache::CacheableObject*)));

    return true;
}

void QNetworkAccessCache::timerEvent(QTimerEvent *)
{
    // expire old items
    const QDateTime now = QDateTime::currentDateTimeUtc();

    while (oldest && oldest->timestamp < now) {
        Node *next = oldest->newer;
        oldest->object->dispose();

        hash.remove(oldest->key); // oldest gets deleted
        oldest = next;
    }

    // fixup the list
    if (oldest)
        oldest->older = nullptr;
    else
        newest = nullptr;

    updateTimer();
}

void QNetworkAccessCache::addEntry(const QByteArray &key, CacheableObject *entry)
{
    Q_ASSERT(!key.isEmpty());

    if (unlinkEntry(key))
        updateTimer();

    Node &node = hash[key];     // create the entry in the hash if it didn't exist
    if (node.useCount)
        qWarning("QNetworkAccessCache::addEntry: overriding active cache entry '%s'",
                 key.constData());
    if (node.object)
        node.object->dispose();
    node.object = entry;
    node.object->key = key;
    node.key = key;
    node.useCount = 1;
}

bool QNetworkAccessCache::hasEntry(const QByteArray &key) const
{
    return hash.contains(key);
}

bool QNetworkAccessCache::requestEntry(const QByteArray &key, QObject *target, const char *member)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return false;           // no such entry

    Node *node = &it.value();

    if (node->useCount > 0 && !node->object->shareable) {
        // object is not shareable and is in use
        // queue for later use
        Q_ASSERT(node->older == nullptr && node->newer == nullptr);
        node->receiverQueue.push_back({target, member});

        // request queued
        return true;
    } else {
        // node not in use or is shareable
        if (unlinkEntry(key))
            updateTimer();

        ++node->useCount;
        return emitEntryReady(node, target, member);
    }
}

QNetworkAccessCache::CacheableObject *QNetworkAccessCache::requestEntryNow(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return nullptr;
    if (it->useCount > 0) {
        if (it->object->shareable) {
            ++it->useCount;
            return it->object;
        }

        // object in use and not shareable
        return nullptr;
    }

    // entry not in use, let the caller have it
    bool wasOldest = unlinkEntry(key);
    ++it->useCount;

    if (wasOldest)
        updateTimer();
    return it->object;
}

void QNetworkAccessCache::releaseEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end()) {
        qWarning("QNetworkAccessCache::releaseEntry: trying to release key '%s' that is not in cache",
                 key.constData());
        return;
    }

    Node *node = &it.value();
    Q_ASSERT(node->useCount > 0);

    // are there other objects waiting?
    const auto objectStillExists = [](const Receiver &r) { return !r.object.isNull(); };

    auto &queue = node->receiverQueue;
    auto qit = std::find_if(queue.begin(), queue.end(), objectStillExists);

    const Receiver receiver = qit == queue.end() ? Receiver{} : std::move(*qit++) ;

    queue.erase(queue.begin(), qit);

    if (receiver.object) {
        // queue another activation
        emitEntryReady(node, receiver.object, receiver.member);
        return;
    }

    if (!--node->useCount) {
        // no objects waiting; add it back to the expiry list
        if (node->object->expires)
            linkEntry(key);

        if (oldest == node)
            updateTimer();
    }
}

void QNetworkAccessCache::removeEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end()) {
        qWarning("QNetworkAccessCache::removeEntry: trying to remove key '%s' that is not in cache",
                 key.constData());
        return;
    }

    Node *node = &it.value();
    if (unlinkEntry(key))
        updateTimer();
    if (node->useCount > 1)
        qWarning("QNetworkAccessCache::removeEntry: removing active cache entry '%s'",
                 key.constData());

    node->object->key.clear();
    hash.remove(node->key);
}

QT_END_NAMESPACE
