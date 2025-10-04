#include "Core/Attribute.hpp"
#include "Core/DataTypes.hpp"
#include "Core/TypeRegistry.hpp"
#include "gtest/gtest.h"

using namespace cf;
using namespace cf::core;

class AttributeTest : public ::testing::Test {
protected:
    void SetUp() override {
        registerTestTypes();

        intHandle = TypeRegistry::getTypeHandle<Int32>();
        stringHandle = TypeRegistry::getTypeHandle<String>();

        //Register int attribute descriptor
        intDescriptor.name = "Int32 Attribute";
        intDescriptor.typeHandle = intHandle;
        TypeRegistry::registerAttributeDescriptor(intDescriptor);

        //Register string attribute descriptor
        stringDescriptor.name = "String Attribute";
        stringDescriptor.typeHandle = stringHandle;
        TypeRegistry::registerAttributeDescriptor(stringDescriptor);
    }

    void TearDown() override {
    }

    AttributeHandle kTestableHandle = 2;
    TypeHandle intHandle;
    TypeHandle stringHandle;
    AttributeDescriptor intDescriptor;
    AttributeDescriptor stringDescriptor;

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
};

TEST_F(AttributeTest, BasicUsage)
{
    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();
    TypeRegistry::registerAttributeDescriptor(desc);

    Attribute attr(desc, kTestableHandle);

    EXPECT_EQ(attr.getHandle(), kTestableHandle);
    EXPECT_EQ(attr.getAttributeDescriptor().name, "Test Attribute");
    EXPECT_EQ(attr.getAttributeDescriptor().typeHandle, TypeRegistry::getTypeHandle<Int32>());
    EXPECT_EQ(attr.getAttributeDescriptor().role, AttributeRole::eInput);
    EXPECT_EQ(attr.getAttributeDescriptor().setter, nullptr);

    EXPECT_EQ(attr.getValue<Int32>(), 0);

    attr.setValue(39);
    EXPECT_EQ(attr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, InvalidAttribute)
{
    Attribute attr;
    EXPECT_THROW(attr.getHandle(), std::runtime_error);
    EXPECT_THROW(attr.getAttributeDescriptor(), std::runtime_error);
    EXPECT_THROW(attr.getValue<Int32>(), std::runtime_error);
    EXPECT_THROW(attr.setValue(39), std::runtime_error);
}

TEST_F(AttributeTest, TypeMismatch)
{
    AttributeDescriptor desc;
    desc.name = "Test Attribute";
    desc.typeHandle = TypeRegistry::getTypeHandle<Int32>();
    TypeRegistry::registerAttributeDescriptor(desc);

    Attribute attr(desc, kTestableHandle);

    EXPECT_THROW(attr.getValue<Float>(), std::runtime_error);
    EXPECT_THROW(attr.setValue(3.14f), std::runtime_error);
}


/*--------------------------------------------------------------*/
/*---------------------Assignment-to-Attribute------------------*/
/*--------------------------------------------------------------*/


// Basic value category tests
TEST_F(AttributeTest, LvalueReference) {
    Attribute attr(intDescriptor, kTestableHandle);
    Int32 value = 39;
    
    EXPECT_NO_THROW(attr.setValue(value));
    EXPECT_EQ(attr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, ConstLvalueReference) {
    Attribute attr(intDescriptor, kTestableHandle);
    const Int32 value = 39;
    
    EXPECT_NO_THROW(attr.setValue(value));
    EXPECT_EQ(attr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, RvalueReference) {
    Attribute attr(intDescriptor, kTestableHandle);
    
    EXPECT_NO_THROW(attr.setValue(200));
    EXPECT_EQ(attr.getValue<Int32>(), 200);
}

TEST_F(AttributeTest, TemporaryObjects) {
    Attribute attr(stringDescriptor, kTestableHandle);
    
    EXPECT_NO_THROW(attr.setValue(String("test")));
    EXPECT_EQ(attr.getValue<String>(), "test");
}

// Attribute type copying tests
TEST_F(AttributeTest, AttributeSharedPtr) {
    auto sourceAttr = std::make_shared<Attribute>(intDescriptor, kTestableHandle);
    sourceAttr->setValue(39);
    Attribute targetAttr(intDescriptor, kTestableHandle + 1);
    

    EXPECT_NO_THROW(targetAttr.setValue(sourceAttr));
    EXPECT_EQ(targetAttr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, AttributeReference) {
    auto sourceAttr = std::make_shared<Attribute>(intDescriptor, kTestableHandle);
    sourceAttr->setValue(39);
    Attribute targetAttr(intDescriptor, kTestableHandle + 1);
    
    EXPECT_NO_THROW(targetAttr.setValue(*sourceAttr));
    EXPECT_EQ(targetAttr.getValue<Int32>(), 39);
}

TEST_F(AttributeTest, AttributeConstReference) {
    auto sourceAttr = std::make_shared<Attribute>(intDescriptor, kTestableHandle);
    sourceAttr->setValue(39);
    Attribute targetAttr(intDescriptor, kTestableHandle + 1);
    
    EXPECT_NO_THROW(targetAttr.setValue(static_cast<const Attribute&>(*sourceAttr)));
    EXPECT_EQ(targetAttr.getValue<Int32>(), 39);
}

/*--------------------------------------------------------------*/
/*---------------------End Assignment-to-Attribute--------------*/
/*--------------------------------------------------------------*/


