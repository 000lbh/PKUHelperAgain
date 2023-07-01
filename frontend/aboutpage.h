#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QDialog>

namespace Ui {
class AboutPage;
}

class AboutPage : public QDialog
{
    Q_OBJECT
    explicit AboutPage(QWidget *parent);
    static AboutPage *the_only_instance;
public:
    ~AboutPage();
    static AboutPage *get(QWidget *parent = nullptr);

private slots:
    void on_AboutOkButton_clicked();

private:
    Ui::AboutPage *ui;
};

#endif // ABOUTPAGE_H
