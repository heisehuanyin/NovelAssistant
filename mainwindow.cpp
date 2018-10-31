#include "mainwindow.h"

#include <QApplication>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      menubar(new QMenuBar(this)),
      toolsbar(new QToolBar(this)),
      mainSplit(new QSplitter(Qt::Horizontal, this)),
      pjtSelect(new QToolBox(this)),
      editStack(new QTabWidget(this)),
      //rightView(new QScrollArea(this)),
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


    this->addToolBar(toolsbar);
    this->setCentralWidget(this->mainSplit);
    this->mainSplit->addWidget(pjtSelect);
    auto v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "One");
    v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "Two");
    v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "Three");
    v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "Four");
    v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "Five");
    v = new QTextEdit;
    v->setReadOnly(true);
    pjtSelect->addItem(v, "Six");



    this->mainSplit->addWidget(editStack);
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