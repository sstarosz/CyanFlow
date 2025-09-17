set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

AddLibrary(
    Ui
    VERSION 0.0.1
    DESCRIPTION "User interface library that provides the graphical user interface components and layout"
    SOURCES
        Source/MainWindow.cpp
        Source/GuiManager.cpp
        Source/NodeEditor.cpp
        Source/AttributeEditor.cpp
        Source/SelectionManager.cpp
        Source/QtApplicationContext.cpp
        
    PUBLIC_HEADERS
        Include/Ui/MainWindow.hpp
        Include/Ui/GuiManager.hpp
        Include/Ui/NodeEditor.hpp
        Include/Ui/AttributeEditor.hpp
        Include/Ui/SelectionManager.hpp
        Include/Ui/QtApplicationContext.hpp
    PRIVATE_HEADERS

    PUBLIC_LIBS
        cf::Core
        cf::Framework
        Qt6::Widgets
        Qt6::OpenGLWidgets
        spdlog::spdlog
)
