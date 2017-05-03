#ifndef JSONFLUXCONNECTOR_H
#define JSONFLUXCONNECTOR_H

#include <QObject>
#include <QQmlParserStatus>
#include <QVariantList>
#include <QMap>

class JsonFluxModel;
class JsonFluxView;
class JsonFluxModifier;

class JsonFluxConnector : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelChanged)
    Q_PROPERTY(JsonFluxModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QVariantList connections READ connections WRITE setConnections NOTIFY connectionsChanged)

    explicit JsonFluxConnector(QObject *parent = 0);

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

    bool enabled() const;
    void setEnabled(bool newEnabled);

    QString modelName() const;
    void setModelName(QString newModelName);

    JsonFluxModel* model() const;
    void setModel(JsonFluxModel * newModel);

    QVariantList connections() const;
    void setConnections(QVariantList newConnections);


signals:
    void enabledChanged();
    void modelNameChanged();
    void modelChanged();
    void connectionsChanged();
private slots:
    void onTextFieldEditingFinished();
    void onSpinBoxEditingFinished();
private:
    void doConnection();
    void doDisconnection();
private:
    bool m_enabled = true, m_initialized = false;
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QVariantList m_connections;
    QMap<QObject*, JsonFluxView*> m_fluxViews;
    QMap<QObject*, JsonFluxModifier*> m_fluxModifiers;
};

#endif // JSONFLUXCONNECTOR_H
