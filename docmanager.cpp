#include "docmanager.h"

#include <QFileInfo>
#include <QTextStream>

using namespace Support;

DocManager::DocManager()
{

}

DocManager::~DocManager()
{
}

int DocManager::openDocument(QString filePath, QTextEdit **view)
{
    QFile file(filePath);
    if(!file.exists())
        return -1;

    if(this->docCon.contains(filePath)){
        *view = this->docCon.value(filePath);
        return 0;
    }

    auto v = new QTextEdit;
    this->docCon.insert(filePath, v);
    *view = v;

    QTextStream in(&file);
    while (!file.atEnd()) {
        auto line = in.readLine();
        v->append(line);
    }
    file.close();
    return 0;
}

int DocManager::newDocument(QString filePath, QString *realPath, QTextEdit **view)
{
    qsrand((unsigned)time(nullptr));

    QFileInfo info(filePath+".txt");
    while (info.exists()) {
        filePath +=  QString("%1").arg(qrand());
        info.setFile(filePath+".txt");
    }

    *realPath = info.canonicalFilePath();

    QFile file(*realPath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -1;
    file.close();

    if(int x = this->openDocument(*realPath, view) != 0)
        return x;

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
