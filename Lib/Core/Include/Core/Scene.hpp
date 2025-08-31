#ifndef CF_CORE_SCENE
#define CF_CORE_SCENE

#include "Core/Attribute.hpp"
#include "Core/Node.hpp"
#include "Core/TypeRegistry.hpp"

#include <memory>
#include <unordered_map>

namespace cf::core {
class Scene {
public:
    template <NodeConcept NodeType>
    void addNode(std::unique_ptr<NodeType> node)
    {
        const auto& desc = TypeRegistry::getNodeDescriptor<NodeType>();

        for (const auto& attrDesc : desc.attributes) {
            auto attribute = createAttribute(attrDesc);
            if (attrDesc.setter) {
                attrDesc.setter(static_cast<void*>(node.get()), attribute);
            }
        }

        NodeHandle handle = generateNodeHandle();
        m_nodes[handle] = std::move(node);
        m_nodes[handle]->setName("Node " + std::to_string(handle));
    }

    const std::unordered_map<NodeHandle, std::shared_ptr<Node>>& getNodes() const { return m_nodes; }

private:
    NodeHandle generateNodeHandle() { return m_nextNodeHandle++; }

    AttributeHandle generateAttributeHandle()
    {
        return nextAttributeHandle++;
    }

    std::shared_ptr<Attribute> createAttribute(const AttributeDescriptor& desc)
    {
        AttributeHandle handle = generateAttributeHandle();
        auto attribute = std::make_shared<Attribute>(desc, handle);
        attributes[handle] = attribute;
        return attribute;
    }

    NodeHandle m_nextNodeHandle = 1;
    uint64_t nextAttributeHandle { 1 };


    std::unordered_map<NodeHandle, std::shared_ptr<Node>> m_nodes;
    std::unordered_map<AttributeHandle, std::shared_ptr<Attribute>> attributes;
};

} // namespace cf::core

#endif // CF_CORE_SCENE