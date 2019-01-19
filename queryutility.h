#ifndef QUERYUTILITY_H
#define QUERYUTILITY_H

#include <QComboBox>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>

namespace Component {
    class StoryDisplay;


    class QueryUtility: public QTabWidget
    {
        Q_OBJECT
    public:
        explicit QueryUtility(QWidget* parent=nullptr);
        virtual ~QueryUtility();

        //全局重置，用于切换项目后清空之前的残留数据
        void reset();
        /**
     * @brief 接收keywords扫描事件，简略信息呈现在快速阅览tab中
     * @param item
     */
        void resetFocusedKeywords(qlonglong item, const QString &type);

    public slots:
        /**
     * @brief 接收BaseEvent重置事件
     * @attention 影响既往事件查询：多人物事件，多地点事件
     * @attention 影响当前指定角色道具列表呈现
     * @attention 影响当前指定角色技能清单查询
     * @attention 影响当前指定角色社会关系查询
     * @attention 影响当前指定地点社会和风貌查询
     * @param ev_node
     */
        void resetBaseEvent(qlonglong ev_node);

    private:
        QList<QPair<QString,qlonglong>> keywordsList;
        QTableView *const quickLook;//快速信息清单
        QStandardItemModel*const qlook;
        qlonglong   ev_node;
        QComboBox *const place1;
        QTableView *const previousEvents;
        QStandardItemModel *const pre_Events;
        StoryDisplay *const eventsView;
        QComboBox *const character;
        QTableView *const char_items;
        QSqlQueryModel *const citemsm;
        QTableView *const char_abilitys;
        QSqlQueryModel *const cabilitysm;
        QTableView *const char_relates;
        QSqlQueryModel *const crelatesm;
        QComboBox *const place2;
        QTableView *const physicalView;
        QStandardItemModel*const physicalModel;
        QTableView *const socialView;
        QStandardItemModel*const socialModel;


        typedef void (QueryUtility::*Exefp)(QPair<QString, qlonglong> &node, QSqlQuery &exec);
        QHash<QString, Exefp> map;

        /**
     * @brief 基于事件时间点，汇总相同地点的不同事件（不同角色、地点等）
     */
        void refreshEventMap();
        /**
     * @brief 基于事件时间点，刷新角色相关（道具、技能、人际）
     */
        void refreshCharacter__About();
        /**
     * @brief 基于事件时间点，刷新地点相关（人文、物理风貌，绑定道具）
     */
        void refreshLocation_About();

        /**
     * @brief 快速查询角色简略信息
     * @param node 信息节点
     * @param exec empty execStatement
     */
        void quickQueryCharacter(QPair<QString,qlonglong>& node, QSqlQuery& exec);
        /**
     * @brief 快速查询道具简略信息
     * @param node 信息节点
     * @param exec empty execStatement
     */
        void quickQueryItems(QPair<QString,qlonglong>&node, QSqlQuery& exec);
        /**
     * @brief 快速查询能力简略信息
     * @param node 信息节点
     * @param exec empty execStatement
     */
        void quickQueryAbility(QPair<QString,qlonglong>& node, QSqlQuery& exec);
        /**
     * @brief 快速查询地点简略信息
     * @param node 信息节点
     * @param exec empty execStatement
     */
        void quickQueryPlace(QPair<QString,qlonglong>& node, QSqlQuery& exec);
        /**
     * @brief 快速查询事件简略信息
     * @param node 信息节点
     * @param exec empty execStatement
     */
        void quickQueryEvent(QPair<QString,qlonglong>& node, QSqlQuery& exec);

    private slots:
        /**
     * @brief 页面切换时调用
     * @param index tab索引
     */
        void targetTabContextRefresh(int index);

    };


}
#endif // QUERYUTILITY_H
