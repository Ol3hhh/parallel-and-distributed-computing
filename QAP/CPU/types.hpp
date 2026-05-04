#pragma once

#include <random>
#include <array>

inline std::random_device rd;
inline std::mt19937 gen(rd());

constexpr int N = 5;
constexpr int NN = N*N;
inline std::array<float, NN> dc;
inline std::array<float, NN> dd;
inline std::array<int, N> devices;

constexpr float T = 1000;
constexpr float T_min = 0.01;
constexpr float a = 0.85;