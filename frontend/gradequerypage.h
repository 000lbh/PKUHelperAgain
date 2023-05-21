#ifndef GRADEQUERYPAGE_H
#define GRADEQUERYPAGE_H

#include <QDialog>

namespace Ui {
class GradeQueryPage;
}

class GradeQueryPage : public QDialog
{
    Q_OBJECT

public:
    explicit GradeQueryPage(QWidget *parent = nullptr);
    ~GradeQueryPage();

private:
    Ui::GradeQueryPage *ui;
};

#endif // GRADEQUERYPAGE_H
