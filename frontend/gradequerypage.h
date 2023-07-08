#ifndef GRADEQUERYPAGE_H
#define GRADEQUERYPAGE_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QTimer>
#include <QSystemTrayIcon>

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
    QSystemTrayIcon *tray;
public:
    ~GradeQueryPage();
    static GradeQueryPage *get(QWidget *parent = nullptr);
    void setTray(QSystemTrayIcon *tray);

public slots:
    void on_QueryGradeButton_clicked();
    void logged_in(QString username, QString oldname, bool online);

private slots:
    void makeColor();
    void updateDisplay();
    void updateGrade(bool success, QString reason);
    void on_GradeTable_itemDoubleClicked(QTableWidgetItem *item);
    void on_scoreVisBox_clicked();

    void on_refreshBox_stateChanged(int arg1);

    void on_GradeTable_itemClicked(QTableWidgetItem *item);

private:
    Ui::GradeQueryPage *ui;
    enum ColorMethod {no, pf, th, lvl};
    ColorMethod getColorMethod();
};

#endif // GRADEQUERYPAGE_H
