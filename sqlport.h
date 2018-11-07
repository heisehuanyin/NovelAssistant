#ifndef SQLPORT_H
#define SQLPORT_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>
namespace Support {
    class DBInitTool:public QObject
    {
        Q_OBJECT
    public:
        explicit DBInitTool();
        virtual ~DBInitTool();

    private:
        bool createConnection();
        void init_emptytable();
    };

}
#endif // SQLPORT_H
