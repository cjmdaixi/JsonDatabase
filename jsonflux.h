#ifndef JSONFLUX_H
#define JSONFLUX_H

#include <QObject>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QJSValue>
#include <QtQml>
#include <QMap>
#include <QtDebug>
#include "json.h"
using namespace nlohmann;

class JsonFluxModel;
class JsonFluxView;

class JsonFlux : public QObject
{
    Q_OBJECT
public:
    explicit JsonFlux(QObject *parent = 0);

    Q_INVOKABLE JsonFluxModel * model(QString modelName);
    bool registerModel(QString modelName, JsonFluxModel *model);
    bool unregisterModel(QString modelName, bool deleteModel = false);

    static QVariantMap toVariantMap(json &jo);
    static QVariantList toVariantList(json::value_type &jsonArray);
    static QVariant toVariant(json::value_type &jsonValue);
    static json::value_type toJsonValue(QVariant variant);
    static json::value_type toJsonObject(QVariantMap vm);
    static json::value_type toJsonArray(QVariantList vl);
signals:
    void updated(JsonFluxModel *model);
public slots:
    void onModelUpdated();

private:
    QMap<QString, JsonFluxModel*> m_models;
};

JsonFlux *flux();

#endif // JSONFLUX_H
