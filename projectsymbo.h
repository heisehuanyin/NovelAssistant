#ifndef PROJECTSYMBO_H
#define PROJECTSYMBO_H

#include <QObject>
#include <QStandardItemModel>
#include <QDomDocument>
#include "docmanager.h"

namespace Support {

namespace __projectsymbo {
    class FileSymbo : public QStandardItem{
    public:
        explicit FileSymbo(QDomElement dom);
        virtual ~FileSymbo();

        QString getRefPath();
        QDomElement& getDom();

    private:
        QDomElement domData;
    };

    class GroupSymbo : public FileSymbo{
    public:
        explicit GroupSymbo(QDomElement dom);
        virtual ~GroupSymbo() override;
    };
}

/**
 * @brief 本类型用于代表项目本身，针对项目进行的操作都转化为对本类型示例的操作
 */
class ProjectSymbo : public QObject, private __projectsymbo::DocManager
{
    Q_OBJECT

signals:
    void signal_nodeModified(const QModelIndex &index);

public:
    /**
     * @brief 构建一个空的项目
     * @param parent
     */
    explicit ProjectSymbo(QObject *parent = nullptr);
    virtual ~ProjectSymbo();

    //Config Service=========================
    /**
     * @brief 获取项目配置信息
     * @param key 配置键
     * @return 配置值，如果未配置则返回QString()
     */
    QString config(QString key);
    /**
     * @brief 设置项目配置信息
     * @param key 配置键
     * @param value 配置值
     */
    void setConfig(QString key, QString value);



    //Project Operate========================
    /**
     * @brief 打开项目文件，提取内容
     * @param filePath 项目路径，项目指向
     * @return 0,成功；-1,失败
     */
    int openProject(QString filePath);
    /**
     * @brief 返回项目路径
     * @return 项目路径
     */
    QString projectPath();
    /**
     * @brief 获取项目结构
     * @return 树模型
     */
    QStandardItemModel* getStructure();
    /**
     * @brief 保存项目文件，filePath !=QString() 另存为且改变指向
     * @param filePath 路径
     * @return 0,成功；-1,失败
     */
    int saveProject(QString filePath=QString());
    /**
     * @brief 在指定父节点下新建项目文件节点
     * @param name 节点名称
     * @param parent 父节点索引
     * @return 新节点的索引
     */
    QModelIndex newChildFile(QString name, const QModelIndex &parent);
    /**
     * @brief 在指定父节点下新建项目集合节点
     * @param name 节点名称
     * @param parent 父节点索引
     * @return 新节点索引
     */
    QModelIndex newChildGroup(QString name, const QModelIndex &parent);
    /**
     * @brief 删除指定索引指向的节点
     * @warning 不可删除根节点
     * @param target 指定节点索引
     */
    void removeNode(const QModelIndex &target);
    /**
     * @brief 将from指向的节点，移动到to索引位置
     * @warning from和to都必须是根节点的后代节点
     * @param from 指定节点的索引
     * @param to 目标位置索引，可以指向集合尾节点的下一个位置
     */
    void moveNodeTo(const QModelIndex &from, const QModelIndex &to);




    //Document Operate=======================
    /**
     * @brief 通过modelindex打开项目结构中对应的文档视图，如果文档已打开，返回对应视图
     * @param index modelIndex
     * @param title 视图标题
     * @param view 返回视图
     */
    void openDocument(const QModelIndex &index, QString &title, QTextEdit** view);
    /**
     * @brief 关闭指定文档
     * @param index itemIndex
     */
    void closeDocument(const QModelIndex &index);
    /**
     * @brief 获取视图对应的ModelIndex
     * @param view 编辑视图
     * @return Index
     */
    QModelIndex getIndexofDocumentView(QTextEdit *view);

private:
    QStandardItemModel*const structure;
    QDomDocument*const domData;
    QString pjtPath;
    QHash<QModelIndex, QString> list;


    /**
     * @brief 通过模型接口创建一个文件节点
     * @return 文件节点
     */
    QStandardItem* newChildFile(QString name);

    /**
     * @brief 通过模型接口创建一个集合节点
     * @return 集合节点
     */
    QStandardItem* newChildGroup(QString name);

    /**
     * @brief 将特定节点node插入到指定节点parent的指定位序下，两个节点都必须存在,索引小于parent.rowCount
     * @param node 任意类型模型节点
     * @param parent 必须是集合节点
     * @param index 索引必须小于parent.rowCount
     */
    void insertItemUnder(QStandardItem* node, QStandardItem* parent, int index);

    /**
     * @brief 添加特定节点到指定父节点的尾部
     * @param node 特定节点
     * @param parent 指定父节点
     */
    void appendItemUnder(QStandardItem*node, QStandardItem* parent);

    /**
     * @brief 移除指定节点parent下的特定节点node
     * @param node 任意类型模型节点,且此节点必须在parent下
     * @param parent 必须是集合节点
     */
    void removeItemUnder(QStandardItem* node, QStandardItem*parent);

    /**
     * @brief 解析dom节点构建模型
     * @param dom dom节点
     * @param group 父节点节点，若为nullptr，表示此节点代表content节点
     */
    void parseStructureDom(QDomElement dom, QStandardItem *group = nullptr);

private slots:
    void slot_nodeModify(QStandardItem* item);

};

}

#endif // PROJECTSYMBO_H
