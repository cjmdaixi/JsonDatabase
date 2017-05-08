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
    enum ModifyType
    {
        ReplaceOrInsert,
        Append,
        Remove
    };
    Q_ENUM(ModifyType)

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

    Q_INVOKABLE bool modify(ModifyType modifyType, QString jsonPath, QVariant value);
    Q_INVOKABLE bool modify(ModifyType modifyType, QString jsonPath, QVariantList values);
    Q_INVOKABLE bool modify(ModifyType modifyType, QString jsonPath, QVariantMap object);

    Q_INVOKABLE bool modify(ModifyType modifyType, QVariant value);
    Q_INVOKABLE bool modify(ModifyType modifyType, QVariantList values);
    Q_INVOKABLE bool modify(ModifyType modifyType, QVariantMap object);
signals:
    void modelChanged();
    void pathChanged();
private:

private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    QString m_path = "/";
};

#endif // JSONFLUXMODIFIER_H
