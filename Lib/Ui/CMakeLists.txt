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
    PUBLIC_HEADERS
        Include/Ui/MainWindow.hpp
        Include/Ui/GuiManager.hpp
        Include/Ui/NodeEditor.hpp
    PRIVATE_HEADERS

    PUBLIC_LIBS
        cf::Core
        Qt6::Widgets
        Qt6::OpenGLWidgets
        spdlog::spdlog
)
