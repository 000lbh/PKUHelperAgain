#include "coursemanagepage.h"
#include "ui_coursemanagepage.h"

#include <qdialog.h>
#include <QMessageBox>
#include <QTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

class Arrangement{
public:
    QString title;
    QString content;
    QDateTime datetime;
    Arrangement(QString _title, QString _content, QDateTime _datetime)
        : title(_title), content(_content), datetime(_datetime) {
    }
};

std::map<QDate,std::vector<Arrangement> > calendar;
// TODO: Use QSettings to save it globally
CourseManagePage *CourseManagePage::the_only_instance = nullptr;

CourseManagePage::CourseManagePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseManagePage)
{
    ui->setupUi(this);
}

CourseManagePage::~CourseManagePage()
{
    delete ui;
    the_only_instance = nullptr;
}

CourseManagePage *CourseManagePage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new CourseManagePage(parent);
    return the_only_instance;
}

void CourseManagePage::on_AddArrangement_clicked()
{
    QDialog* dialog = new QDialog(this);
    QVBoxLayout* layout = new QVBoxLayout(dialog);

    QDateTimeEdit* dateTimeEdit = new QDateTimeEdit(dialog);
    QLineEdit* titleLineEdit = new QLineEdit(dialog);
    QTextEdit* contentTextEdit = new QTextEdit(dialog);
    QPushButton* confirmButton = new QPushButton("确定", dialog);

    layout->addWidget(new QLabel("事件日期和时间:", dialog));
    layout->addWidget(dateTimeEdit);
    layout->addWidget(new QLabel("事件标题:", dialog));
    layout->addWidget(titleLineEdit);
    layout->addWidget(new QLabel("内容:", dialog));
    layout->addWidget(contentTextEdit);
    layout->addWidget(confirmButton);

    dialog->setLayout(layout);

    connect(confirmButton, &QPushButton::clicked, dialog, &QDialog::accept);

    if (dialog->exec() == QDialog::Accepted) {
        QDateTime selectedDateTime = dateTimeEdit->dateTime();
        QString title = titleLineEdit->text();
        QString content = contentTextEdit->toPlainText();

        Arrangement arrangement(title, content, selectedDateTime);
        calendar[selectedDateTime.date()].push_back(arrangement);
        QTextCharFormat format;
        format.setForeground(Qt::blue);
        ui->Calendar->setDateTextFormat(selectedDateTime.date(), format);
    }

    delete dialog;
}


void CourseManagePage::on_Calendar_clicked(const QDate &date)
{
    if (calendar.find(date) == calendar.end()) {
        QMessageBox::information(this, "提示", "当天没有日程");
    }
    else {
        std::vector<Arrangement> arrangements = calendar[date];
        std::sort(arrangements.begin(), arrangements.end(), [](const Arrangement& a1, const Arrangement& a2) {
        return a1.datetime < a2.datetime;
        });

        QString content;
        for (const Arrangement& arrangement : arrangements) {
            QString time = arrangement.datetime.time().toString("hh:mm");
            QString title = arrangement.title;
            QString arrangementContent = arrangement.content;

            content += time + " " + title + " " + arrangementContent + "\n";
        }

        QMessageBox::information(this, "当天日程", content);
    }
}

