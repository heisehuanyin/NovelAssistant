#include "mainwindow.h"
#include "sqlport.h"
#include <QApplication>
#include "components.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"
#include "superdatetool.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Support::DBInitTool tool;

    MainWindow w ;
    w.show();

    return a.exec();
}
