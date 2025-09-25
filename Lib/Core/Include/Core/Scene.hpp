#ifndef CF_CORE_SCENE
#define CF_CORE_SCENE

#include "Core/Attribute.hpp"
#include "Core/Node.hpp"
#include "Core/EventBus.hpp"
#include "Core/TypeRegistry.hpp"
#include "Core/InputAttribute.hpp"
#include "Core/OutputAttribute.hpp"
#include "Core/Events/AttributeEvent.hpp"

#include <memory>
#include <unordered_map>
#include <algorithm>

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
    Scene()
    {
        EventBus::SubscriptionId subId = EventBus::subscribe<AttributeEvent>([&](const AttributeEvent& event) {
            if (event.m_message == AttributeEvent::AttributeMessage::eAttributeChanged) {
                evaluate();
            }
        });

        m_subscriptions.push_back(subId);
    }

    ~Scene()
    {
        for (const auto& subId : m_subscriptions) {
            EventBus::unsubscribe(subId);
        }
    }


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

    void addConnection(AttributeHandle fromAttr, AttributeHandle toAttr)
    {
        auto fromNodeIt = nodeAttributes.find(fromAttr);
        auto toNodeIt = nodeAttributes.find(toAttr);

        if (fromNodeIt == nodeAttributes.end() || toNodeIt == nodeAttributes.end()) {
            spdlog::error("Scene::addConnection - Invalid attribute handle(s) provided");
            return;
        }

        connections.emplace_back(
            Connection { fromNodeIt->second, fromAttr, toNodeIt->second, toAttr });
    }

    void removeConnection(AttributeHandle fromAttr, AttributeHandle toAttr)
    {
        std::erase_if(connections, [fromAttr, toAttr](const Connection& conn) {
            return conn.attributeSource == fromAttr && conn.attributeTarget == toAttr;
        });
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

    std::vector<std::shared_ptr<Node>> topologicalSort()
    {
        std::unordered_map<NodeHandle, size_t> inDegree;
        for (const auto& [handle, node] : m_nodes) {
            inDegree[handle] = 0;
        }

        for (const auto& conn : connections) {
            inDegree[conn.nodeTarget]++;
        }

        std::vector<std::shared_ptr<Node>> sortedNodes;
        std::vector<NodeHandle> zeroInDegreeNodes;

        // Find nodes with zero in-degree
        for (const auto& [handle, node] : m_nodes) {
            if (inDegree[handle] == 0) {
                zeroInDegreeNodes.push_back(handle);
            }
        }

        while (!zeroInDegreeNodes.empty()) {
            NodeHandle currentNodeHandle = zeroInDegreeNodes.back();
            zeroInDegreeNodes.pop_back();

            sortedNodes.push_back(m_nodes[currentNodeHandle]);

            for (const auto& conn : connections) {
                // Iterate through connections and reduce in-degree
                // of all nodes that are connected to the current node
                if (conn.nodeSource == currentNodeHandle) {
                    inDegree[conn.nodeTarget]--;
                    if (inDegree[conn.nodeTarget] == 0) {
                        zeroInDegreeNodes.push_back(conn.nodeTarget);
                    }
                }
            }
        }

        return sortedNodes;
    }

    void evaluate()
    {
        if (m_isEvaluating)
            return;
        m_isEvaluating = true;

        std::vector<std::shared_ptr<Node>> sortedNodes = topologicalSort();
        for (const auto& node : sortedNodes) {
            if (node) {
                propagateConnectionsToNode(node);

                Status status = node->compute();
                if (status != Status::eOK) {
                    spdlog::error("Node '{}' computation failed with status: {}", node->getName(), static_cast<int>(status));
                }
            }
        }

        m_isEvaluating = false;
    }

    void propagateConnectionsToNode(std::shared_ptr<Node> node)
    {
        NodeHandle targetHandle = getNodeHandle(node);
        for (const auto& conn : connections) {
            if (conn.nodeTarget == targetHandle) {
                auto fromAttr = attributes[conn.attributeSource];
                auto toAttr = attributes[conn.attributeTarget];
                if (fromAttr && toAttr) {           
                    toAttr->copyDataFrom(fromAttr);
                }
            }
        }
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

    bool m_isEvaluating { false };
    uint64_t m_m_evaluationCount { 0 };


    NodeHandle m_nextNodeHandle { 1 };
    AttributeHandle nextAttributeHandle { 1 };


    std::unordered_map<NodeHandle, std::shared_ptr<Node>> m_nodes;
    std::unordered_map<AttributeHandle, std::shared_ptr<Attribute>> attributes;
    std::unordered_map<AttributeHandle, NodeHandle> nodeAttributes;
    std::vector<Connection> connections;

    std::vector<EventBus::SubscriptionId> m_subscriptions;

};

} // namespace cf::core

#endif // CF_CORE_SCENE