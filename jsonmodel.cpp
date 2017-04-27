#include "jsonmodel.h"
#include "jsondatabase.h"
#include <QtDebug>

JsonModel::JsonModel(QObject *parent)
    : QObject(parent)
{
    connect(this, &JsonModel::updated, database(), &JsonDatabase::updated);
}

JsonModel::~JsonModel()
{
    database()->unregisterModel(m_name);
}

QString JsonModel::name() const
{
    return m_name;
}

void JsonModel::setName(QString newName)
{
    if(m_name == newName) return;

    m_name = newName;

    database()->registerModel(m_name, this);
    emit nameChanged();
}

QString JsonModel::source() const
{
    return m_source;
}

void JsonModel::setSource(QString newSource)
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

    emit sourceChanged();
}

QVariantMap JsonModel::data() const
{
    return m_jsonObject.toVariantMap();
}

void JsonModel::setData(QVariantMap newData)
{
    auto newJsonObj = QJsonObject::fromVariantMap(newData);
    if(newJsonObj.isEmpty()){
        qCritical()<<"The data is not a valid json!";
        return;
    }
    m_jsonObject = newJsonObj;
    emit updated(m_name);
}

QJsonObject JsonModel::jsonObject() const
{
    return m_jsonObject;
}
