#include "gradequerypage.h"
#include "ui_gradequerypage.h"

class GradeOfCourse{
public:
    double gpa;
    QString name;
    int credit;
    int semester;
    GradeOfCourse(double _gpa, QString _name, int _credit, int _semester = 0) : gpa(_gpa),name(_name),credit(_credit),semester(_semester) {}
};

GradeQueryPage *GradeQueryPage::the_only_instance = nullptr;

GradeQueryPage::GradeQueryPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GradeQueryPage)
{
    ui->setupUi(this);
    ui->GradeTable->hide();
    ui->AverageGpa->hide();
}

GradeQueryPage *GradeQueryPage::get(QWidget *parent) {
    if (!the_only_instance)
        the_only_instance = new GradeQueryPage(parent);
    return the_only_instance;
}

GradeQueryPage::~GradeQueryPage()
{
    delete ui;
    the_only_instance = nullptr;
}

void GradeQueryPage::on_QueryGradeButton_clicked()
{
    std::vector<GradeOfCourse> gradetable;
    /* all data in this part are not well initialized.
     * please finish this process later.
    */

    gradetable.push_back(GradeOfCourse(3.0,"程序设计实习",3,3));
    gradetable.push_back(GradeOfCourse(4.0,"睡眠导论",3,2));
    gradetable.push_back(GradeOfCourse(3.5,"吃饭概论",3,2));
    gradetable.push_back(GradeOfCourse(1.0,"Qt实践",3,4));
    gradetable.push_back(GradeOfCourse(2.0,"内存泄漏学",3,4));

    std::sort(gradetable.begin(), gradetable.end(), [](const GradeOfCourse& obj1, const GradeOfCourse& obj2) {
        return obj1.semester < obj2.semester;
    });
    double sumgpa = 0.0;
    for (const auto& obj : gradetable) {
        sumgpa += obj.gpa;
    }
    double average_gpa = sumgpa / gradetable.size();
    QString gpa = QString::number(average_gpa, 'f', 3);
    ui->AverageGpa->setText("综合GPA：" + gpa);
    int row = 0;
    for(const GradeOfCourse& grade : gradetable) {
        ui->GradeTable->insertRow(row);
        ui->GradeTable->setItem(row, 2, new QTableWidgetItem(QString::number(grade.gpa, 'f', 3)));
        ui->GradeTable->setItem(row, 0, new QTableWidgetItem(grade.name));
        ui->GradeTable->setItem(row, 1, new QTableWidgetItem(QString::number(grade.credit)));
        row++;
    }
    ui->GradeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->GradeTable->show();
    ui->AverageGpa->show();
}

