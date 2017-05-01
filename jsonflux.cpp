#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"

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
