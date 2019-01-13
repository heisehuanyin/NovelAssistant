#include "syntaxhightlighter.h"

using namespace Support;

SyntaxHighlighter::SyntaxHighlighter(QObject *parent):
    QSyntaxHighlighter (parent)
{

}

SyntaxHighlighter::~SyntaxHighlighter(){
    auto list = this->formates.keys();

    foreach(auto type, list){
        auto itemPair = this->formates.value(type);
        delete itemPair->first;
        delete itemPair->second;
        delete itemPair;
    }
}

void SyntaxHighlighter::addKeywords(const QString type, const QString keywords, qlonglong itemId){
    if(!this->formates.contains(type)){
        //默认format
        auto format = new QTextCharFormat;

        format->setFontItalic(true);
        format->setForeground(Qt::blue);
        format->setFontUnderline(true);
        format->setFontWeight(QFont::Bold);

        auto list = new QList<QPair<QString,qlonglong>>;
        auto keyList = new QPair<QTextCharFormat*, QList<QPair<QString,qlonglong>>*>(format, list);
        this->formates.insert(type, keyList);
    }

    auto kwList = this->formates.value(type);
    if(kwList->second->contains(qMakePair(keywords, itemId)))
        return;
    kwList->second->append(qMakePair(keywords, itemId));
}

void SyntaxHighlighter::clear(QString type){
    if(this->formates.contains(type)){
        auto keyList = this->formates.value(type);
        keyList->second->clear();
    }
}

QList<QString> SyntaxHighlighter::getTypes(){
    return this->formates.keys();
}

QTextCharFormat SyntaxHighlighter::getFormat(const QString &type){
    if(this->formates.contains(type))
        return * this->formates.value(type)->first;

    return QTextCharFormat();
}

void SyntaxHighlighter::setCustomFormat(const QString type, QTextCharFormat &format){
    auto kwList = this->formates.value(type);

    kwList->first->setFontFamily(format.fontFamily());
    kwList->first->setFontItalic(format.fontItalic());
    kwList->first->setFontWeight(format.fontWeight());
    kwList->first->setFontUnderline(format.fontUnderline());
    kwList->first->setForeground(format.foreground());
}

void SyntaxHighlighter::setGlobalFontSize(int weight){
    auto keys = this->formates.keys();
    foreach(auto type, keys){
        auto kwlist = this->formates.value(type);
        kwlist->first->setFontPointSize(weight);
    }
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    auto types = this->formates.keys();

    foreach (auto type, types) {
        auto kwList = this->formates.value(type);

        foreach (auto keyword, *kwList->second) {
            int index = text.indexOf(keyword.first);
            if(index >=0)
                emit this->discoveryKeywords(keyword.second, type);

            while (index >= 0) {
                int length = keyword.first.length();
                setFormat(index, length, *kwList->first);
                index = text.indexOf(keyword.first, index+length);
            }
        }
    }
}
