#include "projectsymbo.h"
#include "syntaxhightlighter.h"

#include <QFile>
#include <QFileInfo>
#include <QStyle>
#include <QTextStream>
#include <QWidget>


using namespace Support;

__projectsymbo::FileSymbo::FileSymbo(QDomElement dom):
    QStandardItem (dom.attribute("name")),
    domData(dom)
{
    this->setEditable(true);
    auto icon = QWidget().style()->standardIcon(QStyle::StandardPixmap::SP_FileIcon);
    this->setIcon(icon);
}

__projectsymbo::FileSymbo::~FileSymbo(){}

QString __projectsymbo::FileSymbo::getRefPath()
{
    return this->domData.attribute("ref");
}

QDomElement &__projectsymbo::FileSymbo::getDom()
{
    return this->domData;
}


__projectsymbo::GroupSymbo::GroupSymbo(QDomElement dom):
    FileSymbo (dom)
{
    auto icon = QWidget().style()->standardIcon(QStyle::StandardPixmap::SP_DirIcon);
    this->setIcon(icon);
}

__projectsymbo::GroupSymbo::~GroupSymbo()
{

}

ProjectSymbo::ProjectSymbo(QObject *parent):
    __projectsymbo::DocManager (new __syntaxhighlighter::DataSource, parent),
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
    this->saveProject();

    delete this->domData;
    delete this->structure;
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

void ProjectSymbo::refreshHighlighterDataSource()
{
    this->getDataSource().refreshKeywordsSet();
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

    this->structure->clear();
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

void ProjectSymbo::openDocument(const QModelIndex &index, QString &title, QTextEdit **view)
{
    if(!index.isValid())
        return;

    auto item = this->structure->itemFromIndex(index);
    auto xitem = dynamic_cast<__projectsymbo::FileSymbo*>(item);

    title = xitem->text();
    this->list.insert(index, xitem->getRefPath());
    this->DocManager::openDocument(xitem->getRefPath(),view);
}

void ProjectSymbo::closeDocument(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto item = this->structure->itemFromIndex(index);
    auto xitem = dynamic_cast<__projectsymbo::FileSymbo*>(item);

    this->list.remove(index);
    this->saveDocument(xitem->getRefPath());
    this->closeDocumentWithoutSave(xitem->getRefPath());
}

QModelIndex ProjectSymbo::getIndexofDocumentView(QTextEdit *view)
{
    auto path = this->returnDocpath(view);
    return this->list.key(path);
}

int ProjectSymbo::saveProject(QString filePath)
{
    if(filePath == QString())
        filePath = this->pjtPath;

    this->pjtPath = filePath;

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return -1;

    QTextStream out(&file);
    this->domData->save(out, 2);

    out.flush();
    file.flush();
    file.close();

    this->saveAllActived();

    return 0;
}

QStandardItemModel *ProjectSymbo::getStructure()
{
    return this->structure;
}

QModelIndex ProjectSymbo::newChildFile(QString name, const QModelIndex &parent)
{
    if(!parent.isValid())
        return QModelIndex();

    auto newf = this->newChildFile(name);
    auto prnt = this->structure->itemFromIndex(parent);
    this->appendItemUnder(newf, prnt);

    return newf->index();
}

QModelIndex ProjectSymbo::newChildGroup(QString name, const QModelIndex &parent)
{
    if(!parent.isValid())
        return QModelIndex();

    auto newg = this->newChildGroup(name);
    auto prnt = this->structure->itemFromIndex(parent);
    this->appendItemUnder(newg, prnt);

    return newg->index();
}

void ProjectSymbo::removeNode(const QModelIndex &target)
{
    if(!target.isValid())
        return;
    auto parent = target.parent();
    //不可删除根节点
    if(!parent.isValid())
        return;

    auto item = this->structure->itemFromIndex(target);
    auto prnt = this->structure->itemFromIndex(parent);

    this->removeItemUnder(item, prnt);
}

void ProjectSymbo::moveNodeTo(const QModelIndex &from, const QModelIndex &to)
{
    if(!from.isValid())
        return;

    auto _1prnt = from.parent();
    auto _2prnt = to.parent();

    //不可移动根节点  ||  不可替换根节点:根节点不允许存在sibling节点
    if((!_1prnt.isValid()) || (!_2prnt.isValid()))
        return;

    auto prnt2 = this->structure->itemFromIndex(_2prnt);
    //目标位置非法
    if(to.row() > prnt2->rowCount())
        return;

    auto node = this->structure->itemFromIndex(_1prnt)->takeRow(from.row()).at(0);
    int rows = prnt2->rowCount();
    if(to.row() == rows){
        this->appendItemUnder(node, prnt2);
    }else{
        this->insertItemUnder(node, prnt2, to.row());
    }
}

QStandardItem *ProjectSymbo::newChildFile(QString name)
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

    return new __projectsymbo::FileSymbo(node);
}

QStandardItem *ProjectSymbo::newChildGroup(QString name)
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

    return new __projectsymbo::GroupSymbo(node);
}

void ProjectSymbo::insertItemUnder(QStandardItem *node, QStandardItem *parent, int index)
{
    if(node == nullptr || parent==nullptr)
        return;
    if(parent != nullptr && typeid (*parent) != typeid (__projectsymbo::GroupSymbo))
        return;
    //索引必须指向确定项，以便插入到指定项目之前
    if(index < 0 || index >= parent->rowCount())
        return;

    auto& domElm = dynamic_cast<__projectsymbo::GroupSymbo*>(parent)->getDom();
    parent->insertRow(index, node);
    auto indexElm = domElm.childNodes().at(index);
    domElm.insertBefore(dynamic_cast<__projectsymbo::FileSymbo*>(node)->getDom(),indexElm);
}

void ProjectSymbo::appendItemUnder(QStandardItem *node, QStandardItem *parent)
{
    auto& domElm = dynamic_cast<__projectsymbo::GroupSymbo*>(parent)->getDom();
    parent->appendRow(node);
    domElm.appendChild(dynamic_cast<__projectsymbo::FileSymbo*>(node)->getDom());
}

void ProjectSymbo::removeItemUnder(QStandardItem *node, QStandardItem *parent)
{
    if(node == nullptr || parent == nullptr)
        return;

    for(int i=0; i<node->rowCount(); ++i){
        auto item = node->child(i);
        this->removeItemUnder(item, node);
    }

    for(int i=0; i<parent->rowCount(); ++i){
        auto item = parent->child(i);
        if(item == node){
            auto &dom = dynamic_cast<__projectsymbo::GroupSymbo*>(parent)->getDom();
            auto &child = dynamic_cast<__projectsymbo::FileSymbo*>(node)->getDom();
            auto filePath = child.attribute("ref");
            if(this->isActived(filePath)){
                this->closeDocumentWithoutSave(filePath);
            }
            QFile file(filePath);
            if(file.exists())
                file.remove();
            dom.removeChild(child);
            parent->removeRow(i);
        }
    }
}

void ProjectSymbo::parseStructureDom(QDomElement dom,QStandardItem* group)
{
    auto children = dom.childNodes();

    __projectsymbo::FileSymbo *xtem = nullptr;
    if(dom.tagName() == "group" || dom.tagName()=="content"){
        xtem = new __projectsymbo::GroupSymbo(dom);
    }else if (dom.tagName() == "file") {
        xtem = new __projectsymbo::FileSymbo(dom);
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
    auto item2 = dynamic_cast<__projectsymbo::FileSymbo*>(item);
    auto value = item->text();
    QDomElement& dom_ = item2->getDom();
    dom_.setAttribute("name", value);

    emit this->signal_nodeModified(item->index());
}
























