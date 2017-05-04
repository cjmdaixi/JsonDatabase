#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include "jsonfluxconnector.h"

Q_GLOBAL_STATIC(JsonFlux, gJsonFlux)

JsonFlux *flux()
{
    return gJsonFlux;
}

static QObject * json_flux_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return flux();
}

static void preCreateJsonDatabase()
{
    qmlRegisterSingletonType<JsonFlux>("JsonFlux", 1, 0, "JsonFlux", json_flux_singletontype_provider);
    qmlRegisterType<JsonFluxModel>("JsonFlux", 1, 0, "JsonFluxModel");
    qmlRegisterType<JsonFluxView>("JsonFlux", 1, 0, "JsonFluxView");
    qmlRegisterType<JsonFluxModifier>("JsonFlux", 1, 0, "JsonFluxModifier");
    qmlRegisterType<JsonFluxConnector>("JsonFlux", 1, 0, "JsonFluxConnector");
    qDebug()<<"register json flux types";
}

Q_COREAPP_STARTUP_FUNCTION(preCreateJsonDatabase)

JsonFlux::JsonFlux(QObject *parent) : QObject(parent)
{

}

JsonFluxModel * JsonFlux::model(QString modelName)
{
    return m_models.value(modelName, Q_NULLPTR);
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
    for (auto it = jo.begin(); it != jo.end(); ++it) {
        auto key = QString::fromStdString(it.key());
        vm[key] = toVariant(it.value());
    }
    return vm;
}

QVariant JsonFlux::toVariant(json::value_type &jsonValue){
    QVariant v;
    auto valueType = jsonValue.type();
    if(valueType  == json::value_t::boolean){
        v = jsonValue.get<bool>();
    }
    else if(valueType == json::value_t::string){
        v = QString::fromStdString(jsonValue.get<std::string>());
    }
    else if(valueType == json::value_t::number_integer){
        v = jsonValue.get<int>();
    }
    else if(valueType == json::value_t::number_unsigned){
        v = jsonValue.get<quint32>();
    }
    else if(valueType == json::value_t::number_float){
        v = jsonValue.get<qreal>();
    }
    else if(valueType == detail::value_t::array){
        QVariantList vl;
        for(auto it = jsonValue.begin(); it != jsonValue.end(); ++it){
            vl.push_back(toVariant(it.value()));
        }
        v = vl;
    }
    else if(valueType == detail::value_t::object){
        QVariantMap vm = toVariantMap(jsonValue);
        v = vm;
    }
    return v;
}

json::value_type JsonFlux::toJsonValue(QVariant variant)
{
    json::value_type jsonValue;

    switch (variant.type()) {
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
    for (auto v : vl){
        jsonVal.push_back(toJsonValue(v));
    }
    return jsonVal;
}

json::value_type JsonFlux::toJsonObject(QVariantMap vm)
{
    json::value_type jsonVal;
    for (auto it = vm.begin(); it != vm.end(); ++it){
        jsonVal[it.key().toStdString()] = toJsonValue(it.value());
    }
    return jsonVal;
}
