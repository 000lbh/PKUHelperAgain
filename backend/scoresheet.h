#ifndef SCORESHEET_H
#define SCORESHEET_H

#include <QObject>

#include "CourseTable.hpp"
#include "pkuportal.h"

class ScoreSheet : public QObject
{
    Q_OBJECT
private:
    QList<std::pair<CourseEntry, QString>> _gradelist;
    PKUPortal *internalportal;
    double gpa;
public:
    explicit ScoreSheet(QObject *parent = nullptr);
    QList<QString> get_available_sems() const;
    void online_get(PKUPortal &portal);
    double get_gpa() const;
    //QList<std::pair<QString, double>> get_gpas() const;
    void merge(const ScoreSheet &other);
    void diff(QList<QString> *added, QList<QString> *deleted, const ScoreSheet &other) const;

private slots:
    void network_finished(QNetworkReply *response);

signals:
    void finished(bool success, QString reason);

};

#endif // SCORESHEET_H
