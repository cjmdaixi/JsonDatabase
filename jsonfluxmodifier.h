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

    explicit JsonFluxModifier(QObject *parent = 0);
    ~JsonFluxModifier();

    JsonFluxModel * model() const;
    void setModel(JsonFluxModel *newModel);

    QString modelName() const;
    void setModelName(QString newModelName);

    Q_INVOKABLE bool modify(QString jsonPath, QVariant newValue);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantList newValues);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantMap newObject);
signals:
    void modelChanged();
private:

private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
};

#endif // JSONFLUXMODIFIER_H
