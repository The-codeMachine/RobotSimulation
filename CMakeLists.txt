cmake_minimum_required(VERSION 3.15)

project(RobotSimulationLib
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)

file(GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(RobotSimulationLib ${SOURCE_FILES})

target_include_directories(RobotSimulationLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(RobotSimulationLib
    PUBLIC
        nlohmann_json::nlohmann_json
)

enable_testing()

add_subdirectory(tests)
add_subdirectory(examples) 