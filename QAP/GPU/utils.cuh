#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda/std/utility>
#include <curand_kernel.h>
#include "types.hpp"

__device__ __forceinline__ bool P(float E, float E_next, float T, curandState* state) {
    if (E_next < E) {
        return true;
    }
    return curand_uniform(state) < __expf(-(E_next - E) / T);
}

__device__ __forceinline__ cuda::std::pair<int, int> pi(curandState* state, int N) {
    int idx1 = curand(state) % N;
    int idx2 = curand(state) % N;

    while (idx1 == idx2) {
        idx2 = curand(state) % N;
    }

    return {idx1, idx2};
}

__device__ __forceinline__ void update_T(float &T, float a) {
    T = T * a;
}

__device__ __forceinline__ void calc_cost(
    const float* __restrict__ dc,
    const float* __restrict__ dd,
    int* __restrict__ devices,
    float* s_delta,
    int N,
    int idx1,
    int idx2
) {
    float delta = 0.0f;

    for (int idx = threadIdx.x; idx < N; idx += blockDim.x) {
        if (idx != idx1 && idx != idx2) {
            float dc_1 = __ldg(&dc[idx1 * N + idx]);
            float dc_2 = __ldg(&dc[idx2 * N + idx]);

            int dev_idx = __ldg(&devices[idx]);
            int dev_1 = __ldg(&devices[idx1]);
            int dev_2 = __ldg(&devices[idx2]);

            float dd_1 = __ldg(&dd[dev_1 * N + dev_idx]);
            float dd_2 = __ldg(&dd[dev_2 * N + dev_idx]);

            float old_rel = __fmaf_rn(dc_1, dd_1, dc_2 * dd_2);
            float new_rel = __fmaf_rn(dc_1, dd_2, dc_2 * dd_1);

            delta += (new_rel - old_rel);
        }
    }

    unsigned int mask = 0xffffffff;
    #pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        delta += __shfl_down_sync(mask, delta, offset);
    }

    __shared__ float shared_deltas[32];

    int lane = threadIdx.x % warpSize;
    int warp_id = threadIdx.x / warpSize;

    if (lane == 0) {
        shared_deltas[warp_id] = delta;
    }

    __syncthreads();

    if (warp_id == 0) {
        int num_warps = (blockDim.x + warpSize - 1) / warpSize;

        delta = (lane < num_warps) ? shared_deltas[lane] : 0.0f;

        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            delta += __shfl_down_sync(mask, delta, offset);
        }

        if (lane == 0) {
            *s_delta = delta * 2.0f;
        }
    }
}
