#pragma once
#include <cmath>
#include <array>
#include <utility>
#include <random>
#include <tuple>

#include "types.hpp"

bool P(float& E, float& E_next, float& T, std::mt19937& gen);

std::pair<int, int> pi(std::mt19937& gen);

void update_T(float& T);

std::tuple<float, int, int> calc_cost(std::array<int, N>& devices, const std::array<float, NN>& dc, const std::array<float, NN>& dd, float prev_cost, std::mt19937& gen);

void SA(std::array<int, N>& devices, const std::array<float, NN>& dc, const std::array<float, NN>& dd, std::mt19937& gen);
