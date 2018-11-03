#include "mainwindow.h"
#include "sqlport.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseInitialTool tool;

    return a.exec();
}
