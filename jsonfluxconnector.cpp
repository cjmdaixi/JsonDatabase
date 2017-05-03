#include "jsonfluxconnector.h"
#include "jsonflux.h"
#include "jsonfluxmodel.h"
#include "jsonfluxview.h"
#include "jsonfluxmodifier.h"
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>

static QString SpinBox = QStringLiteral("SpinBox");
static QString TextField = QStringLiteral("TextField");

JsonFluxConnector::JsonFluxConnector(QObject *parent)
    : QObject(parent)
{

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
        doConnection();
        emit connectionsChanged();
    }
}

void JsonFluxConnector::doConnection()
{
    for(auto cnVar : m_connections)
    {
        auto connection = cnVar.value<QVariantMap>();
        auto control = connection["control"].value<QObject*>();
        auto query = connection["query"].value<QString>();

        auto mo = control->metaObject();
        auto cn = QString(mo->className());
        if(cn == TextField)
        {
            auto view = new JsonFluxView(this);
            view->setModel(m_modelObject);
            view->setQuery(QStringList(query));
            QObject::connect(view, &JsonFluxView::valuesChanged, this, [this, control, view](){
                auto values = view->values();
                control->setProperty("text", values[0]);
            });
            m_fluxViews[control] = view;

            auto modifier = new JsonFluxModifier(this);
            modifier->setModel(m_modelObject);
            modifier->setPath(query);
            QObject::connect(control, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
            m_fluxModifiers[control] = modifier;
        }
        else if(cn == SpinBox)
        {

        }
    }
}

void JsonFluxConnector::doDisconnection()
{
    for(auto cnVar : m_connections)
    {
        auto connection = cnVar.value<QVariantMap>();
        auto control = connection["control"].value<QObject*>();
        auto mo = control->metaObject();
        auto cn = QString(mo->className());
        if(cn == TextField)
        {
            // disconnect
            //QObject::disconnect(control, );
        }
        else if(cn == SpinBox)
        {

        }

    }
}

void JsonFluxConnector::onTextFieldEditingFinished()
{
    auto textFieldCtrl = sender();
    auto modifier = m_fluxModifiers[textFieldCtrl];
    auto newText = textFieldCtrl->property("text").value<QString>();
    if(!modifier->modify(newText))
    {
        qCritical()<<"Cannot modify control"<<textFieldCtrl->objectName();
    }
}

void JsonFluxConnector::onSpinBoxEditingFinished()
{

}
