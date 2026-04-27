cmake_minimum_required(VERSION 3.15)

project(HttpServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(server/basic)
add_subdirectory(server/network)
add_subdirectory(examples)