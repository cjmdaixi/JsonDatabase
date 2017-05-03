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
    Q_PROPERTY(bool fileSync READ fileSync WRITE setFileSync NOTIFY fileSyncChanged)
    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY updated)
    Q_PROPERTY(bool indent READ indent WRITE setIndent NOTIFY indentChanged)

    explicit JsonFluxModel(QObject *parent = 0);
    ~JsonFluxModel();

    QString name() const;
    void setName(QString newName);

    QString source() const;
    void setSource(QString newSource);

    QVariantMap data() const;
    void setData(QVariantMap newData);

    bool fileSync() const;
    void setFileSync(bool newFileSync);

    bool modified() const;
    void setModified(bool newModified);

    int indent() const;
    void setIndent(int newIndent);

    json* jsonObject();

    bool modify(QString jsonPath, QVariant newValue);
    bool modify(QString jsonPath, QVariantList newValues);
    bool modify(QString jsonPath, QVariantMap newObject);

    bool save();

    Q_INVOKABLE bool saveAs(QString filePath);
signals:
    void updated();
    void nameChanged();
    void sourceChanged();
    void fileSyncChanged();
    void indentChanged();
public slots:

private:
    QString m_name = "", m_source;
    json m_json;
    bool m_initialized = false;
    bool m_fileSync = false, m_modified = false;
    int m_indent = 4;
};

#endif // JSONFLUXMODEL_H
