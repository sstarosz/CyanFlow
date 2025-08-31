#ifndef CF_CORE_SCENE
#define CF_CORE_SCENE

#include "Core/Node.hpp"

#include <memory>
#include <unordered_map>

namespace cf::core {
class Scene {
public:
    void addNode(std::unique_ptr<Node> node)
    {
        NodeHandle handle = generateNodeHandle();
        m_nodes[handle] = std::move(node);
        m_nodes[handle]->setName("Node " + std::to_string(handle));
    }

    const std::unordered_map<NodeHandle, std::shared_ptr<Node>>& getNodes() const { return m_nodes; }

private:
    NodeHandle generateNodeHandle() { return m_nextNodeHandle++; }

    NodeHandle m_nextNodeHandle = 1;

    std::unordered_map<NodeHandle, std::shared_ptr<Node>> m_nodes;
};

} // namespace cf::core

#endif // CF_CORE_SCENE