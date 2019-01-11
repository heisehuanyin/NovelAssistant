#ifndef QUERYUTILITY_H
#define QUERYUTILITY_H

#include <QComboBox>
#include <QTabWidget>
#include <QTableView>

namespace Component {


class QueryUtility: public QTabWidget
{
    Q_OBJECT
public:
    explicit QueryUtility(QWidget* parent=nullptr);
    virtual ~QueryUtility();

    void clear();

public slots:
    void resetEventNode(qlonglong evnode);

private:
    QTableView *const quickLook;
};


}
#endif // QUERYUTILITY_H
