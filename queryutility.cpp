#include "queryutility.h"

#include <QVBoxLayout>

using namespace Component;

QueryUtility::QueryUtility(QWidget *parent):
    QWidget (parent),
    anchor(new QComboBox(this)),
    quickTab(new QTabWidget(this)),
    quickLook(new QTableView(this))
{
    auto grid(new QVBoxLayout(this));
    this->setLayout(grid);

    grid->addWidget(this->quickTab);
    this->quickTab->addTab(this->quickLook, tr("同步简略"));

    grid->addWidget(this->anchor);
}

QueryUtility::~QueryUtility()
{

}

void QueryUtility::clear()
{

}

void QueryUtility::resetEventNode(qlonglong evnode)
{

}
