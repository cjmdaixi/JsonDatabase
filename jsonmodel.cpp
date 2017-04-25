#include "jsonmodel.h"
#include "jsondatabase.h"

JsonModel::JsonModel(QString modelName, QObject *parent)
    : QObject(parent), m_name(modelName)
{
    database()->registerModel(m_name, this);
}

JsonModel::~JsonModel()
{
    database()->unregisterModel(m_name);
}

QString JsonModel::name() const
{
    return m_name;
}

void JsonModel::setName(QString newName)
{
    if(m_name == newName) return;

    m_name = newName;

    emit nameChanged();
}
