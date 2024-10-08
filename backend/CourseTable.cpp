#include "CourseTable.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <QRegularExpression>

QDataStream &operator<<(QDataStream &stream, const CourseTime &time)
{
    return stream << time.time_in_day << time.classroom << time.day_in_week << time.week_start << time.week_stop << time.step;
}

QDataStream &operator>>(QDataStream &stream, CourseTime &time)
{
    return stream >> time.time_in_day >> time.classroom >> time.day_in_week >> time.week_start >> time.week_stop >> time.step;
}

CourseEntry::CourseEntry(const QJsonObject &entry, JsonSource source) {
    if (source == Dean) {
        id = entry.value("kch").toString();
        class_no = entry.value("jxbh").toString().toInt();
        course_name = entry.value("kcmc").toString();
        type_name = entry.value("kctxm").toString();
        college_name = entry.value("kkxsmc").toString();
        credit = entry.value("xf").toString().toDouble();
        execute_plan_id = entry.value("zxjhbh").toString();
        // teacher info process
        QString rawteachers = entry.value("teacher").toString();
        while (rawteachers.contains("<p>")) {
            auto i = rawteachers.indexOf("<p>");
            rawteachers.remove(0, i + 3);
            i = rawteachers.indexOf("</p>");
            teachers.push_back({rawteachers.sliced(0, i), ""});
            rawteachers.remove(0, i + 4);
        }
        QString raw_start_end_week = entry.value("qzz").toString();
        QString raw_day_in_week = entry.value("sksj").toString();

        time += "第" + raw_start_end_week.section('-', 0, 0) + "周到第" + raw_start_end_week.section('-', 1, 1) + "周:";

        // 使用正则表达式提取每天的上课时间
        static const QRegularExpression regex("<p>(.*?)\\(第(\\d+)节-第(\\d+)节\\)</p>");
        QRegularExpressionMatchIterator it = regex.globalMatch(raw_day_in_week);
        int dot_cnt = 0; // for processing ","
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            if (match.hasMatch()) {
                if (dot_cnt) time += ",";
                QString day = match.captured(1);
                QString start = match.captured(2);
                QString end = match.captured(3);
                time += day + "(第" + start + "节-第" + end + "节)";
                dot_cnt ++;
            }
        }

        remarks = entry.value("bz").toString();
    }
    else if (source == Portal) {
        id = entry["kch"].toString();
        class_no = entry["jxbh"].toString().toInt();
        course_name = entry["kcmc"].toString();
        eng_name = entry["ywmc"].toString();
        type_name = entry["kctx"].toString();
        credit = entry["xf"].toString().toDouble();
        execute_plan_id = entry["zxjhbh"].toString();
        for (const auto &i : entry["skjsxm"].toString().split(",")) {
            auto &&tmp = i.split("$");
            if (tmp.length() < 3)
                continue;
            teachers.push_back({tmp[0].split("-")[1], tmp[2]});
        }
        grade = entry["xqcj"].toString();
        bool gp_ok;
        grade_point = entry["jd"].toString().toDouble(&gp_ok);
        if (!gp_ok)
            grade_point = NAN;
    }
}

bool CourseEntry::is_same(const CourseEntry &other) const {
    return id == other.id;
}

bool CourseEntry::operator==(const CourseEntry &other) const {
    return execute_plan_id == other.execute_plan_id;
}

QDataStream &operator<<(QDataStream &stream, const CourseEntry &entry)
{
     return stream <<
            entry.id <<
            entry.course_name <<
            entry.eng_name <<
            entry.type_name <<
            entry.college_name <<
            entry.class_no <<
            entry.credit <<
            entry.execute_plan_id <<
            entry.time <<
            entry.times <<
            entry.teachers <<
            entry.remarks <<
            entry.grade <<
            entry.grade_point <<
            entry.location;
}

QDataStream &operator>>(QDataStream &stream, CourseEntry &entry)
{
     return stream >>
            entry.id >>
            entry.course_name >>
            entry.eng_name >>
            entry.type_name >>
            entry.college_name >>
            entry.class_no >>
            entry.credit >>
            entry.execute_plan_id >>
            entry.time >>
            entry.times >>
            entry.teachers >>
            entry.remarks >>
            entry.grade >>
            entry.grade_point >>
            entry.location;
}

CourseTable::CourseTable(QObject *parent)
    : QObject{parent}
    , available{true}
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
    case 1:
        [[fallthrough]];
    case 2:
        [[fallthrough]];
    case 3:
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
