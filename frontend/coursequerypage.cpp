#include <QMessageBox>

#include "coursequerypage.h"
#include "ui_coursequerypage.h"
#include "../backend/unifieddatabase.h"
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

void CourseQueryPage::on_updateButton_clicked()
{
    connect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    connect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    connect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    this->ui->widget->setEnabled(false);
    courses.online_get({"", "", ui->xndxqEdit->text(), "0", "0", 0});
    return;
}

void CourseQueryPage::on_queryButton_clicked()
{
    QString errmsg;
    QueryData request{
        ui->kcmcEdit->text(),
        ui->jsmcEdit->text(),
        ui->xndxqEdit->text(),
        "0",
        "0",
        ui->remarkEdit->text(),
        ui->kchEdit->text(),
        0
    };
    QList<CourseEntry> courses{UnifiedDatabase::getInstance().ct_query(ui->xndxqEdit->text(), request, &errmsg)};
    if (errmsg != QString{}) {
        QMessageBox::critical(this, "Error", errmsg);
        return;
    }
    ui->courseTable->clearContents();
    int row = 0;
    for(const CourseEntry& course : courses) {
        if (ui->courseTable->rowCount() <= row)
            ui->courseTable->insertRow(row);
        ui->courseTable->setItem(row, 0, new QTableWidgetItem(course.id));
        ui->courseTable->setItem(row, 1, new QTableWidgetItem(course.course_name));
        ui->courseTable->setItem(row, 2, new QTableWidgetItem(course.college_name));
        ui->courseTable->setItem(row, 3, new QTableWidgetItem(QString::number(course.credit)));
        row++;
    }
}

void CourseQueryPage::update_progressbar(int total, int current)
{
    this->ui->progressBar->setValue(int(current / double(total) * 100.));
}

QString get_course_time(QVector<CourseTime> ctime){
    if (ctime.empty()) return "无";
    QString week;
    week = "第" + QString::number(ctime[0].week_start) + "周到第" + QString::number(ctime[0].week_stop) + "周 ";
    QString daytime;
    for (const CourseTime& time : ctime) {
        if (time.step == 1) daytime += "每周";
        else if (time.step == 2 && time.week_start % 2 == 0) daytime += "双周";
        else if (time.step == 2 && time.week_start % 2 == 1) daytime += "单周";
        daytime += "周" + QString::number(time.day_in_week);
        daytime += "第" + QString::number(time.time_in_day) + "节";
    }
    return week + daytime;
}

void CourseQueryPage::get_course_finished_succ()
{
    this->ui->progressBar->setValue(100);
    this->ui->widget->setEnabled(true);
    disconnect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    disconnect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    disconnect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    QMessageBox::information(this, "Hint", "Finished");
    QVector<CourseEntry> course_table_query = courses.get_course_table();
    UnifiedDatabase::getInstance().ct_reset(courses.get_semester(),courses.get_course_table());
    ui->courseTable->clearContents();
    int row = 0;
    ui->courseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for(const CourseEntry& course : course_table_query) {
        if (ui->courseTable->rowCount() <= row)
            ui->courseTable->insertRow(row);
        ui->courseTable->setItem(row, 0, new QTableWidgetItem(course.id));
        ui->courseTable->setItem(row, 1, new QTableWidgetItem(course.course_name));
        ui->courseTable->setItem(row, 2, new QTableWidgetItem(course.college_name));
        ui->courseTable->setItem(row, 3, new QTableWidgetItem(QString::number(course.credit)));
        row++;
    }
}

void CourseQueryPage::get_course_finished_fail(QString reason)
{
    this->ui->progressBar->setValue(100);
    this->ui->widget->setEnabled(true);
    disconnect(&courses, &CourseTable::ready, this, &CourseQueryPage::get_course_finished_succ);
    disconnect(&courses, &CourseTable::fail, this, &CourseQueryPage::get_course_finished_fail);
    disconnect(&courses, &CourseTable::progress_update, this, &CourseQueryPage::update_progressbar);
    QMessageBox::information(this, "Error", "Failed:" + reason);
}
