#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_pressed()
{
    QMessageBox::information(this, "提示", "暂时不能给你明确答复！");
}


void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(2); // TODO: Avoid magic number
}


void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(0); // TODO: Avoid magic number
}

