
#ifndef UNIFIEDDATABASE_H
#define UNIFIEDDATABASE_H


#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "CourseTable.hpp"
#include "frontend/coursemanagepage.h"


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
    QMap<QString, QList<CourseEntry>> ss_get(const QString &username, double *gpa = nullptr, QString *errmsg = nullptr);
    void ss_reset(const QString &username, const QMap<QString, QList<CourseEntry>> &courses, double gpa = 0., QString *errmsg = nullptr);

    // Personal Course DB
    QMap<QString, QList<CourseEntry>> pc_get(const QString &username, QString *errmsg = nullptr);
    void pc_reset(const QString &username, const QMap<QString, QList<CourseEntry>> &data, QString *errmsg = nullptr);

    // Personal DDL DB
    QMap<QDate, QList<Arrangement>> ddl_get(const QString &username, QString *errmsg = nullptr);
    void ddl_reset(const QString &username, const QMap<QDate, QList<Arrangement>> &data, QString *errmsg = nullptr);

signals:

};

#endif // UNIFIEDDATABASE_H
