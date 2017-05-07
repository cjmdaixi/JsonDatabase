#include "jsonfluxlistmodel.h"
#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include <QRegularExpression>

JsonFluxListModel::JsonFluxListModel(JsonFluxModel *modelObject, QString query, QObject *parent)
    : QAbstractListModel(parent), m_modelObject(modelObject)
{
    m_fluxView = new JsonFluxView(m_modelObject, this);
    m_fluxModifier = new JsonFluxModifier(m_modelObject, this);

    QObject::connect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxListModel::onValuesChanged);
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
    Q_UNUSED(role)
    if (index.row() < 0 || index.row() >= m_values.count())
        return QVariant();

    auto vm = m_values[index.row()].value<QVariantMap>();
    qDebug()<<"role:"<<vm["xname"].value<QString>();
    return vm["xname"].value<QString>();
}

QHash<int, QByteArray> JsonFluxListModel::roleNames() const
{
    return m_roles;
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
            auto queryIdx = query.right(query.length() - 1).toInt();
            auto role = m_rolesAndQueries[queryIdx].first;
            newValues.push_back(it.value());
        }
    }

    if(newValues.size() != 1 || newValues[0].type() != QMetaType::QVariantList)
        return;

    auto newVariantList = newValues[0].value<QVariantList>();

    removeAbsentValues(newVariantList);

    auto findIf = [&](QVariant newVal)->bool{
        for(auto oldVal : m_values)
        {
            if(newVal == oldVal)
                return true;
        }
        return false;
    };

    for(auto newVal : newVariantList)
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

QString JsonFluxListModel::rootPath() const
{
    return m_rootPath;
}

void JsonFluxListModel::setRootPath(QString newRootPath)
{
    if(m_rootPath == newRootPath) return;

    m_rootPath = newRootPath;

    m_rootPath.remove(QRegularExpression("/*$"));
    QStringList queries;

    for (auto p : m_rolesAndQueries)
    {
        queries << m_rootPath + "/" + p.second;
    }

    m_fluxView->setQuery(queries);
    emit rootPathChanged();
}

QVariantList JsonFluxListModel::rolesAndQueries() const
{
    QVariantList results;
    for(auto it = m_rolesAndQueries.begin(); it != m_rolesAndQueries.end(); ++it)
    {
        QVariantMap vm;
        vm["role"] = it->first;
        vm["query"] = it->second;
        results.push_back(vm);
    }
    return results;
}

void JsonFluxListModel::setRolesAndQueries(QVariantList newRolesAndQueries)
{
    decltype(m_rolesAndQueries) newRAQ;
    for(auto v : newRolesAndQueries)
    {
        auto vm = v.value<QVariantMap>();
        auto r = vm["role"].value<QString>();
        auto q = vm["query"].value<QString>();
        newRAQ.push_back(QPair<QString, QString>(r, q));
    }

    if(newRAQ == m_rolesAndQueries) return;

    m_rolesAndQueries = newRAQ;
    m_roles.clear();
    for(auto i = 0; i != m_rolesAndQueries.size(); ++i)
    {
        m_roles[i] = m_rolesAndQueries[i].first.toLatin1();
    }

    emit rolesAndQueriesChanged();
}
