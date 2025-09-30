function(AddTests test_name)
    
    set(options)
    set(oneValueArgs VERSION DESCRIPTION)
    set(multiValueArgs SOURCES TEST_LIBS PUBLIC_LIBS PRIVATE_LIBS COMPILER_FLAGS)
    cmake_parse_arguments(MVL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    project(${test_name}
        VERSION ${MVL_VERSION}
        DESCRIPTION "${MVL_DESCRIPTION}"
        LANGUAGES CXX
    )

    add_executable(${test_name} ${MVL_SOURCES})
    target_sources(${test_name} PRIVATE "${CMAKE_SOURCE_DIR}/Tests/TestMain.cpp")

    target_compile_features(${test_name} PRIVATE cxx_std_23)

    if(MVL_COMPILER_FLAGS)
        target_compile_options(${TARGET_NAME} PRIVATE ${MVL_COMPILER_FLAGS})
    endif()

    if(MVL_TEST_LIBS)
        target_link_libraries(${test_name} PRIVATE ${MVL_TEST_LIBS})
    endif()

    if(MVL_PUBLIC_LIBS)
        target_link_libraries(${test_name} PUBLIC ${MVL_PUBLIC_LIBS})
    endif()

    if(MVL_PRIVATE_LIBS)
        target_link_libraries(${test_name} PRIVATE ${MVL_PRIVATE_LIBS})
    endif()

    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gmock)

    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()