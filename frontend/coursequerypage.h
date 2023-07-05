#ifndef COURSEQUERYPAGE_H
#define COURSEQUERYPAGE_H

#include <QDialog>

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

private slots:
    void on_pushButton_clicked();

public slots:
    void get_course_finished_succ();
    void get_course_finished_fail(QString reason);
    void update_progressbar(int total, int current);

private:
    Ui::CourseQueryPage *ui;
};

#endif // COURSEQUERYPAGE_H
