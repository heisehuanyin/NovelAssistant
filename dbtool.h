#ifndef SQLPORT_H
#define SQLPORT_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
namespace Support {
    class DBTool:public QObject
    {
        Q_OBJECT
    public:
        explicit DBTool();
        virtual ~DBTool();

        /**
         * @brief 获取指定事件节点的开始时间和结束时间
         * @param event_id 事件节点id
         * @param startime 开始时间输出
         * @param endtime 结束时间输出
         * @return 执行成功返回true
         */
        static bool getRealtimeOfEventnode(const qlonglong event_id, qlonglong &startime,qlonglong &endtime);

        /**
         * @brief 获取指定角色在某时刻（事件节点中）的确切位置
         * @param event_nid 事件节点id
         * @param char_id 角色id
         * @param location_id 返回地点id
         * @param timeUntil 返回endtime
         * @return 成功获取该位置返回true，false表示角色未曾经历该事件、没有位置信息或数据库查询出错
         */
        static bool getRealLocationOnEventnode(const qlonglong event_nid,
                                               const qlonglong char_id,
                                               qlonglong &location_id,
                                               qlonglong &timeUntil);

        /**
         * @brief 获取"指定时间点之前，基准角色的所有关系"查询语句，能够查询所有相关关系[target_id, ]target_name, relationship, relationcomment
         * @param tidAccess 目标角色的id是否可见
         * @param char_id 基准角色id
         * @param timePoint 指定时间点
         * @param out 返回携带结果
         * @return 执行成功否
         */
        static bool getCharactersRelationshipUntilTime(const bool tidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery &out);


        /**
         * @brief 获取“指定时间点之前，基准角色的所有道具”查询语句，能够查询所有相关道具[items_id, ]items_name, items_type, items_grade, items_number, record_comment
         * @param pidAccess itemtype_id是否可见
         * @param char_id 基准角色id
         * @param timePoint 指定时间点
         * @param out 返回结果
         * @return 执行成功返回true
         */
        static bool getCharactersPropsUntilTime(const bool pidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery& out);


        /**
         * @brief 获取“指定时间点之前，基准角色的所有技能”查询语句，能够查询所有相关技能[items_id, ]items_name, items_type, items_grade, record_comment
         * @param sidAccess itemtype_id是否可见
         * @param char_id 基准角色id
         * @param timePoint 时间点
         * @param q 返回执行结果
         * @return 执行成功返回true
         */
        static bool getCharactersSkillsUntilTime(const bool sidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery &out);

    private:
        bool createConnection();
        void init_emptytable(QSqlDatabase db);
    };

}
#endif // SQLPORT_H
