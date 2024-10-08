#include "gradequerypage.h"
#include "ui_gradequerypage.h"
#include "backend/CourseTable.hpp"
#include "backend/scoresheet.h"
#include "backend/pkuportal.h"
#include "backend/unifieddatabase.h"
#include "items.h"
#include <QMessageBox>
#include <QSettings>

GradeQueryPage *GradeQueryPage::the_only_instance = nullptr;

GradeQueryPage::GradeQueryPage(QWidget *parent) :
    QDialog(parent),
    cur_scores{nullptr},
    next_scores{nullptr},
    timer{this},
    tray{nullptr},
    ui(new Ui::GradeQueryPage)
{
    ui->setupUi(this);
    ui->AverageGpa->hide();
    connect(ui->lvlColorBtn, &QRadioButton::clicked, this, &GradeQueryPage::makeColor);
    connect(ui->noColorBtn, &QRadioButton::clicked, this, &GradeQueryPage::makeColor);
    connect(ui->thColorBtn, &QRadioButton::clicked, this, &GradeQueryPage::makeColor);
    connect(ui->pfColorBtn, &QRadioButton::clicked, this, &GradeQueryPage::makeColor);
    timer.callOnTimeout(this, &GradeQueryPage::on_QueryGradeButton_clicked);
    ui->scoreVisBox->setChecked(QSettings{}.value("showScore", true).toBool());
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
    if (next_scores)
        delete next_scores;
    if (cur_scores) {
        UnifiedDatabase::getInstance().ss_reset(IAAA::get_instance().get_username(), cur_scores->get_gradelist(), cur_scores->get_gpa());
        delete cur_scores;
    }
}

void GradeQueryPage::setTray(QSystemTrayIcon *tray)
{
    this->tray = tray;
}

void GradeQueryPage::logged_in(QString username, QString oldname, bool online)
{
    if (oldname != QString{})
        UnifiedDatabase::getInstance().ss_reset(oldname, cur_scores->get_gradelist(), cur_scores->get_gpa());
    if (!cur_scores)
        cur_scores = new ScoreSheet{};
    cur_scores->get_gradelist() = UnifiedDatabase::getInstance().ss_get(username, &cur_scores->get_gpa());
    updateDisplay();
}

GradeQueryPage::ColorMethod GradeQueryPage::getColorMethod()
{
    if (ui->noColorBtn->isChecked())
        return no;
    if (ui->lvlColorBtn->isChecked())
        return lvl;
    if (ui->pfColorBtn->isChecked())
        return pf;
    if (ui->thColorBtn->isChecked())
        return th;
    return no;
}

void GradeQueryPage::makeColor()
{
    const int max_row = ui->GradeTable->rowCount();
    const int max_col = ui->GradeTable->columnCount();
    ColorMethod method = getColorMethod();
    for (int i = 0; i < max_row; i++) {
        if (method == pf) {
            QString rawgrade = dynamic_cast<GradeTableItem *>(ui->GradeTable->item(i, 4))->getGrade();
            bool ok;
            double grade{rawgrade.toDouble(&ok)};
            bool pass;
            if (ok) {
                pass = grade >= 60.;
            }
            else {
                const static QSet<QString> pass_set{"A+", "A", "A-", "B+", "B", "B-", "C+", "C", "C-", "D+", "D", "EX", "合格", "P"};
                pass = pass_set.find(rawgrade) != pass_set.end();
            }
            for (int j = 0; j < max_col; j++)
                if (pass)
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{40, 255, 40}});
                else
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 40, 40}});
        }
        else if (method == lvl) {
            QString rawgrade = dynamic_cast<GradeTableItem *>(ui->GradeTable->item(i, 4))->getGrade();
            bool ok;
            double grade{rawgrade.toDouble(&ok)};
            int gradelvl;
            if (ok) {
                if (grade >= 85.)
                    gradelvl = 0;
                else if (grade >= 75.)
                    gradelvl = 1;
                else if (grade >= 65.)
                    gradelvl = 2;
                else if (grade >= 60.)
                    gradelvl = 3;
                else
                    gradelvl = 4;
            }
            else {
                const static QSet<QString> a_set{"A+", "A", "A-"};
                const static QSet<QString> b_set{"B+", "B", "B-"};
                const static QSet<QString> c_set{"C+", "C", "C-"};
                const static QSet<QString> d_set{"D+", "D"};
                const static QSet<QString> f_set{"F"};
                const static QSet<QString> no_gpa_set{"合格", "不合格", "P", "NP", "W", "EX", "I", "IP"};
                if (a_set.find(rawgrade) != a_set.end())
                    gradelvl = 0;
                else if (b_set.find(rawgrade) != b_set.end())
                    gradelvl = 1;
                else if (c_set.find(rawgrade) != c_set.end())
                    gradelvl = 2;
                else if (d_set.find(rawgrade) != d_set.end())
                    gradelvl = 3;
                else if (f_set.find(rawgrade) != f_set.end())
                    gradelvl = 4;
                else if (no_gpa_set.find(rawgrade) != no_gpa_set.end())
                    gradelvl = 5;
                else
                    gradelvl = 5;
            }
            for (int j = 0; j < max_col; j++)
                switch (gradelvl) {
                case 0:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{40, 255, 40}});
                    break;
                case 1:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 255, 40}});
                    break;
                case 2:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 137, 40}});
                    break;
                case 3:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 40, 40}});
                    break;
                case 4:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 0, 127}});
                    break;
                default:
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 255, 255}});
                }
        }
        else if (method == th) {
            QString rawgrade = dynamic_cast<GradeTableItem *>(ui->GradeTable->item(i, 4))->getGrade();
            bool ok;
            QColor color{};
            static QImage rainbow{":/rainbow.png", "png"};
            double grade{rawgrade.toDouble(&ok)};
            if (ok) {
                if (grade >= 80.) {
                    color.setRed((100. - grade) / 20 * 255);
                    color.setGreen(255);
                    color.setBlue(0);
                }
                else if (grade >= 60.) {
                    color.setRed(255);
                    color.setGreen(255 - (80. - grade) / 20 * 255);
                    color.setBlue(0);
                }
                else
                    color.setRgb(255, 0, 127);
            }
            else {
                if (rawgrade == "A+" || rawgrade == "A" || rawgrade == "A-")
                    color.setRgb(40, 255, 40);
                else if (rawgrade == "B+" || rawgrade == "B" || rawgrade == "B-")
                    color.setRgb(255, 255, 40);
                else if (rawgrade == "C+" || rawgrade == "C" || rawgrade == "C-")
                    color.setRgb(255, 137, 40);
                else if (rawgrade == "D+" || rawgrade == "D")
                    color.setRgb(255, 40, 40);
                else if (rawgrade == "F" || rawgrade == "NP" || rawgrade == "不合格")
                    color.setRgb(255, 0, 127);
                else
                    color.setRgb(255, 140, 255);
            }
            for (int j = 0; j < max_col; j++) {
                if (ok && grade == 100.)
                    ui->GradeTable->item(i, j)->setBackground(QBrush{rainbow});
                else
                    ui->GradeTable->item(i, j)->setBackground(QBrush{color});
            }
        }
        else if (method == no) {
            for (int j = 0; j < max_col; j++) {
                if (i & 1)
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{255, 255, 255}});
                else
                    ui->GradeTable->item(i, j)->setBackground(QBrush{QColor{224, 224, 224}});
            }
        }
    }
    return;
}

void GradeQueryPage::on_QueryGradeButton_clicked()
{
    if (next_scores)
        delete next_scores;
    next_scores = new ScoreSheet{};
    ui->QueryGradeButton->setEnabled(false);
    connect(next_scores, &ScoreSheet::finished, this, &GradeQueryPage::updateGrade);
    next_scores->online_get(PKUPortal::get_instance());
}

void GradeQueryPage::updateGrade(bool success, QString reason) {
    disconnect(next_scores, &ScoreSheet::finished, this, &GradeQueryPage::updateGrade);
    ui->QueryGradeButton->setEnabled(true);
    if (!success) {
        QMessageBox::critical(this, "Error", reason);
        return;
    }
    if (cur_scores) {
        QList<CourseEntry> add{}, deleted{};
        cur_scores->diff(&add, &deleted, *next_scores);
        if (add.size() != 0 || deleted.size() != 0) {
            if (tray && !isVisible())
                tray->showMessage("成绩更新", QString{"你新增了%1门成绩，撤回了%2门成绩"}.arg(add.size(), deleted.size()));
            else
                QMessageBox::information(this, "Score Update", QString::asprintf("You have %d course(s) added and %d course(s) deleted", add.size(), deleted.size()));
        }
        delete cur_scores;
    }
    cur_scores = next_scores;
    next_scores = nullptr;
    updateDisplay();
}

void GradeQueryPage::updateDisplay() {
    ui->AverageGpa->setText(QString::asprintf("综合GPA：%.3lf", cur_scores->get_gpa()));
    int row = 0;
    for(const auto &[sems, lists] : cur_scores->get_gradelist_const().toStdMap()) {
        for (const CourseEntry &grade : lists) {
            if (ui->GradeTable->rowCount() <= row)
                ui->GradeTable->insertRow(row);
            ui->GradeTable->insertRow(row);
            ui->GradeTable->setItem(row, 0, new QTableWidgetItem(grade.course_name));
            ui->GradeTable->setItem(row, 1, new QTableWidgetItem(grade.eng_name));
            QString teacher{};
            for (auto &[name, level] : grade.teachers) {
                if (level == QString{})
                    teacher += name + ',';
                else
                    teacher += name + '(' + level + ')' + ',';
            }
            teacher.chop(1);
            ui->GradeTable->setItem(row, 2, new QTableWidgetItem(std::move(teacher)));
            ui->GradeTable->setItem(row, 3, new QTableWidgetItem(QString::number(grade.credit)));
            ui->GradeTable->setItem(row, 4, new GradeTableItem(grade.grade, ui->scoreVisBox->isChecked()));
            ui->GradeTable->setItem(row, 5, new RefTableItem("双击查看", "https://elective.pku.edu.cn/elective2008/edu/pku/stu/elective/controller/courseDetail/getCourseDetail.do?kclx=BK&course_seq_no=" + grade.execute_plan_id));
            row++;
        }
    }
    int rrowcnt = ui->GradeTable->rowCount() - row;
    for (int i = 0; i < rrowcnt; i++)
        ui->GradeTable->removeRow(row);
    makeColor();
    ui->GradeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->AverageGpa->show();
}


void GradeQueryPage::on_GradeTable_itemDoubleClicked(QTableWidgetItem *item)
{
    RefTableItem *ref_item = dynamic_cast<RefTableItem *>(item);
    if (ref_item) {
        ref_item->exec();
        return;
    }
    GradeTableItem *grade_item = dynamic_cast<GradeTableItem *>(item);
    if (grade_item) {
        grade_item->exec();
        return;
    }
}


void GradeQueryPage::on_scoreVisBox_clicked()
{
    QSettings{}.setValue("showScore", ui->scoreVisBox->isChecked());
    const int max_row = ui->GradeTable->rowCount();
    for (int i = 0; i < max_row; i++) {
        dynamic_cast<GradeTableItem *>(ui->GradeTable->item(i, 4))->setVisibility(ui->scoreVisBox->isChecked());
    }
    return;
}


void GradeQueryPage::on_refreshBox_stateChanged(int arg1)
{
    if (ui->refreshBox->isChecked()) {
        timer.setInterval(ui->timeEdit->time().msecsSinceStartOfDay());
        timer.start();
    }
    else {
        timer.stop();
    }
}


void GradeQueryPage::on_GradeTable_itemClicked(QTableWidgetItem *item)
{
    ui->infoEdit->setText(item->text());
}

