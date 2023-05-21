#ifndef COURSEQUERYPAGE_H
#define COURSEQUERYPAGE_H

#include <QDialog>

namespace Ui {
class CourseQueryPage;
}

class CourseQueryPage : public QDialog
{
    Q_OBJECT

public:
    explicit CourseQueryPage(QWidget *parent = nullptr);
    ~CourseQueryPage();

private:
    Ui::CourseQueryPage *ui;
};

#endif // COURSEQUERYPAGE_H
