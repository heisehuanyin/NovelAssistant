#include "syntaxhightlighter.h"

using namespace Support;

SyntaxHighlighter::SyntaxHighlighter(__syntaxhighlighter::DataSource *dataSource, QObject *parent):
    QSyntaxHighlighter (parent),
    dataSource(dataSource){}

SyntaxHighlighter::~SyntaxHighlighter(){}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    auto formates = this->dataSource->returnDataSource();
    auto types = formates.keys();

    foreach (auto type, types) {
        auto kwList = formates.value(type);

        foreach (auto keyword, *kwList->second) {
            int index = text.indexOf(keyword.first);
            if(index >=0)
                emit this->discoveryKeywords(keyword.second, type);

            while (index >= 0) {
                int length = keyword.first.length();
                setFormat(index, length, *kwList->first);
                index = text.indexOf(keyword.first, index+length);
            }
        }
    }
}

__syntaxhighlighter::DataSource::DataSource()
{

}

__syntaxhighlighter::DataSource::~DataSource()
{
    auto list = this->formats.keys();

    foreach(auto type, list){
        auto itemPair = this->formats.value(type);
        delete itemPair->first;
        delete itemPair->second;
        delete itemPair;
    }
}

void __syntaxhighlighter::DataSource::addKeywords(const QString type, const QString keywords, qlonglong itemId){

    if(!formats.contains(type)){
        //默认format style
        auto format = new QTextCharFormat;

        format->setFontItalic(true);
        format->setForeground(Qt::blue);
        format->setFontUnderline(true);
        format->setFontWeight(QFont::Bold);

        auto list = new QList<QPair<QString,qlonglong>>;
        auto keyList = new QPair<QTextCharFormat*, QList<QPair<QString,qlonglong>>*>(format, list);
        formats.insert(type, keyList);
    }

    auto kwList = formats.value(type);
    if(kwList->second->contains(qMakePair(keywords, itemId)))
        return;
    kwList->second->append(qMakePair(keywords, itemId));
}

void __syntaxhighlighter::DataSource::refreshKeywordsSet(){
    auto types = this->getTypes();
    foreach(auto type, types){
        this->clear(type);
    }

    //add types keywords========================
    QSqlQuery q;
    if(!q.exec("select char_id,"
               "name "
               "from table_characterbasic ;"))
    {
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->addKeywords("table_characterbasic",
                          q.value(1).toString(),
                          q.value(0).toLongLong());
    }
    if(!q.exec("select ev_node_id, "
               "event_name "
               "from table_eventnodebasic;"))
    {
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->addKeywords("table_eventnodebasic",
                          q.value(1).toString(),
                          q.value(0).toLongLong());
    }
    if(!q.exec("select location_id, "
               "location_name "
               "from table_locationlist;"))
    {
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->addKeywords("table_locationlist",
                          q.value(1).toString(),
                          q.value(0).toLongLong());
    }
    if(!q.exec("select prop_id, "
               "name "
               "from table_propbasic;"))
    {
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->addKeywords("table_propbasic",
                          q.value(1).toString(),
                          q.value(0).toLongLong());
    }
    if(!q.exec("select skill_id, "
               "name "
               "from table_skilllist;"))
    {
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->addKeywords("table_skilllist",
                          q.value(1).toString(),
                          q.value(0).toLongLong());
    }
}

__syntaxhighlighter::DataSource::Formats &__syntaxhighlighter::DataSource::returnDataSource(){
    return this->formats;
}

void __syntaxhighlighter::DataSource::clear(QString type){
    if(formats.contains(type)){
        auto keyList = formats.value(type);
        keyList->second->clear();
    }
}

QList<QString> __syntaxhighlighter::DataSource::getTypes(){
    return formats.keys();
}

QTextCharFormat __syntaxhighlighter::DataSource::getFormat(const QString &type){
    if(formats.contains(type))
        return * formats.value(type)->first;

    return QTextCharFormat();
}

void __syntaxhighlighter::DataSource::setCustomFormat(const QString type, QTextCharFormat &format){
    auto kwList = formats.value(type);

    kwList->first->setFontFamily(format.fontFamily());
    kwList->first->setFontItalic(format.fontItalic());
    kwList->first->setFontWeight(format.fontWeight());
    kwList->first->setFontUnderline(format.fontUnderline());
    kwList->first->setForeground(format.foreground());
}

void __syntaxhighlighter::DataSource::setGlobalFontSize(int weight){
    auto keys = formats.keys();
    foreach(auto type, keys){
        auto kwlist = formats.value(type);
        kwlist->first->setFontPointSize(weight);
    }
}
