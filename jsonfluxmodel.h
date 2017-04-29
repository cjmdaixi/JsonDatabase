#ifndef JSONFLUXMODEL_H
#define JSONFLUXMODEL_H

#include <QObject>
#include <QVariantMap>
#include "json.h"
using namespace nlohmann;

class JsonFluxModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVariantMap data READ data WRITE setData NOTIFY updated)

    explicit JsonFluxModel(QObject *parent = 0);
    ~JsonFluxModel();

    QString name() const;
    void setName(QString newName);

    QString source() const;
    void setSource(QString newSource);

    QVariantMap data() const;
    void setData(QVariantMap newData);

    json* jsonObject();

    //QVariant
signals:
    void updated();
    void nameChanged();
    void sourceChanged();
public slots:
private:
    QString m_name = "", m_source;
    json m_json;
    bool m_initialized = false;
};

#endif // JSONFLUXMODEL_H
