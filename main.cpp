#include "mainwindow.h"
#include "sqlport.h"
#include <QApplication>
#include "components.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Support::DatabaseInitialTool tool;
    UIComp::GTME x;
    x.exec();

    return a.exec();
}
