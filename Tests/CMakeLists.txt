# Use the new behavior for CMP0135 (FetchContent download timestamp)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

cmake_minimum_required(VERSION 3.14)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define a DEBUG option
option(DEBUG "Build in Debug mode" OFF)

# Include GeneralLib as a subdirectory
add_subdirectory(../GeneralLib ${CMAKE_CURRENT_BINARY_DIR}/GeneralLib)

# Main executable
add_executable(${PROJECT_NAME} Tests.cpp)

# Link to GeneralLib and gtest/gmock
target_link_libraries(${PROJECT_NAME} PRIVATE GeneralLib gtest_main gmock_main)

# Check if googletest target already exists
if (NOT TARGET googletest)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/master.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_test(NAME Test COMMAND ${PROJECT_NAME})

# Post build command to run tests
add_custom_command(
    TARGET ${PROJECT_NAME}
    COMMENT "Run Tests"
    POST_BUILD
    COMMAND ${PROJECT_NAME}
)
