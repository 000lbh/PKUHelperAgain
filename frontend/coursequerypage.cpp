#include "coursequerypage.h"
#include "ui_coursequerypage.h"

CourseQueryPage * CourseQueryPage::the_only_instance = nullptr;

CourseQueryPage::CourseQueryPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseQueryPage)
{
    ui->setupUi(this);
}

CourseQueryPage::~CourseQueryPage()
{
    delete ui;
    the_only_instance = nullptr;
}

CourseQueryPage *CourseQueryPage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new CourseQueryPage(parent);
    return the_only_instance;
}
