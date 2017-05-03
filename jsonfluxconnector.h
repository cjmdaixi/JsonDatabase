#ifndef JSONFLUXCONNECTOR_H
#define JSONFLUXCONNECTOR_H

#include <QObject>
#include <QQmlParserStatus>
#include <QVariantList>
#include <QVariantMap>
#include <QMap>

class JsonFluxModel;
class JsonFluxView;
class JsonFluxModifier;

struct Connection{
    QObject *control = Q_NULLPTR;
    int controlType = 0;
    QString query;
    Connection(){}
    Connection(QObject *c, int t, QString q)
        : control(c), controlType(t), query(q)
    {}
};

class JsonFluxConnector : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    enum ControlType{
        TextField,
        SpinBox
    };
    Q_ENUM(ControlType)

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

    void onValuesChanged();
private:
    void doConnection();
    void doDisconnection();
    QList<Connection> searchQuery(QString query);
    Connection searchControl(QObject *control);
private:
    bool m_enabled = true, m_initialized = false;
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QVariantList m_connections;
    JsonFluxView* m_fluxView = Q_NULLPTR;
    JsonFluxModifier* m_fluxModifier = Q_NULLPTR;
    QList<Connection> m_connectionDetails;
};

#endif // JSONFLUXCONNECTOR_H
