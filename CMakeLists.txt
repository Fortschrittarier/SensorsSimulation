cmake_minimum_required(VERSION 3.10)

project(sensors_project)

include_directories(${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp/include/)

add_subdirectory("${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp")

add_library(yaml-lib STATIC IMPORTED)
set_target_properties(yaml-lib PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp/build/libyaml-cpp.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rd_party/yaml-cpp/include"
)

add_executable(sensors main.cpp)

set_property(TARGET sensors PROPERTY CXX_STANDARD 17)


target_link_libraries(sensors pthread atomic yaml-lib)
