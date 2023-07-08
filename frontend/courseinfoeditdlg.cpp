#include "courseinfoeditdlg.h"
#include "ui_courseinfoeditdlg.h"

CourseInfoEditDlg::CourseInfoEditDlg(CourseEntry &entry, QWidget *parent) :
    QDialog(parent),
    entry(entry),
    ui(new Ui::CourseInfoEditDlg)
{
    ui->setupUi(this);
    ui->nameEdit->setText(entry.course_name);
    ui->classNoEdit->setText(QString::number(entry.class_no));
    ui->engNameEdit->setText(entry.eng_name);
    ui->locationEdit->setText(entry.location);
    ui->creditEdit->setText(QString::asprintf("%.1lf", entry.credit));
    for (auto &[name, level] : entry.teachers) {
        if (level == QString{})
            ui->teacherListWidget->addItem(name);
        else
            ui->teacherListWidget->addItem(QString{"%s(%s)"}.arg(name, level));
    }
}

CourseInfoEditDlg::~CourseInfoEditDlg()
{
    delete ui;
}

void CourseInfoEditDlg::accept()
{
    entry.location = ui->locationEdit->text();
    entry.eng_name = ui->engNameEdit->text();
    done(1);
}

void CourseInfoEditDlg::reject()
{
    done(0);
}
