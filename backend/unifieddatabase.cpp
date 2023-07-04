
#include "unifieddatabase.h"

UnifiedDatabase::UnifiedDatabase(QObject *parent)
    : QObject{parent}
    , CourseTableDB{QSqlDatabase::addDatabase("QSQLITE")}
{
    CourseTableDB.setDatabaseName(":memory:");
    if (!CourseTableDB.open())
        throw "failed";
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

