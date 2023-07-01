#include "pkuportal.h"
#include "QRandomGenerator"

PKUPortal::PKUPortal(QObject *parent)
    : QObject{parent}
{
    qnam.setCookieJar(new QNetworkCookieJar);
    qnam.setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);
}

PKUPortal::PKUPortal(IAAA &iaaa, QObject *parent)
    : PKUPortal{parent}
{
    login(iaaa);
}

void PKUPortal::login(IAAA &iaaa)
{
    if (!iaaa.is_available()) {
        emit finished(false, "IAAA not logged in");
        return;
    }
    QString token{iaaa.token};
    iaaa.token = QString();
    QUrl portal_url{"https://portal.pku.edu.cn/portal2017/ssoLogin.do"};
    QString rand = QString::asprintf("%.15lf", QRandomGenerator::global()->generateDouble());
    portal_url.setQuery(IAAA::urlencode({{"_rand", rand}, {"token", token}}));
    QNetworkRequest myreq{portal_url};
    myreq.setRawHeader("User-Agent", user_agent);
    myreq.setRawHeader("Host", "portal.pku.edu.cn");
    connect(&qnam, &QNetworkAccessManager::finished, this, &PKUPortal::network_finished);
    qnam.get(myreq);
}

void PKUPortal::network_finished(QNetworkReply *response)
{
    if (!response->isFinished())
        return;
    if (response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302)
        return;
    if (response->error() != response->NoError) {
        emit finished(false, response->errorString());
    }
    else {
        logged_in = true;
        emit finished(true, QString());
    }
    response->deleteLater();
    return;
}
