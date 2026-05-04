#pragma once

#include <array>
#include "types.hpp"

inline float f(const std::array<int, N>& devices, const std::array<float, NN>& dc, const std::array<float, NN>& dd) {
    float val = 0;
    float distance;
    float cost;
    int device_i;
    int device_j;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            device_i = devices[i];
            device_j = devices[j];
            distance = dd[device_i*N + device_j];
            cost = dc[i*N + j];
            
            val += distance*cost;
        }
    }
    return val;
}