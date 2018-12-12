#ifndef SUPERDATETOOL_H
#define SUPERDATETOOL_H

#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QString>


namespace Support {

    class SuperDateTool:public QDialog
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
         * @brief 弹框进行日期编辑
         * @return 返回此次编辑时间结果
         */
        qlonglong dateEdit();

        /**
         * @brief 获取内部实际时间
         * @return 时间数字
         */
        qlonglong toLongLong();

        /**
         * @brief 将qlonglong数据转换成为字符串日期格式
         * @param days 以天为单位代表的日期
         * @return 内部实际时间
         */
        QString toString();


    private:
        //基础依据    时间标签  转换率
        QList<QPair<QString, qlonglong>> baseList;
        //真实时间  标签  真实值
        QHash<QString, qlonglong> innerValues;
        //不为零的项目
        QList<QString> item_not0;
        //编辑器列表
        QList<QLineEdit *> editList;

    private slots:
        void slot_cannotAssignZero();

    };

}

#endif // SUPERDATETOOL_H
