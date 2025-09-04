#ifndef CF_CORE_TYPEDESCRIPTORS
#define CF_CORE_TYPEDESCRIPTORS

#include <string_view>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace cf::core {

class Attribute;

using TypeHandle = std::uint64_t;
constexpr TypeHandle kInvalidTypeHandle = 0;


struct TypeDescriptor
{
    std::string_view name;
    size_t size;


    std::function<void*(void)> create;
    std::function<void(void*, const void*)> copy;
    std::function<void(void*)> destroy;

    std::function<std::string(const void*)> toString;
};


enum class AttributeRole {
    eInput,    // Readable only
    eOutput,   // Writable only
    eInOut     // Readable and writable (pass-through)
};

struct AttributeDescriptor {
    TypeHandle handle;
    std::string name { "" };
    AttributeRole role { AttributeRole::eInput };

    std::function<void(void* nodePtr, std::shared_ptr<Attribute> attribute)> setter;
};

struct NodeDescriptor {
    TypeHandle handle;
    std::string typeName { "" };
    std::vector<AttributeDescriptor> attributes;
};

} // namespace cf::core

#endif // CF_CORE_TYPEDESCRIPTORS