cmake_minimum_required(VERSION 3.10)

project(DiningPhilosophers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Curses REQUIRED)

add_executable(dining_philosophers 
    main.cpp 
    philosopher.cpp
)

target_include_directories(dining_philosophers PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(dining_philosophers 
    PRIVATE 
    Threads::Threads 
    ${CURSES_LIBRARIES}
)