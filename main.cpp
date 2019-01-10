#include <QApplication>
#include "edserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EdServer x("test");
    x.openGraphicsModel();

    auto val = a.exec();

    //默认自动保存左右状态
    x.slot_saveAll();
    return val;

}
