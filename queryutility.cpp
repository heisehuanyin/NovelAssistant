#include "queryutility.h"

using namespace Component;

QueryUtility::QueryUtility(QWidget *parent):
    QTabWidget (parent),
    quickLook(new QTableView(this))
{
    this->addTab(this->quickLook, tr("同步简略"));

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
