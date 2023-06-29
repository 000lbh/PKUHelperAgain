
#ifndef IAAA_H
#define IAAA_H


#include <QObject>
#include <QString>
#include <QNetworkAccessManager>


class IAAA : QObject
{
    Q_OBJECT
private:
    QString username;
    QString password;
    QString token;
    bool login_status;
    bool busy;
    QNetworkAccessManager qnam;
public:
    IAAA();
    explicit IAAA(const QString &username, const QString &password, bool autologin = false);
    void setUsername(const QString &username);
    void setPassword(const QString &pasword);
    void login(bool block = false);
    QString getToken();
signals:
    void finish(bool success, QString reason);
};

#endif // IAAA_H
