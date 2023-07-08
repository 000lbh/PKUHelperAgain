#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    bool directClose = QSettings{}.value("directClose", false).toBool();
    ui->checkBox->setChecked(directClose);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_clicked()
{
    constexpr const char loc[]{"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"};
    QSettings autorun(loc, QSettings::NativeFormat);
    QFileInfo curFile{QApplication::applicationFilePath()};
    QString keyName{curFile.baseName()};
    if (autorun.value(keyName).toString() == QString{}) {
        autorun.setValue(keyName, QDir::toNativeSeparators(QApplication::applicationFilePath()) + " --hide");
        QMessageBox::information(this, "提示", "成功设置开机启动");
    }
    else {
        autorun.remove(keyName);
        QMessageBox::information(this, "提示", "成功取消设置开机启动");
    }
}


void SettingsDialog::on_pushButton_2_clicked()
{
    QMessageBox::warning(this, "提示", "功能尚未实现");
}

void SettingsDialog::accept()
{
    QSettings{}.setValue("directClose", ui->checkBox->isChecked());
    done(1);
}

