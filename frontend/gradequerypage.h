#ifndef GRADEQUERYPAGE_H
#define GRADEQUERYPAGE_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QTimer>

#include "backend/scoresheet.h"

namespace Ui {
class GradeQueryPage;
}

class GradeQueryPage : public QDialog
{
    Q_OBJECT
    explicit GradeQueryPage(QWidget *parent);
    static GradeQueryPage *the_only_instance;
    ScoreSheet *cur_scores, *next_scores;
    QTimer timer;
public:
    ~GradeQueryPage();
    static GradeQueryPage *get(QWidget *parent = nullptr);

public slots:
    void on_QueryGradeButton_clicked();

private slots:
    void makeColor();
    void updateGrade(bool success, QString reason);
    void on_GradeTable_itemDoubleClicked(QTableWidgetItem *item);
    void on_scoreVisBox_clicked();

    void on_refreshBox_stateChanged(int arg1);

private:
    Ui::GradeQueryPage *ui;
    enum ColorMethod {no, pf, th, lvl};
    ColorMethod getColorMethod();
};

#endif // GRADEQUERYPAGE_H
