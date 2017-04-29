#ifndef JSONFLUXVIEW_H
#define JSONFLUXVIEW_H

#include <QObject>
#include <QVariantMap>
#include <QQmlParserStatus>
#include "json.h"
using namespace nlohmann;

class JsonFluxModel;

class JsonFluxView : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelChanged)
    Q_PROPERTY(QStringList query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(JsonFluxModel* model READ model WRITE setModel NOTIFY modelChanged)

    explicit JsonFluxView(QObject *parent = Q_NULLPTR);
    ~JsonFluxView();

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

    QString modelName() const;
    void setModelName(QString newModelName);

    JsonFluxModel* model() const;
    void setModel(JsonFluxModel * newModel);

    QStringList query() const;
    void setQuery(QStringList newQuery);

    QVariantMap values() const;

    bool enabled() const;
    void setEnabled(bool newEnabled);

private slots:
    void onModelUpdated(JsonFluxModel *model);
private:
    QVariantMap doQuery() const;
signals:
    void modelNameChanged();
    void modelChanged();
    void queryChanged();
    void valuesChanged();
    void enabledChanged();

private:
    QStringList m_query;
    bool m_enabled = true;
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    bool m_initialized = false;
    QVariantMap m_queryValues;
};

#endif // JSONFLUXVIEW_H
