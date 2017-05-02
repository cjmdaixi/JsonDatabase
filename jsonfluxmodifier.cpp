#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxmodifier.h"

JsonFluxModifier::JsonFluxModifier(QObject *parent)
    : QObject(parent)
{

}

JsonFluxModifier::~JsonFluxModifier()
{

}

JsonFluxModel * JsonFluxModifier::model() const
{
    return m_modelObject;
}

void JsonFluxModifier::setModel(JsonFluxModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;
    emit modelChanged();
}


QString JsonFluxModifier::modelName() const
{
    return m_modelObject->name();
}

void JsonFluxModifier::setModelName(QString newModelName)
{
    if(m_modelObject && (m_modelObject->name() == newModelName))
        return;

    auto newModelObj = flux()->model(newModelName);
    if(newModelObj == Q_NULLPTR){
        qWarning()<<"The new model does not exist!";
        return;
    }
    setModel(newModelObj);
}

bool JsonFluxModifier::modify(QString jsonPath, QVariant newValue)
{
    json::json_pointer ptr(jsonPath.toStdString());
    auto jsonObject = m_modelObject->jsonObject();
    try{
        (*jsonObject)[ptr] = JsonFlux::toJsonValue(newValue);
    }
    catch(std::out_of_range&){
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    QMetaObject::invokeMethod(m_modelObject, "updated", Qt::AutoConnection);

    return true;
}

bool JsonFluxModifier::modify(QString jsonPath, QVariantList newValues)
{
    json::json_pointer ptr(jsonPath.toStdString());
    auto jsonObject = m_modelObject->jsonObject();
    try{
        (*jsonObject)[ptr] = JsonFlux::toJsonArray(newValues);
    }
    catch(std::out_of_range&){
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    QMetaObject::invokeMethod(m_modelObject, "updated", Qt::AutoConnection);

    return true;
}

bool JsonFluxModifier::modify(QString jsonPath, QVariantMap newObject)
{
    json::json_pointer ptr(jsonPath.toStdString());
    auto jsonObject = m_modelObject->jsonObject();
    try{
        (*jsonObject)[ptr] = JsonFlux::toJsonObject(newObject);
    }
    catch(std::out_of_range&){
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    QMetaObject::invokeMethod(m_modelObject, "updated", Qt::AutoConnection);

    return true;
}
