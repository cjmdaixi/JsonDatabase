#include "jsonfluxlistmodel.h"
#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include <QRegularExpression>

JsonFluxListModel::JsonFluxListModel(JsonFluxModel *modelObject, QString query, QStringList roles, QObject *parent)
    : QAbstractListModel(parent), m_modelObject(modelObject), m_roles(roles)
{
    m_fluxView = new JsonFluxView(m_modelObject, this);
    m_fluxModifier = new JsonFluxModifier(m_modelObject, this);

    QObject::connect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxListModel::onValuesChanged);

    if(!m_roles.contains("$"))
        m_roles << "$";
    if(!m_roles.contains("$key"))
        m_roles << "$key";

    m_fluxView->setQuery(QStringList(query));
}

JsonFluxListModel::~JsonFluxListModel()
{

}

int JsonFluxListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_values.size();
}

QVariant JsonFluxListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_values.count())
        return QVariant();

    auto vm = m_values[index.row()].value<QVariantMap>();
    auto roleName = m_roles[role];
    if(roleName == "$"){
        return vm;
    }
    else
        return vm[roleName];
}

QHash<int, QByteArray> JsonFluxListModel::roleNames() const
{
    QHash<int, QByteArray> rolesHash;
    for(auto i = 0; i != m_roles.size(); ++i)
    {
        rolesHash[i] = m_roles[i].toLatin1();
    }
    return rolesHash;
}


void JsonFluxListModel::onValuesChanged()
{
    auto values = m_fluxView->values();

    QVariantList newValues;
    for (auto it = values.begin(); it != values.end(); ++it)
    {
        auto query = it.key();
        if(query[0] == '@')
        {  
            auto v = it.value();
            if(v.type() == QMetaType::QVariantList)
            {
                newValues = v.value<QVariantList>();
                m_modeltype = MTArray;
            }
            else if(v.type() == QMetaType::QVariantMap)
            {
                auto jsonObject = v.value<QVariantMap>();
                m_modeltype = MTObject;
                for(auto it2 = jsonObject.begin(); it2 != jsonObject.end(); ++it2)
                {
                    QVariantMap newVM;
                    newVM.insert("$key", it2.key());
                    newVM.insert("$value", it2.value());
                    newValues.push_back(newVM);
                }
            }
            break;
        }
    }

    if(newValues.empty())
        return;

    removeAbsentValues(newValues);

    auto findIf = [&](QVariant newVal)->bool{
        for(auto oldVal : m_values)
        {
            if(newVal == oldVal)
                return true;
        }
        return false;
    };

    for(auto newVal : newValues)
    {
        if(!findIf(newVal))
        {
            beginInsertRows(QModelIndex(), m_values.count(), m_values.count());
            m_values.push_back(newVal);
            endInsertRows();
        }
    }
}

void JsonFluxListModel::removeAbsentValues(QVariantList newValues)
{
    auto findIf = [&newValues](QVariant oldVal)->bool{
        for(auto newVal : newValues)
        {
            if(newVal == oldVal)
                return true;
        }
        return false;
    };

    forever
    {
        auto removed = false;

        for(auto i = 0; i != m_values.size(); ++i)
        {
            auto oldVal = m_values[i];
            if(!findIf(oldVal))
            {
                beginRemoveRows(QModelIndex(), i, i);
                m_values.removeAt(i);
                endRemoveRows();
                removed = true;
                break;
            }
        }

        if(removed == false)
            break;
    }
}

QString JsonFluxListModel::query() const
{
    return m_query;
}

void JsonFluxListModel::setQuery(QString newQuery)
{
    if(m_query == newQuery) return;

    m_query = newQuery;

    m_query.remove(QRegularExpression("/*$"));
    QStringList queries;

    for (auto query : m_roles)
    {
        queries << m_query + "/" + query;
    }

    m_fluxView->setQuery(queries);
    emit queryChanged();
}

QStringList JsonFluxListModel::roles() const
{
    return m_roles;
}

void JsonFluxListModel::setRoles(QStringList newRoles)
{
    if(newRoles == m_roles) return;

    m_roles = newRoles;
    if(!m_roles.contains("$"))
        m_roles << "$";

    emit rolesChanged();
}
