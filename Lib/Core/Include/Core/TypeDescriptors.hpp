#ifndef CF_CORE_TYPEDESCRIPTORS_HPP
#define CF_CORE_TYPEDESCRIPTORS_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace cf::core {

class Attribute;

// Type
using TypeHandle = std::uint64_t;
constexpr TypeHandle kInvalidTypeHandle = 0;

// Attribute
using AttributeDescriptorHandle = std::uint64_t;
static constexpr AttributeDescriptorHandle kInvalidAttributeDescriptorHandle = 0;

// Node
using NodeDescriptorHandle = std::uint64_t;
static constexpr NodeDescriptorHandle kInvalidNodeDescriptorHandle = 0;

// Event
using EventDescriptorHandle = std::uint64_t;
static constexpr EventDescriptorHandle kInvalidEventDescriptorHandle = 0;

struct TypeDescriptor {
    std::string_view name;
    size_t size;

    std::function<void*(void)> create;
    std::function<void(void*, const void*)> copy;
    std::function<void(void*)> destroy;

    std::function<std::string(const void*)> toString;
};

enum class AttributeRole {
    eInput, // Readable only
    eOutput, // Writable only
    eInOut // Readable and writable (pass-through)
};

using AttributeSetterFunc = std::function<void(void*, std::shared_ptr<Attribute>)>;

struct AttributeDescriptor {
    AttributeDescriptorHandle handle;
    TypeHandle typeHandle { kInvalidTypeHandle };
    std::string name { "" };
    AttributeRole role { AttributeRole::eInput };

    AttributeSetterFunc setter { nullptr };
};

struct NodeDescriptor {
    NodeDescriptorHandle handle;
    std::string typeName { "" };
    std::vector<AttributeDescriptor> attributes;
};

struct EventDescriptor {
    EventDescriptorHandle handle;
    std::string name;
    std::string category;
};

} // namespace cf::core

#endif // CF_CORE_TYPEDESCRIPTORS_HPP