file(GLOB EXAMPLE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE_FILE_PATH ${EXAMPLE_FILES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE_PATH} NAME_WE)
    
    # 1. Prefix the target name to guarantee unique global identity
    set(TARGET_TARGET "example_${EXAMPLE_NAME}")

    add_executable(${TARGET_TARGET} ${EXAMPLE_FILE_PATH})
    target_link_libraries(${TARGET_TARGET} PRIVATE RobotSimulationLib)
    
    # 2. (Optional) Force the final compiled binary filename to still be just "intro.exe"
    set_target_properties(${TARGET_TARGET} PROPERTIES 
        OUTPUT_NAME ${EXAMPLE_NAME}
    )
endforeach()
