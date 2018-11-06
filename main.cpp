#include "mainwindow.h"
#include "sqlport.h"
#include <QApplication>
#include "components.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w ;
    w.show();

    Support::DatabaseInitialTool tool;
    UIComp::SkillEdit x(&w);
    x.exec();

    return a.exec();
}
