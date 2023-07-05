#include <QMessageBox>
#include <QDataStream>
#include <QByteArray>

#include "unifieddatabase.h"

UnifiedDatabase::UnifiedDatabase(QObject *parent)
    : QObject{parent}
    , CourseTableDB{QSqlDatabase::addDatabase("QSQLITE", "ctdb")}
{
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

QList<CourseEntry> UnifiedDatabase::ct_query(const QString &sems, const QString &key, const QString &value)
{
    QSqlQuery myqry{CourseTableDB};
    if (value == QString{}) {
        myqry.prepare("SELECT * FROM ?");
        myqry.addBindValue(sems);
    }
    else {
        myqry.prepare("SELECT * FROM ? WHERE ? LIKE ?");
        myqry.addBindValue(sems);
        myqry.addBindValue(key);
        myqry.addBindValue("%" + value + "%");
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

void UnifiedDatabase::ct_reset(const QString &sems, const QList<CourseEntry> &courses)
{
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
    CourseTableDB.exec(QString{"DELETE FROM %1"}.arg(sems));
    QSqlQuery myqry(CourseTableDB);
    for (const auto &i : courses) {
        QByteArray time;
        QByteArray teachers;
        QDataStream timedata(time);
        QDataStream teachersdata(teachers);
        timedata << i.time;
        teachersdata << i.teachers;
        myqry.prepare("INSERT INTO ? VALUES (?,?,?,?,?,?,?,?,?,?,?);");
        myqry.addBindValue(sems);
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
    }
}
