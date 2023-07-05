
#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

#include <QObject>


class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();
};

#endif // CLICKABLELABEL_H
