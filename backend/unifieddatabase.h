
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
    QSqlDatabase DdlDB;
public:

    static UnifiedDatabase &getInstance();

    // Course Table DB
    QList<CourseEntry> ct_query(QString sems, const QueryData &request, QString *errmsg = nullptr);
    void ct_reset(QString sems, const QList<CourseEntry> &courses);
    void ct_merge();

    // Score Sheet DB
    QMap<QString, QList<CourseEntry>> ss_get();
    void ss_reset(const QMap<QString, QList<CourseEntry>> &courses);

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
