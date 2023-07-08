#include "mainwindow.h"
#include "logindialog.h"
#include "ui_mainwindow.h"
#include "aboutpage.h"
#include "coursequerypage.h"
#include "coursemanagepage.h"
#include "gradequerypage.h"

#include <QMessageBox>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->mainVerticalWidget);
    connect(this, &MainWindow::logged_in, GradeQueryPage::get(this), &GradeQueryPage::logged_in);
    connect(this, &MainWindow::logged_in, CourseQueryPage::get(this), &CourseQueryPage::logged_in);
    connect(this, &MainWindow::logged_in, CourseManagePage::get(this), &CourseManagePage::logged_in);
    QAction *window_show = new QAction{"显示"};
    connect(window_show, &QAction::triggered, this, &MainWindow::show);
    traymenu.addAction(window_show);
    QAction *app_exit = new QAction{"退出"};
    connect(app_exit, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::quit);
    traymenu.addAction(app_exit);
    tray.setIcon(QIcon{":/favicon.png"});
    tray.setContextMenu(&traymenu);
    if (tray.isSystemTrayAvailable())
        tray.show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !this->isVisible()) {
        QCoreApplication::instance()->quit();
    }
    if (tray.isVisible()) {
        hide();
        event->ignore();
        tray.showMessage("提醒", "软件已经最小化到托盘，右键托盘图标可还原或退出");
    }
    else {
        QCoreApplication::instance()->quit();
    }
}

void MainWindow::on_SettingsButton_clicked()
{
    // TODO
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


void MainWindow::on_aboutLabel_clicked()
{
    AboutPage* about = AboutPage::get(this);
    about->show();
    about->setFocus();
}

