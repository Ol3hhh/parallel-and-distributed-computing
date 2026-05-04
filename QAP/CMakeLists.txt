cmake_minimum_required(VERSION 3.12)

project(QAP_Solver 
    VERSION 1.0.0
    DESCRIPTION "Quadratic Assignment Problem Solver with Simulated Annealing"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add threading support
find_package(Threads REQUIRED)

# Define the executable
add_executable(qap_solver
    main.cpp
    graph.cpp
    SA.cpp
)

# Link libraries
target_link_libraries(qap_solver 
    PRIVATE Threads::Threads
)

# Compiler flags for optimization and warnings
if(MSVC)
    target_compile_options(qap_solver PRIVATE /W4)
else()
    target_compile_options(qap_solver PRIVATE -Wall -Wextra -Wpedantic)
    # Optimization flags
    target_compile_options(qap_solver PRIVATE -O3)
endif()

# Include directories
target_include_directories(qap_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
