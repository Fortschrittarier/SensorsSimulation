cmake_minimum_required(VERSION 3.10)

project(sensors)


add_executable(sensors main.cpp)


target_link_libraries(sensors pthread atomic)
