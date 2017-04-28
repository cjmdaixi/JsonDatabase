#include "jsonquery.h"
#include "jsondatabase.h"
#include "jsonmodel.h"
#include <QtDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonValue>
#include <QVariantMap>

JsonQuery::JsonQuery(QObject *parent)
    : QObject(parent)
{
    connect(database(), &JsonDatabase::updated, this, &JsonQuery::onModelUpdated);
}

JsonQuery::~JsonQuery()
{
}

void JsonQuery::classBegin()
{
    m_initialized = false;
}

void JsonQuery::componentComplete()
{
    if(m_enabled && m_modelObject && !m_query.isEmpty()){
        emit enabledChanged();
        emit modelChanged();
        emit queryChanged();
        emit valuesChanged();
    }
    m_initialized = true;
}

QString JsonQuery::modelName() const
{
    return m_modelObject->name();
}

void JsonQuery::setModelName(QString newModelName)
{
    if(m_modelObject->name() == newModelName) return;

    auto newModelObj = database()->model(newModelName);
    if(newModelObj == Q_NULLPTR){
        qWarning()<<"The new model does not exist!";
        return;
    }
    m_modelObject = newModelObj;

    if(m_initialized){
        emit modelChanged();
    }
}

JsonModel* JsonQuery::model() const
{
    return m_modelObject;
}

void JsonQuery::setModel(JsonModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;

    if(m_initialized){
        emit modelChanged();
        emit valuesChanged();
    }
}

QStringList JsonQuery::query() const
{
    return m_query;
}

void JsonQuery::setQuery(QStringList newQuery)
{
    if(m_query == newQuery) return;

    m_query = newQuery;

    if(m_initialized){
        emit valuesChanged();
        emit queryChanged();
    }
}

QVariantMap JsonQuery::values() const
{
    QVariantMap results;

    if(m_modelObject == Q_NULLPTR)
        return results;

    results = doQuery();

    return results;
}

bool JsonQuery::enabled() const
{
    return m_enabled;
}

void JsonQuery::setEnabled(bool newEnabled)
{
    if(m_enabled == newEnabled) return;

    m_enabled = newEnabled;

    if(m_enabled){
        connect(database(), &JsonDatabase::updated, this, &JsonQuery::onModelUpdated);
    }else{
        disconnect(database(), &JsonDatabase::updated, this, &JsonQuery::onModelUpdated);
    }
    if(m_initialized){
        emit enabledChanged();
    }
}

void JsonQuery::onModelUpdated(JsonModel *model)
{
    if(m_modelObject && model == m_modelObject)
        emit valuesChanged();
}

QVariantMap JsonQuery::doQuery() const
{
    QVariantMap results;
    QRegularExpression re("^(?<node>\\w+)(\\[(?<idx>\\d*)\\])?$");
    for(int i = 0; i != m_query.size(); ++i){
        auto &oneQuery = m_query[i];
        auto nodeList = oneQuery.split('.');
        QJsonValue jsonVal = m_modelObject->jsonObject();
        auto foundNode = true;
        for(auto &node : nodeList){
            if(!jsonVal.isObject()){
                qDebug()<<"the json value is not an object!";
                foundNode = false;
                break;
            }
            auto jsonObj = jsonVal.toObject();

            auto match = re.match(node);
            if(!match.hasMatch()){
                qDebug()<<"invalid node name!";
                foundNode = false;
                break;
            }
            auto nodeName = match.captured("node");
            auto nodeIdx = match.captured("idx");
            if(!jsonObj.contains(nodeName)){
                qDebug()<<"no such node"<<nodeName;
                foundNode = false;
                break;
            }
            jsonVal = jsonObj[nodeName];
            if(nodeIdx != ""){
                if(!jsonVal.isArray()){
                    qDebug()<<"the node is not an array!";
                    foundNode = false;
                    break;
                }
                auto idx = nodeIdx.toInt();
                jsonVal = jsonVal.toArray()[idx];
            }
        }
        if(foundNode){
            results[oneQuery] = jsonVal.toVariant();
            results[QString("@%1").arg(i)] = jsonVal.toVariant();
        }
    }
    return results;
}
