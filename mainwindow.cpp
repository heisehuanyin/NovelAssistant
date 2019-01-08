#include "mainwindow.h"

#include <QApplication>
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
      mainSplit(new QSplitter(Qt::Horizontal, this)),
      pjtSelect(new QTreeView(this)),
      editStack(new QTabWidget(this)),
      rightSplit(new QSplitter(Qt::Vertical, this))
{
    this->addToolBar(toolsbar);
    this->setCentralWidget(this->mainSplit);
    this->mainSplit->addWidget(pjtSelect);



    this->mainSplit->addWidget(editStack);
    this->editStack->setTabsClosable(true);
    auto v = new QTextEdit(this);
    editStack->addTab(v, "one");
    v = new QTextEdit(this);
    editStack->addTab(v, "Two");
    v = new QTextEdit(this);
    editStack->addTab(v, "Three");
    v = new QTextEdit(this);
    editStack->addTab(v, "four");



    this->mainSplit->addWidget(rightSplit);
    //this->rightView->setWidget(rightSplit);
    auto x = new QTextEdit(this);
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
    x = new QTextEdit(this);
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
    x = new QTextEdit(this);
    x->setReadOnly(true);
    this->rightSplit->addWidget(x);
}

MainWindow::~MainWindow()
{

}

void MainWindow::openEmptyWin(){
    this->show();
}
