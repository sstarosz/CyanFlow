#include "AddNode.hpp"

namespace cf::core {
Status AddNode::compute()
{
    outputs.result = inputs.input1 + inputs.input2;

    return Status::eOK;
}

TypeHandle AddNode::getType() const
{
    return TypeRegistry::getTypeHandle<AddNode>();
}

} // namespace cf::core