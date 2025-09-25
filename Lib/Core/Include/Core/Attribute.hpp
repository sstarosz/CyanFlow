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
    Attribute();
    // TODO: Is the handle necessary here? Can we simplify this?
    Attribute(AttributeDescriptor desc, AttributeHandle attributeHandle);

    ~Attribute();

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
        spdlog::debug("Setting attribute {} to value: {}", getHandle(), value);
        
        if (!data)
            throw std::runtime_error("Null data pointer in Attribute::setValue");

        if (getTypeHandle() != TypeRegistry::getTypeHandle<Type>())
            throw std::runtime_error("Type mismatch in Attribute::setValue");

        *static_cast<Type*>(data) = value;
        publishAttributeChanged(m_handle);
    }

    void copyDataFrom(const std::shared_ptr<Attribute>& other);

    AttributeHandle getHandle() const;
    TypeHandle getTypeHandle() const;
    AttributeDescriptor getAttributeDescriptor() const;

private:
    AttributeHandle m_handle { kInvalidAttributeHandle };
    AttributeDescriptorHandle m_descriptorHandle { kInvalidAttributeHandle };
    void* data { nullptr };

    void publishAttributeChanged(AttributeHandle handle);
};

} // namespace cf::core

#endif // CF_CORE_ATTRIBUTE