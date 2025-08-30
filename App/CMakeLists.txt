project(CyanFlow
        VERSION 0.0.1
 	DESCRIPTION "Main application which is the entry point of aplication" 
        LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME} "main.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE Framework)

Deploy_QT(${PROJECT_NAME})