#include "jsondatabase.h"

Q_GLOBAL_STATIC(JsonDatabase, gJsonDatabase)

JsonDatabase *database()
{
    return gJsonDatabase;
}

JsonDatabase::JsonDatabase(QObject *parent) : QObject(parent)
{

}
