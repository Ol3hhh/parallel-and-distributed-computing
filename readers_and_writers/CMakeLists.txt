cmake_minimum_required(VERSION 3.10)

project(ReadersWritersProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(readers_writers main.cpp types.hpp)

target_link_libraries(readers_writers PRIVATE Threads::Threads)

target_compile_options(readers_writers PRIVATE -Wall -Wextra)