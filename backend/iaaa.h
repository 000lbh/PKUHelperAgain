
#ifndef IAAA_H
#define IAAA_H


#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include "3rd/httplib/httplib.h"


class IAAA : QObject
{
    Q_OBJECT
private:
    QString username;
    QString password;
    QString token;
    bool login_status;
    bool busy;
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
