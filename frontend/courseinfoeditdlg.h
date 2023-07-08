#ifndef COURSEINFOEDITDLG_H
#define COURSEINFOEDITDLG_H

#include <QDialog>

#include "backend/CourseTable.hpp"

namespace Ui {
class CourseInfoEditDlg;
}

class CourseInfoEditDlg : public QDialog
{
    Q_OBJECT

    CourseEntry &entry;
public:
    explicit CourseInfoEditDlg(CourseEntry &entry, QWidget *parent = nullptr);
    ~CourseInfoEditDlg();

private:
    Ui::CourseInfoEditDlg *ui;

private slots:
    void accept();
    void reject();
};

#endif // COURSEINFOEDITDLG_H
