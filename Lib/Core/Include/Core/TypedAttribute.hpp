#ifndef CF_CORE_TYPEDATTRIBUTE
#define CF_CORE_TYPEDATTRIBUTE

#include "Core/Attribute.hpp"
#include <cassert>

namespace cf::core {

template <typename DataType>
struct TypedAttribute  {
public:
    TypedAttribute () = default;
    TypedAttribute (std::shared_ptr<Attribute> attribute)
        : ptrToAttribute(std::move(attribute))
    {
        if (!ptrToAttribute) {
            throw std::runtime_error("Null attribute pointer in TypedAttribute ");
        }

        if (ptrToAttribute->getType() != TypeRegistry::getTypeHandle<DataType>()) {
            throw std::runtime_error("Data type mismatch in TypedAttribute ");
        }
    }

    operator DataType() const { return this->getValue(); }

    TypedAttribute & operator=(std::shared_ptr<Attribute> attribute)
    {
        setAttributePtr(attribute);
        return *this;
    }

    AttributeHandle getHandle() const
    {
        return ptrToAttribute->getHandle();
    }

protected:
    void setAttributePtr(std::shared_ptr<Attribute> attribute)
    {
        assert(attribute && "Attribute is null");
        if (!attribute) {
            throw std::runtime_error("Null attribute pointer in TypedAttribute ::setAttributePtr");
        }
        ptrToAttribute = std::move(attribute);
    }

    void setValue(const DataType& value) { ptrToAttribute->setValue<DataType>(value); }
    DataType getValue() const { return ptrToAttribute->getValue<DataType>(); }

private:
    std::shared_ptr<Attribute> ptrToAttribute;
};




} // namespace cf::core

#endif // CF_CORE_TYPEDATTRIBUTE