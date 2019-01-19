#include <QApplication>
#include "edserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EdServer x("WsNovelAssistant");
    x.openGraphicsModel();

    auto val = a.exec();

    //默认自动保存左右状态
    x.saveAll();
    return val;

}
