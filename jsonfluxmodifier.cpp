#include "jsonfluxmodifier.h"

JsonFluxModifier::JsonFluxModifier(QObject *parent)
    : QObject(parent)
{

}

JsonFluxModifier::~JsonFluxModifier()
{

}

JsonFluxModel * JsonFluxModifier::model() const
{
    return m_modelObject;
}

void JsonFluxModifier::setModel(JsonFluxModel *newModel)
{
    if(m_modelObject == newModel) return;

    m_modelObject = newModel;
    emit modelChanged();
}

bool JsonFluxModifier::modify(QString jsonPath, QVariant newValue)
{
    return true;
}

