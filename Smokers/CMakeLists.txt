cmake_minimum_required(VERSION 3.10)

project(Smokers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Smokers 
    main.cpp 
    types.cpp
)

target_include_directories(Smokers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})