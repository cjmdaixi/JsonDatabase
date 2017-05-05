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
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

    explicit JsonFluxModifier(QObject *parent = 0);
    explicit JsonFluxModifier(JsonFluxModel *modelObject, QObject *parent = Q_NULLPTR);
    ~JsonFluxModifier();

    JsonFluxModel * model() const;
    void setModel(JsonFluxModel *newModel);

    QString modelName() const;
    void setModelName(QString newModelName);

    QString path() const;
    void setPath(QString newPath);

    Q_INVOKABLE bool modify(QString jsonPath, QVariant newValue);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantList newValues);
    Q_INVOKABLE bool modify(QString jsonPath, QVariantMap newObject);

    Q_INVOKABLE bool modify(QVariant newValue);
    Q_INVOKABLE bool modify(QVariantList newValues);
    Q_INVOKABLE bool modify(QVariantMap newObject);
signals:
    void modelChanged();
    void pathChanged();
private:

private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QString m_path = "/";
};

#endif // JSONFLUXMODIFIER_H
