#ifndef PROJECTSYMBO_H
#define PROJECTSYMBO_H

#include <QObject>
#include <QStandardItemModel>
#include <QDomDocument>

namespace Support {

namespace __project {
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
        virtual ~GroupSymbo();
    };
}

/**
 * @brief 本类型用于代表项目本身，针对项目进行的操作都转化为对本类型示例的操作
 */
class ProjectSymbo : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构建一个空的项目
     * @param parent
     */
    explicit ProjectSymbo(QObject *parent = nullptr);
    virtual ~ProjectSymbo();

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
     * @brief 保存项目文件，filePath !=QString() 另存为且改变指向
     * @param filePath 路径
     * @return 0,成功；-1,失败
     */
    int save(QString filePath=QString());

    /**
     * @brief 获取项目结构
     * @return 树模型
     */
    QStandardItemModel* getStructure();

    /**
     * @brief 通过模型接口创建一个文件节点
     * @return 文件节点
     */
    QStandardItem* newFile(QString name);

    /**
     * @brief 通过模型接口创建一个集合节点
     * @return 集合节点
     */
    QStandardItem* newGroup(QString name);

    /**
     * @brief 将特定节点node插入到指定节点parent的指定位序下
     * @param node 任意类型模型节点
     * @param parent 必须是集合节点，如果为nullptr表示根节点
     * @param index 次序，如果为 -1，表示append操作
     */
    void addItemUnder(QStandardItem* node, QStandardItem* parent=nullptr, int index= -1);

    /**
     * @brief 移除指定节点parent下的特定节点node
     * @param node 任意类型模型节点,且此节点必须在parent下
     * @param parent 必须是集合节点，如果未nullptr表示根节点
     */
    void removeItemUnder(QStandardItem* node, QStandardItem*parent=nullptr);

    /**
     * @brief 获取模型节点指向的文件路径
     * @param node 此节点出自于本项目模型
     * @return 路径
     */
    QString referenceFilePath(QStandardItem* node);

private:
    QStandardItemModel*const structure;
    QDomDocument*const domData;
    QString pjtPath;

    void parseStructureDom(QDomElement dom, QStandardItem *group = nullptr);

private slots:
    void slot_nodeModify(QStandardItem* item);
};

}

#endif // PROJECTSYMBO_H
