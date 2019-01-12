#ifndef QUERYUTILITY_H
#define QUERYUTILITY_H

#include <QComboBox>
#include <QTabWidget>
#include <QTableView>

namespace Component {


class QueryUtility: public QWidget
{
    Q_OBJECT
public:
    explicit QueryUtility(QWidget* parent=nullptr);
    virtual ~QueryUtility();

    void clear();

public slots:
    void resetEventNode(qlonglong evnode);

private:
    QComboBox*const anchor;//锚定设施

    QTabWidget*const quickTab;//快速信息面板
    QTableView *const quickLook;//快速信息清单


};


}
#endif // QUERYUTILITY_H
