AddTests(
    CoreTests
    VERSION 1.0
    DESCRIPTION "Unit tests for the Core module"
    SOURCES
        AttributeTests.cpp
    TEST_LIBS
        cf::Core
    PUBLIC_LIBS
        
    PRIVATE_LIBS
)