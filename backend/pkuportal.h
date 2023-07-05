#ifndef PKUPORTAL_H
#define PKUPORTAL_H

#include <QObject>
#include <QNetworkCookieJar>

#include "CourseTable.hpp"
#include "iaaa.h"

class PKUPortal : public QObject
{
    Q_OBJECT
private:
    bool logged_in;
    QNetworkAccessManager qnam;
    friend class ScoreSheet;
public:
    explicit PKUPortal(QObject *parent = nullptr);
    explicit PKUPortal(IAAA &iaaa, QObject *parent = nullptr);

    void login(IAAA &iaaa);

    static PKUPortal &get_instance();

private slots:
    void network_finished(QNetworkReply *response);

signals:
    void finished(bool success, QString reason);

};

#endif // PKUPORTAL_H
