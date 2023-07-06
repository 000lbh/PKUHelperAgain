#ifndef IAAA_H
#define IAAA_H

#include <QObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class IAAA : public QObject
{
    Q_OBJECT
private:
    QMap<QString, QString> params;
    QNetworkAccessManager qnam;
    QString token;
    friend class PKUPortal;
public:
    explicit IAAA(QObject *parent = nullptr);
    explicit IAAA(const QString &username,
                  const QString &password,
                  const QString &otpCode = QString(),
                  bool autologin = false,
                  QObject *parent = nullptr);
    void set_username(const QString &username);
    void set_password(const QString &password);
    void set_otpCode(const QString &otpCode);
    void set_smsCode(const QString &smsCode);
    QString get_token();
    QString get_username();
    bool has_password();
    bool is_available();

    void login();

    static QByteArray urlencode(const QMap<QString, QString> &params);

    // Unused API
    void set_app(const QString &appid, const QString &redirUrl);

    static IAAA &get_instance();

private slots:
    void network_finished(QNetworkReply *response);

signals:
    void finished(bool success, QString reason);

};

#endif // IAAA_H
