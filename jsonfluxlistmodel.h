#ifndef JSONFLUXLISTMODEL_H
#define JSONFLUXLISTMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include <QSet>
#include <QList>
#include <QPair>

class JsonFluxView;
class JsonFluxModel;
class JsonFluxModifier;

class JsonFluxListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QStringList roles READ roles WRITE setRoles NOTIFY rolesChanged)

    explicit JsonFluxListModel(JsonFluxModel * modelObject, QString query, QStringList roles, QObject *parent = nullptr);
    ~JsonFluxListModel();

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QString query() const;
    void setQuery(QString newQuery);

    QStringList roles() const;
    void setRoles(QStringList newRoles);
signals:
    void queryChanged();
    void rolesChanged();
private slots:
    void onValuesChanged();
private:
    void removeAbsentValues(QVariantList newValues);
private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    JsonFluxView *m_fluxView = Q_NULLPTR;
    JsonFluxModifier *m_fluxModifier = Q_NULLPTR;
    QVariantList m_values;
    QString m_query = "/";
    QStringList m_roles;
};

#endif // JSONFLUXLISTMODEL_H
