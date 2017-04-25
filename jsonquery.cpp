#include "jsonquery.h"

JsonQuery::JsonQuery(QObject *parent)
    : QObject(parent)
{

}

QString JsonQuery::model() const
{
    return m_model;
}

void JsonQuery::setModel(QString newModel)
{
    if(m_model == newModel) return;

    m_model = newModel;

    emit modelChanged();
}

QString JsonQuery::query() const
{
    return m_query;
}

void JsonQuery::setQuery(QString newQuery)
{
    if(m_query == newQuery) return;

    m_query = newQuery;

    emit queryChanged();
}

QVariantMap JsonQuery::values() const
{
    return QVariantMap();
}


