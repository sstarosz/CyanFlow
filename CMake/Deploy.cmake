#Ship Qt dynamic libraries using windeployqt
function(Deploy_QT TARGET)
    # Set the path to the windeployqt executable
    if(MSVC)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${QT6_INSTALL_PREFIX})
    endif()

    if(NOT WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "Could not find the windeployqt executable")
    endif()

    # Run windeployqt to copy the required Qt dynamic libraries
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE} 
        --debug                                                         #TODO: Remove this when we have a release build
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw 
        $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        COMMENT "Running windeployqt... on $<TARGET_FILE:${PROJECT_NAME}"
        VERBATIM
    )

endfunction()
