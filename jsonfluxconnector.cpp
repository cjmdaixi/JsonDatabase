#include "jsonfluxconnector.h"
#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QQmlEngine>
#include <QQmlContext>

JsonFluxConnector::JsonFluxConnector(QObject *parent)
    : QObject(parent)
{
    m_fluxView = new JsonFluxView(this);
    m_fluxModifier = new JsonFluxModifier(this);
}


void JsonFluxConnector::classBegin()
{
    m_initialized = false;
}

void JsonFluxConnector::componentComplete()
{
    if(m_enabled && m_modelObject)
    {
        emit modelChanged();
        if(!m_connections.empty())
        {
            doConnection();
            emit connectionsChanged();
        }

        QObject::connect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxConnector::onValuesChanged);
        emit enabledChanged();
    }
    m_initialized = true;
}

bool JsonFluxConnector::enabled() const
{
    return m_enabled;
}

void JsonFluxConnector::setEnabled(bool newEnabled)
{
    if(m_enabled == newEnabled) return;

    m_enabled = newEnabled;

    if(m_initialized)
    {
        emit enabledChanged();
        if(m_enabled)
        {
            QObject::connect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxConnector::onValuesChanged);
        }
        else
        {
            QObject::disconnect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxConnector::onValuesChanged);
        }
    }
}

QString JsonFluxConnector::modelName() const
{
    return m_modelObject->name();
}

void JsonFluxConnector::setModelName(QString newModelName)
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

JsonFluxModel* JsonFluxConnector::model() const
{
    return m_modelObject;
}

void JsonFluxConnector::setModel(JsonFluxModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;

    if(m_initialized)
    {
        emit modelChanged();
    }
}

QVariantList JsonFluxConnector::connections() const
{
    return m_connections;
}

void JsonFluxConnector::setConnections(QVariantList newConnections)
{
    if(m_connections == newConnections) return;

    m_connections = newConnections;

    if(m_initialized)
    {
        doDisconnection();
        doConnection();
        emit connectionsChanged();
    }
}

void JsonFluxConnector::doConnection()
{
    QStringList viewQuery;
    for(auto cnVar : m_connections)
    {
        auto connection = cnVar.value<QVariantMap>();
        auto control = connection["control"].value<QObject*>();
        auto query = connection["query"].value<QString>();
        auto controlType = connection["type"].value<int>();

        Connection conn (control, controlType, query);
        m_connectionDetails.push_back(conn);

        viewQuery << query;

        if(controlType == TextField)
        {
            QObject::connect(control, SIGNAL(editingFinished()), this, SLOT(onTextFieldEditingFinished()));
        }
        else if(controlType == SpinBox)
        {

        }
    }
    m_fluxView->setQuery(viewQuery);
}

void JsonFluxConnector::onValuesChanged()
{
    auto values = m_fluxView->values();
    for (auto it = values.begin(); it != values.end(); ++it)
    {
        auto query = it.key();
        if(query[0] != '@')
        {
            auto conns = searchQuery(query);
            for(auto &conn : conns)
            {
                switch (conn.controlType) {
                case TextField:
                    conn.control->setProperty("text", it.value());
                    break;
                default:
                    break;
                }
            }
        }
    }
}

QList<Connection> JsonFluxConnector::searchQuery(QString query)
{
    QList<Connection> list;
    for(auto &conn : m_connectionDetails)
    {
        if(conn.query == query)
        {
            list.append(conn);
        }
    }
    return list;
}

Connection JsonFluxConnector::searchControl(QObject *control)
{
    for(auto &conn : m_connectionDetails)
    {
        if(conn.control == control)
            return conn;
    }
    return Connection();
}

void JsonFluxConnector::doDisconnection()
{
    for(auto conn : m_connectionDetails)
    {
        if(conn.controlType == TextField)
        {
            // disconnect
            //QObject::disconnect(control, );
        }
        else if(conn.controlType == SpinBox)
        {

        }

    }
    m_connectionDetails.clear();
}

void JsonFluxConnector::onTextFieldEditingFinished()
{
    auto textFieldCtrl = sender();
    auto conn = searchControl(textFieldCtrl);
    auto newText = textFieldCtrl->property("text").value<QString>();
    m_fluxModifier->setPath(conn.query);
    if(!m_fluxModifier->modify(newText))
    {
        qCritical()<<"Cannot modify control"<<textFieldCtrl->objectName();
    }
}

void JsonFluxConnector::onSpinBoxEditingFinished()
{

}
