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

#ifndef QSQLDRIVER_H
#define QSQLDRIVER_H

#include <QtSql/qtsqlglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE


class QSqlDatabase;
class QSqlDriverPrivate;
class QSqlError;
class QSqlField;
class QSqlIndex;
class QSqlRecord;
class QSqlResult;
class QVariant;

class Q_SQL_EXPORT QSqlDriver : public QObject
{
    friend class QSqlDatabase;
    friend class QSqlResultPrivate;
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSqlDriver)

public:
    enum DriverFeature { Transactions, QuerySize, BLOB, Unicode, PreparedQueries,
                         NamedPlaceholders, PositionalPlaceholders, LastInsertId,
                         BatchOperations, SimpleLocking, LowPrecisionNumbers,
                         EventNotifications, FinishQuery, MultipleResultSets, CancelQuery };

    enum StatementType { WhereStatement, SelectStatement, UpdateStatement,
                         InsertStatement, DeleteStatement };

    enum IdentifierType { FieldName, TableName };

    enum NotificationSource { UnknownSource, SelfSource, OtherSource };

    enum DbmsType {
        UnknownDbms,
        MSSqlServer,
        MySqlServer,
        PostgreSQL,
        Oracle,
        Sybase,
        SQLite,
        Interbase,
        DB2
    };

    explicit QSqlDriver(QObject *parent = nullptr);
    ~QSqlDriver();
    virtual bool isOpen() const;
    bool isOpenError() const;

    virtual bool beginTransaction();
    virtual bool commitTransaction();
    virtual bool rollbackTransaction();
    virtual QStringList tables(QSql::TableType tableType) const;
    virtual QSqlIndex primaryIndex(const QString &tableName) const;
    virtual QSqlRecord record(const QString &tableName) const;
    virtual QString formatValue(const QSqlField& field, bool trimStrings = false) const;

    virtual QString escapeIdentifier(const QString &identifier, IdentifierType type) const;
    virtual QString sqlStatement(StatementType type, const QString &tableName,
                                 const QSqlRecord &rec, bool preparedStatement) const;

    QSqlError lastError() const;

    virtual QVariant handle() const;
    virtual bool hasFeature(DriverFeature f) const = 0;
    virtual void close() = 0;
    virtual QSqlResult *createResult() const = 0;

    virtual bool open(const QString& db,
                      const QString& user = QString(),
                      const QString& password = QString(),
                      const QString& host = QString(),
                      int port = -1,
                      const QString& connOpts = QString()) = 0;
    virtual bool subscribeToNotification(const QString &name);
    virtual bool unsubscribeFromNotification(const QString &name);
    virtual QStringList subscribedToNotifications() const;

    virtual bool isIdentifierEscaped(const QString &identifier, IdentifierType type) const;
    virtual QString stripDelimiters(const QString &identifier, IdentifierType type) const;

    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;

    DbmsType dbmsType() const;

public Q_SLOTS:
    virtual bool cancelQuery();

Q_SIGNALS:
#if QT_DEPRECATED_SINCE(5, 15)
    QT_DEPRECATED_X("Use the 3-args version of notification() instead.")
    void notification(const QString &name);
#endif
    void notification(const QString &name, QSqlDriver::NotificationSource source, const QVariant &payload);

protected:
    QSqlDriver(QSqlDriverPrivate &dd, QObject *parent = nullptr);
    virtual void setOpen(bool o);
    virtual void setOpenError(bool e);
    virtual void setLastError(const QSqlError& e);


private:
    Q_DISABLE_COPY(QSqlDriver)
};

QT_END_NAMESPACE

#endif // QSQLDRIVER_H
