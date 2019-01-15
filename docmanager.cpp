#include "docmanager.h"
#include "syntaxhightlighter.h"

#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

using namespace Support::__projectsymbo;

void DocManager::keywordsFocused(qlonglong item, const QString &type)
{
    emit this->discoveryKeywords(item, type);
}

DocManager::DocManager(QObject *parent):
    QObject (parent),
    dataSource(new __syntaxhighlighter::DataSource)
{

}

DocManager::~DocManager()
{
    auto lighters = this->lighters.keys();
    auto views = this->docCon.keys();

    foreach (auto view, views) {
        auto item = this->docCon.value(view);
        delete item;
    }
    foreach (auto lighter, lighters) {
        auto item = this->lighters.value(lighter);
        delete item;
    }
    delete this->dataSource;
}

int DocManager::openDocument(QString filePath, QTextEdit **view)
{
    if(this->docCon.contains(filePath)){
        *view = this->docCon.value(filePath);
        return 0;
    }

    QFile file(filePath);
    if(!file.exists()){
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return -1;
        file.close();
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;


    auto v = new QTextEdit;
    this->docCon.insert(filePath, v);
    *view = v;

    auto lighter = new SyntaxHighlighter(this->dataSource, this);
    lighter->setDocument(v->document());
    this->lighters.insert(filePath, lighter);
    QObject::connect(lighter,   &SyntaxHighlighter::discoveryKeywords,
                     this,      &DocManager::keywordsFocused);

    QTextStream in(&file);
    while (!in.atEnd()) {
        auto line = in.readLine();
        v->append(line);
    }
    file.close();

    return 0;
}

int DocManager::saveDocument(QString filePath)
{
    if(!this->docCon.contains(filePath))
        return -1;

    auto view = this->docCon.value(filePath);
    auto content = view->toPlainText();

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -2;

    QTextStream out(&file);
    out << content;

    out.flush();
    file.flush();
    file.close();

    return 0;
}

int DocManager::saveAllActived()
{
    auto keys = this->docCon.keys();

    for(int i=0; i< keys.size(); ++i){
        if(int ret = this->saveDocument(keys.at(i)) != 0)
            return ret;
    }
    return 0;
}

Support::__syntaxhighlighter::DataSource &DocManager::getDataSource()
{
    return *this->dataSource;
}

QList<QString> DocManager::getActiveDocs()
{
    return this->docCon.keys();
}

bool DocManager::isActived(QString filePath)
{
    return this->docCon.contains(filePath);
}

int DocManager::closeDocumentWithoutSave(QString filePath)
{
    if(!this->docCon.contains(filePath))
        return -1;

    auto view = this->docCon.take(filePath);
    delete view;
    auto lighter = this->lighters.take(filePath);
    QObject::disconnect(lighter,    &SyntaxHighlighter::discoveryKeywords,
                        this,       &DocManager::keywordsFocused);
    delete lighter;

    return 0;
}

QString DocManager::returnDocpath(QTextEdit *view)
{
    return this->docCon.key(view);
}
