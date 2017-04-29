#include "jsonfluxmodel.h"
#include "jsonflux.h"
#include <QtDebug>
#include <fstream>
#include "json.h"
using namespace nlohmann;

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

    std::ifstream inf(newSource.toStdString());

    if(!inf.is_open()){
        qCritical()<<"Cannot open the json file"<<newSource;
        return;
    }

    inf >> m_json;

    m_source = newSource;

    emit updated();
    emit sourceChanged();
}

QVariantMap JsonFluxModel::data() const
{
    auto str = m_json.dump();

    auto jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(str));
    return jsonDoc.object().toVariantMap();
}

void JsonFluxModel::setData(QVariantMap newData)
{
    auto newJsonObj = QJsonObject::fromVariantMap(newData);
    if(newJsonObj.isEmpty()){
        qCritical()<<"The data is not a valid json!";
        return;
    }
    QJsonDocument jsonDoc;
    jsonDoc.setObject(newJsonObj);
    auto jsonStr = jsonDoc.toJson();
    m_json = json::parse(jsonStr.data());
    emit updated();
}

json* JsonFluxModel::jsonObject()
{
    return &m_json;
}
