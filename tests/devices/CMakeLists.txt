file(GLOB TEST_FILES "*.cpp")

foreach(TEST_FILE_PATH ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE_PATH} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_FILE_PATH})
    target_link_libraries(${TEST_NAME} PRIVATE RobotSimulationLib)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    set_tests_properties(${TEST_NAME} PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endforeach()