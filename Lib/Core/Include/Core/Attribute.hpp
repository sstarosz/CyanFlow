#ifndef CF_CORE_ATTRIBUTE_HPP
#define CF_CORE_ATTRIBUTE_HPP

#include "Core/TypeDescriptors.hpp"
#include "Core/TypeRegistry.hpp"

namespace cf::core {

template <typename Type>
concept IsAttribute = std::same_as<std::remove_cvref_t<Type>, std::shared_ptr<Attribute>> || std::same_as<std::remove_cvref_t<Type>, Attribute>;

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
        if constexpr (IsAttribute<Type>) {
            copyDataFrom(value);
        } else {
            copyDataFromPrimitive(value);
        }
    }

    AttributeHandle getHandle() const;
    AttributeDescriptor getAttributeDescriptor() const;

private:
    AttributeHandle m_handle { kInvalidAttributeHandle };
    AttributeDescriptorHandle m_descriptorHandle { kInvalidAttributeHandle };
    void* data { nullptr };

    void publishAttributeChanged(AttributeHandle handle);
    TypeHandle getTypeHandle() const;

    void copyDataFrom(const std::shared_ptr<Attribute>& other);
    void copyDataFrom(const Attribute& other);

    template <typename Type>
    void copyDataFromPrimitive(const Type& value)
    {
        if (!data) {
            throw std::runtime_error("Null data pointer in copyDataFromPrimitive");
        }
        if (getTypeHandle() != TypeRegistry::getTypeHandle<Type>()) {
            throw std::runtime_error("Type mismatch in copyDataFromPrimitive");
        }

        *static_cast<Type*>(data) = value;
        publishAttributeChanged(m_handle);
    }
};

} // namespace cf::core

#endif // CF_CORE_ATTRIBUTE_HPP