#ifndef DOCMANAGER_H
#define DOCMANAGER_H

#include <QString>
#include <QTextEdit>

namespace Support {
/**
 * @brief 用于管理打开的文档，基于文件路径对视图实例进行管理。重复打开此路径，将返回打开的视图实例。
 */
class DocManager
{
private:
    QHash<QString, QTextEdit*> docCon;

public:
    /**
     * @brief 构建新的文档管理器
     */
    explicit DocManager();
    virtual ~DocManager();

    /**
     * @brief 打开指定路径文档（不含后缀），如果文档已经打开，那么返回打开实例
     * @warning 如果文档路径不存在，会新建一个空白文档
     * @param filePath 文档路径,不含后缀
     * @param view 视图组件指针
     * @return 操作码：0，成功；-1，打开失败；-2，其他错误
     */
    int openDocument(QString filePath, QTextEdit **view);

    /**
     * @brief 保存指定文档
     * @param filePath 文件路径
     * @return 操作状态：0，成功；-1，此路径未打开；-2，保存失败，文档无法打开
     */
    int saveDocument(QString filePath);

    /**
     * @brief 保存全部打开文档
     * @return 操作状态：0，成功；其他，失败；
     */
    int saveAll();

    /**
     * @brief 获取所有打开文档
     * @return 文档key集合
     */
    QList<QString> getAllDocuments();

    /**
     * @brief 强制关闭指定文档，不保存
     * @warning 不保存视图内容，谨慎使用
     * @param filePath 文件路径
     * @return 操作状态：0，成功；-1，此路径未打开
     */
    int closeDocumentWithoutSave(QString filePath);

    /**
     * @brief 通过实例进行查询本实例关联的文档路径
     * @param view 实例指针
     * @return 地址路径，如果返回“”，表示不存在该视图
     */
    QString returnDocpath(QTextEdit *view);
};

}
#endif // DOCMANAGER_H
