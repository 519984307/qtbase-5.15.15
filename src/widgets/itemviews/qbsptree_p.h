/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QBSPTREE_P_H
#define QBSPTREE_P_H

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

#include <QtWidgets/private/qtwidgetsglobal_p.h>
#include <qvector.h>
#include <qrect.h>

QT_BEGIN_NAMESPACE

class QBspTree
{
public:

    struct Node
    {
        enum Type { None = 0, VerticalPlane = 1, HorizontalPlane = 2, Both = 3 };
        inline Node() : pos(0), type(None) {}
        int pos;
        Type type;
    };
    typedef Node::Type NodeType;

    struct Data
    {
        Data(void *p) : ptr(p) {}
        Data(int n) : i(n) {}
        union {
            void *ptr;
            int i;
        };
    };
    typedef QBspTree::Data QBspTreeData;
    typedef void callback(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);

    QBspTree();

    void create(int n, int d = -1);
    void destroy();

    inline void init(const QRect &area, NodeType type) { init(area, depth, type, 0); }

    void climbTree(const QRect &rect, callback *function, QBspTreeData data);

    inline int leafCount() const { return leaves.count(); }
    inline QVector<int> &leaf(int i) { return leaves[i]; }
    inline void insertLeaf(const QRect &r, int i) { climbTree(r, &insert, i, 0); }
    inline void removeLeaf(const QRect &r, int i) { climbTree(r, &remove, i, 0); }

protected:
    void init(const QRect &area, int depth, NodeType type, int index);
    void climbTree(const QRect &rect, callback *function, QBspTreeData data, int index);

    inline int parentIndex(int i) const { return (i & 1) ? ((i - 1) / 2) : ((i - 2) / 2); }
    inline int firstChildIndex(int i) const { return ((i * 2) + 1); }

    static void insert(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);
    static void remove(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);

private:
    uint depth;
    mutable uint visited;
    QVector<Node> nodes;
    mutable QVector< QVector<int> > leaves; // the leaves are just indices into the items
};

QT_END_NAMESPACE

#endif // QBSPTREE_P_H
