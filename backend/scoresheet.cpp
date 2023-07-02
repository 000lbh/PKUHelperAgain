#include "scoresheet.h"

#include <QJsonDocument>
#include <QJsonObject>

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
}
