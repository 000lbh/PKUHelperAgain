#include <QMouseEvent>

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::MouseButton::LeftButton)
        emit clicked();
    QLabel::mousePressEvent(event);
}
