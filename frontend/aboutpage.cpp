#include "aboutpage.h"
#include "ui_aboutpage.h"

AboutPage *AboutPage::the_only_instance = nullptr;

AboutPage::AboutPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPage)
{
    ui->setupUi(this);
}

AboutPage::~AboutPage()
{
    delete ui;
    the_only_instance = nullptr;
}

void AboutPage::on_AboutOkButton_clicked()
{
    this->close();
}

AboutPage *AboutPage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new AboutPage(parent);
    return the_only_instance;
}

