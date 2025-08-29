project(CyanFlow
        VERSION 0.0.1
 		DESCRIPTION "Main application which is the entry point of aplication" 
        LANGUAGES CXX)



set(Sources "main.cpp")


add_executable(${PROJECT_NAME} ${Headers} ${Sources})



target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
