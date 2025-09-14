#ifndef CF_CORE_ATTRIBUTE
#define CF_CORE_ATTRIBUTE

#include "Core/TypeDescriptors.hpp"
#include "Core/TypeRegistry.hpp"

namespace cf::core {


using AttributeHandle = uint64_t;
static constexpr AttributeHandle kInvalidAttributeHandle = 0;

/**
 * @brief Attribute is a type erased container for different basic data types
 * used in the application
 *
 */
class Attribute {
public:
    Attribute()
        : handle(kInvalidAttributeHandle)
        , m_descriptorHandle(kInvalidAttributeHandle)
        , data(nullptr)
    {
    }

    // TODO: Is the handle necessary here? Can we simplify this?
    Attribute(AttributeDescriptor desc, AttributeHandle attributeHandle)
        : handle(attributeHandle)
        , m_descriptorHandle(desc.handle)
        , data(nullptr)
    {
        data = TypeRegistry::getTypeDescriptor(desc.typeHandle).create();
    }


    
    ~Attribute()
    {
        //TODO: Implement destruction of different data types
        if (data) {
            TypeRegistry::getTypeDescriptor(getAttributeDescriptor().typeHandle).destroy(data);
            data = nullptr;
        }
    }

    template <typename Type>
    Type getValue() const
    {
        if (!data)
            throw std::runtime_error("Null data pointer in Attribute::getValue");

        if (getTypeHandle() != TypeRegistry::getTypeHandle<Type>())
            throw std::runtime_error("Type mismatch in Attribute::getValue");

        return *static_cast<Type*>(data);
    }

    template <typename Type>
    void setValue(const Type& value)
    {
        if (!data)
            throw std::runtime_error("Null data pointer in Attribute::setValue");

        if (getTypeHandle() != TypeRegistry::getTypeHandle<Type>())
            throw std::runtime_error("Type mismatch in Attribute::setValue");

        *static_cast<Type*>(data) = value;
        //TODO: Publish event
        //EventBus::publish(AttributeEvent {
        //    AttributeEvent::AttributeMessage::eAttributeChanged, handle });
    }


    void copyDataFrom(const std::shared_ptr<Attribute>& other) {
        if (!other || !other->data || !data)
            throw std::runtime_error("Null data pointer in copyDataFrom");
        if (getTypeHandle() != other->getTypeHandle())
            throw std::runtime_error("Type mismatch in copyDataFrom");

        // Use reflection to copy the data
        const auto& typeDesc = TypeRegistry::getTypeDescriptor(getTypeHandle());
        typeDesc.copy(data, other->data);

        //TODO: Publish event
        //EventBus::publish(AttributeEvent {
        //    AttributeEvent::AttributeMessage::eAttributeChanged, handle });
    }


    AttributeHandle getHandle() const { return handle; }
    TypeHandle getTypeHandle() const { return getAttributeDescriptor().typeHandle; }

    const AttributeDescriptor getAttributeDescriptor() const
    {
        return TypeRegistry::getAttributeDescriptor(m_descriptorHandle);
    }

private:
    AttributeHandle handle { kInvalidAttributeHandle };
    AttributeDescriptorHandle m_descriptorHandle { kInvalidAttributeHandle };
    void* data { nullptr };
};

} // namespace cf::core

#endif // CF_CORE_ATTRIBUTE