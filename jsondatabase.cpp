#include "jsondatabase.h"
#include "jsonmodel.h"
#include "jsonquery.h"

Q_GLOBAL_STATIC(JsonDatabase, gJsonDatabase)

JsonDatabase *database()
{
    return gJsonDatabase;
}

static QObject * json_database_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return database();
}

static void preCreateJsonDatabase()
{
    qmlRegisterSingletonType<JsonDatabase>("JsonDatabase", 1, 0, "JsonDatabase", json_database_singletontype_provider);
    qmlRegisterType<JsonModel>("JsonDatabase", 1, 0, "JsonModel");
    qmlRegisterType<JsonQuery>("JsonDatabase", 1, 0, "JsonQuery");
    qDebug()<<"register json database types";
}

Q_COREAPP_STARTUP_FUNCTION(preCreateJsonDatabase)

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

void JsonDatabase::onModelUpdated()
{
    auto model = qobject_cast<JsonModel*>(sender());
    emit updated(model);
}
