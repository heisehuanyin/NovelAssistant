#ifndef HIDDENIDMODEL_H
#define HIDDENIDMODEL_H

#include <QAbstractTableModel>


namespace Support {

    /**
     * @brief 专用隐藏条目id的table模型，利用sql查询数据库，本模型不支持条目编辑，仅支持勾选条目，以方便选择功能开发。
     * 使用方法：
     * 1.新建模型，可以利用changeCheckable决定是否允许勾选条目；
     * 2.【可选】首先调用setSelectedDetermine方法，根据传入的sql获取id集合，此id集合用于判定结果条目是否选中状态，条目id包含在其中意味着被勾选；
     * 3.使用setQuery方法，根据传入的sql语句进行查询，该语句中必须包含结果条目id，此id将会被隐藏，供后台使用；
     * 4.【可选】使用setHorizontalHeader方法为结果设置列标题；
     * 5.可以对视图进行操作，勾选条目和取消条目，这个过程中表格模型会发出对应信号；
     * 6.【可选】使用selectedRecordIDs获取所有被选中的条目的id；
     * 7.【可选】使用oppositeID方法，能够获取视图中指定结果条目的id；
     * 8.【可选】使用clear方法，清空结果集合，仅清空结果集合；
     */
    class HiddenIdModel:public QAbstractTableModel
    {
        Q_OBJECT
    public:
        /**
         * @brief 新建一个自定义SqlQueryModel
         * @param parent 父对象
         * @param checkable 决定是否允许显示checkable特性
         */
        explicit HiddenIdModel(QObject*parent=nullptr);
        virtual ~HiddenIdModel() override;

        /**
         * @brief 修改模型条目是否允许勾选
         * @param value 是否允许
         */
        void changeCheckable(bool value);

        /**
         * @brief 执行查询语句,执行语句中必须指明结果包含每条记录的唯一id，指明id位于结果的索引，以便于模型隐藏它
         * @param valueCount 指明此语句中目标值数量
         * @param sqlStatement sql语句
         * @param indexOfId 指明在此语句的目标值序列中记录id所占的index
         */
        void setQuery(int valueCount, QString sqlStatement, int indexOfId = 0);

        void clear();

        void setHorizontalHeader(int index, QString header);

        /**
         * @brief 首先执行本函数获取结果，后执行setquery，结果决定结果视图中每条记录的选定状态，
         * 传入执行的语句执行结果第一个值必须是记录的id，而且此id与setQuery隐藏的id相同。
         * 本函数执行结果可以重复利用，每次使用setquery获得的记录都会使用此结果，执行一次本函数，结果集刷新一次。
         */
        void setSelectedDetermine(QString sqlStatement);

        /**
         * @brief 查询此模型中所有选中的记录的id
         * @return 选中记录id列表
         */
        QList<QVariant> selectedRecordIDs();

        QVariant oppositeID(QModelIndex &index);

    signals:
        /**
         * @brief 本模型中指定记录被勾选
         * @return 指定记录的id
         */
        void signal_recordHasBeenSelected(qlonglong id);

        /**
         * @brief 本模型中指定记录被取消勾选
         * @return 指定记录的id
         */
        void signal_recordHasBeenDeselected(qlonglong id);

    private:
        //                recordID  checkstate   value
        QList<QPair<QPair<QVariant,bool>,QList<QVariant>>> contents;
        QList<QString> headers;
        QList<QVariant> selectedIDs;
        bool checkable;


        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &) const override;
        virtual int columnCount(const QModelIndex &) const override;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    };

}

#endif // SQLQUERYMODELWITHHIDDENID_H
