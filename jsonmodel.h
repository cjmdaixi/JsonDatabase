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
    explicit JsonModel(QObject *parent = 0);

signals:

public slots:
};

#endif // JSONMODEL_H
