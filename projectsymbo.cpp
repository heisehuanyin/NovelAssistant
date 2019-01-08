#include "projectsymbo.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

using namespace Support;

__project::FileSymbo::FileSymbo(QDomElement dom):
    QStandardItem (dom.attribute("name")),
    domData(dom)
{
    this->setEditable(true);
}

__project::FileSymbo::~FileSymbo(){}

QString __project::FileSymbo::getRefPath()
{
    return this->domData.attribute("ref");
}

QDomElement &__project::FileSymbo::getDom()
{
    return this->domData;
}

__project::GroupSymbo::GroupSymbo(QDomElement dom):
    FileSymbo (dom)
{

}

__project::GroupSymbo::~GroupSymbo()
{

}

ProjectSymbo::ProjectSymbo(QObject *parent):
    QObject (parent),
    structure(new QStandardItemModel),
    domData(new QDomDocument)
{
    auto root = domData->createElement("project");
    this->domData->appendChild(root);
    auto setting = domData->createElement("setting");
    root.appendChild(setting);

    auto content = domData->createElement("content");
    root.appendChild(content);
    content.setAttribute("name", "无名称");
    content.setAttribute("ref", "info.txt");

    this->parseStructureDom(content);

    this->connect(structure,    &QStandardItemModel::itemChanged,
                  this,         &ProjectSymbo::slot_nodeModify);
}

ProjectSymbo::~ProjectSymbo()
{

}

QString ProjectSymbo::config(QString key)
{
    auto setting = this->domData->documentElement()
            .elementsByTagName("setting").at(0);
    auto settings = setting.childNodes();

    for(int i=0; i<settings.size(); ++i){
        auto elm = settings.at(i).toElement();
        if(elm.attribute("key")==key){
            return elm.attribute("value");
        }
    }
    return QString();
}

void ProjectSymbo::setConfig(QString key, QString value)
{

    auto setting = this->domData->documentElement()
            .elementsByTagName("setting").at(0);
    auto settings = setting.childNodes();

    for(int i=0; i<settings.size(); ++i){
        auto elm = settings.at(i).toElement();
        if(elm.attribute("key")==key){
            elm.setAttribute("value",value);
            return;
        }
    }
    auto item = this->domData->createElement("item");
    item.setAttribute("key", key);
    item.setAttribute("value", value);

    setting.appendChild(item);
}

int ProjectSymbo::openProject(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return -1;
    if (!this->domData->setContent(&file)) {
        file.close();
        return -1;
    }
    file.close();

    this->pjtPath = filePath;
    this->parseStructureDom(this->domData
                            ->documentElement()
                            .elementsByTagName("content")
                            .at(0).toElement());
    return 0;
}

QString ProjectSymbo::projectPath()
{
    return this->pjtPath;
}

int ProjectSymbo::save(QString filePath)
{
    if(filePath == QString())
        filePath = this->pjtPath;

    this->pjtPath = filePath;

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return -1;

    QTextStream out(&file);
    out << this->domData->toString(2);

    out.flush();
    file.flush();
    file.close();

    return 0;
}

QStandardItemModel *ProjectSymbo::getStructure()
{
    return this->structure;
}

QStandardItem *ProjectSymbo::newFile(QString name)
{
    auto node = this->domData->createElement("file");
    node.setAttribute("name", name);

    qsrand((unsigned)time(nullptr));
    QString filePath = "file";

    QFileInfo info(filePath+".txt");
    while (info.exists()) {
        filePath +=  QString("%1").arg(qrand());
        info.setFile(filePath+".txt");
    }
    if(!info.exists()){
        QFile file(info.filePath());
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    QString realPath = info.fileName();
    node.setAttribute("ref", realPath);

    return new __project::FileSymbo(node);
}

QStandardItem *ProjectSymbo::newGroup(QString name)
{
    auto node = this->domData->createElement("group");
    node.setAttribute("name", name);

    qsrand((unsigned)time(nullptr));
    QString filePath = "group";

    QFileInfo info(filePath+".txt");
    while (info.exists()) {
        filePath +=  QString("%1").arg(qrand());
        info.setFile(filePath+".txt");
    }
    if(!info.exists()){
        QFile file(info.filePath());
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    QString realPath = info.fileName();
    node.setAttribute("ref", realPath);

    return new __project::GroupSymbo(node);
}

void ProjectSymbo::addItemUnder(QStandardItem *node, QStandardItem *parent, int index)
{
    if(node == nullptr)
        return;
    if(parent != nullptr && typeid (*parent) != typeid (__project::GroupSymbo))
        return;
    if(parent != nullptr && (index < -1 || index > parent->rowCount()))
        return;
    if(parent == nullptr && (index < -1 || index > this->structure->rowCount()))
        return;


    if(parent == nullptr){
        QList<QStandardItem*>list;
        list.append(node);

        auto content = this->domData->documentElement()
                .elementsByTagName("content")
                .at(0).toElement();

        if(index == -1){
            this->structure->appendRow(list);
            content.appendChild(dynamic_cast<__project::FileSymbo*>(node)->getDom());
        }else{
            this->structure->insertRow(index, list);
            auto indexElm = content.childNodes().at(index);
            content.insertBefore(dynamic_cast<__project::FileSymbo*>(node)->getDom(),
                                  indexElm);
        }
    }else{
        auto& domElm = dynamic_cast<__project::GroupSymbo*>(parent)->getDom();
        if(index == -1){
            parent->appendRow(node);
            domElm.appendChild(dynamic_cast<__project::FileSymbo*>(node)->getDom());
        }else{
            parent->insertRow(index, node);
            auto indexElm = domElm.childNodes().at(index);
            domElm.insertBefore(dynamic_cast<__project::FileSymbo*>(node)->getDom(),
                                  indexElm);
        }
    }
}

void ProjectSymbo::removeItemUnder(QStandardItem *node, QStandardItem *parent)
{
    if(node == nullptr)
        return;
    if(parent != nullptr)
        for(int i=0; i<parent->rowCount(); ++i){
            auto item = parent->child(i);
            if(item == node){
                auto &dom = dynamic_cast<__project::GroupSymbo*>(parent)->getDom();
                auto &child = dynamic_cast<__project::FileSymbo*>(node)->getDom();
                QFile file(child.attribute("ref"));
                if(file.exists())
                    file.remove();
                dom.removeChild(child);
                parent->removeRow(i);
            }
        }
    else
        for(int i=0; i<this->structure->rowCount(); ++i){
            auto item = this->structure->item(i);
            if(item == node){
                auto dom = this->domData->documentElement().elementsByTagName("content").at(0);
                auto &child = dynamic_cast<__project::FileSymbo*>(node)->getDom();
                QFile file(child.attribute("ref"));
                if(file.exists())
                    file.remove();
                dom.removeChild(child);
                this->structure->removeRow(i);
            }
        }
}

QString ProjectSymbo::referenceFilePath(QStandardItem *node)
{
    if(node == nullptr)
        return QString();

    return dynamic_cast<__project::FileSymbo*>(node)->getRefPath();
}

void ProjectSymbo::parseStructureDom(QDomElement dom,QStandardItem* group)
{
    auto children = dom.childNodes();

    __project::FileSymbo *xtem = nullptr;
    if(dom.tagName() == "group" || dom.tagName()=="content"){
        xtem = new __project::GroupSymbo(dom);
    }else if (dom.tagName() == "file") {
        xtem = new __project::FileSymbo(dom);
    }
    QList<QStandardItem*>list;
    list.append(xtem);

    if(group==nullptr){
        this->structure->appendRow(list);
    }else{
        group->appendRow(list);
    }


    for(int i=0; i<children.size(); ++i){
        auto child = children.at(i).toElement();
        this->parseStructureDom(child, xtem);
    }
}

void ProjectSymbo::slot_nodeModify(QStandardItem *item)
{
    auto item2 = dynamic_cast<__project::FileSymbo*>(item);
    auto value = item->text();
    QDomElement& dom_ = item2->getDom();
    dom_.setAttribute("name", value);
}

























