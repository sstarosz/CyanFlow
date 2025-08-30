AddLibrary(
    Framework
    VERSION 0.0.1
    DESCRIPTION "Core framework library that provides the main application logic and structure"
    SOURCES
        Source/Application.cpp
    PUBLIC_HEADERS
        Include/Framework/Application.hpp
    PRIVATE_HEADERS

    PUBLIC_LIBS
        cf::Core
        cf::Ui
        Qt6::Widgets
        spdlog::spdlog
)
