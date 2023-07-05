#include "gradequerypage.h"
#include "ui_gradequerypage.h"
#include "backend/CourseTable.hpp"
#include "backend/scoresheet.h"
#include "backend/pkuportal.h"
#include <QMessageBox>
#include <QTimer>

GradeQueryPage *GradeQueryPage::the_only_instance = nullptr;

GradeQueryPage::GradeQueryPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GradeQueryPage)
{
    ui->setupUi(this);
    ui->GradeTable->hide();
    ui->AverageGpa->hide();
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

void GradeQueryPage::on_QueryGradeButton_clicked()
{
    ScoreSheet scores;
    bool finished{false};
    bool query_success;
    QString query_reason;
    connect(&scores, &ScoreSheet::finished, this, [&](bool success, QString reason){
        finished = true;
        query_success = success;
        query_reason = reason;
    });
    scores.online_get(PKUPortal::get_instance());
    //_sleep(1000);
    if (!query_success) {
        QMessageBox::critical(this, "Error", query_reason);
        return;
    }
    /* all data in this part are not well initialized.
     * please finish this process later.
    */
    ui->AverageGpa->setText(QString::asprintf("综合GPA：%.3lf", scores.get_gpa()));
    int row = 0;
    for(const auto &[sems, lists] : scores.get_gradelist_const().toStdMap()) {
        for (const CourseEntry &grade : lists) {
            ui->GradeTable->insertRow(row);
            ui->GradeTable->setItem(row, 2, new QTableWidgetItem(grade.grade));
            ui->GradeTable->setItem(row, 0, new QTableWidgetItem(grade.course_name));
            ui->GradeTable->setItem(row, 1, new QTableWidgetItem(QString::number(grade.credit)));
            row++;
        }
    }
    ui->GradeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->GradeTable->show();
    ui->AverageGpa->show();
}

