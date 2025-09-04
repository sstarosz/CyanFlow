#ifndef CF_CORE_TYPEREGISTRY
#define CF_CORE_TYPEREGISTRY

#include "Core/TypeDescriptors.hpp"

#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace cf::core {

namespace {
    //TODO: Use better type id generation
    inline std::atomic<uint64_t> global_type_id_counter { 0 };
}

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
    static NodeDescriptor getNodeDescriptor()
    {
        return getInstance().getNodeDescriptorImpl<Type>();
    }

    template <typename Type>
    NodeDescriptor getNodeDescriptorImpl() const
    {
        TypeHandle handle = getTypeHandle<Type>();
        auto it = nodeMap.find(handle);
        if (it != nodeMap.end()) {
            return it->second;
        }

        throw std::runtime_error("Node type not registered: " + std::string(typeid(Type).name()));
    }

    static NodeDescriptor getNodeDescriptor(TypeHandle handle)
    {
        return getInstance().getNodeDescriptorImpl(handle);
    }

    NodeDescriptor getNodeDescriptorImpl(TypeHandle handle) const
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
    TypeHandle registerNodeTypeImpl()
    {
        TypeHandle handle = getTypeHandle<Type>();
        NodeDescriptor desc = Type::initialize();

        nodeMap[handle] = desc;

        return handle;
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

private:
    std::unordered_map<TypeHandle, TypeDescriptor> typeMap;
    std::unordered_map<TypeHandle, NodeDescriptor> nodeMap;
};

} // namespace cf::core

#endif // CF_CORE_TYPEREGISTRY