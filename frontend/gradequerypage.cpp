#include "gradequerypage.h"
#include "ui_gradequerypage.h"

GradeQueryPage::GradeQueryPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GradeQueryPage)
{
    ui->setupUi(this);
}

GradeQueryPage::~GradeQueryPage()
{
    delete ui;
}
