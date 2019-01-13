#ifndef EDSERVER_H
#define EDSERVER_H

#include <QString>
#include <QObject>
#include <QAction>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QToolBar>

class FrontEnd;

namespace Support {
    class ProjectSymbo;
    class DBInitTool;
    class SyntaxHighlighter;
}

namespace Component {
    class QueryUtility;
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
    void saveAll();



private:
    FrontEnd *const mainView;
    Support::ProjectSymbo* pjtSymbo;
    Support::DBInitTool *dbTool;
    QToolBar * const toolsBar;
    Component::QueryUtility *const queryUtility;
    Support::SyntaxHighlighter *const highter;

    /**
     * @brief 打开小说数据库
     * @param pjtPath 项目文件夹位置
     */
    void openNovelDatabase(QString pjtPath);
    /**
     * @brief 当组件状态发生变化时候，刷新ui组件状态，主要用于更新菜单栏
     * @attention menubar
     */
    void refreshUIStatus();
    /**
     * @brief 对项目关键数据进行变动时候，调用本接口自动刷新相关组件
     */
    void refreshDataStatus();

private slots:
    /**
     * @brief 创建新工程
     */
    void newProject();
    /**
     * @brief 打开已有工程
     */
    void openProject();
    /**
     * @brief 关闭当前工程
     */
    void closeProject();


    /**
     * @brief 打开项目树中的文件
     * @param index 树索引
     */
    void openItem(const QModelIndex &index);
    /**
     * @brief 关闭指定视图
     * @param view
     */
    void closeTargetView(QTextEdit* view);
    /**
     * @brief 传入当前索引，新建节点，如果当前索引指向文件节点，则新节点为兄弟节点，否则为子节点
     * @param index 当前索引
     */
    void newFileNode(const QModelIndex& index);
    /**
     * @brief 传入当前索引，新建节点，如果当前索引指向文件节点，则新节点为兄弟节点，否则为子节点
     * @param index 当前索引
     */
    void newGroupNode(const QModelIndex& index);
    /**
     * @brief 删除当前节点及其子节点
     * @param index 当前索引
     */
    void removeNode(const QModelIndex& index);
    /**
     * @brief 移动当前节点到其他位置
     * @param from 起始点
     * @param to 终末点
     */
    void nodeMove(const QModelIndex& from, const QModelIndex& to);


    /**
     * @brief 聚焦关键词
     * @param keywords 关键词id
     */
    void keywordsFocuse(qlonglong keywords, QString);


    /**
     * @brief 响应工具菜单
     * @param item 条目
     */
    void responseToolsSet(QAction * item);
    /**
     * @brief 退出程序
     */
    void exit();
};


#endif // EDSERVER_H
