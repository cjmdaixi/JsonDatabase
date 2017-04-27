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

signals:
    void updated(QString modelName);
public slots:
private:
    QMap<QString, JsonModel*> m_models;
};

JsonDatabase *database();

#endif // JSONDATABASE_H
