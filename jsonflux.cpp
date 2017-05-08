#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include "jsonfluxconnector.h"
#include "jsonfluxlistmodel.h"
#include <QQmlEngine>

Q_GLOBAL_STATIC(JsonFlux, gJsonFlux)

JsonFlux *flux()
{
    return gJsonFlux;
}

static QObject * json_flux_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    auto f = flux();
    QQmlEngine::setObjectOwnership(f, QQmlEngine::CppOwnership);
    return f;
}

static void preCreateJsonDatabase()
{
    qmlRegisterSingletonType<JsonFlux>("JsonFlux", 1, 0, "JsonFlux", json_flux_singletontype_provider);
    qmlRegisterType<JsonFluxModel>("JsonFlux", 1, 0, "JsonFluxModel");
    qmlRegisterType<JsonFluxView>("JsonFlux", 1, 0, "JsonFluxView");
    qmlRegisterType<JsonFluxModifier>("JsonFlux", 1, 0, "JsonFluxModifier");
    qmlRegisterUncreatableType<JsonFluxConnector>("JsonFlux", 1, 0, "JsonFluxConnector",
                                                  "JsonFluxConnector cannot be created directly. Using JsonFlux.createConnector instead.");
    qmlRegisterUncreatableType<JsonFluxListModel>("JsonFlux", 1, 0, "JsonFluxListModel",
                                                  "JsonFluxListModel cannot be created directly. Using JsonFlux.createListModel instead.");
    qDebug()<<"register json flux types";
}

Q_COREAPP_STARTUP_FUNCTION(preCreateJsonDatabase)

JsonFlux::JsonFlux(QObject *parent) : QObject(parent)
{

}

JsonFlux::~JsonFlux()
{
    //qDebug()<<"JsonFlux destroyed";
}

JsonFluxModel * JsonFlux::model(QString modelName)
{
    return m_models.value(modelName, Q_NULLPTR);
}

JsonFluxConnector * JsonFlux::connector(QString connectorName)
{
    return m_connectors.value(connectorName, Q_NULLPTR);
}

JsonFluxConnector * JsonFlux::getOrCreateConnector(QString connectorName, QString modelName)
{
    auto modelObject = model(modelName);
    if(modelObject == Q_NULLPTR) return Q_NULLPTR;

    return getOrCreateConnector(connectorName, modelObject);
}

JsonFluxConnector * JsonFlux::getOrCreateConnector(QString connectorName, JsonFluxModel *modelObject)
{
    if(modelObject == Q_NULLPTR) return Q_NULLPTR;

    if(connector(connectorName)) return connector(connectorName);
    auto newConnector = new JsonFluxConnector(modelObject, this);
    QQmlEngine::setObjectOwnership(newConnector, QQmlEngine::CppOwnership);

    m_connectors[connectorName] = newConnector;
    return newConnector;
}

JsonFluxListModel * JsonFlux::listModel(QString listName)
{
    return m_listModels.value(listName, Q_NULLPTR);
}

JsonFluxListModel * JsonFlux::getOrCreateListModel(QString listName, QString query, QStringList roles, QString modelName)
{
    auto modelObject = model(modelName);
    if(modelObject == Q_NULLPTR) return Q_NULLPTR;

    return getOrCreateListModel(listName, query, roles, modelObject);
}

JsonFluxListModel * JsonFlux::getOrCreateListModel(QString listName, QString query, QStringList roles, JsonFluxModel *modelObject)
{
    if(modelObject == Q_NULLPTR) return Q_NULLPTR;

    if(listModel(listName))
    {
        auto lm = listModel(listName);
        lm->setQuery(query);
        lm->setRoles(roles);
        lm->setModel(modelObject);
        return lm;
    }

    auto newListModel = new JsonFluxListModel(modelObject, query, roles, this);
    QQmlEngine::setObjectOwnership(newListModel, QQmlEngine::CppOwnership);

    m_listModels[listName] = newListModel;
    return newListModel;
}

bool JsonFlux::registerModel(QString modelName, JsonFluxModel *model)
{
    if(m_models.contains(modelName)) return false;

    m_models.insert(modelName, model);

    return true;
}

bool JsonFlux::unregisterModel(QString modelName, bool deleteModel)
{
    if(!m_models.contains(modelName)) return false;

    auto model = m_models.value(modelName, Q_NULLPTR);
    m_models.remove(modelName);

    if(deleteModel && model)
        model->deleteLater();
    return true;
}

void JsonFlux::onModelUpdated()
{
    auto model = qobject_cast<JsonFluxModel*>(sender());
    emit updated(model);
}

QVariantMap JsonFlux::toVariantMap(json &jo)
{
    QVariantMap vm;
    for (auto it = jo.begin(); it != jo.end(); ++it)
    {
        auto key = QString::fromStdString(it.key());
        vm[key] = toVariant(it.value());
    }
    return vm;
}

QVariantList JsonFlux::toVariantList(json::value_type &jsonArray)
{
    QVariantList vl;
    for(auto it = jsonArray.begin(); it != jsonArray.end(); ++it)
    {
        vl.push_back(toVariant(it.value()));
    }
    return vl;
}

QVariant JsonFlux::toVariant(json::value_type &jsonValue){
    QVariant v;
    auto valueType = jsonValue.type();
    switch (valueType)
    {
    case json::value_t::boolean:
        v = jsonValue.get<bool>();
        break;
    case json::value_t::string:
        v = QString::fromStdString(jsonValue.get<std::string>());
        break;
    case json::value_t::number_integer:
        v = jsonValue.get<int>();
        break;
    case json::value_t::number_unsigned:
        v = jsonValue.get<quint32>();
        break;
    case json::value_t::number_float:
        v = jsonValue.get<qreal>();
        break;
    case detail::value_t::array:
        v = toVariantList(jsonValue);
        break;
    case detail::value_t::object:
        v = toVariantMap(jsonValue);
        break;
    default:
        break;
    }
    return v;
}

json::value_type JsonFlux::toJsonValue(QVariant variant)
{
    json::value_type jsonValue;

    switch (variant.type())
    {
    case QMetaType::Bool:
        jsonValue = variant.value<bool>();
        break;
    case QMetaType::QString:
        jsonValue = variant.value<QString>().toStdString();
        break;
    case QMetaType::Int:
        jsonValue = variant.value<int>();
        break;
    case QMetaType::QReal:
        jsonValue = variant.value<qreal>();
        break;
    case QMetaType::QJsonValue:
        jsonValue = toJsonArray(variant.value<QVariantList>());
        break;
    case QMetaType::QVariantMap:
        jsonValue = toJsonObject(variant.value<QVariantMap>());
        break;
    default:
        break;
    }
    return jsonValue;
}

json::value_type JsonFlux::toJsonArray(QVariantList vl)
{
    json::value_type jsonVal;
    for (auto v : vl)
    {
        jsonVal.push_back(toJsonValue(v));
    }
    return jsonVal;
}

json::value_type JsonFlux::toJsonObject(QVariantMap vm)
{
    json::value_type jsonVal;
    for (auto it = vm.begin(); it != vm.end(); ++it)
    {
        jsonVal[it.key().toStdString()] = toJsonValue(it.value());
    }
    return jsonVal;
}
