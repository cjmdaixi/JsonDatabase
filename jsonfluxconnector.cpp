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
#include <QQmlProperty>
#include <QtQml>
#include <QQuickItem>

QMap<int, JsonFluxConnector::ControlInterface> JsonFluxConnector::UIControlInterfaces =
{
    {
        JsonFluxConnector::TextField,
        {
            SIGNAL(textChanged()), "text",
            [](QObject *control)->QVariant{
                return control->property("text").value<QString>();
            },
            [](QObject *control, QVariant variant)->bool{
                return control->setProperty("text", variant);
            }
        }
    },

    {
        JsonFluxConnector::SpinBox,
        {
            SIGNAL(valueChanged()), "value",
            [](QObject *control)->QVariant{
                auto newValue = control->property("value").value<qreal>();
                auto decimals = control->property("decimals").value<int>();
                if(decimals > 0)
                {
                    return QVariant(newValue);
                }
                else
                {
                    auto intValue = int(newValue);
                    return QVariant(intValue);
                }
            },
            [](QObject *control, QVariant variant)->bool{
                return control->setProperty("value", variant);
            }
        }
    },

    {
        JsonFluxConnector::Switch,
        {
            SIGNAL(checkedChanged()), "checked",
            [](QObject *control)->QVariant{
                return control->property("checked").value<bool>();
            },
            [](QObject *control, QVariant variant)->bool{
                return control->setProperty("checked", variant);
            }
        }
    },

    {
        JsonFluxConnector::ComboBox,
        {
            SIGNAL(currentTextChanged()), "currentText",
            [](QObject *control)->QVariant{
                return control->property("currentText").value<QString>();
            },
            [](QObject *control, QVariant variant)->bool{
                auto strIndex = -1;
                QVariant varIndex;
                QMetaObject::invokeMethod(control, "find", Q_RETURN_ARG(QVariant, varIndex), Q_ARG(QVariant, variant));
                strIndex = varIndex.value<int>();
                if(strIndex == -1)
                    return false;
                else
                    return control->setProperty("currentIndex", strIndex);
            }
        }
    }/*,

    {
        JsonFluxConnector::ListView,
        {
            SIGNAL(currentTextChanged()), "currentText",
            [](QObject *control)->QVariant{
                return control->property("currentText").value<QString>();
            },
            [](QObject *control, QVariant variant)->bool{
                auto strIndex = -1;
                QVariant varIndex;
                QMetaObject::invokeMethod(control, "find", Q_RETURN_ARG(QVariant, varIndex), Q_ARG(QVariant, variant));
                strIndex = varIndex.value<int>();
                if(strIndex == -1)
                    return false;
                else
                    return control->setProperty("currentIndex", strIndex);
            }
        }
    }*/
};

JsonFluxConnector::JsonFluxConnector(JsonFluxModel *modelObject, QObject *parent)
    : QObject(parent), m_modelObject(modelObject)
{
    m_fluxView = new JsonFluxView(m_modelObject, this);
    m_fluxModifier = new JsonFluxModifier(m_modelObject, this);

    QObject::connect(m_fluxView, &JsonFluxView::valuesChanged, this, &JsonFluxConnector::onValuesChanged);
}

JsonFluxConnector::~JsonFluxConnector()
{
    //qDebug()<<"Connector destroyed";
}

bool JsonFluxConnector::enabled() const
{
    return m_enabled;
}

void JsonFluxConnector::setEnabled(bool newEnabled)
{
    if(m_enabled == newEnabled) return;

    m_enabled = newEnabled;

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
    m_fluxView->setModel(m_modelObject);
    m_fluxModifier->setModel(m_modelObject);

    emit modelChanged();
}

JsonFluxModel* JsonFluxConnector::model() const
{
    return m_modelObject;
}

void JsonFluxConnector::setModel(JsonFluxModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;
    m_fluxView->setModel(m_modelObject);
    m_fluxModifier->setModel(m_modelObject);
    emit modelChanged();
}

QVariantList JsonFluxConnector::connections() const
{
    return m_connections;
}

void JsonFluxConnector::setConnections(QVariantList newConnections)
{
    if(m_connections == newConnections) return;

    doDisconnection();
    m_connections = newConnections;

    doConnection();
    emit connectionsChanged();
}

void JsonFluxConnector::addConnection(QVariantMap newConnection)
{
    auto control = newConnection["control"].value<QObject*>();
    auto query = newConnection["query"].value<QString>();
    auto controlType = newConnection["type"].value<int>();

    Connection conn = {control, controlType, query};
    m_connectionDetails.push_back(conn);

    auto queries = m_fluxView->query();
    queries << query;

    m_fluxView->setQuery(queries);
}

void JsonFluxConnector::addConnections(QVariantList newConnections)
{
    QStringList viewQuery = m_fluxView->query();
    for(auto cnVar : newConnections)
    {
        auto connection = cnVar.value<QVariantMap>();
        auto control = connection["control"].value<QObject*>();
        auto query = connection["query"].value<QString>();
        auto controlType = connection["type"].value<int>();

        Connection conn = {control, controlType, query};
        m_connectionDetails.push_back(conn);
        viewQuery << query;
    }
    m_fluxView->setQuery(viewQuery);
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

        Connection conn = {control, controlType, query};
        m_connectionDetails.push_back(conn);
        viewQuery << query;
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
                QObject::disconnect(conn.control, controlChangeSignal(conn.controlType), this, SLOT(onControlContentChanged()));
                auto setContent = controlSetContent(conn.controlType);
                if(!setContent(conn.control, it.value())){
                    qCritical()<<"Cannot set the property:"<<controlPropretyName(conn.controlType);
                }
                QObject::connect(conn.control, controlChangeSignal(conn.controlType), this, SLOT(onControlContentChanged()));
            }
        }
    }
}

QList<JsonFluxConnector::Connection> JsonFluxConnector::searchQuery(QString query)
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

JsonFluxConnector::Connection JsonFluxConnector::searchConnection(QObject *control)
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
        QObject::disconnect(conn.control, controlChangeSignal(conn.controlType), this, SLOT(onControlContentChanged()));
    }
    m_connectionDetails.clear();
}

void JsonFluxConnector::onControlContentChanged()
{
    auto qmlName = QQmlEngine::contextForObject(sender())->nameForObject(sender());
    auto conn = searchConnection(sender());
    m_fluxModifier->setPath(conn.query);

    auto getContent = controlGetContent(conn.controlType);
    auto newValue = getContent(conn.control);

    if(!m_fluxModifier->modify(newValue))
    {
        qCritical()<<"Cannot modify control"<<conn.control->objectName();
    }
}

const char * JsonFluxConnector::controlChangeSignal(int type)
{
    return UIControlInterfaces[type].propertyChangeSignal;
}

const char* JsonFluxConnector::controlPropretyName(int type)
{
    return UIControlInterfaces[type].propertyName;
}

JsonFluxConnector::GET_CONTENT_FUNC JsonFluxConnector::controlGetContent(int type)
{
    return UIControlInterfaces[type].getContent;
}

JsonFluxConnector::SET_CONTENT_FUNC JsonFluxConnector::controlSetContent(int type)
{
    return UIControlInterfaces[type].setContent;
}
