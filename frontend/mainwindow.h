#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void on_SettingsButton_clicked();

    void on_NoClearReplyButton_clicked();

    void on_CourseQueryButton_clicked();

    void on_GradeQueryButton_clicked();

    void on_CourseManageButton_clicked();

    void on_loginLabel_clicked();

    void on_aboutLabel_clicked();

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon tray;
    QMenu traymenu;

signals:
    void logged_in(QString username, QString oldname, bool online);
};
#endif // MAINWINDOW_H
