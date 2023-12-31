/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QSHADERGRAPH_P_H
#define QSHADERGRAPH_P_H

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

#include <QtGui/private/qtguiglobal_p.h>

#include <QtGui/private/qshadernode_p.h>

QT_BEGIN_NAMESPACE

class QShaderGraph
{
public:
    class Edge
    {
    public:
        QStringList layers;
        QUuid sourceNodeUuid;
        QString sourcePortName;
        QUuid targetNodeUuid;
        QString targetPortName;
    };

    class Statement
    {
    public:
        Q_GUI_EXPORT QUuid uuid() const noexcept;
        Q_GUI_EXPORT int portIndex(QShaderNodePort::Direction direction, const QString &portName) const noexcept;

        QShaderNode node;
        QVector<int> inputs;
        QVector<int> outputs;
    };

    Q_GUI_EXPORT void addNode(const QShaderNode &node);
    Q_GUI_EXPORT void removeNode(const QShaderNode &node);
    Q_GUI_EXPORT QVector<QShaderNode> nodes() const noexcept;

    Q_GUI_EXPORT void addEdge(const Edge &edge);
    Q_GUI_EXPORT void removeEdge(const Edge &edge);
    Q_GUI_EXPORT QVector<Edge> edges() const noexcept;

    Q_GUI_EXPORT QVector<Statement> createStatements(const QStringList &enabledLayers = QStringList()) const;

private:
    QVector<QShaderNode> m_nodes;
    QVector<Edge> m_edges;
};

Q_GUI_EXPORT bool operator==(const QShaderGraph::Edge &lhs, const QShaderGraph::Edge &rhs) noexcept;

inline bool operator!=(const QShaderGraph::Edge &lhs, const QShaderGraph::Edge &rhs) noexcept
{
    return !(lhs == rhs);
}

Q_GUI_EXPORT bool operator==(const QShaderGraph::Statement &lhs, const QShaderGraph::Statement &rhs) noexcept;

inline bool operator!=(const QShaderGraph::Statement &lhs, const QShaderGraph::Statement &rhs) noexcept
{
    return !(lhs == rhs);
}

Q_DECLARE_TYPEINFO(QShaderGraph, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QShaderGraph::Edge, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QShaderGraph::Statement, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QShaderGraph)
Q_DECLARE_METATYPE(QShaderGraph::Edge)
Q_DECLARE_METATYPE(QShaderGraph::Statement)

#endif // QSHADERGRAPH_P_H
