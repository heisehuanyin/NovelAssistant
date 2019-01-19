#include "dbtool.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace Support;

DBTool::DBTool()
{
    this->createConnection();
    auto db = QSqlDatabase::database();
    this->init_emptytable(db);
}

DBTool::~DBTool()
{
    auto db = QSqlDatabase::database();
    db.close();
}

bool DBTool::getRealtimeOfEventnode(const qlonglong event_id, qlonglong& startime, qlonglong &endtime)
{
    QSqlQuery q;
    q.prepare("select "
              "enb.end_time, "
              "enb.begin_time "
              "from table_eventnodebasic enb "
              "where enb.ev_node_id = :id");
    q.bindValue(":id", event_id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    if(!q.next())
        return false;
    startime = q.value(1).toLongLong();
    endtime = q.value(0).toLongLong();
    return true;
}

bool DBTool::getRealLocationOnEventnode(const qlonglong event_nid, const qlonglong char_id, qlonglong &location_id, qlonglong& timeUntil)
{
    QSqlQuery q;
    q.prepare("SELECT "
              "clt.location_id, "
              "enb.end_time "
              "FROM table_characterlifetracker clt "
              "inner join table_eventnodebasic enb on clt.event_id = enb.ev_node_id "
              "WHERE (clt.char_id = :cid) AND "
              "(clt.event_id = :eid);");
    q.bindValue(":cid", char_id);
    q.bindValue(":eid", event_nid);

    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    if(!q.next())
        return false;

    location_id = q.value(0).toLongLong();
    timeUntil = q.value(1).toLongLong();
    return true;
}


bool DBTool::getCharactersRelationshipUntilTime(const bool tidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery &q)
{
    QString execStr = "SELECT ";
    if(tidAccess)
        execStr += "recent.char_id, ";

    execStr+= "cb.name, "
              "recent.relationship, "
              "recent.comment "
              "FROM ("
              "SELECT char_new char_id, "
              "relationship, "
              "comment "
              "FROM ( "
              "SELECT crs.target_id char_new, "
              "crs.relationship, "
              "crs.comment, "
              "enb.end_time "
              "FROM table_characterrelationship crs "
              "INNER JOIN "
              "table_eventnodebasic enb ON enb.ev_node_id = crs.event_id "
              "WHERE (crs.char_id = :cid) AND "
              "(enb.end_time <= :time) "
              "GROUP BY crs.target_id "
              "HAVING enb.end_time = max(enb.end_time) "
              "UNION "
              "SELECT crs.char_id char_new, "
              "crs.relationship, "
              "crs.comment, "
              "enb.end_time "
              "FROM table_characterrelationship crs "
              "INNER JOIN "
              "table_eventnodebasic enb ON enb.ev_node_id = crs.event_id "
              "WHERE (crs.target_id = :cid) AND "
              "(enb.end_time <= :time) "
              "GROUP BY crs.char_id "
              "HAVING enb.end_time = max(enb.end_time) "
              ")"
              "GROUP BY char_new "
              "HAVING end_time = max(end_time) )"
              "recent "
              "INNER JOIN "
              "table_characterbasic cb ON recent.char_id = cb.char_id;";

    q.prepare(execStr);
    q.bindValue(":cid", char_id);
    q.bindValue(":time", timePoint);

    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}


bool DBTool::getCharactersPropsUntilTime(const bool pidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery &q)
{
    QString exs ="SELECT ";
    if(pidAccess)
        exs += "pb.prop_id, ";
    exs += "pb.name, "
           "gtm.type_name, "
           "gtm.mark_name, "
           "cpc.number, "
           "cpc.comment "
           "FROM table_characterpropchange cpc "
           "INNER JOIN "
           "table_eventnodebasic enb ON cpc.event_node = enb.ev_node_id "
           "INNER JOIN "
           "table_propbasic pb ON cpc.prop = pb.prop_id "
           "inner join "
           "table_gtm gtm on pb.mark = gtm.mark_id "
           "WHERE (enb.end_time <= :endtime) and "
           "(cpc.char_id = :id) "
           "GROUP BY cpc.prop "
           "HAVING enb.end_time = max(enb.end_time);";

    q.prepare(exs);
    q.bindValue(":id", char_id);
    q.bindValue(":endtime", timePoint);

    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}


bool DBTool::getCharactersSkillsUntilTime(const bool sidAccess, const qlonglong char_id, const qlonglong timePoint, QSqlQuery &q)
{
    QString rtn = "SELECT ";
    if(sidAccess)
        rtn +=  "sl.skill_id, ";
    rtn += "sl.name, "
           "gtm.type_name, "
           "gtm.mark_name, "
           "cs.comment "
           "FROM table_characterskills cs "
           "INNER JOIN "
           "table_eventnodebasic enb ON cs.event_node = enb.ev_node_id "
           "INNER JOIN "
           "table_skilllist sl ON cs.skill = sl.skill_id "
           "inner join "
           "table_gtm gtm on sl.mark = gtm.mark_id "
           "WHERE (cs.character = :csid) AND "
           "(enb.end_time <= :endtime) "
           "GROUP BY sl.skill_id "
           "HAVING enb.end_time = max(enb.end_time);";
    q.prepare(rtn);
    q.bindValue(":csid", char_id);
    q.bindValue(":endtime", timePoint);

    if(!q.exec()){
        qDebug() << q.lastError();
        return false;
    }
    return true;
}

bool DBTool::createConnection(){
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("DO_NOT_TOUCH_ANY_FILE.db");
    if(!db.open()){
        return false;
    }
    QSqlQuery x(db);
    return x.exec("PRAGMA foreign_keys = ON;");
}

void DBTool::init_emptytable(QSqlDatabase db)
{
    QSqlQuery query(QSqlDatabase::database());
    auto tables = db.tables();

    //创建Group_Type_Mark 表格
    query.exec("CREATE TABLE IF NOT EXISTS table_gtm("
               "mark_id     integer PRIMARY KEY autoincrement,"
               "group_name  text,"
               "type_name   text,"
               "mark_number integer,"
               "mark_name   text,"
               "comment     text"
               ");");

    query.exec("create table IF NOT EXISTS table_skilllist("
               "skill_id integer primary key autoincrement,"
               "name text,"
               "skill_desc text,"
               "mark integer,"
               "number integer,"
               "comment text,"
               "CONSTRAINT sl_gtm_key FOREIGN KEY(mark)"
               " REFERENCES table_gtm(mark_id)"
               " ON DELETE CASCADE);");

    query.exec("create table IF NOT EXISTS table_propbasic("
               "prop_id integer primary key autoincrement,"
               "name text,"
               "prop_desc text,"
               "mark integer,"
               "number integer,"
               "comment text,"
               "CONSTRAINT pb_gtm FOREIGN KEY(mark)"
               " REFERENCES table_gtm(mark_id)"
               " ON DELETE CASCADE);");

    //创建EventNode_Basic 表格
    query.exec("CREATE TABLE IF NOT EXISTS table_eventnodebasic("
               "ev_node_id integer primary key  autoincrement,"
               "node_name  text,"
               "event_name text,"
               "begin_time integer,"
               "end_time   integer,"
               "node_desc  text,"
               "comment    text);");

    query.exec("CREATE TABLE IF NOT EXISTS table_locationlist("
               "location_id       integer  primary key  autoincrement,"
               "location_name     text,"
               "corrdinate_suffix text,"
               "xposition         integer,"
               "yposition         integer,"
               "zposition         integer,"
               "nickname          text,"
               "story             text,"
               "comment           text);");

    //创建CharacterBasic 表格
    query.exec("CREATE TABLE IF NOT EXISTS table_characterbasic("
               "char_id  integer primary key autoincrement,"
               "name     text,"
               "nikename text,"
               "birthday integer,"
               "deathday integer,"
               "comment  text);");

    //创建CharacterRelationship 表格
    query.exec("CREATE TABLE IF NOT EXISTS table_characterrelationship("
               "id           integer primary key autoincrement,"
               "char_id      integer,"
               "target_id    integer,"
               "relationship text,"
               "event_id     integer,"
               "comment      text,"
               "CONSTRAINT cr_cb_key1 FOREIGN KEY(char_id)"
               " REFERENCES table_characterbasic(char_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT cr_cb_key2 FOREIGN KEY(target_id)"
               " REFERENCES table_characterbasic(char_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT cr_ene_key FOREIGN KEY(event_id)"
               " REFERENCES table_eventnodebasic(ev_node_id)"
               " ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS table_characterlifetracker("
               "id          integer primary key autoincrement,"
               "char_id     integer ,"
               "event_id    integer,"
               "location_id integer,"
               "char_desc   text,"
               "comment     text,"
               "CONSTRAINT clt_cb_key FOREIGN KEY(char_id)"
               " REFERENCES table_characterbasic(char_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT clt_ene_key FOREIGN KEY(event_id)"
               " REFERENCES table_eventnodebasic(ev_node_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT clt_ll_key FOREIGN KEY(location_id)"
               " REFERENCES table_locationlist(location_id)"
               " ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS table_characterskills("
               "id         integer primary key autoincrement,"
               "character  integer,"
               "skill      integer,"
               "event_node integer,"
               "comment    text,"
               "CONSTRAINT cs_cb_key FOREIGN KEY(character)"
               " REFERENCES table_characterbasic(char_id) "
               " ON DELETE CASCADE,"
               "CONSTRAINT cs_sl_key FOREIGN KEY(skill)"
               " REFERENCES table_skilllist(skill_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT cs_ene_key FOREIGN KEY(event_node)"
               " REFERENCES table_eventnodebasic(ev_node_id)"
               " ON DELETE CASCADE);");

    query.exec("CREATE TABLE IF NOT EXISTS table_characterpropchange("
               "id         integer primary key autoincrement,"
               "char_id    integer,"
               "event_node integer,"
               "prop       integer,"
               "number     integer,"
               "comment    text,"
               "CONSTRAINT cpc_cb_key FOREIGN KEY(char_id)"
               " REFERENCES table_characterbasic(char_id)"
               " on delete cascade,"
               "CONSTRAINT cpc_ene_key FOREIGN KEY(event_node)"
               " REFERENCES table_eventnodebasic(ev_node_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT cpc_pb_key FOREIGN KEY(prop)"
               " REFERENCES table_propbasic(prop_id)"
               " ON DELETE CASCADE);");

    query.exec("create table IF NOT EXISTS table_locationchange("
               "id            integer primary key autoincrement,"
               "location      integer,"
               "event_node    integer,"
               "location_desc text,"
               "social_desc   text,"
               "comment       text,"
               "CONSTRAINT lc_ll_key FOREIGN KEY(location)"
               " REFERENCES table_locationlist(location_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT lc_ene_key FOREIGN KEY(event_node)"
               " REFERENCES table_eventnodebasic(ev_node_id)"
               " ON DELETE CASCADE);");

    if(!tables.contains("table_timeformat")){
        query.exec("create table table_timeformat ("
                   "id integer primary key autoincrement,"
                   "_index integer,"
                   "unit text,"
                   "base integer,"
                   "not_0 text,"
                   "comment text"
                   ");");

        query.exec("insert into table_timeformat "
                   "(_index, unit, base, not_0, comment)"
                   "values(0, '日', 1, 'yes', '每个base数值都是以天为转换基础');");
        query.exec("insert into table_timeformat "
                   "(_index, unit, base, not_0, comment)"
                   "values(1, '月', 30, 'yes', '每个base数值都是以天为转换基础');");
        query.exec("insert into table_timeformat "
                   "(_index, unit, base, comment)"
                   "values(2, '年', 360, '每个base数值都是以天为转换基础');");
        query.exec("insert into table_timeformat "
                   "(_index, unit, base, comment)"
                   "values(3, '纪', 36000, '每个base数值都是以天为转换基础');");
        query.exec("insert into table_timeformat "
                   "(_index, unit, base, comment)"
                   "values(4, '元', 36000000, '每个base数值都是以天为转换基础');");
    }
}




























