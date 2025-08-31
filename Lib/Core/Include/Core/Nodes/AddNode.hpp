#ifndef CF_CORE_NODES_ADDNODE
#define CF_CORE_NODES_ADDNODE

#include "Core/Node.hpp"

namespace cf::core {
class AddNode : public Node {
public:
    AddNode() = default;
    ~AddNode() override = default;

    Status compute() override;
    TypeHandle getType() const override;

private:
    float m_inputA = 0.0f;
    float m_inputB = 0.0f;
    float m_result = 0.0f;
};

} // namespace cf::core

#endif // CF_CORE_NODES_ADDNODE