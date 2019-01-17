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
         * @brief 获取"指定时间点之前，基准角色的所有关系"查询语句，能够查询所有相关关系，无论正反
         * @param tidAccess 目标角色的id是否可见
         * @param char_id 基准角色id
         * @param timePoint 时间点
         * @return 查询字符串[target_id, ]target_name, relationship, relationcomment
         */
        static QString getCharsRelationshipUntilTime(const bool tidAccess, const qlonglong char_id, const qlonglong timePoint);

        /**
         * @brief 获取“指定时间点之前，基准角色的所有道具”查询语句，能够查询所有相关道具，包括继承下来的
         * @param pidAccess 道具的id是否可见
         * @param char_id 基准角色id
         * @param timePoint 时间点
         * @return 字符串[items_id, ]items_name, items_number, record_comment
         */
        static QString getCharsPropsUntilTime(const bool pidAccess, const qlonglong char_id, const qlonglong timePoint);

        /**
         * @brief 获取“指定时间点之前，基准角色的所有技能”查询语句，能够查询所有相关技能，包括继承下来的
         * @param sidAccess 技能的id是否可见
         * @param char_id 基准角色id
         * @param timePoint 时间点
         * @return 字符串[ability_id, ]ability_name, record_comment
         */
        static QString getCharsSkillsUntilTime(const bool sidAccess, const qlonglong char_id, const qlonglong timePoint);

    private:
        bool createConnection();
        void init_emptytable(QSqlDatabase db);
    };

}
#endif // SQLPORT_H
