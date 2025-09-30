#include "Core/Attribute.hpp"
#include "Core/DataTypes.hpp"
#include "Core/TypeRegistry.hpp"
#include "gtest/gtest.h"
#include <print>

using namespace cf;
using namespace cf::core;

// Register types for testing
void registerTestTypes()
{
    TypeRegistry::registerType<Bool>("Bool");

    TypeRegistry::registerType<Int32>("Int32");
    TypeRegistry::registerType<UInt32>("UInt32");
    TypeRegistry::registerType<Int64>("Int64");
    TypeRegistry::registerType<UInt64>("UInt64");

    TypeRegistry::registerType<Float>("Float");
    TypeRegistry::registerType<Double>("Double");

    TypeRegistry::registerType<String>("String");
}

class AttributeTest : public ::testing::Test {
protected:
    void SetUp() override { registerTestTypes(); } // called before every test
};

TEST_F(AttributeTest, BasicUsage)
{

    constexpr AttributeHandle kTestableHandle = 2;

    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();

    Attribute attr(desc, kTestableHandle);

    EXPECT_EQ(attr.getHandle(), kTestableHandle);
    EXPECT_EQ(attr.getAttributeDescriptor().name, "Test Attribute");
    EXPECT_EQ(attr.getAttributeDescriptor().typeHandle, TypeRegistry::getTypeHandle<Int32>());
    EXPECT_EQ(attr.getAttributeDescriptor().role, AttributeRole::eInput);
    EXPECT_EQ(attr.getAttributeDescriptor().setter, nullptr);

    EXPECT_EQ(attr.getValue<Int32>(), 0);

    attr.setValue<Int32>(39);
    EXPECT_EQ(attr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, InvalidAttribute)
{
    Attribute attr;
    EXPECT_EQ(attr.getHandle(), kInvalidAttributeHandle);
    EXPECT_THROW(attr.getAttributeDescriptor(), std::runtime_error);
    EXPECT_THROW(attr.getValue<Int32>(), std::runtime_error);
    EXPECT_THROW(attr.setValue<Int32>(42), std::runtime_error);
}

TEST_F(AttributeTest, TypeMismatch)
{
    constexpr AttributeHandle kTestableHandle = 3;

    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();

    Attribute attr(desc, kTestableHandle);

    EXPECT_THROW(attr.getValue<Float>(), std::runtime_error);
    EXPECT_THROW(attr.setValue<Float>(3.14f), std::runtime_error);
}

TEST_F(AttributeTest, CopyFromAnotherAttribute)
{
    constexpr AttributeHandle kTestableHandle1 = 4;
    constexpr AttributeHandle kTestableHandle2 = 5;

    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();

    Attribute attr1(desc, kTestableHandle1);
    Attribute attr2(desc, kTestableHandle2);

    attr1.setValue<Int32>(123);
    EXPECT_EQ(attr1.getValue<Int32>(), 123);
    EXPECT_EQ(attr2.getValue<Int32>(), 0);

    attr2.setValue<const std::shared_ptr<Attribute>&>(std::make_shared<Attribute>(attr1));
    EXPECT_EQ(attr2.getValue<Int32>(), 123);

    attr1.setValue<Int32>(456);
    EXPECT_EQ(attr1.getValue<Int32>(), 456);
    EXPECT_EQ(attr2.getValue<Int32>(), 123); // Ensure attr2 remains unchanged
}

TEST_F(AttributeTest, CopyFromNullAttribute)
{
    constexpr AttributeHandle kTestableHandle1 = 6;
    constexpr AttributeHandle kTestableHandle2 = 7;

    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();

    Attribute attr1(desc, kTestableHandle1);
    Attribute attr2(desc, kTestableHandle2);

    attr1.setValue<Int32>(123);
    EXPECT_EQ(attr1.getValue<Int32>(), 123);
    EXPECT_EQ(attr2.getValue<Int32>(), 0);

    std::shared_ptr<Attribute> nullAttr = nullptr;
    EXPECT_THROW(attr2.setValue<const std::shared_ptr<Attribute>&>(nullAttr), std::runtime_error);
    EXPECT_EQ(attr2.getValue<Int32>(), 0); // Ensure attr2 remains unchanged
}

TEST_F(AttributeTest, CopyFromTypeMismatch)
{
    constexpr AttributeHandle kTestableHandle1 = 8;
    constexpr AttributeHandle kTestableHandle2 = 9;

    AttributeDescriptor desc1;
    desc1.name = "Int32 Attribute";
    desc1.typeHandle = TypeRegistry::getTypeHandle<Int32>();

    AttributeDescriptor desc2;
    desc2.name = "Float Attribute";
    desc2.typeHandle = TypeRegistry::getTypeHandle<Float>();

    Attribute attr1(desc1, kTestableHandle1);
    Attribute attr2(desc2, kTestableHandle2);

    attr1.setValue<Int32>(123);
    EXPECT_EQ(attr1.getValue<Int32>(), 123);
    EXPECT_EQ(attr2.getValue<Float>(), 0.0f);

    EXPECT_THROW(attr2.setValue<const std::shared_ptr<Attribute>&>(std::make_shared<Attribute>(attr1)), std::runtime_error);
    EXPECT_EQ(attr2.getValue<Float>(), 0.0f); // Ensure attr2 remains unchanged
}
