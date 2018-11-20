#ifndef SUPERDATETOOL_H
#define SUPERDATETOOL_H

#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QString>


namespace Support {

    class SuperDateTool: public QDialog
    {
        Q_OBJECT
    public:
        explicit SuperDateTool(QWidget *parent = nullptr);
        virtual ~SuperDateTool();

        /**
         * @brief 重新设置内部日期状态
         * @param daysCount 以天为计算的日期基准
         */
        void resetDate(qlonglong days);

        /**
         * @brief 弹框进行日期编辑，传入下限进行限制日期负向范围
         * @param downLimit 日期允许的下限
         * @return 编辑结果转换为天数
         */
        qlonglong dateEdit();

        qlonglong toLongLong();

        /**
         * @brief 将qlonglong数据转换成为字符串日期格式
         * @param days 以天为单位代表的日期
         * @return
         */
        QString toString();


    private:
        QList<QPair<QString, qlonglong>> baseList;
        QHash<QString, qlonglong> innerValues;
        QList<QString> item_not0;
        QList<QLineEdit *> editList;

    private slots:
        void slot_cannotAssignZero();

    };

}

#endif // SUPERDATETOOL_H
