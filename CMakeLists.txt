# Use the new behavior for CMP0135 (FetchContent download timestamp)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

cmake_minimum_required(VERSION 3.14)
project(CarDetction VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define a DEBUG option
option(DEBUG "Build in Debug mode" OFF)

# Set targets list
set(targets Camera Server Tests)

# Include GeneralLib as a subdirectory
add_subdirectory(GeneralLib ${CMAKE_CURRENT_BINARY_DIR}/GeneralLib)

foreach(target ${targets})
    add_executable(${target} ${target}/${target}.cpp)
    target_link_libraries(${target} PRIVATE GeneralLib)

    if(${target} STREQUAL "Tests")
        target_link_libraries(${target} PRIVATE gtest_main gmock_main)
        
        if (NOT TARGET googletest) # Check if googletest target already exists
            include(FetchContent)
            FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/refs/heads/master.zip
            )
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(googletest)
        endif()

        add_test(NAME Test COMMAND ${target})
        
        # Post build command to run tests
        add_custom_command(
            TARGET ${target}
            COMMENT "Run tests"
            POST_BUILD
            COMMAND ${target}
        )
    endif()
endforeach()
