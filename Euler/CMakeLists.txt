cmake_minimum_required(VERSION 3.10)

project(EulerProject CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(Euler 
    main.cpp 
    server.cpp 
    client.cpp
)

option(EULER_ENABLE_AVX2 "Enable AVX2 optimizations for Euler client" OFF)
if(EULER_ENABLE_AVX2)
    target_compile_options(Euler PRIVATE -mavx2)
endif()

target_link_libraries(Euler PRIVATE m)