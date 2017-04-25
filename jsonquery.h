#ifndef JSONQUERY_H
#define JSONQUERY_H

#include <QObject>
#include <QVariantMap>

class JsonQuery : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QVariantMap values READ values NOTIFY valuesChanged)

    explicit JsonQuery(QObject *parent = Q_NULLPTR);

    QString model() const;
    void setModel(QString newModel);

    QString query() const;
    void setQuery(QString newQuery);

    QVariantMap values() const;

private slots:
    void onModelUpdated(QString model);
signals:
    void modelChanged();
    void queryChanged();
    void valuesChanged();

private:
    QString m_model, m_query;
};

#endif // JSONQUERY_H
