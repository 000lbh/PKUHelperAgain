
#ifndef TABLEITEM_H
#define TABLEITEM_H

#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDesktopServices>

#include "backend/CourseTable.hpp"
#include "backend/iaaa.h"
#include "coursequerypage.h"

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
    }
};

#endif // TABLEITEM_H
