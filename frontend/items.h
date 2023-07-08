
#ifndef ITEMS_H
#define ITEMS_H

#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDesktopServices>

#include "backend/CourseTable.hpp"
#include "backend/iaaa.h"
#include "coursequerypage.h"
#include "gradequerypage.h"
#include "coursemanagepage.h"
#include "courseinfoeditdlg.h"

class RefTableItem final : public QTableWidgetItem
{
    QString href;
public:
    explicit RefTableItem(const QString &text, const QString &href)
        : QTableWidgetItem(text), href{href}
    {
        QFont font = this->font();
        font.setUnderline(true);
        this->setFont(font);
        this->setTextAlignment(Qt::AlignCenter);
        return;
    }
    void exec() {
        QDesktopServices::openUrl(QUrl(href));
        return;
    }
};

class EleTableItem final : public QTableWidgetItem
{
    CourseEntry course_info;
public:
    template <typename T>
    explicit EleTableItem(const QString &text, T &&entry)
        : QTableWidgetItem(text), course_info{std::forward<T>(entry)}
    {
        QFont font = this->font();
        font.setUnderline(true);
        this->setFont(font);
        this->setTextAlignment(Qt::AlignCenter);
        return;
    }
    void exec() {
        if (IAAA::get_instance().get_username() == QString{})
            QMessageBox::critical(CourseQueryPage::get(), "Error", "Please login (can be offline) first!");
        else {
            CourseManagePage::courses[course_info.sems].push_back(course_info);
            QMessageBox::information(CourseQueryPage::get(), "成功", "添加成功，请检查");
            CourseManagePage::get()->updateCourses();
        }
        return;
    }
};

class GradeTableItem final : public QTableWidgetItem
{
    QString grade;
    bool visibility;
public:
    explicit GradeTableItem(const QString &grade, bool visibility = true) {
        this->grade = grade;
        this->visibility = visibility;
        if (!visibility)
            this->setText("***");
        else
            this->setText(grade);
        return;
    }
    void exec() {
        if (!visibility)
            QMessageBox::information(GradeQueryPage::get(), "Grade", "你的成绩是:" + grade);
        return;
    }
    QString getGrade() {
        return grade;
    }
    void setVisibility(bool visibility) {
        this->visibility = visibility;
        if (visibility)
            this->setText(grade);
        else
            this->setText("***");
    }
};

class CourseListItem : public QListWidgetItem
{
    CourseEntry &entry;
public:
    explicit CourseListItem(CourseEntry &entry) : entry(entry) {
        this->setText(entry.course_name);
    }
    void exec() {
        CourseInfoEditDlg editdlg{entry, CourseManagePage::get()};
        editdlg.exec();
        return;
    }
};

#endif // ITEMS_H
