#include "CUDA_graph.cuh"
#include <iostream>

#include <curand_kernel.h> 
size_t get_curand_state_size() {
    return sizeof(curandState);
}

void run_graph_driver_api(config& cfg, CUmodule module) {
    CUfunction f_func, SA_func;
    cuModuleGetFunction(&f_func, module, "f");
    cuModuleGetFunction(&SA_func, module, "SA");

    CUgraph graph;
    CUgraphNode f_node, SA_node;

    cuGraphCreate(&graph, 0);

    void* f_args[] = { &cfg.devices, &cfg.dc, &cfg.dd, &cfg.E, &cfg.N };
    
    CUDA_KERNEL_NODE_PARAMS f_params = {0};
    f_params.func = f_func;
    f_params.gridDimX = (cfg.N + 127) / 128;
    f_params.gridDimY = 1;
    f_params.gridDimZ = 1;
    f_params.blockDimX = 128;
    f_params.blockDimY = 1;
    f_params.blockDimZ = 1;
    f_params.sharedMemBytes = 0;
    f_params.kernelParams = f_args;
    f_params.extra = NULL;

    cuGraphAddKernelNode(&f_node, graph, NULL, 0, &f_params);

    void* SA_args[] = { &cfg.devices, &cfg.dc, &cfg.dd, &cfg.E, &cfg.T, &cfg.T_min, &cfg.state, &cfg.N, &cfg.a };
    
    CUDA_KERNEL_NODE_PARAMS SA_params = {0};
    SA_params.func = SA_func;
    SA_params.gridDimX = 1;
    SA_params.gridDimY = 1;
    SA_params.gridDimZ = 1;
    SA_params.blockDimX = 128;
    SA_params.blockDimY = 1;
    SA_params.blockDimZ = 1;
    SA_params.sharedMemBytes = 0;
    SA_params.kernelParams = SA_args;
    SA_params.extra = NULL;

    CUgraphNode dependencies[] = { f_node };
    cuGraphAddKernelNode(&SA_node, graph, dependencies, 1, &SA_params);

    CUgraphExec graphExec;
    cuGraphInstantiate(&graphExec, graph, 0);
    CUstream stream;
    cuStreamCreate(&stream, CU_STREAM_NON_BLOCKING);

    cuGraphLaunch(graphExec, stream);

    cuStreamSynchronize(stream);

    cuStreamDestroy(stream);
    cuGraphExecDestroy(graphExec);
    cuGraphDestroy(graph);
}