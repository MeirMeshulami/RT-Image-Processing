cmake_minimum_required(VERSION 3.14)
project(libs VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set the gRPC and Protobuf package path for Debug/Release
if (WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Configurations.json ${CMAKE_BINARY_DIR}/Configurations.json COPYONLY) # For VS debugger

    if(Release)
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_PREFIX_PATH "C:/Program Files/grpc/release")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Configurations.json ${CMAKE_BINARY_DIR}/Release/Configurations.json COPYONLY)
        message("cmake prefix path: Release")
    else()
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_PREFIX_PATH "C:/Program Files/grpc/debug")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Configurations.json ${CMAKE_BINARY_DIR}/Debug/Configurations.json COPYONLY)
        message("cmake prefix path: Debug")
    endif()
endif()

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Find Protobuf package
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC package
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# Get proto files
set (proto_name service)
get_filename_component(proto "protos/${proto_name}.proto" ABSOLUTE)
get_filename_component(proto_dir "${proto}" DIRECTORY)

# Generate proto files
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_dir}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}" "${proto}"
    DEPENDS "${proto}")

# List and add header and source files
file(GLOB SOURCE_FILES "*.h" "*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${proto_hdrs} ${grpc_hdrs} ${proto_srcs} ${grpc_srcs})

# Include SOURCE_DIR and BINARY_DIR
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Include and link gRPC
target_include_directories(${PROJECT_NAME} PUBLIC ${Protobuf_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

# Include and link OpenCV
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

# Include and link json
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/json") 

# Include and link spdlog
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include") 

# Include and link sqlite
add_library(sqlite3 STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3/sqlite3.c")
target_include_directories(sqlite3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3")
target_link_libraries(${PROJECT_NAME} PUBLIC sqlite3)
