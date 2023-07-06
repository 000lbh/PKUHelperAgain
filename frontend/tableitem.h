
#ifndef TABLEITEM_H
#define TABLEITEM_H

#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDesktopServices>

#include "backend/CourseTable.hpp"
#include "backend/iaaa.h"
#include "coursequerypage.h"
#include "gradequerypage.h"

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
    explicit EleTableItem(const QString &text, const CourseEntry &entry)
        : QTableWidgetItem(text), course_info{entry}
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
        else
            QMessageBox::critical(CourseQueryPage::get(), "Error", "Not implemented yet!");
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

#endif // TABLEITEM_H
