#include <QMessageBox>
#include <QDataStream>
#include <QByteArray>
#include <QSqlError>

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
        QDataStream teachersdata(teachers);
        QByteArray time;
        QDataStream timedata(time);
        tmp.id = myqry.value(0).toString();
        tmp.course_name = myqry.value(1).toString();
        tmp.eng_name = myqry.value(2).toString();
        tmp.type_name = myqry.value(3).toString();
        tmp.college_name = myqry.value(4).toString();
        tmp.class_no = myqry.value(5).toInt();
        tmp.credit = myqry.value(6).toDouble();
        tmp.execute_plan_id = myqry.value(7).toString();
        time = myqry.value(8).toByteArray();
        timedata >> tmp.time;
        teachers = myqry.value(9).toByteArray();
        teachersdata >> tmp.teachers;
        tmp.remarks = myqry.value(10).toString();
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
                               "teachers        BLOB,"
                               "remarks         TEXT)"
                               ";"}.arg(sems));
    qDebug() << CourseTableDB.lastError().text();
    QSqlQuery myqry(CourseTableDB);
    for (const auto &i : courses) {
        QByteArray time;
        QByteArray teachers;
        QDataStream timedata(&time, QDataStream::WriteOnly);
        QDataStream teachersdata(&teachers, QDataStream::WriteOnly);
        timedata << i.time;
        teachersdata << i.teachers;
        myqry.prepare(QString{"INSERT INTO %1 VALUES (?,?,?,?,?,?,?,?,?,?,?);"}.arg(sems));
        myqry.addBindValue(i.id);
        myqry.addBindValue(i.course_name);
        myqry.addBindValue(i.eng_name);
        myqry.addBindValue(i.type_name);
        myqry.addBindValue(i.college_name);
        myqry.addBindValue(i.class_no);
        myqry.addBindValue(i.credit);
        myqry.addBindValue(i.execute_plan_id);
        myqry.addBindValue(time);
        myqry.addBindValue(teachers);
        myqry.addBindValue(i.remarks);
        if (!myqry.exec())
            qDebug() << myqry.lastError().text();
    }
}
