#ifndef STORYBOARD_H
#define STORYBOARD_H

#include "storydisplay.h"

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>


namespace UIComp {

class StoryBoard: public QDialog
{
public:
    explicit StoryBoard(QWidget * parent = nullptr);
    virtual ~StoryBoard() override;

private:
    QLabel *const charName;
    QLabel *const nodeName;
    UIComp::StoryDisplay *const time_Story;
    QTabWidget *const tabCon;

    QPushButton *const addEvent,
                *const removeEvent;
    QComboBox *const locationSelect;
    QTextEdit *const evNode2Char_Desc;

    QPushButton *const addProp,
                *const removeProp;
    QTableView  *const propTable;


    QPushButton *const addSkill,
                *const removeSkill;
    QTableView  *const skillTable;


    QPushButton *const addRelation,
                *const removeRelation;
    QTableView  *const relationTable;
};

}
#endif // STORYBOARD_H
