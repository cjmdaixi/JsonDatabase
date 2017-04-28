#include "jsonfluxmodel.h"
#include "jsonflux.h"
#include <QtDebug>

JsonFluxModel::JsonFluxModel(QObject *parent)
    : QObject(parent)
{
    connect(this, &JsonFluxModel::updated, flux(), &JsonFlux::onModelUpdated);
}

JsonFluxModel::~JsonFluxModel()
{
    flux()->unregisterModel(m_name);
}

QString JsonFluxModel::name() const
{
    return m_name;
}

void JsonFluxModel::setName(QString newName)
{
    if(m_name == newName) return;

    m_name = newName;

    flux()->registerModel(m_name, this);
    emit nameChanged();
}

QString JsonFluxModel::source() const
{
    return m_source;
}

void JsonFluxModel::setSource(QString newSource)
{
    if(m_source == newSource) return;

    QFile infile(newSource);
    if(!infile.open(QIODevice::ReadOnly)){
        qCritical()<<"Cannot open the json file"<<newSource;
        return;
    }

    auto jsonDoc = QJsonDocument::fromJson(infile.readAll());
    if(jsonDoc.isNull()){
        qCritical()<<"The file is not a valid json file!";
        return;
    }
    infile.close();

    m_jsonObject = jsonDoc.object();
    if(m_jsonObject.isEmpty()){
        qCritical()<<"The file is not a valid json file!";
        return;
    }

    m_source = newSource;

    emit updated();
    emit sourceChanged();
}

QVariantMap JsonFluxModel::data() const
{
    return m_jsonObject.toVariantMap();
}

void JsonFluxModel::setData(QVariantMap newData)
{
    auto newJsonObj = QJsonObject::fromVariantMap(newData);
    if(newJsonObj.isEmpty()){
        qCritical()<<"The data is not a valid json!";
        return;
    }
    m_jsonObject = newJsonObj;
    emit updated();
}

QJsonObject JsonFluxModel::jsonObject() const
{
    return m_jsonObject;
}
