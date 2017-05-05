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

class JsonFluxConnector : public QObject
{
    Q_OBJECT
public:
    enum ControlType{
        TextField,
        SpinBox,
        Switch,
        ComboBox
    };
    Q_ENUM(ControlType)

    typedef QVariant (*GET_CONTENT_FUNC)(QObject *control);
    typedef bool (*SET_CONTENT_FUNC)(QObject *control, QVariant variant);

    struct ControlInterface{
        const char * propertyChangeSignal;
        const char * propertyName;
        GET_CONTENT_FUNC getContent;
        SET_CONTENT_FUNC setContent;
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

    explicit JsonFluxConnector(JsonFluxModel *modelObject, QObject *parent = 0);
    ~JsonFluxConnector();

    bool enabled() const;
    void setEnabled(bool newEnabled);

    QString modelName() const;
    void setModelName(QString newModelName);

    JsonFluxModel* model() const;
    void setModel(JsonFluxModel * newModel);

    QVariantList connections() const;
    void setConnections(QVariantList newConnections);


    Q_INVOKABLE void addConnection(QVariantMap newConnection);
    Q_INVOKABLE void addConnections(QVariantList newConnections);
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
    inline SET_CONTENT_FUNC controlSetContent(int type);
private:
    bool m_enabled = true;
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QVariantList m_connections;
    JsonFluxView* m_fluxView = Q_NULLPTR;
    JsonFluxModifier* m_fluxModifier = Q_NULLPTR;
    QList<Connection> m_connectionDetails;
    static QMap<int, ControlInterface> c_controlInterfaces;
};

#endif // JSONFLUXCONNECTOR_H
