#ifndef JSONQUERY_H
#define JSONQUERY_H

#include <QObject>
#include <QVariantMap>
#include <QQmlParserStatus>

class JsonModel;

class JsonQuery : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelChanged)
    Q_PROPERTY(QStringList query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(JsonModel* model READ model WRITE setModel NOTIFY modelChanged)

    explicit JsonQuery(QObject *parent = Q_NULLPTR);
    ~JsonQuery();

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

    QString modelName() const;
    void setModelName(QString newModelName);

    JsonModel* model() const;
    void setModel(JsonModel * newModel);

    QStringList query() const;
    void setQuery(QStringList newQuery);

    QVariantMap values() const;

    bool enabled() const;
    void setEnabled(bool newEnabled);

private slots:
    void onModelUpdated(JsonModel *model);
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
    JsonModel *m_modelObject = Q_NULLPTR;
    bool m_initialized = false;
    QVariantMap m_queryValues;
};

#endif // JSONQUERY_H
