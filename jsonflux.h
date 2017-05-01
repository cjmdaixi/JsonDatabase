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

    JsonFluxModel * model(QString modelName);
    bool registerModel(QString modelName, JsonFluxModel *model);
    bool unregisterModel(QString modelName, bool deleteModel = false);

    static QVariantMap toVariantMap(json &jo);
    static QVariant toVariant(json::value_type &jsonValue);
signals:
    void updated(JsonFluxModel *model);
public slots:
    void onModelUpdated();

private:
    QMap<QString, JsonFluxModel*> m_models;
};

JsonFlux *flux();

#endif // JSONFLUX_H
