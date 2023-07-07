#include "mainwindow.h"
#include "logindialog.h"
#include "ui_mainwindow.h"
#include "aboutpage.h"
#include "coursequerypage.h"
#include "coursemanagepage.h"
#include "gradequerypage.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, &MainWindow::logged_in, GradeQueryPage::get(this), &GradeQueryPage::logged_in);
    connect(this, &MainWindow::logged_in, CourseQueryPage::get(this), &CourseQueryPage::logged_in);
    connect(this, &MainWindow::logged_in, CourseManagePage::get(this), &CourseManagePage::logged_in);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SettingsButton_clicked() // Settings & About
{
    AboutPage* about = AboutPage::get(this);
    about->show();
    about->setFocus();
}


void MainWindow::on_NoClearReplyButton_clicked()
{
    QMessageBox qiangjibox;
    qiangjibox.setIcon(QMessageBox::Warning);
    qiangjibox.setText("暂时不能给你明确的答复。每个人承担自己的风险！");
    qiangjibox.exec();
}


void MainWindow::on_CourseQueryButton_clicked()
{
    CourseQueryPage* courseqry = CourseQueryPage::get(this);
    courseqry->show();
    courseqry->setFocus();
}


void MainWindow::on_GradeQueryButton_clicked()
{
    GradeQueryPage* gradeqry = GradeQueryPage::get(this);
    gradeqry->show();
    gradeqry->setFocus();
}


void MainWindow::on_CourseManageButton_clicked()
{
    CourseManagePage* courseman = CourseManagePage::get(this);
    courseman->show();
    courseman->setFocus();
}


void MainWindow::on_loginLabel_clicked()
{
    LoginDialog dialog(this);
    int result;
    QString old_username = IAAA::get_instance().get_username();
    if ((result = dialog.exec())) {
        ui->loginLabel->setText("Logged in as " + IAAA::get_instance().get_username() + (result == 1 ? "(online)" : "(offline)"));
        emit logged_in(IAAA::get_instance().get_username(), old_username, result == 1);
    }
    return;
}

