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

    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged)
    Q_PROPERTY(QVariantList rolesAndQueries READ rolesAndQueries WRITE setRolesAndQueries NOTIFY rolesAndQueriesChanged)

    explicit JsonFluxListModel(JsonFluxModel * modelObject, QString query, QObject *parent = nullptr);
    ~JsonFluxListModel();

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QString rootPath() const;
    void setRootPath(QString newRootPath);

    QVariantList rolesAndQueries() const;
    void setRolesAndQueries(QVariantList newRolesAndQueries);
signals:
    void rootPathChanged();
    void rolesAndQueriesChanged();
private slots:
    void onValuesChanged();
private:
    void removeAbsentValues(QVariantList newValues);
private:
    JsonFluxModel *m_modelObject = Q_NULLPTR;
    JsonFluxView *m_fluxView = Q_NULLPTR;
    JsonFluxModifier *m_fluxModifier = Q_NULLPTR;
    QVariantList m_values;
    QString m_rootPath = "/";
    QHash<int, QByteArray> m_roles;
    QList<QPair<QString, QString>> m_rolesAndQueries;
};

#endif // JSONFLUXLISTMODEL_H
