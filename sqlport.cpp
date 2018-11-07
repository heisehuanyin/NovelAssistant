#include "sqlport.h"

#include <QSqlQuery>

using namespace Support;

DBInitTool::DBInitTool()
{
    this->createConnection();
    auto db = QSqlDatabase::database();
    auto tables = db.tables();
    if(!tables.contains("table_gtm"))
        this->init_emptytable();
}

DBInitTool::~DBInitTool()
{
    auto db = QSqlDatabase::database();
    db.close();
}

bool DBInitTool::createConnection(){
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("NovelData.db");
    if(!db.open()){
        return false;
    }
    QSqlQuery x(db);
    return x.exec("PRAGMA foreign_keys = ON;");
}

void DBInitTool::init_emptytable()
{
    QSqlQuery query(QSqlDatabase::database());

    //创建Group_Type_Mark 表格
    query.exec("CREATE TABLE table_gtm("
               "mark_id     integer PRIMARY KEY autoincrement,"
               "group_name  text,"
               "type_name   text,"
               "mark_number integer,"
               "mark_name   text,"
               "comment     text"
               ");");

    query.exec("create table table_skilllist("
               "skill_id integer primary key autoincrement,"
               "name text,"
               "skill_desc text,"
               "mark integer,"
               "number integer,"
               "comment text,"
               "CONSTRAINT sl_gtm_key FOREIGN KEY(mark)"
               " REFERENCES table_gtm(mark_id)"
               " ON DELETE CASCADE);");

    query.exec("create table table_propbasic("
               "prop_id integer primary key autoincrement,"
               "name text,"
               "prop_desc text,"
               "mark integer,"
               "number integer,"
               "comment text,"
               "CONSTRAINT pb_gtm FOREIGN KEY(mark)"
               " REFERENCES table_gtm(mark_id)"
               " ON DELETE CASCADE);");

    //创建EventNode_Effect 表格
    query.exec("CREATE TABLE table_eventnodeeffect("
               "ev_node_id integer primary key  autoincrement,"
               "node_name  text,"
               "event_name text,"
               "begin_time integer,"
               "end_time   integer,"
               "node_desc  text,"
               "comment    text);");

    query.exec("CREATE TABLE table_locationlist("
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
    query.exec("CREATE TABLE table_characterbasic("
               "char_id  integer primary key autoincrement,"
               "name     text,"
               "nikename text,"
               "birthday integer,"
               "deathday integer,"
               "comment  text);");

    //创建CharacterRelationship 表格
    query.exec("CREATE TABLE table_characterrelationship("
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
               " REFERENCES table_eventnodeeffect(ev_node_id)"
               " ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE table_characterlifetracker("
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
               " REFERENCES table_eventnodeeffect(ev_node_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT clt_ll_key FOREIGN KEY(location_id)"
               " REFERENCES table_locationlist(location_id)"
               " ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE table_characterskills("
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
               " REFERENCES table_eventnodeeffect(ev_node_id)"
               " ON DELETE CASCADE);");

    query.exec("CREATE TABLE table_characterpropchange("
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
               " REFERENCES table_eventnodeeffect(ev_node_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT cpc_pb_key FOREIGN KEY(prop)"
               " REFERENCES table_propbasic(prop_id)"
               " ON DELETE CASCADE);");

    query.exec("create table table_locationchange("
               "id            integer primary key autoincrement,"
               "location      integer,"
               "event_node    integer,"
               "location_desc text,"
               "social_desc   text,"
               "prop          integer,"
               "prop_num      integer,"
               "comment       text,"
               "CONSTRAINT lc_ll_key FOREIGN KEY(location)"
               " REFERENCES table_locationlist(location_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT lc_ene_key FOREIGN KEY(event_node)"
               " REFERENCES table_eventnodeeffect(ev_node_id)"
               " ON DELETE CASCADE,"
               "CONSTRAINT lc_pb_key FOREIGN KEY(prop)"
               " REFERENCES table_propbasic(prop_id)"
               " ON DELETE CASCADE);");
    query.exec("create table table_timeformat ("
               "id integer primary key autoincrement,"
               "_index integer,"
               "unit text,"
               "base integer,"
               "comment text"
               ");");
    query.exec("insert into table_timeformat "
               "(_index, unit, base, comment)"
               "values(0, '日', 1, '每个base数值都是以天为转换基础');");
    query.exec("insert into table_timeformat "
               "(_index, unit, base, comment)"
               "values(1, '月', 30, '每个base数值都是以天为转换基础');");
    query.exec("insert into table_timeformat "
               "(_index, unit, base, comment)"
               "values(2, '年', 365, '每个base数值都是以天为转换基础');");
    query.exec("insert into table_timeformat "
               "(_index, unit, base, comment)"
               "values(3, '世纪', 36500, '每个base数值都是以天为转换基础');");
    query.exec("insert into table_timeformat "
               "(_index, unit, base, comment)"
               "values(4, '元', 36500000, '每个base数值都是以天为转换基础');");
}




























