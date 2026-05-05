#include "kernels.cuh"

__global__ void f(const int* __restrict__ devices, const float* __restrict__ dc, const float* __restrict__ dd, float* __restrict__ E, int N) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    float my_sum = 0;

    if (idx < N) {
        int device_i = devices[idx];
        for (int j = 0; j < N; j++) {
            int device_j = devices[j];
            my_sum = __fmaf_rn(dc[idx * N + j], dd[device_i * N + device_j], my_sum);
        }
    }

    unsigned int mask = 0xffffffff;
    #pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        my_sum += __shfl_down_sync(mask, my_sum, offset);
    }

    __shared__ float shared_sums[32]; 
    int lane = threadIdx.x % warpSize;
    int warp_id = threadIdx.x / warpSize;

    if (lane == 0) {
        shared_sums[warp_id] = my_sum;
    }

    __syncthreads();

    if (warp_id == 0) {
        my_sum = (lane < blockDim.x / warpSize) ? shared_sums[lane] : 0;
        #pragma unroll
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
            my_sum += __shfl_down_sync(mask, my_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(E, my_sum);
        }
    }
}

__global__ void SA(int* __restrict__ devices, const float* __restrict__ dc, const float* __restrict__ dd, float* __restrict__ E, float T, float T_min, curandState* state, int N, float a) {    
    __shared__ int s_idx1, s_idx2;
    __shared__ float s_delta;

    while (T > T_min) {
        if (threadIdx.x == 0) {
            cuda::std::pair<int, int> p = pi(state, N);
            s_idx1 = p.first;
            s_idx2 = p.second;
        }

        __syncthreads();

        calc_cost(dc, dd, devices, &s_delta, N, s_idx1, s_idx2);

        __syncthreads();

        if (threadIdx.x == 0) {
            if (P(*E, *E + s_delta, T, state)) {
                *E += s_delta;
                
                int tmp = devices[s_idx1];
                devices[s_idx1] = devices[s_idx2];
                devices[s_idx2] = tmp;
            }
        }


        __syncthreads();
            update_T(T, a);

    }
}