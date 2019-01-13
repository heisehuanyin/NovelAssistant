#include "edserver.h"
#include "frontend.h"
#include "projectsymbo.h"
#include "dbinittool.h"
#include "location.h"
#include "items.h"
#include "ability.h"
#include "typekindgrade.h"
#include "character.h"
#include "queryutility.h"
#include "frontend.h"
#include "syntaxhightlighter.h"
#include "eventnodes.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

void EdServer::newProject()
{
    QString fileName = QFileDialog::getSaveFileName(
                this->mainView, tr("新建项目"),
                QDir::currentPath(), tr("WsNovel (*.wsnovel )"));

    if(fileName == QString())
        return;
    QFileInfo info(fileName);
    if(!info.exists()){
        QFile file(info.filePath());
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    this->pjtSymbo = new Support::ProjectSymbo(this);
    this->pjtSymbo->saveProject(info.filePath());

    this->openNovelDatabase(info.path());
}

void EdServer::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(
                this->mainView, tr("打开项目"), QDir::currentPath(),
                tr("WsNovel (*.wsnovel )"));
    if(fileName == QString())
        return;

    QFileInfo info(fileName);
    this->pjtSymbo = new Support::ProjectSymbo(this);
    this->pjtSymbo->openProject(info.filePath());

    this->openNovelDatabase(info.path());
}

void EdServer::closeProject()
{
    delete this->pjtSymbo;
    this->pjtSymbo = nullptr;

    this->refreshUIStatus();
    this->queryUtility->hide();

    this->mainView->addDocumentView("", nullptr);
}

void EdServer::saveAll()
{
    if(this->pjtSymbo)
        this->pjtSymbo->saveProject();
}

void EdServer::openItem(const QModelIndex &index)
{
    QTextEdit *edit = nullptr;
    QString title;

    this->pjtSymbo->openDocument(index, title, &edit);
    this->mainView->addDocumentView(title, edit);
}

void EdServer::closeTargetView(QTextEdit *view)
{
    auto index = this->pjtSymbo->getIndexofDocumentView(view);
    this->pjtSymbo->closeDocument(index);

    this->mainView->addDocumentView("", nullptr);
}

void EdServer::newFileNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto node = this->pjtSymbo->getStructure()->itemFromIndex(index);
    if(typeid (*node) == typeid (Support::__projectsymbo::FileSymbo)){
        auto newIndex = this->pjtSymbo->newChildFile(tr("新章节"), index.parent());
        this->pjtSymbo->moveNodeTo(newIndex, index.sibling(index.row()+1, index.column()));
    }else{
        this->pjtSymbo->newChildFile(tr("新章节"), index);
    }
}

void EdServer::newGroupNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    auto node = this->pjtSymbo->getStructure()->itemFromIndex(index);
    if(typeid (*node) == typeid (Support::__projectsymbo::FileSymbo)){
        auto newIndex = this->pjtSymbo->newChildGroup(tr("新集合"), index.parent());
        this->pjtSymbo->moveNodeTo(newIndex, index.sibling(index.row()+1, index.column()));
    }else{
        this->pjtSymbo->newChildGroup(tr("新集合"), index);
    }
}

void EdServer::removeNode(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    this->pjtSymbo->removeNode(index);
}

void EdServer::nodeMove(const QModelIndex &from, const QModelIndex &to)
{
    this->pjtSymbo->moveNodeTo(from, to);
}

void EdServer::keywordsFocuse(qlonglong keywords, QString &type)
{
    qDebug() << keywords << type;
}

void EdServer::responseToolsSet(QAction *act)
{
    auto text = act->text();
    if(text == tr("等级编辑")){
        Editor::TypeKindGrade x(this->mainView);
        x.exec();
        return;
    }
    if(text == tr("道具编辑")){
        Editor::Items x(this->mainView);
        x.exec();
    }
    if(text == tr("技能编辑")){
        Editor::Ability x(this->mainView);
        x.exec();
    }
    if(text == tr("地点编辑")){
        Editor::Location x(this->mainView);
        x.exec();
    }
    if(text == tr("人物编辑")){
        Editor::Character x(this->mainView);
        x.exec();
    }
    if(text == tr("事件编辑")){
        Editor::EventNodes x(this->mainView);
        x.exec();
    }
    if(this->pjtSymbo)
        this->pjtSymbo->refreshHighlighterDataSource();
}

void EdServer::exit()
{
    QApplication::closeAllWindows();
}

EdServer::EdServer(QString title):
    mainView(new FrontEnd),
    pjtSymbo(nullptr),
    //toolsBar(new QToolBar(this->mainView)),
    queryUtility(new Component::QueryUtility(this->mainView))
{
    this->mainView->setWindowTitle(title);
    //this->mainView->addToolBar(this->toolsBar);
    this->mainView->setQueryUtility(this->queryUtility);
    this->queryUtility->hide();
}

EdServer::~EdServer()
{
    delete this->dbTool;
    delete this->pjtSymbo;
    delete this->mainView;
}

void EdServer::openNovelDatabase(QString pjtPath)
{
    QDir::setCurrent(pjtPath);
    this->dbTool = new Support::DBInitTool;
    this->mainView->setProjectTree(this->pjtSymbo->getStructure());

    this->connect(this->mainView,   &FrontEnd::signal_openProjectItem,
                  this,             &EdServer::openItem);
    this->connect(this->mainView,   &FrontEnd::signal_closeTargetView,
                  this,             &EdServer::closeTargetView);
    this->connect(this->mainView,   &FrontEnd::signal_newFile,
                  this,             &EdServer::newFileNode);
    this->connect(this->mainView,   &FrontEnd::signal_newGroup,
                  this,             &EdServer::newGroupNode);
    this->connect(this->mainView,   &FrontEnd::signal_removeNode,
                  this,             &EdServer::removeNode);
    this->connect(this->mainView,   &FrontEnd::signal_moveNode,
                  this,             &EdServer::nodeMove);

    this->connect(this->pjtSymbo,   &Support::ProjectSymbo::signal_nodeModified,
                  this,             &EdServer::openItem);

    this->queryUtility->setVisible(true);

    this->refreshUIStatus();
    this->refreshDataStatus();
}

void EdServer::refreshUIStatus()
{
    auto temp = this->mainView->menuBar();
    auto mbar = new QMenuBar();

    auto m_file = new QMenu(tr("文件"), mbar);
    mbar->addMenu(m_file);
    auto m_newf = new QAction(tr("打开项目"), m_file);
    m_file->addAction(m_newf);
    this->connect(m_newf,   &QAction::triggered, this,  &EdServer::openProject);
    auto m_opnf = new QAction(tr("新建项目"), m_file);
    this->connect(m_opnf,   &QAction::triggered, this,  &EdServer::newProject);
    m_file->addAction(m_opnf);
    auto m_save = new QAction(tr("保存所有"),m_file);
    this->connect(m_save,   &QAction::triggered, this,  &EdServer::saveAll);
    m_file->addAction(m_save);
    m_file->addSeparator();
    auto m_clsp = new QAction(tr("关闭项目"), m_file);
    m_file->addAction(m_clsp);
    this->connect(m_clsp,   &QAction::triggered, this,  &EdServer::closeProject);
    m_file->addSeparator();
    QAction* _exit = new QAction(tr("退出"), m_file);
    this->connect(_exit,    &QAction::triggered, this,  &EdServer::exit);
    m_file->addAction(_exit);

    //项目未打开
    if(this->pjtSymbo==nullptr){
        m_save->setEnabled(false);
        m_clsp->setEnabled(false);
    }else{
        m_newf->setEnabled(false);
        m_opnf->setEnabled(false);
    }

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
                      this,   &EdServer::responseToolsSet);
    }
    this->mainView->setMenuBar(mbar);
    delete temp;
    this->mainView->setHidden(true);
    this->mainView->setHidden(false);
}

void EdServer::refreshDataStatus()
{
    if(this->pjtSymbo)
        this->pjtSymbo->refreshHighlighterDataSource();
}

void EdServer::openGraphicsModel()
{
    this->refreshUIStatus();
    this->mainView->openEmptyWindow();

    this->mainView->setFocus();
}
