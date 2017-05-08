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

JsonFluxView::JsonFluxView(JsonFluxModel *modelObject, QObject *parent)
    : QObject(parent), m_modelObject(modelObject)
{
    connect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
    m_initialized = true;
}

JsonFluxView::~JsonFluxView()
{
    //qDebug()<<"View destroyed";
}

void JsonFluxView::classBegin()
{
    m_initialized = false;
}

void JsonFluxView::componentComplete()
{
    if(m_enabled && m_modelObject && !m_query.isEmpty())
    {
        m_queryValues = doQuery();
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
    if(m_modelObject && (m_modelObject->name() == newModelName))
        return;

    auto newModelObj = flux()->model(newModelName);
    if(newModelObj == Q_NULLPTR)
    {
        qWarning()<<"The new model does not exist!";
        return;
    }
    m_modelObject = newModelObj;

    if(m_initialized)
    {
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

    if(m_initialized)
    {
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

    if(m_initialized)
    {
        m_queryValues = doQuery();
        emit valuesChanged();
        emit queryChanged();
    }
}

QVariantMap JsonFluxView::values() const
{
    return m_queryValues;
}

bool JsonFluxView::enabled() const
{
    return m_enabled;
}

void JsonFluxView::setEnabled(bool newEnabled)
{
    if(m_enabled == newEnabled) return;

    m_enabled = newEnabled;

    if(m_enabled)
    {
        connect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
    }
    else
    {
        disconnect(flux(), &JsonFlux::updated, this, &JsonFluxView::onModelUpdated);
    }
    if(m_initialized)
    {
        emit enabledChanged();
    }
}

void JsonFluxView::onModelUpdated(JsonFluxModel *model)
{
    if(m_modelObject && model == m_modelObject)
    {
        auto newValues = doQuery();
        if(newValues != m_queryValues)
        {
            m_queryValues = newValues;
            emit valuesChanged();
        }
    }
}

QVariantMap JsonFluxView::doQuery() const
{
    QVariantMap results;
    auto jsonObject = m_modelObject->jsonObject();
    for(int i = 0; i != m_query.size(); ++i)
    {
        auto &oneQuery = m_query[i];
        //auto foundNode = true;
        json::json_pointer ptr(oneQuery.toStdString());
        try
        {
            if ((*jsonObject)[ptr] != json::value_t::null)
            {
                auto jsonVal = (*jsonObject)[ptr];
                auto v = JsonFlux::toVariant(jsonVal);
                results[oneQuery] = v;
                results[QString("@%1").arg(i)] = v;
            }
            else
            {
                qDebug() << oneQuery << "doesn't exist!";
            }
        }
        catch (std::out_of_range&)
        {
            qCritical() << oneQuery << "doesn't exist!";
        }
    }
    return results;
}
