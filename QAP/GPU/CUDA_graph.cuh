#pragma once
#include <cuda.h>

#include <cstddef> 

size_t get_curand_state_size();
struct config {
    CUdeviceptr devices;
    CUdeviceptr dc;
    CUdeviceptr dd;
    CUdeviceptr E;
    
    float T;
    float T_min;
    CUdeviceptr state; 
    int N;
    float a;
};

void run_graph_driver_api(config& cfg, CUmodule module);