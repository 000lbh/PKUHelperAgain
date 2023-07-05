
#ifndef UNIFIEDDATABASE_H
#define UNIFIEDDATABASE_H


#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "CourseTable.hpp"


class UnifiedDatabase : public QObject
{
    Q_OBJECT
    explicit UnifiedDatabase(QObject *parent = nullptr);
    ~UnifiedDatabase();
    QSqlDatabase CourseTableDB;
    QSqlDatabase PersonCourseDB;
    QSqlDatabase ScoreDB;
    QSqlDatabase DdlDB;
public:

    static UnifiedDatabase &getInstance();

    // Course Table DB
    QList<CourseEntry> ct_query(const QString &sems, const QString &key, const QString &value = QString{});
    void ct_reset(const QString &sems, const QList<CourseEntry> &courses);
    void ct_merge();

    // User manipulation

    // User-specific db connect. All below need to execute this first
    void user_connect(const QString &username, const QString &password = QString{});
    void user_new(const QString &username, const QString &password = QString{});
    void user_query(const QString &username);
    // Note that password is to protect user's presonal data from leaking,
    // instead of unauthorized removing. Operate this with caution and
    // confirm, but not authentication.
    void user_delete(const QString &username);

    // Score Sheet DB
    void ss_add();
    void ss_remove();
    void ss_query();
    void ss_reset();
    void ss_merge();
    void ss_diff();

    // Personal Course DB
    void pc_query();
    void pc_reset();
    void pc_merge();
    void pc_diff();
    void pc_add();
    void pc_remove();

    // Personal DDL DB
    void ddl_add();
    void ddl_query();
    void ddl_remove();
    void ddl_reset();
    void ddl_merge();

signals:

};

#endif // UNIFIEDDATABASE_H
