#include <iostream>
#include <array>
#include <numeric>
#include "types.hpp"
#include "graph.hpp"
#include "SA.hpp"
#include "utils.hpp"

#ifdef USE_GPU
    #include "CUDA_graph.cuh"
#endif

void print_matrix(const std::array<float, NN>& graph) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << graph[i * N + j] << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main() {

    std::iota(devices.begin(), devices.end(), 0);

    generate_graph(dc); 
    generate_graph(dd); 

    if constexpr (N <= 10) {
        std::cout << "--- Flow matrix (dc) ---\n";
        print_matrix(dc);
        
        std::cout << "--- Distance matrix (dd) ---\n";
        print_matrix(dd);
    }

    float initial_cost = f(devices, dc, dd);
    std::cout << "Initial cost (before SA): " << initial_cost << "\n";

#ifdef USE_GPU
    std::cout << "[METHOD] Using GPU (CUDA Driver API) method...\n";

    cuInit(0);
    CUdevice cuDevice;
    cuDeviceGet(&cuDevice, 0); 
    
    CUcontext cuContext;
    cuDevicePrimaryCtxRetain(&cuContext, cuDevice); 
    
    cuCtxSetCurrent(cuContext);

    CUmodule module;
    CUresult res = cuModuleLoad(&module, "kernels.fatbin"); 
    if (res != CUDA_SUCCESS) {
        const char* errStr = nullptr;
        cuGetErrorName(res, &errStr); 
        std::cerr << "Can't load kernels.fatbin!\n"; 
        std::cerr << "Error code: " << res;
        if (errStr) std::cerr << " (" << errStr << ")";
        std::cerr << "\n";
        return 1;
    }

    config cfg;
    cfg.N = N;
    cfg.T = 100.0f;
    cfg.T_min = 0.1f;
    cfg.a = 0.99f;

    cuMemAlloc(&cfg.devices, N * sizeof(int));
    cuMemAlloc(&cfg.dc, N * N * sizeof(float));
    cuMemAlloc(&cfg.dd, N * N * sizeof(float));
    cuMemAlloc(&cfg.E, sizeof(float));

    float zero = 0.0f;
    cuMemcpyHtoD(cfg.E, &zero, sizeof(float));
    
    size_t state_size = get_curand_state_size();
    cuMemAlloc(&cfg.state, 128 * state_size); 

    cuMemcpyHtoD(cfg.devices, devices.data(), N * sizeof(int));
    cuMemcpyHtoD(cfg.dc, dc.data(), N * N * sizeof(float));
    cuMemcpyHtoD(cfg.dd, dd.data(), N * N * sizeof(float));

    run_graph_driver_api(cfg, module); 

    cuMemcpyDtoH(devices.data(), cfg.devices, N * sizeof(int));

    float final_cost = f(devices, dc, dd);
    std::cout << "Final cost (after SA): " << final_cost << "\n\n";

    std::cout << "--- Best found assignment ---\n";
    if (N <= 10) {
        for (int i = 0; i < N; ++i) {
            std::cout << "Device " << i << " -> Location " << devices[i] << "\n";
        }
    }

    cuMemFree(cfg.devices);
    cuMemFree(cfg.dc);
    cuMemFree(cfg.dd);
    cuMemFree(cfg.E);
    cuMemFree(cfg.state);
    
    cuModuleUnload(module);
    
    cuDevicePrimaryCtxRelease(cuDevice); 

#else

    float final_cost = f(devices, dc, dd);
    std::cout << "Final cost (after SA): " << final_cost << "\n\n";

    std::cout << "--- Best found assignment ---\n";
    for (int i = 0; i < N; ++i) {
        std::cout << "Device " << i << " -> Location " << devices[i] << "\n";
    }

#endif

    return 0;
}