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
namespace Component {
    class QueryUtility;
}

class FrontEnd : public QMainWindow
{
    Q_OBJECT

public:
    FrontEnd(QWidget *parent = nullptr);
    ~FrontEnd();

    /**
     * @brief 打开一个空白窗口，等同于widget.show
     */
    void openEmptyWindow();
    /**
     * @brief 添加一个编辑视图到窗口，并刷新组件状态
     * @param title 视图标题
     * @param view 视图组件
     */
    void addDocumentView(QString title, QWidget *view);
    /**
     * @brief 添加项目树模型到窗口
     * @param model 树模型
     */
    void setProjectTree(QStandardItemModel *model);
    /**
     * @brief 添加utility到窗口
     * @param utility utility实体
     */
    void setQueryUtility(Component::QueryUtility* utility);


signals:
    /**
     * @brief 打开项目条目
     * @param index 模型索引
     */
    void signal_openProjectItem(const QModelIndex &index);
    /**
     * @brief 关闭指定项目视图
     * @param ref 视图
     */
    void signal_closeTargetView(QTextEdit* ref);
    /**
     * @brief 基于指定索引创建新节点指向文件
     * @param index 指定索引
     */
    void signal_newFile(const QModelIndex &index);
    /**
     * @brief 基于指定索引创建新节点指向集合
     * @param index 指定索引
     */
    void signal_newGroup(const QModelIndex &index);
    /**
     * @brief 移除指定索引指向的节点及其后代节点
     * @param index 指定索引
     */
    void signal_removeNode(const QModelIndex &index);
    /**
     * @brief 移动节点位置
     * @param from 起始点索引
     * @param to 目标点索引
     */
    void signal_moveNode(const QModelIndex &from, const QModelIndex &to);

private:
    QSplitter *const baseFrame;
    QTreeView *const pjtStructure;
    QTabWidget *const contentStack;
    QWidget *const welcome;

    QWidget* generateWelcomePanel();
    void refreshTabviewStatus();

private slots:
    /**
     * @brief 接收打开文档信号
     * @param index
     */
    void slot_receptOpenDocument(const QModelIndex &index);
    /**
     * @brief 接收关闭tab页信号
     * @param index
     */
    void slot_receptCloseDocument(int index);

    /**
     * @brief 响应弹出菜单请求
     * @param point 坐标位置
     */
    void slot_displayPopupMenu(const QPoint &point);

    /**
     * @brief 处理弹出菜单上条目
     * @param act 条目
     */
    void slot_ProjectManage(QAction* act);

};


#endif // MAINWINDOW_H
