#include "coursemanagepage.h"
#include "ui_coursemanagepage.h"

CourseManagePage::CourseManagePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseManagePage)
{
    ui->setupUi(this);
}

CourseManagePage::~CourseManagePage()
{
    delete ui;
}
