#include <QMessageBox>

#include "coursequerypage.h"
#include "ui_coursequerypage.h"
#include "../backend/CourseTable.hpp"

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

void CourseQueryPage::on_pushButton_clicked()
{
    connect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    connect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    connect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    courses.online_get({"", "", ui->xndxqEdit->text(), "0", "0", 0});
    return;
}

void CourseQueryPage::update_progressbar(int total, int current)
{
    this->ui->progressBar->setValue(int(current / double(total) * 100.));
}

void CourseQueryPage::get_course_finished_succ()
{
    this->ui->progressBar->setValue(100);
    disconnect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    disconnect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    disconnect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    QMessageBox::information(this, "Hint", "Finished");
}

void CourseQueryPage::get_course_finished_fail(QString reason)
{
    disconnect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    disconnect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    disconnect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    QMessageBox::information(this, "Error", "Failed:" + reason);
}
