#include "coursemanagepage.h"
#include "ui_coursemanagepage.h"
#include "items.h"
#include "backend/unifieddatabase.h"

#include <qdialog.h>
#include <QMessageBox>
#include <QTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>

CourseManagePage *CourseManagePage::the_only_instance = nullptr;
QMap<QDate, QList<Arrangement>> CourseManagePage::calendar{};
QMap<QString, QList<CourseEntry>> CourseManagePage::courses{};

CourseManagePage::CourseManagePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseManagePage)
{
    ui->setupUi(this);
    connect(ui->semsBox, &QComboBox::currentIndexChanged, this, &CourseManagePage::updateCourses);
}

CourseManagePage::~CourseManagePage()
{
    delete ui;
    the_only_instance = nullptr;
    auto username = IAAA::get_instance().get_username();
    auto &dbInstance = UnifiedDatabase::getInstance();
    if (IAAA::get_instance().get_username() != QString{}) {
        dbInstance.pc_reset(username, courses);
        dbInstance.ddl_reset(username, calendar);
    }
}

CourseManagePage *CourseManagePage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new CourseManagePage(parent);
    return the_only_instance;
}

void CourseManagePage::logged_in(QString username, QString oldname, bool online)
{
    auto &dbInstance = UnifiedDatabase::getInstance();
    if (oldname != QString{}) {
        dbInstance.pc_reset(oldname, courses);
        dbInstance.ddl_reset(oldname, calendar);
    }
    courses = dbInstance.pc_get(username);
    calendar = dbInstance.ddl_get(username);
    return;
}

void CourseManagePage::updateCourses()
{
    while (ui->courseListWidget->count()) {
        delete ui->courseListWidget->takeItem(0);
    }
    for (auto &i : courses[ui->semsBox->currentText()]) {
        ui->courseListWidget->addItem(new CourseListItem(i));
    }
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
        QList<Arrangement> arrangements = calendar[date];
        std::ranges::sort(arrangements, [](const Arrangement& a1, const Arrangement& a2) {
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


void CourseManagePage::on_removeButton_clicked()
{
    if (!ui->courseListWidget->count())
        return;
    int row = ui->courseListWidget->currentRow();
    courses[ui->semsBox->currentText()].remove(row);
    updateCourses();
}


void CourseManagePage::on_courseListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    dynamic_cast<CourseListItem *>(item)->exec();
    return;
}

