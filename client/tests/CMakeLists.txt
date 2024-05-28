cmake_minimum_required(VERSION 3.14)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define a DEBUG option
option(DEBUG "Build in Debug mode" OFF)

# Main executable
add_executable(${PROJECT_NAME} client_tests.cpp)

# Run MOC on the header file that contains QObject-derived classes
# set(MOC_HEADERS
#     ../CoreAPI.h
# )

# qt_wrap_cpp(${PROJECT_NAME}_MOC_SOURCES ${MOC_HEADERS})

# Link the test executable with Google Test and the required libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main gmock_main libs Qt6::Widgets)

# Append the MOC-generated sources to the target
# target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_MOC_SOURCES})

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
