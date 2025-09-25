#include "AddNode.hpp"
#include "spdlog/spdlog.h"

namespace cf::core {
Status AddNode::compute()
{
    spdlog::info("AddNode '{}' computed: {} + {} = {}", getName(),
    static_cast<float>(inputs.input1),
    static_cast<float>(inputs.input2),
    static_cast<float>(outputs.result));
    
    outputs.result = inputs.input1 + inputs.input2;

    spdlog::info("AddNode '{}' result: {}", getName(), 
    static_cast<float>(outputs.result));

    return Status::eOK;
}

} // namespace cf::core