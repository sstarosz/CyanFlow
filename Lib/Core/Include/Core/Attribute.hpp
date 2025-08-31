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
        : m_typeHandle(kInvalidTypeHandle)
        , handle(kInvalidAttributeHandle)
        , data(nullptr)
    {
    }

    // TODO: Is the handle necessary here? Can we simplify this?
    Attribute(AttributeDescriptor desc, AttributeHandle attributeHandle)
        : m_typeHandle(desc.handle)
        , handle(attributeHandle)
        , data(nullptr)
    {
        data = TypeRegistry::getTypeDescriptor(desc.handle).create();
    }


    
    ~Attribute()
    {
        //TODO: Implement destruction of different data types
        if (data) {
            TypeRegistry::getTypeDescriptor(m_typeHandle).destroy(data);
            data = nullptr;
        }
    }

    template <typename Type>
    Type getValue() const
    {
        if (!data)
            throw std::runtime_error("Null data pointer in Attribute::getValue");

        if (m_typeHandle != TypeRegistry::getTypeHandle<Type>())
            throw std::runtime_error("Type mismatch in Attribute::getValue");

        return *static_cast<Type*>(data);
    }

    template <typename Type>
    void setValue(const Type& value)
    {
        if (!data)
            throw std::runtime_error("Null data pointer in Attribute::setValue");

        if (m_typeHandle != TypeRegistry::getTypeHandle<Type>())
            throw std::runtime_error("Type mismatch in Attribute::setValue");

        *static_cast<Type*>(data) = value;
        //TODO: Publish event
        //EventBus::publish(AttributeEvent {
        //    AttributeEvent::AttributeMessage::eAttributeChanged, handle });
    }


    void copyDataFrom(const std::shared_ptr<Attribute>& other) {
        if (!other || !other->data || !data)
            throw std::runtime_error("Null data pointer in copyDataFrom");
        if (m_typeHandle != other->m_typeHandle)
            throw std::runtime_error("Type mismatch in copyDataFrom");

        // Use reflection to copy the data
        const auto& typeDesc = TypeRegistry::getTypeDescriptor(m_typeHandle);
        typeDesc.copy(data, other->data);

        //TODO: Publish event
        //EventBus::publish(AttributeEvent {
        //    AttributeEvent::AttributeMessage::eAttributeChanged, handle });
    }


    AttributeHandle getHandle() const { return handle; }

    TypeHandle getType() const { return m_typeHandle; }

private:
    TypeHandle m_typeHandle;
    AttributeHandle handle { kInvalidAttributeHandle };
    void* data { nullptr };
};

} // namespace cf::core

#endif // CF_CORE_ATTRIBUTE