#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_SettingsButton_clicked();

    void on_NoClearReplyButton_clicked();

    void on_CourseQueryButton_clicked();

    void on_GradeQueryButton_clicked();

    void on_CourseManageButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
