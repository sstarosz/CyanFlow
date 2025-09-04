#ifndef CF_CORE_NODES_ADDNODE
#define CF_CORE_NODES_ADDNODE

#include "Core/Node.hpp"
#include "Core/InputAttribute.hpp"
#include "Core/OutputAttribute.hpp"

namespace cf::core {

class AddNode : public Node {
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
    TypeHandle getType() const override;

    
    static NodeDescriptor initialize()
    {
      NodeDescriptor descriptor;
      descriptor.typeName = "Add Node";

      //TODO Make handy function to create attribute descriptors
      AttributeDescriptor input1Desc;
      input1Desc.handle = TypeRegistry::getTypeHandle<float>();
      input1Desc.name = "Input 1";
      input1Desc.role = AttributeRole::eInput;
      input1Desc.setter = [](void* nodePtr, std::shared_ptr<Attribute> attribute) {
            auto* node = static_cast<AddNode*>(nodePtr);
            node->inputs.input1 = attribute;
        };

      AttributeDescriptor input2Desc;
      input2Desc.handle = TypeRegistry::getTypeHandle<float>();
      input2Desc.name = "Input 2";
      input2Desc.role = AttributeRole::eInput;
      input2Desc.setter = [](void* nodePtr, std::shared_ptr<Attribute> attribute) {
            auto* node = static_cast<AddNode*>(nodePtr);
            node->inputs.input2 = attribute;
        };

      AttributeDescriptor outputDesc;
      outputDesc.handle = TypeRegistry::getTypeHandle<float>();
      outputDesc.name = "Result";
      outputDesc.role = AttributeRole::eOutput;
      outputDesc.setter = [](void* nodePtr, std::shared_ptr<Attribute> attribute) {
            auto* node = static_cast<AddNode*>(nodePtr);
            node->outputs.result = attribute;
        };

      descriptor.attributes.push_back(input1Desc);
      descriptor.attributes.push_back(input2Desc);
      descriptor.attributes.push_back(outputDesc);


      return descriptor;
    }
};

} // namespace cf::core

#endif // CF_CORE_NODES_ADDNODE