#ifndef COURSEMANAGEPAGE_H
#define COURSEMANAGEPAGE_H

#include <QDialog>

namespace Ui {
class CourseManagePage;
}

class CourseManagePage : public QDialog
{
    Q_OBJECT

public:
    explicit CourseManagePage(QWidget *parent = nullptr);
    ~CourseManagePage();

private:
    Ui::CourseManagePage *ui;
};

#endif // COURSEMANAGEPAGE_H
