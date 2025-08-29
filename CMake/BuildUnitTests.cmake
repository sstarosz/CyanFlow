


add_executable(${PROJECT_NAME} ${Tests_Sources} ${CMAKE_SOURCE_DIR}/Tests/TestMain.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
                        ${Test_Library} 
                        GTest::gtest 
                        GTest::gmock)



add_test(NAME ${Test_Name} COMMAND ${PROJECT_NAME})
