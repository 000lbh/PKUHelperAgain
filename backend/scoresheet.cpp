#include "scoresheet.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ScoreSheet::ScoreSheet(QObject *parent)
    : QObject{parent}
    , internalportal{nullptr}
{

}

void ScoreSheet::online_get(PKUPortal &portal)
{
    if (!internalportal) {
        emit finished(false, "Cannot re-entry");
        return;
    }
    internalportal = &portal;
    QNetworkRequest myreq{QUrl{"https://portal.pku.edu.cn/portal2017/bizcenter/score/retrScores.do"}};
    if (!portal.logged_in) {
        emit finished(false, "Not logged in");
        return;
    }
    connect(&portal.qnam, &QNetworkAccessManager::finished, this, &ScoreSheet::network_finished);
    portal.qnam.post(myreq, QByteArray{});
}

double ScoreSheet::get_gpa() const
{
    return gpa;
}

QList<QString> ScoreSheet::get_available_sems() const
{
    return _gradelist.keys();
}

void ScoreSheet::merge(const ScoreSheet &other)
{
    for (const QString &sems : other._gradelist.keys()) {
        for (const auto &i : other._gradelist[sems]) {
            bool have_the_same = false;
            for (const auto &j : _gradelist[sems]) {
                if (i == j) {
                    have_the_same = true;
                    break;
                }
            }
            if (!have_the_same)
                _gradelist[sems].push_back(i);
        }
    }
}

// new courses pushed to added list,
// deleted courses pushed to deleted list.
void ScoreSheet::diff(QList<CourseEntry> *added, QList<CourseEntry> *deleted, const ScoreSheet &other) const
{
    QSet<QString> matched;
    for (const QString &sems : other._gradelist.keys()) {
        for (const auto &i : other._gradelist[sems]) {
            bool have_the_same = false;
            for (const auto &j : _gradelist[sems]) {
                if (i == j) {
                    have_the_same = true;
                    matched.insert(j.execute_plan_id);
                }
            }
            if (!have_the_same)
                added->push_back(i);
        }
    }
    for (const auto &i : _gradelist.values()) {
        for (const auto &j : i) {
            if (matched.constFind(j.execute_plan_id) != matched.cend())
                deleted->push_back(j);
        }
    }
}

void ScoreSheet::network_finished(QNetworkReply *response)
{
    disconnect(&internalportal->qnam, &QNetworkAccessManager::finished, this, &ScoreSheet::network_finished);
    if (response->error() != response->NoError) {
        if (response->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 401)
            internalportal->logged_in = false; // session expired
        bool logged_in = internalportal->logged_in;
        internalportal = nullptr;
        emit finished(false, logged_in ? response->errorString() : QString{"Session expired"});
        response->deleteLater();
        return;
    }
    internalportal = nullptr;
    QJsonParseError jerror;
    QJsonDocument data = QJsonDocument::fromJson(response->readAll(), &jerror);
    response->deleteLater();
    if (jerror.error != jerror.NoError) {
        emit finished(false, response->errorString());
        return;
    }
    if (data.isNull() || !data.isObject()) {
        emit finished(false, "Json is not an object");
        return;
    }
    QJsonObject dataobj = data.object();
    if (!data["success"].toBool()) {
        emit finished(false, "Success status is false");
        return;
    }
    // Score adding...
    QJsonArray scores = dataobj["cjxx"].toArray();
    for (const auto &i : scores) {
        QJsonObject iobj = i.toObject();
        QJsonArray listobj = iobj["list"].toArray();
        for (const auto &j : listobj) {
            _gradelist[iobj["xnd"].toString() + "-" + iobj["xq"].toString()].push_back(CourseEntry(j.toObject()));
        }
    }
    gpa = dataobj["gpa"].toString().toDouble();
}
