#ifndef SCORESHEET_H
#define SCORESHEET_H

#include <QObject>

class ScoreSheet : public QObject
{
    Q_OBJECT
public:
    explicit ScoreSheet(QObject *parent = nullptr);

signals:

};

#endif // SCORESHEET_H
