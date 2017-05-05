#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxmodifier.h"

JsonFluxModifier::JsonFluxModifier(QObject *parent)
    : QObject(parent)
{

}

JsonFluxModifier::JsonFluxModifier(JsonFluxModel *modelObject, QObject *parent)
    : QObject(parent), m_modelObject(modelObject)
{

}

JsonFluxModifier::~JsonFluxModifier()
{
    qDebug()<<"Modifier destroyed";
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
    if(newModelObj == Q_NULLPTR)
    {
        qWarning()<<"The new model does not exist!";
        return;
    }
    setModel(newModelObj);
}

bool JsonFluxModifier::modify(QString jsonPath, QVariant newValue)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(jsonPath, newValue);
}

bool JsonFluxModifier::modify(QString jsonPath, QVariantList newValues)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(jsonPath, newValues);
}

bool JsonFluxModifier::modify(QString jsonPath, QVariantMap newObject)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(jsonPath, newObject);
}


bool JsonFluxModifier::modify(QVariant newValue)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(m_path, newValue);
}

bool JsonFluxModifier::modify(QVariantList newValues)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(m_path, newValues);
}

bool JsonFluxModifier::modify(QVariantMap newObject)
{
    if(m_modelObject == Q_NULLPTR) return false;
    return m_modelObject->modify(m_path, newObject);
}

QString JsonFluxModifier::path() const
{
    return m_path;
}

void JsonFluxModifier::setPath(QString newPath)
{
    if(m_path == newPath) return;

    m_path = newPath;
    emit pathChanged();
}
