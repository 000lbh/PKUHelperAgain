#include "coursemanagepage.h"
#include "ui_coursemanagepage.h"

CourseManagePage *CourseManagePage::the_only_instance = nullptr;

CourseManagePage::CourseManagePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseManagePage)
{
    ui->setupUi(this);
}

CourseManagePage::~CourseManagePage()
{
    delete ui;
    the_only_instance = nullptr;
}

CourseManagePage *CourseManagePage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new CourseManagePage(parent);
    return the_only_instance;
}
