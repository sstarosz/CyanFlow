#ifndef CF_CORE_TYPEREGISTRY_HPP
#define CF_CORE_TYPEREGISTRY_HPP

#include "Core/BuildConfig.hpp"
#include "Core/DataTypes.hpp"
#include "Core/TypeDescriptors.hpp"

#include <cstdint>
#include <source_location>
#include <stdexcept>
#include <unordered_map>

namespace cf::core {

// Type trait to extract member type from member pointer
// For example, given &Class::member, it extracts the type of member
template <typename T>
struct member_type_from_member_pointer;

template <typename Class, typename Member>
struct member_type_from_member_pointer<Member Class::*> {
    using type = Member;
};

template <typename T>
using member_type_from_member_pointer_t = typename member_type_from_member_pointer<T>::type;

// Type trait to extract the value type from an attribute type
// For example, InputAttribute<int> -> int
template <typename T>
struct attribute_value_type {
    using type = T::ValueType;
};

template <typename T>
using attribute_value_type_t = typename attribute_value_type<T>::type;

template <typename T>
constexpr const char* getRawTypeName()
{
    return std::source_location::current().function_name();
}

// Workaround to get name of core data types which are type aliases
template <typename Type>
constexpr std::string_view getCoreDataType()
{
    if constexpr (std::same_as<Type, Bool>) {
        return "cf::core::Bool";
    } else if constexpr (std::same_as<Type, Int32>) {
        return "cf::core::Int32";
    } else if constexpr (std::same_as<Type, UInt32>) {
        return "cf::core::UInt32";
    } else if constexpr (std::same_as<Type, Int64>) {
        return "cf::core::Int64";
    } else if constexpr (std::same_as<Type, UInt64>) {
        return "cf::core::UInt64";
    } else if constexpr (std::same_as<Type, Float>) {
        return "cf::core::Float";
    } else if constexpr (std::same_as<Type, Double>) {
        return "cf::core::Double";
    } else if constexpr (std::same_as<Type, String>) {
        return "cf::core::String";
    } else {
        return "UnknownCoreType";
    }
}

template <typename Type>
constexpr std::string_view getCustomTypeName()
{

    // If basic type, return directly
    constexpr const char* rawName = getRawTypeName<Type>();
    std::string_view rawView(rawName);

    rawView.remove_prefix(rawView.find_first_of('<') + 1);
    rawView.remove_suffix(rawView.size() - rawView.find_last_of('>'));

    // If MSVC, remove "class " or "struct "
    if constexpr (BuildConfig::isMSVCCompiler()) {
        if (rawView.starts_with("class ")) {
            rawView.remove_prefix(6);
        } else if (rawView.starts_with("struct ")) {
            rawView.remove_prefix(7);
        } else if (rawView.starts_with("enum ")) {
            rawView.remove_prefix(5);
        } else if (rawView.starts_with("union ")) {
            rawView.remove_prefix(6);
        }
    }

    return rawView;
}

template <typename Type>
constexpr std::string_view getTypeName()
{
    // Check for core data types first
    if constexpr (IsCoreFundamentalType<Type>) {
        return getCoreDataType<Type>();
    } else {
        return getCustomTypeName<Type>();
    }
}

class TypeRegistry {
public:
    static TypeRegistry& getInstance()
    {
        static TypeRegistry instance;
        return instance;
    }

    static void clearInstance()
    {
        getInstance().typeMap.clear();
        getInstance().attributeMap.clear();
        getInstance().nodeMap.clear();
        getInstance().eventMap.clear();

        global_type_id_counter = 1;
        global_attribute_type_id_counter = 1;
        global_node_type_id_counter = 1;
        global_event_type_id_counter = 1;
    }

    template <typename Type>
    static TypeHandle getTypeHandle()
    {
        return getInstance().getTypeHandleImpl<Type>();
    }

    template <typename Type>
    TypeHandle getTypeHandleImpl() const
    {
        static const uint64_t id = global_type_id_counter++;
        return TypeHandle(id);
    }

    template <typename NodeType>
    static NodeDescriptorHandle getNodeDescriptorHandle()
    {
        return getInstance().getNodeDescriptorHandleImpl<NodeType>();
    }

    template <typename NodeType>
    NodeDescriptorHandle getNodeDescriptorHandleImpl() const
    {
        static const uint64_t id = global_node_type_id_counter++;
        return NodeDescriptorHandle(id);
    }

    template <typename Type>
    static TypeDescriptor getTypeDescriptor()
    {
        return getInstance().getTypeDescriptorImpl<Type>();
    }

    template <typename Type>
    TypeDescriptor getTypeDescriptorImpl() const
    {
        TypeHandle handle = getTypeHandle<Type>();
        auto it = typeMap.find(handle);
        if (it != typeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Type not registered: " + std::string(typeid(Type).name()));
    }

    static TypeDescriptor getTypeDescriptor(TypeHandle handle)
    {
        return getInstance().getTypeDescriptorImpl(handle);
    }

    TypeDescriptor getTypeDescriptorImpl(TypeHandle handle) const
    {
        auto it = typeMap.find(handle);
        if (it != typeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Type not registered with handle: " + std::to_string(handle));
    }

    template <typename Type>
    static void registerType()
    {
        getInstance().registerTypeImpl<Type>();
    }

    template <typename Type>
    TypeHandle registerTypeImpl()
    {
        TypeDescriptor desc = makeTypeDescriptor<Type>();
        desc.name = getTypeName<Type>();

        TypeHandle handle = getTypeHandle<Type>();
        typeMap[handle] = desc;

        return handle;
    }

    template <typename Type>
    TypeDescriptor makeTypeDescriptor()
    {
        TypeDescriptor desc;
        // desc.name = TypeInfo<Type>::name();
        desc.size = sizeof(Type);

        desc.create = []() -> void* {
            return new Type();
        };

        desc.copy = [](void* dst, const void* src) {
            new (dst) Type(*static_cast<const Type*>(src));
        };

        desc.destroy = [](void* ptr) {
            if constexpr (std::is_trivially_destructible_v<Type>) {
                delete static_cast<Type*>(ptr);
            } else {
                static_cast<Type*>(ptr)->~Type();
            }
        };

        desc.toString = [](const void* ptr) -> std::string {
            const Type& value = *static_cast<const Type*>(ptr);
            if constexpr (std::is_arithmetic_v<Type>) {
                return std::to_string(value);
            } else if constexpr (std::is_same_v<Type, std::string>) {
                return value;
            } else {
                return "unsupported";
            }
        };

        return desc;
    }

    /*-------------------------*/
    /*--- Node Registration ---*/
    /*-------------------------*/
    template <typename Type>
    static NodeDescriptor getNodeDescriptor()
    {
        return getInstance().getNodeDescriptorImpl<Type>();
    }

    template <typename Type>
    NodeDescriptor getNodeDescriptorImpl() const
    {
        NodeDescriptorHandle handle = getNodeDescriptorHandle<Type>();
        auto it = nodeMap.find(handle);
        if (it != nodeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Node type not registered: " + std::string(typeid(Type).name()));
    }

    // TODO: Make TypeHandle a strong typedef to avoid confusion
    static NodeDescriptor getNodeDescriptor(NodeDescriptorHandle handle)
    {
        return getInstance().getNodeDescriptorImpl(handle);
    }

    NodeDescriptor getNodeDescriptorImpl(NodeDescriptorHandle handle) const
    {
        auto it = nodeMap.find(handle);
        if (it != nodeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Node type not registered with handle: " + std::to_string(handle));
    }

    template <typename Type>
    static void registerNodeType()
    {
        getInstance().registerNodeTypeImpl<Type>();
    }

    template <typename Type>
    NodeDescriptorHandle registerNodeTypeImpl()
    {
        NodeDescriptorHandle handle = getNodeDescriptorHandle<Type>();

        NodeDescriptor desc = Type::initialize();
        desc.handle = handle;

        // Register all attributes from this node type
        for (auto& attrDesc : desc.attributes) {
            attrDesc.handle = registerAttributeDescriptorImpl(attrDesc);
        }

        nodeMap[handle] = desc;

        return handle;
    }

    /*-------------------------------*/
    /*--- Attribute Registration ----*/
    /*-------------------------------*/

    static AttributeDescriptor getAttributeDescriptor(AttributeDescriptorHandle handle)
    {
        return getInstance().getAttributeDescriptorImpl(handle);
    }

    AttributeDescriptor getAttributeDescriptorImpl(AttributeDescriptorHandle handle) const
    {
        auto it = attributeMap.find(handle);
        if (it != attributeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Attribute not registered with handle: " + std::to_string(handle));
    }

    static void registerAttributeDescriptor(AttributeDescriptor& desc)
    {
        getInstance().registerAttributeDescriptorImpl(desc);
    }

    AttributeDescriptorHandle getAttributeDescriptorHandleImpl() const
    {
        return AttributeDescriptorHandle(global_attribute_type_id_counter++);
    }

    AttributeDescriptorHandle registerAttributeDescriptorImpl(AttributeDescriptor& desc)
    {
        AttributeDescriptorHandle handle = getAttributeDescriptorHandleImpl();
        desc.handle = handle;
        attributeMap[handle] = desc;
        return handle;
    }

    // Specialization for nested struct members
    template <typename NodeClassType, typename MemberPtrType, AttributeRole role>
    static AttributeDescriptor make_attribute_descriptor_impl(MemberPtrType member, std::string_view name)
    {
        // Extract the value type from the member type
        using AttributeType = member_type_from_member_pointer_t<MemberPtrType>;
        using ValueType = attribute_value_type_t<AttributeType>;

        AttributeDescriptor desc;
        desc.typeHandle = TypeRegistry::getTypeHandle<ValueType>();
        desc.name = name;
        desc.role = role;

        if constexpr (role == AttributeRole::eInput) {
            desc.setter = [member](void* nodePtr, std::shared_ptr<Attribute> attribute) {
                auto* node = static_cast<NodeClassType*>(nodePtr);
                (node->inputs).*member = attribute;
            };
        } else if constexpr (role == AttributeRole::eOutput) {
            desc.setter = [member](void* nodePtr, std::shared_ptr<Attribute> attribute) {
                auto* node = static_cast<NodeClassType*>(nodePtr);
                (node->outputs).*member = attribute;
            };
        } else {
            throw std::runtime_error("Unsupported attribute role");
        }
        return desc;
    }

    template <typename NodeType, typename MemberPtrType, AttributeRole role>
    static AttributeDescriptor addAttributeDescriptor(MemberPtrType member, std::string_view name)
    {
        return make_attribute_descriptor_impl<NodeType, MemberPtrType, role>(member, name);
    }

    /*--------------------------*/
    /*--- Event Registration ---*/
    /*--------------------------*/
    template <typename EventType>
    static EventDescriptorHandle getEventDescriptorHandle()
    {
        return getInstance().getEventDescriptorHandleImpl<EventType>();
    }

    template <typename EventType>
    EventDescriptorHandle getEventDescriptorHandleImpl() const
    {
        static const uint64_t id = global_event_type_id_counter++;
        return EventDescriptorHandle(id);
    }

    template <typename Type>
    static EventDescriptor getEventDescriptor()
    {
        return getInstance().getEventDescriptorImpl<Type>();
    }

    template <typename Type>
    EventDescriptor getEventDescriptorImpl() const
    {
        EventDescriptorHandle handle = getEventDescriptorHandle<Type>();
        auto it = eventMap.find(handle);
        if (it != eventMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Event type not registered: " + std::string(typeid(Type).name()));
    }

    static EventDescriptor getEventDescriptor(EventDescriptorHandle handle)
    {
        return getInstance().getEventDescriptorImpl(handle);
    }

    EventDescriptor getEventDescriptorImpl(EventDescriptorHandle handle) const
    {
        auto it = eventMap.find(handle);
        if (it != eventMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Event not registered with handle: " + std::to_string(handle));
    }

    template <typename EventType>
    static void registerEventType(std::string_view name, std::string_view category = "")
    {
        getInstance().registerEventTypeImpl<EventType>(name, category);
    }

    template <typename EventType>
    EventDescriptorHandle registerEventTypeImpl(std::string_view name, std::string_view category = "")
    {
        EventDescriptor desc;
        desc.name = name;
        desc.category = category;

        EventDescriptorHandle handle = getEventDescriptorHandle<EventType>();
        desc.handle = handle;

        eventMap[handle] = desc;
        return handle;
    }

    // Get all events in a category
    static std::vector<EventDescriptor> getEventsInCategory(std::string_view category)
    {
        return getInstance().getEventsInCategoryImpl(category);
    }

    std::vector<EventDescriptor> getEventsInCategoryImpl(std::string_view category) const
    {
        std::vector<EventDescriptor> result;
        for (const auto& [handle, descriptor] : eventMap) {
            if (descriptor.category == category) {
                result.push_back(descriptor);
            }
        }
        return result;
    }

private:
    std::unordered_map<TypeHandle, TypeDescriptor> typeMap;
    std::unordered_map<AttributeDescriptorHandle, AttributeDescriptor> attributeMap;
    std::unordered_map<NodeDescriptorHandle, NodeDescriptor> nodeMap;
    std::unordered_map<EventDescriptorHandle, EventDescriptor> eventMap;

    static inline std::atomic<uint64_t> global_type_id_counter { 1 };
    static inline std::atomic<uint64_t> global_attribute_type_id_counter { 1 };
    static inline std::atomic<uint64_t> global_node_type_id_counter { 1 };
    static inline std::atomic<uint64_t> global_event_type_id_counter { 1 };
};

} // namespace cf::core

#endif // CF_CORE_TYPEREGISTRY_HPP