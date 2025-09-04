#ifndef CF_CORE_OUTPUTATTRIBUTE
#define CF_CORE_OUTPUTATTRIBUTE

#include "Core/TypedAttribute.hpp"

namespace cf::core {

template <typename DataType>
class OutputAttribute : public TypedAttribute<DataType> {
public:
    OutputAttribute() = default;
    OutputAttribute(std::shared_ptr<Attribute> attribute)
        : TypedAttribute<DataType>(std::move(attribute))
    {
    }

    OutputAttribute& operator=(const DataType& value)
    {
        this->setValue(value);
        return *this;
    }
};

} // namespace cf::core

#endif // CF_CORE_OUTPUTATTRIBUTE