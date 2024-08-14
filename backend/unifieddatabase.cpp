#include <QMessageBox>
#include <QDataStream>
#include <QByteArray>
#include <QSqlError>
#include <QFile>
#include <QDir>

#include "unifieddatabase.h"

UnifiedDatabase::UnifiedDatabase(QObject *parent)
    : QObject{parent}
    , CourseTableDB{QSqlDatabase::addDatabase("QSQLITE", "ctdb")}
{
    CourseTableDB.setDatabaseName("ctdb.db");
    //CourseTableDB.setConnectOptions("");
    if (!CourseTableDB.open()) {
        QMessageBox::warning(nullptr, "Fatal", "Cannot initialize database, application terminating...");
        throw "failed";
    }
}

UnifiedDatabase::~UnifiedDatabase()
{
    // Destroy and clean resources...
    return;
}

UnifiedDatabase &UnifiedDatabase::getInstance()
{
    static UnifiedDatabase the_only_instance;
    return the_only_instance;
}

QList<CourseEntry> UnifiedDatabase::ct_query(QString sems, const QueryData &request, QString *errmsg)
{
    sems.replace("-", "_");
    sems.push_front("s");
    QSqlQuery myqry{CourseTableDB};
    myqry.prepare("SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = ?");
    myqry.addBindValue(sems);
    myqry.exec();
    if (!(myqry.next() && myqry.value(0).toInt())) {
        if (errmsg)
            *errmsg = "Please cache first!";
        return {};
    }
    if (request.id != QString{}) {
        myqry.prepare("SELECT * FROM " + sems + " WHERE id = ?;");
        myqry.addBindValue(request.id);
    }
    else {
        myqry.prepare("SELECT * FROM " + sems + " WHERE remarks LIKE ? AND teachers LIKE ? AND course_name LIKE ?;");
        myqry.addBindValue("%" + request.remark + "%");
        myqry.addBindValue("%" + request.teacher_name + "%");
        myqry.addBindValue("%" + request.course_name + "%");
    }
    if (!myqry.exec()) {
        if (errmsg)
            *errmsg = myqry.lastError().text();
        return {};
    }
    QList<CourseEntry> result{};
    while (myqry.next()) {
        CourseEntry tmp;
        QByteArray teachers;
        QDataStream teachersdata(&teachers, QIODeviceBase::ReadOnly);
        QByteArray time;
        QDataStream timedata(&time, QIODeviceBase::ReadOnly);
        tmp.id = myqry.value(0).toString();
        tmp.course_name = myqry.value(1).toString();
        tmp.eng_name = myqry.value(2).toString();
        tmp.type_name = myqry.value(3).toString();
        tmp.college_name = myqry.value(4).toString();
        tmp.class_no = myqry.value(5).toInt();
        tmp.credit = myqry.value(6).toDouble();
        tmp.execute_plan_id = myqry.value(7).toString();
        time = myqry.value(8).toByteArray();
        timedata >> tmp.times;
        tmp.time = myqry.value(9).toString();
        teachers = myqry.value(10).toByteArray();
        teachersdata >> tmp.teachers;
        tmp.remarks = myqry.value(11).toString();
        result.push_back(std::move(tmp));
    }
    return result;
}

void UnifiedDatabase::ct_reset(QString sems, const QList<CourseEntry> &courses)
{
    sems.replace("-", "_");
    sems.push_front("s");
    CourseTableDB.exec(QString{"DROP TABLE %1"}.arg(sems));
    CourseTableDB.exec(QString{"CREATE TABLE %1 ("
                               "id              TEXT,"
                               "course_name     TEXT,"
                               "eng_name        TEXT,"
                               "type_name       TEXT,"
                               "college_name    TEXT,"
                               "class_no        INTEGER,"
                               "credit          REAL,"
                               "execute_plan_id TEXT,"
                               "time            BLOB,"
                               "time_str        TEXT,"
                               "teachers        BLOB,"
                               "remarks         TEXT)"
                               ";"}.arg(sems));
    qDebug() << CourseTableDB.lastError().text();
    QSqlQuery myqry(CourseTableDB);
    CourseTableDB.transaction();
    for (const auto &i : courses) {
        QByteArray time;
        QByteArray teachers;
        QDataStream timedata(&time, QIODeviceBase::WriteOnly);
        QDataStream teachersdata(&teachers, QIODeviceBase::WriteOnly);
        timedata << i.times;
        teachersdata << i.teachers;
        myqry.prepare(QString{"INSERT INTO %1 VALUES (?,?,?,?,?,?,?,?,?,?,?,?);"}.arg(sems));
        myqry.addBindValue(i.id);
        myqry.addBindValue(i.course_name);
        myqry.addBindValue(i.eng_name);
        myqry.addBindValue(i.type_name);
        myqry.addBindValue(i.college_name);
        myqry.addBindValue(i.class_no);
        myqry.addBindValue(i.credit);
        myqry.addBindValue(i.execute_plan_id);
        myqry.addBindValue(time);
        myqry.addBindValue(i.time);
        myqry.addBindValue(teachers);
        myqry.addBindValue(i.remarks);
        if (!myqry.exec())
            qDebug() << myqry.lastError().text();
    }
    CourseTableDB.commit();
}

QMap<QString, QList<CourseEntry>> UnifiedDatabase::ss_get(const QString &username, double *gpa, QString *errmsg)
{
    QDir ss_dir{username};
    if (!ss_dir.exists())
        ss_dir.mkpath(".");
    QFile ss_file{username + "/score.dat"};
    if (!ss_file.open(QFile::ReadOnly | QFile::ExistingOnly)) {
        if (errmsg)
            *errmsg = ss_file.errorString();
        *gpa = 0.;
        return {};
    }
    QDataStream ss_stream{&ss_file};
    QMap<QString, QList<CourseEntry>> result;
    ss_stream >> result;
    if (gpa)
        ss_stream >> *gpa;
    ss_file.close();
    return result;
}

void UnifiedDatabase::ss_reset(const QString &username, const QMap<QString, QList<CourseEntry>> &courses, double gpa, QString *errmsg)
{
    QDir ss_dir{username};
    if (!ss_dir.exists())
        ss_dir.mkpath(".");
    QFile ss_file{username + "/score.dat"};
    if (!ss_file.open(QFile::WriteOnly | QFile::Truncate)) {
        if (errmsg)
            *errmsg = ss_file.errorString();
        return;
    }
    QDataStream ss_stream{&ss_file};
    ss_stream << courses << gpa;
    ss_file.close();
    return;
}

QMap<QString, QList<CourseEntry>> UnifiedDatabase::pc_get(const QString &username, QString *errmsg)
{
    QDir pc_dir{username};
    if (!pc_dir.exists())
        pc_dir.mkpath(".");
    QFile pc_file{username + "/course.dat"};
    if (!pc_file.open(QFile::ReadOnly | QFile::ExistingOnly)) {
        if (errmsg)
            *errmsg = pc_file.errorString();
        return {};
    }
    QDataStream pc_stream{&pc_file};
    QMap<QString, QList<CourseEntry>> result;
    pc_stream >> result;
    pc_file.close();
    return result;
}

void UnifiedDatabase::pc_reset(const QString &username, const QMap<QString, QList<CourseEntry>> &data, QString *errmsg)
{
    QDir pc_dir{username};
    if (!pc_dir.exists())
        pc_dir.mkpath(".");
    QFile pc_file{username + "/course.dat"};
    if (!pc_file.open(QFile::WriteOnly | QFile::Truncate)) {
        if (errmsg)
            *errmsg = pc_file.errorString();
        return;
    }
    QDataStream pc_stream{&pc_file};
    pc_stream << data;
    pc_file.close();
    return;
}

QMap<QDate, QList<Arrangement>> UnifiedDatabase::ddl_get(const QString &username, QString *errmsg)
{
    QDir ddl_dir{username};
    if (!ddl_dir.exists())
        ddl_dir.mkpath(".");
    QFile ddl_file{username + "/ddl.dat"};
    if (!ddl_file.open(QFile::ReadOnly | QFile::ExistingOnly)) {
        if (errmsg)
            *errmsg = ddl_file.errorString();
        return {};
    }
    QDataStream ddl_stream{&ddl_file};
    QMap<QDate, QList<Arrangement>> result;
    ddl_stream >> result;
    ddl_file.close();
    return result;
}

void UnifiedDatabase::ddl_reset(const QString &username, const QMap<QDate, QList<Arrangement>> &data, QString *errmsg)
{
    QDir ddl_dir{username};
    if (!ddl_dir.exists())
        ddl_dir.mkpath(".");
    QFile ddl_file{username + "/ddl.dat"};
    if (!ddl_file.open(QFile::WriteOnly | QFile::Truncate)) {
        if (errmsg)
            *errmsg = ddl_file.errorString();
        return;
    }
    QDataStream ddl_stream{&ddl_file};
    ddl_stream << data;
    ddl_file.close();
    return;
}
