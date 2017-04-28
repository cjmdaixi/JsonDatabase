#include "jsonfluxview.h"
#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include <QtDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QJsonValue>
#include <QVariantMap>

JsonFluxView::JsonFluxView(QObject *parent)
    : QObject(parent)
{
    connect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
}

JsonFluxView::~JsonFluxView()
{
}

void JsonFluxView::classBegin()
{
    m_initialized = false;
}

void JsonFluxView::componentComplete()
{
    if(m_enabled && m_modelObject && !m_query.isEmpty()){
        emit enabledChanged();
        emit modelChanged();
        emit queryChanged();
        emit valuesChanged();
    }
    m_initialized = true;
}

QString JsonFluxView::modelName() const
{
    return m_modelObject->name();
}

void JsonFluxView::setModelName(QString newModelName)
{
    if(m_modelObject->name() == newModelName) return;

    auto newModelObj = flux()->model(newModelName);
    if(newModelObj == Q_NULLPTR){
        qWarning()<<"The new model does not exist!";
        return;
    }
    m_modelObject = newModelObj;

    if(m_initialized){
        emit modelChanged();
    }
}

JsonFluxModel* JsonFluxView::model() const
{
    return m_modelObject;
}

void JsonFluxView::setModel(JsonFluxModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;

    if(m_initialized){
        emit modelChanged();
        emit valuesChanged();
    }
}

QStringList JsonFluxView::query() const
{
    return m_query;
}

void JsonFluxView::setQuery(QStringList newQuery)
{
    if(m_query == newQuery) return;

    m_query = newQuery;

    if(m_initialized){
        emit valuesChanged();
        emit queryChanged();
    }
}

QVariantMap JsonFluxView::values() const
{
    QVariantMap results;

    if(m_modelObject == Q_NULLPTR)
        return results;

    results = doQuery();

    return results;
}

bool JsonFluxView::enabled() const
{
    return m_enabled;
}

void JsonFluxView::setEnabled(bool newEnabled)
{
    if(m_enabled == newEnabled) return;

    m_enabled = newEnabled;

    if(m_enabled){
        connect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
    }else{
        disconnect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
    }
    if(m_initialized){
        emit enabledChanged();
    }
}

void JsonFluxView::onModelUpdated(JsonFluxModel *model)
{
    if(m_modelObject && model == m_modelObject)
        emit valuesChanged();
}

QVariantMap JsonFluxView::doQuery() const
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
