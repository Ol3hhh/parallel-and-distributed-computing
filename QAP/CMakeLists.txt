cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(QAP_Optimizer LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

string(APPEND CMAKE_CUDA_FLAGS " -use_fast_math -O3")
string(APPEND CMAKE_CXX_FLAGS " -O3")

option(USE_GPU "Build the GPU version of the solver" ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU
    ${CMAKE_CURRENT_SOURCE_DIR}/GPU
)

set(SOURCES
    main.cpp
    graph.cpp
    CPU/SA.cpp
)

if(USE_GPU)
    message(STATUS "Budowanie w wersji: GPU + CPU (Driver API)")
    list(APPEND SOURCES
        GPU/CUDA_graph.cu
    )
    
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/kernels.fatbin
        COMMAND ${CMAKE_CUDA_COMPILER} -fatbin -arch=native -std=c++17 -use_fast_math -O3 -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/GPU ${CMAKE_CURRENT_SOURCE_DIR}/GPU/kernels.cu -o ${CMAKE_CURRENT_BINARY_DIR}/kernels.fatbin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/GPU/kernels.cu
    )
    add_custom_target(generate_ptx ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/kernels.fatbin)
else()
    message(STATUS "Budowanie w wersji: TYLKO CPU")
endif()

add_executable(QAP_Solver ${SOURCES})
set_target_properties(QAP_Solver PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

if(USE_GPU)
    target_link_libraries(QAP_Solver PRIVATE curand cuda)
    target_compile_definitions(QAP_Solver PRIVATE USE_GPU)
    target_include_directories(QAP_Solver PRIVATE ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
    
    add_dependencies(QAP_Solver generate_ptx)
endif()