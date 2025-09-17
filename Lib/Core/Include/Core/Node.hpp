#ifndef CF_CORE_NODE
#define CF_CORE_NODE

#include "Core/TypeRegistry.hpp"

#include <cstdint>
#include <string>

namespace cf::core {
    
enum class Status {
    eOK,
    eError
};

using NodeHandle = uint64_t;
static constexpr NodeHandle kInvalidNodeHandle = 0;

template<typename NodeType>
concept NodeConcept = requires(NodeType node) {
    { node.compute() } -> std::same_as<Status>;
    { NodeType::initialize() } -> std::same_as<NodeDescriptor>;
    { node.getType() } -> std::same_as<TypeHandle>;
};

class Node {
public:
    virtual ~Node() = default;
    virtual Status compute() = 0;
    virtual TypeHandle getType() const = 0;

    NodeHandle getHandle() const { return m_handle; }

    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    NodeHandle m_handle = kInvalidNodeHandle;
    std::string m_name = "Unnamed Node";
};

template<typename DerivedNodeType>
class NodeBase : public Node {
public:
    using derived_type = DerivedNodeType;

    static NodeDescriptor getStaticDescriptor()
    {
        static NodeDescriptor descriptor = DerivedNodeType::initialize();
        return descriptor;
    }

    TypeHandle getType() const override
    {
        return TypeRegistry::getNodeDescriptorHandle<DerivedNodeType>();
    }

    template<typename MemberPtr>
    static AttributeDescriptor addInputAttributeDescriptor(MemberPtr member, std::string name)
    {
        return TypeRegistry::addAttributeDescriptor<DerivedNodeType, MemberPtr, AttributeRole::eInput>(member, name);
    }

    template<typename MemberPtr>
    static AttributeDescriptor addOutputAttributeDescriptor(MemberPtr member, std::string name)
    {
        return TypeRegistry::addAttributeDescriptor<DerivedNodeType, MemberPtr, AttributeRole::eOutput>(member, name);
    }
};

} // namespace cf::core

#endif // CF_CORE_NODE