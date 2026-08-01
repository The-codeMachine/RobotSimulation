cmake_minimum_required(VERSION 3.15)
project(RobotSimulationLib VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RobotSimulationLib
    src/world.cpp
    src/robot.cpp
    src/object.cpp
    src/Devices/Motor.cpp
)

target_include_directories(RobotSimulationLib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

enable_testing()

add_subdirectory(tests)