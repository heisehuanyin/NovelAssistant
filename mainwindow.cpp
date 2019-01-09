#include "mainwindow.h"

#include <QApplication>
#include <QStyle>
#include <QTextEdit>
#include "characteredit.h"
#include "components.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"
#include "eventnodeedit.h"

using namespace UIComp;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      toolsbar(new QToolBar(this)),
      baseFrame(new QSplitter(Qt::Horizontal, this)),
      pjtStructure(new QTreeView(this)),
      contentStack(new QTabWidget(this)),
      welcome(new QWidget),
      rightSplit(new QSplitter(Qt::Vertical, this))
{
    this->addToolBar(toolsbar);
    this->setCentralWidget(this->baseFrame);
    this->baseFrame->addWidget(pjtStructure);
    this->pjtStructure->setHeaderHidden(true);

    this->baseFrame->addWidget(contentStack);
    this->contentStack->setTabsClosable(true);

    this->baseFrame->addWidget(rightSplit);

    this->connect(this->contentStack,   &QTabWidget::tabCloseRequested,
                  this,                 &MainWindow::slot_receptCloseDocument);

    this->pjtStructure->setContextMenuPolicy(Qt::CustomContextMenu);
    this->connect(this->pjtStructure,   &QTreeView::customContextMenuRequested,
                  this,                 &MainWindow::slot_displayPopupMenu);

    this->baseFrame->setStretchFactor(0,0);
    this->baseFrame->setStretchFactor(1,1);
    this->baseFrame->setStretchFactor(2,0);

    this->welcome = this->generateWelcomePanel();
    this->contentStack->addTab(this->welcome, "Welcome");
}

MainWindow::~MainWindow()
{

}

void MainWindow::openEmptyWindow(){
    this->show();
}

void MainWindow::addDocumentView(QString title, QWidget *view)
{
    this->contentStack->addTab(view, title);
    this->contentStack->setCurrentWidget(view);

    if(this->welcome != nullptr){
        delete this->welcome;
        this->welcome = nullptr;
    }
}

void MainWindow::setProjectTree(QStandardItemModel *model)
{
    auto xtemp = this->pjtStructure->selectionModel();
    this->pjtStructure->setModel(model);
    delete xtemp;
    this->connect(this->pjtStructure,    &QTreeView::clicked,
                  this,     &MainWindow::slot_receptOpenDocument);
}

QWidget *MainWindow::generateWelcomePanel()
{
    return new QWidget;
}

void MainWindow::slot_receptOpenDocument(const QModelIndex &index)
{
    emit this->signal_openWithinProject(index);
}

void MainWindow::slot_receptCloseDocument(int index)
{
    auto widget = dynamic_cast<QTextEdit*>(this->contentStack->widget(index));
    emit this->signal_closeTargetView(widget);
    if(this->contentStack->count() < 1){
        this->pjtStructure->clearSelection();
        this->welcome = this->generateWelcomePanel();
        this->contentStack->addTab(this->welcome, "Welcome");
    }

}

void MainWindow::slot_displayPopupMenu(const QPoint &point)
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
                  this, &MainWindow::slot_ProjectManage);

    m.exec(this->mapToGlobal(point));
}

void MainWindow::slot_ProjectManage(QAction *act)
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
