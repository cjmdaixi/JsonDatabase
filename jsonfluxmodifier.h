#ifndef JSONFLUXMODIFIER_H
#define JSONFLUXMODIFIER_H

#include <QObject>
#include <QVariant>
#include "json.h"
using namespace nlohmann;

class JsonFluxModel;

class JsonFluxModifier : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(JsonFluxModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelChanged)
    Q_PROPERTY(bool fileSync READ fileSync WRITE setFileSync NOTIFY fileSyncChanged)

    explicit JsonFluxModifier(QObject *parent = 0);
    ~JsonFluxModifier();

    JsonFluxModel * model() const;
    void setModel(JsonFluxModel *newModel);

    QString modelName() const;
    void setModelName(QString newModelName);

    bool fileSync() const;
    void setFileSync(bool newFileSync);

    Q_INVOKABLE bool modify(QString jsonPath, QVariant newValue);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantList newValues);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantMap newObject);
signals:
    void modelChanged();
    void fileSyncChanged();
private:

private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    bool m_fileSync = false;
};

#endif // JSONFLUXMODIFIER_H
