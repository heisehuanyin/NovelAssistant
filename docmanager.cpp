#include "docmanager.h"

#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

using namespace Support;

DocManager::DocManager()
{

}

DocManager::~DocManager()
{
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

    auto v = new QTextEdit;
    this->docCon.insert(filePath, v);
    *view = v;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

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

int DocManager::saveAll()
{
    auto keys = this->docCon.keys();

    for(int i=0; i< keys.size(); ++i){
        if(int ret = this->saveDocument(keys.at(i)) != 0)
            return ret;
    }
    return 0;
}

QList<QString> DocManager::getActiveDocs()
{
    return this->docCon.keys();
}

int DocManager::closeDocumentWithoutSave(QString filePath)
{
    if(!this->docCon.contains(filePath))
        return -1;

    auto view = this->docCon.take(filePath);
    delete view;
    return 0;
}

QString DocManager::returnDocpath(QTextEdit *view)
{
    return this->docCon.key(view);
}
