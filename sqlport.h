#ifndef SQLPORT_H
#define SQLPORT_H

#include <QHash>
#include <QObject>
#include <QSqlDatabase>
namespace Support {
    class DatabaseInitialTool:public QObject
    {
        Q_OBJECT
    public:
        explicit DatabaseInitialTool();
        virtual ~DatabaseInitialTool();

    private:
        bool createConnection();
        void init_emptytable();
    };

}
#endif // SQLPORT_H
