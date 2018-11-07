#ifndef SUPERDATETOOL_H
#define SUPERDATETOOL_H

#include <QList>
#include <QString>


namespace Support {

    class SuperDateTool
    {
    public:
        explicit SuperDateTool();
        virtual ~SuperDateTool();

        /**
         * @brief 将qlonglong数据转换成为字符串日期格式
         * @param days 以天为单位代表的日期
         * @return
         */
        QString turn2Str(qlonglong days);
        qlonglong turn2Longlong(const QString &text);

    private:
        QList<QPair<QString, qlonglong>> list;

    };

}

#endif // SUPERDATETOOL_H
