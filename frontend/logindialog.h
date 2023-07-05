#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "backend/iaaa.h"
#include "backend/pkuportal.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

private:
    IAAA &iaaa;
    PKUPortal &pkuportal;

public:
    explicit LoginDialog(QWidget *parent = nullptr, IAAA &iaaa = IAAA::get_instance(), PKUPortal &pkuportal = PKUPortal::get_instance());
    ~LoginDialog();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void accept();
    void reject();
    void iaaa_finished(bool success, QString reason);
    void pkuportal_finished(bool success, QString reason);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
