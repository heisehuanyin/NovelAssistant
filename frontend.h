#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QTreeView>

class FrontEnd : public QMainWindow
{
    Q_OBJECT

public:
    FrontEnd(QWidget *parent = nullptr);
    ~FrontEnd();

    void openEmptyWindow();
    void addDocumentView(QString title, QWidget *view);

    void setProjectTree(QStandardItemModel *model);

signals:
    void signal_openWithinProject(const QModelIndex &index);
    void signal_closeTargetView(QTextEdit* ref);

    void signal_newFile(const QModelIndex &index);
    void signal_newGroup(const QModelIndex &index);
    void signal_removeNode(const QModelIndex &index);
    void signal_moveNode(const QModelIndex &from, const QModelIndex &to);

private:
    QToolBar *const toolsbar;
    QSplitter *const baseFrame;
    QTreeView *const pjtStructure;
    QTabWidget *const contentStack;
    QWidget * welcome;
    QSplitter *const rightSplit;

    QWidget* generateWelcomePanel();

private slots:
    void slot_receptOpenDocument(const QModelIndex &index);
    void slot_receptCloseDocument(int index);

    void slot_displayPopupMenu(const QPoint &point);
    void slot_ProjectManage(QAction* act);
};


#endif // MAINWINDOW_H
