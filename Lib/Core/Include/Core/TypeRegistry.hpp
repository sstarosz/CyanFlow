#ifndef CF_CORE_TYPEREGISTRY
#define CF_CORE_TYPEREGISTRY

#include "Core/TypeDescriptors.hpp"

#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace cf::core {

namespace {
    //TODO: Use better type id generation
    inline std::atomic<uint64_t> global_type_id_counter { 1 };
    inline std::atomic<uint64_t> global_attribute_type_id_counter { 1 };
    inline std::atomic<uint64_t> global_node_type_id_counter { 1 };
    inline std::atomic<uint64_t> global_event_type_id_counter { 1 };
}


// Type trait to extract member type from member pointer
// For example, given &Class::member, it extracts the type of member
template<typename T>
struct member_type_from_member_pointer;

template<typename Class, typename Member>
struct member_type_from_member_pointer<Member Class::*> {
    using type = Member;
};

template<typename T>
using member_type_from_member_pointer_t = typename member_type_from_member_pointer<T>::type;

// Type trait to extract the value type from an attribute type
// For example, InputAttribute<int> -> int
template<typename T>
struct attribute_value_type {
    using type = T::ValueType;
};

template<typename T>
using attribute_value_type_t = typename attribute_value_type<T>::type;



class TypeRegistry {
public:
    static TypeRegistry& getInstance()
    {
        static TypeRegistry instance;
        return instance;
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

    template<typename NodeType>
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
    static void registerType(std::string_view name)
    {
        getInstance().registerTypeImpl<Type>(name);
    }

    template <typename Type>
    TypeHandle registerTypeImpl(std::string_view name)
    {
        TypeDescriptor desc = makeTypeDescriptor<Type>();
        desc.name = name;

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
            } 
            else {
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

    //TODO: Make TypeHandle a strong typedef to avoid confusion
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
    template<typename NodeClassType, typename MemberPtrType,  AttributeRole role>
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
        } 
        else if constexpr (role == AttributeRole::eOutput) {
            desc.setter = [member](void* nodePtr, std::shared_ptr<Attribute> attribute) {
                auto* node = static_cast<NodeClassType*>(nodePtr);
                (node->outputs).*member = attribute;
            };
        } 
        else {
            throw std::runtime_error("Unsupported attribute role");
        }
        return desc;
    }


    template<typename NodeType, typename MemberPtrType, AttributeRole role>
    static AttributeDescriptor addAttributeDescriptor(MemberPtrType member, std::string_view name)
    {
        return make_attribute_descriptor_impl<NodeType, MemberPtrType, role>(member, name);
    }

    /*--------------------------*/
    /*--- Event Registration ---*/
    /*--------------------------*/
    template <typename EventType>
    static EventDescriptorHandle getEventDescriptorHandle() {
        return getInstance().getEventDescriptorHandleImpl<EventType>();
    }

    template <typename EventType>
    EventDescriptorHandle getEventDescriptorHandleImpl() const {
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

    static EventDescriptor getEventDescriptor(EventDescriptorHandle handle) {
        return getInstance().getEventDescriptorImpl(handle);
    }

    EventDescriptor getEventDescriptorImpl(EventDescriptorHandle handle) const {
        auto it = eventMap.find(handle);
        if (it != eventMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Event not registered with handle: " + std::to_string(handle));
    }

    template <typename EventType>
    static void registerEventType(std::string_view name, std::string_view category = "") {
        getInstance().registerEventTypeImpl<EventType>(name, category);
    }

    template <typename EventType>
    EventDescriptorHandle registerEventTypeImpl(std::string_view name, std::string_view category = "") {
        EventDescriptor desc;
        desc.name = name;
        desc.category = category;
        
        EventDescriptorHandle handle = getEventDescriptorHandle<EventType>();
        desc.handle = handle;
        
        eventMap[handle] = desc;
        return handle;
    }

    // Get all events in a category
    static std::vector<EventDescriptor> getEventsInCategory(std::string_view category) {
        return getInstance().getEventsInCategoryImpl(category);
    }

    std::vector<EventDescriptor> getEventsInCategoryImpl(std::string_view category) const {
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
};

} // namespace cf::core

#endif // CF_CORE_TYPEREGISTRY