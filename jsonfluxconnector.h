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

class JsonFluxConnector : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    enum ControlType{
        TextField,
        SpinBox,
        Switch
    };
    Q_ENUM(ControlType)

    typedef QVariant (*GET_CONTENT_FUNC)(QObject *control);

    struct ControlInterface{
        const char * propertyChangeSignal;
        const char * propertyName;
        GET_CONTENT_FUNC getContent;
    };

    struct Connection{
        QObject *control;
        int controlType;
        QString query;
    };

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
    void onControlContentChanged();
    void onValuesChanged();
private:
    void doConnection();
    void doDisconnection();
    QList<Connection> searchQuery(QString query);
    Connection searchConnection(QObject *control);

    inline const char * controlChangeSignal(int type);
    inline const char *controlPropretyName(int type);
    inline GET_CONTENT_FUNC controlGetContent(int type);
private:
    bool m_enabled = true, m_initialized = false;
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QVariantList m_connections;
    JsonFluxView* m_fluxView = Q_NULLPTR;
    JsonFluxModifier* m_fluxModifier = Q_NULLPTR;
    QList<Connection> m_connectionDetails;
    static QMap<int, ControlInterface> c_controlInterfaces;
};

#endif // JSONFLUXCONNECTOR_H
