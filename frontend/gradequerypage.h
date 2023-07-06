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
    void makeColor();

private:
    Ui::GradeQueryPage *ui;
    enum ColorMethod {no, pf, th, lvl};
    ColorMethod getColorMethod();
};

#endif // GRADEQUERYPAGE_H
