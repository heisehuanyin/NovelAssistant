#include <QApplication>
#include "edserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EdServer x("test");
    x.openGraphicsModel();

    return a.exec();

}
