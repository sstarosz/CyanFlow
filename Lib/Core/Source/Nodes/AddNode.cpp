#include "AddNode.hpp"
#include "spdlog/spdlog.h"

namespace cf::core {

Status AddNode::compute()
{
    outputs.result = inputs.input1 + inputs.input2;

    return Status::eOK;
}

} // namespace cf::core