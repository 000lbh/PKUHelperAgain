#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <QMap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "3rd/httplib/httplib.h"
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

void WebManager::set_username(const QString &username)
{
    this->username = username;
    return;
}

void WebManager::set_password(const QString &password)
{
    this->password = password;
    return;
}

void WebManager::set_otpCode(const QString &otpCode)
{
    this->otpCode = otpCode;
    return;
}

void WebManager::run()
{
    QMap<QString, QString> params{
        {"appid", "portal2017"},
        {"userName", username},
        {"password", password},
        {"randCode", ""},
        {"smsCode", ""},
        {"otpCode", ""},
        {"redirUrl", "https://portal.pku.edu.cn/portal2017/ssoLogin.do"}
    };
    httplib::Client client("iaaa.pku.edu.cn", 443);
    return;
}

void IAAA::login(bool block)
{
    web_manager.set_username(username);
    web_manager.set_password(password);
    web_manager.start();
    if (block)
        web_manager.wait();
}
