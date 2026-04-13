cmake_minimum_required(VERSION 3.10)

project(EulerProject CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -Wall -Wextra")

add_executable(Euler 
    main.cpp 
    server.cpp 
    client.cpp
)

target_link_libraries(Euler PRIVATE m)