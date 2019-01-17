#ifndef EVENTNODEEDIT_H
#define EVENTNODEEDIT_H

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextEdit>
#include "hideidmodel.h"
#include "superdatetool.h"

namespace Editor {

    class EventNodes : public QDialog
    {
        Q_OBJECT
    public:
        explicit EventNodes(QWidget *parent=nullptr);
        virtual ~EventNodes();

        /**
         * @brief 获取选中条目id集合
         * @return ID集合
         */
        static QList<QVariant> getSelectedItems();

    private:
        QLineEdit   *const eventNameInput;
        QPushButton *const addItem,
        *const removeItem,
        *const apply;
        QVariant focusedID;

        QTableView  *const eventTable;
        Support::HideIdModel *const eventModel;

        QTabWidget  *const tabCon;

        QLineEdit   *const evtNodeNameInput;
        QPushButton *const birthDay,
        *const deathDay;
        Support::SuperDateTool *const beginStatus,
        *const endStatus;
        QTextEdit   *const evNodeDesc;
        QTextEdit   *const evNodeComment;

        QTableView  *const localTable;
        QStandardItemModel * const locals_Model;
        QList<QVariant> local_List_ids;
        QPushButton *const addLocation;
        QPushButton *const removeLocation;

    private slots:
        /**
         * @brief 与事件名输入直连，在数据库中查询事件
         * @param text 事件名
         */
        void slot_queryEventNode(const QString &text);
        /**
         * @brief 添加事件阶段
         */
        void slot_respond2Additem();
        /**
         * @brief 移除事件阶段
         */
        void slot_respond2Removeitem();
        /**
         * @brief 响应查询返回的事件列表的选择操作
         */
        void slot_targetItemChanged(const QItemSelection &, const QItemSelection &);
        /**
         * @brief 编辑事件开始时间
         */
        void slot_editBeginTime();
        /**
         * @brief 编辑事件结束时间
         */
        void slot_editEndTime();
        void slot_stateChanged();
        /**
         * @brief 响应应用按钮操作，存储修改数据到数据库
         */
        void slot_4Apply();

        //地点范围性影响子面板的相关操作
        void slot_Respond2LocationAdd();
        void slot_Respond2LocationRemove();

        void slot_ItemChanged(QStandardItem* item);
    };

}

#endif // EVENTNODEEDIT_H
