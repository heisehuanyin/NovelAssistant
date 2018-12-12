#include "superdatetool.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

using namespace Support;

SuperDateTool::SuperDateTool(QWidget *parent):
    QDialog (parent)
{
    auto grid(new QGridLayout);
    this->setLayout(grid);

    QSqlQuery q;
    if(!q.exec("select "
               "unit,"
               "base,"
               "not_0,"
               "comment "
               "from table_timeformat "
               "order by _index desc;")){
        qDebug() << q.lastError();
    }
    while (q.next()) {
        this->baseList << QPair<QString,qlonglong>(q.value(0).toString(), q.value(1).toLongLong());
        if(q.value(2).isValid() && q.value(2).toString() == "yes"){
            this->item_not0.append(q.value(0).toString());
        }
    }

    for(int i=0; i< this->baseList.size(); ++i){
        auto m1 = this->baseList.at(i);
        auto input = new QLineEdit();
        grid->addWidget(input, i, 0);
        this->editList.append(input);
        if(this->item_not0.contains(m1.first)){
            this->connect(input, &QLineEdit::textEdited,
                          this,  &SuperDateTool::slot_cannotAssignZero);
        }
        grid->addWidget(new QLabel(m1.first), i, 1);
    }
}

SuperDateTool::~SuperDateTool()
{

}



void SuperDateTool::resetDate(qlonglong days)
{
    QPair<QString,qlonglong> refp;
    foreach (refp, this->baseList) {
        qlonglong times = days / refp.second;
        days = days % refp.second;

        if(days < 0){
            days = refp.second + days;
        }
        this->innerValues.insert(refp.first, times);
    }

}

qlonglong SuperDateTool::dateEdit()
{
    for(int i=0; i<this->baseList.size(); ++i){
        auto label = baseList.at(i).first;
        auto val = this->innerValues.value(label);
        if(this->item_not0.contains(label)){
            val += 1;
        }
        this->editList.at(i)->setText(QString("%1").arg(val));
    }
    exec();
    qlonglong number = 0;
    for(int i=0; i<this->baseList.size(); ++i){
        auto base = baseList.at(i).second;
        auto value = editList.at(i)->text().toLongLong();

        if(this->item_not0.contains(baseList.at(i).first)){
            value -= 1;
        }
        number += value * base;
    }
    return number;
}

qlonglong SuperDateTool::toLongLong()
{
    qlonglong number = 0;
    for(int i=0; i<this->baseList.size(); ++i){
        auto baseNum = baseList.at(i).second;
        auto value = this->innerValues.value(baseList.at(i).first);

        if(value < 0)
            value -= 1;

        number += value * baseNum;
    }
    return number;
}

QString SuperDateTool::toString()
{
    QString rtnStr;
    QPair<QString,qlonglong> refp;
    foreach (refp, this->baseList) {
        auto val = this->innerValues.value(refp.first);
        if(val < 0)
            rtnStr = "前" + rtnStr;
        if(this->item_not0.contains(refp.first)){
            val += 1;
        }
        rtnStr += QString("%1").arg(qAbs(val)) + refp.first;
    }
    return rtnStr;
}

void SuperDateTool::slot_cannotAssignZero()
{
    for(int i=0; i< this->editList.size(); ++i){
        QLineEdit *unit = this->editList.at(i);
        QString label = this->baseList.at(i).first;
        auto val = unit->text().toLongLong();
        if(this->item_not0.contains(label) && val == 0){
            unit->setText(QString("%1").arg(1));
        }
    }
}
