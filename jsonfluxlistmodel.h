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
    enum ModelType
    {
        MTArray,
        MTObject
    };

    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QStringList roles READ roles WRITE setRoles NOTIFY rolesChanged)
    Q_PROPERTY(JsonFluxModel* model READ model WRITE setModel NOTIFY modelChanged)

    explicit JsonFluxListModel(JsonFluxModel * modelObject, QString query, QStringList roles, QObject *parent = nullptr);
    ~JsonFluxListModel();

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    JsonFluxModel *model() const;
    void setModel(JsonFluxModel *newModel);

    QString query() const;
    void setQuery(QString newQuery);

    QStringList roles() const;
    void setRoles(QStringList newRoles);
signals:
    void queryChanged();
    void rolesChanged();
    void modelChanged();
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
    ModelType m_modeltype = MTArray;
};

#endif // JSONFLUXLISTMODEL_H
