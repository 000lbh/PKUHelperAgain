
#ifndef IAAA_H
#define IAAA_H

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include "3rd/httplib/httplib.h"

class WebManager: public QThread
{
    Q_OBJECT
private:
    QString username;
    QString password;
    QString otpCode;
    QString smsCode;
    QString randCode;
protected:
    void run() override;
public:
    bool success;
    QString reason;
    QString token;
    void set_username(const QString &username);
    void set_password(const QString &password);
    void set_otpCode(const QString &otpCode);
signals:
    void finished(bool success, QString reason);
};

class IAAA : QObject
{
    Q_OBJECT
private:
    QString username;
    QString password;
    QString token;
    bool login_status;
    bool busy;
    WebManager web_manager;

public:
    IAAA();
    explicit IAAA(const QString &username, const QString &password, bool autologin = false);
    void setUsername(const QString &username);
    void setPassword(const QString &pasword);
    void login(bool block = false);
    QString getToken();
private slots:
signals:
    void finished(bool success, QString reason);
};

#endif // IAAA_H
