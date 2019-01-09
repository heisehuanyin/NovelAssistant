#include "edserver.h"
#include "mainwindow.h"
#include "docmanager.h"
#include "projectsymbo.h"
#include "sqlport.h"
#include "eventnodeedit.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"
#include "components.h"
#include "characteredit.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

void EdServer::slot_newProject()
{
    QString fileName = QFileDialog::getSaveFileName(this->mainView, tr("新建项目"),
                                                    QDir::currentPath(), tr("WsNovel (*.wsnovel )"));
    if(fileName == QString())
        return;
    QFileInfo info(fileName);
    if(!info.exists()){
        QFile file(info.filePath());
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    this->pjtSymbo = new Support::ProjectSymbo;
    this->pjtSymbo->save(info.filePath());

    this->openNovelDatabase(info.path());
}

void EdServer::slot_openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this->mainView, tr("打开项目"), QDir::currentPath()
                                                    , tr("WsNovel (*.wsnovel )"));
    if(fileName == QString())
        return;

    QFileInfo info(fileName);
    this->pjtSymbo = new Support::ProjectSymbo;
    this->pjtSymbo->openProject(info.filePath());

    this->openNovelDatabase(info.path());
}

void EdServer::slot_closeProject()
{
    this->manager->saveAll();
    auto list = this->manager->getAllDocuments();
    for(int i=0; i<list.size(); ++i){
        this->manager->closeDocumentWithoutSave(list.takeAt(i));
    }
    if(this->pjtSymbo->projectPath() == QString()){
        auto filename = QFileDialog::getSaveFileName(this->mainView, tr("保存"), QDir::currentPath(), "WsNovel (*.wsnovel )");
        this->pjtSymbo->save(filename);
    }
    else{
        this->pjtSymbo->save();
    }

    delete this->pjtSymbo;
    this->pjtSymbo = nullptr;

    this->refreshUIStatus();
}

void EdServer::slot_saveProject()
{
    this->manager->saveAll();
    if(this->pjtSymbo->projectPath() == QString()){
        auto filename = QFileDialog::getSaveFileName(this->mainView, tr("保存"), QDir::currentPath(), "WsNovel (*.wsnovel )");
        this->pjtSymbo->save(filename);
    }
    else{
        this->pjtSymbo->save();
    }
}

void EdServer::slot_openWithinProject(const QModelIndex &index)
{
    if(!index.isValid() || this->pjtSymbo == nullptr)
        return;

    auto model = this->pjtSymbo->getStructure();
    auto item = model->itemFromIndex(index);
    auto path = this->pjtSymbo->referenceFilePath(item);

    QTextEdit *edit = nullptr;
    this->manager->openDocument(path, &edit);
    this->mainView->addDocumentView(item->text(), edit);
}

void EdServer::slot_closeTargetView(QTextEdit *view)
{
    auto label = this->manager->returnDocpath(view);
    if(label == QString())
        return;

    this->manager->saveDocument(label);
    this->manager->closeDocumentWithoutSave(label);
}

void EdServer::slot_newFileNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto node = this->pjtSymbo->getStructure()->itemFromIndex(index);
    if(typeid (*node) == typeid (Support::__project::FileSymbo)){
        auto newIndex = this->pjtSymbo->newFile(tr("新章节"), index.parent());
        this->pjtSymbo->moveNodeTo(newIndex, index.sibling(index.row()+1, index.column()));
    }else{
        this->pjtSymbo->newFile(tr("新章节"), index);
    }
}

void EdServer::slot_newGroupNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto node = this->pjtSymbo->getStructure()->itemFromIndex(index);
    if(typeid (*node) == typeid (Support::__project::FileSymbo)){
        auto newIndex = this->pjtSymbo->newGroup(tr("新集合"), index.parent());
        this->pjtSymbo->moveNodeTo(newIndex, index.sibling(index.row()+1, index.column()));
    }else{
        this->pjtSymbo->newGroup(tr("新集合"), index);
    }
}

void EdServer::slot_removeNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    this->pjtSymbo->removeNode(index);
}

void EdServer::slot_NodeMove(const QModelIndex &from, const QModelIndex &to)
{
    this->pjtSymbo->moveNodeTo(from, to);
}

void EdServer::slot_ResponseToolsAct(QAction *act)
{
    auto text = act->text();
    if(text == tr("等级编辑")){
        UIComp::GTME x(this->mainView);
        x.exec();
    }
    if(text == tr("道具编辑")){
        UIComp::PropEdit x(this->mainView);
        x.exec();
    }
    if(text == tr("技能编辑")){
        UIComp::SkillEdit x(this->mainView);
        x.exec();
    }
    if(text == tr("地点编辑")){
        UIComp::LocationEdit x(this->mainView);
        x.exec();
    }
    if(text == tr("人物编辑")){
        UIComp::CharacterEdit x(this->mainView);
        x.exec();
    }
    if(text == tr("事件编辑")){
        UIComp::EventnodeEdit x(this->mainView);
        x.exec();
    }
}

void EdServer::exit()
{
    QApplication::closeAllWindows();
}

EdServer::EdServer(QString title):
    mainView(new UIComp::MainWindow),
    manager(new Support::DocManager),
    pjtSymbo(nullptr)
{
    this->mainView->setWindowTitle(title);
}

EdServer::~EdServer()
{
    delete this->dbTool;
    delete this->manager;
    delete this->pjtSymbo;
    delete this->mainView;
}

void EdServer::openNovelDatabase(QString pjtPath)
{
    QDir::setCurrent(pjtPath);
    this->dbTool = new Support::DBInitTool;

    this->refreshUIStatus();
    this->mainView->setProjectTree(this->pjtSymbo->getStructure());

    this->connect(this->mainView,   &UIComp::MainWindow::signal_openWithinProject,
                  this,             &EdServer::slot_openWithinProject);
    this->connect(this->mainView,   &UIComp::MainWindow::signal_closeTargetView,
                  this,             &EdServer::slot_closeTargetView);
    this->connect(this->mainView,   &UIComp::MainWindow::signal_newFile,
                  this,             &EdServer::slot_newFileNode);
    this->connect(this->mainView,   &UIComp::MainWindow::signal_newGroup,
                  this,             &EdServer::slot_newGroupNode);
    this->connect(this->mainView,   &UIComp::MainWindow::signal_removeNode,
                  this,             &EdServer::slot_removeNode);
    this->connect(this->mainView,   &UIComp::MainWindow::signal_moveNode,
                  this,             &EdServer::slot_NodeMove);
}

void EdServer::refreshUIStatus()
{
    auto mbar = this->mainView->menuBar();
    mbar->clear();

    auto m_file = new QMenu(tr("文件"), mbar);
    mbar->addMenu(m_file);
    auto m_newf = new QAction(tr("打开项目"), m_file);
    m_file->addAction(m_newf);
    this->connect(m_newf,   &QAction::triggered,
                  this,     &EdServer::slot_openProject);
    auto m_opnf = new QAction(tr("新建项目"), m_file);
    this->connect(m_opnf,   &QAction::triggered,
                  this,     &EdServer::slot_newProject);
    m_file->addAction(m_opnf);
    auto m_save = new QAction(tr("保存所有"),m_file);
    this->connect(m_save,   &QAction::triggered,
                  this,    &EdServer::slot_saveProject);
    m_file->addAction(m_save);
    m_file->addSeparator();
    auto m_clsp = new QAction(tr("关闭项目"), m_file);
    m_file->addAction(m_clsp);
    this->connect(m_clsp,   &QAction::triggered,
                  this,     &EdServer::slot_closeProject);
    m_file->addSeparator();
    QAction* _exit = new QAction(tr("退出"), m_file);
    this->connect(_exit, &QAction::triggered, this, &EdServer::exit);
    m_file->addAction(_exit);

    if(this->pjtSymbo != nullptr){
        QMenu* _tools = new QMenu(tr("工具"),mbar);
        mbar->addMenu(_tools);
        auto levelE = new QAction(tr("等级编辑"),_tools);
        _tools->addAction(levelE);
        auto propE = new QAction(tr("道具编辑"),_tools);
        _tools->addAction(propE);
        auto skillE = new QAction(tr("技能编辑"),_tools);
        _tools->addAction(skillE);
        auto locationE = new QAction(tr("地点编辑"),_tools);
        _tools->addAction(locationE);
        auto characterE = new QAction(tr("人物编辑"),_tools);
        _tools->addAction(characterE);
        auto ev_nodeE = new QAction(tr("事件编辑"), _tools);
        _tools->addAction(ev_nodeE);
        this->connect(_tools, &QMenu::triggered,
                      this,   &EdServer::slot_ResponseToolsAct);
    }
    this->mainView->setHidden(true);
    this->mainView->setHidden(false);
}

void EdServer::openGraphicsModel()
{
    this->refreshUIStatus();
    this->mainView->openEmptyWindow();

    this->mainView->setFocus();
}
