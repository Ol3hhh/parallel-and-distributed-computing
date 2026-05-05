#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include "utils.cuh"

extern "C" {
__global__ void f(const int* __restrict__ devices, const float* __restrict__ dc, const float* __restrict__ dd, float* __restrict__ E, int N);

__global__ void SA(int* __restrict__ devices, const float* __restrict__ dc, const float* __restrict__ dd, float* __restrict__ E, float T, float T_min, curandState* state, int N, float a);
}