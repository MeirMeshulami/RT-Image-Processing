cmake_minimum_required(VERSION 3.14)
project(Server VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define a DEBUG option
option(DEBUG "Build in Debug mode" OFF)

# Include GeneralLib as a subdirectory
add_subdirectory(../GeneralLib ${CMAKE_CURRENT_BINARY_DIR}/GeneralLib)
add_subdirectory(../GUI ${CMAKE_CURRENT_BINARY_DIR}/GUI)

# Main executable
add_executable(${PROJECT_NAME} main.cpp)

# Link to GeneralLib
target_link_libraries(${PROJECT_NAME} PRIVATE GeneralLib)