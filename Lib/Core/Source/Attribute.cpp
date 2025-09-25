#include "Attribute.hpp"
#include "Core/Events/AttributeEvent.hpp"

namespace cf::core {

Attribute::Attribute()
    : m_handle(kInvalidAttributeHandle)
    , m_descriptorHandle(kInvalidAttributeHandle)
    , data(nullptr)
{
}

// TODO: Is the handle necessary here? Can we simplify this?
Attribute::Attribute(AttributeDescriptor desc, AttributeHandle attributeHandle)
    : m_handle(attributeHandle)
    , m_descriptorHandle(desc.handle)
    , data(nullptr)
{
    data = TypeRegistry::getTypeDescriptor(desc.typeHandle).create();
}

Attribute::~Attribute()
{
    // TODO: Implement destruction of different data types
    if (data) {
        TypeRegistry::getTypeDescriptor(getAttributeDescriptor().typeHandle).destroy(data);
        data = nullptr;
    }
}

void Attribute::copyDataFrom(const std::shared_ptr<Attribute>& other)
{
    if (!other || !other->data || !data)
        throw std::runtime_error("Null data pointer in copyDataFrom");
    if (getTypeHandle() != other->getTypeHandle())
        throw std::runtime_error("Type mismatch in copyDataFrom");

    // Use reflection to copy the data
    const auto& typeDesc = TypeRegistry::getTypeDescriptor(getTypeHandle());
    typeDesc.copy(data, other->data);

    // TODO: Publish event
    EventBus::publish(AttributeEvent {
        AttributeEvent::AttributeMessage::eAttributeChanged, m_handle });
}

AttributeHandle Attribute::getHandle() const
{
    return m_handle;
}

TypeHandle Attribute::getTypeHandle() const
{
    return getAttributeDescriptor().typeHandle;
}

AttributeDescriptor Attribute::getAttributeDescriptor() const
{
    return TypeRegistry::getAttributeDescriptor(m_descriptorHandle);
}

void Attribute::publishAttributeChanged(AttributeHandle handle)
{
    EventBus::publish(AttributeEvent {
        AttributeEvent::AttributeMessage::eAttributeChanged, handle });
}

} // namespace cf::core