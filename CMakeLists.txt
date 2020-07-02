cmake_minimum_required(VERSION 3.10)
project(sensors_project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp/include/)
add_subdirectory("${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp")


add_executable(sensors main.cpp)


if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    target_link_libraries(sensors PRIVATE yaml-cpp pthread atomic)
elseif( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
    target_link_libraries(sensors PRIVATE yaml-cpp)
endif()