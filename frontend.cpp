#include "frontend.h"
#include "character.h"
#include "typekindgrade.h"
#include "location.h"
#include "items.h"
#include "ability.h"
#include "eventnodes.h"
#include "queryutility.h"

#include <QApplication>
#include <QStyle>
#include <QTextEdit>

using namespace Component;

FrontEnd::FrontEnd(QWidget *parent)
    : QMainWindow(parent),
      baseFrame(new QSplitter(Qt::Horizontal, this)),
      pjtStructure(new QTreeView(this)),
      contentStack(new QTabWidget(this)),
      welcome(this->generateWelcomePanel())
{
    this->setCentralWidget(this->baseFrame);
    this->baseFrame->addWidget(pjtStructure);
    this->pjtStructure->setHeaderHidden(true);

    this->baseFrame->addWidget(contentStack);
    this->contentStack->setTabsClosable(true);

    this->connect(this->contentStack,   &QTabWidget::tabCloseRequested,
                  this,                 &FrontEnd::slot_receptCloseDocument);

    this->pjtStructure->setContextMenuPolicy(Qt::CustomContextMenu);
    this->connect(this->pjtStructure,   &QTreeView::customContextMenuRequested,
                  this,                 &FrontEnd::slot_displayPopupMenu);

    this->baseFrame->setStretchFactor(0,0);
    this->baseFrame->setStretchFactor(1,1);
    this->baseFrame->setStretchFactor(2,0);

    this->contentStack->addTab(this->welcome, "Welcome");
    //setUnifiedTitleAndToolBarOnMac(true);
}

FrontEnd::~FrontEnd()
{

}

void FrontEnd::openEmptyWindow(){
    this->show();
}

void FrontEnd::addDocumentView(QString title, QWidget *view)
{
    this->refreshTabviewStatus();
    if(!view) return;

    if(this->contentStack->count() == 1 &&
            this->contentStack->widget(0) == this->welcome){
        this->contentStack->removeTab(0);
    }

    this->contentStack->addTab(view, title);
    this->contentStack->setCurrentWidget(view);
}

void FrontEnd::setProjectTree(QStandardItemModel *model)
{
    auto xtemp = this->pjtStructure->selectionModel();
    this->pjtStructure->setModel(model);
    delete xtemp;
    this->connect(this->pjtStructure,    &QTreeView::clicked,
                  this,     &FrontEnd::slot_receptOpenDocument);
}

void FrontEnd::setQueryUtility(QueryUtility *utility)
{
    this->baseFrame->addWidget(utility);
}

void FrontEnd::refreshTabviewStatus()
{
    if(this->contentStack->count() < 1){
        this->pjtStructure->clearSelection();
        this->contentStack->addTab(this->welcome, "Welcome");
    }
}

QWidget *FrontEnd::generateWelcomePanel()
{
    auto welcome(new QWidget(this));
    auto layout(new QHBoxLayout(welcome));
    welcome->setLayout(layout);
    auto words(new QLabel(this));
    words->setText("<body>"
                   "<h1>简要说明</h1>"
                   "<hr />"
                   "<p>本软件基于项目操作，用户能够打开、关闭、保存指定项目（保存操作属于全局保存）。</p>"
                   "<p>可以进行修改，增加、删除、移动、修改节点，每个节点关联一个文件用于存储内容。</p>"
                   "<p>为了支持长篇作品创作，内置了基于时间线的事件管理系统，对复数的道具、技能、人际关系等统一管理。</p>"
                   "<ul>"
                   "<li><b>文件 》 打开项目</b>：打开系统文件选框选择指定项目进行打开操作。"
                   "<li><b>文件 》 新建项目</b>：打开系统选择框，选择指定位置新建项目。"
                   "<li><b>文件 》 关闭项目</b>：关闭当前正在编辑的项目。"
                   "<li><b>文件 》 保存项目</b>：保存当前项目文件和所有在编辑内容。"
                   "<li><b>文件 》 退出</b>：正常退出，关闭当前窗口。项目和所有编辑内容自动保存。"
                   "</ul>"
                   "</body>");
    words->setWordWrap(true);

    layout->addWidget(words);
    return welcome;
}

void FrontEnd::slot_receptOpenDocument(const QModelIndex &index)
{
    emit this->signal_openProjectItem(index);
}

void FrontEnd::slot_receptCloseDocument(int index)
{
    auto widget = dynamic_cast<QTextEdit*>(this->contentStack->widget(index));
    emit this->signal_closeTargetView(widget);

    this->refreshTabviewStatus();
}

void FrontEnd::slot_displayPopupMenu(const QPoint &point)
{
    auto index = this->pjtStructure->indexAt(point);
    if(!index.isValid())
        return;
    auto p = index.parent();
    auto model = index.model();
    this->pjtStructure->setCurrentIndex(index);

    QMenu m;

    auto dirIcon = m.style()->standardIcon(QStyle::StandardPixmap::SP_DirIcon);
    auto fileIcon = m.style()->standardIcon(QStyle::StandardPixmap::SP_FileIcon);
    auto upIcon = m.style()->standardIcon(QStyle::StandardPixmap::SP_ArrowUp);
    auto downIcon = m.style()->standardIcon(QStyle::StandardPixmap::SP_ArrowDown);
    auto delIcon = m.style()->standardIcon(QStyle::StandardPixmap::SP_BrowserStop);


    auto item(new QAction(fileIcon, tr("新建文件"), &m));
    m.addAction(item);
    item = new QAction(dirIcon ,tr("新建集合"), &m);
    m.addAction(item);

    m.addSeparator();
    item = new QAction(tr("编辑"), &m);
    m.addAction(item);

    m.addSeparator();
    item = new QAction(upIcon, tr("节点上移"), &m);
    m.addAction(item);
    if(index.row() == 0)
        item->setEnabled(false);
    item = new QAction(downIcon, tr("节点下移"), &m);
    m.addAction(item);
    if((!p.isValid()) || index.row() == model->rowCount(p)-1)
        item->setEnabled(false);

    m.addSeparator();
    item = new QAction(delIcon, tr("删除节点"), &m);
    m.addAction(item);
    if(!p.isValid())
        item->setEnabled(false);
    this->connect(&m,   &QMenu::triggered,
                  this, &FrontEnd::slot_ProjectManage);

    m.exec(this->mapToGlobal(point));
}

void FrontEnd::slot_ProjectManage(QAction *act)
{
    auto index = this->pjtStructure->currentIndex();
    if(!index.isValid())
        return;

    if(act->text() == tr("新建文件"))
        emit this->signal_newFile(index);
    if(act->text() == tr("新建集合"))
        emit this->signal_newGroup(index);
    if(act->text() == tr("节点上移"))
        emit this->signal_moveNode(index, index.sibling(index.row()-1, index.column()));
    if(act->text() == tr("节点下移"))
        emit this->signal_moveNode(index, index.sibling(index.row()+1, index.column()));
    if(act->text() == tr("删除节点"))
        emit this->signal_removeNode(index);
    if(act->text() == tr("编辑"))
        this->pjtStructure->edit(index);
}
