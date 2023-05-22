#ifndef COURSEQUERYPAGE_H
#define COURSEQUERYPAGE_H

#include <QDialog>

namespace Ui {
class CourseQueryPage;
}

class CourseQueryPage : public QDialog
{
    Q_OBJECT
    explicit CourseQueryPage(QWidget *parent);
    static CourseQueryPage *the_only_instance;
public:
    ~CourseQueryPage();
    static CourseQueryPage *get(QWidget *parent = nullptr);

private:
    Ui::CourseQueryPage *ui;
};

#endif // COURSEQUERYPAGE_H
