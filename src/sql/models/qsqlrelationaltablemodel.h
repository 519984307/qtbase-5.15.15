/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQLRELATIONALTABLEMODEL_H
#define QSQLRELATIONALTABLEMODEL_H

#include <QtSql/qtsqlglobal.h>
#include <QtSql/qsqltablemodel.h>

#include <QtCore/qtypeinfo.h>

QT_REQUIRE_CONFIG(sqlmodel);

QT_BEGIN_NAMESPACE


class Q_SQL_EXPORT QSqlRelation
{
public:
    QSqlRelation() {}
    QSqlRelation(const QString &aTableName, const QString &indexCol,
               const QString &displayCol)
        : tName(aTableName), iColumn(indexCol), dColumn(displayCol) {}

    void swap(QSqlRelation &other) noexcept
    {
        qSwap(tName, other.tName);
        qSwap(iColumn, other.iColumn);
        qSwap(dColumn, other.dColumn);
    }

    inline QString tableName() const
    { return tName; }
    inline QString indexColumn() const
    { return iColumn; }
    inline QString displayColumn() const
    { return dColumn; }
    bool isValid() const noexcept
    { return !(tName.isEmpty() || iColumn.isEmpty() || dColumn.isEmpty()); }
private:
    QString tName, iColumn, dColumn;
};
Q_DECLARE_SHARED_NOT_MOVABLE_UNTIL_QT6(QSqlRelation)

class QSqlRelationalTableModelPrivate;

class Q_SQL_EXPORT QSqlRelationalTableModel: public QSqlTableModel
{
    Q_OBJECT

public:
    enum JoinMode {
        InnerJoin,
        LeftJoin
    };

    explicit QSqlRelationalTableModel(QObject *parent = nullptr,
                                      QSqlDatabase db = QSqlDatabase());
    virtual ~QSqlRelationalTableModel();

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    void clear() override;
    bool select() override;

    void setTable(const QString &tableName) override;
    virtual void setRelation(int column, const QSqlRelation &relation);
    QSqlRelation relation(int column) const;
    virtual QSqlTableModel *relationModel(int column) const;
    void setJoinMode( QSqlRelationalTableModel::JoinMode joinMode );

public Q_SLOTS:
    void revertRow(int row) override;

protected:
    QString selectStatement() const override;
    bool updateRowInTable(int row, const QSqlRecord &values) override;
    bool insertRowIntoTable(const QSqlRecord &values) override;
    QString orderByClause() const override;

private:
    Q_DECLARE_PRIVATE(QSqlRelationalTableModel)
};

QT_END_NAMESPACE

#endif // QSQLRELATIONALTABLEMODEL_H
