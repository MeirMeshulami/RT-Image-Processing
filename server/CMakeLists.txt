cmake_minimum_required(VERSION 3.14)
project(Camera VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define a DEBUG option
option(DEBUG "Build in Debug mode" OFF)

# Include libs as a subdirectory
add_subdirectory(../libs ${CMAKE_CURRENT_BINARY_DIR}/libs)

# Main executable
add_executable(${PROJECT_NAME} Camera.cpp)

# Link to libs
target_link_libraries(${PROJECT_NAME} PRIVATE libs)