#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include <QObject>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QJSValue>
#include <QtQml>
#include <QMap>
#include <QtDebug>

class JsonModel;
class JsonQuery;

class JsonDatabase : public QObject
{
    Q_OBJECT
public:
    explicit JsonDatabase(QObject *parent = 0);

    JsonModel * model(QString modelName);
    bool registerModel(QString modelName, JsonModel *model);
    bool unregisterModel(QString modelName, bool deleteModel = false);

signals:
    void updated(JsonModel *model);
public slots:
    void onModelUpdated();
private:
    QMap<QString, JsonModel*> m_models;
};

JsonDatabase *database();

#endif // JSONDATABASE_H
