#include <QString>
#include <QMap>
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonDocument>

#include "iaaa.h"
#include "backend/CourseTable.hpp"

IAAA::IAAA(QObject *parent)
    : QObject{parent}
{

}

IAAA::IAAA(const QString &username,
           const QString &password,
           const QString &otpCode,
           bool autologin,
           QObject *parent)
    : QObject{parent}
    , params{
          {"username", username},
          {"password", password},
          {"appid", "portal2017"},
          {"otpCode", otpCode},
          {"smsCode", ""},
          {"randCode", ""},
          {"redirUrl", "https://portal.pku.edu.cn/portal2017/ssoLogin.do"}
      }
{
    if (autologin)
        login();
}

QByteArray IAAA::urlencode(const QMap<QString, QString> &params) {
    QByteArray result;
    for (const auto &[key, value] : params.toStdMap()) {
        result += QUrl::toPercentEncoding(key) + "=" + QUrl::toPercentEncoding(value) + "&";
    }
    result.chop(1);
    return result;
}

void IAAA::login()
{
    QByteArray rawparams = urlencode(params);
    QNetworkRequest myreq(QUrl("https://iaaa.pku.edu.cn/iaaa/oauthlogin.do"));
    myreq.setRawHeader("User-Agent", user_agent);
    myreq.setRawHeader("Host", "iaaa.pku.edu.cn");
    myreq.setRawHeader("Origin", "https://iaaa.pku.edu.cn/iaaa/oauth.jsp");
    myreq.setRawHeader("Referer", "https://iaaa.pku.edu.cn/iaaa/oauth.jsp");
    connect(&qnam, &QNetworkAccessManager::finished, this, &IAAA::network_finished);
    qnam.post(myreq, rawparams);
}

bool IAAA::is_available() {
    return token != "";
}

void IAAA::network_finished(QNetworkReply *response)
{
    if (!response->isReadable())
        return;
    if (response->error() != QNetworkReply::NoError) {
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &IAAA::network_finished);
        emit finished(false, response->errorString());
        response->deleteLater();
        return;
    }
    QJsonParseError jerror;
    QJsonDocument data = QJsonDocument::fromJson(response->readAll(), &jerror);
    response->deleteLater();
    if (jerror.error != jerror.NoError) {
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &IAAA::network_finished);
        emit finished(false, jerror.errorString());
        return;
    }
    if (!data.isObject()) {
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &IAAA::network_finished);
        emit finished(false, "JSON is not an object.");
        return;
    }
    QJsonObject dataobj = data.object();
    if (!data["success"].toBool()) {
        QJsonObject errdata= data["errors"].toObject();
        disconnect(&qnam, &QNetworkAccessManager::finished, this, &IAAA::network_finished);
        emit finished(false, "Err Code: " + errdata["code"].toString() + ", Message: " + errdata["msg"].toString());
        return;
    }
    token = data["token"].toString();
    emit finished(true, QString());
    return;
}
