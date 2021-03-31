cmake_minimum_required(VERSION 3.2)

# our project
project(ffsrv C CXX)

# build options
option(BUILD_DB        "Build database"   OFF)
option(BUILD_DEPS_ONLY "Builds deps only" OFF)

# compilation options
add_compile_options(-fPIC)

# update our cmake module path (to find our custom cmake modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules/)

# configure variables referenced in the build
set(THIRDPARTY_ROOT ${CMAKE_SOURCE_DIR}/thirdparty CACHE PATH "Third party libraries")

# configure output directories
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# flags
set(other_flags "${other_flags} -fvisibility=default")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${other_flags})

# grpc
add_subdirectory(grpc EXCLUDE_FROM_ALL)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_GRPCPP grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

# testing
enable_testing()
include(CTest)
set(CTEST_OUTPUT_ON_FAILURE 1)
add_subdirectory(ext/gtest)

# db
set(FIREFLY_DB_NAME "grpc_cmake_test_db" CACHE STRING "The DB name")
if(BUILD_DB)
    add_subdirectory(db)
endif()

# ca, host / client name (testing)
set(CA_NAME          "ffsrv.fireflytech.org" CACHE STRING "Test CA name")
set(SERVER_HOST_NAME "$ENV{HOSTNAME}"        CACHE STRING "Test server host name")
set(CLIENT_HOST_NAME "$ENV{HOSTNAME}"        CACHE STRING "Test client host name")
add_definitions(-DCA_NAME="${CA_NAME}" -DSERVER_HOST_NAME="${SERVER_HOST_NAME}" -DCLIENT_HOST_NAME="${CLIENT_HOST_NAME}" -DDB_NAME="${FIREFLY_ANYWHERE_DB_NAME}" -DTRACE_TO_STD_OUT=1)

# server
add_subdirectory(server)

# test
if(NOT BUILD_DEPS_ONLY)
    add_subdirectory(test)
endif()
