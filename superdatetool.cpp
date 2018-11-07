#include "superdatetool.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

using namespace Support;

SuperDateTool::SuperDateTool()
{
    QSqlQuery q;

    if(!q.exec("select "
              "unit,"
              "base,"
              "comment "
              "from table_timeformat "
               "order by _index desc;")){
        qDebug() << q.lastError();
    }
    while (q.next()) {
        this->list << QPair<QString,qlonglong>(q.value(0).toString(), q.value(1).toLongLong());
    }
}

SuperDateTool::~SuperDateTool()
{

}

QString SuperDateTool::turn2Str(qlonglong days)
{
    QString rtnStr;
    QPair<QString,qlonglong> pair;
    if(days < 0)
        rtnStr = "前";
    foreach (pair, this->list) {
        qlonglong times = days / pair.second;
        days = days % pair.second;
        if(times == 0 && days < 0){
            days = pair.second + days;
        }
        rtnStr += QString("%1").arg(times) + pair.first;
    }
    return rtnStr;
}

qlonglong SuperDateTool::turn2Longlong(const QString &text)
{

}
