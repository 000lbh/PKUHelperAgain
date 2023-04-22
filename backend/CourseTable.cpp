#include "CourseTable.hpp"

#include <QNetworkRequest>

CourseTable::CourseTable() {
    return;
}

void CourseTable::online_get(const QueryData &req) {
    // Sanity check, you can move it to the front-end
    auto list = req.xndxq.split("-");
    if (list.length() != 3)
        // We need a basic exception here
        throw "error";
    if (list[0].toInt() < 13 || list[0].toInt() > 30)
        throw "error";
    if (list[1].toInt() - list[0].toInt() != 1)
        throw "error";
    switch (list[2].toInt()) {
    case 1: // Fall through
    case 2: // Fall through
    case 3: // Fall through
        break;
    default:
        throw "error";
    }

    for (auto i : query_course_type) {
        if (req.course_type == i)
            goto course_type_fine;
    }
    throw "error";
course_type_fine:
    // End sanity check

    // Get data online from
    // http://www.dean.pku.edu.cn/service/web/courseSearch_do.php
    // method: POST
    QNetworkRequest myreq;
    return;
}
