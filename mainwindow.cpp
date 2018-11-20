#include "mainwindow.h"

#include <QApplication>
#include <QTextEdit>
#include "characteredit.h"
#include "components.h"
#include "locationedit.h"
#include "propedit.h"
#include "skilledit.h"
#include "eventnodeedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      menubar(new QMenuBar(this)),
      toolsbar(new QToolBar(this)),
      mainSplit(new QSplitter(Qt::Horizontal, this)),
      pjtSelect(new QTreeView(this)),
      editStack(new QTabWidget(this)),
      rightSplit(new QSplitter(Qt::Vertical, this))
{
    this->setMenuBar(this->menubar);
    //CUSTOM MENUBAR
    QMenu* file = new QMenu(tr("文件"), menubar);
    menubar->addMenu(file);
    QAction * _open = new QAction(tr("打开"), file);
    file->addAction(_open);
    QMenu * _new = new QMenu(tr("新建"), file);
    file->addMenu(_new);
    QAction* _newF = new QAction(tr("文件"), _new);
    _new->addAction(_newF);
    QAction* _newP = new QAction(tr("项目"), _new);
    _new->addAction(_newP);

    file->addSeparator();
    QAction* _exit = new QAction(tr("退出"), file);
    this->connect(_exit, &QAction::triggered, this, &MainWindow::exit);
    file->addAction(_exit);

    QMenu* _tools = new QMenu(tr("工具"),menubar);
    menubar->addMenu(_tools);
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
                  this,   &MainWindow::slot_ResponseToolsAct);

    this->addToolBar(toolsbar);
    this->setCentralWidget(this->mainSplit);
    this->mainSplit->addWidget(pjtSelect);



    this->mainSplit->addWidget(editStack);
    auto v = new QTextEdit;
    v = new QTextEdit;
    v->setReadOnly(true);
    editStack->addTab(v, "one");
    v = new QTextEdit;
    v->setReadOnly(true);
    editStack->addTab(v, "Two");
    v = new QTextEdit;
    v->setReadOnly(true);
    editStack->addTab(v, "Three");
    v = new QTextEdit;
    v->setReadOnly(true);
    editStack->addTab(v, "four");



    this->mainSplit->addWidget(rightSplit);
    //this->rightView->setWidget(rightSplit);
    auto x = new QTextEdit;
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
    x = new QTextEdit;
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
    x = new QTextEdit;
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
}

MainWindow::~MainWindow()
{

}

void MainWindow::exit()
{
    auto x = qobject_cast<QApplication*>(QApplication::instance());
    x->closeAllWindows();
}

void MainWindow::slot_ResponseToolsAct(QAction *act)
{
    auto text = act->text();
    if(text == tr("等级编辑")){
        UIComp::GTME x(this);
        x.exec();
    }
    if(text == tr("道具编辑")){
        UIComp::PropEdit x(this);
        x.exec();
    }
    if(text == tr("技能编辑")){
        UIComp::SkillEdit x(this);
        x.exec();
    }
    if(text == tr("地点编辑")){
        UIComp::LocationEdit x(this);
        x.exec();
    }
    if(text == tr("人物编辑")){
        UIComp::CharacterEdit x(this);
        x.exec();
    }
    if(text == tr("事件编辑")){
        UIComp::EventnodeEdit x(this);
        x.exec();
    }
}
