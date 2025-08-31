#include "AddNode.hpp"

namespace cf::core {
Status AddNode::compute()
{
    m_result = m_inputA + m_inputB;

    return Status::eOK;
}

TypeHandle AddNode::getType() const
{
    // TODO
    return kInvalidNodeHandle;
}

} // namespace cf::core