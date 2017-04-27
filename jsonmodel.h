#ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariantMap>

class JsonModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVariantMap data READ data WRITE setData NOTIFY updated)

    explicit JsonModel(QObject *parent = 0);
    ~JsonModel();

    QString name() const;
    void setName(QString newName);

    QString source() const;
    void setSource(QString newSource);

    QVariantMap data() const;
    void setData(QVariantMap newData);

    QJsonObject jsonObject() const;
signals:
    void updated();
    void nameChanged();
    void sourceChanged();
public slots:
private:
    QString m_name, m_source;
    QJsonObject m_jsonObject;
};

#endif // JSONMODEL_H
