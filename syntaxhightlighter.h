#ifndef SYNTAXHIGHTLIGHTER_H
#define SYNTAXHIGHTLIGHTER_H

#include <QSyntaxHighlighter>


namespace Support {

    /**
     * @brief 自定义语法高亮，允许后期添加语法高亮关键词，允许分类别定义关键词格式
     * @attention 允许变更字体，允许设置斜体，允许设置颜色，允许设置粗体，允许下划线
     * @warning 不允许设置字体大小，字体大小与目标document同步
     */
    class SyntaxHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT
    public:
        explicit SyntaxHighlighter(QObject* parent=nullptr);
        virtual ~SyntaxHighlighter() override;

        /**
         * @brief 分类别添加语法高亮关键词
         * @param type 分类
         * @param keywords 关键词
         */
        void addKeywords(const QString type, const QString keywords, qlonglong itemId);
        /**
         * @brief 清空指定类别关键词库，保持分类以及分类自定义
         * @param type 类别
         */
        void clear(QString type);

        /**
         * @brief 关键词库类别集合
         * @return
         */
        QList<QString> getTypes();
        /**
         * @brief 获取指定类型关键此格式
         * @param type 分类
         * @return 格式
         */
        QTextCharFormat getFormat(const QString& type);
        /**
         * @brief 设置自定义类别format，如果该类别不存在，忽略本操作
         * @param type 分类
         * @param format 格式
         */
        void setCustomFormat(const QString type, QTextCharFormat& format);
        /**
         * @brief 所有关键字共享的字体大小设置
         * @param weight 字体大小，以pointSize计算
         */
        void setGlobalFontSize(int weight);

    signals:
        void discoveryKeywords(qlonglong item, QString &type);

    private:
        //      type         format        items
        QHash<QString, QPair<QTextCharFormat*, QList<QPair<QString, qlonglong>>*>*> formates;

        // QSyntaxHighlighter interface
    protected:
        virtual void highlightBlock(const QString &text) override;
    };
}

#endif // SYNTAXHIGHTLIGHTER_H
