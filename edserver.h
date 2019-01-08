#ifndef EDSERVER_H
#define EDSERVER_H

#include <QString>
#include <QObject>
#include <QAction>

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
     * @brief 获取文档管理服务接口
     * @return 管理器实例
     */
    Support::DocManager* getDocumentManager();

    /**
     * @brief 打开小说数据库
     * @param pjtPath 项目文件夹位置
     */
    void openNovelDatabase(QString pjtPath);

    void refreshMenubarStatus();

private slots:
    void slot_newProject();
    void slot_openProject();
    void slot_ResponseToolsAct(QAction * item);
    void exit();

public:
    explicit EdServer(QString title);
    virtual ~EdServer();


    void openGraphicsModel();

};


#endif // EDSERVER_H
