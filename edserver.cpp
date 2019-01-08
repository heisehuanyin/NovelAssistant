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

}

Support::DocManager *EdServer::getDocumentManager()
{
    return this->manager;
}

void EdServer::openNovelDatabase(QString pjtPath)
{
    QDir::setCurrent(pjtPath);
    this->dbTool = new Support::DBInitTool;

    this->refreshMenubarStatus();
}

void EdServer::refreshMenubarStatus()
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

        this->mainView->setHidden(true);
        this->mainView->setHidden(false);
    }
}

void EdServer::openGraphicsModel()
{
    this->refreshMenubarStatus();
    this->mainView->openEmptyWin();

    this->mainView->setFocus();
}
