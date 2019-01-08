#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QTreeView>
namespace UIComp {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openEmptyWin();
    void addDocumentView(QTextEdit *view);

private:
    QToolBar *const toolsbar;
    QSplitter *const mainSplit;
    QTreeView *const pjtSelect;
    QTabWidget *const editStack;
    QSplitter *const rightSplit;

};
}

#endif // MAINWINDOW_H
