#include "frontend/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    MainWindow w;
    if (argc < 2 || argv[1] != QString{"--startup"})
        w.show();
    return a.exec();
}
