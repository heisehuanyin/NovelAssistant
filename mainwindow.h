#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>
#include <QTableWidget>
#include <QToolBar>
#include <QToolBox>
#include <QTreeView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QMenuBar *const menubar;
    QToolBar *const toolsbar;
    QSplitter *const mainSplit;
    QToolBox *const pjtSelect;
    QTabWidget *const editStack;
    //QScrollArea *const rightView;
    QSplitter *const rightSplit;

private slots:
    void exit();
    void slot_ResponseToolsAct(QAction* act);
};

#endif // MAINWINDOW_H
