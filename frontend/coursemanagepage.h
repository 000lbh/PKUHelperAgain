#ifndef COURSEMANAGEPAGE_H
#define COURSEMANAGEPAGE_H

#include <QDialog>

#include "backend/CourseTable.hpp"
#include <QListWidgetItem>
#include <QDataStream>

namespace Ui {
class CourseManagePage;
}

struct Arrangement{
    QString title;
    QString content;
    QDateTime datetime;
    Arrangement(QString _title, QString _content, QDateTime _datetime)
        : title(_title), content(_content), datetime(_datetime) {
    }
    Arrangement() = default;

    friend QDataStream &operator<<(QDataStream &stream, const Arrangement &data)
    {
        return stream << data.title << data.content << data.datetime;
    }
    friend QDataStream &operator>>(QDataStream &stream, Arrangement &data)
    {
        return stream >> data.title >> data.content >> data.datetime;
    }
};

class CourseManagePage : public QDialog
{
    Q_OBJECT
    explicit CourseManagePage(QWidget *parent);
    static CourseManagePage *the_only_instance;
    static QMap<QDate, QList<Arrangement>> calendar;
    static QMap<QString, QList<CourseEntry>> courses;
    friend class EleTableItem;
public:
    ~CourseManagePage();
    static CourseManagePage *get(QWidget *parent = nullptr);

public slots:
    void logged_in(QString username, QString oldname, bool online);
    void updateCourses();

private slots:
    void on_AddArrangement_clicked();

    void on_Calendar_clicked(const QDate &date);

    void on_removeButton_clicked();

    void on_courseListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::CourseManagePage *ui;
};

#endif // COURSEMANAGEPAGE_H
