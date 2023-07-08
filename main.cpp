#include "frontend/mainwindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Li Bohai & Zhang Yihao");
    QCoreApplication::setOrganizationDomain("https://github.com/000lbh/PKUHelperAgain");
    QCoreApplication::setApplicationName("PKU Helper Again");
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    MainWindow w;
    if (argc < 2 || argv[1] != QString{"--hide"})
        w.show();
    return a.exec();
}
