#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("pe-jot");
    QCoreApplication::setApplicationName("DHCP Server");

    MainWindow w;
    w.show();
    return a.exec();
}
