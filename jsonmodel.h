#ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

class JsonModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    explicit JsonModel(QString modelName, QObject *parent = 0);
    ~JsonModel();

    QString name() const;
    void setName(QString newName);
signals:
    void updated();
    void nameChanged();
public slots:
private:
    QString m_name;
};

#endif // JSONMODEL_H
