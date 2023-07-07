#ifndef COURSEQUERYPAGE_H
#define COURSEQUERYPAGE_H

#include <QDialog>
#include <QTableWidgetItem>

#include "backend/CourseTable.hpp"

namespace Ui {
class CourseQueryPage;
}

class CourseQueryPage : public QDialog
{
    Q_OBJECT
    explicit CourseQueryPage(QWidget *parent);
    static CourseQueryPage *the_only_instance;
    CourseTable courses;
public:
    ~CourseQueryPage();
    static CourseQueryPage *get(QWidget *parent = nullptr);

public slots:
    void logged_in(QString username, QString oldname, bool online);

private slots:
    void on_updateButton_clicked();
    void on_queryButton_clicked();

    void on_courseTable_itemDoubleClicked(QTableWidgetItem *item);

    void on_courseTable_itemClicked(QTableWidgetItem *item);

public slots:
    void get_course_finished_succ();
    void get_course_finished_fail(QString reason);
    void update_progressbar(int total, int current);

private:
    Ui::CourseQueryPage *ui;
};

#endif // COURSEQUERYPAGE_H
