#include <QMap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "http.h"

#include "iaaa.h"

IAAA::IAAA()
    : username{}
    , password{}
    , token{}
    , login_status{false}
    , busy{false}
{
    return;
}

IAAA::IAAA(const QString &username, const QString &password, bool autologin)
    : username{username}
    , password{password}
    , token{}
    , login_status{false}
    , busy{false}
{
    if (autologin)
        login();
    return;
}

void IAAA::login(bool block) {
    QMap<QString, QString> params{
        {"appid", "portal2017"},
        {"userName", username},
        {"password", password},
        {"randCode", ""},
        {"smsCode", ""},
        {"otpCode", ""},
        {"redirUrl", "https://portal.pku.edu.cn/portal2017/ssoLogin.do"}
    };
    client.setMaxRetries(2);
    client.setReadTimeout(2);
    client.setRequestHeaders({{}, {}});
    client.post(QUrl("https://iaaa.pku.edu.cn:443/iaaa/oauthlogin.do"), params);
}
