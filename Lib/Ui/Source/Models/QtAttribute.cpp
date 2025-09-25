#include "Models/QtAttribute.hpp"
#include "Core/EventBus.hpp"
#include "Core/Events/AttributeEvent.hpp"

namespace cf::ui {

QtAttribute::QtAttribute(std::shared_ptr<core::Attribute> attribute, QObject* parent)
    : QObject(parent)
    , m_attribute(attribute)
{
    m_eventSubscriptionId = core::EventBus::subscribe<core::AttributeEvent>([this](const core::AttributeEvent& event) {
        if (event.m_message == core::AttributeEvent::AttributeMessage::eAttributeChanged) {
            if (event.attributeHandle == m_attribute->getHandle()) {
                emit valueChanged();
            }
        }
    });
}

QtAttribute::~QtAttribute()
{
    if (m_eventSubscriptionId != core::EventBus::kInvalidSubscriptionId) {
        core::EventBus::unsubscribe(m_eventSubscriptionId);
        m_eventSubscriptionId = core::EventBus::kInvalidSubscriptionId;
    }
}

bool QtAttribute::isInput() const
{
    return m_attribute->getAttributeDescriptor().role == core::AttributeRole::eInput;
}

bool QtAttribute::isOutput() const
{
    return m_attribute->getAttributeDescriptor().role == core::AttributeRole::eOutput;
}

bool QtAttribute::isInOut() const
{
    return m_attribute->getAttributeDescriptor().role == core::AttributeRole::eInOut;
}

QString QtAttribute::getName() const
{
    return QString::fromStdString(m_attribute->getAttributeDescriptor().name);
}

std::shared_ptr<core::Attribute> QtAttribute::getAttribute() const
{
    return m_attribute;
}

core::AttributeDescriptor QtAttribute::getAttributeDescriptor() const
{
    return m_attribute->getAttributeDescriptor();
}

QVariant QtAttribute::getValue() const
{
    // This is a simplified example. You might want to handle more types.


    auto typeHandle = m_attribute->getTypeHandle();
    if (typeHandle == core::TypeRegistry::getTypeHandle<int>()) 
    {
        spdlog::debug("QtAttribute::getValue() - Attribute '{}' is of type int with value {}",
            getName().toStdString(),
            m_attribute->getValue<int>());

        return QVariant(m_attribute->getValue<int>());
        
    } else if (typeHandle == core::TypeRegistry::getTypeHandle<float>()) 
    {
        spdlog::debug("QtAttribute::getValue() - Attribute '{}' is of type float with value {}",
            getName().toStdString(),
            m_attribute->getValue<float>());
        return QVariant(m_attribute->getValue<float>());

    } else if (typeHandle == core::TypeRegistry::getTypeHandle<double>()) 
    {
        spdlog::debug("QtAttribute::getValue() - Attribute '{}' is of type double with value {}",
            getName().toStdString(),
            m_attribute->getValue<double>());
        return QVariant(m_attribute->getValue<double>());

    } else if (typeHandle == core::TypeRegistry::getTypeHandle<std::string>()) 
    {
        spdlog::debug("QtAttribute::getValue() - Attribute '{}' is of type std::string with value {}",
            getName().toStdString(),
            m_attribute->getValue<std::string>());

        return QString::fromStdString(m_attribute->getValue<std::string>());
    }
    // Add more types as needed
    return QVariant();
}

} // namespace cf::ui
