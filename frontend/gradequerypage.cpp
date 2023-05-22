#include "gradequerypage.h"
#include "ui_gradequerypage.h"

GradeQueryPage *GradeQueryPage::the_only_instance = nullptr;

GradeQueryPage::GradeQueryPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GradeQueryPage)
{
    ui->setupUi(this);
}

GradeQueryPage *GradeQueryPage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new GradeQueryPage(parent);
    return the_only_instance;
}

GradeQueryPage::~GradeQueryPage()
{
    delete ui;
    the_only_instance = nullptr;
}
