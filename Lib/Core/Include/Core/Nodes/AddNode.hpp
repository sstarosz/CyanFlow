#ifndef CF_CORE_NODES_ADDNODE_HPP
#define CF_CORE_NODES_ADDNODE_HPP

#include "Core/InputAttribute.hpp"
#include "Core/Node.hpp"
#include "Core/OutputAttribute.hpp"

namespace cf::core {

class AddNode : public NodeBase<AddNode> {
public:
    struct Inputs {
        InputAttribute<float> input1;
        InputAttribute<float> input2;
    } inputs;

    struct Outputs {
        OutputAttribute<float> result;
    } outputs;

    AddNode() = default;
    ~AddNode() override = default;

    Status compute() override;

    static NodeDescriptor getStaticDescriptor()
    {
        static NodeDescriptor descriptor = initialize();
        return descriptor;
    }

    static NodeDescriptor initialize()
    {
        NodeDescriptor descriptor;
        descriptor.typeName = "Add Node";

        AttributeDescriptor input1Desc = addInputAttributeDescriptor(
            &Inputs::input1,
            "Input 1");
            
        AttributeDescriptor input2Desc = addInputAttributeDescriptor(
            &Inputs::input2,
            "Input 2");

        AttributeDescriptor outputDesc = addOutputAttributeDescriptor(
            &Outputs::result,
            "Result");

        descriptor.attributes.push_back(input1Desc);
        descriptor.attributes.push_back(input2Desc);
        descriptor.attributes.push_back(outputDesc);

        return descriptor;
    }
};

} // namespace cf::core

#endif // CF_CORE_NODES_ADDNODE_HPP