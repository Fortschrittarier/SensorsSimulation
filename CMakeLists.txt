cmake_minimum_required(VERSION 3.10)
project(sensors_project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# yaml-cpp
include_directories("${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp/include/")
add_subdirectory("${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp")

# GTest
add_subdirectory("3rd_party/googletest")
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# main
add_executable(sensors main.cpp)

# Platform libs
if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    target_link_libraries(sensors PRIVATE yaml-cpp pthread atomic)
elseif( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
    target_link_libraries(sensors PRIVATE yaml-cpp)
endif()

# Tests
add_executable(test_main ${CMAKE_SOURCE_DIR}/tests/test_main.cpp)
target_link_libraries(test_main gtest_main)
add_test(NAME test_main COMMAND test_main)