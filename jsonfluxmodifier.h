#ifndef JSONFLUXMODIFIER_H
#define JSONFLUXMODIFIER_H

#include <QObject>
#include <QVariant>

class JsonFluxModel;

class JsonFluxModifier : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(JsonFluxModel* model READ model WRITE setModel NOTIFY modelChanged)
    explicit JsonFluxModifier(QObject *parent = 0);
    ~JsonFluxModifier();

    JsonFluxModel * model() const;
    void setModel(JsonFluxModel *newModel);

    Q_INVOKABLE bool modify(QString jsonPath, QVariant newValue);
signals:
    void modelChanged();
private:

private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
};

#endif // JSONFLUXMODIFIER_H
