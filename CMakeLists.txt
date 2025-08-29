cmake_minimum_required(VERSION 3.26)

project(CyanFlow
        VERSION 0.0.1 
        DESCRIPTION  ""
        LANGUAGES  CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#---------------------------------------------------------------------------------------#
#----------------------------------Project option---------------------------------------#
#---------------------------------------------------------------------------------------#
option(Build_Docs           "Determine if build documentation" OFF)
option(Build_Tests          "Determine if build unit tests" OFF)
option(Build_Examples       "Determine if build examples" OFF)
option(Build_Benchmarks     "Determine if build benchmarks" OFF)

option(Build_Tests_For_Gpu  "Determine if build unit that use Vulkan or other gpu features" OFF)

option(Enable_ASAN          "Build with AddressSanitizer" OFF) #TODO


#---------------------------------------------------------------------------------------#
#----------------------------------Project configuration-------------------------------#
#---------------------------------------------------------------------------------------#
include(CMake/Deploy.cmake)
include(CMake/AddLibrary.cmake)


if(MSVC)
    message(STATUS "MSVC detected")
    add_compile_options(/W4)            #Enable all warnings
    add_compile_options(/WX)            #Treat warnings as errors    
    add_compile_options(/permissive-)   #Disable language extensions
    add_compile_options(/EHsc)          #Enable C++ exception handling
else()
    add_compile_options(-Wall)          #Enable all warnings
    add_compile_options(-Wextra)        #Enable extra warnings
    add_compile_options(-Wpedantic)     #Enable pedantic warnings
    add_compile_options(-Werror)        #Treat warnings as errors
endif()


#---------------------------------------------------------------------------------------#
#----------------------------------Project dependicies----------------------------------#
#---------------------------------------------------------------------------------------#
find_package(Qt6 CONFIG REQUIRED
             COMPONENTS Core
                        Gui
                        Widgets
                        CoreTools
                        OpenGLWidgets
            )

find_package(Vulkan REQUIRED
                COMPONENTS  glslc 
                            shaderc_combined
            )

find_package(Stb                        REQUIRED)
find_package(spirv_cross_core   CONFIG  REQUIRED)
find_package(spirv_cross_glsl   CONFIG  REQUIRED)
find_package(Eigen3             CONFIG  REQUIRED)
find_package(spdlog             CONFIG  REQUIRED)

#---------------------------------------------------------------------------------------#
#----------------------------------Project Structure------------------------------------#
#---------------------------------------------------------------------------------------#
add_subdirectory(Lib)
add_subdirectory(App)
add_subdirectory(Assets)

if(Build_Docs)
    add_subdirectory(Docs)
endif()

if(Build_Benchmarks)
    add_subdirectory(Benchmarks)
endif()

if(Build_Tests)
    enable_testing()
    add_subdirectory(Tests)
endif()

if(Build_Examples)
    add_subdirectory(Examples)
endif()

