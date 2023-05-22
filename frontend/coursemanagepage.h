#ifndef COURSEMANAGEPAGE_H
#define COURSEMANAGEPAGE_H

#include <QDialog>

namespace Ui {
class CourseManagePage;
}

class CourseManagePage : public QDialog
{
    Q_OBJECT
    explicit CourseManagePage(QWidget *parent);
    static CourseManagePage *the_only_instance;
public:
    ~CourseManagePage();
    static CourseManagePage *get(QWidget *parent = nullptr);

private:
    Ui::CourseManagePage *ui;
};

#endif // COURSEMANAGEPAGE_H
