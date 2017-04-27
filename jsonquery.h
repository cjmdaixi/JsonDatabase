#ifndef JSONQUERY_H
#define JSONQUERY_H

#include <QObject>
#include <QVariantMap>

class JsonModel;

class JsonQuery : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QStringList query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    explicit JsonQuery(QObject *parent = Q_NULLPTR);
    ~JsonQuery();

    QString model() const;
    void setModel(QString newModel);

    QStringList query() const;
    void setQuery(QStringList newQuery);

    QVariantMap values() const;

    bool enabled() const;
    void setEnabled(bool newEnabled);

private slots:
    void onModelUpdated(QString modelName);
signals:
    void modelChanged();
    void queryChanged();
    void valuesChanged();
    void enabledChanged();

private:
    QString m_model;
    QStringList m_query;
    bool m_enabled = true;
};

#endif // JSONQUERY_H
