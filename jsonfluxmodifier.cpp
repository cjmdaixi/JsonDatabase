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
    //qDebug()<<"Modifier destroyed";
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

bool JsonFluxModifier::modify(ModifyType modifyType, QString jsonPath, QVariant value)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(jsonPath, value);
    case Append:
        return m_modelObject->append(jsonPath, value);
    case Remove:
        if(value.type() == QMetaType::Int)
            return m_modelObject->remove(jsonPath, value.value<int>());
        else if(value.type() == QMetaType::QString)
            return m_modelObject->remove(jsonPath, value.value<QString>());
    default:
        break;
    }
    return false;
}

bool JsonFluxModifier::modify(ModifyType modifyType, QString jsonPath, QVariantList values)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(jsonPath, values);
    case Append:
        return m_modelObject->append(jsonPath, values);
    default:
        break;
    }
    return false;
}

bool JsonFluxModifier::modify(ModifyType modifyType, QString jsonPath, QVariantMap object)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(jsonPath, object);
    case Append:
        return m_modelObject->append(jsonPath, object);
    default:
        break;
    }
    return false;
}


bool JsonFluxModifier::modify(ModifyType modifyType, QVariant value)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(m_path, value);
    case Append:
        return m_modelObject->append(m_path, value);
    default:
        break;
    }
    return false;
}

bool JsonFluxModifier::modify(ModifyType modifyType, QVariantList values)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(m_path, values);
    case Append:
        return m_modelObject->append(m_path, values);
    default:
        break;
    }
    return false;
}

bool JsonFluxModifier::modify(ModifyType modifyType, QVariantMap object)
{
    if(m_modelObject == Q_NULLPTR) return false;
    switch (modifyType)
    {
    case ReplaceOrInsert:
        return m_modelObject->modify(m_path, object);
    case Append:
        return m_modelObject->append(m_path, object);
    default:
        break;
    }
    return false;
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
