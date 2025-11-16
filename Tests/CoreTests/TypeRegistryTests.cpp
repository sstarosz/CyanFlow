#include "Core/DataTypes.hpp"
#include "Core/InputAttribute.hpp"
#include "Core/Node.hpp"
#include "Core/OutputAttribute.hpp"
#include "Core/TypeRegistry.hpp"
#include "gtest/gtest.h"

namespace cf::core::test {

struct MyCustomType { };

struct TestNode : public NodeBase<TestNode> {
    Status compute() override
    {
        return Status::eOK;
    }

    static NodeDescriptor initialize()
    {
        NodeDescriptor desc;
        desc.typeName = "cf::core::test::TestNode";
        return desc;
    }
};

struct TestNodeWithAttributes : public NodeBase<TestNodeWithAttributes> {
    struct Inputs {
        InputAttribute<float> input1;
        InputAttribute<float> input2;
    } inputs;

    struct Outputs {
        OutputAttribute<float> result;
    } outputs;

    Status compute() override
    {
        return Status::eOK;
    }

    static NodeDescriptor initialize()
    {
        NodeDescriptor descriptor;
        descriptor.typeName = "cf::core::test::TestNodeWithAttributes";

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

struct TestEvent { };

class TypeRegistryTest : public ::testing::Test {
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        TypeRegistry::clearInstance();
    }
};

TEST_F(TypeRegistryTest, SingletonInstance)
{
    auto& instance1 = TypeRegistry::getInstance();
    auto& instance2 = TypeRegistry::getInstance();

    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(TypeRegistryTest, ClearInstanceResetsState)
{
    // Register some types first
    TypeRegistry::registerType<int>();
    TypeRegistry::registerType<double>();

    // Verify they exist
    EXPECT_NO_THROW(TypeRegistry::getTypeDescriptor<int>());
    EXPECT_NO_THROW(TypeRegistry::getTypeDescriptor<double>());

    // Clear instance
    TypeRegistry::clearInstance();

    // Now they should not exist
    EXPECT_THROW(TypeRegistry::getTypeDescriptor<int>(), std::runtime_error);
    EXPECT_THROW(TypeRegistry::getTypeDescriptor<double>(), std::runtime_error);
}

TEST_F(TypeRegistryTest, ExtractNameFromType)
{
    TypeRegistry::registerType<Int32>();
    TypeRegistry::registerType<MyCustomType>();

    auto int32Desc = TypeRegistry::getTypeDescriptor<Int32>();
    auto desc = TypeRegistry::getTypeDescriptor<MyCustomType>();

    EXPECT_EQ(int32Desc.name, "cf::core::Int32");
    EXPECT_EQ(desc.name, "cf::core::test::MyCustomType");
}

// Node Registration Tests
TEST_F(TypeRegistryTest, RegisterAndRetrieveNodeType)
{
    TypeRegistry::registerNodeType<TestNode>();

    auto desc = TypeRegistry::getNodeDescriptor<TestNode>();

    EXPECT_EQ(desc.typeName, "cf::core::test::TestNode");
    EXPECT_EQ(desc.handle, TypeRegistry::getNodeDescriptorHandle<TestNode>());
    EXPECT_EQ(desc.attributes.size(), 0);
}

TEST_F(TypeRegistryTest, RegisterNodeWithAttributes)
{
    TypeRegistry::registerNodeType<TestNodeWithAttributes>();

    auto desc = TypeRegistry::getNodeDescriptor<TestNodeWithAttributes>();

    EXPECT_EQ(desc.typeName, "cf::core::test::TestNodeWithAttributes");
    EXPECT_EQ(desc.handle, TypeRegistry::getNodeDescriptorHandle<TestNodeWithAttributes>());
    EXPECT_EQ(desc.attributes.size(), 3);

    EXPECT_EQ(desc.attributes[0].name, "Input 1");
    EXPECT_EQ(desc.attributes[0].role, AttributeRole::eInput);

    EXPECT_EQ(desc.attributes[1].name, "Input 2");
    EXPECT_EQ(desc.attributes[1].role, AttributeRole::eInput);

    EXPECT_EQ(desc.attributes[2].name, "Result");
    EXPECT_EQ(desc.attributes[2].role, AttributeRole::eOutput);
}

} // namespace cf::core::test