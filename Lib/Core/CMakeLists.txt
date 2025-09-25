AddLibrary(
    Core
    VERSION 0.0.1
    DESCRIPTION "Core library that provides essential functionalities and utilities"
    SOURCES
        Source/Scene.cpp
        Source/Document.cpp
        Source/Command.cpp
        Source/Attribute.cpp

        Source/Nodes/AddNode.cpp
    PUBLIC_HEADERS
        Include/Core/Scene.hpp
        Include/Core/TypeDescriptors.hpp
        Include/Core/TypeRegistry.hpp
        Include/Core/Node.hpp
        Include/Core/Document.hpp
        Include/Core/Command.hpp

        #Nodes
        Include/Core/Nodes/AddNode.hpp
        
    PRIVATE_HEADERS

    PUBLIC_LIBS
        spdlog::spdlog
)
