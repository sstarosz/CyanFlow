AddTests(
    CoreTests
    VERSION 1.0
    DESCRIPTION "Unit tests for the Core module"
    SOURCES
        AttributeTests.cpp
        TypeRegistryTests.cpp
        UndoStackTests.cpp
    TEST_LIBS
        cf::Core
    PUBLIC_LIBS
        
    PRIVATE_LIBS
)