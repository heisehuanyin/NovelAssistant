#ifndef EDSERVER_H
#define EDSERVER_H

#include <QString>
#include <QObject>
#include <QAction>
#include <QStandardItemModel>
#include <QTextEdit>

namespace Support {
    class DocManager;
    class ProjectSymbo;
    class DBInitTool;
}
namespace UIComp {
    class MainWindow;
}

class EdServer:public QObject
{
    Q_OBJECT
private:
    UIComp::MainWindow *const mainView;
    Support::DocManager*const manager;
    Support::ProjectSymbo* pjtSymbo;
    Support::DBInitTool *dbTool;

    /**
     * @brief 打开小说数据库
     * @param pjtPath 项目文件夹位置
     */
    void openNovelDatabase(QString pjtPath);

    void refreshUIStatus();

private slots:
    void slot_newProject();
    void slot_openProject();
    void slot_closeProject();
    void slot_saveAll();

    /**
     * @brief 打开项目树中的文件
     * @param index 树索引
     */
    void slot_openWithinProject(const QModelIndex &index);
    void slot_closeTargetView(QTextEdit* view);

    /**
     * @brief 传入当前索引，新建节点，如果当前索引指向文件节点，则新节点为兄弟节点，否则为子节点
     * @param index 当前索引
     */
    void slot_newFileNode(const QModelIndex& index);
    /**
     * @brief 传入当前索引，新建节点，如果当前索引指向文件节点，则新节点为兄弟节点，否则为子节点
     * @param index 当前索引
     */
    void slot_newGroupNode(const QModelIndex& index);
    /**
     * @brief 删除当前节点及其子节点
     * @param index 当前索引
     */
    void slot_removeNode(const QModelIndex& index);
    /**
     * @brief 移动当前节点到其他位置
     * @param from 起始点
     * @param to 终末点
     */
    void slot_NodeMove(const QModelIndex& from, const QModelIndex& to);

    void slot_ResponseToolsAct(QAction * item);
    void exit();

public:
    explicit EdServer(QString title);
    virtual ~EdServer();

    void openGraphicsModel();

};


#endif // EDSERVER_H
