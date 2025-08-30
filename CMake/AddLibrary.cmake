function(AddLibrary TARGET_NAME)

    set(options)
    set(oneValueArgs VERSION DESCRIPTION)
    set(multiValueArgs SOURCES PUBLIC_HEADERS PRIVATE_HEADERS COMPILER_FLAGS PUBLIC_LIBS PRIVATE_LIBS)
    cmake_parse_arguments(MVL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    project(${TARGET_NAME}
        VERSION ${MVL_VERSION}
        DESCRIPTION "${MVL_DESCRIPTION}"
        LANGUAGES CXX
    )

    add_library(${TARGET_NAME} ${MVL_SOURCES} ${MVL_PUBLIC_HEADERS} ${MVL_PRIVATE_HEADERS})
    add_library(cf::${TARGET_NAME} ALIAS ${TARGET_NAME})

    target_include_directories(${TARGET_NAME} PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/")
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET_NAME}")
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include/${TARGET_NAME}/Nodes")

    target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)

    if(MVL_COMPILER_FLAGS)
        target_compile_options(${TARGET_NAME} PRIVATE ${MVL_COMPILER_FLAGS})
    endif()

    if(MVL_PUBLIC_LIBS)
        target_link_libraries(${TARGET_NAME} PUBLIC ${MVL_PUBLIC_LIBS})
    endif()

    if(MVL_PRIVATE_LIBS)
        target_link_libraries(${TARGET_NAME} PRIVATE ${MVL_PRIVATE_LIBS})
    endif()

endfunction()