#ifndef GRADEQUERYPAGE_H
#define GRADEQUERYPAGE_H

#include <QDialog>

namespace Ui {
class GradeQueryPage;
}

class GradeQueryPage : public QDialog
{
    Q_OBJECT
    explicit GradeQueryPage(QWidget *parent);
    static GradeQueryPage *the_only_instance;
public:
    ~GradeQueryPage();
    static GradeQueryPage *get(QWidget *parent = nullptr);

private slots:
    void on_QueryGradeButton_clicked();

private:
    Ui::GradeQueryPage *ui;
};

#endif // GRADEQUERYPAGE_H
