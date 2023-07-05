#include <QResizeEvent>
#include <QMessageBox>

#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent, IAAA &iaaa, PKUPortal &pkuportal) :
    QDialog(parent),
    iaaa(iaaa),
    pkuportal(pkuportal),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->usernameEdit->setText(iaaa.get_username());
}

void LoginDialog::resizeEvent(QResizeEvent *event)
{
    auto [x, y] = event->size();
    ui->verticalLayoutWidget->setGeometry(QRect{{x / 20, y / 20}, event->size() * 0.9});
    QDialog::resizeEvent(event);
}

void LoginDialog::accept()
{
    if (ui->usernameEdit->text() == QString{}) {
        done(0);
        return;
    }
    iaaa.set_username(ui->usernameEdit->text());
    if (ui->passwordEdit->text() != QString{})
        iaaa.set_password(ui->passwordEdit->text());
    if (!ui->onlineCheckBox->isChecked()) {
        done(2);
        return;
    }
    connect(&iaaa, &IAAA::finished, this, &LoginDialog::iaaa_finished);
    iaaa.login();
}

void LoginDialog::reject()
{
    done(0);
}

void LoginDialog::iaaa_finished(bool success, QString reason)
{
    disconnect(&iaaa, &IAAA::finished, this, &LoginDialog::iaaa_finished);
    if (!success) {
        QMessageBox::critical(this, "Error", reason);
        done(2);
    }
    else {
        connect(&pkuportal, &PKUPortal::finished, this, &LoginDialog::pkuportal_finished);
        pkuportal.login(iaaa);
    }
}

void LoginDialog::pkuportal_finished(bool success, QString reason)
{
    disconnect(&pkuportal, &PKUPortal::finished, this, &LoginDialog::pkuportal_finished);
    if (!success) {
        QMessageBox::critical(this, "Error", reason);
        done(2);
    }
    else {
        QMessageBox::information(this, "Success", "You have logged in as " + iaaa.get_username());
        done(1);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
