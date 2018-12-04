#include "mainwindow.h"
#include "sqlport.h"
#include <QApplication>
#include "components.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"
#include "storydisplay.h"
#include "storydisplay.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Support::DBInitTool tool;

    //MainWindow w ;
    //w.show();
    QMainWindow w;

    UIComp::StoryDisplay x;

    w.setCentralWidget(&x);
    w.resize(400, 300);
    w.show();

    for (int i=0; i < 20 ; ++i) {
        auto evn = new UIComp::EventSymbo(QString("测试重叠_%1").arg(i), i*100, i*100 + 30);
        x.addEvent(i, evn);
    }
    auto evn = new UIComp::EventSymbo("secondLine测试超长名字的边界问题secondLine测试超长名字的边界问题", 101, 400);
    x.addEvent(100, evn);
    auto evn2 = new UIComp::EventSymbo("汉字命名", 102, 353);
    x.addEvent(1003, evn2);
    return a.exec();
}
