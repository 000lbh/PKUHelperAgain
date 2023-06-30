#include "CourseTable.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>

CourseEntry::CourseEntry(const QJsonObject &entry) {
    id = entry.value("kch").toString();
    class_no = entry.value("jxbh").toInt();
    course_name = entry.value("kcmc").toString();
    type_name = entry.value("kctxm").toString();
    college_name = entry.value("kkxsmc").toString();
    credit = entry.value("xf").toString().toDouble();
    execute_plan_id = entry.value("zxjhbh").toString();
    // TODO: Need XML process such things like "<p>John</p><p>Mike</p>"
    teachers = {{"Guowei", "Lecturer"}};
    // TODO: Need process qzz(start-stop week) and time
}

CourseTable::CourseTable()
    : available(true)
{
    connect(this, &CourseTable::online_get_signal, this, &CourseTable::online_get_slot);
    return;
}

void CourseTable::online_get(const QueryData &req) {
    // availability check
    if (!available) {
        emit fail("Not available");
        return;
    }
    // Sanity check, can be moved it to the front-end
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

    available = false;
    this->temp_req = req;
    _course_data.clear();

    //qnam.connectToHostEncrypted("dean.pku.edu.cn");
    connect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
    emit online_get_signal(req, 0);
    return;
}

void CourseTable::online_get_slot(QueryData req, int start) {
    // Get data online from
    // https://dean.pku.edu.cn/service/web/courseSearch.php
    // method: POST
    QNetworkRequest myreq(QUrl("https://dean.pku.edu.cn/service/web/courseSearch_do.php"));
    myreq.setRawHeader("User-Agent", user_agent);
    myreq.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    myreq.setRawHeader("Connect", "keep-alive");
    myreq.setRawHeader("Host", "dean.pku.edu.cn");
    myreq.setRawHeader("Origin", "https://dean.pku.edu.cn");
    myreq.setRawHeader("Referer", "https://dean.pku.edu.cn/service/web/courseSearch.php");

    // make post payload
    const QString fmt = "coursename=%1&teachername=%2&yearandseme=%3&coursetype=%4&yuanxi=%5&startrow=%6";
    QString payload = fmt.arg(req.course_name).arg(req.teacher_name).arg(req.xndxq).arg(req.course_type).arg(req.college).arg(start);

    // Send data
    qnam.post(myreq, QByteArray(payload.toStdString().c_str()));
    return;
}

void CourseTable::qnam_request_finished(QNetworkReply *response) {
    if (!response->isReadable())
        return;
    // Response status OK?
    if (response->error() != QNetworkReply::NoError) {
        available = true;
        emit fail(response->errorString());
        response->deleteLater();
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        return;
    }

    // Json decode ok?
    QJsonParseError jsonerror;
    QJsonDocument data = QJsonDocument::fromJson(response->readAll(), &jsonerror);
    response->deleteLater();
    if (jsonerror.error != jsonerror.NoError) {
        available = true;
        emit fail(jsonerror.errorString());
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        return;
    }
    if (!data.isObject() || data.isNull())
        emit fail("Response data is null or not json object");
    QJsonObject dataobj = data.object();
    if (dataobj["status"].toString() != "ok" ) {
        available = true;
        emit fail("Response data not OK: " + dataobj["status"].toString());
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        return;
    }

    // Json decode;
    int total = dataobj["count"].toString().toInt();
    QJsonValue courselist = dataobj["courselist"];
    if (!courselist.isArray()) {
        available = true;
        emit fail("Cannot find course info");
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        return;

    }
    QJsonArray courselist2 = courselist.toArray();
    for (const auto &i : courselist2) {
        _course_data.push_back(CourseEntry(i.toObject()));
    }
    if (_course_data.size() >= total) {
        available = true;
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        emit ready();
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &CourseTable::qnam_request_finished);
        return;
    }
    emit progress_update(total, _course_data.size());
    emit online_get_signal(temp_req, _course_data.size());
    return;
}
