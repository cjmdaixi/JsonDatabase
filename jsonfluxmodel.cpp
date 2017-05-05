#include "jsonfluxmodel.h"
#include "jsonflux.h"
#include <QtDebug>
#include <fstream>
#include <QFileInfo>
#include <QFile>
#include "json.h"
using namespace nlohmann;

JsonFluxModel::JsonFluxModel(QObject *parent)
    : QObject(parent)
{
    connect(this, &JsonFluxModel::updated, flux(), &JsonFlux::onModelUpdated);
}

JsonFluxModel::~JsonFluxModel()
{
    flux()->unregisterModel(m_name);
    //qDebug()<<"Model destroyed";
}

QString JsonFluxModel::name() const
{
    return m_name;
}

void JsonFluxModel::setName(QString newName)
{
    if(m_name == newName) return;

    m_name = newName;

    flux()->registerModel(m_name, this);
    emit nameChanged();
}

QString JsonFluxModel::source() const
{
    return m_source;
}

void JsonFluxModel::setSource(QString newSource)
{
    if(m_source == newSource) return;

    // test if source is a file
    if(QFileInfo::exists(newSource))
    {
        std::ifstream inf(newSource.toStdString());

        if(!inf.is_open())
        {
            qCritical()<<"Cannot open the json file"<<newSource;
            return;
        }

        inf >> m_json;
    }
    else
    {
        m_json = json::parse(newSource.toStdString());
        if(m_json.is_null())
        {
            qCritical()<<"Cannot parse the json string"<<newSource;
            return;
        }
    }

    m_source = newSource;

    emit updated();
    emit sourceChanged();
}

QVariantMap JsonFluxModel::data() const
{
    auto str = m_json.dump();

    auto jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(str));
    return jsonDoc.object().toVariantMap();
}

void JsonFluxModel::setData(QVariantMap newData)
{
    auto newJsonObj = QJsonObject::fromVariantMap(newData);
    if(newJsonObj.isEmpty())
    {
        qCritical()<<"The data is not a valid json!";
        return;
    }
    QJsonDocument jsonDoc;
    jsonDoc.setObject(newJsonObj);
    auto jsonStr = jsonDoc.toJson();
    m_json = json::parse(jsonStr.data());
    emit updated();
}

json* JsonFluxModel::jsonObject()
{
    return &m_json;
}

bool JsonFluxModel::fileSync() const
{
    return m_fileSync;
}

void JsonFluxModel::setFileSync(bool newFileSync)
{
    if(m_fileSync == newFileSync) return;
    m_fileSync = newFileSync;
    emit fileSyncChanged();
}


bool JsonFluxModel::modify(QString jsonPath, QVariant newValue)
{
    json::json_pointer ptr(jsonPath.toStdString());
    try
    {
        m_json[ptr] = JsonFlux::toJsonValue(newValue);
    }
    catch(std::out_of_range&)
    {
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    emit updated();
    if(m_fileSync)
        return save();
    else{
        setModified(true);
        return true;
    }
}

bool JsonFluxModel::modify(QString jsonPath, QVariantList newValues)
{
    json::json_pointer ptr(jsonPath.toStdString());
    try
    {
        m_json[ptr] = JsonFlux::toJsonArray(newValues);
    }
    catch(std::out_of_range&)
    {
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    emit updated();
    if(m_fileSync)
        return save();
    else{
        setModified(true);
        return true;
    }
}

bool JsonFluxModel::modify(QString jsonPath, QVariantMap newObject)
{
    json::json_pointer ptr(jsonPath.toStdString());
    try
    {
        m_json[ptr] = JsonFlux::toJsonObject(newObject);
    }
    catch(std::out_of_range&)
    {
        qCritical()<<"The jsonpath is invalid!";
        return false;
    }
    emit updated();

    if(m_fileSync)
        return save();
    else
    {
        setModified(true);
        return true;
    }
}

bool JsonFluxModel::modified() const
{
    return m_modified;
}

void JsonFluxModel::setModified(bool newModified)
{
    if(m_modified == newModified) return;

    m_modified = newModified;

    emit updated();
}

int JsonFluxModel::indent() const
{
    return m_indent;
}

void JsonFluxModel::setIndent(int newIndent)
{
    if(m_indent == newIndent) return;

    m_indent = newIndent;

    emit indentChanged();
}

bool JsonFluxModel::save()
{
    // if source isn't a file, then just return true cause the content has already been in memory
    if(!QFile::exists(m_source))
        return true;
    QFile outf(m_source);
    if(!outf.open(QIODevice::WriteOnly))
    {
        qCritical()<<"cannot open the file"<<m_source;
        return false;
    }
    outf.write(QByteArray::fromStdString(m_json.dump(m_indent)));
    return true;
}

bool JsonFluxModel::saveAs(QString filePath)
{
    QFile outf(filePath);
    if(!outf.open(QIODevice::WriteOnly))
    {
        qCritical()<<"cannot open the file" << filePath;
        return false;
    }
    outf.write(QByteArray::fromStdString(m_json.dump(m_indent)));
    m_source = filePath;
    emit sourceChanged();
    return true;
}
