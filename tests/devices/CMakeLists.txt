file(GLOB EXAMPLE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE_FILE_PATH ${EXAMPLE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE_PATH} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE_PATH})
    
    target_link_libraries(${EXAMPLE_NAME} PRIVATE RobotSimulationLib)
    
endforeach()
