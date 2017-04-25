#include "jsondatabase.h"

Q_GLOBAL_STATIC(JsonDatabase, gJsonDatabase)

JsonDatabase *database()
{
    return gJsonDatabase;
}

JsonDatabase::JsonDatabase(QObject *parent) : QObject(parent)
{

}

JsonModel * JsonDatabase::model(QString modelName)
{
    return m_models.value(modelName, Q_NULLPTR);
}

bool JsonDatabase::registerModel(QString modelName, JsonModel *model)
{
    if(m_models.contains(modelName)) return false;

    m_models.insert(modelName, model);

    return true;
}

bool JsonDatabase::unregisterModel(QString modelName, bool deleteModel)
{
    if(!m_models.contains(modelName)) return false;

    auto model = m_models.value(modelName, Q_NULLPTR);
    m_models.remove(modelName);

    if(deleteModel && model)
        model->deleteLater();
    return true;
}
