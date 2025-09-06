#ifndef CF_CORE_SCENE
#define CF_CORE_SCENE

#include "Core/Attribute.hpp"
#include "Core/Node.hpp"
#include "Core/TypeRegistry.hpp"
#include "Core/InputAttribute.hpp"
#include "Core/OutputAttribute.hpp"

#include <memory>
#include <unordered_map>

#include<spdlog/spdlog.h>

namespace cf::core {

struct Connection {
    NodeHandle nodeSource;
    AttributeHandle attributeSource;
    NodeHandle nodeTarget;
    AttributeHandle attributeTarget;
};



class Scene {
public:
    template <NodeConcept NodeType>
    std::shared_ptr<NodeType> addNode(std::unique_ptr<NodeType> node)
    {
        const auto& desc = TypeRegistry::getNodeDescriptor<NodeType>();

        NodeHandle handle = generateNodeHandle();
        m_nodes[handle] = std::move(node);
        m_nodes[handle]->setName("Node " + std::to_string(handle));

        for (const auto& attrDesc : desc.attributes) {
            auto attribute = createAttribute(attrDesc);
            if (attrDesc.setter) {
                attrDesc.setter(static_cast<void*>(m_nodes[handle].get()), attribute);
            }

            spdlog::info("Created attribute '{}' with handle {} for node '{}'", 
                attrDesc.name, attribute->getHandle(), m_nodes[handle]->getName());
            nodeAttributes[attribute->getHandle()] = handle;
            attributeDescriptors[attribute->getHandle()] = attrDesc;
        }

        return std::static_pointer_cast<NodeType>(m_nodes[handle]);
    }

    template<typename Type>
    void connect(std::shared_ptr<Node> formNode, OutputAttribute<Type>& fromAttr,
                 std::shared_ptr<Node> toNode, InputAttribute<Type>& toAttr)
    {

        AttributeHandle fromHandleAttr = getAttributeHandle(fromAttr);
        AttributeHandle toHandleAttr = getAttributeHandle(toAttr);

        NodeHandle fromNodeHandle = getNodeHandle(formNode);
        NodeHandle toNodeHandle = getNodeHandle(toNode);

        connections.emplace_back(
            Connection { fromNodeHandle, fromHandleAttr, toNodeHandle, toHandleAttr });
    }


    const std::unordered_map<NodeHandle, std::shared_ptr<Node>>& getNodes() const { return m_nodes; }
    const std::unordered_map<AttributeHandle, std::shared_ptr<Attribute>>& getAttributes() const { return attributes; }
    const std::vector<Connection>& getConnections() const { return connections; }

    std::shared_ptr<Attribute> getAttribute(AttributeHandle handle) const
    {
        auto it = attributes.find(handle);
        if (it != attributes.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<Attribute>> getNodeAttributes(std::shared_ptr<Node> node) const
    {
        std::vector<std::shared_ptr<Attribute>> result;
        NodeHandle nodeHandle = getNodeHandle(node);
        for(const auto& [attrHandle, handleNode] : nodeAttributes) {
            if(handleNode == nodeHandle) {
                auto attrIt = attributes.find(attrHandle);
                if(attrIt != attributes.end()) {
                    result.push_back(attrIt->second);
                }
            }
        }

        spdlog::info("Found {} attributes for node '{}'", result.size(), node->getName());
        return result;
    }

    NodeHandle getNodeHandle(std::shared_ptr<Node> node) const
    {
        for (const auto& [handle, n] : m_nodes) {
            if (n == node) {
                return handle;
            }
        }
        return kInvalidNodeHandle;
    }

    AttributeHandle getAttributeHandle(const TypedAttribute<float>& input) const
    {
        const AttributeHandle attrHandle = input.getHandle();
        if (attributes.find(attrHandle) != attributes.end()) {
            return attrHandle;
        } else {
            return kInvalidAttributeHandle;
        }
    }

    //TODO: should AttributeDescriptor be stored in Scene?
    AttributeDescriptor getAttributeDescriptor(AttributeHandle handle) const
    {
        auto it = attributeDescriptors.find(handle);
        if (it != attributeDescriptors.end()) {
            return it->second;
        }
        throw std::runtime_error("Attribute descriptor not found for handle: " + std::to_string(handle));
    }

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

    NodeHandle m_nextNodeHandle { 1 };
    AttributeHandle nextAttributeHandle { 1 };


    std::unordered_map<NodeHandle, std::shared_ptr<Node>> m_nodes;
    std::unordered_map<AttributeHandle, std::shared_ptr<Attribute>> attributes;
    std::unordered_map<AttributeHandle, NodeHandle> nodeAttributes;
    std::unordered_map<AttributeHandle, AttributeDescriptor> attributeDescriptors;
    std::vector<Connection> connections;
};

} // namespace cf::core

#endif // CF_CORE_SCENE