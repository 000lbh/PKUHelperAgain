#ifndef SCORESHEET_H
#define SCORESHEET_H

#include <QObject>

#include "CourseTable.hpp"
#include "pkuportal.h"

class ScoreSheet : public QObject
{
    Q_OBJECT
private:
    QMap<QString, QList<CourseEntry>> _gradelist;
    PKUPortal *internalportal;
    double gpa;
public:
    explicit ScoreSheet(QObject *parent = nullptr);
    QList<QString> get_available_sems() const;
    void online_get(PKUPortal &portal);
    const double &get_gpa() const;
    double &get_gpa();
    double get_gpa_const() const;
    //QList<std::pair<QString, double>> get_gpas() const;
    void merge(const ScoreSheet &other);
    void diff(QList<CourseEntry> *added, QList<CourseEntry> *deleted, const ScoreSheet &other) const;
    QMap<QString, QList<CourseEntry>> &get_gradelist();
    const QMap<QString, QList<CourseEntry>> &get_gradelist() const;
    const QMap<QString, QList<CourseEntry>> &get_gradelist_const() const;

private slots:
    void network_finished(QNetworkReply *response);

signals:
    void finished(bool success, QString reason);

};

#endif // SCORESHEET_H
