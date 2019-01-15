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
#include <QStyle>

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

void EdServer::keywordsFocuse(qlonglong keywords, const QString &type)
{
    this->queryUtility->resetFocusedKeywords(keywords, type);
}

void EdServer::baseEventChanged(int index)
{
    auto event = this->anchor->itemData(index);
    this->queryUtility->resetBaseEvent(event.toLongLong());
}

void EdServer::responseToolsSet(QAction *act)
{
    auto text = act->text();
    if(text == tr("等级")){
        Editor::TypeKindGrade x(this->mainView);
        x.exec();
        return;
    }
    if(text == tr("道具")){
        Editor::Items x(this->mainView);
        x.exec();
    }
    if(text == tr("技能")){
        Editor::Ability x(this->mainView);
        x.exec();
    }
    if(text == tr("地点")){
        Editor::Location x(this->mainView);
        x.exec();
    }
    if(text == tr("角色")){
        Editor::Character x(this->mainView);
        x.exec();
    }
    if(text == tr("事件")){
        Editor::EventNodes x(this->mainView);
        x.exec();
    }
    this->refreshDataStatus();
}

void EdServer::exit()
{
    QApplication::closeAllWindows();
}

EdServer::EdServer(QString title):
    mainView(new FrontEnd),
    pjtSymbo(nullptr),
    dbTool(nullptr),
    toolsBar(new QToolBar(this->mainView)),
    queryUtility(new Component::QueryUtility(this->mainView)),
    anchor(new QComboBox(this->mainView))
{
    this->mainView->setWindowTitle(title);

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

    this->mainView->addToolBar(this->toolsBar);
    this->toolsBar->setMovable(false);
    this->toolsBar->setFloatable(false);
    this->mainView->setQueryUtility(this->queryUtility);

    this->queryUtility->setHidden(true);
    this->anchor->setDisabled(true);
    this->anchor->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
    this->connect(this->anchor, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                  this,         &EdServer::baseEventChanged);
}

EdServer::~EdServer()
{
    delete this->dbTool;
    delete this->pjtSymbo;
    delete this->mainView;
    //安置在mainView上的组件会被一并删除，不需要另行释放
}

void EdServer::openNovelDatabase(QString pjtPath)
{
    QDir::setCurrent(pjtPath);

    if(this->dbTool)
        delete this->dbTool;
    this->dbTool = new Support::DBInitTool;


    this->mainView->setProjectTree(this->pjtSymbo->getStructure());

    this->connect(this->pjtSymbo,   &Support::ProjectSymbo::signal_nodeModified,
                  this,             &EdServer::openItem);
    this->connect(this->pjtSymbo,   &Support::ProjectSymbo::discoveryKeywords,
                  this,             &EdServer::keywordsFocuse);

    this->refreshUIStatus();
    this->refreshDataStatus();
}

void EdServer::refreshUIStatus()
{
    auto style = this->mainView->style();

    auto temp = this->mainView->menuBar();
    auto mbar = new QMenuBar(this->mainView);

    //Common Menu && MenuItem
    auto m_file = new QMenu(tr("文件"), mbar);
    mbar->addMenu(m_file);
    auto mnficon = style->standardIcon(QStyle::StandardPixmap::SP_DirOpenIcon);
    auto m_open = new QAction(mnficon, tr("打开项目"), m_file);
    m_file->addAction(m_open);
    this->connect(m_open,   &QAction::triggered, this,  &EdServer::openProject);

    auto m_newf = new QAction(tr("新建项目"), m_file);
    this->connect(m_newf,   &QAction::triggered, this,  &EdServer::newProject);
    m_file->addAction(m_newf);

    auto msicon = style->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton);
    auto m_save = new QAction(msicon ,tr("保存所有"),m_file);
    this->connect(m_save,   &QAction::triggered, this,  &EdServer::saveAll);
    m_file->addAction(m_save);
    m_file->addSeparator();

    auto mcpicon = style->standardIcon(QStyle::StandardPixmap::SP_DialogCancelButton);
    auto m_clsp = new QAction(mcpicon, tr("关闭项目"), m_file);
    m_file->addAction(m_clsp);
    this->connect(m_clsp,   &QAction::triggered, this,  &EdServer::closeProject);
    m_file->addSeparator();

    QAction* _exit = new QAction(tr("退出"), m_file);
    this->connect(_exit,    &QAction::triggered, this,  &EdServer::exit);
    m_file->addAction(_exit);




    //Custom Menu && MenuItem
    if(this->pjtSymbo != nullptr){
        QMenu* _tools = new QMenu(tr("编辑工具"),mbar);
        mbar->addMenu(_tools);
        auto levelE = new QAction(tr("等级"),_tools);
        _tools->addAction(levelE);
        auto propE = new QAction(tr("道具"),_tools);
        _tools->addAction(propE);
        auto skillE = new QAction(tr("技能"),_tools);
        _tools->addAction(skillE);
        auto locationE = new QAction(tr("地点"),_tools);
        _tools->addAction(locationE);
        auto characterE = new QAction(tr("角色"),_tools);
        _tools->addAction(characterE);
        auto ev_nodeE = new QAction(tr("事件"), _tools);
        _tools->addAction(ev_nodeE);
        this->connect(_tools, &QMenu::triggered,
                      this,   &EdServer::responseToolsSet);
    }

    auto acts = this->toolsBar->actions();
    if(acts.size() ==0){
        this->toolsBar->setIconSize(QSize(20,20));
        this->connect(this->toolsBar->addAction(mnficon, "打开项目"),   &QAction::triggered,
                      this,                                            &EdServer::openProject);
        this->connect(this->toolsBar->addAction(msicon, "保存"),    &QAction::triggered,
                      this,                                         &EdServer::saveAll);
        this->connect(this->toolsBar->addAction(mcpicon, "关闭项目"),&QAction::triggered,
                      this,                                         &EdServer::closeProject);
        this->toolsBar->addSeparator();

        this->toolsBar->addAction(tr("等级"));
        this->toolsBar->addAction(tr("道具"));
        this->toolsBar->addAction(tr("技能"));
        this->toolsBar->addAction(tr("地点"));
        this->toolsBar->addAction(tr("角色"));
        this->toolsBar->addAction(tr("事件"));
        this->toolsBar->addSeparator();

        this->connect(this->toolsBar,  &QToolBar::actionTriggered,
                      this,            &EdServer::responseToolsSet);
        this->toolsBar->addWidget(new QLabel(tr("锚定事件：")));
        this->toolsBar->addWidget(this->anchor);
    }
    //项目未打开
    if(this->pjtSymbo==nullptr){
        m_save->setEnabled(false);
        m_clsp->setEnabled(false);
        auto actions = this->toolsBar->actions();
        foreach(auto item, actions){
            if(item->text() == "打开项目")
                item->setEnabled(true);
            else
                item->setEnabled(false);
        }

        this->queryUtility->hide();
        this->anchor->setEnabled(false);
    }else{
        m_open->setEnabled(false);
        m_newf->setEnabled(false);
        auto actions = this->toolsBar->actions();
        foreach(auto item,actions){
            if(item->text() == "打开项目")
                item->setEnabled(false);
            else
                item->setEnabled(true);
        }

        this->queryUtility->setHidden(false);
        this->anchor->setEnabled(true);
    }

    this->mainView->setMenuBar(mbar);
    delete temp;

    this->mainView->setHidden(true);
    this->mainView->setHidden(false);
}

void EdServer::refreshDataStatus()
{
    if(!this->pjtSymbo){
        return;
    }
    this->pjtSymbo->refreshHighlighterDataSource();

    auto value = this->anchor->currentData();
    QString ev_name = QString();
    this->anchor->clear();

    QSqlQuery q;
    if(!q.exec("select event_name,"
               "node_name, "
               "ev_node_id "
               "from table_eventnodebasic;"))
        qDebug() << q.lastError();
    while (q.next()) {
        auto vx = q.value(2);
        auto name = q.value(0).toString()+":"+q.value(1).toString();

        this->anchor->addItem(name, vx);
        if(vx == value)
            ev_name = name;
    }
    if(ev_name != QString())
        this->anchor->setCurrentText(ev_name);

    this->queryUtility->reset();
}

void EdServer::openGraphicsModel()
{
    this->mainView->openEmptyWindow();
    this->refreshUIStatus();

    this->mainView->setFocus();
}
