#ifndef EDSERVER_H
#define EDSERVER_H

#include <QString>
#include <QObject>
#include <QAction>
#include <QStandardItemModel>
#include <QTextEdit>
#include "frontend.h"

namespace Support {
    class ProjectSymbo;
    class DBInitTool;
}

class EdServer:public QObject
{
    Q_OBJECT
public:
    explicit EdServer(QString title);
    virtual ~EdServer();

    void openGraphicsModel();

public slots:
    /**
     * @brief 简单的全局保存操作，保存软件所有状态
     */
    void slot_saveAll();



private:
    FrontEnd *const mainView;
    Support::ProjectSymbo* pjtSymbo;
    Support::DBInitTool *dbTool;
    QToolBar * const toolsBar;
    Component::QueryUtility *const queryUtility;

    /**
     * @brief 打开小说数据库
     * @param pjtPath 项目文件夹位置
     */
    void openNovelDatabase(QString pjtPath);

    /**
     * @brief 刷新ui组件状态，主要用于更新菜单栏
     */
    void refreshUIStatus();

private slots:
    /**
     * @brief 创建新工程
     */
    void slot_newProject();

    /**
     * @brief 打开已有工程
     */
    void slot_openProject();

    /**
     * @brief 关闭当前工程
     */
    void slot_closeProject();


    /**
     * @brief 打开项目树中的文件
     * @param index 树索引
     */
    void slot_openItem(const QModelIndex &index);

    /**
     * @brief 关闭指定视图
     * @param view
     */
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

    /**
     * @brief 响应工具菜单
     * @param item 条目
     */
    void slot_ResponseToolsAct(QAction * item);

    /**
     * @brief 退出程序
     */
    void exit();
};


#endif // EDSERVER_H
