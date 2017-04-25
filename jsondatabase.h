#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include <QObject>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QJSValue>
#include <QtQml>
#include <QMap>
#include <QtDebug>
#include "jsonmodel.h"
#include "jsonquery.h"

class JsonDatabase : public QObject
{
    Q_OBJECT
public:
    explicit JsonDatabase(QObject *parent = 0);

    JsonModel * model(QString modelName);
    bool registerModel(QString modelName, JsonModel *model);
    bool unregisterModel(QString modelName, bool deleteModel = false);

public slots:
private:
    QMap<QString, JsonModel*> m_models;
};

JsonDatabase *database();

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

#endif // JSONDATABASE_H
