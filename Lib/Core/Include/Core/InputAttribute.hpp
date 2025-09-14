#ifndef CF_CORE_INPUTATTRIBUTE
#define CF_CORE_INPUTATTRIBUTE

#include "Core/TypedAttribute.hpp"

namespace cf::core {

/**
 * @brief Type defined wrapper over an attribute that can be used as an input to
 * a node. Input attributes are used to read data from the graph context. Inputs
 * are read-only and cannot be modified directly.
 *
 * @tparam DataType
 */
template <typename DataType>
struct InputAttribute : public TypedAttribute<DataType> {


    InputAttribute() = default;
    InputAttribute(std::shared_ptr<Attribute> attribute)
        : TypedAttribute<DataType>(std::move(attribute))
    {
    }

    DataType operator+(const InputAttribute& other) const
    {
        return this->getValue() + other.getValue();
    }

    DataType operator-(const InputAttribute& other) const
    {
        return this->getValue() - other.getValue();
    }

    template <typename OtherType>
    DataType operator*(const InputAttribute<OtherType>& other) const
    {
        return this->getValue() * other.getValue();
    }

    DataType operator/(const InputAttribute& other) const
    {
        if (other.getValue() == 0) {
            throw std::runtime_error("Division by zero in InputAttribute");
        }
        return this->getValue() / other.getValue();
    }
};

} // namespace cf::core

#endif // CF_CORE_INPUTATTRIBUTE